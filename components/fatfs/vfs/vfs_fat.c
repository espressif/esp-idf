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

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/lock.h>
#include "esp_vfs.h"
#include "esp_log.h"
#include "ff.h"
#include "diskio_impl.h"

typedef struct {
    char fat_drive[8];  /* FAT drive name */
    char base_path[ESP_VFS_PATH_MAX];   /* base path in VFS where partition is registered */
    size_t max_files;   /* max number of simultaneously open files; size of files[] array */
    _lock_t lock;       /* guard for access to this structure */
    FATFS fs;           /* fatfs library FS structure */
    char tmp_path_buf[FILENAME_MAX+3];  /* temporary buffer used to prepend drive name to the path */
    char tmp_path_buf2[FILENAME_MAX+3]; /* as above; used in functions which take two path arguments */
    bool *o_append;  /* O_APPEND is stored here for each max_files entries (because O_APPEND is not compatible with FA_OPEN_APPEND) */
    FIL files[0];   /* array with max_files entries; must be the final member of the structure */
} vfs_fat_ctx_t;

typedef struct {
    DIR dir;
    long offset;
    FF_DIR ffdir;
    FILINFO filinfo;
    struct dirent cur_dirent;
} vfs_fat_dir_t;

/* Date and time storage formats in FAT */
typedef union {
    struct {
        uint16_t mday : 5;  /* Day of month, 1 - 31 */
        uint16_t mon : 4;   /* Month, 1 - 12 */
        uint16_t year : 7;  /* Year, counting from 1980. E.g. 37 for 2017 */
    };
    uint16_t as_int;
} fat_date_t;

typedef union {
    struct {
        uint16_t sec : 5;   /* Seconds divided by 2. E.g. 21 for 42 seconds */
        uint16_t min : 6;   /* Minutes, 0 - 59 */
        uint16_t hour : 5;  /* Hour, 0 - 23 */
    };
    uint16_t as_int;
} fat_time_t;

static const char* TAG = "vfs_fat";

static ssize_t vfs_fat_write(void* p, int fd, const void * data, size_t size);
static off_t vfs_fat_lseek(void* p, int fd, off_t size, int mode);
static ssize_t vfs_fat_read(void* ctx, int fd, void * dst, size_t size);
static ssize_t vfs_fat_pread(void *ctx, int fd, void *dst, size_t size, off_t offset);
static ssize_t vfs_fat_pwrite(void *ctx, int fd, const void *src, size_t size, off_t offset);
static int vfs_fat_open(void* ctx, const char * path, int flags, int mode);
static int vfs_fat_close(void* ctx, int fd);
static int vfs_fat_fstat(void* ctx, int fd, struct stat * st);
static int vfs_fat_stat(void* ctx, const char * path, struct stat * st);
static int vfs_fat_fsync(void* ctx, int fd);
static int vfs_fat_link(void* ctx, const char* n1, const char* n2);
static int vfs_fat_unlink(void* ctx, const char *path);
static int vfs_fat_rename(void* ctx, const char *src, const char *dst);
static DIR* vfs_fat_opendir(void* ctx, const char* name);
static struct dirent* vfs_fat_readdir(void* ctx, DIR* pdir);
static int vfs_fat_readdir_r(void* ctx, DIR* pdir, struct dirent* entry, struct dirent** out_dirent);
static long vfs_fat_telldir(void* ctx, DIR* pdir);
static void vfs_fat_seekdir(void* ctx, DIR* pdir, long offset);
static int vfs_fat_closedir(void* ctx, DIR* pdir);
static int vfs_fat_mkdir(void* ctx, const char* name, mode_t mode);
static int vfs_fat_rmdir(void* ctx, const char* name);
static int vfs_fat_access(void* ctx, const char *path, int amode);
static int vfs_fat_truncate(void* ctx, const char *path, off_t length);
static int vfs_fat_utime(void* ctx, const char *path, const struct utimbuf *times);

