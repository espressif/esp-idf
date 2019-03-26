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
#include "esp32/rom/crc.h"
#include "esp_debug_helpers.h"
#include "esp_partition.h"
#include "esp_core_dump_priv.h"

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_common";

#if CONFIG_ESP32_ENABLE_COREDUMP

static esp_err_t esp_core_dump_write_binary(void *frame, core_dump_write_config_t *write_cfg)
{
    esp_err_t err;
    core_dump_task_header_t tasks[CONFIG_ESP32_CORE_DUMP_MAX_TASKS_NUM];
    uint32_t tcb_sz, task_num, tcb_sz_padded;
    bool task_is_valid = false;
    uint32_t data_len = 0, i;
    union
    {
        core_dump_header_t      hdr;
        core_dump_task_header_t task_hdr;
    } dump_data;

    task_num = esp_core_dump_get_tasks_snapshot(tasks, CONFIG_ESP32_CORE_DUMP_MAX_TASKS_NUM, &tcb_sz);
    ESP_COREDUMP_LOGI("Found tasks: (%d)!", task_num);

    // Take TCB padding into account, actual TCB size will be stored in header
    if (tcb_sz % sizeof(uint32_t))
        tcb_sz_padded = (tcb_sz / sizeof(uint32_t) + 1) * sizeof(uint32_t);
    else
        tcb_sz_padded = tcb_sz;

    // Verifies all tasks in the snapshot
    for (i = 0; i < task_num; i++) {
        task_is_valid = esp_core_dump_process_tcb(frame, &tasks[i], tcb_sz);
        // Check if task tcb is corrupted
        if (!task_is_valid) {
            write_cfg->bad_tasks_num++;
            continue;
        } else {
            data_len += (tcb_sz_padded + sizeof(core_dump_task_header_t));
        }
        uint32_t len = 0;
        task_is_valid = esp_core_dump_process_stack(&tasks[i], &len);
        if (task_is_valid) {
            // Increase core dump size by task stack size
            data_len += len;
        } else {
            // If task tcb is ok but stack is corrupted
            write_cfg->bad_tasks_num++;
        }
    }
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
    dump_data.hdr.data_len  = data_len;
    dump_data.hdr.version   = COREDUMP_VERSION;
    dump_data.hdr.tasks_num = task_num - write_cfg->bad_tasks_num;
    dump_data.hdr.tcb_sz    = tcb_sz;
    err = write_cfg->write(write_cfg->priv, &dump_data, sizeof(core_dump_header_t));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write core dump header (%d)!", err);
        return err;
    }
    // Write tasks
    for (i = 0; i < task_num; i++) {
        if (!esp_tcb_addr_is_sane((uint32_t)tasks[i].tcb_addr, tcb_sz)) {
            ESP_COREDUMP_LOG_PROCESS("Skip TCB with bad addr %x!", tasks[i].tcb_addr);
            continue;
        }
        ESP_COREDUMP_LOG_PROCESS("Dump task %x", tasks[i].tcb_addr);
        // Save TCB address, stack base and stack top addr
        dump_data.task_hdr.tcb_addr    = tasks[i].tcb_addr;
        dump_data.task_hdr.stack_start = tasks[i].stack_start;
        dump_data.task_hdr.stack_end   = tasks[i].stack_end;
        err = write_cfg->write(write_cfg->priv, (void*)&dump_data, sizeof(core_dump_task_header_t));
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to write task header (%d)!", err);
            return err;
        }
        // Save TCB
        err = write_cfg->write(write_cfg->priv, tasks[i].tcb_addr, tcb_sz);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to write TCB (%d)!", err);
            return err;
        }
        // Save task stack
        if (tasks[i].stack_start != 0 && tasks[i].stack_end != 0) {
            err = write_cfg->write(write_cfg->priv, (void*)tasks[i].stack_start,
                    tasks[i].stack_end - tasks[i].stack_start);
            if (err != ESP_OK) {
                ESP_COREDUMP_LOGE("Failed to write task stack (%d)!", err);
                return err;
            }
        } else {
            ESP_COREDUMP_LOG_PROCESS("Skip corrupted task %x stack!", tasks[i].tcb_addr);
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
