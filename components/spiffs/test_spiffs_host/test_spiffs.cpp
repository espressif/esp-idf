#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>

#include "esp_partition.h"
#include "spiffs.h"
#include "spiffs_nucleus.h"
#include "spiffs_api.h"

#include "catch.hpp"

extern "C" void _spi_flash_init(const char* chip_size, size_t block_size, size_t sector_size, size_t page_size, const char* partition_bin);

static void init_spiffs(spiffs *fs, uint32_t max_files)
{
    spiffs_config cfg;
    s32_t spiffs_res;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "storage");
    REQUIRE(partition);

    // Configure objects needed by SPIFFS
    esp_spiffs_t *user_data = (esp_spiffs_t*) calloc(1, sizeof(*user_data));
    user_data->partition = partition;
    fs->user_data = (void*)user_data;

    cfg.hal_erase_f = spiffs_api_erase;
    cfg.hal_read_f = spiffs_api_read;
    cfg.hal_write_f = spiffs_api_write;
    cfg.log_block_size = CONFIG_WL_SECTOR_SIZE;
    cfg.log_page_size = CONFIG_SPIFFS_PAGE_SIZE;
    cfg.phys_addr = 0;
    cfg.phys_erase_block = CONFIG_WL_SECTOR_SIZE;
    cfg.phys_size = partition->size;

    uint32_t work_sz = cfg.log_page_size * 2;
    uint8_t *work = (uint8_t*) malloc(work_sz);

    uint32_t fds_sz = max_files * sizeof(spiffs_fd);
    uint8_t *fds = (uint8_t*) malloc(fds_sz);

#if CONFIG_SPIFFS_CACHE
    uint32_t cache_sz = sizeof(spiffs_cache) + max_files * (sizeof(spiffs_cache_page)
                          + cfg.log_page_size);
    uint8_t *cache = (uint8_t*) malloc(cache_sz);
#else
    uint32_t cache_sz = 0;
    uint8_t cache = NULL;
#endif

    // Special mounting procedure: mount, format, mount as per
    // https://github.com/pellepl/spiffs/wiki/Using-spiffs
    spiffs_res = SPIFFS_mount(fs, &cfg, work, fds, fds_sz,
                            cache, cache_sz, spiffs_api_check);

    if (spiffs_res == SPIFFS_ERR_NOT_A_FS) {
        spiffs_res = SPIFFS_format(fs);
        REQUIRE(spiffs_res >= SPIFFS_OK);

        spiffs_res = SPIFFS_mount(fs, &cfg, work, fds, fds_sz,
                                cache, cache_sz, spiffs_api_check);
    }

    REQUIRE(spiffs_res >= SPIFFS_OK);
}

static void deinit_spiffs(spiffs *fs)
{
    SPIFFS_unmount(fs);

    free(fs->work);
    free(fs->user_data);
    free(fs->fd_space);

#if CONFIG_SPIFFS_CACHE
    free(fs->cache);
#endif
}

static void check_spiffs_files(spiffs *fs, const char *base_path, char* cur_path)
{
    DIR *dir;
    struct dirent *entry;
    size_t len = strlen(cur_path);

    if (len == 0) {
        strcpy(cur_path, base_path);
        len = strlen(base_path);
    }

    dir = opendir(cur_path);
    REQUIRE(dir != 0);

    while ((entry = readdir(dir)) != NULL) {
        char *name = entry->d_name;

        char path[PATH_MAX] = { 0 };

        // Read the file from host FS
        strcpy(path, cur_path);
        strcat(path, "/");
        strcat(path, name);

        struct stat sb;
        stat(path, &sb);

        if (S_ISDIR(sb.st_mode)) {
            if (!strcmp(name, ".") || !strcmp(name, ".."))
                continue;
            cur_path[len] = '/';
            strcpy(cur_path + len + 1, name);
            check_spiffs_files(fs, base_path, cur_path);
            cur_path[len] = '\0';
        } else {
            FILE* f = fopen(path , "r");
            REQUIRE(f);
            fseek(f, 0, SEEK_END);
            long sz = ftell(f);
            fseek(f, 0, SEEK_SET);

            char *f_contents = (char*) malloc(sz);
            fread(f_contents, 1, sz, f);
            fclose(f);

            s32_t spiffs_res;

            // Read the file from SPIFFS
            char *spiffs_path = path + strlen(base_path);
            spiffs_res = SPIFFS_open(fs, spiffs_path, SPIFFS_RDONLY, 0);

            REQUIRE(spiffs_res > SPIFFS_OK);

            spiffs_file fd = spiffs_res;

            spiffs_stat stat;
            spiffs_res = SPIFFS_stat(fs, spiffs_path, &stat);

            char *spiffs_f_contents = (char*) malloc(stat.size);
            spiffs_res = SPIFFS_read(fs, fd, spiffs_f_contents, stat.size);
            REQUIRE(spiffs_res == stat.size);

            // Compare the contents
            REQUIRE(sz == stat.size);

            bool same = memcmp(f_contents, spiffs_f_contents, sz) == 0;
            REQUIRE(same);

            free(f_contents);
            free(spiffs_f_contents);
        }
    }
    closedir(dir);
}

