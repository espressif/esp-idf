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
#pragma once

#include "hal/sha_types.h"
#include "esp_types.h"

/** @brief Low-level support functions for the hardware SHA engine
 *
 * @note If you're looking for a SHA API to use, try mbedtls component
 * mbedtls/shaXX.h. That API supports hardware acceleration.
 *
 * The API in this header provides some building blocks for implementing a
 * full SHA API such as the one in mbedtls, and also a basic SHA function esp_sha().
 *
 * Some technical details about the hardware SHA engine:
 *
 * - SHA accelerator engine calculates one digest at a time, per SHA
 *   algorithm type. It initialises and maintains the digest state
 *   internally. It is possible to read out an in-progress SHA digest
 *   state, but it is not possible to restore a SHA digest state
 *   into the engine.
 *
 * - The memory block SHA_TEXT_BASE is shared between all SHA digest
 *   engines, so all engines must be idle before this memory block is
 *   modified.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif


/** @brief Calculate SHA1 or SHA2 sum of some data, using hardware SHA engine
 *
 * @note For more versatile SHA calculations, where data doesn't need
 * to be passed all at once, try the mbedTLS mbedtls/shaX.h APIs. The
 * hardware-accelerated mbedTLS implementation is also faster when
 * hashing large amounts of data.
 *
 * @note It is not necessary to lock any SHA hardware before calling
 * this function, thread safety is managed internally.
 *
 * @note If a TLS connection is open then this function may block
 * indefinitely waiting for a SHA engine to become available. Use the
 * mbedTLS SHA API to avoid this problem.
 *
 * @param sha_type SHA algorithm to use.
 *
 * @param input Input data buffer.
 *
 * @param ilen Length of input data in bytes.
 *
 * @param output Buffer for output SHA digest. Output is 20 bytes for
 * sha_type SHA1, 32 bytes for sha_type SHA2_256, 48 bytes for
 * sha_type SHA2_384, 64 bytes for sha_type SHA2_512.
 */
void esp_sha(esp_sha_type sha_type, const unsigned char *input, size_t ilen, unsigned char *output);

/* @brief Begin to execute a single SHA block operation
 *
 * @note This is a piece of a SHA algorithm, rather than an entire SHA
 * algorithm.
 *
 * @note Call esp_sha_try_lock_engine() before calling this
 * function. Do not call esp_sha_lock_memory_block() beforehand, this
 * is done inside the function.
 *
 * @param sha_type SHA algorithm to use.
 *
 * @param data_block Pointer to block of data. Block size is
 * determined by algorithm (SHA1/SHA2_256 = 64 bytes,
 * SHA2_384/SHA2_512 = 128 bytes)
 *
 * @param is_first_block If this parameter is true, the SHA state will
 * be initialised (with the initial state of the given SHA algorithm)
 * before the block is calculated. If false, the existing state of the
 * SHA engine will be used.
 *
 * @return As a performance optimisation, this function returns before
 * the SHA block operation is complete. Both this function and
 * esp_sha_read_state() will automatically wait for any previous
 * operation to complete before they begin. If using the SHA registers
 * directly in another way, call esp_sha_wait_idle() after calling this
 * function but before accessing the SHA registers.
 */
void esp_sha_block(esp_sha_type sha_type, const void *data_block, bool is_first_block);

/** @brief Read out the current state of the SHA digest loaded in the engine.
 *
 * @note This is a piece of a SHA algorithm, rather than an entire SHA algorithm.
 *
 * @note Call esp_sha_try_lock_engine() before calling this
 * function. Do not call esp_sha_lock_memory_block() beforehand, this
 * is done inside the function.
 *
 * If the SHA suffix padding block has been executed already, the
 * value that is read is the SHA digest (in big endian
 * format). Otherwise, the value that is read is an interim SHA state.
 *
 * @note If sha_type is SHA2_384, only 48 bytes of state will be read.
 * This is enough for the final SHA2_384 digest, but if you want the
 * interim SHA-384 state (to continue digesting) then pass SHA2_512 instead.
 *
 * @param sha_type SHA algorithm in use.
 *
 * @param state Pointer to a memory buffer to hold the SHA state. Size
 * is 20 bytes (SHA1), 32 bytes (SHA2_256), 48 bytes (SHA2_384) or 64 bytes (SHA2_512).
 *
 */
void esp_sha_read_digest_state(esp_sha_type sha_type, void *digest_state);

/**
 * @brief Obtain exclusive access to a particular SHA engine
 *
 * @param sha_type Type of SHA engine to use.
 *
 * Blocks until engine is available. Note: Can block indefinitely
 * while a TLS connection is open, suggest using
 * esp_sha_try_lock_engine() and failing over to software SHA.
 */
void esp_sha_lock_engine(esp_sha_type sha_type);

/**
 * @brief Try and obtain exclusive access to a particular SHA engine
 *
 * @param sha_type Type of SHA engine to use.
 *
 * @return Returns true if the SHA engine is locked for exclusive
 * use. Call esp_sha_unlock_sha_engine() when done.  Returns false if
 * the SHA engine is already in use, caller should use software SHA
 * algorithm for this digest.
 */
bool esp_sha_try_lock_engine(esp_sha_type sha_type);

/**
 * @brief Unlock an engine previously locked with esp_sha_lock_engine() or esp_sha_try_lock_engine()
 *
 * @param sha_type Type of engine to release.
 */
void esp_sha_unlock_engine(esp_sha_type sha_type);

/**
 * @brief Acquire exclusive access to the SHA shared memory block at SHA_TEXT_BASE
 *
 * This memory block is shared across all the SHA algorithm types.
 *
 * Caller should have already locked a SHA engine before calling this function.
 *
 * Note that it is possible to obtain exclusive access to the memory block even
 * while it is in use by the SHA engine. Caller should use esp_sha_wait_idle()
 * to ensure the SHA engine is not reading from the memory block in hardware.
 *
 * @note This function enters a critical section. Do not block while holding this lock.
 *
 * @note You do not need to lock the memory block before calling esp_sha_block() or esp_sha_read_digest_state(), these functions handle memory block locking internally.
 *
 * Call esp_sha_unlock_memory_block() when done.
 */
void esp_sha_lock_memory_block(void);

/**
 * @brief Release exclusive access to the SHA register memory block at SHA_TEXT_BASE
 *
 * Caller should have already locked a SHA engine before calling this function.
 *
 * This function releases the critical section entered by esp_sha_lock_memory_block().
 *
 * Call following esp_sha_lock_memory_block().
 */
void esp_sha_unlock_memory_block(void);

/** @brief Wait for the SHA engine to finish any current operation
 *
 * @note This function does not ensure exclusive access to any SHA
 * engine. Caller should use esp_sha_try_lock_engine() and
 * esp_sha_lock_memory_block() as required.
 *
 * @note Functions declared in this header file wait for SHA engine
 * completion automatically, so you don't need to use this API for
 * these. However if accessing SHA registers directly, you will need
 * to call this before accessing SHA registers if using the
 * esp_sha_block() function.
 *
 * @note This function busy-waits, so wastes CPU resources.
 * Best to delay calling until you are about to need it.
 *
 */
void esp_sha_wait_idle(void);

#ifdef __cplusplus
}
#endif


