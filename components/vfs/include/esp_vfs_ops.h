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

typedef           int  (*esp_vfs_stat_ctx_op_t)      (void *ctx, const char *path, struct stat *st);                    /*!< stat with context pointer */
typedef           int  (*esp_vfs_stat_op_t)          (           const char *path, struct stat *st);                    /*!< stat without context pointer */
typedef           int  (*esp_vfs_link_ctx_op_t)      (void *ctx, const char *n1, const char *n2);                       /*!< link with context pointer */
typedef           int  (*esp_vfs_link_op_t)          (           const char *n1, const char *n2);                       /*!< link without context pointer */
typedef           int  (*esp_vfs_unlink_ctx_op_t)    (void *ctx, const char *path);                                     /*!< unlink with context pointer */
typedef           int  (*esp_vfs_unlink_op_t)        (           const char *path);                                     /*!< unlink without context pointer */
typedef           int  (*esp_vfs_rename_ctx_op_t)    (void *ctx, const char *src, const char *dst);                     /*!< rename with context pointer */
typedef           int  (*esp_vfs_rename_op_t)        (           const char *src, const char *dst);                     /*!< rename without context pointer */
typedef           DIR* (*esp_vfs_opendir_ctx_op_t)   (void *ctx, const char *name);                                     /*!< opendir with context pointer */
typedef           DIR* (*esp_vfs_opendir_op_t)       (           const char *name);                                     /*!< opendir without context pointer */
typedef struct dirent* (*esp_vfs_readdir_ctx_op_t)   (void *ctx, DIR *pdir);                                            /*!< readdir with context pointer */
typedef struct dirent* (*esp_vfs_readdir_op_t)       (           DIR *pdir);                                            /*!< readdir without context pointer */
typedef           int  (*esp_vfs_readdir_r_ctx_op_t) (void *ctx, DIR *pdir, struct dirent *entry, struct dirent **out); /*!< readdir_r with context pointer */
typedef           int  (*esp_vfs_readdir_r_op_t)     (           DIR *pdir, struct dirent *entry, struct dirent **out); /*!< readdir_r without context pointer */
typedef          long  (*esp_vfs_telldir_ctx_op_t)   (void *ctx, DIR *pdir);                                            /*!< telldir with context pointer */
typedef          long  (*esp_vfs_telldir_op_t)       (           DIR *pdir);                                            /*!< telldir without context pointer */
typedef          void  (*esp_vfs_seekdir_ctx_op_t)   (void *ctx, DIR *pdir, long offset);                               /*!< seekdir with context pointer */
typedef          void  (*esp_vfs_seekdir_op_t)       (           DIR *pdir, long offset);                               /*!< seekdir without context pointer */
typedef           int  (*esp_vfs_closedir_ctx_op_t)  (void *ctx, DIR *pdir);                                            /*!< closedir with context pointer */
typedef           int  (*esp_vfs_closedir_op_t)      (           DIR *pdir);                                            /*!< closedir without context pointer */
typedef           int  (*esp_vfs_mkdir_ctx_op_t)     (void *ctx, const char *name, mode_t mode);                        /*!< mkdir with context pointer */
typedef           int  (*esp_vfs_mkdir_op_t)         (           const char *name, mode_t mode);                        /*!< mkdir without context pointer */
typedef           int  (*esp_vfs_rmdir_ctx_op_t)     (void *ctx, const char *name);                                     /*!< rmdir with context pointer */
typedef           int  (*esp_vfs_rmdir_op_t)         (           const char *name);                                     /*!< rmdir without context pointer */
typedef           int  (*esp_vfs_access_ctx_op_t)    (void *ctx, const char *path, int amode);                          /*!< access with context pointer */
typedef           int  (*esp_vfs_access_op_t)        (           const char *path, int amode);                          /*!< access without context pointer */
typedef           int  (*esp_vfs_truncate_ctx_op_t)  (void *ctx, const char *path, off_t length);                       /*!< truncate with context pointer */
typedef           int  (*esp_vfs_truncate_op_t)      (           const char *path, off_t length);                       /*!< truncate without context pointer */
typedef           int  (*esp_vfs_ftruncate_ctx_op_t) (void *ctx, int fd, off_t length);                                 /*!< ftruncate with context pointer */
typedef           int  (*esp_vfs_ftruncate_op_t)     (           int fd, off_t length);                                 /*!< ftruncate without context pointer */
typedef           int  (*esp_vfs_utime_ctx_op_t)     (void *ctx, const char *path, const struct utimbuf *times);        /*!< utime with context pointer */
typedef           int  (*esp_vfs_utime_op_t)         (           const char *path, const struct utimbuf *times);        /*!< utime without context pointer */

