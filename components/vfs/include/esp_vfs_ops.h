/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <unistd.h>
#include <utime.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include <sys/types.h>
#include <sys/reent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/termios.h>
#include <sys/poll.h>
#ifdef __clang__ // TODO LLVM-330
#include <sys/dirent.h>
#else
#include <dirent.h>
#endif
#include <string.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SYS_TYPES_FD_SET
#error "VFS should be used with FD_SETSIZE and FD_SET from sys/types.h"
#endif

/*
 * @brief VFS identificator used for esp_vfs_register_with_id()
 */
typedef int esp_vfs_id_t;

/**
 * @brief VFS semaphore type for select()
 *
 */
typedef struct
{
    bool is_sem_local;      /*!< type of "sem" is SemaphoreHandle_t when true, defined by socket driver otherwise */
    void *sem;              /*!< semaphore instance */
} esp_vfs_select_sem_t;

#ifdef CONFIG_VFS_SUPPORT_DIR

/**
 * @brief Struct containing function pointers to directory related functionality.
 *
 */
typedef struct {
    union {
        int (*stat_p)(void* ctx, const char * path, struct stat * st);                               /*!< stat with context pointer */
        int (*stat)(const char * path, struct stat * st);                                            /*!< stat without context pointer */
    };
    union {
        int (*link_p)(void* ctx, const char* n1, const char* n2);                                    /*!< link with context pointer */
        int (*link)(const char* n1, const char* n2);                                                 /*!< link without context pointer */
    };
    union {
        int (*unlink_p)(void* ctx, const char *path);                                                /*!< unlink with context pointer */
        int (*unlink)(const char *path);                                                             /*!< unlink without context pointer */
    };
    union {
        int (*rename_p)(void* ctx, const char *src, const char *dst);                               /*!< rename with context pointer */
        int (*rename)(const char *src, const char *dst);                                            /*!< rename without context pointer */
    };
    union {
        DIR* (*opendir_p)(void* ctx, const char* name);                                             /*!< opendir with context pointer */
        DIR* (*opendir)(const char* name);                                                          /*!< opendir without context pointer */
    };
    union {
        struct dirent* (*readdir_p)(void* ctx, DIR* pdir);                                          /*!< readdir with context pointer */
        struct dirent* (*readdir)(DIR* pdir);                                                       /*!< readdir without context pointer */
    };
    union {
        int (*readdir_r_p)(void* ctx, DIR* pdir, struct dirent* entry, struct dirent** out_dirent); /*!< readdir_r with context pointer */
        int (*readdir_r)(DIR* pdir, struct dirent* entry, struct dirent** out_dirent);              /*!< readdir_r without context pointer */
    };
    union {
        long (*telldir_p)(void* ctx, DIR* pdir);                                                    /*!< telldir with context pointer */
        long (*telldir)(DIR* pdir);                                                                 /*!< telldir without context pointer */
    };
    union {
        void (*seekdir_p)(void* ctx, DIR* pdir, long offset);                                       /*!< seekdir with context pointer */
        void (*seekdir)(DIR* pdir, long offset);                                                    /*!< seekdir without context pointer */
    };
    union {
        int (*closedir_p)(void* ctx, DIR* pdir);                                                    /*!< closedir with context pointer */
        int (*closedir)(DIR* pdir);                                                                 /*!< closedir without context pointer */
    };
    union {
        int (*mkdir_p)(void* ctx, const char* name, mode_t mode);                                   /*!< mkdir with context pointer */
        int (*mkdir)(const char* name, mode_t mode);                                                /*!< mkdir without context pointer */
    };
    union {
        int (*rmdir_p)(void* ctx, const char* name);                                                /*!< rmdir with context pointer */
        int (*rmdir)(const char* name);                                                             /*!< rmdir without context pointer */
    };
    union {
        int (*access_p)(void* ctx, const char *path, int amode);                                    /*!< access with context pointer */
        int (*access)(const char *path, int amode);                                                 /*!< access without context pointer */
    };
    union {
        int (*truncate_p)(void* ctx, const char *path, off_t length);                               /*!< truncate with context pointer */
        int (*truncate)(const char *path, off_t length);                                            /*!< truncate without context pointer */
    };
    union {
        int (*ftruncate_p)(void* ctx, int fd, off_t length);                                        /*!< ftruncate with context pointer */
        int (*ftruncate)(int fd, off_t length);                                                     /*!< ftruncate without context pointer */
    };
    union {
        int (*utime_p)(void* ctx, const char *path, const struct utimbuf *times);                   /*!< utime with context pointer */
        int (*utime)(const char *path, const struct utimbuf *times);                                /*!< utime without context pointer */
    };
} esp_vfs_dir_ops_t;

