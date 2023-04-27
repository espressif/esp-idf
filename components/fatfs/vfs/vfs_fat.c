/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/lock.h>
#include "esp_vfs_fat.h"
#include "esp_vfs.h"
#include "esp_log.h"
#include "ff.h"
#include "diskio_impl.h"

#define F_WRITE_MALLOC_ZEROING_BUF_SIZE_LIMIT 512

#ifdef CONFIG_VFS_SUPPORT_DIR
struct cached_data{
#if FF_USE_LFN
	char file_path[FILENAME_MAX+FF_LFN_BUF+1];
#else
	char file_path[FILENAME_MAX+FF_SFN_BUF+1];
#endif
	FILINFO fileinfo;
};
#endif

typedef struct {
    char fat_drive[8];  /* FAT drive name */
    char base_path[ESP_VFS_PATH_MAX];   /* base path in VFS where partition is registered */
    size_t max_files;   /* max number of simultaneously open files; size of files[] array */
    _lock_t lock;       /* guard for access to this structure */
    FATFS fs;           /* fatfs library FS structure */
    char tmp_path_buf[FILENAME_MAX+3];  /* temporary buffer used to prepend drive name to the path */
    char tmp_path_buf2[FILENAME_MAX+3]; /* as above; used in functions which take two path arguments */
    bool *o_append;  /* O_APPEND is stored here for each max_files entries (because O_APPEND is not compatible with FA_OPEN_APPEND) */
#ifdef CONFIG_VFS_SUPPORT_DIR
    char dir_path[FILENAME_MAX]; /* variable to store path of opened directory*/
    struct cached_data cached_fileinfo;
#endif
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
static int vfs_fat_fsync(void* ctx, int fd);
#ifdef CONFIG_VFS_SUPPORT_DIR
static int vfs_fat_stat(void* ctx, const char * path, struct stat * st);
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
static int vfs_fat_ftruncate(void* ctx, int fd, off_t length);
static int vfs_fat_utime(void* ctx, const char *path, const struct utimbuf *times);
#endif // CONFIG_VFS_SUPPORT_DIR
static int fresult_to_errno(FRESULT fr);

static vfs_fat_ctx_t* s_fat_ctxs[FF_VOLUMES] = { NULL };
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

static size_t find_unused_context_index(void)
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
    esp_vfs_fat_conf_t conf = {
        .base_path = base_path,
        .fat_drive = fat_drive,
        .max_files = max_files,
    };
    return esp_vfs_fat_register_cfg(&conf, out_fs);
}

esp_err_t esp_vfs_fat_register_cfg(const esp_vfs_fat_conf_t* conf, FATFS** out_fs)
{
    size_t ctx = find_context_index_by_path(conf->base_path);
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
        .fsync_p = &vfs_fat_fsync,
#ifdef CONFIG_VFS_SUPPORT_DIR
        .stat_p = &vfs_fat_stat,
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
        .ftruncate_p = &vfs_fat_ftruncate,
        .utime_p = &vfs_fat_utime,
#endif // CONFIG_VFS_SUPPORT_DIR
    };

    size_t max_files = conf->max_files;
    if (max_files < 1) {
        max_files = 1;  // ff_memalloc(max_files * sizeof(bool)) below will fail if max_files == 0
    }

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
    strlcpy(fat_ctx->fat_drive, conf->fat_drive, sizeof(fat_ctx->fat_drive) - 1);
    strlcpy(fat_ctx->base_path, conf->base_path, sizeof(fat_ctx->base_path) - 1);

    esp_err_t err = esp_vfs_register(conf->base_path, &vfs, fat_ctx);
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

