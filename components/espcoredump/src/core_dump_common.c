// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include <stdbool.h>
#include "rom/crc.h"
#include "esp_panic.h"
#include "esp_partition.h"
#include "esp_core_dump_priv.h"

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_common";

#if CONFIG_ESP32_ENABLE_COREDUMP

static inline uint32_t esp_core_dump_get_tcb_len(void)
{
    if (COREDUMP_TCB_SIZE % sizeof(uint32_t)) {
        return ((COREDUMP_TCB_SIZE / sizeof(uint32_t) + 1) * sizeof(uint32_t));
    }
    return COREDUMP_TCB_SIZE;
}

static inline uint32_t esp_core_dump_get_stack_len(uint32_t stack_start, uint32_t stack_end)
{
    uint32_t len = stack_end - stack_start;
    // Take stack padding into account
    return (len + sizeof(uint32_t) - 1) & ~(sizeof(uint32_t) - 1);
}

static esp_err_t esp_core_dump_save_task(core_dump_write_config_t *write_cfg,
                                            core_dump_task_header_t *task)
{
    uint32_t stk_paddr, stk_len;

    if (task->tcb_addr == 0) {
        ESP_COREDUMP_LOG_PROCESS("Skip task with bad TCB addr!");
        return ESP_OK; // skip
    }
    ESP_COREDUMP_LOG_PROCESS("Dump task %x", task->tcb_addr);
    int err = write_cfg->write(write_cfg->priv, task, sizeof(*task));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write task header (%d)!", err);
        return err;
    }
    // Save TCB
    err = write_cfg->write(write_cfg->priv, task->tcb_addr, esp_core_dump_get_tcb_len());
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write TCB (%d)!", err);
        return err;
    }
    stk_paddr = esp_core_dump_get_stack(task, &stk_len);
    stk_len = esp_core_dump_get_stack_len(stk_paddr, stk_paddr+stk_len);
    // Save task stack
    err = write_cfg->write(write_cfg->priv, (void *)stk_paddr, stk_len);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write task stack (%d)!", err);
        return err;
    }
    return ESP_OK;
}

static esp_err_t esp_core_dump_save_mem_segment(core_dump_write_config_t* write_cfg,
                                                core_dump_mem_seg_header_t* seg)
{
    esp_err_t err = ESP_FAIL;

    if (!esp_core_dump_mem_seg_is_sane(seg->start, seg->size)) {
        ESP_COREDUMP_LOG_PROCESS("Skip invalid memory segment, (%x, %lu)!",
                                seg->start, seg->size);
        return ESP_OK;
    }
    // Save mem segment header
    err = write_cfg->write(write_cfg->priv, (void*)seg, sizeof(core_dump_mem_seg_header_t));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write memory segment header, error=%d!", err);
        return err;
    }
    // Save memory contents
    err = write_cfg->write(write_cfg->priv, (void*)seg->start, seg->size);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write memory segment, (%x, %lu), error=%d!",
                                seg->start, seg->size, err);
        return err;
    }
    ESP_COREDUMP_LOG_PROCESS("Memory segment (%x, %lu) is saved.",
                                seg->start, seg->size);
    return ESP_OK;
}

