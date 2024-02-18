/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "core_dump_binary.h"
#include "esp_core_dump_port.h"
#include "esp_core_dump_common.h"
#include "hal/efuse_hal.h"

#if CONFIG_ESP_COREDUMP_DATA_FORMAT_BIN

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_binary";


static esp_err_t esp_core_dump_save_task(core_dump_write_config_t *write_cfg,
                                         core_dump_task_header_t *task)
{
    esp_err_t err = ESP_FAIL;
    uint32_t stk_vaddr = 0;
    uint32_t stk_paddr = 0;
    uint32_t stk_len = esp_core_dump_get_stack(task, &stk_vaddr, &stk_paddr);

    stk_len = esp_core_dump_get_memory_len(stk_vaddr, stk_vaddr+stk_len);

    // Save memory segment header
    err = write_cfg->write(write_cfg->priv, (void*)task, sizeof(core_dump_task_header_t));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write task header, error=%d!", err);
        return err;
    }
    // Save TCB block
    err = write_cfg->write(write_cfg->priv, task->tcb_addr, esp_core_dump_get_tcb_len());
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write TCB, error=%d!", err);
        return err;
    }
    // Save task stack
    err = write_cfg->write(write_cfg->priv, (void*)stk_paddr, stk_len);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write stack for task (TCB:%x), stack_start=%x, error=%d!",
                                task->tcb_addr,
                                stk_vaddr,
                                err);
        return err;
    }

    ESP_COREDUMP_LOG_PROCESS("Task (TCB:%x) dump is saved.",
                                    task->tcb_addr);
    return ESP_OK;
}

