// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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
#include <sys/dirent.h>
#include <string.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SYS_TYPES_FD_SET
#error "VFS should be used with FD_SETSIZE and FD_SET from sys/types.h"
#endif

/**
 * Maximum number of (global) file descriptors.
 */
#define MAX_FDS         FD_SETSIZE /* for compatibility with fd_set and select() */

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

/**
 * @brief VFS definition structure
 *
 * This structure should be filled with pointers to corresponding
 * FS driver functions.
 *
 * VFS component will translate all FDs so that the filesystem implementation
 * sees them starting at zero. The caller sees a global FD which is prefixed
 * with an pre-filesystem-implementation.
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
    int flags;      /*!< ESP_VFS_FLAG_CONTEXT_PTR or ESP_VFS_FLAG_DEFAULT */
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
#ifdef CONFIG_VFS_SUPPORT_DIR
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
#endif // CONFIG_VFS_SUPPORT_DIR
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
    union {
        int (*access_p)(void* ctx, const char *path, int amode);                                    /*!< access with context pointer */
        int (*access)(const char *path, int amode);                                                 /*!< access without context pointer */
    };
    union {
        int (*truncate_p)(void* ctx, const char *path, off_t length);                               /*!< truncate with context pointer */
        int (*truncate)(const char *path, off_t length);                                            /*!< truncate without context pointer */
    };
    union {
        int (*utime_p)(void* ctx, const char *path, const struct utimbuf *times);                   /*!< utime with context pointer */
        int (*utime)(const char *path, const struct utimbuf *times);                                /*!< utime without context pointer */
    };
#endif // CONFIG_VFS_SUPPORT_DIR
#ifdef CONFIG_VFS_SUPPORT_TERMIOS
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
#endif // CONFIG_VFS_SUPPORT_TERMIOS
#ifdef CONFIG_VFS_SUPPORT_SELECT
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
#endif // CONFIG_VFS_SUPPORT_SELECT
} esp_vfs_t;


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
 * Special case function for registering a VFS that uses a method other than
 * open() to open new file descriptors from the interval <min_fd; max_fd).
 *
 * This is a special-purpose function intended for registering LWIP sockets to VFS.
 *
 * @param vfs Pointer to esp_vfs_t. Meaning is the same as for esp_vfs_register().
 * @param ctx Pointer to context structure. Meaning is the same as for esp_vfs_register().
 * @param min_fd The smallest file descriptor this VFS will use.
 * @param max_fd Upper boundary for file descriptors this VFS will use (the biggest file descriptor plus one).
 *
 * @return  ESP_OK if successful, ESP_ERR_NO_MEM if too many VFSes are
 *          registered, ESP_ERR_INVALID_ARG if the file descriptor boundaries
 *          are incorrect.
 */
esp_err_t esp_vfs_register_fd_range(const esp_vfs_t *vfs, void *ctx, int min_fd, int max_fd);

/**
 * Special case function for registering a VFS that uses a method other than
 * open() to open new file descriptors. In comparison with
 * esp_vfs_register_fd_range, this function doesn't pre-registers an interval
 * of file descriptors. File descriptors can be registered later, by using
 * esp_vfs_register_fd.
 *
 * @param vfs Pointer to esp_vfs_t. Meaning is the same as for esp_vfs_register().
 * @param ctx Pointer to context structure. Meaning is the same as for esp_vfs_register().
 * @param vfs_id Here will be written the VFS ID which can be passed to
 *               esp_vfs_register_fd for registering file descriptors.
 *
 * @return  ESP_OK if successful, ESP_ERR_NO_MEM if too many VFSes are
 *          registered, ESP_ERR_INVALID_ARG if the file descriptor boundaries
 *          are incorrect.
 */
esp_err_t esp_vfs_register_with_id(const esp_vfs_t *vfs, void *ctx, esp_vfs_id_t *vfs_id);

/**
 * Unregister a virtual filesystem for given path prefix
 *
 * @param base_path  file prefix previously used in esp_vfs_register call
 * @return ESP_OK if successful, ESP_ERR_INVALID_STATE if VFS for given prefix
 *         hasn't been registered
 */
esp_err_t esp_vfs_unregister(const char* base_path);

/**
 * Unregister a virtual filesystem with the given index
 *
 * @param vfs_id  The VFS ID returned by esp_vfs_register_with_id
 * @return ESP_OK if successful, ESP_ERR_INVALID_STATE if VFS for the given index
 *         hasn't been registered
 */
esp_err_t esp_vfs_unregister_with_id(esp_vfs_id_t vfs_id);

