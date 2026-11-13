/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "umode_elf.h"

static const char *TAG = "umode_elf";

/* The parts of the ELF32 format and the RISC-V psABI that the loader uses */
#define EI_NIDENT          16
#define ELFCLASS32         1
#define ELFDATA2LSB        1
#define EV_CURRENT         1
#define ET_DYN             3
#define EM_RISCV           243
#define EF_RISCV_FLOAT_ABI 0x6u
#define EF_RISCV_RVE       0x8u

#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_TLS     7

#define PF_X 1u
#define PF_W 2u

#define DT_NULL    0
#define DT_RELA    7
#define DT_RELASZ  8
#define DT_RELAENT 9
#define DT_REL     17
#define DT_JMPREL  23

#define R_RISCV_NONE     0
#define R_RISCV_RELATIVE 3

typedef struct {
    uint8_t ident[EI_NIDENT];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} elf_ehdr_t;

typedef struct {
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
} elf_phdr_t;

typedef struct {
    int32_t tag;
    uint32_t val;
} elf_dyn_t;

typedef struct {
    uint32_t offset;
    uint32_t info;
    int32_t addend;
} elf_rela_t;

#define PAGE_SIZE    ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE
#define MAX_SEGMENTS UMODE_ELF_MAX_SEGMENTS

/* A program whose headers were checked. Holds copies of the headers, because the file may not be
 * aligned for them. */
typedef struct {
    const uint8_t *file;
    size_t size;
    elf_ehdr_t ehdr;
    elf_phdr_t load[MAX_SEGMENTS]; /* Loadable segments with a nonzero size, in address order */
    size_t load_count;
    uint32_t end;                  /* End of the last page of the program, before adding the base */
    uint32_t rela_offset;          /* File offset of the relocations */
    uint32_t rela_count;
} program_t;

/* 64-bit, so that a segment that ends near 4 GiB cannot wrap to a small page address */
static uint64_t page_up(uint64_t address)
{
    return (address + PAGE_SIZE - 1) & ~(uint64_t)(PAGE_SIZE - 1);
}

static uint32_t page_down(uint32_t address)
{
    return address & ~(uint32_t)(PAGE_SIZE - 1);
}

static bool in_file(const program_t *p, uint32_t offset, uint32_t length)
{
    return offset <= p->size && length <= p->size - offset;
}

/* Find the file offset of length bytes at a program address, inside the file part of a segment */
static bool vaddr_in_file(const program_t *p, uint32_t vaddr, uint32_t length, uint32_t *offset)
{
    for (size_t i = 0; i < p->load_count; i++) {
        const elf_phdr_t *s = &p->load[i];
        if (vaddr >= s->vaddr && length <= s->filesz && vaddr - s->vaddr <= s->filesz - length) {
            *offset = s->offset + (vaddr - s->vaddr);
            return true;
        }
    }
    return false;
}

/* Find the loadable segment that holds length bytes at a program address. Returns -1 if none does. */
static int segment_of(const program_t *p, uint32_t vaddr, uint32_t length)
{
    for (size_t i = 0; i < p->load_count; i++) {
        const elf_phdr_t *s = &p->load[i];
        if (vaddr >= s->vaddr && length <= s->memsz && vaddr - s->vaddr <= s->memsz - length) {
            return (int)i;
        }
    }
    return -1;
}

/* Check that length bytes at a program address lie inside one loadable segment */
static bool vaddr_in_segment(const program_t *p, uint32_t vaddr, uint32_t length)
{
    return segment_of(p, vaddr, length) >= 0;
}

