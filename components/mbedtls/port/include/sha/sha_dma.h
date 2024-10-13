/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/sha_types.h"

/** @brief Low-level support functions for the hardware SHA engine using DMA
 *
 * @note If you're looking for a SHA API to use, try mbedtls component
 * mbedtls/shaXX.h. That API supports hardware acceleration.
 *
 * The API in this header provides some building blocks for implementing a
 * full SHA API such as the one in mbedtls, and also a basic SHA function esp_sha().
 *
 * Some technical details about the hardware SHA engine:
 *
 * - The crypto DMA is shared between the SHA and AES engine, it is not
 *   possible for them to run calcalutions in parallel.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif


/** @brief Calculate SHA1 or SHA2 sum of some data, using hardware SHA engine
 *
 * @note For more versatile SHA calculations, where data doesn't need
 * to be passed all at once, try the mbedTLS mbedtls/shaX.h APIs.
 *
 * @note It is not necessary to lock any SHA hardware before calling
 * this function, thread safety is managed internally.
 *
 * @param sha_type SHA algorithm to use.
 *
 * @param input Input data buffer.
 *
 * @param ilen Length of input data in bytes.
 *
 * @param output Buffer for output SHA digest. Output is 20 bytes for
 * sha_type SHA1, 28 bytes for sha_type SHA2_224, 32 bytes for
 * sha_type SHA2_256, 48 bytes for sha_type SHA2_384, 64 bytes for
 * sha_type SHA2_512.
 */
void esp_sha(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output);

/** @brief Execute SHA block operation using DMA
 *
 * @note This is a piece of a SHA algorithm, rather than an entire SHA
 * algorithm.
 *
 * @note Call esp_sha_aquire_hardware() before calling this
 * function.
 *
 * @param sha_type SHA algorithm to use.
 *
 * @param input Pointer to the input data. Block size is
 * determined by algorithm (SHA1/SHA2_256 = 64 bytes,
 * SHA2_384/SHA2_512 = 128 bytes)
 *
 * @param ilen length of input data should be multiple of block length.
 *
 * @param buf Pointer to blocks of data that will be prepended
 * to data_block before hashing. Useful when there is two sources of
 * data that need to be efficiently calculated in a single SHA DMA
 * operation.
 *
 * @param buf_len length of buf data should be multiple of block length.
 * Should not be longer than the maximum amount of bytes in a single block
 * (128 bytes)
 *
 * @param is_first_block If this parameter is true, the SHA state will
 * be initialised (with the initial state of the given SHA algorithm)
 * before the block is calculated. If false, the existing state of the
 * SHA engine will be used.
 *
 * @param t The number of bits for the SHA512/t hash function, with
 * output truncated to t bits. Used for calculating the initial hash.
 * t is any positive integer between 1 and 512, except 384.
 *
 * @return 0 if successful
 */
int esp_sha_dma(esp_sha_type sha_type, const void *input, uint32_t ilen,
                const void *buf, uint32_t buf_len, bool is_first_block);

/**
 * @brief Read out the current state of the SHA digest
 *
 * @note This is a piece of a SHA algorithm, rather than an entire SHA algorithm.
 *
 * @note Call esp_sha_aquire_hardware() before calling this
 * function.
 *
 * If the SHA suffix padding block has been executed already, the
 * value that is read is the SHA digest.
 * Otherwise, the value that is read is an interim SHA state.
 *
 * @param sha_type SHA algorithm in use.
 * @param digest_state Pointer to a memory buffer to hold the SHA state. Size
 * is 20 bytes (SHA1), 32 bytes (SHA2_256), or 64 bytes (SHA2_384, SHA2_512).
 */
void esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state);

/**
 * @brief Set the current state of the SHA digest
 *
 * @note Call esp_sha_aquire_hardware() before calling this
 * function.
 *
 * When resuming a
 *
 * @param sha_type SHA algorithm in use.
 * @param digest_state
 */
void esp_sha_write_digest_state(esp_sha_type sha_type, void *digest_state);


/**
 * @brief Enables the SHA and crypto DMA peripheral and takes the
 * locks for both of them.
 */
void esp_sha_acquire_hardware(void);

/**
 * @brief Disables the SHA and crypto DMA peripheral and releases the
 * locks.
 */
void esp_sha_release_hardware(void);

/**
 * @brief Sets the initial hash value for SHA512/t.
 *
 * @note Is generated according to the algorithm described in the TRM,
 * chapter SHA-Accelerator
 *
 * @note The engine must be locked until the value is used for an operation
 * or read out. Else you risk another operation overwriting it.
 *
 * @param t
 *
 * @return 0 if successful
 */
int esp_sha_512_t_init_hash(uint16_t t);

#ifdef __cplusplus
}
#endif
