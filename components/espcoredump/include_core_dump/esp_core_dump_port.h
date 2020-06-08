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
#ifndef ESP_CORE_DUMP_PORT_H_
#define ESP_CORE_DUMP_PORT_H_

#include "freertos/FreeRTOS.h"
#if CONFIG_ESP32_COREDUMP_CHECKSUM_CRC32
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/crc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/crc.h"
#endif
#elif CONFIG_ESP32_COREDUMP_CHECKSUM_SHA256
#include "mbedtls/sha256.h"
#endif
#include "esp_core_dump_priv.h"
#include "soc/cpu.h"
#include "esp_debug_helpers.h"
#include "esp_app_format.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32
#define COREDUMP_VERSION_CHIP ESP_CHIP_ID_ESP32
#elif CONFIG_IDF_TARGET_ESP32S2
#define COREDUMP_VERSION_CHIP ESP_CHIP_ID_ESP32S2
#endif

#define COREDUMP_TCB_SIZE   sizeof(StaticTask_t)

// Gets RTOS tasks snapshot
uint32_t esp_core_dump_get_tasks_snapshot(core_dump_task_header_t** const tasks,
                        const uint32_t snapshot_size);

// Checks TCB consistency
bool esp_core_dump_tcb_addr_is_sane(uint32_t addr);
// Checks stack address
bool esp_core_dump_task_stack_end_is_sane(uint32_t sp);
bool esp_core_dump_mem_seg_is_sane(uint32_t addr, uint32_t sz);
void *esp_core_dump_get_current_task_handle(void);
bool esp_core_dump_check_task(panic_info_t *info, core_dump_task_header_t *task_snaphort, bool* is_current, bool* stack_is_valid);
bool esp_core_dump_check_stack(uint32_t stack_start, uint32_t stack_end);
uint32_t esp_core_dump_get_stack(core_dump_task_header_t* task_snapshot, uint32_t* stk_base, uint32_t* stk_len);

uint16_t esp_core_dump_get_arch_id(void);
uint32_t esp_core_dump_get_task_regs_dump(core_dump_task_header_t *task, void **reg_dump);
void esp_core_dump_init_extra_info(void);
uint32_t esp_core_dump_get_extra_info(void **info);

// Data integrity check functions
void esp_core_dump_checksum_init(core_dump_write_data_t* wr_data);
void esp_core_dump_checksum_update(core_dump_write_data_t* wr_data, void* data, size_t data_len);
size_t esp_core_dump_checksum_finish(core_dump_write_data_t* wr_data, void** chs_ptr);

#if CONFIG_ESP32_COREDUMP_CHECKSUM_SHA256
void esp_core_dump_print_sha256(const char* msg, const uint8_t* sha_output);
int esp_core_dump_sha(mbedtls_sha256_context *ctx,
        const unsigned char *input, size_t ilen, unsigned char output[32]);
#endif

#define esp_core_dump_in_isr_context() xPortInterruptedFromISRContext()
uint32_t esp_core_dump_get_isr_stack_end(void);

#if CONFIG_ESP32_CORE_DUMP_STACK_SIZE > 0
#if LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG
// increase stack size in verbose mode
#define ESP32_CORE_DUMP_STACK_SIZE (CONFIG_ESP32_CORE_DUMP_STACK_SIZE+100)
#else
#define ESP32_CORE_DUMP_STACK_SIZE CONFIG_ESP32_CORE_DUMP_STACK_SIZE
#endif
#endif

void esp_core_dump_report_stack_usage(void);

#if ESP32_CORE_DUMP_STACK_SIZE > 0
#define COREDUMP_STACK_FILL_BYTE	        (0xa5U)
extern uint8_t s_coredump_stack[];
extern uint8_t *s_core_dump_sp;

#if LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG
#define esp_core_dump_fill_stack() \
    memset(s_coredump_stack, COREDUMP_STACK_FILL_BYTE, ESP32_CORE_DUMP_STACK_SIZE)
#else
#define esp_core_dump_fill_stack()
#endif

#define esp_core_dump_setup_stack() \
{ \
    s_core_dump_sp = (uint8_t *)((uint32_t)(s_coredump_stack + ESP32_CORE_DUMP_STACK_SIZE - 1) & ~0xf); \
    esp_core_dump_fill_stack(); \
    /* watchpoint 1 can be used for task stack overflow detection, re-use it, it is no more necessary */ \
	esp_clear_watchpoint(1); \
	esp_set_watchpoint(1, s_coredump_stack, 1, ESP_WATCHPOINT_STORE); \
    asm volatile ("mov sp, %0" :: "r"(s_core_dump_sp)); \
    ESP_COREDUMP_LOGD("Use core dump stack @ 0x%x", get_sp()); \
}
#else
#define esp_core_dump_setup_stack() \
{ \
    /* if we are in ISR set watchpoint to the end of ISR stack */ \
    if (xPortInterruptedFromISRContext()) { \
        extern uint8_t port_IntStack; \
        esp_clear_watchpoint(1); \
        esp_set_watchpoint(1, &port_IntStack+xPortGetCoreID()*configISR_STACK_SIZE, 1, ESP_WATCHPOINT_STORE); \
    } else { \
        /* for tasks user should enable stack overflow detection in menuconfig
        TODO: if not enabled in menuconfig enable it ourselves */ \
    } \
}
#endif

#ifdef __cplusplus
}
#endif

#endif
