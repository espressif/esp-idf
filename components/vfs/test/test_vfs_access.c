/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "unity.h"
#include "driver/uart.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"
#include "wear_levelling.h"
#include "test_utils.h"

static wl_handle_t test_wl_handle;

TEST_CASE("Can use access() for UART", "[vfs]")
{
    const char *uarts[] = {
        "/dev/uart/0",
        "/dev/uart/1",
#if SOC_UART_NUM > 2
        "/dev/uart/2"
#endif
        };

    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    uart_driver_install(UART_NUM_1, 256, 0, 0, NULL, 0);
#if SOC_UART_NUM > 2
    uart_driver_install(UART_NUM_2, 256, 0, 0, NULL, 0);
#endif

    for (size_t i = 0; i < sizeof(uarts)/sizeof(uarts[0]); ++i) {
        TEST_ASSERT_EQUAL_MESSAGE(access(uarts[i], F_OK), 0, uarts[i]);

        TEST_ASSERT_EQUAL_MESSAGE(access(uarts[i], R_OK), 0, uarts[i]);
        TEST_ASSERT_EQUAL_MESSAGE(access(uarts[i], W_OK), 0, uarts[i]);
        TEST_ASSERT_EQUAL_MESSAGE(access(uarts[i], X_OK), -1, uarts[i]);
        TEST_ASSERT_EQUAL_MESSAGE(errno, EACCES, uarts[i]);

        TEST_ASSERT_EQUAL_MESSAGE(access(uarts[i], R_OK | W_OK), 0, uarts[i]);
        TEST_ASSERT_EQUAL_MESSAGE(access(uarts[i], R_OK | X_OK), -1, uarts[i]);
        TEST_ASSERT_EQUAL_MESSAGE(errno, EACCES, uarts[i]);
        TEST_ASSERT_EQUAL_MESSAGE(access(uarts[i], W_OK | X_OK), -1, uarts[i]);
        TEST_ASSERT_EQUAL_MESSAGE(errno, EACCES, uarts[i]);
        TEST_ASSERT_EQUAL_MESSAGE(access(uarts[i], R_OK | W_OK | X_OK), -1, uarts[i]);
        TEST_ASSERT_EQUAL_MESSAGE(errno, EACCES, uarts[i]);
    }

    TEST_ASSERT_EQUAL(access("/dev/uart/3", F_OK), -1);
    TEST_ASSERT_EQUAL(errno, ENOENT);

    uart_driver_delete(UART_NUM_0);
    uart_driver_delete(UART_NUM_1);
#if SOC_UART_NUM > 2
    uart_driver_delete(UART_NUM_2);
#endif
}

static inline void test_spi_flash_setup(void)
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5
    };

    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash", NULL, &mount_config, &test_wl_handle));
}

static inline void test_spi_flash_teardown(void)
{
    TEST_ESP_OK(esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", test_wl_handle));
}

static inline void test_fatfs_create_file(const char *name)
{
    int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC);
    TEST_ASSERT_NOT_EQUAL(fd, -1);
    TEST_ASSERT_EQUAL(0, close(fd));
}

static inline void test_fatfs_delete_file(const char *name)
{
    int ret = unlink(name);
    TEST_ASSERT_EQUAL(ret, 0);
}

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
//IDF-5139
TEST_CASE("Can use access() for FATFS", "[vfs][fatfs][wear_levelling]")
{
    const char *path = "/spiflash/access.txt";

    test_spi_flash_setup();

    {
        int ret = access(path, F_OK);

        if (ret != -1) {
            // it wasn't deleted before so we delete it now to pass the test
            // case the next time
            test_fatfs_delete_file(path);
        }

        TEST_ASSERT_EQUAL(ret, -1);
        TEST_ASSERT_EQUAL(errno, ENOENT);
    }

    test_fatfs_create_file(path);
    TEST_ASSERT_EQUAL(access(path, F_OK), 0);

    TEST_ASSERT_EQUAL(access(path, R_OK), 0);
    TEST_ASSERT_EQUAL(access(path, W_OK), 0);
    TEST_ASSERT_EQUAL(access(path, X_OK), 0);
    TEST_ASSERT_EQUAL(access(path, R_OK | W_OK), 0);
    TEST_ASSERT_EQUAL(access(path, R_OK | X_OK), 0);
    TEST_ASSERT_EQUAL(access(path, W_OK | X_OK), 0);
    TEST_ASSERT_EQUAL(access(path, R_OK | W_OK | X_OK), 0);

    //TODO f_chmod the file and re-test the access rights (this requires
    // f_chmod support to be implemented in VFS)

    test_fatfs_delete_file(path);
    TEST_ASSERT_EQUAL(access(path, F_OK), -1);
    TEST_ASSERT_EQUAL(errno, ENOENT);

    test_spi_flash_teardown();
}
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