#endif // CONFIG_VFS_SUPPORT_DIR

#ifdef CONFIG_VFS_SUPPORT_TERMIOS

/**
 * @brief Struct containing function pointers to termios related functionality.
 *
 */
typedef struct {
    union {
        int (*tcsetattr_p)(void *ctx, int fd, int optional_actions, const struct termios *p);       /*!< tcsetattr with context pointer */
        int (*tcsetattr)(int fd, int optional_actions, const struct termios *p);                    /*!< tcsetattr without context pointer */
    };
    union {
        int (*tcgetattr_p)(void *ctx, int fd, struct termios *p);                                   /*!< tcgetattr with context pointer */
        int (*tcgetattr)(int fd, struct termios *p);                                                /*!< tcgetattr without context pointer */
    };
    union {
        int (*tcdrain_p)(void *ctx, int fd);                                                        /*!< tcdrain with context pointer */
        int (*tcdrain)(int fd);                                                                     /*!< tcdrain without context pointer */
    };
    union {
        int (*tcflush_p)(void *ctx, int fd, int select);                                            /*!< tcflush with context pointer */
        int (*tcflush)(int fd, int select);                                                         /*!< tcflush without context pointer */
    };
    union {
        int (*tcflow_p)(void *ctx, int fd, int action);                                             /*!< tcflow with context pointer */
        int (*tcflow)(int fd, int action);                                                          /*!< tcflow without context pointer */
    };
    union {
        pid_t (*tcgetsid_p)(void *ctx, int fd);                                                     /*!< tcgetsid with context pointer */
        pid_t (*tcgetsid)(int fd);                                                                  /*!< tcgetsid without context pointer */
    };
    union {
        int (*tcsendbreak_p)(void *ctx, int fd, int duration);                                      /*!< tcsendbreak with context pointer */
        int (*tcsendbreak)(int fd, int duration);                                                   /*!< tcsendbreak without context pointer */
    };
} esp_vfs_termios_ops_t;

#endif // CONFIG_VFS_SUPPORT_TERMIOS

#ifdef CONFIG_VFS_SUPPORT_SELECT

/**
 * @brief Struct containing function pointers to select related functionality.
 *
 */
typedef struct {
    /** start_select is called for setting up synchronous I/O multiplexing of the desired file descriptors in the given VFS */
    esp_err_t (*start_select)(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, esp_vfs_select_sem_t sem, void **end_select_args);

    /** socket select function for socket FDs with the functionality of POSIX select(); this should be set only for the socket VFS */
    int (*socket_select)(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout);

    /** called by VFS to interrupt the socket_select call when select is activated from a non-socket VFS driver; set only for the socket driver */
    void (*stop_socket_select)(void *sem);

    /** stop_socket_select which can be called from ISR; set only for the socket driver */
    void (*stop_socket_select_isr)(void *sem, BaseType_t *woken);

    /** end_select is called to stop the I/O multiplexing and deinitialize the environment created by start_select for the given VFS */
    void* (*get_socket_select_semaphore)(void);

    /** get_socket_select_semaphore returns semaphore allocated in the socket driver; set only for the socket driver */
    esp_err_t (*end_select)(void *end_select_args);
} esp_vfs_select_ops_t;

#endif // CONFIG_VFS_SUPPORT_SELECT

/**
 * @brief Main struct of the minified vfs API, containing basic function pointers as well as pointers to the other subcomponents.
 *
 */
