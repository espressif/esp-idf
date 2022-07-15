/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Core dump checksum interface.
 *
 * This file contains all the functions required by the core dump component to
 * calculate checksums for data to write (or already written) on the flash.
 * Currently, both CRC32 and SHA256 are supported, but this interface is
 * implementation independent.
 */

#ifndef CORE_DUMP_CHECKSUM_H_
#define CORE_DUMP_CHECKSUM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum possible length for a checksum (regardless of the
 * implentation). This can be modified in the future if a new implementation
 * requires a larger size.
 */
#define COREDUMP_CHECKSUM_MAX_LEN 32

/**
 * @brief Type describing a checksum context. It is an abstract type as it is
 * implementation independent, it is defined in the C source counterpart.
 */
typedef struct core_dump_checksum_ctx core_dump_checksum_ctx;

/**
 * @brief Type returned by `esp_core_dump_checksum_finish()`. It describes a
 * checksum as an array of bytes. It can also be provided to `esp_core_dump_print_checksum()`.
 */
typedef uint8_t* core_dump_checksum_bytes;


/**
 * @brief Get ELF core dump version.
 *
 * @note Currently, this is used in the core dump header to recognize the
 * checksum used for a certain dump, as the version varies with the checksum.
 *
 * @return Version of the core dump used.
 */
uint32_t esp_core_dump_elf_version(void);

/**
 * @brief Initialize checksum calculation for the given context.
 *
 * @param wr_data Core dump checksum context to fill.
 */
void esp_core_dump_checksum_init(core_dump_checksum_ctx** wr_data);

/**
 * @brief Update checksum calculation by integrating the given data in the context.
 *
 * @param wr_data Core dump checksum context.
 * @param data    Pointer to the data to integrate in the checksum calculation.
 *                This is usually the new data to write (or already written) on
 *                the flash.
 */
void esp_core_dump_checksum_update(core_dump_checksum_ctx* wr_data, void* data, size_t data_len);

/**
 * @brief Terminate and return checksum calculated for the given context.
 *
 * @param wr_data Core dump checksum context. It can be NULL only if chs_ptr is
 *                also NULL.
 * @param chs_ptr Pointer used to return the checksum calculated. It can be
 *                NULL, in this case, it will be ignored but the correct size
 *                of the checksum will be returned.
 *
 * @return The size, in bytes, of the checksum.
 */
uint32_t esp_core_dump_checksum_finish(core_dump_checksum_ctx* wr_data, core_dump_checksum_bytes* chs_ptr);

/**
 * @brief Return the size of the checksums.
 *
 * @note This is equivalent to `esp_core_dump_checksum_finish(NULL, NULL)`.
 *
 * @return The size, in bytes, of the checksums.
 */
uint32_t esp_core_dump_checksum_size(void);

/**
 * @brief Print a message followed by the checksum given as a parameter.
 *
 * @note The checksum will be printed in hex format and followed by \r\n.
 *
 * @param msg      Message to print before the checksum. Can be NULL.
 * @param checksum Checksum to print. Must not be NULL.
 */
void esp_core_dump_print_checksum(const char* msg, core_dump_checksum_bytes checksum);

#ifdef __cplusplus
}
#endif

#endif
