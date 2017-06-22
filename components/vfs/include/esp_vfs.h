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

#ifndef __ESP_VFS_H__
#define __ESP_VFS_H__

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include <sys/types.h>
#include <sys/reent.h>
#include <sys/stat.h>
#include <dirent.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Maximum length of path prefix (not including zero terminator)
 */
#define ESP_VFS_PATH_MAX 15

/**
 * Default value of flags member in esp_vfs_t structure.
 */
#define ESP_VFS_FLAG_DEFAULT        0

/**
 * Flag which indicates that FS needs extra context pointer in syscalls.
 */
#define ESP_VFS_FLAG_CONTEXT_PTR    1

/**
 * @brief VFS definition structure
 *
 * This structure should be filled with pointers to corresponding
 * FS driver functions.
 *
 * If the FS implementation has an option to use certain offset for
 * all file descriptors, this value should be passed into fd_offset
 * field. Otherwise VFS component will translate all FDs to start
 * at zero offset.
 *
 * Some FS implementations expect some state (e.g. pointer to some structure)
 * to be passed in as a first argument. For these implementations,
 * populate the members of this structure which have _p suffix, set
 * flags member to ESP_VFS_FLAG_CONTEXT_PTR and provide the context pointer
 * to esp_vfs_register function.
 * If the implementation doesn't use this extra argument, populate the
 * members without _p suffix and set flags member to ESP_VFS_FLAG_DEFAULT.
 *
 * If the FS driver doesn't provide some of the functions, set corresponding
 * members to NULL.
 */
typedef struct
{
    int fd_offset;  /*!< file descriptor offset, determined by the FS driver */
    int flags;      /*!< ESP_VFS_FLAG_CONTEXT_PTR or ESP_VFS_FLAG_DEFAULT */
    union {
        ssize_t (*write_p)(void* p, int fd, const void * data, size_t size);
        ssize_t (*write)(int fd, const void * data, size_t size);
    };
    union {
        off_t (*lseek_p)(void* p, int fd, off_t size, int mode);
        off_t (*lseek)(int fd, off_t size, int mode);
    };
    union {
        ssize_t (*read_p)(void* ctx, int fd, void * dst, size_t size);
        ssize_t (*read)(int fd, void * dst, size_t size);
    };
    union {
        int (*open_p)(void* ctx, const char * path, int flags, int mode);
        int (*open)(const char * path, int flags, int mode);
    };
    union {
        int (*close_p)(void* ctx, int fd);
        int (*close)(int fd);
    };
    union {
        int (*fstat_p)(void* ctx, int fd, struct stat * st);
        int (*fstat)(int fd, struct stat * st);
    };
    union {
        int (*stat_p)(void* ctx, const char * path, struct stat * st);
        int (*stat)(const char * path, struct stat * st);
    };
    union {
        int (*link_p)(void* ctx, const char* n1, const char* n2);
        int (*link)(const char* n1, const char* n2);
    };
    union {
        int (*unlink_p)(void* ctx, const char *path);
        int (*unlink)(const char *path);
    };
    union {
        int (*rename_p)(void* ctx, const char *src, const char *dst);
        int (*rename)(const char *src, const char *dst);
    };
    union {
        DIR* (*opendir_p)(void* ctx, const char* name);
        DIR* (*opendir)(const char* name);
    };
    union {
        struct dirent* (*readdir_p)(void* ctx, DIR* pdir);
        struct dirent* (*readdir)(DIR* pdir);
    };
    union {
        int (*readdir_r_p)(void* ctx, DIR* pdir, struct dirent* entry, struct dirent** out_dirent);
        int (*readdir_r)(DIR* pdir, struct dirent* entry, struct dirent** out_dirent);
    };
    union {
        long (*telldir_p)(void* ctx, DIR* pdir);
        long (*telldir)(DIR* pdir);
    };
    union {
        void (*seekdir_p)(void* ctx, DIR* pdir, long offset);
        void (*seekdir)(DIR* pdir, long offset);
    };
    union {
        int (*closedir_p)(void* ctx, DIR* pdir);
        int (*closedir)(DIR* pdir);
    };
    union {
        int (*mkdir_p)(void* ctx, const char* name, mode_t mode);
        int (*mkdir)(const char* name, mode_t mode);
    };
    union {
        int (*rmdir_p)(void* ctx, const char* name);
        int (*rmdir)(const char* name);
    };
} esp_vfs_t;


/**
 * Register a virtual filesystem for given path prefix.
 *
 * @param base_path  file path prefix associated with the filesystem.
 *                   Must be a zero-terminated C string, up to ESP_VFS_PATH_MAX
 *                   characters long, and at least 2 characters long.
 *                   Name must start with a "/" and must not end with "/".
 *                   For example, "/data" or "/dev/spi" are valid.
 *                   These VFSes would then be called to handle file paths such as
 *                   "/data/myfile.txt" or "/dev/spi/0".
 * @param vfs  Pointer to esp_vfs_t, a structure which maps syscalls to
 *             the filesystem driver functions. VFS component doesn't
 *             assume ownership of this pointer.
 * @param ctx  If vfs->flags has ESP_VFS_FLAG_CONTEXT_PTR set, a pointer
 *             which should be passed to VFS functions. Otherwise, NULL.
 *
 * @return  ESP_OK if successful, ESP_ERR_NO_MEM if too many VFSes are
 *          registered.
 */
esp_err_t esp_vfs_register(const char* base_path, const esp_vfs_t* vfs, void* ctx);


/**
 * Unregister a virtual filesystem for given path prefix
 *
 * @param base_path  file prefix previously used in esp_vfs_register call
 * @return ESP_OK if successful, ESP_ERR_INVALID_STATE if VFS for given prefix
 *         hasn't been registered
 */
esp_err_t esp_vfs_unregister(const char* base_path);

/**
 * These functions are to be used in newlib syscall table. They will be called by
 * newlib when it needs to use any of the syscalls.
 */
/**@{*/
ssize_t esp_vfs_write(struct _reent *r, int fd, const void * data, size_t size);
off_t esp_vfs_lseek(struct _reent *r, int fd, off_t size, int mode);
ssize_t esp_vfs_read(struct _reent *r, int fd, void * dst, size_t size);
int esp_vfs_open(struct _reent *r, const char * path, int flags, int mode);
int esp_vfs_close(struct _reent *r, int fd);
int esp_vfs_fstat(struct _reent *r, int fd, struct stat * st);
int esp_vfs_stat(struct _reent *r, const char * path, struct stat * st);
int esp_vfs_link(struct _reent *r, const char* n1, const char* n2);
int esp_vfs_unlink(struct _reent *r, const char *path);
int esp_vfs_rename(struct _reent *r, const char *src, const char *dst);
/**@}*/


#ifdef __cplusplus
} // extern "C"
#endif


#endif //__ESP_VFS_H__