static vfs_fat_ctx_t* s_fat_ctxs[FF_VOLUMES] = { NULL, NULL };
//backwards-compatibility with esp_vfs_fat_unregister()
static vfs_fat_ctx_t* s_fat_ctx = NULL;

static size_t find_context_index_by_path(const char* base_path)
{
    for(size_t i=0; i<FF_VOLUMES; i++) {
        if (s_fat_ctxs[i] && !strcmp(s_fat_ctxs[i]->base_path, base_path)) {
            return i;
        }
    }
    return FF_VOLUMES;
}

static size_t find_unused_context_index()
{
    for(size_t i=0; i<FF_VOLUMES; i++) {
        if (!s_fat_ctxs[i]) {
            return i;
        }
    }
    return FF_VOLUMES;
}

esp_err_t esp_vfs_fat_register(const char* base_path, const char* fat_drive, size_t max_files, FATFS** out_fs)
{
    size_t ctx = find_context_index_by_path(base_path);
    if (ctx < FF_VOLUMES) {
        return ESP_ERR_INVALID_STATE;
    }

    ctx = find_unused_context_index();
    if (ctx == FF_VOLUMES) {
        return ESP_ERR_NO_MEM;
    }

    const esp_vfs_t vfs = {
        .flags = ESP_VFS_FLAG_CONTEXT_PTR,
        .write_p = &vfs_fat_write,
        .lseek_p = &vfs_fat_lseek,
        .read_p = &vfs_fat_read,
        .pread_p = &vfs_fat_pread,
        .pwrite_p = &vfs_fat_pwrite,
        .open_p = &vfs_fat_open,
        .close_p = &vfs_fat_close,
        .fstat_p = &vfs_fat_fstat,
        .stat_p = &vfs_fat_stat,
        .fsync_p = &vfs_fat_fsync,
        .link_p = &vfs_fat_link,
        .unlink_p = &vfs_fat_unlink,
        .rename_p = &vfs_fat_rename,
        .opendir_p = &vfs_fat_opendir,
        .closedir_p = &vfs_fat_closedir,
        .readdir_p = &vfs_fat_readdir,
        .readdir_r_p = &vfs_fat_readdir_r,
        .seekdir_p = &vfs_fat_seekdir,
        .telldir_p = &vfs_fat_telldir,
        .mkdir_p = &vfs_fat_mkdir,
        .rmdir_p = &vfs_fat_rmdir,
        .access_p = &vfs_fat_access,
        .truncate_p = &vfs_fat_truncate,
        .utime_p = &vfs_fat_utime,
    };
    size_t ctx_size = sizeof(vfs_fat_ctx_t) + max_files * sizeof(FIL);
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ff_memalloc(ctx_size);
    if (fat_ctx == NULL) {
        return ESP_ERR_NO_MEM;
    }
    memset(fat_ctx, 0, ctx_size);
    fat_ctx->o_append = ff_memalloc(max_files * sizeof(bool));
    if (fat_ctx->o_append == NULL) {
        free(fat_ctx);
        return ESP_ERR_NO_MEM;
    }
    memset(fat_ctx->o_append, 0, max_files * sizeof(bool));
    fat_ctx->max_files = max_files;
    strlcpy(fat_ctx->fat_drive, fat_drive, sizeof(fat_ctx->fat_drive) - 1);
    strlcpy(fat_ctx->base_path, base_path, sizeof(fat_ctx->base_path) - 1);

    esp_err_t err = esp_vfs_register(base_path, &vfs, fat_ctx);
    if (err != ESP_OK) {
        free(fat_ctx->o_append);
        free(fat_ctx);
        return err;
    }

    _lock_init(&fat_ctx->lock);
    s_fat_ctxs[ctx] = fat_ctx;

    //compatibility
    s_fat_ctx = fat_ctx;

    *out_fs = &fat_ctx->fs;

    return ESP_OK;
}