static esp_err_t esp_core_dump_write_binary(void *frame, core_dump_write_config_t *write_cfg)
{
    esp_err_t err;
    static core_dump_task_header_t tasks[CONFIG_ESP32_CORE_DUMP_MAX_TASKS_NUM];
    uint32_t task_num, tcb_sz_padded;
    bool task_is_valid = false, is_current_task;
    uint32_t data_len = 0, i, curr_task_idx = (uint32_t)-1, good_task_idx = (uint32_t)-1;
    core_dump_header_t      hdr;
    core_dump_mem_seg_header_t interrupted_task_stack = {.start = 0, .size = 0};

    task_num = esp_core_dump_get_tasks_snapshot(tasks, CONFIG_ESP32_CORE_DUMP_MAX_TASKS_NUM);
    ESP_COREDUMP_LOGI("Found tasks: (%d)!", task_num);

    tcb_sz_padded = esp_core_dump_get_tcb_len();

    // Verifies all tasks in the snapshot
    for (i = 0; i < task_num; i++) {
        task_is_valid = esp_core_dump_process_tcb(frame, &tasks[i], &is_current_task);
        // Check if task tcb is corrupted
        if (!task_is_valid) {
            write_cfg->bad_tasks_num++;
            continue;
        } else {
            data_len += (tcb_sz_padded + sizeof(core_dump_task_header_t));
        }
        if (is_current_task) {
            curr_task_idx = i;
            ESP_COREDUMP_LOGI("Task #%d (TCB:%x) is crashed task.",
                                        i, tasks[i].tcb_addr);
        }
        uint32_t len = 0;
        task_is_valid = esp_core_dump_process_stack(&tasks[i], &len);
        if (task_is_valid) {
            // Increase core dump size by task stack size
            data_len += esp_core_dump_get_stack_len(tasks[i].stack_start, tasks[i].stack_end);
            ESP_COREDUMP_LOG_PROCESS("Core dump stack,i = %d, len = %d data_len=%d", i, len, data_len);
            good_task_idx = i;
        } else {
            // If task tcb is ok but stack is corrupted
            write_cfg->bad_tasks_num++;
            data_len -= (tcb_sz_padded + sizeof(core_dump_task_header_t));
        }
    }

    if (curr_task_idx == (uint32_t)-1) {
        curr_task_idx = good_task_idx;
    }
    assert((curr_task_idx != (uint32_t)-1) && "No any good task in the system!");
    XtExcFrame * exc_frame =(XtExcFrame *)frame;
    if (esp_core_dump_in_isr_context() || esp_core_dump_check_in_bt_isr(exc_frame->a1)) {
        interrupted_task_stack.start = tasks[curr_task_idx].stack_start;
        interrupted_task_stack.size = esp_core_dump_get_stack_len(tasks[curr_task_idx].stack_start, tasks[curr_task_idx].stack_end);
        // size of the task's stack has been already taken into account, also addresses have also been checked
        data_len += sizeof(core_dump_mem_seg_header_t);
        tasks[curr_task_idx].stack_start = (uint32_t)frame;
        if (esp_core_dump_check_in_bt_isr(exc_frame->a1)) {
            extern uint32_t _l4_intr_stack_end;
            tasks[curr_task_idx].stack_end = &_l4_intr_stack_end;
        } else {
            tasks[curr_task_idx].stack_end = esp_core_dump_get_isr_stack_end();
        }
        ESP_COREDUMP_LOG_PROCESS("Add ISR stack %lu bytes [%x..%x]",
            tasks[curr_task_idx].stack_end - tasks[curr_task_idx].stack_start,
            tasks[curr_task_idx].stack_start, tasks[curr_task_idx].stack_end);
        // take into account size of the ISR stack
        data_len += esp_core_dump_get_stack_len(tasks[curr_task_idx].stack_start, tasks[curr_task_idx].stack_end);
    }

    core_dump_log_header_t logs = { 0 };
    if (esp_core_dump_process_log(&logs) == true) {
        data_len += (logs.len + sizeof(core_dump_log_header_t));
    }
    ESP_COREDUMP_LOGI("LOG dump len = (%d %p)", logs.len, logs.start);
    // Add core dump header size
    data_len += sizeof(core_dump_header_t);
    ESP_COREDUMP_LOG_PROCESS("Core dump len = %lu (%d %d)", data_len, task_num, write_cfg->bad_tasks_num);

    // Prepare write
    if (write_cfg->prepare) {
        err = write_cfg->prepare(write_cfg->priv, &data_len);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to prepare core dump (%d)!", err);
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
    // Write header
    hdr.data_len  = data_len;
    hdr.version   = COREDUMP_VERSION;
    hdr.tasks_num = task_num - write_cfg->bad_tasks_num;
    hdr.tcb_sz    = COREDUMP_TCB_SIZE;
    hdr.mem_segs_num = 0;
    if (interrupted_task_stack.size > 0) {
        hdr.mem_segs_num++; // stack of interrupted task
    }
    err = write_cfg->write(write_cfg->priv, &hdr, sizeof(hdr));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write core dump header (%d)!", err);
        return err;
    }
    // save crashed task as the first one
    err = esp_core_dump_save_task(write_cfg, &tasks[curr_task_idx]);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to save crashed task #%d (TCB:%x), error=%d!",
                            curr_task_idx, tasks[curr_task_idx].tcb_addr, err);
        return err;
    }
    // Write tasks
    for (i = 0; i < task_num; i++) {
        if (i == curr_task_idx)
            continue;
        err = esp_core_dump_save_task(write_cfg, &tasks[i]);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to save core dump task #%d (TCB:%x), error=%d!",
                                    i, tasks[i].tcb_addr, err);
            return err;
        }
    }
    if (interrupted_task_stack.size > 0) {
        err = esp_core_dump_save_mem_segment(write_cfg, &interrupted_task_stack);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to save interrupted task stack, error=%d!", err);
            return err;
        }
    }

    union {
        uint32_t i32[128];
        char     str[512];
    } iram_str;

    if (logs.len > 0 && logs.start != NULL) {
        err = write_cfg->write(write_cfg->priv, (void*)&logs, sizeof(core_dump_log_header_t));
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to save log, error=%d!", err);
            return err;
        }
        for (int i = 0; i < logs.len / 512; i++) {
            for (int j = 0; j < 128; j++) {
                if (i * 128 + j < logs.len / 4) {
                    iram_str.i32[j] = logs.start[i * 128+ j];
                }
            }
            err = write_cfg->write(write_cfg->priv, (void*)iram_str.str, 512);
        }
    }

    // write end
    if (write_cfg->end) {
        err = write_cfg->end(write_cfg->priv);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to end core dump (%d)!", err);
            return err;
        }
    }
    if (write_cfg->bad_tasks_num) {
        ESP_COREDUMP_LOGE("Skipped %d tasks with bad TCB!", write_cfg->bad_tasks_num);
    }
    return err;
}

