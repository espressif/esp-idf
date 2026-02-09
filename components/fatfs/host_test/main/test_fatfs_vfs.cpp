/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "ff.h"
#include "esp_partition.h"
#include "wear_levelling.h"
#include "diskio_impl.h"
#include "diskio_wl.h"
#include "esp_vfs_fat.h"
#include "esp_vfs.h"

#include <catch2/catch_test_macros.hpp>

static const char* TAG = "linux_vfs";

static BYTE pdrv;
static wl_handle_t wl_handle;
static char drv[3];

static void test_setup(void)
{
    FATFS *fs;
    esp_err_t ret = ESP_OK;
    FRESULT fr_result;

    esp_err_t esp_result;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "storage");
    REQUIRE(partition != NULL);

    // Mount wear-levelled partition
    esp_result = wl_mount(partition, &wl_handle);
    REQUIRE(esp_result == ESP_OK);

    // Get a physical drive
    esp_result = ff_diskio_get_drive(&pdrv);
    REQUIRE(esp_result == ESP_OK);

    // Register physical drive as wear-levelled partition
    esp_result = ff_diskio_register_wl_partition(pdrv, wl_handle);

    char temp_drv[3] = {(char)('0' + pdrv), ':', 0};
    strncpy(drv, temp_drv, 3);

    // Create FAT volume on the entire disk
    LBA_t part_list[] = {100, 0, 0, 0};
    BYTE workbuf[FF_MAX_SS];
    const size_t workbuf_size = sizeof(workbuf);

    fr_result = f_fdisk(pdrv, part_list, workbuf);
    REQUIRE(fr_result == FR_OK);
    const MKFS_PARM opt = {(BYTE)FM_ANY, 0, 0, 0, 0};
    fr_result = f_mkfs("", &opt, workbuf, workbuf_size); // Use default volume
    REQUIRE(fr_result == FR_OK);

    esp_vfs_fat_conf_t conf = {
        .base_path = "/linux",
        .fat_drive = drv,
        .max_files = 5,
    };
    ret = esp_vfs_fat_register_cfg(&conf, &fs);
    if (ret == ESP_ERR_INVALID_STATE) {
        // it's okay, already registered with VFS
    } else if (ret != ESP_OK) {
        ESP_LOGD(TAG, "esp_vfs_fat_register_cfg failed 0x(%x)", ret);
    }
    REQUIRE(ret == ESP_OK);

    //Mount the volume
    fr_result = f_mount(fs, "", 0);
    REQUIRE(fr_result == FR_OK);
}

static void test_teardown(void)
{
    esp_err_t esp_result;
    FRESULT fr_result;
    // Unmount default volume
    fr_result = f_mount(0, drv, 0);
    REQUIRE(fr_result == FR_OK);

    ff_diskio_unregister(pdrv);
    ff_diskio_clear_pdrv_wl(wl_handle);
    esp_result = wl_unmount(wl_handle);
    esp_vfs_fat_unregister_path("/linux");
    REQUIRE(esp_result == ESP_OK);
}

static void test_fatfs_create_file_with_text(const char* name, const char* text)
{
    int fd = -1;
    fd = open(name, O_CREAT|O_RDWR, 0777);
    REQUIRE(fd != -1);

    ssize_t sz = write(fd, text, strlen(text));
    REQUIRE(sz == strlen(text));

    REQUIRE(0 == close(fd));
}

static void test_open_read_write_file()
{
    constexpr const char *test_str = "0123456789\n";
    const char *filename = "/linux/test.txt";
    test_fatfs_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);

    char data[strlen(test_str)];
    size_t sz = read(fd, data, strlen(test_str));
    REQUIRE(sz == strlen(test_str));
    REQUIRE(0 == strncmp(data, test_str, strlen(test_str)));

    close(fd);
}

TEST_CASE("open file, write and read back data via VFS", "[fatfs]")
{
    test_setup();
    test_open_read_write_file();
    test_teardown();
}

static void test_lseek()
{
    const char *test_str = "0123456789\n";
    const char *filename = "/linux/lseek.txt";
    test_fatfs_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);

    off_t off = lseek(fd, 6, SEEK_CUR);
    REQUIRE(off == 6);
    off = lseek(fd, 3, SEEK_SET);
    REQUIRE(off == 3);
    off = lseek(fd, -9, SEEK_END);
    REQUIRE(off == 2);

    close(fd);
}

TEST_CASE("lseek via VFS", "[fatfs]")
{
    test_setup();
    test_lseek();
    test_teardown();
}