esp_err_t esp_vfs_fat_unregister_path(const char* base_path)
{
    size_t ctx = find_context_index_by_path(base_path);
    if (ctx == FF_VOLUMES) {
        return ESP_ERR_INVALID_STATE;
    }
    vfs_fat_ctx_t* fat_ctx = s_fat_ctxs[ctx];
    esp_err_t err = esp_vfs_unregister(fat_ctx->base_path);
    if (err != ESP_OK) {
        return err;
    }
    _lock_close(&fat_ctx->lock);
    free(fat_ctx->o_append);
    free(fat_ctx);
    s_fat_ctxs[ctx] = NULL;
    return ESP_OK;
}

static int get_next_fd(vfs_fat_ctx_t* fat_ctx)
{
    for (size_t i = 0; i < fat_ctx->max_files; ++i) {
        if (fat_ctx->files[i].obj.fs == NULL) {
            return (int) i;
        }
    }
    return -1;
}

static int fat_mode_conv(int m)
{
    int res = 0;
    int acc_mode = m & O_ACCMODE;
    if (acc_mode == O_RDONLY) {
        res |= FA_READ;
    } else if (acc_mode == O_WRONLY) {
        res |= FA_WRITE;
    } else if (acc_mode == O_RDWR) {
        res |= FA_READ | FA_WRITE;
    }
    if ((m & O_CREAT) && (m & O_EXCL)) {
        res |= FA_CREATE_NEW;
    } else if ((m & O_CREAT) && (m & O_TRUNC)) {
        res |= FA_CREATE_ALWAYS;
    } else if (m & O_APPEND) {
        res |= FA_OPEN_ALWAYS;
    } else {
        res |= FA_OPEN_EXISTING;
    }
    return res;
}

static int fresult_to_errno(FRESULT fr)
{
    switch(fr) {
        case FR_DISK_ERR:       return EIO;
        case FR_INT_ERR:
            assert(0 && "fatfs internal error");
            return EIO;
        case FR_NOT_READY:      return ENODEV;
        case FR_NO_FILE:        return ENOENT;
        case FR_NO_PATH:        return ENOENT;
        case FR_INVALID_NAME:   return EINVAL;
        case FR_DENIED:         return EACCES;
        case FR_EXIST:          return EEXIST;
        case FR_INVALID_OBJECT: return EBADF;
        case FR_WRITE_PROTECTED: return EACCES;
        case FR_INVALID_DRIVE:  return ENXIO;
        case FR_NOT_ENABLED:    return ENODEV;
        case FR_NO_FILESYSTEM:  return ENODEV;
        case FR_MKFS_ABORTED:   return EINTR;
        case FR_TIMEOUT:        return ETIMEDOUT;
        case FR_LOCKED:         return EACCES;
        case FR_NOT_ENOUGH_CORE: return ENOMEM;
        case FR_TOO_MANY_OPEN_FILES: return ENFILE;
        case FR_INVALID_PARAMETER: return EINVAL;
        case FR_OK: return 0;
    }
    assert(0 && "unhandled FRESULT");
    return ENOTSUP;
}

static void file_cleanup(vfs_fat_ctx_t* ctx, int fd)
{
    memset(&ctx->files[fd], 0, sizeof(FIL));
}

/**
 * @brief Prepend drive letters to path names
 * This function returns new path path pointers, pointing to a temporary buffer
 * inside ctx.
 * @note Call this function with ctx->lock acquired. Paths are valid while the
 *       lock is held.
 * @param ctx vfs_fat_ctx_t context
 * @param[inout] path as input, pointer to the path; as output, pointer to the new path
 * @param[inout] path2 as input, pointer to the path; as output, pointer to the new path
 */