inline void esp_core_dump_write(void *frame, core_dump_write_config_t *write_cfg)
{
    esp_err_t err = esp_core_dump_write_binary(frame, write_cfg);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Core dump write binary failed with error: %d", err);
    }
}

#endif

void esp_core_dump_init()
{
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
    esp_core_dump_flash_init();
#endif
#if CONFIG_ESP32_ENABLE_COREDUMP_TO_UART
    ESP_COREDUMP_LOGI("Init core dump to UART");
#endif
}

esp_err_t esp_core_dump_image_get(size_t* out_addr, size_t *out_size)
{
    esp_err_t err;
    const void *core_data;
    spi_flash_mmap_handle_t core_data_handle;

    if (out_addr == NULL || out_size == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *core_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, NULL);
    if (!core_part) {
        ESP_LOGE(TAG, "No core dump partition found!");
        return ESP_FAIL;
    }
    if (core_part->size < sizeof(uint32_t)) {
        ESP_LOGE(TAG, "Too small core dump partition!");
        return ESP_FAIL;
    }

    err = esp_partition_mmap(core_part, 0,  sizeof(uint32_t),
                             SPI_FLASH_MMAP_DATA, &core_data, &core_data_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mmap core dump data (%d)!", err);
        return err;
    }
    uint32_t *dw = (uint32_t *)core_data;
    *out_size = *dw;
    spi_flash_munmap(core_data_handle);

    // remap full core dump with CRC
    err = esp_partition_mmap(core_part, 0, *out_size,
                             SPI_FLASH_MMAP_DATA, &core_data, &core_data_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mmap core dump data (%d)!", err);
        return err;
    }
    uint32_t *crc = (uint32_t *)(((uint8_t *)core_data) + *out_size);
    crc--; // Point to CRC field
    // Calc CRC over core dump data except for CRC field
    core_dump_crc_t cur_crc = crc32_le(0, (uint8_t const *)core_data, *out_size - sizeof(core_dump_crc_t));
    if (*crc != cur_crc) {
        ESP_LOGE(TAG, "Core dump data CRC check failed: 0x%x -> 0x%x!", *crc, cur_crc);
        spi_flash_munmap(core_data_handle);
        return ESP_FAIL;
    }

    spi_flash_munmap(core_data_handle);

    *out_addr = core_part->address;
    return ESP_OK;
}