static void test_pread()
{
    char buf[32] = { 0 };
    const char *test_str = "0123456789\n";
    const char *filename = "/linux/pread.txt";
    test_fatfs_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);

    int r = pread(fd, buf, sizeof(buf), 0); // it is a regular read() with offset==0
    REQUIRE(0 == strcmp(test_str, buf));
    REQUIRE(strlen(test_str) == r);

    memset(buf, 0, sizeof(buf));
    r = pread(fd, buf, sizeof(buf), 1); // offset==1
    REQUIRE(0 == strcmp(test_str + 1, buf));
    REQUIRE(strlen(test_str) - 1 == r);

    memset(buf, 0, sizeof(buf));
    r = pread(fd, buf, sizeof(buf), 5); // offset==5
    REQUIRE(0 == strcmp(test_str + 5, buf));
    REQUIRE(strlen(test_str) - 5 == r);

    close(fd);
}

TEST_CASE("can read file with pread", "[fatfs]")
{
    test_setup();
    test_pread();
    test_teardown();
}

static void test_pwrite()
{
    const char *test_str = "0123456789\n";
    const char *msg = "ESP";
    const char *filename = "/linux/pwrite.txt";
    test_fatfs_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);

    const off_t current_pos = lseek(fd, 0, SEEK_END); // O_APPEND is not the same - jumps to the end only before write()

    const int r = pwrite(fd, msg, strlen(msg), 0);
    REQUIRE(strlen(msg) == r);

    REQUIRE(current_pos == lseek(fd, 0, SEEK_CUR)); // pwrite should not move the pointer

    close(fd);
}

TEST_CASE("pwrite() works well", "[fatfs]")
{
    test_setup();
    test_pwrite();
    test_teardown();
}

static void test_link_unlink_rename()
{
    const char *filename_prefix = "/linux/link";

    char name_copy[64];
    char name_dst[64];
    char name_src[64];
    snprintf(name_copy, sizeof(name_copy), "%s_cpy.txt", filename_prefix);
    snprintf(name_dst, sizeof(name_dst), "%s_dst.txt", filename_prefix);
    snprintf(name_src, sizeof(name_src), "%s_src.txt", filename_prefix);

    unlink(name_copy);
    unlink(name_dst);
    unlink(name_src);

    const char *test_str = "0123456789\n";
    test_fatfs_create_file_with_text(name_src, test_str);

    REQUIRE(0 == link(name_src, name_copy));
    REQUIRE(-1 != open(name_copy, O_RDONLY));

    test_fatfs_create_file_with_text(name_copy, test_str);
    rename(name_copy, name_dst);
    REQUIRE(-1 == open(name_copy, O_RDONLY));
    REQUIRE(-1 != open(name_dst, O_RDONLY));

    unlink(name_dst);
    unlink(name_src);

    REQUIRE(-1 == open(name_src, O_RDONLY));
    REQUIRE(-1 == open(name_dst, O_RDONLY));
}

TEST_CASE("link copies a file, unlink removes file, rename moves a file", "[fatfs]")
{
    test_setup();
    test_link_unlink_rename();
    test_teardown();
}

static void test_opendir_closedir_readdir()
{
    const char *dirname = "/linux/dir";

    REQUIRE(0 == mkdir(dirname, 0755));

    char name_dir_file[64];
    const char * file_name = "test_opd.txt";
    snprintf(name_dir_file, sizeof(name_dir_file), "%s/%s", dirname, file_name);
    unlink(name_dir_file);
    test_fatfs_create_file_with_text(name_dir_file, "test_opendir\n");
    DIR* dir = opendir(dirname);
    REQUIRE(dir != NULL);
    bool found = false;
    while (true) {
        struct dirent* de = readdir(dir);
        if (!de) {
            break;
        }
        if (strcasecmp(de->d_name, file_name) == 0) {
            found = true;
            break;
        }
    }
    REQUIRE(found == true);
    REQUIRE(0 == closedir(dir));
    unlink(name_dir_file);
}