TEST_CASE("format disk, open file, write and read file", "[spiffs]")
{
    _spi_flash_init(CONFIG_ESPTOOLPY_FLASHSIZE, CONFIG_WL_SECTOR_SIZE * 16, CONFIG_WL_SECTOR_SIZE, CONFIG_WL_SECTOR_SIZE, "partition_table.bin");

    spiffs fs;
    s32_t spiffs_res;

    init_spiffs(&fs, 5);

    // Open test file
    spiffs_res = SPIFFS_open(&fs, "test.txt", SPIFFS_O_CREAT | SPIFFS_O_RDWR, 0);
    REQUIRE(spiffs_res >= SPIFFS_OK);

    // Generate data
    spiffs_file file = spiffs_res;

    uint32_t data_size = 100000;

    char *data = (char*) malloc(data_size);
    char *read = (char*) malloc(data_size);

    for(uint32_t i = 0; i < data_size; i += sizeof(i))
    {
        *((uint32_t*)(data + i)) = i;
    }

    // Write data to file
    spiffs_res = SPIFFS_write(&fs, file, (void*)data, data_size);
    REQUIRE(spiffs_res >= SPIFFS_OK);
    REQUIRE(spiffs_res == data_size);

    // Set the file object pointer to the beginning
    spiffs_res = SPIFFS_lseek(&fs, file, 0, SPIFFS_SEEK_SET);
    REQUIRE(spiffs_res >= SPIFFS_OK);

    // Read the file
    spiffs_res = SPIFFS_read(&fs, file, (void*)read, data_size);
    REQUIRE(spiffs_res >= SPIFFS_OK);
    REQUIRE(spiffs_res == data_size);

    // Close the test file
    spiffs_res = SPIFFS_close(&fs, file);
    REQUIRE(spiffs_res >= SPIFFS_OK);

    REQUIRE(memcmp(data, read, data_size) == 0);

    deinit_spiffs(&fs);

    free(read);
    free(data);
}

TEST_CASE("can read spiffs image", "[spiffs]")
{
    _spi_flash_init(CONFIG_ESPTOOLPY_FLASHSIZE, CONFIG_WL_SECTOR_SIZE * 16, CONFIG_WL_SECTOR_SIZE, CONFIG_WL_SECTOR_SIZE, "partition_table.bin");

    spiffs fs;
    s32_t spiffs_res;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "storage");

    // Write the contents of the image file to partition
    FILE* img_file = fopen("image.bin", "r");
    REQUIRE(img_file);

    fseek(img_file, 0, SEEK_END);
    long img_size = ftell(img_file);
    fseek(img_file, 0, SEEK_SET);

    char *img = (char*) malloc(img_size);
    fread(img, 1, img_size, img_file);
    fclose(img_file);

    REQUIRE(partition->size == img_size);

    esp_partition_erase_range(partition, 0, partition->size);
    esp_partition_write(partition, 0, img, img_size);

    free(img);

    // Mount the spiffs partition and init filesystem, using the contents of
    // the image file
    init_spiffs(&fs, 1024);

    // Check spiffs consistency
    spiffs_res = SPIFFS_check(&fs);
    REQUIRE(spiffs_res == SPIFFS_OK);

    char path_buf[PATH_MAX];

    // The image is created from the spiffs source directory. Compare the files in that
    // directory to the files read from the SPIFFS image.
    check_spiffs_files(&fs, "../spiffs", path_buf);

    deinit_spiffs(&fs);
}