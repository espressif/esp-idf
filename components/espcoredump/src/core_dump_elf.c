/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_attr.h"
#include "esp_partition.h"
#include "esp_flash_encrypt.h"
#include "sdkconfig.h"
#include "core_dump_checksum.h"
#include "esp_core_dump_port.h"
#include "esp_core_dump_port_impl.h"
#include "esp_core_dump_common.h"
#include "hal/efuse_hal.h"
#include "esp_task_wdt.h"

#ifdef CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
#include <sys/param.h>      // for the MIN macro
#include "esp_app_desc.h"
#endif

#define ELF_CLASS ELFCLASS32

#include "elf.h"                    // for ELF file types

#define ELF_SEG_HEADERS_COUNT(_self_) ((_self_)->segs_count)

#define ELF_HLEN 52
#define ELF_CORE_SEC_TYPE 1
#define ELF_PR_STATUS_SEG_NUM 0
#define ELF_ESP_CORE_DUMP_INFO_TYPE 8266
#define ELF_ESP_CORE_DUMP_EXTRA_INFO_TYPE 677
#define ELF_ESP_CORE_DUMP_PANIC_DETAILS_TYPE 679
#define ELF_NOTE_NAME_MAX_SIZE 32
#define ELF_APP_SHA256_SIZE 66

#define ELF_ESP_CORE_DUMP_PANIC_DETAILS_NOTE_NAME "ESP_PANIC_DETAILS"