void test_fatfs_opendir_readdir_rewinddir(const char* dir_prefix)
{
    char name_dir_inner_file[64];
    char name_dir_inner[64];
    char name_dir_file3[64];
    char name_dir_file2[64];
    char name_dir_file1[64];

    snprintf(name_dir_inner_file, sizeof(name_dir_inner_file), "%s/inner/3.txt", dir_prefix);
    snprintf(name_dir_inner, sizeof(name_dir_inner), "%s/inner", dir_prefix);
    snprintf(name_dir_file3, sizeof(name_dir_file2), "%s/boo.bin", dir_prefix);
    snprintf(name_dir_file2, sizeof(name_dir_file2), "%s/2.txt", dir_prefix);
    snprintf(name_dir_file1, sizeof(name_dir_file1), "%s/1.txt", dir_prefix);

    unlink(name_dir_inner_file);
    rmdir(name_dir_inner);
    unlink(name_dir_file1);
    unlink(name_dir_file2);
    unlink(name_dir_file3);
    rmdir(dir_prefix);

    REQUIRE(0 == mkdir(dir_prefix, 0755));
    test_fatfs_create_file_with_text(name_dir_file1, "1\n");
    test_fatfs_create_file_with_text(name_dir_file2, "2\n");
    test_fatfs_create_file_with_text(name_dir_file3, "\01\02\03");
    REQUIRE(0 == mkdir(name_dir_inner, 0755));
    test_fatfs_create_file_with_text(name_dir_inner_file, "3\n");

    DIR* dir = opendir(dir_prefix);
    REQUIRE(dir != NULL);
    int count = 0;
    const char* names[4];
    while(count < 4) {
        struct dirent* de = readdir(dir);
        if (!de) {
            break;
        }
        printf("found '%s'\n", de->d_name);
        if (strcasecmp(de->d_name, "1.txt") == 0) {
            REQUIRE(de->d_type == DT_REG);
            names[count] = "1.txt";
            ++count;
        } else if (strcasecmp(de->d_name, "2.txt") == 0) {
            REQUIRE(de->d_type == DT_REG);
            names[count] = "2.txt";
            ++count;
        } else if (strcasecmp(de->d_name, "inner") == 0) {
            REQUIRE(de->d_type == DT_DIR);
            names[count] = "inner";
            ++count;
        } else if (strcasecmp(de->d_name, "boo.bin") == 0) {
            REQUIRE(de->d_type == DT_REG);
            names[count] = "boo.bin";
            ++count;
        } else {
            printf("unexpected directory entry");
        }
    }
    REQUIRE(count == 4);

    rewinddir(dir);
    struct dirent* de = readdir(dir);
    REQUIRE(de != NULL);
    REQUIRE(0 == strcasecmp(de->d_name, names[0]));
    seekdir(dir, 3);
    de = readdir(dir);
    REQUIRE(de != NULL);
    REQUIRE(0 == strcasecmp(de->d_name, names[3]));
    seekdir(dir, 1);
    de = readdir(dir);
    REQUIRE(de != NULL);
    REQUIRE(0 == strcasecmp(de->d_name, names[1]));
    seekdir(dir, 2);
    de = readdir(dir);
    REQUIRE(de != NULL);
    REQUIRE(0 == strcasecmp(de->d_name, names[2]));

    REQUIRE(0 == closedir(dir));
}

TEST_CASE("opendir, readdir, rewinddir, seekdir work as expected", "[fatfs]")
{
    test_setup();
    test_fatfs_opendir_readdir_rewinddir("/linux/dir");
    test_teardown();
}

TEST_CASE("can opendir, closedir and readdir of FS", "[fatfs]")
{
    test_setup();
    test_opendir_closedir_readdir();
    test_teardown();
}

static void test_truncate()
{
    const char *test_str = "0123456789\n";
    const char *filename = "/linux/truncate.txt";
    test_fatfs_create_file_with_text(filename, test_str);

    struct stat st;
    size_t size;
    REQUIRE(0 == stat(filename, &st));
    size = st.st_size;
    REQUIRE(strlen(test_str) == size);

    size_t trunc_add = 2;
    off_t new_size = strlen(test_str) + trunc_add;
    REQUIRE(0 == truncate(filename, new_size));
    stat(filename, &st);
    size = st.st_size;
    REQUIRE(new_size == size);

    const char truncated_1[] = "01234";
    off_t truncated_len = strlen(truncated_1);

    REQUIRE(0 == truncate(filename, truncated_len));

    stat(filename, &st);
    size = st.st_size;
    REQUIRE(strlen(truncated_1) == size);
}

TEST_CASE("can truncate", "[fatfs]")
{
    test_setup();
    test_truncate();
    test_teardown();
}

static void test_ftruncate()
{
    const char *test_str = "0123456789\n";
    const char *filename = "/linux/ftrunc.txt";
    test_fatfs_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);
    struct stat st;
    size_t size;
    fstat(fd, &st);
    size = st.st_size;
    REQUIRE(strlen(test_str) == size);
    close(fd);

    size_t trunc_add = 2;
    off_t new_size = strlen(test_str) + trunc_add;
    fd = open(filename, O_RDWR);
    REQUIRE(0 == ftruncate(fd, new_size));
    fstat(fd, &st);
    size = st.st_size;
    REQUIRE(new_size == size);
    close(fd);

    const char truncated_1[] = "01234";
    off_t truncated_len = strlen(truncated_1);

    fd = open(filename, O_RDWR);
    REQUIRE(0 == ftruncate(fd, truncated_len));
    fstat(fd, &st);
    size = st.st_size;
    REQUIRE(strlen(truncated_1) == size);
    close(fd);
}

TEST_CASE("can ftruncate", "[fatfs]")
{
    test_setup();
    test_ftruncate();
    test_teardown();
}

