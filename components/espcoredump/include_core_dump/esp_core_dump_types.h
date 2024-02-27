/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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
#include "core_dump_checksum.h"

#if CONFIG_ESP_COREDUMP_LOGS
#define ESP_COREDUMP_LOG( level, format, ... )  if (LOG_LOCAL_LEVEL >= level)   { esp_rom_printf((format), esp_log_early_timestamp(), (const char *)TAG, ##__VA_ARGS__); }
#else
#define ESP_COREDUMP_LOG( level, format, ... )  // dummy define doing nothing
#endif

#define ESP_COREDUMP_LOGE( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_ERROR, LOG_FORMAT(E, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGW( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_WARN, LOG_FORMAT(W, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGI( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_INFO, LOG_FORMAT(I, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGD( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_DEBUG, LOG_FORMAT(D, format), ##__VA_ARGS__)
#define ESP_COREDUMP_LOGV( format, ... )  ESP_COREDUMP_LOG(ESP_LOG_VERBOSE, LOG_FORMAT(V, format), ##__VA_ARGS__)

/**
 * @brief Always print the given message, regardless of the log level
 */
#define ESP_COREDUMP_PRINT( format, ... ) do { esp_rom_printf((format), ##__VA_ARGS__); } while(0)

/**
 * @brief Assertion to be verified in a release context. Cannot be muted.
 */
#define ESP_COREDUMP_ASSERT( condition ) if(!(condition)){ abort(); } else { }

/**
 * @brief Assertion to be verified in a debug context. Can be muted.
 */
#define ESP_COREDUMP_DEBUG_ASSERT( condition ) assert(condition)

/**
 * @brief Logging should only be enabled if the core dump is not written to
 * the UART.
 */
#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH
#define ESP_COREDUMP_LOG_PROCESS( format, ... )  ESP_COREDUMP_LOGD(format, ##__VA_ARGS__)
#else
#define ESP_COREDUMP_LOG_PROCESS( format, ... )  do{/*(__VA_ARGS__);*/}while(0)
#endif

#define COREDUMP_MAX_TASK_STACK_SIZE        (64*1024)

/**
 * @brief The following macros defined below are used to create a version
 * numbering. This number is then used in the core dump header.
 */
#define COREDUMP_VERSION_MAKE(_maj_, _min_) ( \
                                                (((COREDUMP_VERSION_CHIP)&0xFFFF) << 16) | \
                                                (((_maj_)&0xFF) << 8) | \
                                                (((_min_)&0xFF) << 0) \
                                            )
#define COREDUMP_VERSION_BIN                0
#define COREDUMP_VERSION_ELF                1

/* legacy bin coredumps (before IDF v4.1) has version set to 1 */
#define COREDUMP_VERSION_BIN_LEGACY         COREDUMP_VERSION_MAKE(COREDUMP_VERSION_BIN, 1) // -> 0x0001
#define COREDUMP_VERSION_BIN_CURRENT        COREDUMP_VERSION_MAKE(COREDUMP_VERSION_BIN, 3) // -> 0x0003
#define COREDUMP_VERSION_ELF_CRC32          COREDUMP_VERSION_MAKE(COREDUMP_VERSION_ELF, 2) // -> 0x0102
#define COREDUMP_VERSION_ELF_SHA256         COREDUMP_VERSION_MAKE(COREDUMP_VERSION_ELF, 3) // -> 0x0103
#define COREDUMP_CURR_TASK_MARKER           0xDEADBEEF
#define COREDUMP_CURR_TASK_NOT_FOUND        -1

/**
 * @brief Macro defining the size of the cache used to write the core dump.
 */
#define COREDUMP_CACHE_SIZE 32

/**
 * @brief If the core dump has to be written to an encrypted flash, the
 * smallest data block we can write to it is 16 bytes long. Thus, this macro
 * MUST be a multiple of 16.
 */
#if (COREDUMP_CACHE_SIZE % 16) != 0
#error "Coredump cache size must be a multiple of 16"
#endif

typedef uint32_t core_dump_crc_t;

#if CONFIG_ESP_COREDUMP_CHECKSUM_CRC32

typedef struct {
    core_dump_crc_t crc;
    uint32_t total_bytes_checksum;  /* Number of bytes used to calculate the checksum */
} core_dump_crc_ctx_t;

typedef core_dump_crc_ctx_t checksum_ctx_t;

#else

#if CONFIG_IDF_TARGET_ESP32
#include "mbedtls/sha256.h" /* mbedtls_sha256_context */
typedef mbedtls_sha256_context sha256_ctx_t;
#else
#include "hal/sha_types.h"  /* SHA_CTX */
typedef SHA_CTX sha256_ctx_t;
#endif

#define COREDUMP_SHA256_LEN     32

typedef struct {
    sha256_ctx_t ctx;
    uint8_t result[COREDUMP_SHA256_LEN];
    uint32_t total_bytes_checksum;  /* Number of bytes used to calculate the checksum */
} core_dump_sha_ctx_t;

typedef core_dump_sha_ctx_t checksum_ctx_t;

#endif

/**
 * @brief Chip ID associated to this implementation.
 */
#define COREDUMP_VERSION_CHIP CONFIG_IDF_FIRMWARE_CHIP_ID

typedef struct _core_dump_write_data_t {
    uint32_t off; /*!< Current offset of data being written */
    uint8_t  cached_data[COREDUMP_CACHE_SIZE]; /*!< Cache used to write to flash */
    uint8_t  cached_bytes; /*!< Number of bytes filled in the cached */
    checksum_ctx_t checksum_ctx; /*!< Checksum context */
} core_dump_write_data_t;

/**
 * @brief Core dump data header
 * This header predecesses the actual core dump data (ELF or binary). */
typedef struct _core_dump_header_t {
    uint32_t data_len;  /*!< Data length */
    uint32_t version;   /*!< Core dump version */
    uint32_t tasks_num; /*!< Number of tasks */
    uint32_t tcb_sz;    /*!< Size of a TCB, in bytes */
    uint32_t mem_segs_num; /*!< Number of memory segments */
    uint32_t chip_rev; /*!< Chip revision */
} core_dump_header_t;

/**
 * @brief Core dump task data header
 * The main goal of this definition is to add typing to the code.
 */
typedef void* core_dump_task_handle_t;

/**
 * @brief Header for the tasks
 */
typedef struct _core_dump_task_header_t {
    core_dump_task_handle_t tcb_addr;    /*!< TCB address */
    uint32_t                stack_start; /*!< Start of the stack address */
    uint32_t                stack_end;   /*!< End of the stack address */
} core_dump_task_header_t;

/**
 * @brief Core dump memory segment header
 */
typedef struct _core_dump_mem_seg_header_t {
    uint32_t start; /*!< Memory region start address */
    uint32_t size;  /*!< Memory region size */
} core_dump_mem_seg_header_t;

#ifdef __cplusplus
}
#endif

#endif
