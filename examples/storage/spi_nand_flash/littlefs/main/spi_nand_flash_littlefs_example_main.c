/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * LittleFS over BDL on SPI NAND (wear-leveling stack)
 *
 * Stack (bottom to top):
 *
 *     +----------------+
 *     | LittleFS + VFS |   esp_vfs_littlefs_register (.blockdev)
 *     +----------------+
 *     |    WL BDL      |   spi_nand_flash_init_with_layers (Dhara FTL)
 *     +----------------+
 *     |  Flash BDL     |   raw NAND pages/blocks
 *     +----------------+
 *     |  SPI + chip    |   spi_nand_flash driver
 *     +----------------+
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"
#include "soc/spi_pins.h"
#include "spi_nand_flash.h"
#include "esp_littlefs.h"

#define EXAMPLE_FLASH_FREQ_KHZ      40000

static const char *TAG = "example";

#ifdef CONFIG_IDF_TARGET_ESP32
#define HOST_ID  SPI3_HOST
#define PIN_MOSI SPI3_IOMUX_PIN_NUM_MOSI
#define PIN_MISO SPI3_IOMUX_PIN_NUM_MISO
#define PIN_CLK  SPI3_IOMUX_PIN_NUM_CLK
#define PIN_CS   SPI3_IOMUX_PIN_NUM_CS
#define PIN_WP   SPI3_IOMUX_PIN_NUM_WP
#define PIN_HD   SPI3_IOMUX_PIN_NUM_HD
#define SPI_DMA_CHAN SPI_DMA_CH_AUTO
#else
#define HOST_ID  SPI2_HOST
#define PIN_MOSI SPI2_IOMUX_PIN_NUM_MOSI
#define PIN_MISO SPI2_IOMUX_PIN_NUM_MISO
#define PIN_CLK  SPI2_IOMUX_PIN_NUM_CLK
#define PIN_CS   SPI2_IOMUX_PIN_NUM_CS
#define PIN_WP   SPI2_IOMUX_PIN_NUM_WP
#define PIN_HD   SPI2_IOMUX_PIN_NUM_HD
#define SPI_DMA_CHAN SPI_DMA_CH_AUTO
#endif

static const char *base_path = "/nandflash";

static void example_init_nand_flash(esp_blockdev_handle_t *out_wl_bdl, spi_device_handle_t *spi_handle)
{
    const spi_bus_config_t bus_config = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = 4096 * 2,
    };

    ESP_LOGI(TAG, "DMA CHANNEL: %d", SPI_DMA_CHAN);
    ESP_ERROR_CHECK(spi_bus_initialize(HOST_ID, &bus_config, SPI_DMA_CHAN));

    const uint32_t spi_flags = SPI_DEVICE_HALFDUPLEX;

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = EXAMPLE_FLASH_FREQ_KHZ * 1000,
        .mode = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 10,
        .flags = spi_flags,
    };

    spi_device_handle_t spi;
    ESP_ERROR_CHECK(spi_bus_add_device(HOST_ID, &devcfg, &spi));

    spi_nand_flash_config_t nand_flash_config = {
        .device_handle = spi,
        .io_mode = SPI_NAND_IO_MODE_SIO,
        .flags = spi_flags,
    };

    esp_blockdev_handle_t wl_bdl;
    ESP_ERROR_CHECK(spi_nand_flash_init_with_layers(&nand_flash_config, &wl_bdl));

    *out_wl_bdl = wl_bdl;
    *spi_handle = spi;
}

static void example_deinit_spi_bus(spi_device_handle_t spi)
{
    ESP_ERROR_CHECK(spi_bus_remove_device(spi));
    ESP_ERROR_CHECK(spi_bus_free(HOST_ID));
}

/*
 * Tear down SPI. If mounted is true, also unregister LittleFS (which releases the BDL).
 * If register failed after taking the blockdev, LittleFS already released it — pass mounted=false.
 */
static void example_cleanup(esp_blockdev_handle_t wl_bdl, spi_device_handle_t spi, bool mounted)
{
    if (mounted) {
        esp_vfs_littlefs_unregister_blockdev(wl_bdl);
    }
    example_deinit_spi_bus(spi);
}

void app_main(void)
{
    esp_blockdev_handle_t wl_bdl;
    spi_device_handle_t spi;
    example_init_nand_flash(&wl_bdl, &spi);

    esp_vfs_littlefs_conf_t conf = {
        .base_path = base_path,
        .partition_label = NULL,
        .partition = NULL,
        .blockdev = wl_bdl,
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif
        .read_only = false,
        .dont_mount = false,
        .grow_on_mount = false,
    };

    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the NAND to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
        example_cleanup(wl_bdl, spi, false);
        return;
    }

    size_t total = 0, used = 0;
    ESP_ERROR_CHECK(esp_littlefs_blockdev_info(wl_bdl, &total, &used));
    ESP_LOGI(TAG, "LittleFS: %u kB total, %u kB used", (unsigned)(total / 1024), (unsigned)(used / 1024));

    ESP_LOGI(TAG, "Opening file");
    FILE *f = fopen("/nandflash/hello.txt", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        example_cleanup(wl_bdl, spi, true);
        return;
    }
    fprintf(f, "Written using ESP-IDF %s\n", esp_get_idf_version());
    fclose(f);
    ESP_LOGI(TAG, "File written");

    ESP_LOGI(TAG, "Reading file");
    f = fopen("/nandflash/hello.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        example_cleanup(wl_bdl, spi, true);
        return;
    }
    char line[128] = {0};
    if (fgets(line, sizeof(line), f) == NULL) {
        ESP_LOGE(TAG, "Failed to read from file");
        fclose(f);
        example_cleanup(wl_bdl, spi, true);
        return;
    }
    fclose(f);
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    ESP_ERROR_CHECK(esp_littlefs_blockdev_info(wl_bdl, &total, &used));
    ESP_LOGI(TAG, "LittleFS: %u kB total, %u kB used", (unsigned)(total / 1024), (unsigned)(used / 1024));

    example_cleanup(wl_bdl, spi, true);
    ESP_LOGI(TAG, "Done");
}