static void prepend_drive_to_path(vfs_fat_ctx_t * ctx, const char ** path, const char ** path2){
    snprintf(ctx->tmp_path_buf, sizeof(ctx->tmp_path_buf), "%s%s", ctx->fat_drive, *path);
    *path = ctx->tmp_path_buf;
    if(path2){
        snprintf(ctx->tmp_path_buf2, sizeof(ctx->tmp_path_buf2), "%s%s", ((vfs_fat_ctx_t*)ctx)->fat_drive, *path2);
        *path2 = ctx->tmp_path_buf2;
    }
}

static int vfs_fat_open(void* ctx, const char * path, int flags, int mode)
{
    ESP_LOGV(TAG, "%s: path=\"%s\", flags=%x, mode=%x", __func__, path, flags, mode);
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    _lock_acquire(&fat_ctx->lock);
    prepend_drive_to_path(fat_ctx, &path, NULL);
    int fd = get_next_fd(fat_ctx);
    if (fd < 0) {
        _lock_release(&fat_ctx->lock);
        ESP_LOGE(TAG, "open: no free file descriptors");
        errno = ENFILE;
        return -1;
    }
    FRESULT res = f_open(&fat_ctx->files[fd], path, fat_mode_conv(flags));
    if (res != FR_OK) {
        file_cleanup(fat_ctx, fd);
        _lock_release(&fat_ctx->lock);
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        return -1;
    }
    // O_APPEND need to be stored because it is not compatible with FA_OPEN_APPEND:
    //  - FA_OPEN_APPEND means to jump to the end of file only after open()
    //  - O_APPEND means to jump to the end only before each write()
    // Other VFS drivers handles O_APPEND well (to the best of my knowledge),
    // therefore this flag is stored here (at this VFS level) in order to save
    // memory.
    fat_ctx->o_append[fd] = (flags & O_APPEND) == O_APPEND;
    _lock_release(&fat_ctx->lock);
    return fd;
}

static ssize_t vfs_fat_write(void* ctx, int fd, const void * data, size_t size)
{
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    FIL* file = &fat_ctx->files[fd];
    FRESULT res;
    if (fat_ctx->o_append[fd]) {
        if ((res = f_lseek(file, f_size(file))) != FR_OK) {
            ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
            errno = fresult_to_errno(res);
            return -1;
        }
    }
    unsigned written = 0;
    res = f_write(file, data, size, &written);
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        if (written == 0) {
            return -1;
        }
    }
    return written;
}

static ssize_t vfs_fat_read(void* ctx, int fd, void * dst, size_t size)
{
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    FIL* file = &fat_ctx->files[fd];
    unsigned read = 0;
    FRESULT res = f_read(file, dst, size, &read);
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        if (read == 0) {
            return -1;
        }
    }
    return read;
}

static ssize_t vfs_fat_pread(void *ctx, int fd, void *dst, size_t size, off_t offset)
{
    ssize_t ret = -1;
    vfs_fat_ctx_t *fat_ctx = (vfs_fat_ctx_t *) ctx;
    _lock_acquire(&fat_ctx->lock);
    FIL *file = &fat_ctx->files[fd];
    const off_t prev_pos = f_tell(file);

    FRESULT f_res = f_lseek(file, offset);
    if (f_res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, f_res);
        errno = fresult_to_errno(f_res);
        goto pread_release;
    }

    unsigned read = 0;
    f_res = f_read(file, dst, size, &read);
    if (f_res == FR_OK) {
        ret = read;
    } else {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, f_res);
        errno = fresult_to_errno(f_res);
        // No return yet - need to restore previous position
    }

    f_res = f_lseek(file, prev_pos);
    if (f_res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, f_res);
        if (ret >= 0) {
            errno = fresult_to_errno(f_res);
        } // else f_read failed so errno shouldn't be overwritten
        ret = -1; // in case the read was successful but the seek wasn't
    }

pread_release:
    _lock_release(&fat_ctx->lock);
    return ret;
}