static esp_err_t check_header(program_t *p)
{
    const elf_ehdr_t *e = &p->ehdr;
    ESP_RETURN_ON_FALSE(p->size >= sizeof(*e), ESP_ERR_INVALID_SIZE, TAG, "file too small for an ELF header");
    memcpy(&p->ehdr, p->file, sizeof(p->ehdr));
    ESP_RETURN_ON_FALSE(memcmp(e->ident, "\x7f" "ELF", 4) == 0, ESP_ERR_NOT_SUPPORTED, TAG, "not an ELF file");
    ESP_RETURN_ON_FALSE(e->ident[4] == ELFCLASS32 && e->ident[5] == ELFDATA2LSB && e->ident[6] == EV_CURRENT,
                        ESP_ERR_NOT_SUPPORTED, TAG, "not a little-endian ELF32 file");
    ESP_RETURN_ON_FALSE(e->machine == EM_RISCV, ESP_ERR_NOT_SUPPORTED, TAG, "not a RISC-V file");
    ESP_RETURN_ON_FALSE(e->type == ET_DYN, ESP_ERR_NOT_SUPPORTED, TAG, "not a position-independent shared object");
    /* U-mode code cannot use the FPU, and RV32E has a different register file */
    ESP_RETURN_ON_FALSE((e->flags & (EF_RISCV_FLOAT_ABI | EF_RISCV_RVE)) == 0, ESP_ERR_NOT_SUPPORTED, TAG,
                        "built for a floating-point ABI or for RV32E");
    ESP_RETURN_ON_FALSE(e->phentsize == sizeof(elf_phdr_t), ESP_ERR_NOT_SUPPORTED, TAG, "unexpected program header size");
    /* phnum is 16-bit, so the product cannot overflow */
    ESP_RETURN_ON_FALSE(in_file(p, e->phoff, (uint32_t)e->phnum * sizeof(elf_phdr_t)), ESP_ERR_INVALID_SIZE, TAG,
                        "program headers outside the file");
    return ESP_OK;
}

static esp_err_t check_load_segment(program_t *p, const elf_phdr_t *s, int index)
{
    ESP_RETURN_ON_FALSE(p->load_count < MAX_SEGMENTS, ESP_ERR_NOT_SUPPORTED, TAG, "more than %d loadable segments", MAX_SEGMENTS);
    ESP_RETURN_ON_FALSE(s->filesz <= s->memsz && in_file(p, s->offset, s->filesz), ESP_ERR_INVALID_SIZE, TAG,
                        "segment %d outside the file", index);
    ESP_RETURN_ON_FALSE(s->memsz <= UINT32_MAX - s->vaddr, ESP_ERR_INVALID_SIZE, TAG, "segment %d wraps around", index);
    ESP_RETURN_ON_FALSE(s->vaddr % PAGE_SIZE == s->offset % PAGE_SIZE, ESP_ERR_NOT_SUPPORTED, TAG,
                        "segment %d is not page-aligned in the file", index);
    ESP_RETURN_ON_FALSE((s->flags & (PF_W | PF_X)) != (PF_W | PF_X), ESP_ERR_NOT_SUPPORTED, TAG,
                        "segment %d is writable and executable", index);
    /* Each page gets the permissions of one segment, so segments must not share a page. The ELF
     * format lists loadable segments in address order. */
    const uint64_t first = page_down(s->vaddr);
    const uint64_t end = page_up((uint64_t)s->vaddr + s->memsz);
    ESP_RETURN_ON_FALSE(first >= p->end, ESP_ERR_NOT_SUPPORTED, TAG,
                        "segment %d shares a page with the segment before it", index);
    ESP_RETURN_ON_FALSE(end <= UINT32_MAX, ESP_ERR_INVALID_SIZE, TAG, "segment %d wraps around", index);
    p->load[p->load_count++] = *s;
    p->end = (uint32_t)end;
    return ESP_OK;
}