/**
 * @brief Struct containing function pointers to directory related functionality.
 *
 */
typedef struct {
    union {
        const esp_vfs_stat_ctx_op_t      stat_p;      /*!< stat with context pointer */
        const esp_vfs_stat_op_t          stat;        /*!< stat without context pointer */
    };
    union {
        const esp_vfs_link_ctx_op_t      link_p;      /*!< link with context pointer */
        const esp_vfs_link_op_t          link;        /*!< link without context pointer */
    };
    union {
        const esp_vfs_unlink_ctx_op_t    unlink_p;    /*!< unlink with context pointer */
        const esp_vfs_unlink_op_t        unlink;      /*!< unlink without context pointer */
    };
    union {
        const esp_vfs_rename_ctx_op_t    rename_p;    /*!< rename with context pointer */
        const esp_vfs_rename_op_t        rename;      /*!< rename without context pointer */
    };
    union {
        const esp_vfs_opendir_ctx_op_t   opendir_p;   /*!< opendir with context pointer */
        const esp_vfs_opendir_op_t       opendir;     /*!< opendir without context pointer */
    };
    union {
        const esp_vfs_readdir_ctx_op_t   readdir_p;   /*!< readdir with context pointer */
        const esp_vfs_readdir_op_t       readdir;     /*!< readdir without context pointer */
    };
    union {
        const esp_vfs_readdir_r_ctx_op_t readdir_r_p; /*!< readdir_r with context pointer */
        const esp_vfs_readdir_r_op_t     readdir_r;   /*!< readdir_r without context pointer */
    };
    union {
        const esp_vfs_telldir_ctx_op_t   telldir_p;   /*!< telldir with context pointer */
        const esp_vfs_telldir_op_t       telldir;     /*!< telldir without context pointer */
    };
    union {
        const esp_vfs_seekdir_ctx_op_t   seekdir_p;   /*!< seekdir with context pointer */
        const esp_vfs_seekdir_op_t       seekdir;     /*!< seekdir without context pointer */
    };
    union {
        const esp_vfs_closedir_ctx_op_t  closedir_p;  /*!< closedir with context pointer */
        const esp_vfs_closedir_op_t      closedir;    /*!< closedir without context pointer */
    };
    union {
        const esp_vfs_mkdir_ctx_op_t     mkdir_p;     /*!< mkdir with context pointer */
        const esp_vfs_mkdir_op_t         mkdir;       /*!< mkdir without context pointer */
    };
    union {
        const esp_vfs_rmdir_ctx_op_t     rmdir_p;     /*!< rmdir with context pointer */
        const esp_vfs_rmdir_op_t         rmdir;       /*!< rmdir without context pointer */
    };
    union {
        const esp_vfs_access_ctx_op_t    access_p;    /*!< access with context pointer */
        const esp_vfs_access_op_t        access;      /*!< access without context pointer */
    };
    union {
        const esp_vfs_truncate_ctx_op_t  truncate_p;  /*!< truncate with context pointer */
        const esp_vfs_truncate_op_t      truncate;    /*!< truncate without context pointer */
    };
    union {
        const esp_vfs_ftruncate_ctx_op_t ftruncate_p; /*!< ftruncate with context pointer */
        const esp_vfs_ftruncate_op_t     ftruncate;   /*!< ftruncate without context pointer */
    };
    union {
        const esp_vfs_utime_ctx_op_t     utime_p;     /*!< utime with context pointer */
        const esp_vfs_utime_op_t         utime;       /*!< utime without context pointer */
    };
} esp_vfs_dir_ops_t;

#endif // CONFIG_VFS_SUPPORT_DIR

#ifdef CONFIG_VFS_SUPPORT_TERMIOS