static ssize_t vfs_fat_pwrite(void *ctx, int fd, const void *src, size_t size, off_t offset)
{
    ssize_t ret = -1;
    vfs_fat_ctx_t *fat_ctx = (vfs_fat_ctx_t *) ctx;
    _lock_acquire(&fat_ctx->lock);
    FIL *file = &fat_ctx->files[fd];
    const off_t prev_pos = f_tell(file);

    FRESULT f_res = f_lseek(file, offset);
    if (f_res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, f_res);
        errno = fresult_to_errno(f_res);
        goto pwrite_release;
    }

    unsigned wr = 0;
    f_res = f_write(file, src, size, &wr);
    if (f_res == FR_OK) {
        ret = wr;
    } else {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, f_res);
        errno = fresult_to_errno(f_res);
        // No return yet - need to restore previous position
    }

    f_res = f_lseek(file, prev_pos);
    if (f_res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, f_res);
        if (ret >= 0) {
            errno = fresult_to_errno(f_res);
        } // else f_write failed so errno shouldn't be overwritten
        ret = -1; // in case the write was successful but the seek wasn't
    }

pwrite_release:
    _lock_release(&fat_ctx->lock);
    return ret;
}

static int vfs_fat_fsync(void* ctx, int fd)
{
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    _lock_acquire(&fat_ctx->lock);
    FIL* file = &fat_ctx->files[fd];
    FRESULT res = f_sync(file);
    _lock_release(&fat_ctx->lock);
    int rc = 0;
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        rc = -1;
    }
    return rc;
}

static int vfs_fat_close(void* ctx, int fd)
{
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    _lock_acquire(&fat_ctx->lock);
    FIL* file = &fat_ctx->files[fd];
    FRESULT res = f_close(file);
    file_cleanup(fat_ctx, fd);
    _lock_release(&fat_ctx->lock);
    int rc = 0;
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        rc = -1;
    }
    return rc;
}

static off_t vfs_fat_lseek(void* ctx, int fd, off_t offset, int mode)
{
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    FIL* file = &fat_ctx->files[fd];
    off_t new_pos;
    if (mode == SEEK_SET) {
        new_pos = offset;
    } else if (mode == SEEK_CUR) {
        off_t cur_pos = f_tell(file);
        new_pos = cur_pos + offset;
    } else if (mode == SEEK_END) {
        off_t size = f_size(file);
        new_pos = size + offset;
    } else {
        errno = EINVAL;
        return -1;
    }
    FRESULT res = f_lseek(file, new_pos);
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        return -1;
    }
    return new_pos;
}

static int vfs_fat_fstat(void* ctx, int fd, struct stat * st)
{
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    FIL* file = &fat_ctx->files[fd];
    st->st_size = f_size(file);
    st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO | S_IFREG;
    st->st_mtime = 0;
    st->st_atime = 0;
    st->st_ctime = 0;
    return 0;
}

static inline mode_t get_stat_mode(bool is_dir)
{
    return S_IRWXU | S_IRWXG | S_IRWXO |
            ((is_dir) ? S_IFDIR : S_IFREG);
}

static int vfs_fat_stat(void* ctx, const char * path, struct stat * st)
{
    if (strcmp(path, "/") == 0) {
        /* FatFS f_stat function does not work for the drive root.
         * Just pretend that this is a directory.
         */
        memset(st, 0, sizeof(*st));
        st->st_mode = get_stat_mode(true);
        return 0;
    }

    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    _lock_acquire(&fat_ctx->lock);
    prepend_drive_to_path(fat_ctx, &path, NULL);
    FILINFO info;
    FRESULT res = f_stat(path, &info);
    _lock_release(&fat_ctx->lock);
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        return -1;
    }

    memset(st, 0, sizeof(*st));
    st->st_size = info.fsize;
    st->st_mode = get_stat_mode((info.fattrib & AM_DIR) != 0);
    fat_date_t fdate = { .as_int = info.fdate };
    fat_time_t ftime = { .as_int = info.ftime };
    struct tm tm = {
        .tm_mday = fdate.mday,
        .tm_mon = fdate.mon - 1,    /* unlike tm_mday, tm_mon is zero-based */
        .tm_year = fdate.year + 80,
        .tm_sec = ftime.sec * 2,
        .tm_min = ftime.min,
        .tm_hour = ftime.hour
    };
    st->st_mtime = mktime(&tm);
    st->st_atime = 0;
    st->st_ctime = 0;
    return 0;
}

