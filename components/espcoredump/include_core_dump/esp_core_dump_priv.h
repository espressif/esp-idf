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
#ifndef ESP_CORE_DUMP_H_
#define ESP_CORE_DUMP_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"

#define ESP_COREDUMP_LOG( level, format, ... )  if (LOG_LOCAL_LEVEL >= level)   { ets_printf(DRAM_STR(format), esp_log_early_timestamp(), (const char *)TAG, ##__VA_ARGS__); }
#define ESP_COREDUMP_LOGE( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_ERROR, LOG_FORMAT(E, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGW( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_WARN, LOG_FORMAT(W, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGI( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_INFO, LOG_FORMAT(I, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGD( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_DEBUG, LOG_FORMAT(D, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGV( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_VERBOSE, LOG_FORMAT(V, format), ##__VA_ARGS__)

#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH
#define ESP_COREDUMP_LOG_PROCESS( format, ... )  ESP_COREDUMP_LOGD(format, ##__VA_ARGS__)
#else
#define ESP_COREDUMP_LOG_PROCESS( format, ... )  do{/*(__VA_ARGS__);*/}while(0)
#endif

#define COREDUMP_MAX_TASK_STACK_SIZE        (64*1024)
#define COREDUMP_VERSION                    1

typedef uint32_t core_dump_crc_t;

#if CONFIG_ESP32_ENABLE_COREDUMP

typedef esp_err_t (*esp_core_dump_write_prepare_t)(void *priv, uint32_t *data_len);
typedef esp_err_t (*esp_core_dump_write_start_t)(void *priv);
typedef esp_err_t (*esp_core_dump_write_end_t)(void *priv);
typedef esp_err_t (*esp_core_dump_flash_write_data_t)(void *priv, void * data, uint32_t data_len);

/** core dump emitter control structure */
typedef struct _core_dump_write_config_t
{
    // this function is called before core dump data writing
    // used for sanity checks
    esp_core_dump_write_prepare_t       prepare;
    // this function is called at the beginning of data writing
    esp_core_dump_write_start_t         start;
    // this function is called when all dump data are written
    esp_core_dump_write_end_t           end;
    // this function is called to write data chunk
    esp_core_dump_flash_write_data_t    write;
    // number of tasks with corrupted TCBs
    uint32_t                            bad_tasks_num;
    // pointer to data which are specific for particular core dump emitter
    void *                              priv;
} core_dump_write_config_t;

/** core dump data header */
typedef struct _core_dump_header_t
{
    uint32_t data_len;  // data length
    uint32_t version;   // core dump struct version
    uint32_t tasks_num; // number of tasks
    uint32_t tcb_sz;    // size of TCB
} core_dump_header_t;

/** core dump task data header */
typedef struct _core_dump_task_header_t
{
    void *   tcb_addr;    // TCB address
    uint32_t stack_start; // stack start address
    uint32_t stack_end;   // stack end address
} core_dump_task_header_t;

#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH

//  Core dump flash init function
void esp_core_dump_flash_init();

#endif

// Common core dump write function
void esp_core_dump_write(void *frame, core_dump_write_config_t *write_cfg);

// Gets RTOS tasks snapshot
uint32_t esp_core_dump_get_tasks_snapshot(core_dump_task_header_t* const tasks,
                        const uint32_t snapshot_size, uint32_t* const tcb_sz);

// Checks TCB consistency
bool esp_tcb_addr_is_sane(uint32_t addr, uint32_t sz);

bool esp_core_dump_process_tcb(void *frame, core_dump_task_header_t *task_snaphort, uint32_t tcb_sz);

bool esp_core_dump_process_stack(core_dump_task_header_t* task_snaphort, uint32_t *length);

#endif

#endif