typedef   int (*esp_vfs_tcsetattr_ctx_op_t)   (void *ctx, int fd, int optional_actions, const struct termios *p); /*!< tcsetattr with context pointer */
typedef   int (*esp_vfs_tcsetattr_op_t)       (           int fd, int optional_actions, const struct termios *p); /*!< tcsetattr without context pointer */
typedef   int (*esp_vfs_tcgetattr_ctx_op_t)   (void *ctx, int fd, struct termios *p);                             /*!< tcgetattr with context pointer */
typedef   int (*esp_vfs_tcgetattr_op_t)       (           int fd, struct termios *p);                             /*!< tcgetattr without context pointer */
typedef   int (*esp_vfs_tcdrain_ctx_op_t)     (void *ctx, int fd);                                                /*!< tcdrain with context pointer */
typedef   int (*esp_vfs_tcdrain_op_t)         (           int fd);                                                /*!< tcdrain without context pointer */
typedef   int (*esp_vfs_tcflush_ctx_op_t)     (void *ctx, int fd, int select);                                    /*!< tcflush with context pointer */
typedef   int (*esp_vfs_tcflush_op_t)         (           int fd, int select);                                    /*!< tcflush without context pointer */
typedef   int (*esp_vfs_tcflow_ctx_op_t)      (void *ctx, int fd, int action);                                    /*!< tcflow with context pointer */
typedef   int (*esp_vfs_tcflow_op_t)          (           int fd, int action);                                    /*!< tcflow without context pointer */
typedef pid_t (*esp_vfs_tcgetsid_ctx_op_t)    (void *ctx, int fd);                                                /*!< tcgetsid with context pointer */
typedef pid_t (*esp_vfs_tcgetsid_op_t)        (           int fd);                                                /*!< tcgetsid without context pointer */
typedef   int (*esp_vfs_tcsendbreak_ctx_op_t) (void *ctx, int fd, int duration);                                  /*!< tcsendbreak with context pointer */
typedef   int (*esp_vfs_tcsendbreak_op_t)     (           int fd, int duration);                                  /*!< tcsendbreak without context pointer */

/**
 * @brief Struct containing function pointers to termios related functionality.
 *
 */
typedef struct {
    union {
        const esp_vfs_tcsetattr_ctx_op_t   tcsetattr_p;   /*!< tcsetattr with context pointer */
        const esp_vfs_tcsetattr_op_t       tcsetattr;     /*!< tcsetattr without context pointer */
    };
    union {
        const esp_vfs_tcgetattr_ctx_op_t   tcgetattr_p;   /*!< tcgetattr with context pointer */
        const esp_vfs_tcgetattr_op_t       tcgetattr;     /*!< tcgetattr without context pointer */
    };
    union {
        const esp_vfs_tcdrain_ctx_op_t     tcdrain_p;     /*!< tcdrain with context pointer */
        const esp_vfs_tcdrain_op_t         tcdrain;       /*!< tcdrain without context pointer */
    };
    union {
        const esp_vfs_tcflush_ctx_op_t     tcflush_p;     /*!< tcflush with context pointer */
        const esp_vfs_tcflush_op_t         tcflush;       /*!< tcflush without context pointer */
    };
    union {
        const esp_vfs_tcflow_ctx_op_t      tcflow_p;      /*!< tcflow with context pointer */
        const esp_vfs_tcflow_op_t          tcflow;        /*!< tcflow without context pointer */
    };
    union {
        const esp_vfs_tcgetsid_ctx_op_t    tcgetsid_p;    /*!< tcgetsid with context pointer */
        const esp_vfs_tcgetsid_op_t        tcgetsid;      /*!< tcgetsid without context pointer */
    };
    union {
        const esp_vfs_tcsendbreak_ctx_op_t tcsendbreak_p; /*!< tcsendbreak with context pointer */
        const esp_vfs_tcsendbreak_op_t     tcsendbreak;   /*!< tcsendbreak without context pointer */
    };
} esp_vfs_termios_ops_t;

#endif // CONFIG_VFS_SUPPORT_TERMIOS

#ifdef CONFIG_VFS_SUPPORT_SELECT

