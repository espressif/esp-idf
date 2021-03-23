// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


/**
 * Archive to parse cpio data in the newc and crc formats. Generate a cpio archive like that by e.g.
 * find . | cpio -o -H newc > archive.cpio
 */

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CPIO_MODE_FILETYPE_MASK     0xF000
#define CPIO_MODE_FILETYPE_SOCKET   0xC000
#define CPIO_MODE_FILETYPE_SYMLINK  0xA000
#define CPIO_MODE_FILETYPE_REGULAR  0x8000
#define CPIO_MODE_FILETYPE_BLOCKDEV 0x6000
#define CPIO_MODE_FILETYPE_DIR      0x4000
#define CPIO_MODE_FILETYPE_CHARDEV  0x2000
#define CPIO_MODE_FILETYPE_FIFO     0x1000
#define CPIO_MODE_SUID              0x0800
#define CPIO_MODE_SGID              0x0400
#define CPIO_MODE_STICKY            0x0200

typedef struct {
    size_t filesize;
    char *name;
    uint32_t mode;
    uint32_t check;
} cpio_file_t;

typedef enum {
    CPIO_RET_MORE = 0,
    CPIO_RET_DONE,
    CPIO_RET_ERR
} cpio_ret_t;

typedef struct cpio_handle_data_t cpio_handle_data_t;
typedef cpio_handle_data_t *cpio_handle_t;

typedef enum {
    CPIO_RSN_FILE_ALL = 0,
    CPIO_RSN_FILE_INITIAL,
    CPIO_RSN_FILE_MORE,
    CPIO_RSN_FILE_END
} cpio_callback_reason_t;


/**
 * Callback for cpio file data.
 *
 * This callback will be called by the library to indicate data for a file is available.
 *
 * For files in the cpio archive that fit entirely in the internal buffer, or when no internal
 * buffer is available, are entirely contained in the buffer fed to cpio_feed(), this callback
 * is only called once, with reason=CPIO_RNS_FILE_ALL. fileinfo will contain the information
 * for that specific file (name, size, ...), buff_offset will be 0, buff_len is the file
 * size and buff contains all the information for the file.
 *
 * For files that do not fit in the buffer, this callback will be called multiple times.
 * The initial time with reason=CPIO_RSN_FILE_INITIAL, when more data is available with
 * CPIO_RSN_FILE_MORE and finally with CPIO_RSN_FILE_END. For these calls, fileinfo
 * will again contain file information. buff will be the information contained in the
 * file at offset buff_offset, and the lenght of this buffer will be in buff_len.
 *
 * The library guarantees to feed all file data to the callback consequitively, so
 * within the same file, the buff_offset from a call will always be (buff_offset+buff_len)
 * from the call before that. If cpio_start is
 *
 * The library also guarantees every file in the cpio archive will either generate a single
 * callback call with CPIO_RSN_ALL, or multiple with in sequence CPIO_RSN_FILE_INITIAL, 0 or
 * more CPIO_RSN_FILE_MORE and finally a CPIO_RSN_FILE_END.
 *
 * When a non-zero buffer size is passed to cpio_start, the library guarantees that all callback
 * calls with a reason of CPIO_RSN_FILE_INITIAL and CPIO_RSN_FILE_MORE will have a buffer
 * filled with exactly this amount of bytes.
 *
 */
typedef void (*cpio_callback_t)(cpio_callback_reason_t reason, cpio_file_t *fileinfo, size_t buff_offset, size_t buff_len, char *buff, void *arg);


/**
 * @brief      Initialize a cpio handle.
 *
 * Call this to start parsing a cpio archive. You can set the callback that handles the
 * files/data here.
 *
 * @param  callback The callback that will handle the data of the files inside the cpio archive
 *
 * @param  cbarg User-supplied argument. The callback will be called with this as an argument.
 *
 * @param  buflen Length of internal buffer used.
 *                If this is zero, the callback will be called with data that lives in the data buffer
 *                supplied to the cpio library by whomever called cpio_feed(). Because this library has
 *                no power over that buffer, the callback can be passed as little as 1 and as many as
 *                INT_MAX bytes at a time.
 *                If this is non-zero, the library will allocate an internal buffer of this size. All
 *                cpio_feed()-calls will be rebuffered, and the callback is guaranteed to only be called
 *                with this many bytes in the buffer, given there's enough data in the file to fill it.
 *
 * @param memchunk Chunk of memory to allocate everything (handle, I/O buffer, filename buffer) in. Minimum size
 *                 (estimate) is 160+buflen+sizeof(largest filename/path).
 * @param memchunklen Size of the mem chunk
 *
 * @return
 *     - Success: A pointer to a cpio handle
 *     - Error: NULL
 *
 */
cpio_handle_t cpio_start(cpio_callback_t callback, void *cbarg, size_t buflen, void *memchunk, int memchunklen);

/**
 * @brief      Feed data from a cpio archive into the library
 *
 * This routine is used to feed consecutive data of the cpio archive into the library. While processing,
 * the library can call the callback function one or more times if needed.
 *
 * @param  cpio Handle obtained by calling cpio_start()
 *
 * @param  buffer Pointer to buffer containing cpio archive data
 *
 * @param  len Length of the buffer, in bytes
 *
 * @return
 *     - CPIO_RET_MORE: CPIO archive isn't done yet, please feed more data.
 *     - CPIO_RET_DONE: CPUI archive is finished.
 *     - CPIO_RET_ERR: Invalid CPIO archive data; decoding aborted.
 *
 */
cpio_ret_t cpio_feed(cpio_handle_t cpio, char *buffer, int len);

/**
 * @brief      Indicate there is no more cpio data to be fed into the archive
 *
 * This call is to be called when the source data is exhausted. Normally, the library can find the end of the
 * cpio archive by looking for the end marker,
 *
 * @param  timer_conf Pointer of LEDC timer configure struct
 *
 *
 * @return
 *     - CPIO_RET_DONE on success
 *     - CPIO_RET_ERR when cpio archive is invalid
 *
 */
cpio_ret_t cpio_done(cpio_handle_t cpio);


/**
 * @brief      Free the memory allocated for a cpio handle.
 *
 * @param  cpio Handle obtained by calling cpio_start()
 *
 * @return
 *     - CPIO_RET_DONE on success
 *
 */
cpio_ret_t cpio_destroy(cpio_handle_t cpio);

#ifdef __cplusplus
}
#endif