static int vfs_fat_unlink(void* ctx, const char *path)
{
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    _lock_acquire(&fat_ctx->lock);
    prepend_drive_to_path(fat_ctx, &path, NULL);
    FRESULT res = f_unlink(path);
    _lock_release(&fat_ctx->lock);
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        return -1;
    }
    return 0;
}

static int vfs_fat_link(void* ctx, const char* n1, const char* n2)
{
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    _lock_acquire(&fat_ctx->lock);
    prepend_drive_to_path(fat_ctx, &n1, &n2);
    const size_t copy_buf_size = fat_ctx->fs.csize;
    FRESULT res;
    FIL* pf1 = (FIL*) ff_memalloc(sizeof(FIL));
    FIL* pf2 = (FIL*) ff_memalloc(sizeof(FIL));
    void* buf = ff_memalloc(copy_buf_size);
    if (buf == NULL || pf1 == NULL || pf2 == NULL) {
        _lock_release(&fat_ctx->lock);
        ESP_LOGD(TAG, "alloc failed, pf1=%p, pf2=%p, buf=%p", pf1, pf2, buf);
        free(pf1);
        free(pf2);
        free(buf);
        errno = ENOMEM;
        return -1;
    }
    memset(pf1, 0, sizeof(*pf1));
    memset(pf2, 0, sizeof(*pf2));
    res = f_open(pf1, n1, FA_READ | FA_OPEN_EXISTING);
    if (res != FR_OK) {
        _lock_release(&fat_ctx->lock);
        goto fail1;
    }
    res = f_open(pf2, n2, FA_WRITE | FA_CREATE_NEW);
    _lock_release(&fat_ctx->lock);
    if (res != FR_OK) {
        goto fail2;
    }
    size_t size_left = f_size(pf1);
    while (size_left > 0) {
        size_t will_copy = (size_left < copy_buf_size) ? size_left : copy_buf_size;
        size_t read;
        res = f_read(pf1, buf, will_copy, &read);
        if (res != FR_OK) {
            goto fail3;
        } else if (read != will_copy) {
            res = FR_DISK_ERR;
            goto fail3;
        }
        size_t written;
        res = f_write(pf2, buf, will_copy, &written);
        if (res != FR_OK) {
            goto fail3;
        } else if (written != will_copy) {
            res = FR_DISK_ERR;
            goto fail3;
        }
        size_left -= will_copy;
    }
fail3:
    f_close(pf2);
    free(pf2);
fail2:
    f_close(pf1);
    free(pf1);
fail1:
    free(buf);
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        return -1;
    }
    return 0;
}

static int vfs_fat_rename(void* ctx, const char *src, const char *dst)
{
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    _lock_acquire(&fat_ctx->lock);
    prepend_drive_to_path(fat_ctx, &src, &dst);
    FRESULT res = f_rename(src, dst);
    _lock_release(&fat_ctx->lock);
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        return -1;
    }
    return 0;
}

static DIR* vfs_fat_opendir(void* ctx, const char* name)
{
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    _lock_acquire(&fat_ctx->lock);
    prepend_drive_to_path(fat_ctx, &name, NULL);
    vfs_fat_dir_t* fat_dir = ff_memalloc(sizeof(vfs_fat_dir_t));
    if (!fat_dir) {
        _lock_release(&fat_ctx->lock);
        errno = ENOMEM;
        return NULL;
    }
    memset(fat_dir, 0, sizeof(*fat_dir));

    FRESULT res = f_opendir(&fat_dir->ffdir, name);
    _lock_release(&fat_ctx->lock);
    if (res != FR_OK) {
        free(fat_dir);
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        return NULL;
    }
    return (DIR*) fat_dir;
}

