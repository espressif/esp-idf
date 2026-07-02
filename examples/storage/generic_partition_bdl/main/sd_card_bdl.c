/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * SD/eMMC whole-disk block device helpers for the generic-partition BDL example.
 *
 * Both the SDMMC and the SDSPI host paths initialize an `sdmmc_card_t` and then
 * hand it to `sdmmc_get_blockdev()`, so the rest of the example works with a
 * plain BDL handle and does not care which peripheral is used underneath.
 */

#include <stdlib.h>

#include "esp_check.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "sdmmc_cmd.h"

#if CONFIG_EXAMPLE_SD_HOST_SDMMC
#include "driver/sdmmc_host.h"
#elif CONFIG_EXAMPLE_SD_HOST_SDSPI
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#endif

#if SOC_SDMMC_IO_POWER_EXTERNAL || SOC_SDMMC_IO_UHS_POWER_EXTERNAL
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#endif

#include "sd_card_bdl.h"

#if CONFIG_EXAMPLE_STORAGE_MEDIA_SDCARD

static const char *TAG = "example_sd";

#if CONFIG_EXAMPLE_SD_HOST_SDMMC
static sdmmc_host_t s_host = SDMMC_HOST_DEFAULT();
#elif CONFIG_EXAMPLE_SD_HOST_SDSPI
static sdmmc_host_t s_host = SDSPI_HOST_DEFAULT();
static spi_host_device_t s_spi_host_id;  /* SPI bus id, saved before host.slot is reused for the device handle */
#endif

static sdmmc_card_t *s_card;

#if SOC_SDMMC_IO_POWER_EXTERNAL || SOC_SDMMC_IO_UHS_POWER_EXTERNAL
static sd_pwr_ctrl_handle_t s_pwr_ctrl_handle;

static esp_err_t init_power_control(sdmmc_host_t *host)
{
#if CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_INTERNAL_IO
    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_IO_ID,
    };
    ESP_RETURN_ON_ERROR(sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &s_pwr_ctrl_handle), TAG,
                        "failed to create on-chip LDO power control driver");
    host->pwr_ctrl_handle = s_pwr_ctrl_handle;
#else
    (void)host;
#endif
    return ESP_OK;
}

static void deinit_power_control(void)
{
    if (s_pwr_ctrl_handle != NULL) {
        esp_err_t err = sd_pwr_ctrl_del_on_chip_ldo(s_pwr_ctrl_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "failed to delete on-chip LDO power control driver: %s", esp_err_to_name(err));
        }
        s_pwr_ctrl_handle = NULL;
    }
}
#else
static inline esp_err_t init_power_control(sdmmc_host_t *host)
{
    (void)host;
    return ESP_OK;
}
static inline void deinit_power_control(void) {}
#endif

/* ---------------------------------------------------------------------- */

#if CONFIG_EXAMPLE_SD_HOST_SDMMC

static esp_err_t init_card(void)
{
    ESP_LOGI(TAG, "Whole disk: SD/eMMC card via SDMMC (slot %d)", s_host.slot);

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
#if CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_1
    slot_config.width = 1;
#else
    slot_config.width = 4;
#endif

#if CONFIG_EXAMPLE_SDMMC_INTERNAL_PULLUP
    /* Boards without external pull-ups on the SD lines (e.g. ESP32-S3-USB-OTG)
     * need the internal ones enabled. */
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
#endif

#if SOC_SDMMC_USE_GPIO_MATRIX
    /* On SoCs that route the SDMMC controller through the GPIO matrix the slot
     * pins are assignable, so apply the configured GPIOs. */
    slot_config.clk = CONFIG_EXAMPLE_SDMMC_PIN_CLK;
    slot_config.cmd = CONFIG_EXAMPLE_SDMMC_PIN_CMD;
    slot_config.d0 = CONFIG_EXAMPLE_SDMMC_PIN_D0;
#if CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.d1 = CONFIG_EXAMPLE_SDMMC_PIN_D1;
    slot_config.d2 = CONFIG_EXAMPLE_SDMMC_PIN_D2;
    slot_config.d3 = CONFIG_EXAMPLE_SDMMC_PIN_D3;
#endif
#endif // SOC_SDMMC_USE_GPIO_MATRIX

    ESP_RETURN_ON_ERROR(init_power_control(&s_host), TAG, "power control init failed");
    ESP_RETURN_ON_ERROR(sdmmc_host_init(), TAG, "SDMMC host init failed");
    ESP_RETURN_ON_ERROR(sdmmc_host_init_slot(s_host.slot, &slot_config), TAG, "SDMMC slot init failed");
    return ESP_OK;
}

