/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sdkconfig.h"

static const char *TAG = "example";

// Mount path for the partition
static const char *base_path = "/spiflash";

// File name
static const char *filename = "/spiflash/hello.txt";

// Function to dump contents of a directory
static void list_dir(const char *path);

// Best effort recursive function to clean a directory
static void clean_dir(const char *path);

void app_main(void)
{
    ESP_LOGI(TAG, "Mounting FAT filesystem");

    // To mount device we need name of device partition, define base_path
    // and allow format partition in case if it is new one and was not formatted before
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = true,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
            .use_one_fat = false,
    };

    wl_handle_t wl_handle = WL_INVALID_HANDLE;

    esp_err_t err = ESP_OK;

    err = esp_vfs_fat_spiflash_mount_rw_wl(base_path, "storage", &mount_config, &wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }

    // Ensure the working directory is empty
    clean_dir(base_path);

    ESP_LOGI(TAG, "Creating a file");

    // Unlike C standard library which uses FILE*, POSIX API uses file descriptors for file operations
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0);
    if (fd < 0) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

    ESP_LOGI(TAG, "Writing to the file");
    const char *text = "Hello World!\n";
    write(fd, text, strlen(text));

    struct stat info;
    // We have to use `stat` instead of `fstat`, because `fstat` currently isn't fully supported
    if (stat(filename, &info) < 0) {
        ESP_LOGE(TAG, "Failed to stat file: %s", strerror(errno));
        close(fd);
        return;
    }

    ESP_LOGI(
        TAG,
        "File stats:\n"
        "\tFile size:                %ld bytes\n"
        "\tFile modification time:   %s",
        info.st_size,
        ctime(&info.st_mtime)
    );

    ESP_LOGI(TAG, "Wait for 3 seconds");
    sleep(3);

    ESP_LOGI(TAG, "Write more to the file");
    write(fd, text, strlen(text));

    ESP_LOGI(TAG, "Force cached data and metadata to the filesystem");
    fsync(fd);

    if (stat(filename, &info) < 0) {
        ESP_LOGE(TAG, "Failed to stat file: %s", strerror(errno));
        close(fd);
        return;
    }

    ESP_LOGI(
        TAG,
        "File stats:\n"
        "\tFile size:                %ld bytes\n"
        "\tFile modification time:   %s",
        info.st_size,
        ctime(&info.st_mtime)
    );

    ESP_LOGI(TAG, "Go to the beginning of the file");
    lseek(fd, 0, SEEK_SET);

    ESP_LOGI(TAG, "Reading from file:");

    char buf[128] = {0};

    ssize_t len = read(fd, buf, sizeof(buf) - 1);
    if (len < 0) {
        ESP_LOGE(TAG, "Failed to read file: %s", strerror(errno));
        close(fd);
        return;
    }

    printf("%.*s\n", len, buf);

    ESP_LOGI(TAG, "Closing file");
    close(fd);

    // List files in the directory
    list_dir(base_path);

    ESP_LOGI(TAG, "Creating a new directory");
    if (mkdir("/spiflash/new_dir", 0777) < 0) {
        ESP_LOGE(TAG, "Failed to create a new directory: %s", strerror(errno));
        return;
    }

    // List files in the directory
    list_dir(base_path);

    ESP_LOGI(TAG, "Rename a file");

    if (rename(filename, "/spiflash/new_dir/hello_renamed.txt") < 0) {
        ESP_LOGE(TAG, "Failed to rename file: %s", strerror(errno));
        return;
    }

    // List files in the directory
    list_dir(base_path);
    list_dir("/spiflash/new_dir");

    ESP_LOGI(TAG, "Unmounting FAT filesystem");
    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_rw_wl(base_path, wl_handle));

    ESP_LOGI(TAG, "Done");
}

void list_dir(const char *path)
{
    ESP_LOGI(TAG, "Listing files in %s:", path);

    DIR *dir = opendir(path);
    if (!dir) {
        ESP_LOGE(TAG, "Failed to open directory: %s", strerror(errno));
        return;
    }

    printf("%s:\n", path);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf(
            "    %s: %s\n",
            (entry->d_type == DT_DIR)
                ? "directory"
                : "file     ",
            entry->d_name
        );
    }

    closedir(dir);
}

void clean_dir(const char *path)
{
    ESP_LOGI(TAG, "Deleting everything in %s:", path);

    DIR *dir = opendir(path);
    if (!dir) {
        ESP_LOGE(TAG, "Failed to open directory: %s", strerror(errno));
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char full_path[64] = {0};
        snprintf(full_path, sizeof(full_path), "%.20s/%.40s", path, entry->d_name);
        if (entry->d_type == DT_DIR)
            clean_dir(full_path);
        if (remove(full_path) != 0) {
            ESP_LOGE(TAG, "Failed to remove %s: %s", full_path, strerror(errno));
        }
    }

    closedir(dir);
}