static esp_err_t esp_core_dump_save_mem_segment(core_dump_write_config_t* write_cfg,
                                                core_dump_mem_seg_header_t* seg)
{
    esp_err_t err = ESP_FAIL;

    if (!esp_core_dump_mem_seg_is_sane(seg->start, seg->size)) {
        ESP_COREDUMP_LOGE("Failed to write memory segment, (%x, %lu)!",
                                seg->start, seg->size);
        return ESP_FAIL;
    }
    // Save TCB address, stack base and stack top addr
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

esp_err_t esp_core_dump_write_binary(core_dump_write_config_t *write_cfg)
{
    esp_err_t err = ESP_OK;
    uint32_t tcb_sz = esp_core_dump_get_tcb_len();
    uint32_t data_len = 0;
    uint32_t bad_tasks_num = 0;
    core_dump_header_t hdr = { 0 };
    core_dump_task_header_t task_hdr = { 0 };
    core_dump_mem_seg_header_t mem_seg = { 0 };
    TaskIterator_t task_iter;
    void *cur_task = NULL;

    // Verifies all tasks in the snapshot
    esp_core_dump_reset_tasks_snapshots_iter();
    esp_core_dump_task_iterator_init(&task_iter);
    while (esp_core_dump_task_iterator_next(&task_iter) != -1) {
        if (!esp_core_dump_get_task_snapshot(task_iter.pxTaskHandle, &task_hdr, &mem_seg)) {
            bad_tasks_num++;
            continue;
        }
        hdr.tasks_num++;
        if (task_iter.pxTaskHandle == esp_core_dump_get_current_task_handle()) {
            cur_task = task_iter.pxTaskHandle;
            ESP_COREDUMP_LOG_PROCESS("Task %x %x is first crashed task.", cur_task, task_hdr.tcb_addr);
        }
        ESP_COREDUMP_LOG_PROCESS("Stack len = %lu (%x %x)", task_hdr.stack_end-task_hdr.stack_start,
                                    task_hdr.stack_start, task_hdr.stack_end);
        // Increase core dump size by task stack size
        uint32_t stk_vaddr = 0;
        uint32_t stk_paddr = 0;
        uint32_t stk_len = esp_core_dump_get_stack(&task_hdr, &stk_vaddr, &stk_paddr);
        data_len += esp_core_dump_get_memory_len(stk_vaddr, stk_vaddr+stk_len);
        // Add tcb size
        data_len += (tcb_sz + sizeof(core_dump_task_header_t));
        if (mem_seg.size > 0) {
            ESP_COREDUMP_LOG_PROCESS("Add interrupted task stack %lu bytes @ %x",
                    mem_seg.size, mem_seg.start);
            data_len += esp_core_dump_get_memory_len(mem_seg.start, mem_seg.start+mem_seg.size);
            data_len += sizeof(core_dump_mem_seg_header_t);
            hdr.mem_segs_num++;
        }
    }
    ESP_COREDUMP_LOGI("Found tasks: good %d, bad %d, mem segs %d", hdr.tasks_num, bad_tasks_num, hdr.mem_segs_num);

    // Check if current task TCB is broken
    if (cur_task == NULL) {
        ESP_COREDUMP_LOG_PROCESS("The current crashed task is broken.");
        esp_core_dump_task_iterator_init(&task_iter);
		while (esp_core_dump_task_iterator_next(&task_iter) != -1) {
			if (task_iter.pxTaskHandle != NULL) {
				cur_task = task_iter.pxTaskHandle;
				break;
			}
		}
		if (cur_task == NULL) {
			ESP_COREDUMP_LOGE("No valid tasks in the system!");
			return ESP_FAIL;
		}
    }

    // Add user memory regions data size
    for (coredump_region_t i = COREDUMP_MEMORY_START; i < COREDUMP_MEMORY_MAX; i++) {
        uint32_t start = 0;
        int data_sz = esp_core_dump_get_user_ram_info(i, &start);
        if (data_sz < 0) {
            ESP_COREDUMP_LOGE("Invalid memory segment size!");
            return ESP_FAIL;
        }
        if (data_sz > 0) {
            hdr.mem_segs_num++;
            data_len += sizeof(core_dump_mem_seg_header_t) + esp_core_dump_get_memory_len(start, start + data_sz);
        }
    }

    // Add core dump header size
    data_len += sizeof(core_dump_header_t);

    ESP_COREDUMP_LOG_PROCESS("Core dump length=%lu, tasks processed: %d, broken tasks: %d",
                                data_len, hdr.tasks_num, bad_tasks_num);
    // Prepare write
    if (write_cfg->prepare) {
        err = write_cfg->prepare(write_cfg->priv, &data_len);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to prepare core dump, error=%d!", err);
            return err;
        }
    }

    // Write start
    if (write_cfg->start) {
        err = write_cfg->start(write_cfg->priv);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to start core dump, error=%d!", err);
            return err;
        }
    }

    // Write header
    hdr.data_len  = data_len;
    hdr.version   = COREDUMP_VERSION_BIN_CURRENT;
    hdr.tcb_sz    = tcb_sz;
    hdr.chip_rev  = efuse_hal_chip_revision();
    err = write_cfg->write(write_cfg->priv, &hdr, sizeof(core_dump_header_t));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write core dump header error=%d!", err);
        return err;
    }

    // Save tasks
    esp_core_dump_reset_tasks_snapshots_iter();
    // Write first crashed task data first (not always first task in the snapshot)
    ESP_COREDUMP_LOGD("Save first crashed task %x", cur_task);
    if (esp_core_dump_get_task_snapshot(cur_task, &task_hdr, NULL)) {
        err = esp_core_dump_save_task(write_cfg, &task_hdr);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to save first crashed task %x, error=%d!",
                                task_hdr.tcb_addr, err);
            return err;
        }
    }
    // Write all other tasks in the snapshot
    esp_core_dump_task_iterator_init(&task_iter);
    while (esp_core_dump_task_iterator_next(&task_iter) != -1) {
        if (!esp_core_dump_get_task_snapshot(task_iter.pxTaskHandle, &task_hdr, NULL))
            continue;
        // Skip first crashed task
        if (task_iter.pxTaskHandle == cur_task) {
            continue;
        }
        ESP_COREDUMP_LOGD("Save task %x (TCB:%x, stack:%x..%x)",
			task_iter.pxTaskHandle, task_hdr.tcb_addr, task_hdr.stack_start, task_hdr.stack_end);
        err = esp_core_dump_save_task(write_cfg, &task_hdr);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to save core dump task %x, error=%d!",
                                    task_hdr.tcb_addr, err);
            return err;
        }
    }

    // Save interrupted stacks of the tasks
    // Actually there can be tasks interrupted at the same time, one on every core including the crashed one.
    esp_core_dump_reset_tasks_snapshots_iter();
    esp_core_dump_task_iterator_init(&task_iter);
	while (esp_core_dump_task_iterator_next(&task_iter) != -1) {
        if (!esp_core_dump_get_task_snapshot(task_iter.pxTaskHandle, &task_hdr, &mem_seg))
            continue;
        if (mem_seg.size > 0) {
            ESP_COREDUMP_LOG_PROCESS("Save interrupted task stack %lu bytes @ %x",
                    mem_seg.size, mem_seg.start);
            err = esp_core_dump_save_mem_segment(write_cfg, &mem_seg);
            if (err != ESP_OK) {
                ESP_COREDUMP_LOGE("Failed to save interrupted task stack, error=%d!", err);
                return err;
            }
        }
    }

    // save user memory regions
    if (esp_core_dump_get_user_ram_segments() > 0) {
        for (coredump_region_t i = COREDUMP_MEMORY_START; i < COREDUMP_MEMORY_MAX; i++) {
            uint32_t start = 0;
            int data_sz = esp_core_dump_get_user_ram_info(i, &start);

            if (data_sz < 0) {
                ESP_COREDUMP_LOGE("Invalid memory segment size");
                return ESP_FAIL;
            }

            if (data_sz > 0) {
                mem_seg.start = start;
                mem_seg.size = esp_core_dump_get_memory_len(start, start + data_sz);;
                ESP_COREDUMP_LOG_PROCESS("Save user memory region %lu bytes @ %x",
                        mem_seg.size, mem_seg.start);
                err = esp_core_dump_save_mem_segment(write_cfg, &mem_seg);
                if (err != ESP_OK) {
                    ESP_COREDUMP_LOGE("Failed to save user memory region, error=%d!", err);
                    return err;
                }
            }
        }
    }

    // Write end
    if (write_cfg->end) {
        err = write_cfg->end(write_cfg->priv);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to end core dump error=%d!", err);
            return err;
        }
    }
    if (bad_tasks_num) {
        ESP_COREDUMP_LOGE("Found %d broken tasks!", bad_tasks_num);
    }
    return err;
}

#endif