esp_err_t esp_vfs_fat_info(const char* base_path,
                           uint64_t* out_total_bytes,
                           uint64_t* out_free_bytes)
{
    size_t ctx = find_context_index_by_path(base_path);
    if (ctx == FF_VOLUMES) {
        return ESP_ERR_INVALID_STATE;
    }
    char* path = s_fat_ctxs[ctx]->fat_drive;

    FATFS* fs;
    DWORD free_clusters;
    int res = f_getfree(path, &free_clusters, &fs);
    if (res != FR_OK) {
        ESP_LOGE(TAG, "Failed to get number of free clusters (%d)", res);
        errno = fresult_to_errno(res);
        return ESP_FAIL;
    }
    uint64_t total_sectors = ((uint64_t)(fs->n_fatent - 2)) * fs->csize;
    uint64_t free_sectors = ((uint64_t)free_clusters) * fs->csize;
    WORD sector_size = FF_MIN_SS; // 512
#if FF_MAX_SS != FF_MIN_SS
    sector_size = fs->ssize;
#endif

    // Assuming the total size is < 4GiB, should be true for SPI Flash
    if (out_total_bytes != NULL) {
        *out_total_bytes = total_sectors * sector_size;
    }
    if (out_free_bytes != NULL) {
        *out_free_bytes = free_sectors * sector_size;
    }
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
    } else if ((m & O_APPEND) || (m & O_CREAT)) {
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
        case FR_INT_ERR:        return EIO;
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

#ifdef CONFIG_FATFS_USE_FASTSEEK
    FIL* file = &fat_ctx->files[fd];
    //fast-seek is only allowed in read mode, since file cannot be expanded
    //to use it.
    if(!(fat_mode_conv(flags) & (FA_WRITE))) {
        DWORD *clmt_mem =  ff_memalloc(sizeof(DWORD) * CONFIG_FATFS_FAST_SEEK_BUFFER_SIZE);
        if (clmt_mem == NULL) {
            f_close(file);
            file_cleanup(fat_ctx, fd);
            _lock_release(&fat_ctx->lock);
            ESP_LOGE(TAG, "open: Failed to pre-allocate CLMT buffer for fast-seek");
            errno = ENOMEM;
            return -1;
        }

        file->cltbl = clmt_mem;
        file->cltbl[0] = CONFIG_FATFS_FAST_SEEK_BUFFER_SIZE;
        res = f_lseek(file, CREATE_LINKMAP);
        ESP_LOGD(TAG, "%s: fast-seek has: %s",
                __func__,
                (res == FR_OK) ? "activated" : "failed");
        if(res != FR_OK) {
            ESP_LOGW(TAG, "%s: fast-seek not activated reason code: %d",
                    __func__, res);
            //If linkmap creation fails, fallback to the non fast seek.
            ff_memfree(file->cltbl);
            file->cltbl = NULL;
        }
    } else {
        file->cltbl = NULL;
    }
#endif

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
    _lock_acquire(&fat_ctx->lock);
    if (fat_ctx->o_append[fd]) {
        if ((res = f_lseek(file, f_size(file))) != FR_OK) {
            ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
            errno = fresult_to_errno(res);
            _lock_release(&fat_ctx->lock);
            return -1;
        }
    }
    unsigned written = 0;
    res = f_write(file, data, size, &written);
    if (((written == 0) && (size != 0)) && (res == 0)) {
        errno = ENOSPC;
        _lock_release(&fat_ctx->lock);
        return -1;
    }
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        if (written == 0) {
            _lock_release(&fat_ctx->lock);
            return -1;
        }
    }

#if CONFIG_FATFS_IMMEDIATE_FSYNC
    if (written > 0) {
        res = f_sync(file);
        if (res != FR_OK) {
            ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
            errno = fresult_to_errno(res);
            _lock_release(&fat_ctx->lock);
            return -1;
        }
     }
#endif
    _lock_release(&fat_ctx->lock);
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
    if (((wr == 0) && (size != 0)) && (f_res == 0)) {
        errno = ENOSPC;
        return -1;
    }
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

#if CONFIG_FATFS_IMMEDIATE_FSYNC
    if (wr > 0) {
        FRESULT f_res2 = f_sync(file); // We need new result to check whether we can overwrite errno
        if (f_res2 != FR_OK) {
            ESP_LOGD(TAG, "%s: fresult=%d", __func__, f_res2);
            if (f_res == FR_OK)
                errno = fresult_to_errno(f_res2);
            ret = -1;
        }
    }
#endif

pwrite_release:
    _lock_release(&fat_ctx->lock);
    return ret;
}