static esp_err_t read_relocations(program_t *p, const elf_phdr_t *dynamic)
{
    ESP_RETURN_ON_FALSE(in_file(p, dynamic->offset, dynamic->filesz), ESP_ERR_INVALID_SIZE, TAG,
                        "dynamic section outside the file");
    uint32_t rela = 0, relasz = 0, relaent = sizeof(elf_rela_t);
    for (uint32_t i = 0; i < dynamic->filesz / sizeof(elf_dyn_t); i++) {
        elf_dyn_t d;
        memcpy(&d, p->file + dynamic->offset + i * sizeof(d), sizeof(d));
        if (d.tag == DT_NULL) {
            break;
        }
        switch (d.tag) {
        case DT_RELA:
            rela = d.val;
            break;
        case DT_RELASZ:
            relasz = d.val;
            break;
        case DT_RELAENT:
            relaent = d.val;
            break;
        case DT_REL:
        case DT_JMPREL:
            /* Calls through a PLT would need symbols of the application */
            ESP_LOGE(TAG, "the program has relocations of a kind that is not supported (tag %" PRId32 ")", d.tag);
            return ESP_ERR_NOT_SUPPORTED;
        default:
            break;
        }
    }
    if (relasz == 0) {
        return ESP_OK;
    }
    ESP_RETURN_ON_FALSE(relaent == sizeof(elf_rela_t) && relasz % sizeof(elf_rela_t) == 0, ESP_ERR_NOT_SUPPORTED,
                        TAG, "unexpected relocation entry size");
    ESP_RETURN_ON_FALSE(vaddr_in_file(p, rela, relasz, &p->rela_offset), ESP_ERR_INVALID_SIZE, TAG,
                        "relocations outside the file");
    p->rela_count = relasz / sizeof(elf_rela_t);

    /* Check every relocation now, so that loading fails before anything is allocated */
    for (uint32_t i = 0; i < p->rela_count; i++) {
        elf_rela_t r;
        memcpy(&r, p->file + p->rela_offset + i * sizeof(r), sizeof(r));
        const uint32_t type = r.info & 0xff;
        if (type == R_RISCV_NONE) {
            continue;
        }
        ESP_RETURN_ON_FALSE(type == R_RISCV_RELATIVE, ESP_ERR_NOT_SUPPORTED, TAG,
                            "relocation %" PRIu32 " has type %" PRIu32 ", only R_RISCV_RELATIVE is supported", i, type);
        ESP_RETURN_ON_FALSE(vaddr_in_segment(p, r.offset, sizeof(uint32_t)), ESP_ERR_INVALID_SIZE, TAG,
                            "relocation %" PRIu32 " is outside the program", i);
    }
    return ESP_OK;
}

static esp_err_t parse(const void *file, size_t size, program_t *p)
{
    ESP_RETURN_ON_FALSE(file != NULL, ESP_ERR_INVALID_ARG, TAG, "file is NULL");
    memset(p, 0, sizeof(*p));
    p->file = file;
    p->size = size;
    ESP_RETURN_ON_ERROR(check_header(p), TAG, "invalid header");

    elf_phdr_t dynamic = { 0 };
    for (int i = 0; i < p->ehdr.phnum; i++) {
        elf_phdr_t s;
        memcpy(&s, p->file + p->ehdr.phoff + i * sizeof(s), sizeof(s));
        switch (s.type) {
        case PT_LOAD:
            if (s.memsz != 0) {
                ESP_RETURN_ON_ERROR(check_load_segment(p, &s, i), TAG, "invalid segment");
            }
            break;
        case PT_DYNAMIC:
            ESP_RETURN_ON_FALSE(dynamic.type == 0, ESP_ERR_NOT_SUPPORTED, TAG, "more than one dynamic section");
            dynamic = s;
            break;
        case PT_INTERP:
        case PT_TLS:
            ESP_LOGE(TAG, "segment %d needs an interpreter or thread-local storage", i);
            return ESP_ERR_NOT_SUPPORTED;
        default:
            break;
        }
    }
    ESP_RETURN_ON_FALSE(p->load_count > 0, ESP_ERR_NOT_SUPPORTED, TAG, "no loadable segment");
    if (dynamic.type != 0) {
        ESP_RETURN_ON_ERROR(read_relocations(p, &dynamic), TAG, "invalid relocations");
    }
    return ESP_OK;
}

/* Size of the pages that a segment occupies. parse() checked that it fits in 32 bits. */
static size_t segment_size(const elf_phdr_t *s)
{
    return (size_t)(page_up((uint64_t)s->vaddr + s->memsz) - page_down(s->vaddr));
}

static void free_memory(umode_elf_image_t *image)
{
    for (size_t i = 0; i < image->count; i++) {
        heap_caps_free(image->memory[i]);
    }
    memset(image, 0, sizeof(*image));
}