/**
 * Special function for registering another file descriptor for a VFS registered
 * by esp_vfs_register_with_id.
 *
 * @param vfs_id VFS identificator returned by esp_vfs_register_with_id.
 * @param fd The registered file descriptor will be written to this address.
 *
 * @return  ESP_OK if the registration is successful,
 *          ESP_ERR_NO_MEM if too many file descriptors are registered,
 *          ESP_ERR_INVALID_ARG if the arguments are incorrect.
 */
esp_err_t esp_vfs_register_fd(esp_vfs_id_t vfs_id, int *fd);

/**
 * Special function for registering another file descriptor with given local_fd
 * for a VFS registered by esp_vfs_register_with_id.
 *
 * @param vfs_id VFS identificator returned by esp_vfs_register_with_id.
 * @param local_fd The fd in the local vfs. Passing -1 will set the local fd as the (*fd) value.
 * @param permanent Whether the fd should be treated as permannet (not removed after close())
 * @param fd The registered file descriptor will be written to this address.
 *
 * @return  ESP_OK if the registration is successful,
 *          ESP_ERR_NO_MEM if too many file descriptors are registered,
 *          ESP_ERR_INVALID_ARG if the arguments are incorrect.
 */
esp_err_t esp_vfs_register_fd_with_local_fd(esp_vfs_id_t vfs_id, int local_fd, bool permanent, int *fd);

/**
 * Special function for unregistering a file descriptor belonging to a VFS
 * registered by esp_vfs_register_with_id.
 *
 * @param vfs_id VFS identificator returned by esp_vfs_register_with_id.
 * @param fd File descriptor which should be unregistered.
 *
 * @return  ESP_OK if the registration is successful,
 *          ESP_ERR_INVALID_ARG if the arguments are incorrect.
 */
esp_err_t esp_vfs_unregister_fd(esp_vfs_id_t vfs_id, int fd);

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
int esp_vfs_utime(const char *path, const struct utimbuf *times);
/**@}*/

/**
 * @brief Synchronous I/O multiplexing which implements the functionality of POSIX select() for VFS
 * @param nfds      Specifies the range of descriptors which should be checked.
 *                  The first nfds descriptors will be checked in each set.
 * @param readfds   If not NULL, then points to a descriptor set that on input
 *                  specifies which descriptors should be checked for being
 *                  ready to read, and on output indicates which descriptors
 *                  are ready to read.
 * @param writefds  If not NULL, then points to a descriptor set that on input
 *                  specifies which descriptors should be checked for being
 *                  ready to write, and on output indicates which descriptors
 *                  are ready to write.
 * @param errorfds  If not NULL, then points to a descriptor set that on input
 *                  specifies which descriptors should be checked for error
 *                  conditions, and on output indicates which descriptors
 *                  have error conditions.
 * @param timeout   If not NULL, then points to timeval structure which
 *                  specifies the time period after which the functions should
 *                  time-out and return. If it is NULL, then the function will
 *                  not time-out.
 *
 * @return      The number of descriptors set in the descriptor sets, or -1
 *              when an error (specified by errno) have occurred.
 */
int esp_vfs_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout);

/**
 * @brief Notification from a VFS driver about a read/write/error condition
 *
 * This function is called when the VFS driver detects a read/write/error
 * condition as it was requested by the previous call to start_select.
 *
 * @param sem semaphore structure which was passed to the driver by the start_select call
 */
void esp_vfs_select_triggered(esp_vfs_select_sem_t sem);

/**
 * @brief Notification from a VFS driver about a read/write/error condition (ISR version)
 *
 * This function is called when the VFS driver detects a read/write/error
 * condition as it was requested by the previous call to start_select.
 *
 * @param sem semaphore structure which was passed to the driver by the start_select call
 * @param woken is set to pdTRUE if the function wakes up a task with higher priority
 */
void esp_vfs_select_triggered_isr(esp_vfs_select_sem_t sem, BaseType_t *woken);

/**
 *
 * @brief Implements the VFS layer of POSIX pread()
 *
 * @param fd         File descriptor used for read
 * @param dst        Pointer to the buffer where the output will be written
 * @param size       Number of bytes to be read
 * @param offset     Starting offset of the read
 *
 * @return           A positive return value indicates the number of bytes read. -1 is return on failure and errno is
 *                   set accordingly.
 */
ssize_t esp_vfs_pread(int fd, void *dst, size_t size, off_t offset);

/**
 *
 * @brief Implements the VFS layer of POSIX pwrite()
 *
 * @param fd         File descriptor used for write
 * @param src        Pointer to the buffer from where the output will be read
 * @param size       Number of bytes to write
 * @param offset     Starting offset of the write
 *
 * @return           A positive return value indicates the number of bytes written. -1 is return on failure and errno is
 *                   set accordingly.
 */
ssize_t esp_vfs_pwrite(int fd, const void *src, size_t size, off_t offset);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //__ESP_VFS_H__