static int vfs_fat_closedir(void* ctx, DIR* pdir)
{
    assert(pdir);
    vfs_fat_dir_t* fat_dir = (vfs_fat_dir_t*) pdir;
    FRESULT res = f_closedir(&fat_dir->ffdir);
    free(pdir);
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        return -1;
    }
    return 0;
}

static struct dirent* vfs_fat_readdir(void* ctx, DIR* pdir)
{
    vfs_fat_dir_t* fat_dir = (vfs_fat_dir_t*) pdir;
    struct dirent* out_dirent;
    int err = vfs_fat_readdir_r(ctx, pdir, &fat_dir->cur_dirent, &out_dirent);
    if (err != 0) {
        errno = err;
        return NULL;
    }
    return out_dirent;
}

static int vfs_fat_readdir_r(void* ctx, DIR* pdir,
        struct dirent* entry, struct dirent** out_dirent)
{
    assert(pdir);
    vfs_fat_dir_t* fat_dir = (vfs_fat_dir_t*) pdir;
    FRESULT res = f_readdir(&fat_dir->ffdir, &fat_dir->filinfo);
    if (res != FR_OK) {
        *out_dirent = NULL;
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        return fresult_to_errno(res);
    }
    if (fat_dir->filinfo.fname[0] == 0) {
        // end of directory
        *out_dirent = NULL;
        return 0;
    }
    entry->d_ino = 0;
    if (fat_dir->filinfo.fattrib & AM_DIR) {
        entry->d_type = DT_DIR;
    } else {
        entry->d_type = DT_REG;
    }
    strlcpy(entry->d_name, fat_dir->filinfo.fname,
            sizeof(entry->d_name));
    fat_dir->offset++;
    *out_dirent = entry;
    return 0;
}

static long vfs_fat_telldir(void* ctx, DIR* pdir)
{
    assert(pdir);
    vfs_fat_dir_t* fat_dir = (vfs_fat_dir_t*) pdir;
    return fat_dir->offset;
}

static void vfs_fat_seekdir(void* ctx, DIR* pdir, long offset)
{
    assert(pdir);
    vfs_fat_dir_t* fat_dir = (vfs_fat_dir_t*) pdir;
    FRESULT res;
    if (offset < fat_dir->offset) {
        res = f_rewinddir(&fat_dir->ffdir);
        if (res != FR_OK) {
            ESP_LOGD(TAG, "%s: rewinddir fresult=%d", __func__, res);
            errno = fresult_to_errno(res);
            return;
        }
        fat_dir->offset = 0;
    }
    while (fat_dir->offset < offset) {
        res = f_readdir(&fat_dir->ffdir, &fat_dir->filinfo);
        if (res != FR_OK) {
            ESP_LOGD(TAG, "%s: f_readdir fresult=%d", __func__, res);
            errno = fresult_to_errno(res);
            return;
        }
        fat_dir->offset++;
    }
}

static int vfs_fat_mkdir(void* ctx, const char* name, mode_t mode)
{
    (void) mode;
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    _lock_acquire(&fat_ctx->lock);
    prepend_drive_to_path(fat_ctx, &name, NULL);
    FRESULT res = f_mkdir(name);
    _lock_release(&fat_ctx->lock);
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        return -1;
    }
    return 0;
}

static int vfs_fat_rmdir(void* ctx, const char* name)
{
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    _lock_acquire(&fat_ctx->lock);
    prepend_drive_to_path(fat_ctx, &name, NULL);
    FRESULT res = f_unlink(name);
    _lock_release(&fat_ctx->lock);
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        return -1;
    }
    return 0;
}