typedef struct {
    union {
        ssize_t (*write_p)(void* p, int fd, const void * data, size_t size);                         /*!< Write with context pointer */
        ssize_t (*write)(int fd, const void * data, size_t size);                                    /*!< Write without context pointer */
    };
    union {
        off_t (*lseek_p)(void* p, int fd, off_t size, int mode);                                     /*!< Seek with context pointer */
        off_t (*lseek)(int fd, off_t size, int mode);                                                /*!< Seek without context pointer */
    };
    union {
        ssize_t (*read_p)(void* ctx, int fd, void * dst, size_t size);                               /*!< Read with context pointer */
        ssize_t (*read)(int fd, void * dst, size_t size);                                            /*!< Read without context pointer */
    };
    union {
        ssize_t (*pread_p)(void *ctx, int fd, void * dst, size_t size, off_t offset);                /*!< pread with context pointer */
        ssize_t (*pread)(int fd, void * dst, size_t size, off_t offset);                             /*!< pread without context pointer */
    };
    union {
        ssize_t (*pwrite_p)(void *ctx, int fd, const void *src, size_t size, off_t offset);          /*!< pwrite with context pointer */
        ssize_t (*pwrite)(int fd, const void *src, size_t size, off_t offset);                       /*!< pwrite without context pointer */
    };
    union {
        int (*open_p)(void* ctx, const char * path, int flags, int mode);                            /*!< open with context pointer */
        int (*open)(const char * path, int flags, int mode);                                         /*!< open without context pointer */
    };
    union {
        int (*close_p)(void* ctx, int fd);                                                           /*!< close with context pointer */
        int (*close)(int fd);                                                                        /*!< close without context pointer */
    };
    union {
        int (*fstat_p)(void* ctx, int fd, struct stat * st);                                         /*!< fstat with context pointer */
        int (*fstat)(int fd, struct stat * st);                                                      /*!< fstat without context pointer */
    };
    union {
        int (*fcntl_p)(void* ctx, int fd, int cmd, int arg);                                        /*!< fcntl with context pointer */
        int (*fcntl)(int fd, int cmd, int arg);                                                     /*!< fcntl without context pointer */
    };
    union {
        int (*ioctl_p)(void* ctx, int fd, int cmd, va_list args);                                   /*!< ioctl with context pointer */
        int (*ioctl)(int fd, int cmd, va_list args);                                                /*!< ioctl without context pointer */
    };
    union {
        int (*fsync_p)(void* ctx, int fd);                                                          /*!< fsync with context pointer */
        int (*fsync)(int fd);                                                                       /*!< fsync without context pointer */
    };

#ifdef CONFIG_VFS_SUPPORT_DIR
    esp_vfs_dir_ops_t *dir;                                                                             /*!< pointer to the dir subcomponent */
#endif

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    esp_vfs_termios_ops_t *termios;                                                                     /*!< pointer to the termios subcomponent */
#endif

#if CONFIG_VFS_SUPPORT_SELECT || defined __DOXYGEN__
    esp_vfs_select_ops_t *select;                                                                       /*!< pointer to the select subcomponent */
#endif

} esp_vfs_fs_ops_t;

/**
 * Register a virtual filesystem for given path prefix.
 *
 * @param base_path  file path prefix associated with the filesystem.
 *                   Must be a zero-terminated C string, may be empty.
 *                   If not empty, must be up to ESP_VFS_PATH_MAX
 *                   characters long, and at least 2 characters long.
 *                   Name must start with a "/" and must not end with "/".
 *                   For example, "/data" or "/dev/spi" are valid.
 *                   These VFSes would then be called to handle file paths such as
 *                   "/data/myfile.txt" or "/dev/spi/0".
 *                   In the special case of an empty base_path, a "fallback"
 *                   VFS is registered. Such VFS will handle paths which are not
 *                   matched by any other registered VFS.
 * @param vfs  Pointer to esp_vfs_fs_ops_t, a structure which maps syscalls to
 *             the filesystem driver functions. VFS component does not assume ownership of this struct, but see flags for more info
 *
 * @param flags Set of binary flags controlling how the registered FS should be treated
 *             - ESP_VFS_FLAG_STATIC - if this flag is specified VFS assumes the provided esp_vfs_fs_ops_t and all its subcomponents are statically allocated,
 *                                     if it is not enabled a deep copy of the provided struct will be created, which will be managed by the VFS component
 *             - ESP_VFS_FLAG_CONTEXT_PTR - If set, the VFS will use the context-aware versions of the filesystem operation functions (suffixed with `_p`) in `esp_vfs_fs_ops_t` and its subcomponents.
 *                                          The `ctx` parameter will be passed as the context argument when these functions are invoked.
 *
 * @param ctx  Context pointer for fs operation functions, see the ESP_VFS_FLAG_CONTEXT_PTR.
 *             Should be `NULL` if not used.
 *
 * @return  ESP_OK if successful, ESP_ERR_NO_MEM if too many FSes are
 *          registered.
 */
esp_err_t esp_vfs_register_fs(const char* base_path, const esp_vfs_fs_ops_t* vfs, int flags, void* ctx);

/**
 * Analog of esp_vfs_register_with_id which accepts esp_vfs_fs_ops_t instead.
 *
 */
esp_err_t esp_vfs_register_fs_with_id(const esp_vfs_fs_ops_t* vfs, int flags, void* ctx, esp_vfs_id_t* id);

/**
 * Alias for esp_vfs_unregister for naming consistency
 */
esp_err_t esp_vfs_unregister_fs(const char* base_path);

/**
 * Alias for esp_vfs_unregister_with_id for naming consistency
 */
esp_err_t esp_vfs_unregister_fs_with_id(esp_vfs_id_t id);

#ifdef __cplusplus
}
#endif