#define ELF_CHECK_ERR(a, ret_val, str, ...) \
    if (!(a)) { \
        ESP_COREDUMP_LOGE("%s(%u): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }

typedef enum {
    ELF_STAGE_CALC_SPACE = 0,
    ELF_STAGE_PLACE_HEADERS = 1,
    ELF_STAGE_PLACE_DATA = 2
} core_dump_elf_stages_t;

typedef enum _elf_err_t {
    ELF_PROC_ERR_SKIP_HEADER = 0,
    ELF_PROC_ERR_STACK_CORRUPTED = -1,
    ELF_PROC_ERR_WRITE_FAIL = -2,
    ELF_PROC_ERR_OTHER = -3
} core_dump_elf_proc_err_t;

typedef struct _core_dump_task_info_t {
    elf_phdr* phdr;
    void* frame;
    core_dump_task_header_t* task_hdr;
    uint32_t task_id;
    size_t tcb_sz;
    int* size_ptr;
} core_dump_task_data_t;

typedef struct {
    uint32_t version; // coredump version
    uint8_t app_elf_sha256[ELF_APP_SHA256_SIZE]; // sha256 of elf file
} core_dump_elf_version_info_t;

const static char TAG[] __attribute__((unused)) = "esp_core_dump_elf";

// Main ELF handle type
typedef struct _core_dump_elf_t {
    core_dump_elf_version_info_t    elf_version_info;
    uint16_t                        elf_stage;
    uint32_t                        elf_next_data_offset;
    uint16_t                        segs_count;
    core_dump_write_data_t          write_data;
    uint32_t                        note_data_size; /* can be used where static storage needed */
#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM
    /* To avoid checksum failure, coredump stack region will be excluded while storing the sections. */
    uint32_t                        coredump_stack_start;
    uint32_t                        coredump_stack_size;
#endif
} core_dump_elf_t;

typedef struct {
    core_dump_elf_t *self;
    uint32_t total_size;
    bool size_only;
} core_dump_elf_opaque_t;

// Represents lightweight implementation to save core dump data into ELF formatted binary

#if CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF

#ifdef ALIGN_UP
#undef ALIGN_UP
#endif
#define ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))

esp_err_t esp_core_dump_store(void) __attribute__((alias("esp_core_dump_write_elf")));

// Builds elf header and check all data offsets
static int elf_write_file_header(core_dump_elf_t *self, uint32_t seg_count)
{
    elfhdr elf_hdr; // declare as static to save stack space

    if (self->elf_stage == ELF_STAGE_PLACE_HEADERS) {
        ESP_COREDUMP_LOG_PROCESS("Segment count %u", seg_count);
        memset(&elf_hdr, 0, sizeof(elfhdr));
        elf_hdr.e_ident[0] = ELFMAG0;
        elf_hdr.e_ident[1] = ELFMAG1;
        elf_hdr.e_ident[2] = ELFMAG2;
        elf_hdr.e_ident[3] = ELFMAG3;
        elf_hdr.e_ident[4] = ELFCLASS32;
        elf_hdr.e_ident[5] = ELFDATA2LSB;
        elf_hdr.e_ident[6] = EV_CURRENT;
        elf_hdr.e_ident[7] = ELFOSABI_NONE;
        elf_hdr.e_ident[8] = 0;
        elf_hdr.e_type = ET_CORE;
        elf_hdr.e_machine = esp_core_dump_get_arch_id();
        elf_hdr.e_flags = 0;
        elf_hdr.e_version = EV_CURRENT;
        elf_hdr.e_entry = 0;
        _Static_assert(sizeof(elfhdr) == ELF_HLEN, "Invalid ELF header struct length!");
        elf_hdr.e_phoff = sizeof(elfhdr);          // program header table's file offset in bytes.
        elf_hdr.e_phentsize = sizeof(elf_phdr);    // size in bytes of one entry in the file program header table
        elf_hdr.e_phnum = seg_count;                 // number of program segments
        elf_hdr.e_shoff = 0;                       // section header table's file offset in bytes.
        elf_hdr.e_ehsize = sizeof(elfhdr);         // elf header size
        elf_hdr.e_shentsize = sizeof(elf_shdr);    // section header's size in bytes.
        elf_hdr.e_shnum = 0;                       // initial section counter is 0
        elf_hdr.e_shstrndx = SHN_UNDEF;            // do not use string table
        // write built elf header into elf image
        esp_err_t err = esp_core_dump_write_data(&self->write_data, &elf_hdr, sizeof(elf_hdr));
        ELF_CHECK_ERR((err == ESP_OK), ELF_PROC_ERR_WRITE_FAIL,
                      "Write ELF header failure (%d)", err);
        ESP_COREDUMP_LOG_PROCESS("Add file header %u bytes", sizeof(elf_hdr));
    }

    return self->elf_stage == ELF_STAGE_PLACE_DATA ? 0 : sizeof(elf_hdr);
}

static int elf_write_segment_header(core_dump_elf_t *self, elf_phdr* phdr)
{
    ELF_CHECK_ERR(phdr, ELF_PROC_ERR_SKIP_HEADER,
                  "Header is skipped, stage=(%d).", self->elf_stage);

    phdr->p_offset = self->elf_next_data_offset;
    // set segment data information and write it into image
    esp_err_t err = esp_core_dump_write_data(&self->write_data, phdr, sizeof(elf_phdr));
    ELF_CHECK_ERR((err == ESP_OK), ELF_PROC_ERR_WRITE_FAIL,
                  "Write ELF segment header failure (%d)", err);
    ESP_COREDUMP_LOG_PROCESS("Add segment header %u bytes: type %d, sz %u, off = 0x%x",
                             sizeof(elf_phdr), phdr->p_type, phdr->p_filesz, phdr->p_offset);

    return sizeof(elf_phdr);
}

static int elf_add_segment(core_dump_elf_t *self,
                           uint32_t type, uint32_t vaddr,
                           void* data, uint32_t data_sz)
{
    esp_err_t err = ESP_FAIL;
    elf_phdr seg_hdr = { 0 };
    int data_len = ALIGN_UP(data_sz, 4);

    ELF_CHECK_ERR((data != NULL), ELF_PROC_ERR_OTHER,
                  "Invalid data for segment.");

    if (self->elf_stage == ELF_STAGE_CALC_SPACE) {
        self->segs_count++;
        return data_len + sizeof(elf_phdr);
    }
    if (self->elf_stage == ELF_STAGE_PLACE_HEADERS) {
        seg_hdr.p_type = type;
        seg_hdr.p_vaddr = vaddr;
        seg_hdr.p_paddr = vaddr;
        seg_hdr.p_filesz = data_len;
        seg_hdr.p_memsz = data_len;
        seg_hdr.p_flags = (PF_R | PF_W);
        int ret = elf_write_segment_header(self, &seg_hdr);
        ELF_CHECK_ERR((ret > 0), ret,
                      "Write ELF segment data failure (%d)", ret);
        self->elf_next_data_offset += data_len;
        return ret;
    }
    ESP_COREDUMP_LOG_PROCESS("Add segment size=%u, start_off=0x%x",
                             (uint32_t)data_len, self->elf_next_data_offset);
    // write segment data only when write function is set and phdr = NULL
    // write data into segment
    err = esp_core_dump_write_data(&self->write_data, data, (uint32_t)data_len);
    ELF_CHECK_ERR((err == ESP_OK), ELF_PROC_ERR_WRITE_FAIL,
                  "Write ELF segment data failure (%d)", err);
    self->elf_next_data_offset += data_len;
    return data_len;
}

static int elf_write_note_header(core_dump_elf_t *self,
                                 const char* name, uint32_t name_len, uint32_t data_sz, uint32_t type)
{
    // temporary aligned buffer for note name
    static char name_buffer[ELF_NOTE_NAME_MAX_SIZE] = { 0 };
    elf_note note_hdr = { 0 };

    memcpy((void*)name_buffer, (void*)name, name_len);
    note_hdr.n_namesz = name_len;
    note_hdr.n_descsz = data_sz;
    note_hdr.n_type = type;
    // write note header
    esp_err_t err = esp_core_dump_write_data(&self->write_data, &note_hdr, sizeof(note_hdr));
    ELF_CHECK_ERR((err == ESP_OK), ELF_PROC_ERR_WRITE_FAIL,
                  "Write ELF note header failure (%d)", err);
    // write note name
    err = esp_core_dump_write_data(&self->write_data, name_buffer, name_len);
    ELF_CHECK_ERR((err == ESP_OK), ELF_PROC_ERR_WRITE_FAIL,
                  "Write ELF note name failure (%d)", err);

    return err;
}

static int elf_write_note(core_dump_elf_t *self,
                          const char* name,
                          uint32_t type,
                          void* data,
                          uint32_t data_sz)
{
    esp_err_t err = ESP_FAIL;
    uint32_t name_len = ALIGN_UP(strlen(name) + 1, 4); // get name length including terminator
    uint32_t data_len = ALIGN_UP(data_sz, 4);

    ELF_CHECK_ERR((name_len <= ELF_NOTE_NAME_MAX_SIZE), 0,
                  "Segment note name is too long %d.", name_len);

    uint32_t note_size = ALIGN_UP(name_len + data_len + sizeof(elf_note), 4);

    // write segment data during second pass
    if (self->elf_stage == ELF_STAGE_PLACE_DATA) {
        ELF_CHECK_ERR(data, ELF_PROC_ERR_OTHER, "Invalid data pointer %x.", (uint32_t)data);
        err = elf_write_note_header(self, name, name_len, data_sz, type);
        if (err != ESP_OK) {
            return err;
        }

        // note data must be aligned in memory. we write aligned byte structures and panic details in strings,
        // which might not be aligned by default. Therefore, we need to verify alignment and add padding if necessary.
        err = esp_core_dump_write_data(&self->write_data, data, data_sz);
        if (err == ESP_OK) {
            int pad_size = data_len - data_sz;
            if (pad_size != 0) {
                uint8_t pad_bytes[3] = {0};
                ESP_COREDUMP_LOG_PROCESS("Core dump note data needs %d bytes padding", pad_size);
                err = esp_core_dump_write_data(&self->write_data, pad_bytes, pad_size);
            }
        }

        ELF_CHECK_ERR((err == ESP_OK), ELF_PROC_ERR_WRITE_FAIL,
                      "Write ELF note data failure (%d)", err);
        ESP_COREDUMP_LOG_PROCESS("Add note size=%d, start_off=0x%x",
                                 note_size, self->elf_next_data_offset);
    }
    return note_size; // return actual note size
}

static int elf_add_note(core_dump_elf_t *self,
                        const char* name,
                        uint32_t type,
                        void* data,
                        uint32_t data_sz)
{
    ELF_CHECK_ERR((data != NULL), ELF_PROC_ERR_OTHER,
                  "Invalid data pointer for segment");

    int note_size = elf_write_note(self, name, type, data, data_sz);
    ELF_CHECK_ERR((note_size > 0), note_size,
                  "Write ELF note data failure, returned (%d)", note_size);
    return note_size; // return actual note segment size
}

// Append note with registers dump to segment note
static int elf_add_regs(core_dump_elf_t *self, core_dump_task_header_t *task)
{
    void *reg_dump;

    uint32_t len = esp_core_dump_get_task_regs_dump(task, &reg_dump);
    if (len == 0) {
        ESP_COREDUMP_LOGE("Zero size register dump for task 0x%x!", task->tcb_addr);
        return ELF_PROC_ERR_OTHER;
    }

    // append note data with dump to existing note
    return elf_add_note(self,
                        "CORE",                // note name
                        ELF_CORE_SEC_TYPE,     // note type for reg dump
                        reg_dump,      // register dump with pr_status
                        len);
}

static int elf_add_stack(core_dump_elf_t *self, core_dump_task_header_t *task)
{
    uint32_t stack_vaddr, stack_len = 0, stack_paddr = 0;

    ELF_CHECK_ERR((task), ELF_PROC_ERR_OTHER, "Invalid task pointer.");

    stack_len = esp_core_dump_get_stack(task, &stack_vaddr, &stack_paddr);
    ESP_COREDUMP_LOG_PROCESS("Add stack for task 0x%x: addr 0x%x, sz %u",
                             task->tcb_addr, stack_vaddr, stack_len);
    int ret = elf_add_segment(self, PT_LOAD,
                              (uint32_t)stack_vaddr,
                              (void*)stack_paddr,
                              (uint32_t) stack_len);
    return ret;
}

static int elf_add_tcb(core_dump_elf_t *self, core_dump_task_header_t *task)
{
    ELF_CHECK_ERR((task), ELF_PROC_ERR_OTHER, "Invalid task pointer.");
    // add task tcb data into program segment of ELF
    ESP_COREDUMP_LOG_PROCESS("Add TCB for task 0x%x: addr 0x%x, sz %u",
                             task->tcb_addr, task->tcb_addr,
                             esp_core_dump_get_tcb_len());
    int ret = elf_add_segment(self, PT_LOAD,
                              (uint32_t)task->tcb_addr,
                              task->tcb_addr,
                              esp_core_dump_get_tcb_len());
    return ret;
}

static int elf_process_task_tcb(core_dump_elf_t *self, core_dump_task_header_t *task)
{
    int ret = ELF_PROC_ERR_OTHER;

    ELF_CHECK_ERR((task), ELF_PROC_ERR_OTHER, "Invalid input data.");

    // save tcb of the task as is and apply segment size
    ret = elf_add_tcb(self, task);
    if (ret <= 0) {
        ESP_COREDUMP_LOGE("Task (TCB:%x) processing failure = %d",
                          task->tcb_addr,
                          ret);
    }
    return ret;
}

static int elf_process_task_stack(core_dump_elf_t *self, core_dump_task_header_t *task)
{
    int ret = ELF_PROC_ERR_OTHER;

    ELF_CHECK_ERR((task), ELF_PROC_ERR_OTHER, "Invalid input data.");

    ret = elf_add_stack(self, task);
    if (ret <= 0) {
        ESP_COREDUMP_LOGE("Task (TCB:%x), (Stack:%x), stack processing failure = %d.",
                          task->tcb_addr,
                          task->stack_start,
                          ret);
    }
    return ret;
}

static int elf_process_note_segment(core_dump_elf_t *self, int notes_size)
{
    int ret;
    elf_phdr seg_hdr = { 0 };

    if (self->elf_stage == ELF_STAGE_PLACE_HEADERS) {
        // segment header for PR_STATUS notes
        seg_hdr.p_type = PT_NOTE;
        seg_hdr.p_vaddr = 0;
        seg_hdr.p_paddr = 0;
        seg_hdr.p_filesz = notes_size;
        seg_hdr.p_memsz = notes_size;
        seg_hdr.p_flags = (PF_R | PF_W);
        ret = elf_write_segment_header(self, &seg_hdr);
        ELF_CHECK_ERR((ret > 0), ret, "NOTE segment header write failure, returned (%d).", ret);
        self->elf_next_data_offset += notes_size;
        return sizeof(seg_hdr);
    } else if (self->elf_stage == ELF_STAGE_CALC_SPACE) {
        self->segs_count++;
        notes_size += sizeof(seg_hdr);
    } else {
        // in "Place Data" phase segment body is been already filled by other functions
        ESP_COREDUMP_LOG_PROCESS("Add NOTE segment, size=%d, start_off=0x%x",
                                 notes_size, self->elf_next_data_offset);
        self->elf_next_data_offset += notes_size;
    }
    return (int)notes_size;
}

static int elf_process_tasks_regs(core_dump_elf_t *self)
{
    core_dump_task_header_t task_hdr = { 0 };
    TaskIterator_t task_iter;
    void *task = NULL;
    int len = 0;
    int ret = 0;

    esp_core_dump_reset_tasks_snapshots_iter();
    task = esp_core_dump_get_current_task_handle();
    if (esp_core_dump_get_task_snapshot(task, &task_hdr, NULL)) {
        // place current task dump first
        ret = elf_add_regs(self,  &task_hdr);
        if (self->elf_stage == ELF_STAGE_PLACE_HEADERS) {
            // when writing segments headers this function writes nothing
            ELF_CHECK_ERR((ret >= 0), ret, "Task %x, PR_STATUS write failed, return (%d).", task, ret);
        } else {
            ELF_CHECK_ERR((ret > 0), ret, "Task %x, PR_STATUS write failed, return (%d).", task, ret);
        }
        len += ret;
    }
    // processes PR_STATUS and register dump for each task
    // each call to the processing function appends PR_STATUS note into note segment
    // and writes data or updates the segment note header accordingly (if phdr is set)
    esp_core_dump_task_iterator_init(&task_iter);
    while (esp_core_dump_task_iterator_next(&task_iter) != -1) {
        task = task_iter.pxTaskHandle;
        if (!task || task == esp_core_dump_get_current_task_handle()) { // skip current task (already processed)
            continue;
        }
        if (esp_core_dump_get_task_snapshot(task, &task_hdr, NULL)) {
            ret = elf_add_regs(self,  &task_hdr);
            if (self->elf_stage == ELF_STAGE_PLACE_HEADERS) {
                // when writing segments headers this function writes nothing
                ELF_CHECK_ERR((ret >= 0), ret, "Task %x, PR_STATUS write failed, return (%d).", task, ret);
            } else {
                ELF_CHECK_ERR((ret > 0), ret, "Task %x, PR_STATUS write failed, return (%d).", task, ret);
            }
            len += ret;
        }
    }
    ret = elf_process_note_segment(self, len); // tasks regs note
    ELF_CHECK_ERR((ret > 0), ret,
                  "PR_STATUS note segment processing failure, returned(%d).", ret);
    return ret;
}

static int elf_save_task(core_dump_elf_t *self, core_dump_task_header_t *task)
{
    int elf_len = 0;

    int ret = elf_process_task_tcb(self, task);
    ELF_CHECK_ERR((ret > 0), ret,
                  "Task %x, TCB write failed, return (%d).", task->tcb_addr, ret);
    elf_len += ret;
    ret = elf_process_task_stack(self, task);
    ELF_CHECK_ERR((ret != ELF_PROC_ERR_WRITE_FAIL), ELF_PROC_ERR_WRITE_FAIL,
                  "Task %x, stack write failed, return (%d).", task->tcb_addr, ret);
    elf_len += ret;
    return elf_len;
}

static int elf_write_tasks_data(core_dump_elf_t *self)
{
    int elf_len = 0;
    core_dump_task_header_t task_hdr = { 0 };
    core_dump_mem_seg_header_t interrupted_stack = { 0 };
    TaskIterator_t task_iter;
    int ret = ELF_PROC_ERR_OTHER;
    uint16_t tasks_num = 0;
    uint16_t bad_tasks_num = 0;

    ESP_COREDUMP_LOG_PROCESS("================ Processing task registers ================");
    ret = elf_process_tasks_regs(self);
    ELF_CHECK_ERR((ret > 0), ret, "Tasks regs addition failed, return (%d).", ret);
    elf_len += ret;

    ESP_COREDUMP_LOG_PROCESS("================   Processing task data   ================");
    // processes all task's stack data and writes segment data into partition
    // if flash configuration is set
    esp_core_dump_reset_tasks_snapshots_iter();
    esp_core_dump_task_iterator_init(&task_iter);
    while (esp_core_dump_task_iterator_next(&task_iter) != -1) {
        tasks_num++;
        if (!esp_core_dump_get_task_snapshot(task_iter.pxTaskHandle, &task_hdr, &interrupted_stack)) {
            bad_tasks_num++;
            continue;
        }

#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM
        /* Only crashed task data will be saved here. The other task's data will be automatically saved within the sections */
        if (esp_core_dump_get_current_task_handle() != task_iter.pxTaskHandle)
#endif
        {
            ret = elf_save_task(self, &task_hdr);
            ELF_CHECK_ERR((ret > 0), ret,
                          "Task %x, TCB write failed, return (%d).", task_iter.pxTaskHandle, ret);
            elf_len += ret;
        }
        if (interrupted_stack.size > 0) {
            ESP_COREDUMP_LOG_PROCESS("Add interrupted task stack %lu bytes @ %x",
                                     interrupted_stack.size, interrupted_stack.start);
            ret = elf_add_segment(self, PT_LOAD,
                                  (uint32_t)interrupted_stack.start,
                                  (void*)interrupted_stack.start,
                                  (uint32_t)interrupted_stack.size);
            ELF_CHECK_ERR((ret > 0), ret, "Interrupted task stack write failed, return (%d).", ret);
            elf_len += ret;
        }
    }
    ESP_COREDUMP_LOG_PROCESS("Found %d bad task out of %d", bad_tasks_num, tasks_num);
    return elf_len;
}

#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM

/* Coredump stack will also be used by the checksum functions while saving sections.
 * There is a potential for inconsistency when writing coredump stack to the flash and calculating checksum simultaneously.
 * This is because, coredump stack will be modified during the process, leading to incorrect checksum calculations.
 * To mitigate this issue, it's important to ensure that the coredump stack excluded from checksum calculation by
 * filter out from the written regions.
 * Typically, the coredump stack can be located in two different sections.
 * 1. In the bss section;
 *    1.a if `CONFIG_ESP_COREDUMP_STACK_SIZE` set to a nonzero value
 *    1.b if the crashed task is created with a static task buffer using the xTaskCreateStatic() api
 * 2. In the heap section, if custom stack is not defined and the crashed task buffer is allocated in the heap
 * with the xTaskCreate() api
 *
 * esp_core_dump_store_section() will check if the coredump stack is located inside the section.
 * If it is, this part will be skipped.
 * |+++++++++| xxxxxxxxxxxxxx |++++++++|
 * |+++++++++| coredump stack |++++++++|
*/
static int esp_core_dump_store_section(core_dump_elf_t *self, uint32_t start, uint32_t data_len)
{
    uint32_t end = start + data_len;
    int total_sz = 0;
    int ret;

    if (self->coredump_stack_start > start && self->coredump_stack_start < end) {
        /* write until the coredump stack. */
        data_len = self->coredump_stack_start - start;
        ret = elf_add_segment(self, PT_LOAD,
                              start,
                              (void*)start,
                              data_len);

        if (ret <= 0) {
            return ret;
        }
        total_sz += ret;

        /* Skip coredump stack and set offset for the rest of the section */
        start = self->coredump_stack_start + self->coredump_stack_size;
        data_len = end - start;
    }

    if (data_len > 0) {
        ret = elf_add_segment(self, PT_LOAD,
                              (uint32_t)start,
                              (void*)start,
                              (uint32_t)data_len);
        if (ret <= 0) {
            return ret;
        }
        total_sz += ret;
    }

    return total_sz;
}

typedef struct {
    core_dump_elf_t *self;
    int *total_sz;
    int ret;
} heap_block_data_t;

bool esp_core_dump_write_heap_blocks(walker_heap_into_t heap_info, walker_block_info_t block_info, void* user_data)
{
    heap_block_data_t *param = user_data;
    int *total_sz = param->total_sz;
    core_dump_elf_t *self = param->self;
    int *ret = &param->ret;

    if (*ret <= 0) {
        /* There was a flash write failure at the previous write attempt */
        return false;
    }

    if ((intptr_t)heap_info.end - (intptr_t)block_info.ptr < block_info.size) {
        ESP_COREDUMP_LOGE("Block corruption detected in the heap (%p-%p)", heap_info.start, heap_info.end);
        ESP_COREDUMP_LOGE("Corrupted block addr:%p size:%x)", block_info.ptr, block_info.size);
        /* Heap walker will skip the next block in the same heap region and it will continue from the next heap region's block. */
        return false;
    }

    if (block_info.used && block_info.size > 0) {
        ESP_COREDUMP_LOG_PROCESS("heap block @%p sz:(%x)", (void *)block_info.ptr, block_info.size);

        if (!esp_core_dump_mem_seg_is_sane((uint32_t)block_info.ptr, block_info.size)) {
            return false;
        }

        if (self->coredump_stack_start == (uint32_t)block_info.ptr) {
            /* skip writing coredump stack block */
            return true;
        }

        *ret = elf_add_segment(self, PT_LOAD,
                               (uint32_t)block_info.ptr,
                               (void*)block_info.ptr,
                               block_info.size);
        if (*ret <= 0) {
            return false;
        }
        *total_sz += *ret;
    }

    return true;
}

#else

static int esp_core_dump_store_section(core_dump_elf_t *self, uint32_t start, uint32_t data_len)
{
    return elf_add_segment(self, PT_LOAD,
                           start,
                           (void*)start,
                           data_len);
}

#endif

static int elf_write_core_dump_user_data(core_dump_elf_t *self)
{
    int total_sz = 0;
    uint32_t start = 0;

    for (coredump_region_t i = COREDUMP_MEMORY_START; i < COREDUMP_MEMORY_MAX; i++) {
        int data_len = esp_core_dump_get_user_ram_info(i, &start);

        ELF_CHECK_ERR((data_len >= 0), ELF_PROC_ERR_OTHER, "invalid memory region");

        if (data_len > 0) {
            int ret = esp_core_dump_store_section(self, start, data_len);
            ELF_CHECK_ERR((ret > 0), ret, "memory region write failed. Returned (%d).", ret);
            total_sz += ret;
        }
    }

#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM
    heap_block_data_t user_data = {.self = self, .total_sz = &total_sz, .ret = 1};
    heap_caps_walk(MALLOC_CAP_8BIT, esp_core_dump_write_heap_blocks, &user_data);
    ELF_CHECK_ERR((user_data.ret > 0), user_data.ret, "Heap memory write failed. Returned (%d).", user_data.ret);
#endif

    return total_sz;
}

#if CONFIG_ESP_TASK_WDT_EN
static void elf_write_core_dump_note_cb(void *opaque, const char *data)
{
    core_dump_elf_opaque_t *param = opaque;
    core_dump_elf_t *self = param->self;
    const size_t data_len = strlen(data);

    ESP_COREDUMP_LOG_PROCESS("Core dump note cb data_len:(%d)", data_len);

    param->total_size += data_len;

    if (!param->size_only) {
        esp_err_t err = esp_core_dump_write_data(&self->write_data, (void *)data, data_len);
        if (err != ESP_OK) {
            param->total_size = 0;
        }
    }
}

static int elf_add_wdt_panic_details(core_dump_elf_t *self)
{
    uint32_t name_len = ALIGN_UP(sizeof(ELF_ESP_CORE_DUMP_PANIC_DETAILS_NOTE_NAME), 4);
    core_dump_elf_opaque_t param = {
        .self = self,
        .total_size = 0,
        .size_only = false,
    };

    if (self->elf_stage == ELF_STAGE_CALC_SPACE) {
        param.size_only = true;
        esp_task_wdt_print_triggered_tasks(elf_write_core_dump_note_cb, &param, NULL);
        ELF_CHECK_ERR((param.total_size > 0), ELF_PROC_ERR_OTHER, "wdt panic message len is zero!");
        self->note_data_size = param.total_size;
    } else if (self->elf_stage == ELF_STAGE_PLACE_DATA) {
        esp_err_t err = elf_write_note_header(self,
                                              ELF_ESP_CORE_DUMP_PANIC_DETAILS_NOTE_NAME,
                                              name_len,
                                              self->note_data_size,
                                              ELF_ESP_CORE_DUMP_PANIC_DETAILS_TYPE);
        if (err != ESP_OK) {
            return err;
        }

        esp_task_wdt_print_triggered_tasks(elf_write_core_dump_note_cb, &param, NULL);
        ELF_CHECK_ERR((param.total_size > 0), ELF_PROC_ERR_WRITE_FAIL, "Write ELF note data failure (%d)", err);
        const uint32_t mod = self->note_data_size & 3;
        if (mod != 0) {
            uint8_t pad_bytes[3] = {0};
            uint32_t pad_size = 4 - mod;
            ESP_COREDUMP_LOG_PROCESS("Core dump note needs %d bytes padding", pad_size);
            err = esp_core_dump_write_data(&self->write_data, pad_bytes, pad_size);
            ELF_CHECK_ERR((err == ESP_OK), ELF_PROC_ERR_WRITE_FAIL, "Write ELF note padding failure (%d)", err);
        }
    }

    return ALIGN_UP(name_len + ALIGN_UP(self->note_data_size, 4) + sizeof(elf_note), 4);
}
#endif //CONFIG_ESP_TASK_WDT_EN

static int elf_write_core_dump_info(core_dump_elf_t *self)
{
    void *extra_info = NULL;

    ESP_COREDUMP_LOG_PROCESS("================ Processing coredump info ================");
    // This dump function can be called when the cache is disable which requires putting
    // esp_app_get_elf_sha256() into IRAM. But we want to reduce IRAM usage,
    // so we use a function that returns an already formatted string.
    size_t data_len = strlcpy((char*)self->elf_version_info.app_elf_sha256, esp_app_get_elf_sha256_str(),
                              sizeof(self->elf_version_info.app_elf_sha256));
    ESP_COREDUMP_LOG_PROCESS("Application SHA256='%s', length=%d.",
                             self->elf_version_info.app_elf_sha256, data_len);
    self->elf_version_info.version = esp_core_dump_elf_version();
    int ret = elf_add_note(self,
                           "ESP_CORE_DUMP_INFO",
                           ELF_ESP_CORE_DUMP_INFO_TYPE,
                           &self->elf_version_info,
                           sizeof(self->elf_version_info));
    ELF_CHECK_ERR((ret > 0), ret, "Version info note write failed. Returned (%d).", ret);
    data_len = ret;

    uint32_t extra_info_len = esp_core_dump_get_extra_info(&extra_info);
    if (extra_info_len == 0) {
        ESP_COREDUMP_LOGE("Zero size extra info!");
        return ELF_PROC_ERR_OTHER;
    }

    ret = elf_add_note(self,
                       "ESP_EXTRA_INFO",
                       ELF_ESP_CORE_DUMP_EXTRA_INFO_TYPE,
                       extra_info,
                       extra_info_len);
    ELF_CHECK_ERR((ret > 0), ret, "Extra info note write failed. Returned (%d).", ret);
    data_len += ret;

#if CONFIG_ESP_TASK_WDT_EN
    extern bool g_twdt_isr;
    if (g_twdt_isr) {
        ret = elf_add_wdt_panic_details(self);
        if (ret <= 0) {
            return ret;
        }
        data_len += ret;
    }
#endif

    if (g_panic_abort_details && strlen(g_panic_abort_details) > 0) {
        ret = elf_add_note(self,
                           ELF_ESP_CORE_DUMP_PANIC_DETAILS_NOTE_NAME,
                           ELF_ESP_CORE_DUMP_PANIC_DETAILS_TYPE,
                           g_panic_abort_details,
                           strlen(g_panic_abort_details));
        ELF_CHECK_ERR((ret > 0), ret, "Panic details note write failed. Returned (%d).", ret);
        data_len += ret;
    }

    ret = elf_process_note_segment(self, data_len);
    ELF_CHECK_ERR((ret > 0), ret,
                  "EXTRA_INFO note segment processing failure, returned(%d).", ret);
    return ret;
}

static int esp_core_dump_do_write_elf_pass(core_dump_elf_t *self)
{
    int tot_len = 0;

    int data_sz = elf_write_file_header(self, ELF_SEG_HEADERS_COUNT(self));
    if (self->elf_stage == ELF_STAGE_PLACE_DATA) {
        ELF_CHECK_ERR((data_sz >= 0), data_sz, "ELF header writing error, returned (%d).", data_sz);
    } else {
        ELF_CHECK_ERR((data_sz > 0), data_sz, "ELF header writing error, returned (%d).", data_sz);
    }
    tot_len += data_sz;
    // Calculate whole size include headers for all tasks and main elf header
    data_sz = elf_write_tasks_data(self);
    ELF_CHECK_ERR((data_sz > 0), data_sz, "ELF Size writing error, returned (%d).", data_sz);
    tot_len += data_sz;

    // write core dump memory regions defined by user
    data_sz = elf_write_core_dump_user_data(self);
    ELF_CHECK_ERR((data_sz >= 0), data_sz, "memory regions writing error, returned (%d).", data_sz);
    tot_len += data_sz;

    // write data with version control information and some extra info
    // this should go after tasks processing
    data_sz = elf_write_core_dump_info(self);
    ELF_CHECK_ERR((data_sz > 0), data_sz, "Version info writing failed. Returned (%d).", data_sz);
    tot_len += data_sz;

    return tot_len;
}

static esp_err_t esp_core_dump_write_elf(void)
{
    core_dump_elf_t self = { 0 };
    core_dump_header_t dump_hdr = { 0 };
    int tot_len = sizeof(dump_hdr);
    int write_len = sizeof(dump_hdr);

#if CONFIG_ESP_COREDUMP_CAPTURE_DRAM
    esp_core_dump_get_own_stack_info(&self.coredump_stack_start, &self.coredump_stack_size);
    ESP_COREDUMP_LOG_PROCESS("Core dump stack start=%p size = %d",
                             (void *)self.coredump_stack_start, self.coredump_stack_size);
#endif

    esp_err_t err = esp_core_dump_write_init();
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Elf write init failed!");
        return ESP_FAIL;
    }

    // On first pass (do not write actual data), but calculate data length needed to allocate memory
    self.elf_stage = ELF_STAGE_CALC_SPACE;
    ESP_COREDUMP_LOG_PROCESS("================= Calc data size ===============");
    int ret = esp_core_dump_do_write_elf_pass(&self);
    if (ret < 0) {
        return ret;
    }
    tot_len += ret;
    ESP_COREDUMP_LOG_PROCESS("Core dump tot_len=%lu", tot_len);
    ESP_COREDUMP_LOG_PROCESS("============== Data size = %d bytes ============", tot_len);

    // Prepare write elf
    err = esp_core_dump_write_prepare(&self.write_data, (uint32_t*)&tot_len);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to prepare core dump storage (%d)!", err);
        return err;
    }

    // Write start
    err = esp_core_dump_write_start(&self.write_data);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to start core dump (%d)!", err);
        return err;
    }

    // Write core dump header
    dump_hdr.data_len = tot_len;
    dump_hdr.version = esp_core_dump_elf_version();
    dump_hdr.tasks_num = 0; // unused in ELF format
    dump_hdr.tcb_sz = 0; // unused in ELF format
    dump_hdr.mem_segs_num = 0; // unused in ELF format
    dump_hdr.chip_rev = efuse_hal_chip_revision();
    err = esp_core_dump_write_data(&self.write_data, &dump_hdr, sizeof(core_dump_header_t));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write core dump header (%d)!", err);
        return err;
    }

    self.elf_stage = ELF_STAGE_PLACE_HEADERS;
    // set initial offset to elf segments data area
    self.elf_next_data_offset = sizeof(elfhdr) + ELF_SEG_HEADERS_COUNT(&self) * sizeof(elf_phdr);
    ret = esp_core_dump_do_write_elf_pass(&self);
    if (ret < 0) {
        return ret;
    }
    write_len += ret;
    ESP_COREDUMP_LOG_PROCESS("============== Headers size = %d bytes ============", write_len);

    self.elf_stage = ELF_STAGE_PLACE_DATA;
    // set initial offset to elf segments data area, this is not necessary in this stage, just for pretty debug output
    self.elf_next_data_offset = sizeof(elfhdr) + ELF_SEG_HEADERS_COUNT(&self) * sizeof(elf_phdr);
    ret = esp_core_dump_do_write_elf_pass(&self);
    if (ret < 0) {
        return ret;
    }
    write_len += ret;
    ESP_COREDUMP_LOG_PROCESS("=========== Data written size = %d bytes ==========", write_len);

    // Write end, update checksum
    err = esp_core_dump_write_end(&self.write_data);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to end core dump (%d)!", err);
    }

    return err;
}

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH

typedef struct {
#if ELF_CLASS == ELFCLASS32
    Elf32_Word n_type;
    Elf32_Word n_descsz;
#else
    Elf64_Word n_type;
    Elf64_Word n_descsz;
#endif
    void *n_ptr;
} elf_note_content_t;

/* Below are the helper function to parse the core dump ELF stored in flash */
static esp_err_t elf_core_dump_image_mmap(esp_partition_mmap_handle_t* core_data_handle, const void **map_addr)
{
    size_t out_size;
    assert(core_data_handle);
    assert(map_addr);

    /* Find the partition that could potentially contain a (previous) core dump. */
    const esp_partition_t *core_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                                ESP_PARTITION_SUBTYPE_DATA_COREDUMP,
                                                                NULL);
    if (!core_part) {
        ESP_COREDUMP_LOGE("Core dump partition not found!");
        return ESP_ERR_NOT_FOUND;
    }
    if (core_part->size < sizeof(uint32_t)) {
        ESP_COREDUMP_LOGE("Core dump partition too small!");
        return ESP_ERR_INVALID_SIZE;
    }
    /* Data read from the mmapped core dump partition will be garbage if flash
     * encryption is enabled in hardware and core dump partition is not encrypted
     */
    if (esp_flash_encryption_enabled() && !core_part->encrypted) {
        ESP_COREDUMP_LOGE("Flash encryption enabled in hardware and core dump partition is not encrypted!");
        return ESP_ERR_NOT_SUPPORTED;
    }
    /* Read the size of the core dump file from the partition */
    esp_err_t ret = esp_partition_read(core_part, 0, &out_size, sizeof(uint32_t));
    if (ret != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to read core dump data size");
        return ret;
    }
    /* map the full core dump partition, including the checksum. */
    return esp_partition_mmap(core_part, 0, out_size, ESP_PARTITION_MMAP_DATA,
                              map_addr, core_data_handle);
}