static int vfs_fat_access(void* ctx, const char *path, int amode)
{
    FILINFO info;
    int ret = 0;
    FRESULT res;

    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;

    _lock_acquire(&fat_ctx->lock);
    prepend_drive_to_path(fat_ctx, &path, NULL);
    res = f_stat(path, &info);
    _lock_release(&fat_ctx->lock);

    if (res == FR_OK) {
        if (((amode & W_OK) == W_OK) && ((info.fattrib & AM_RDO) == AM_RDO)) {
            ret = -1;
            errno = EACCES;
        }
        // There is no flag to test readable or executable: we assume that if
        // it exists then it is readable and executable
    } else {
        ret = -1;
        errno = ENOENT;
    }

    return ret;
}

static int vfs_fat_truncate(void* ctx, const char *path, off_t length)
{
    FRESULT res;
    FIL* file;

    int ret = 0;
    
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;

    _lock_acquire(&fat_ctx->lock);
    prepend_drive_to_path(fat_ctx, &path, NULL);

    file = (FIL*) ff_memalloc(sizeof(FIL));
    if (file == NULL) {
        _lock_release(&fat_ctx->lock);
        ESP_LOGD(TAG, "truncate alloc failed");
        errno = ENOMEM;
        ret = -1;
        goto out;
    }
    memset(file, 0, sizeof(*file));

    res = f_open(file, path, FA_WRITE);

    if (res != FR_OK) {
        _lock_release(&fat_ctx->lock);
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        ret = -1;
        goto out;
    }

    res = f_size(file);

    if (res < length) {
        _lock_release(&fat_ctx->lock);
        ESP_LOGD(TAG, "truncate does not support extending size");
        errno = EPERM;
        ret = -1;
        goto close;
    }

    res = f_lseek(file, length);
    if (res != FR_OK) {
        _lock_release(&fat_ctx->lock);
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        ret = -1;
        goto close;
    }

    res = f_truncate(file);
    _lock_release(&fat_ctx->lock);

    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        ret = -1;
    }

close:
    res = f_close(file);

    if (res != FR_OK) {
        ESP_LOGE(TAG, "closing file opened for truncate failed");
        // Overwrite previous errors, since not being able to close
        // an opened file is a more critical issue.
        errno = fresult_to_errno(res);
        ret = -1;
    }

out:
    free(file);
    return ret;
}

static int vfs_fat_utime(void *ctx, const char *path, const struct utimbuf *times)
{
    FILINFO filinfo_time;

    {
        struct tm tm_time;

        if (times) {
            localtime_r(&times->modtime, &tm_time);
        } else {
            // use current time
            struct timeval tv;
            gettimeofday(&tv, NULL);
            localtime_r(&tv.tv_sec, &tm_time);
        }

        if (tm_time.tm_year < 80) {
            // FATFS cannot handle years before 1980
            errno = EINVAL;
            return -1;
        }

        fat_date_t fdate;
        fat_time_t ftime;

        // this time transformation is esentially the reverse of the one in vfs_fat_stat()
        fdate.mday = tm_time.tm_mday;
        fdate.mon = tm_time.tm_mon + 1;     // January in fdate.mon is 1, and 0 in tm_time.tm_mon
        fdate.year = tm_time.tm_year - 80;  // tm_time.tm_year=0 is 1900, tm_time.tm_year=0 is 1980
        ftime.sec = tm_time.tm_sec / 2,     // ftime.sec counts seconds by 2
        ftime.min = tm_time.tm_min;
        ftime.hour = tm_time.tm_hour;

        filinfo_time.fdate = fdate.as_int;
        filinfo_time.ftime = ftime.as_int;
    }

    vfs_fat_ctx_t *fat_ctx = (vfs_fat_ctx_t *) ctx;
    _lock_acquire(&fat_ctx->lock);
    prepend_drive_to_path(fat_ctx, &path, NULL);
    FRESULT res = f_utime(path, &filinfo_time);
    _lock_release(&fat_ctx->lock);

    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        return -1;
    }

    return 0;
}