static int vfs_fat_fsync(void* ctx, int fd)
{
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    FIL* file = &fat_ctx->files[fd];
    FRESULT res = f_sync(file);
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

#ifdef CONFIG_FATFS_USE_FASTSEEK
    ff_memfree(file->cltbl);
    file->cltbl = NULL;
#endif

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

#if FF_FS_EXFAT
    ESP_LOGD(TAG, "%s: offset=%ld, filesize:=%" PRIu64, __func__, new_pos, f_size(file));
#else
    ESP_LOGD(TAG, "%s: offset=%ld, filesize:=%" PRIu32, __func__, new_pos, f_size(file));
#endif
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
    memset(st, 0, sizeof(*st));
    st->st_size = f_size(file);
    st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO | S_IFREG;
    st->st_mtime = 0;
    st->st_atime = 0;
    st->st_ctime = 0;
    st->st_blksize = CONFIG_FATFS_VFS_FSTAT_BLKSIZE;
    return 0;
}

#ifdef CONFIG_VFS_SUPPORT_DIR

static inline mode_t get_stat_mode(bool is_dir)
{
    return S_IRWXU | S_IRWXG | S_IRWXO |
            ((is_dir) ? S_IFDIR : S_IFREG);
}

static void update_stat_struct(struct stat *st, FILINFO *info)
{
    memset(st, 0, sizeof(*st));
    st->st_size = info->fsize;
    st->st_mode = get_stat_mode((info->fattrib & AM_DIR) != 0);
    fat_date_t fdate = { .as_int = info->fdate };
    fat_time_t ftime = { .as_int = info->ftime };
    struct tm tm = {
        .tm_mday = fdate.mday,
        .tm_mon = fdate.mon - 1,    /* unlike tm_mday, tm_mon is zero-based */
        .tm_year = fdate.year + 80,
        .tm_sec = ftime.sec * 2,
        .tm_min = ftime.min,
        .tm_hour = ftime.hour,
        /* FAT doesn't keep track if the time was DST or not, ask the C library
         * to try to figure this out. Note that this may yield incorrect result
         * in the hour before the DST comes in effect, when the local time can't
         * be converted to UTC uniquely.
         */
        .tm_isdst = -1
    };
    st->st_mtime = mktime(&tm);
    st->st_atime = 0;
    st->st_ctime = 0;
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

    //If fileinfo is already cached by readdir for requested filename,
    //then return the same info else obtain fileinfo with f_stat function
    if (strcmp(path, fat_ctx->cached_fileinfo.file_path) == 0) {
        update_stat_struct(st, &fat_ctx->cached_fileinfo.fileinfo);
        memset(&fat_ctx->cached_fileinfo, 0 ,sizeof(FILINFO));
        return 0;
    }

    memset(&fat_ctx->cached_fileinfo, 0 ,sizeof(fat_ctx->cached_fileinfo));
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

    update_stat_struct(st, &info);
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

    FRESULT res = FR_OK;
    int ret = 0;

    FIL* pf1 = (FIL*) ff_memalloc(sizeof(FIL));
    FIL* pf2 = (FIL*) ff_memalloc(sizeof(FIL));

    const size_t copy_buf_size = fat_ctx->fs.csize;
    void* buf = ff_memalloc(copy_buf_size);
    if (buf == NULL || pf1 == NULL || pf2 == NULL) {
        ESP_LOGD(TAG, "alloc failed, pf1=%p, pf2=%p, buf=%p", pf1, pf2, buf);
        _lock_release(&fat_ctx->lock);
        errno = ENOMEM;
        ret = -1;
        goto cleanup;
    }

    memset(pf1, 0, sizeof(*pf1));
    memset(pf2, 0, sizeof(*pf2));

    res = f_open(pf1, n1, FA_READ | FA_OPEN_EXISTING);
    if (res != FR_OK) {
        _lock_release(&fat_ctx->lock);
        goto cleanup;
    }

    res = f_open(pf2, n2, FA_WRITE | FA_CREATE_NEW);

#if !CONFIG_FATFS_LINK_LOCK
    _lock_release(&fat_ctx->lock);
#endif

    if (res != FR_OK) {
        goto close_old;
    }

    size_t size_left = f_size(pf1);
    while (size_left > 0) {
        size_t will_copy = (size_left < copy_buf_size) ? size_left : copy_buf_size;
        size_t read;
        res = f_read(pf1, buf, will_copy, &read);
        if (res != FR_OK) {
            goto close_both;
        } else if (read != will_copy) {
            res = FR_DISK_ERR;
            goto close_both;
        }
        size_t written;
        res = f_write(pf2, buf, will_copy, &written);
        if (res != FR_OK) {
            goto close_both;
        } else if (written != will_copy) {
            res = FR_DISK_ERR;
            goto close_both;
        }
        size_left -= will_copy;
    }

close_both:
    f_close(pf2);

close_old:
    f_close(pf1);

#if CONFIG_FATFS_LINK_LOCK
    _lock_release(&fat_ctx->lock);
#endif

cleanup:
    free(buf);
    free(pf2);
    free(pf1);
    if (ret == 0 && res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        return -1;
    }

    return ret;
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
    strlcpy(fat_ctx->dir_path, name, FILENAME_MAX);
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
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    vfs_fat_dir_t* fat_dir = (vfs_fat_dir_t*) pdir;
    struct dirent* out_dirent;
    int err = vfs_fat_readdir_r(ctx, pdir, &fat_dir->cur_dirent, &out_dirent);
    if (err != 0) {
        errno = err;
        return NULL;
    }

    //Store the FILEINFO in the cached_fileinfo. If the stat function is invoked immediately afterward,
    //the cached_fileinfo will provide the FILEINFO directly, as it was already obtained during the readdir operation.
    //During directory size calculation, this optimization can reduce the computation time.
    memset(&fat_ctx->cached_fileinfo, 0 ,sizeof(fat_ctx->cached_fileinfo));
    if (strcmp(fat_ctx->dir_path, "/") == 0) {
        snprintf(fat_ctx->cached_fileinfo.file_path, sizeof(fat_ctx->cached_fileinfo.file_path),
                 "/%s", fat_dir->filinfo.fname);
    } else {
        snprintf(fat_ctx->cached_fileinfo.file_path, sizeof(fat_ctx->cached_fileinfo.file_path),
                 "%s/%s", fat_ctx->dir_path, fat_dir->filinfo.fname);
    }
    fat_ctx->cached_fileinfo.fileinfo = fat_dir->filinfo;
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
    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;
    vfs_fat_dir_t* fat_dir = (vfs_fat_dir_t*) pdir;
    FRESULT res;
    _lock_acquire(&fat_ctx->lock);
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
    _lock_release(&fat_ctx->lock);
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
        errno = fresult_to_errno(res);
    }

    return ret;
}