static void deinit_card(void)
{
    sdmmc_host_deinit();
    deinit_power_control();
}

#elif CONFIG_EXAMPLE_SD_HOST_SDSPI

static esp_err_t init_card(void)
{
    ESP_LOGI(TAG, "Whole disk: SD card via SPI (SDSPI)");

    /* SDSPI_HOST_DEFAULT() puts the SPI host id in host.slot; keep it for later
     * as host.slot is overwritten with the SDSPI device handle below. */
    s_spi_host_id = s_host.slot;

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = CONFIG_EXAMPLE_SDSPI_PIN_MOSI,
        .miso_io_num = CONFIG_EXAMPLE_SDSPI_PIN_MISO,
        .sclk_io_num = CONFIG_EXAMPLE_SDSPI_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    ESP_RETURN_ON_ERROR(init_power_control(&s_host), TAG, "power control init failed");
    ESP_RETURN_ON_ERROR(spi_bus_initialize(s_spi_host_id, &bus_cfg, SDSPI_DEFAULT_DMA), TAG,
                        "failed to initialize SPI bus");

    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    dev_config.gpio_cs = CONFIG_EXAMPLE_SDSPI_PIN_CS;
    dev_config.host_id = s_spi_host_id;

    sdspi_dev_handle_t dev_handle;
    ESP_RETURN_ON_ERROR(sdspi_host_init(), TAG, "SDSPI host init failed");
    ESP_RETURN_ON_ERROR(sdspi_host_init_device(&dev_config, &dev_handle), TAG, "SDSPI device init failed");

    /* Route the card commands through the freshly created SDSPI device. */
    s_host.slot = dev_handle;
    return ESP_OK;
}

static void deinit_card(void)
{
    sdspi_host_deinit();
    spi_bus_free(s_spi_host_id);
    deinit_power_control();
}

#endif

/* ---------------------------------------------------------------------- */

esp_err_t example_sd_card_bdl_create(esp_blockdev_handle_t *out)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGW(TAG, "This will overwrite the card's existing partition table!");

    ESP_RETURN_ON_ERROR(init_card(), TAG, "card host init failed");

    s_card = calloc(1, sizeof(sdmmc_card_t));
    ESP_GOTO_ON_FALSE(s_card != NULL, ESP_ERR_NO_MEM, cleanup, TAG, "no mem for card");

    ESP_GOTO_ON_ERROR(sdmmc_card_init(&s_host, s_card), cleanup, TAG, "SD/eMMC card init failed");
    sdmmc_card_print_info(stdout, s_card);

    ESP_GOTO_ON_ERROR(sdmmc_get_blockdev(s_card, out), cleanup, TAG, "SD/eMMC block device creation failed");
    return ESP_OK;

cleanup:
    free(s_card);
    s_card = NULL;
    deinit_card();
    return ret;
}

esp_err_t example_sd_card_bdl_release(esp_blockdev_handle_t disk)
{
    esp_err_t err = disk->ops->release(disk);
    deinit_card();
    free(s_card);
    s_card = NULL;
    return err;
}

#endif /* CONFIG_EXAMPLE_STORAGE_MEDIA_SDCARD */