static void elf_parse_version_info(esp_core_dump_summary_t *summary, void *data)
{
    core_dump_elf_version_info_t *version = (core_dump_elf_version_info_t *)data;
    summary->core_dump_version = version->version;
    memcpy(summary->app_elf_sha256, version->app_elf_sha256, ELF_APP_SHA256_SIZE);
    ESP_COREDUMP_LOGD("Core dump version 0x%x", summary->core_dump_version);
    ESP_COREDUMP_LOGD("App ELF SHA2 %s", (char *)summary->app_elf_sha256);
}

static void elf_parse_exc_task_name(esp_core_dump_summary_t *summary, void *tcb_data)
{
    StaticTask_t *tcb = (StaticTask_t *) tcb_data;
    /* An ugly way to get the task name. We could possibly use pcTaskGetName here.
     * But that has assumption that TCB pointer can be used as TaskHandle. So let's
     * keep it this way. */
    memset(summary->exc_task, 0, sizeof(summary->exc_task));
    strlcpy(summary->exc_task, (char *)tcb->ucDummy7, sizeof(summary->exc_task));
    ESP_COREDUMP_LOGD("Crashing task %s", summary->exc_task);
}

static uint8_t *elf_core_dump_image_ptr(esp_partition_mmap_handle_t *core_data_handle)
{
    if (!core_data_handle) {
        return NULL;
    }

    const void *map_addr;

    esp_err_t err = elf_core_dump_image_mmap(core_data_handle, &map_addr);
    if (err != ESP_OK) {
        return NULL;
    }
    return (uint8_t *)map_addr + sizeof(core_dump_header_t);
}