static FRESULT f_write_zero_mem(FIL* fp, FSIZE_t data_size, FSIZE_t buf_size, UINT* bytes_written)
{
    if (fp == NULL || data_size <= 0 || buf_size <= 0) {
        return FR_INVALID_PARAMETER;
    }

    void* buf = ff_memalloc(buf_size);
    if (buf == NULL) {
        return FR_DISK_ERR;
    }
    memset(buf, 0, buf_size);

    FRESULT res = FR_OK;
    UINT bw = 0;
    FSIZE_t i = 0;
    if (bytes_written != NULL) {
        *bytes_written = 0;
    }

    if (data_size > buf_size) { // prevent unsigned underflow
        for (; i < (data_size - buf_size); i += buf_size) { // write chunks of buf_size
            res = f_write(fp, buf, (UINT) buf_size, &bw);
            if (res != FR_OK) {
                goto out;
            }
            if (bytes_written != NULL) {
                *bytes_written += bw;
            }
        }
    }

    if (i < data_size) { // write the remaining data
        res = f_write(fp, buf, (UINT) (data_size - i), &bw);
        if (res == FR_OK && bytes_written != NULL) {
            *bytes_written += bw;
        }
    }

out:
    ff_memfree(buf);
    return res;
}

static int vfs_fat_truncate(void* ctx, const char *path, off_t length)
{
    FRESULT res;
    FIL* file = NULL;

    int ret = 0;

    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;

    if (length < 0) {
        errno = EINVAL;
        ret = -1;
        goto out;
    }

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

    FSIZE_t seek_ptr_pos = (FSIZE_t) f_tell(file); // current seek pointer position
    FSIZE_t sz = (FSIZE_t) f_size(file); // current file size (end of file position)

    res = f_lseek(file, length);
    if (res != FR_OK || f_tell(file) != length) {
        goto lseek_or_write_fail;
    }

    if (sz < length) {
        res = f_lseek(file, sz); // go to the previous end of file
        if (res != FR_OK) {
            goto lseek_or_write_fail;
        }

        FSIZE_t new_free_space = ((FSIZE_t) length) - sz;
        UINT written;

        if (new_free_space > UINT32_MAX) {
            _lock_release(&fat_ctx->lock);
            ESP_LOGE(TAG, "%s: Cannot extend the file more than 4GB at once", __func__);
            ret = -1;
            goto close;
        }

        FSIZE_t buf_size_limit = F_WRITE_MALLOC_ZEROING_BUF_SIZE_LIMIT;
        FSIZE_t buf_size = new_free_space < buf_size_limit ? new_free_space : buf_size_limit;
        res = f_write_zero_mem(file, new_free_space, buf_size, &written);

        if (res != FR_OK) {
            goto lseek_or_write_fail;
        } else if (written != (UINT) new_free_space) {
            res = FR_DISK_ERR;
            goto lseek_or_write_fail;
        }

        res = f_lseek(file, seek_ptr_pos); // return to the original position
        if (res != FR_OK) {
            goto lseek_or_write_fail;
        }
    } else {
        res = f_truncate(file);

        if (res != FR_OK) {
            _lock_release(&fat_ctx->lock);
            ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
            errno = fresult_to_errno(res);
            ret = -1;
            goto close;
        }
    }

#if CONFIG_FATFS_IMMEDIATE_FSYNC
    res = f_sync(file);
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        ret = -1;
    }