typedef esp_err_t  (*esp_vfs_start_select_op_t)                (int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, esp_vfs_select_sem_t sem, void **end_select_args);
typedef       int  (*esp_vfs_socket_select_op_t)               (int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout);
typedef      void  (*esp_vfs_stop_socket_select_op_t)          (void *sem);
typedef      void  (*esp_vfs_stop_socket_select_isr_op_t)      (void *sem, BaseType_t *woken);
typedef      void* (*esp_vfs_get_socket_select_semaphore_op_t) (void);
typedef esp_err_t  (*esp_vfs_end_select_op_t)                  (void *end_select_args);

/**
 * @brief Struct containing function pointers to select related functionality.
 *
 */
typedef struct {
    /** start_select is called for setting up synchronous I/O multiplexing of the desired file descriptors in the given VFS */
    const esp_vfs_start_select_op_t                start_select;

    /** socket select function for socket FDs with the functionality of POSIX select(); this should be set only for the socket VFS */
    const esp_vfs_socket_select_op_t               socket_select;

    /** called by VFS to interrupt the socket_select call when select is activated from a non-socket VFS driver; set only for the socket driver */
    const esp_vfs_stop_socket_select_op_t          stop_socket_select;

    /** stop_socket_select which can be called from ISR; set only for the socket driver */
    const esp_vfs_stop_socket_select_isr_op_t      stop_socket_select_isr;

    /** end_select is called to stop the I/O multiplexing and deinitialize the environment created by start_select for the given VFS */
    const esp_vfs_get_socket_select_semaphore_op_t get_socket_select_semaphore;

    /** get_socket_select_semaphore returns semaphore allocated in the socket driver; set only for the socket driver */
    const esp_vfs_end_select_op_t                  end_select;
} esp_vfs_select_ops_t;

#endif // CONFIG_VFS_SUPPORT_SELECT

typedef ssize_t (*esp_vfs_write_ctx_op_t)  (void *ctx, int fd, const void *data, size_t size);              /*!< Write with context pointer */
typedef ssize_t (*esp_vfs_write_op_t)      (           int fd, const void *data, size_t size);              /*!< Write without context pointer */
typedef   off_t (*esp_vfs_lseek_ctx_op_t)  (void *ctx, int fd, off_t size, int mode);                       /*!< Seek with context pointer */
typedef   off_t (*esp_vfs_lseek_op_t)      (           int fd, off_t size, int mode);                       /*!< Seek without context pointer */
typedef ssize_t (*esp_vfs_read_ctx_op_t)   (void *ctx, int fd, void *dst, size_t size);                     /*!< Read with context pointer */
typedef ssize_t (*esp_vfs_read_op_t)       (           int fd, void *dst, size_t size);                     /*!< Read without context pointer */
typedef ssize_t (*esp_vfs_pread_ctx_op_t)  (void *ctx, int fd, void *dst, size_t size, off_t offset);       /*!< pread with context pointer */
typedef ssize_t (*esp_vfs_pread_op_t)      (           int fd, void *dst, size_t size, off_t offset);       /*!< pread without context pointer */
typedef ssize_t (*esp_vfs_pwrite_ctx_op_t) (void *ctx, int fd, const void *src, size_t size, off_t offset); /*!< pwrite with context pointer */
typedef ssize_t (*esp_vfs_pwrite_op_t)     (           int fd, const void *src, size_t size, off_t offset); /*!< pwrite without context pointer */
typedef     int (*esp_vfs_open_ctx_op_t)   (void *ctx, const char *path, int flags, int mode);              /*!< open with context pointer */
typedef     int (*esp_vfs_open_op_t)       (           const char *path, int flags, int mode);              /*!< open without context pointer */
typedef     int (*esp_vfs_close_ctx_op_t)  (void *ctx, int fd);                                             /*!< close with context pointer */
typedef     int (*esp_vfs_close_op_t)      (           int fd);                                             /*!< close without context pointer */
typedef     int (*esp_vfs_fstat_ctx_op_t)  (void *ctx, int fd, struct stat *st);                            /*!< fstat with context pointer */
typedef     int (*esp_vfs_fstat_op_t)      (           int fd, struct stat *st);                            /*!< fstat without context pointer */
typedef     int (*esp_vfs_fcntl_ctx_op_t)  (void *ctx, int fd, int cmd, int arg);                           /*!< fcntl with context pointer */
typedef     int (*esp_vfs_fcntl_op_t)      (           int fd, int cmd, int arg);                           /*!< fcntl without context pointer */
typedef     int (*esp_vfs_ioctl_ctx_op_t)  (void *ctx, int fd, int cmd, va_list args);                      /*!< ioctl with context pointer */
typedef     int (*esp_vfs_ioctl_op_t)      (           int fd, int cmd, va_list args);                      /*!< ioctl without context pointer */
typedef     int (*esp_vfs_fsync_ctx_op_t)  (void *ctx, int fd);                                             /*!< fsync with context pointer */
typedef     int (*esp_vfs_fsync_op_t)      (           int fd);                                             /*!< fsync without context pointer */