esp_err_t umode_elf_load(esp_privilege_umode_domain_handle_t domain, const void *file, size_t size, uint32_t base,
                         umode_elf_image_t *image)
{
    ESP_RETURN_ON_FALSE(domain != NULL && image != NULL, ESP_ERR_INVALID_ARG, TAG, "domain or image is NULL");
    memset(image, 0, sizeof(*image));
    ESP_RETURN_ON_FALSE(base % PAGE_SIZE == 0, ESP_ERR_INVALID_ARG, TAG, "base is not page-aligned");
    program_t p;
    ESP_RETURN_ON_ERROR(parse(file, size, &p), TAG, "cannot load the program");
    ESP_RETURN_ON_FALSE((uint64_t)base + p.end <= ESP_PRIVILEGE_UMODE_DOMAIN_ADDRESS_LIMIT, ESP_ERR_INVALID_SIZE, TAG,
                        "program does not fit below the address limit at base 0x%08" PRIx32, base);

    /* Copy each segment into zeroed pages of its own, which also covers .bss */
    for (size_t i = 0; i < p.load_count; i++) {
        const elf_phdr_t *s = &p.load[i];
        const uint32_t caps = (s->flags & PF_X) ? MALLOC_CAP_INTERNAL | MALLOC_CAP_EXEC :
                              MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT;
        uint8_t *memory = heap_caps_aligned_calloc(PAGE_SIZE, 1, segment_size(s), caps);
        if (memory == NULL) {
            ESP_LOGE(TAG, "no memory for segment %u (%u bytes)", (unsigned)i, (unsigned)segment_size(s));
            free_memory(image);
            return ESP_ERR_NO_MEM;
        }
        image->memory[image->count++] = memory;
        memcpy(memory + (s->vaddr - page_down(s->vaddr)), p.file + s->offset, s->filesz);
    }

    /* The value of a relative relocation is just a number for the U-mode code, which could compute
     * any address itself, so only its target is checked, which parse() did. */
    for (uint32_t i = 0; i < p.rela_count; i++) {
        elf_rela_t r;
        memcpy(&r, p.file + p.rela_offset + i * sizeof(r), sizeof(r));
        if ((r.info & 0xff) != R_RISCV_RELATIVE) {
            continue;
        }
        const int segment = segment_of(&p, r.offset, sizeof(uint32_t));
        assert(segment >= 0);
        const uint32_t value = base + (uint32_t)r.addend;
        uint8_t *memory = image->memory[segment];
        memcpy(memory + (r.offset - page_down(p.load[segment].vaddr)), &value, sizeof(value));
    }

    /* Map the segments only now, so that the domain never sees memory that is still being written */
    for (size_t i = 0; i < p.load_count; i++) {
        const elf_phdr_t *s = &p.load[i];
        /* parse() checked that every segment ends below p.end, and the check above that base + p.end fits */
        const uint32_t first = base + page_down(s->vaddr);
        uint32_t permissions = ESP_PRIVILEGE_UMODE_DOMAIN_READ;
        permissions |= (s->flags & PF_W) ? ESP_PRIVILEGE_UMODE_DOMAIN_WRITE : 0;
        permissions |= (s->flags & PF_X) ? ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE : 0;
        ESP_LOGD(TAG, "segment 0x%08" PRIx32 "..0x%08" PRIx32 " r%c%c", first, first + (uint32_t)segment_size(s),
                 (permissions & ESP_PRIVILEGE_UMODE_DOMAIN_WRITE) ? 'w' : '-',
                 (permissions & ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE) ? 'x' : '-');
        esp_err_t err = esp_privilege_umode_domain_map(domain, first, image->memory[i], segment_size(s), permissions);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "cannot map the segment at 0x%08" PRIx32 ": %s", first, esp_err_to_name(err));
            /* The domain may still use the segments mapped so far, so they stay in image until the caller
             * destroys the domain. A failed map leaves the domain unchanged. */
            for (size_t j = i; j < image->count; j++) {
                heap_caps_free(image->memory[j]);
                image->memory[j] = NULL;
            }
            image->count = i;
            return err;
        }
    }

    image->entry = base + p.ehdr.entry;
    image->end = base + p.end;
    return ESP_OK;
}

void umode_elf_free(umode_elf_image_t *image)
{
    if (image != NULL) {
        free_memory(image);
    }
}