static void esp_core_dump_parse_note_section(uint8_t *coredump_data, elf_note_content_t *target_notes, size_t size)
{
    elfhdr *eh = (elfhdr *)coredump_data;
    elf_phdr *phdr = (elf_phdr *)(coredump_data + eh->e_phoff);

    ESP_COREDUMP_LOGD("ELF ident %02x %c %c %c", eh->e_ident[0], eh->e_ident[1], eh->e_ident[2], eh->e_ident[3]);
    ESP_COREDUMP_LOGD("Ph_num %d offset %x", eh->e_phnum, eh->e_phoff);

    for (unsigned int i = 0; i < eh->e_phnum; i++) {
        const elf_phdr *ph = &phdr[i];
        ESP_COREDUMP_LOGD("PHDR type %d off %x vaddr %x paddr %x filesz %x memsz %x flags %x align %x",
                          ph->p_type, ph->p_offset, ph->p_vaddr, ph->p_paddr, ph->p_filesz, ph->p_memsz,
                          ph->p_flags, ph->p_align);
        if (ph->p_type == PT_NOTE) {
            size_t consumed_note_sz = 0;
            while (consumed_note_sz < ph->p_memsz) {
                const elf_note *note = (const elf_note *)(coredump_data + ph->p_offset + consumed_note_sz);
                for (size_t idx = 0; idx < size; ++idx) {
                    if (target_notes[idx].n_type == note->n_type) {
                        char *nm = (char *)&note[1];
                        target_notes[idx].n_ptr = nm + note->n_namesz;
                        target_notes[idx].n_descsz = note->n_descsz;
                        ESP_COREDUMP_LOGD("%d bytes target note (%X) found in the note section",
                                          note->n_descsz, note->n_type);
                        break;
                    }
                }
                consumed_note_sz += ALIGN_UP(note->n_namesz + note->n_descsz + sizeof(elf_note), 4);
            }
        }
    }
}