/**
 * @brief Main struct of the minified vfs API, containing basic function pointers as well as pointers to the other subcomponents.
 *
 */
typedef struct {
    union {
        const esp_vfs_write_ctx_op_t  write_p;  /*!< Write with context pointer */
        const esp_vfs_write_op_t      write;    /*!< Write without context pointer */
    };
    union {
        const esp_vfs_lseek_ctx_op_t  lseek_p;  /*!< Seek with context pointer */
        const esp_vfs_lseek_op_t      lseek;    /*!< Seek without context pointer */
    };
    union {
        const esp_vfs_read_ctx_op_t   read_p;   /*!< Read with context pointer */
        const esp_vfs_read_op_t       read;     /*!< Read without context pointer */
    };
    union {
        const esp_vfs_pread_ctx_op_t  pread_p;  /*!< pread with context pointer */
        const esp_vfs_pread_op_t      pread;    /*!< pread without context pointer */
    };
    union {
        const esp_vfs_pwrite_ctx_op_t pwrite_p; /*!< pwrite with context pointer */
        const esp_vfs_pwrite_op_t     pwrite;   /*!< pwrite without context pointer */
    };
    union {
        const esp_vfs_open_ctx_op_t   open_p;   /*!< open with context pointer */
        const esp_vfs_open_op_t       open;     /*!< open without context pointer */
    };
    union {
        const esp_vfs_close_ctx_op_t  close_p;  /*!< close with context pointer */
        const esp_vfs_close_op_t      close;    /*!< close without context pointer */
    };
    union {
        const esp_vfs_fstat_ctx_op_t  fstat_p;  /*!< fstat with context pointer */
        const esp_vfs_fstat_op_t      fstat;    /*!< fstat without context pointer */
    };
    union {
        const esp_vfs_fcntl_ctx_op_t  fcntl_p;  /*!< fcntl with context pointer */
        const esp_vfs_fcntl_op_t      fcntl;    /*!< fcntl without context pointer */
    };
    union {
        const esp_vfs_ioctl_ctx_op_t  ioctl_p;  /*!< ioctl with context pointer */
        const esp_vfs_ioctl_op_t      ioctl;    /*!< ioctl without context pointer */
    };
    union {
        const esp_vfs_fsync_ctx_op_t  fsync_p;  /*!< fsync with context pointer */
        const esp_vfs_fsync_op_t      fsync;    /*!< fsync without context pointer */
    };

#ifdef CONFIG_VFS_SUPPORT_DIR
    const esp_vfs_dir_ops_t *const dir;         /*!< pointer to the dir subcomponent */
#endif

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    const esp_vfs_termios_ops_t *const termios; /*!< pointer to the termios subcomponent */
#endif

#if CONFIG_VFS_SUPPORT_SELECT || defined __DOXYGEN__
    const esp_vfs_select_ops_t *const select;   /*!< pointer to the select subcomponent */
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
esp_err_t esp_vfs_register_fs(const char *base_path, const esp_vfs_fs_ops_t *vfs, int flags, void *ctx);

/**
 * Analog of esp_vfs_register_with_id which accepts esp_vfs_fs_ops_t instead.
 *
 */
esp_err_t esp_vfs_register_fs_with_id(const esp_vfs_fs_ops_t *vfs, int flags, void *ctx, esp_vfs_id_t *id);

/**
 * Alias for esp_vfs_unregister for naming consistency
 */
esp_err_t esp_vfs_unregister_fs(const char *base_path);

/**
 * Alias for esp_vfs_unregister_with_id for naming consistency
 */
esp_err_t esp_vfs_unregister_fs_with_id(esp_vfs_id_t id);

#ifdef __cplusplus
}
#endif