void test_fatfs_utime(const char* filename, const char* root_dir)
{
    struct stat achieved_stat;
    struct tm desired_tm;
    struct utimbuf desired_time = {
        .actime = 0, // access time is not supported
        .modtime = 0,
    };
    time_t false_now = 0;
    memset(&desired_tm, 0, sizeof(struct tm));

    // Setting up a false actual time - used when the file is created and for modification with the current time
    desired_tm.tm_mon = 10 - 1;
    desired_tm.tm_mday = 31;
    desired_tm.tm_year = 2018 - 1900;
    desired_tm.tm_hour = 10;
    desired_tm.tm_min = 35;
    desired_tm.tm_sec = 23;

    false_now = mktime(&desired_tm);

    struct timeval now = { .tv_sec = false_now, .tv_usec = 0};
    settimeofday(&now, NULL);

    test_fatfs_create_file_with_text(filename, "");

    // 00:00:00. January 1st, 1980 - FATFS cannot handle earlier dates
    desired_tm.tm_mon = 1 - 1;
    desired_tm.tm_mday = 1;
    desired_tm.tm_year = 1980 - 1900;
    desired_tm.tm_hour = 0;
    desired_tm.tm_min = 0;
    desired_tm.tm_sec = 0;
    printf("Testing mod. time: %s", asctime(&desired_tm));
    desired_time.modtime = mktime(&desired_tm);
    REQUIRE(0 == utime(filename, &desired_time));
    REQUIRE(0 == stat(filename, &achieved_stat));
    REQUIRE(desired_time.modtime == achieved_stat.st_mtime);

    // current time
    REQUIRE(0 == utime(filename, NULL));
    REQUIRE(0 == stat(filename, &achieved_stat));
    printf("Mod. time changed to (false actual time): %s", ctime(&achieved_stat.st_mtime));
    REQUIRE(desired_time.modtime != achieved_stat.st_mtime);
    REQUIRE(false_now - achieved_stat.st_mtime <= 2); // two seconds of tolerance are given

    // 23:59:08. December 31st, 2037
    desired_tm.tm_mon = 12 - 1;
    desired_tm.tm_mday = 31;
    desired_tm.tm_year = 2037 - 1900;
    desired_tm.tm_hour = 23;
    desired_tm.tm_min = 59;
    desired_tm.tm_sec = 8;
    printf("Testing mod. time: %s", asctime(&desired_tm));
    desired_time.modtime = mktime(&desired_tm);
    REQUIRE(0 == utime(filename, &desired_time));
    REQUIRE(0 == stat(filename, &achieved_stat));
    REQUIRE(desired_time.modtime == achieved_stat.st_mtime);

    //WARNING: it has the Unix Millennium bug (Y2K38)

    // 00:00:00. January 1st, 1970 - FATFS cannot handle years before 1980
    desired_tm.tm_mon = 1 - 1;
    desired_tm.tm_mday = 1;
    desired_tm.tm_year = 1970 - 1900;
    desired_tm.tm_hour = 0;
    desired_tm.tm_min = 0;
    desired_tm.tm_sec = 0;
    printf("Testing mod. time: %s", asctime(&desired_tm));
    desired_time.modtime = mktime(&desired_tm);
    REQUIRE(-1 == utime(filename, &desired_time));
    REQUIRE(EINVAL == errno);
}

TEST_CASE("utime sets modification time", "[fatfs]")
{
    test_setup();
    test_fatfs_utime("/linux/utime.txt", "/linux");
    test_teardown();
}

static void test_fstat()
{
    const char *test_str = "0123456789\n";
    const char *filename = "/linux/pwrite.txt";
    test_fatfs_create_file_with_text(filename, test_str);

    int fd = open(filename, O_RDWR);

    struct stat st;
    REQUIRE(0 == fstat(fd, &st));

    REQUIRE((st.st_mode & S_IFREG) == S_IFREG);
    REQUIRE(st.st_size == strlen(test_str));

    close(fd);
}

TEST_CASE("fstat returns correct values", "[fatfs]")
{
    test_setup();
    test_fstat();
    test_teardown();
}

static void test_mkdir_rmdir()
{
    const char *dir_prefix = "/linux/dir";

    REQUIRE(0 == mkdir(dir_prefix, 0777));

    struct stat st;
    REQUIRE(0 == stat(dir_prefix, &st));
    REQUIRE((st.st_mode & S_IFDIR) == S_IFDIR);
    REQUIRE((st.st_mode & S_IFREG) != S_IFREG);
    REQUIRE(0 == rmdir(dir_prefix));
    REQUIRE(-1 == stat(dir_prefix, &st));
}

TEST_CASE("can create and remove directories", "[fatfs]")
{
    test_setup();
    test_mkdir_rmdir();
    test_teardown();
}
