// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string.h>
#include "esp_attr.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "sdkconfig.h"
#include "core_dump_elf.h"

#define ELF_CLASS ELFCLASS32

#include "elf.h"                    // for ELF file types

#define ELF_SEG_HEADERS_COUNT(_self_, _task_num_) (uint32_t)((_task_num_) * 2/*stack + tcb*/ \
                                    + 1/* regs notes */ + 1/* ver info + extra note */ + ((_self_)->interrupted_task.stack_start ? 1 : 0))

#define ELF_HLEN 52
#define ELF_CORE_SEC_TYPE 1
#define ELF_PR_STATUS_SEG_NUM 0
#define ELF_ESP_CORE_DUMP_INFO_TYPE 8266
#define ELF_ESP_CORE_DUMP_EXTRA_INFO_TYPE 677
#define ELF_NOTE_NAME_MAX_SIZE 32
#define ELF_APP_SHA256_SIZE 66

#define ELF_CHECK_ERR(a, ret_val, str, ...) \
    if (!(a)) { \
        ESP_COREDUMP_LOGE("%s(%u): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }

typedef enum
{
    ELF_STAGE_CALC_SPACE = 0,
    ELF_STAGE_PLACE_HEADERS = 1,
    ELF_STAGE_PLACE_DATA = 2
} core_dump_elf_stages_t;

typedef enum _elf_err_t
{
    ELF_PROC_ERR_SKIP_HEADER = 0,
    ELF_PROC_ERR_STACK_CORRUPTED = -1,
    ELF_PROC_ERR_WRITE_FAIL = -2,
    ELF_PROC_ERR_OTHER = -3
} core_dump_elf_proc_err_t;

typedef struct _core_dump_task_info_t
{
    elf_phdr* phdr;
    void* frame;
    core_dump_task_header_t* task_hdr;
    uint32_t task_id;
    size_t tcb_sz;
    int* size_ptr;
} core_dump_task_data_t;

typedef struct
{
    uint32_t version; // coredump version
    uint8_t app_elf_sha256[ELF_APP_SHA256_SIZE]; // sha256 of elf file
} core_dump_elf_version_info_t;

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_elf";

// Main ELF handle type
typedef struct _core_dump_elf_t
{
    core_dump_elf_version_info_t    elf_version_info;
    uint16_t                        elf_stage;
    uint32_t                        elf_next_data_offset;
    uint32_t                        bad_tasks_num;
    core_dump_task_header_t         interrupted_task;
    core_dump_write_config_t *      write_cfg;
} core_dump_elf_t;

// Represents lightweight implementation to save core dump data into ELF formatted binary

#define ALIGN(b, var) var = align(b, var)

#if CONFIG_ESP32_COREDUMP_DATA_FORMAT_ELF

static inline uint32_t align(uint32_t width, uint32_t in)
{
  return (in + (width - 1)) & -width;
}

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
        esp_err_t err = self->write_cfg->write(self->write_cfg->priv, (void*)&elf_hdr, sizeof(elf_hdr));
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
    esp_err_t err = self->write_cfg->write(self->write_cfg->priv, (void*)phdr, sizeof(elf_phdr));
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
    int data_len = data_sz;

    ELF_CHECK_ERR((data != NULL), ELF_PROC_ERR_OTHER,
                "Invalid data for segment.");

    ALIGN(4, data_len);

    if (self->elf_stage == ELF_STAGE_CALC_SPACE) {
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
    err = self->write_cfg->write(self->write_cfg->priv, data, (uint32_t)data_len);
    ELF_CHECK_ERR((err == ESP_OK), ELF_PROC_ERR_WRITE_FAIL,
                    "Write ELF segment data failure (%d)", err);
    self->elf_next_data_offset += data_len;
    return data_len;
}

static int elf_write_note(core_dump_elf_t *self,
                            const char* name,
                            uint32_t type,
                            void* data,
                            uint32_t data_sz)
{
    esp_err_t err = ESP_FAIL;
    // temporary buffer for note name
    static char name_buffer[ELF_NOTE_NAME_MAX_SIZE] = { 0 };
    elf_note note_hdr = { 0 };
    uint32_t name_len = strlen(name) + 1; // get name length including terminator
    uint32_t data_len = data_sz;

    ELF_CHECK_ERR(data, ELF_PROC_ERR_OTHER,
            "Invalid data pointer %x.", (uint32_t)data);
    ELF_CHECK_ERR((name_len <= ELF_NOTE_NAME_MAX_SIZE), 0,
                "Segment note name is too long %d.", name_len);

    ALIGN(4, data_len);
    ALIGN(4, name_len);
    uint32_t note_size = name_len + data_len + sizeof(elf_note);
    ALIGN(4, note_size);

    // write segment data during second pass
    if (self->elf_stage == ELF_STAGE_PLACE_DATA) {
        memcpy((void*)name_buffer, (void*)name, name_len);
        note_hdr.n_namesz = name_len;
        note_hdr.n_descsz = data_sz;
        note_hdr.n_type = type;
        // write note header
        err = self->write_cfg->write(self->write_cfg->priv, (void*)&note_hdr, sizeof(note_hdr));
        ELF_CHECK_ERR((err == ESP_OK), ELF_PROC_ERR_WRITE_FAIL,
                "Write ELF note header failure (%d)", err);
        // write note name
        err = self->write_cfg->write(self->write_cfg->priv, (void*)name_buffer, name_len);
        ELF_CHECK_ERR((err == ESP_OK), ELF_PROC_ERR_WRITE_FAIL,
                        "Write ELF note name failure (%d)", err);
        // write note data
        err = self->write_cfg->write(self->write_cfg->priv, (void*)data, data_len);
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

    stack_paddr = esp_core_dump_get_stack(task, &stack_vaddr, &stack_len);
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
                                task->tcb_addr, task->tcb_addr, COREDUMP_TCB_SIZE);
    int ret = elf_add_segment(self, PT_LOAD,
                                (uint32_t)task->tcb_addr,
                                (void*)task->tcb_addr,
                                COREDUMP_TCB_SIZE);
    return ret;
}

// get index of current crashed task (not always first task in the snapshot)
static int elf_get_current_task_index(core_dump_task_header_t** tasks,
                                        uint32_t task_num)
{
    int task_id;
    int curr_task_index = COREDUMP_CURR_TASK_NOT_FOUND;
    void* curr_task_handle = esp_core_dump_get_current_task_handle();

    // get index of current crashed task (not always first task in the snapshot)
    for (task_id = 0; task_id < task_num; task_id++) {
        bool tcb_is_valid = esp_core_dump_tcb_addr_is_sane((uint32_t)tasks[task_id]->tcb_addr);
        bool stack_is_valid = esp_core_dump_check_stack(tasks[task_id]->stack_start, tasks[task_id]->stack_end);
        if (stack_is_valid && tcb_is_valid && curr_task_handle == tasks[task_id]->tcb_addr) {
            curr_task_index = task_id; // save current crashed task index in the snapshot
            ESP_COREDUMP_LOG_PROCESS("Task #%d, (TCB:%x) is current crashed task.",
                                        task_id,
                                        tasks[task_id]->tcb_addr);
        }
    }
    return curr_task_index;
}

static int elf_process_task_regdump(core_dump_elf_t *self, panic_info_t *info, core_dump_task_header_t *task)
{
    bool task_is_valid = false;
    bool task_is_current = false;

    ELF_CHECK_ERR((task), ELF_PROC_ERR_OTHER, "Invalid input data.");

    if (self->elf_stage == ELF_STAGE_CALC_SPACE) {
        // Check if task tcb is corrupted (do not update the header, save as is)
        task_is_valid = esp_core_dump_check_task(info, task, &task_is_current, NULL);
        if (!task_is_valid) {
            if (task_is_current) {
                ESP_COREDUMP_LOG_PROCESS("Task has incorrect (TCB:%x)!",
                                            task->tcb_addr);
            } else {
                ESP_COREDUMP_LOG_PROCESS("The current crashed task has broken (TCB:%x)!",
                                            task->tcb_addr);
            }
            self->bad_tasks_num++;
        }
    }
    // extract registers from stack and apply elf data size for stack section
    return elf_add_regs(self, task);
}

static int elf_process_task_tcb(core_dump_elf_t *self, core_dump_task_header_t *task)
{
    int ret = ELF_PROC_ERR_OTHER;

    ELF_CHECK_ERR((task), ELF_PROC_ERR_OTHER, "Invalid input data.");

    // save tcb of the task as is and apply segment size
    ret = elf_add_tcb(self, task);
    if (ret > 0) {
        ESP_COREDUMP_LOG_PROCESS("Task (TCB:%x) processing completed.",
                                    task->tcb_addr);
    } else {
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
    if (ret > 0) {
        ESP_COREDUMP_LOG_PROCESS("Task (TCB:%x), (Stack:%x) stack is processed.",
                                    task->tcb_addr,
                                    task->stack_start);
    } else {
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
        notes_size += sizeof(seg_hdr);
    } else {
        // in "Place Data" phase segment body is been already filled by other functions
        ESP_COREDUMP_LOG_PROCESS("Add NOTE segment, size=%d, start_off=0x%x",
                                    notes_size, self->elf_next_data_offset);
        self->elf_next_data_offset += notes_size;
    }
    return (int)notes_size;
}

static int elf_process_tasks_regs(core_dump_elf_t *self, panic_info_t *info,
                                    core_dump_task_header_t** tasks,
                                    uint32_t task_num)
{
    int len = 0;

    uint32_t curr_task_index = elf_get_current_task_index(tasks, task_num);
    if (curr_task_index == COREDUMP_CURR_TASK_NOT_FOUND) {
        ESP_COREDUMP_LOG_PROCESS("The current crashed task is broken.");
        curr_task_index = 0;
    }

    // place current task dump first
    int ret = elf_process_task_regdump(self, info, tasks[curr_task_index]);
    if (self->elf_stage == ELF_STAGE_PLACE_HEADERS) {
        // when writing segments headers this function writes nothing
        ELF_CHECK_ERR((ret >= 0), ret, "Task #%d, PR_STATUS write failed, return (%d).", curr_task_index, ret);
    } else {
        ELF_CHECK_ERR((ret > 0), ret, "Task #%d, PR_STATUS write failed, return (%d).", curr_task_index, ret);
    }
    len += ret;

    // processes PR_STATUS and register dump for each task
    // each call to the processing function appends PR_STATUS note into note segment
    // and writes data or updates the segment note header accordingly (if phdr is set)
    for (int task_id = 0; task_id < task_num; task_id++) {
        if (task_id == curr_task_index) {
            continue; // skip current task (already processed)
        }
        ret = elf_process_task_regdump(self, info, tasks[task_id]);
        if (self->elf_stage == ELF_STAGE_PLACE_HEADERS) {
            // when writing segments headers this function writes nothing
            ELF_CHECK_ERR((ret >= 0), ret, "Task #%d, PR_STATUS write failed, return (%d).", task_id, ret);
        } else {
            ELF_CHECK_ERR((ret > 0), ret, "Task #%d, PR_STATUS write failed, return (%d).", task_id, ret);
        }
        len += ret;
    }
    ret = elf_process_note_segment(self, len);
    ELF_CHECK_ERR((ret > 0), ret,
                    "PR_STATUS note segment processing failure, returned(%d).", ret);

    if (esp_core_dump_in_isr_context()) {
        if (self->elf_stage == ELF_STAGE_CALC_SPACE) {
            // in this stage we can safely replace task's stack with IRQ's one
            // if task had corrupted stack it was replaced with fake one in HW dependent code called by elf_process_task_regdump()
            // in the "write data" stage registers from ISR's stack will be saved in PR_STATUS
            self->interrupted_task.stack_start = tasks[curr_task_index]->stack_start;
            self->interrupted_task.stack_end = tasks[curr_task_index]->stack_end;
            uint32_t isr_stk_end = esp_core_dump_get_isr_stack_end();
            ESP_COREDUMP_LOG_PROCESS("Add ISR stack %lu (%x - %x)", isr_stk_end - (uint32_t)info->frame, (uint32_t)info->frame, isr_stk_end);
            tasks[curr_task_index]->stack_start = (uint32_t)info->frame;
            tasks[curr_task_index]->stack_end = isr_stk_end;
        }

        // actually we write current task's stack here which was replaced by ISR's
        len = elf_add_stack(self, &self->interrupted_task);
        ELF_CHECK_ERR((len > 0), len, "Interrupted task stack write failed, return (%d).", len);
        ret += len;
    }
    return ret;
}

static int elf_write_tasks_data(core_dump_elf_t *self, panic_info_t *info,
                                core_dump_task_header_t** tasks,
                                uint32_t task_num)
{
    int elf_len = 0;
    int task_id;
    int ret = ELF_PROC_ERR_OTHER;

    ELF_CHECK_ERR((info && tasks), ELF_PROC_ERR_OTHER, "Invalid input data.");

    ret = elf_process_tasks_regs(self, info, tasks, task_num);
    ELF_CHECK_ERR((ret > 0), ret, "Tasks regs addition failed, return (%d).", ret);
    elf_len += ret;
    self->bad_tasks_num = 0; // reset bad task counter

    // processes all task's stack data and writes segment data into partition
    // if flash configuration is set
    for (task_id = 0; task_id < task_num; task_id++) {
        ret = elf_process_task_tcb(self, tasks[task_id]);
        ELF_CHECK_ERR((ret > 0), ret,
                        "Task #%d, TCB write failed, return (%d).", task_id, ret);
        elf_len += ret;
        ret = elf_process_task_stack(self, tasks[task_id]);
        ELF_CHECK_ERR((ret != ELF_PROC_ERR_WRITE_FAIL), ELF_PROC_ERR_WRITE_FAIL,
                        "Task #%d, stack write failed, return (%d).", task_id, ret);
        elf_len += ret;
    }
    return elf_len;
}

static int elf_write_core_dump_info(core_dump_elf_t *self)
{
    void *extra_info;

    int data_len = (int)sizeof(self->elf_version_info.app_elf_sha256);
    data_len = esp_ota_get_app_elf_sha256((char*)self->elf_version_info.app_elf_sha256, (size_t)data_len);
    ESP_COREDUMP_LOG_PROCESS("Application SHA256='%s', length=%d.",
                                self->elf_version_info.app_elf_sha256, data_len);
    self->elf_version_info.version = COREDUMP_VERSION;
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
                        "EXTRA_INFO",
                        ELF_ESP_CORE_DUMP_EXTRA_INFO_TYPE,
                        extra_info,
                        extra_info_len);
    ELF_CHECK_ERR((ret > 0), ret, "Extra info note write failed. Returned (%d).", ret);
    data_len += ret;

    ret = elf_process_note_segment(self, data_len);
    ELF_CHECK_ERR((ret > 0), ret,
                    "EXTRA_INFO note segment processing failure, returned(%d).", ret);
    return ret;
}

static int esp_core_dump_do_write_elf_pass(core_dump_elf_t *self, panic_info_t *info,
                                            core_dump_task_header_t** tasks,
                                            uint32_t task_num)
{
    int tot_len = 0;

    int data_sz = elf_write_file_header(self, ELF_SEG_HEADERS_COUNT(self, task_num));
    if (self->elf_stage == ELF_STAGE_PLACE_DATA) {
        ELF_CHECK_ERR((data_sz >= 0), data_sz, "ELF header writing error, returned (%d).", data_sz);
    } else {
        ELF_CHECK_ERR((data_sz > 0), data_sz, "ELF header writing error, returned (%d).", data_sz);
    }
    tot_len += data_sz;
    // Calculate whole size include headers for all tasks and main elf header
    data_sz = elf_write_tasks_data(self, info, tasks, task_num);
    ELF_CHECK_ERR((data_sz > 0), data_sz, "ELF Size writing error, returned (%d).", data_sz);
    tot_len += data_sz;
    // write data with version control information and some extra info
    // this should go after tasks processing
    data_sz = elf_write_core_dump_info(self);
    ELF_CHECK_ERR((data_sz > 0), data_sz, "Version info writing failed. Returned (%d).", data_sz);
    tot_len += data_sz;

    return tot_len;
}

esp_err_t esp_core_dump_write_elf(panic_info_t *info, core_dump_write_config_t *write_cfg)
{
    esp_err_t err = ESP_OK;
    static core_dump_task_header_t *tasks[CONFIG_ESP32_CORE_DUMP_MAX_TASKS_NUM];
    static core_dump_elf_t self;
    core_dump_header_t dump_hdr;
    uint32_t tcb_sz = COREDUMP_TCB_SIZE, task_num;
    int tot_len = sizeof(dump_hdr);
    int write_len = sizeof(dump_hdr);

    ELF_CHECK_ERR((info && write_cfg), ESP_ERR_INVALID_ARG, "Invalid input data.");

    task_num = esp_core_dump_get_tasks_snapshot(tasks, CONFIG_ESP32_CORE_DUMP_MAX_TASKS_NUM);
    ESP_COREDUMP_LOGI("Found tasks: %d", task_num);

    self.write_cfg = write_cfg;

    esp_core_dump_init_extra_info();
    // On first pass (do not write actual data), but calculate data length needed to allocate memory
    self.elf_stage = ELF_STAGE_CALC_SPACE;
    ESP_COREDUMP_LOG_PROCESS("================= Calc data size ===============");
    int ret = esp_core_dump_do_write_elf_pass(&self, info, tasks, task_num);
    if (ret < 0) return ret;
    tot_len += ret;
    ESP_COREDUMP_LOG_PROCESS("Core dump tot_len=%lu, tasks processed: %d, broken tasks: %d",
                                tot_len, task_num, self.bad_tasks_num);
    ESP_COREDUMP_LOG_PROCESS("============== Data size = %d bytes ============", tot_len);

    // Prepare write elf
    if (write_cfg->prepare) {
        err = write_cfg->prepare(write_cfg->priv, (uint32_t*)&tot_len);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to prepare core dump storage (%d)!", err);
            return err;
        }
    }

    // Write start
    if (write_cfg->start) {
        err = write_cfg->start(write_cfg->priv);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to start core dump (%d)!", err);
            return err;
        }
    }

    write_cfg->bad_tasks_num = self.bad_tasks_num;

    // Write core dump header
    ALIGN(4, tot_len);
    ALIGN(4, tcb_sz);
    dump_hdr.data_len = tot_len;
    dump_hdr.version = COREDUMP_VERSION;
    dump_hdr.tasks_num = task_num; // broken tasks are repaired
    dump_hdr.tcb_sz = tcb_sz;
    dump_hdr.mem_segs_num = 0;
    err = write_cfg->write(write_cfg->priv,
                            (void*)&dump_hdr,
                            sizeof(core_dump_header_t));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write core dump header (%d)!", err);
        return err;
    }

    self.elf_stage = ELF_STAGE_PLACE_HEADERS;
    // set initial offset to elf segments data area
    self.elf_next_data_offset = sizeof(elfhdr) + ELF_SEG_HEADERS_COUNT(&self, task_num) * sizeof(elf_phdr);
    ret = esp_core_dump_do_write_elf_pass(&self, info, tasks, task_num);
    if (ret < 0) return ret;
    write_len += ret;
    ESP_COREDUMP_LOG_PROCESS("============== Headers size = %d bytes ============", write_len);

    self.elf_stage = ELF_STAGE_PLACE_DATA;
    // set initial offset to elf segments data area, this is not necessary in this stage, just for pretty debug output
    self.elf_next_data_offset = sizeof(elfhdr) + ELF_SEG_HEADERS_COUNT(&self, task_num) * sizeof(elf_phdr);
    ret = esp_core_dump_do_write_elf_pass(&self, info, tasks, task_num);
    if (ret < 0) return ret;
    write_len += ret;
    ESP_COREDUMP_LOG_PROCESS("=========== Data written size = %d bytes ==========", write_len);

    // Get checksum size
    write_len += esp_core_dump_checksum_finish(write_cfg->priv, NULL);
    if (write_len != tot_len) {
        ESP_COREDUMP_LOGD("Write ELF failed (wrong length): %d != %d.", tot_len, write_len);
    }
    // Write end, update checksum
    if (write_cfg->end) {
        err = write_cfg->end(write_cfg->priv);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to end core dump (%d)!", err);
            return err;
        }
    }
    return err;
}

#endif //CONFIG_ESP32_COREDUMP_DATA_FORMAT_ELF