#endif

    _lock_release(&fat_ctx->lock);

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

lseek_or_write_fail:
    _lock_release(&fat_ctx->lock);
    ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
    errno = fresult_to_errno(res);
    ret = -1;
    goto close;
}

static int vfs_fat_ftruncate(void* ctx, int fd, off_t length)
{
    FRESULT res;
    FIL* file = NULL;

    int ret = 0;

    vfs_fat_ctx_t* fat_ctx = (vfs_fat_ctx_t*) ctx;

    if (length < 0) {
        errno = EINVAL;
        ret = -1;
        return ret;
    }

    _lock_acquire(&fat_ctx->lock);
    file = &fat_ctx->files[fd];
    if (file == NULL) {
        ESP_LOGD(TAG, "ftruncate NULL file pointer");
        errno = EINVAL;
        ret = -1;
        goto out;
    }

    FSIZE_t seek_ptr_pos = (FSIZE_t) f_tell(file); // current seek pointer position
    FSIZE_t sz = (FSIZE_t) f_size(file); // current file size (end of file position)

    res = f_lseek(file, length);
    if (res != FR_OK || f_tell(file) != length) {
        goto fail;
    }

    if (sz < length) {
        res = f_lseek(file, sz); // go to the previous end of file
        if (res != FR_OK) {
            goto fail;
        }

        FSIZE_t new_free_space = ((FSIZE_t) length) - sz;
        UINT written;

        if (new_free_space > UINT32_MAX) {
            ESP_LOGE(TAG, "%s: Cannot extend the file more than 4GB at once", __func__);
            ret = -1;
            goto out;
        }

        FSIZE_t buf_size_limit = F_WRITE_MALLOC_ZEROING_BUF_SIZE_LIMIT;
        FSIZE_t buf_size = new_free_space < buf_size_limit ? new_free_space : buf_size_limit;
        res = f_write_zero_mem(file, new_free_space, buf_size, &written);

        if (res != FR_OK) {
            goto fail;
        } else if (written != (UINT) new_free_space) {
            res = FR_DISK_ERR;
            goto fail;
        }

        res = f_lseek(file, seek_ptr_pos); // return to the original position
        if (res != FR_OK) {
            goto fail;
        }
    } else {
        res = f_truncate(file);

        if (res != FR_OK) {
            goto fail;
        }
    }

#if CONFIG_FATFS_IMMEDIATE_FSYNC
    res = f_sync(file);
    if (res != FR_OK) {
        ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
        errno = fresult_to_errno(res);
        ret = -1;
    }
#endif

out:
    _lock_release(&fat_ctx->lock);
    return ret;

fail:
    ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
    errno = fresult_to_errno(res);
    ret = -1;
    goto out;
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

        // this time transformation is essentially the reverse of the one in vfs_fat_stat()
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

#endif // CONFIG_VFS_SUPPORT_DIR

esp_err_t esp_vfs_fat_create_contiguous_file(const char* base_path, const char* full_path, uint64_t size, bool alloc_now)
{
    if (base_path == NULL || full_path == NULL || size <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t ctx = find_context_index_by_path(base_path);
    if (ctx == FF_VOLUMES) {
        return ESP_ERR_INVALID_STATE;
    }
    vfs_fat_ctx_t* fat_ctx = s_fat_ctxs[ctx];

    _lock_acquire(&fat_ctx->lock);
    const char* file_path = full_path + strlen(base_path); // shift the pointer and omit the base_path
    prepend_drive_to_path(fat_ctx, &file_path, NULL);

    FIL* file = (FIL*) ff_memalloc(sizeof(FIL));
    if (file == NULL) {
        _lock_release(&fat_ctx->lock);
        ESP_LOGD(TAG, "esp_vfs_fat_create_contiguous_file alloc failed");
        errno = ENOMEM;
        return -1;
    }
    memset(file, 0, sizeof(*file));

    FRESULT res = f_open(file, file_path, FA_WRITE | FA_OPEN_ALWAYS);
    if (res != FR_OK) {
        goto fail;
    }

    res = f_expand(file, size, alloc_now ? 1 : 0);
    if (res != FR_OK) {
        f_close(file);
        goto fail;
    }

    res = f_close(file);
    if (res != FR_OK) {
        goto fail;
    }

    _lock_release(&fat_ctx->lock);
    free(file);

    return 0;
fail:
    _lock_release(&fat_ctx->lock);
    free(file);
    ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
    errno = fresult_to_errno(res);
    return -1;
}

static FRESULT test_contiguous_file( // From FATFS examples
    FIL* fp,    /* [IN]  Open file object to be checked */
    int* cont   /* [OUT] 1:Contiguous, 0:Fragmented or zero-length */
) {
    DWORD clst, clsz, step;
    FSIZE_t fsz;
    FRESULT fr;

    *cont = 0;
    fr = f_rewind(fp);              /* Validates and prepares the file */
    if (fr != FR_OK) return fr;

#if FF_MAX_SS == FF_MIN_SS
    clsz = (DWORD)fp->obj.fs->csize * FF_MAX_SS;    /* Cluster size */
#else
    clsz = (DWORD)fp->obj.fs->csize * fp->obj.fs->ssize;
#endif
    fsz = f_size(fp);
    if (fsz > 0) {
        clst = fp->obj.sclust - 1;  /* A cluster leading the first cluster for first test */
        while (fsz) {
            step = (fsz >= clsz) ? clsz : (DWORD)fsz;
            fr = f_lseek(fp, f_tell(fp) + step);    /* Advances file pointer a cluster */
            if (fr != FR_OK) return fr;
            if (clst + 1 != fp->clust) break;       /* Is not the cluster next to previous one? */
            clst = fp->clust; fsz -= step;          /* Get current cluster for next test */
        }
        if (fsz == 0) *cont = 1;    /* All done without fail? */
    }

    return FR_OK;
}

esp_err_t esp_vfs_fat_test_contiguous_file(const char* base_path, const char* full_path, bool* is_contiguous)
{
    if (base_path == NULL || full_path == NULL || is_contiguous == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t ctx = find_context_index_by_path(base_path);
    if (ctx == FF_VOLUMES) {
        return ESP_ERR_INVALID_STATE;
    }
    vfs_fat_ctx_t* fat_ctx = s_fat_ctxs[ctx];

    _lock_acquire(&fat_ctx->lock);
    const char* file_path = full_path + strlen(base_path); // shift the pointer and omit the base_path
    prepend_drive_to_path(fat_ctx, &file_path, NULL);

    FIL* file = (FIL*) ff_memalloc(sizeof(FIL));
    if (file == NULL) {
        _lock_release(&fat_ctx->lock);
        ESP_LOGD(TAG, "esp_vfs_fat_test_contiguous_file alloc failed");
        errno = ENOMEM;
        return -1;
    }
    memset(file, 0, sizeof(*file));

    FRESULT res = f_open(file, file_path, FA_WRITE | FA_OPEN_ALWAYS);
    if (res != FR_OK) {
        goto fail;
    }

    res = test_contiguous_file(file, (int*) is_contiguous);
    if (res != FR_OK) {
        f_close(file);
        goto fail;
    }

    res = f_close(file);
    if (res != FR_OK) {
        goto fail;
    }

    _lock_release(&fat_ctx->lock);
    free(file);

    return 0;
fail:
    _lock_release(&fat_ctx->lock);
    free(file);
    ESP_LOGD(TAG, "%s: fresult=%d", __func__, res);
    errno = fresult_to_errno(res);
    return -1;
}