esp_err_t esp_core_dump_get_panic_reason(char *reason_buffer, size_t buffer_size)
{
    if (!reason_buffer || buffer_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_partition_mmap_handle_t core_data_handle;
    uint8_t *ptr = elf_core_dump_image_ptr(&core_data_handle);
    if (ptr == NULL) {
        return ESP_FAIL;
    }

    elf_note_content_t target_note = { .n_type = ELF_ESP_CORE_DUMP_PANIC_DETAILS_TYPE, .n_ptr = NULL };

    esp_core_dump_parse_note_section(ptr, &target_note, 1);
    if (target_note.n_ptr) {
        size_t len = MIN(target_note.n_descsz, buffer_size - 1);
        strncpy(reason_buffer, target_note.n_ptr, len);
        reason_buffer[len] = '\0';
    }
    esp_partition_munmap(core_data_handle);

    return target_note.n_ptr ? ESP_OK : ESP_ERR_NOT_FOUND;
}

esp_err_t esp_core_dump_get_summary(esp_core_dump_summary_t *summary)
{
    if (!summary) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_partition_mmap_handle_t core_data_handle;
    uint8_t *ptr = elf_core_dump_image_ptr(&core_data_handle);
    if (ptr == NULL) {
        return ESP_FAIL;
    }

    elf_note_content_t target_notes[2] = {
        [0] = { .n_type = ELF_ESP_CORE_DUMP_EXTRA_INFO_TYPE, .n_ptr = NULL },
        [1] = { .n_type = ELF_ESP_CORE_DUMP_INFO_TYPE, .n_ptr = NULL }
    };

    esp_core_dump_parse_note_section(ptr, target_notes, sizeof(target_notes) / sizeof(target_notes[0]));
    if (target_notes[0].n_ptr) {
        esp_core_dump_summary_parse_extra_info(summary, target_notes[0].n_ptr);
    }
    if (target_notes[1].n_ptr) {
        elf_parse_version_info(summary, target_notes[1].n_ptr);
    }

    /* Following code assumes that task stack segment follows the TCB segment for the respective task.
     * In general ELF does not impose any restrictions on segments' order so this can be changed without impacting core dump version.
     * More universal and flexible way would be to retrieve stack start address from crashed task TCB segment and then look for the stack segment with that address.
     */
    elfhdr *eh = (elfhdr *)ptr;
    elf_phdr *phdr = (elf_phdr *)(ptr + eh->e_phoff);
    int flag = 0;
    for (unsigned int i = 0; i < eh->e_phnum; i++) {
        const elf_phdr *ph = &phdr[i];
        if (ph->p_type == PT_LOAD) {
            if (flag) {
                esp_core_dump_summary_parse_exc_regs(summary, (void *)(ptr + ph->p_offset));
                esp_core_dump_summary_parse_backtrace_info(&summary->exc_bt_info, (void *)ph->p_vaddr,
                                                           (void *)(ptr + ph->p_offset), ph->p_memsz);
                break;
            }
            if (ph->p_vaddr == summary->exc_tcb) {
                elf_parse_exc_task_name(summary, (void *)(ptr + ph->p_offset));
                flag = 1;
            }
        }
    }

    esp_partition_munmap(core_data_handle);

    return ESP_OK;
}

#endif // CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH

#endif //CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
