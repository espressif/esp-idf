/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * This header file provides POSIX-compatible definitions of directory access data types.
 * The standard dirent.h cannot be used directly because we have a custom version defined in newlib component,
 * which is used by VFS. Accessing the VFS index (definition of struct DIR) requires this custom dirent.h.
 * This file is a copy of dirent.h from newlib.
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
 *
 * Fields match the glibc layout so that this header is binary-compatible with
 * glibc's readdir() when CONFIG_VFS_SUPPORT_IO is disabled on Linux (in which
 * case glibc's readdir() fills the struct, not the VFS wrapper).
 */
struct dirent {
    ino_t d_ino;              /*!< file number */
    off_t d_off;              /*!< implementation-defined offset (glibc extension) */
    unsigned short d_reclen;  /*!< length of this record (glibc extension) */
    uint8_t d_type;           /*!< not defined in POSIX, but present in BSD and Linux */
#define DT_UNKNOWN  0
#define DT_REG      1
#define DT_DIR      2
#if __BSD_VISIBLE
#define MAXNAMLEN 255
    char d_name[MAXNAMLEN + 1]; /*!< zero-terminated file name */
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
int scandir(const char *dirname, struct dirent ***out_dirlist,
            int (*select_func)(const struct dirent *),
            int (*cmp_func)(const struct dirent **, const struct dirent **));
int alphasort(const struct dirent **d1, const struct dirent **d2);

#ifdef __cplusplus
}
#endif
