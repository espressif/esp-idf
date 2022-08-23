/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * This header file provides POSIX-compatible definitions of directory
 * access data types. Starting with newlib 3.3, related functions are defined
 * in 'dirent.h' bundled with newlib.
 * See http://pubs.opengroup.org/onlinepubs/7908799/xsh/dirent.h.html
 * for reference.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque directory structure
 */
typedef struct {
    uint16_t dd_vfs_idx; /*!< VFS index, not to be used by applications */
    uint16_t dd_rsv;     /*!< field reserved for future extension */
    /* remaining fields are defined by VFS implementation */
} DIR;

/**
 * @brief Directory entry structure
 */
struct dirent {
    ino_t d_ino;          /*!< file number */
    uint8_t d_type;     /*!< not defined in POSIX, but present in BSD and Linux */
#define DT_UNKNOWN  0
#define DT_REG      1
#define DT_DIR      2
#if __BSD_VISIBLE
#define MAXNAMLEN 255
    char d_name[MAXNAMLEN+1];   /*!< zero-terminated file name */
#else
    char d_name[256];
#endif
};

DIR* opendir(const char* name);
struct dirent* readdir(DIR* pdir);
long telldir(DIR* pdir);
void seekdir(DIR* pdir, long loc);
void rewinddir(DIR* pdir);
int closedir(DIR* pdir);
int readdir_r(DIR* pdir, struct dirent* entry, struct dirent** out_dirent);

#ifdef __cplusplus
}
#endif
