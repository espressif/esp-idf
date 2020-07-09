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
#ifndef ESP_CORE_DUMP_PRIV_H_
#define ESP_CORE_DUMP_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "sdkconfig.h"
#include "esp_private/panic_internal.h"
#if CONFIG_ESP_COREDUMP_CHECKSUM_SHA256
// TODO: move this to portable part of the code
#include "mbedtls/sha256.h"
#endif

#define ESP_COREDUMP_LOG( level, format, ... )  if (LOG_LOCAL_LEVEL >= level)   { esp_rom_printf(DRAM_STR(format), esp_log_early_timestamp(), (const char *)TAG, ##__VA_ARGS__); }
#define ESP_COREDUMP_LOGE( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_ERROR, LOG_FORMAT(E, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGW( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_WARN, LOG_FORMAT(W, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGI( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_INFO, LOG_FORMAT(I, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGD( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_DEBUG, LOG_FORMAT(D, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGV( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_VERBOSE, LOG_FORMAT(V, format), ##__VA_ARGS__)

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH
#define ESP_COREDUMP_LOG_PROCESS( format, ... )  ESP_COREDUMP_LOGD(format, ##__VA_ARGS__)
#else
#define ESP_COREDUMP_LOG_PROCESS( format, ... )  do{/*(__VA_ARGS__);*/}while(0)
#endif

#define COREDUMP_MAX_TASK_STACK_SIZE        (64*1024)
// COREDUMP_VERSION_CHIP is defined in ports
#define COREDUMP_VERSION_MAKE(_maj_, _min_)    ((((COREDUMP_VERSION_CHIP)&0xFFFF) << 16) | (((_maj_)&0xFF) << 8) | (((_min_)&0xFF) << 0))
#define COREDUMP_VERSION_BIN                0
#define COREDUMP_VERSION_ELF                1
// legacy bin coredumps (before IDF v4.1) has version set to 1
#define COREDUMP_VERSION_BIN_LEGACY         COREDUMP_VERSION_MAKE(COREDUMP_VERSION_BIN, 1) // -> 0x0001
#define COREDUMP_VERSION_BIN_CURRENT        COREDUMP_VERSION_MAKE(COREDUMP_VERSION_BIN, 2) // -> 0x0002
#define COREDUMP_VERSION_ELF_CRC32          COREDUMP_VERSION_MAKE(COREDUMP_VERSION_ELF, 0) // -> 0x0100
#define COREDUMP_VERSION_ELF_SHA256         COREDUMP_VERSION_MAKE(COREDUMP_VERSION_ELF, 1) // -> 0x0101
#define COREDUMP_CURR_TASK_MARKER           0xDEADBEEF
#define COREDUMP_CURR_TASK_NOT_FOUND        -1

#if CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
#if CONFIG_ESP_COREDUMP_CHECKSUM_CRC32
#define COREDUMP_VERSION                    COREDUMP_VERSION_ELF_CRC32
#elif CONFIG_ESP_COREDUMP_CHECKSUM_SHA256
#define COREDUMP_VERSION                    COREDUMP_VERSION_ELF_SHA256
#define COREDUMP_SHA256_LEN                 32
#endif
#else
#define COREDUMP_VERSION                    COREDUMP_VERSION_BIN_CURRENT
#endif

#define COREDUMP_CHECKSUM_MAX_LEN           32

typedef esp_err_t (*esp_core_dump_write_prepare_t)(void *priv, uint32_t *data_len);
typedef esp_err_t (*esp_core_dump_write_start_t)(void *priv);
typedef esp_err_t (*esp_core_dump_write_end_t)(void *priv);
typedef esp_err_t (*esp_core_dump_flash_write_data_t)(void *priv, void * data, uint32_t data_len);

typedef uint32_t core_dump_crc_t;

/**
 * The following macro defines the size of the cache used to write the coredump
 * to the flash. When the flash is encrypted, the smallest data block we can
 * write to it is 16 bytes long. Thus, this macro MUST be a multiple of 16.
 */
#define COREDUMP_CACHE_SIZE 32

#if (COREDUMP_CACHE_SIZE % 16) != 0
    #error "Coredump cache size must be a multiple of 16"
#endif

typedef struct _core_dump_write_data_t
{
    // TODO: move flash related data to flash-specific code
    uint32_t        off; // current offset in partition
    uint8_t         cached_data[COREDUMP_CACHE_SIZE];
    uint8_t         cached_bytes;
#if CONFIG_ESP_COREDUMP_CHECKSUM_SHA256
    // TODO: move this to portable part of the code
    mbedtls_sha256_context  ctx;
    char            sha_output[COREDUMP_SHA256_LEN];
#elif CONFIG_ESP_COREDUMP_CHECKSUM_CRC32
    core_dump_crc_t crc; // CRC of dumped data
#endif
} core_dump_write_data_t;

// core dump emitter control structure
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
    uint32_t mem_segs_num; // number of memory segments
} core_dump_header_t;

/** core dump task data header */
typedef struct _core_dump_task_header_t
{
    void*    tcb_addr;    // TCB address
    uint32_t stack_start; // stack start address
    uint32_t stack_end;   // stack end address
} core_dump_task_header_t;

/** core dump memory segment header */
typedef struct _core_dump_mem_seg_header_t
{
    uint32_t start; // memory region start address
    uint32_t size;  // memory region size
} core_dump_mem_seg_header_t;

//  Core dump flash init function
void esp_core_dump_flash_init(void);

// Common core dump write function
void esp_core_dump_write(panic_info_t *info, core_dump_write_config_t *write_cfg);

#include "esp_core_dump_port.h"

#ifdef __cplusplus
}
#endif

#endif
