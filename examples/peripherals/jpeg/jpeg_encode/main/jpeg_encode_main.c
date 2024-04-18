/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/jpeg_encode.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"

static const char *TAG = "jpeg.example";
static sdmmc_card_t *s_card;
#define MOUNT_POINT "/sdcard"

const static char s_infile_1080p[] = "/sdcard/esp1080.rgb";
const static char s_outfile_1080p[] = "/sdcard/outjpg.jpg";

static esp_err_t sdcard_init(void)
{
    esp_err_t ret = ESP_OK;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

#if CONFIG_EXAMPLE_SDMMC_IO_POWER_INTERNAL_LDO
    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = 4, // `LDO_VO4` is used as the SDMMC IO power
    };
    sd_pwr_ctrl_handle_t pwr_ctrl_handle = NULL;

    ret = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to new an on-chip ldo power control driver");
        return ret;
    }
    host.pwr_ctrl_handle = pwr_ctrl_handle;
#endif

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 4;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &s_card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return ret;
    }
    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, s_card);
    return ret;
}

static void sdcard_deinit(void)
{
    const char mount_point[] = MOUNT_POINT;
    esp_vfs_fat_sdcard_unmount(mount_point, s_card);
#if SOC_SDMMC_IO_POWER_EXTERNAL
    esp_err_t ret = sd_pwr_ctrl_del_on_chip_ldo(s_card->host.pwr_ctrl_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete on-chip ldo power control driver");
        return;
    }
#endif
}

void app_main(void)
{
    ESP_ERROR_CHECK(sdcard_init());
    uint32_t raw_size_1080p;
    uint32_t jpg_size_1080p;
    jpeg_encoder_handle_t jpeg_handle;

    FILE *file_raw_1080p = fopen(s_infile_1080p, "rb");
    ESP_LOGI(TAG, "s_infile_1080p:%s", s_infile_1080p);
    if (file_raw_1080p == NULL) {
        ESP_LOGE(TAG, "fopen file_raw_1080p error");
        return;
    }

    jpeg_encode_engine_cfg_t encode_eng_cfg = {
        .timeout_ms = 70,
    };

    jpeg_encode_memory_alloc_cfg_t rx_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_OUTPUT_BUFFER,
    };

    jpeg_encode_memory_alloc_cfg_t tx_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_INPUT_BUFFER,
    };

    ESP_ERROR_CHECK(jpeg_new_encoder_engine(&encode_eng_cfg, &jpeg_handle));
    // Read 1080p raw picture
    fseek(file_raw_1080p, 0, SEEK_END);
    raw_size_1080p = ftell(file_raw_1080p);
    fseek(file_raw_1080p, 0, SEEK_SET);
    size_t tx_buffer_size = 0;
    uint8_t *raw_buf_1080p = (uint8_t*)jpeg_alloc_encoder_mem(raw_size_1080p, &tx_mem_cfg, &tx_buffer_size);
    assert(raw_buf_1080p != NULL);
    fread(raw_buf_1080p, 1, raw_size_1080p, file_raw_1080p);
    fclose(file_raw_1080p);

    size_t rx_buffer_size = 0;
    uint8_t *jpg_buf_1080p = (uint8_t*)jpeg_alloc_encoder_mem(raw_size_1080p / 10, &rx_mem_cfg, &rx_buffer_size); // Assume that compression ratio of 10 to 1
    assert(jpg_buf_1080p != NULL);

    jpeg_encode_cfg_t enc_config = {
        .src_type = JPEG_ENCODE_IN_FORMAT_RGB888,
        .sub_sample = JPEG_DOWN_SAMPLING_YUV422,
        .image_quality = 80,
        .width = 1920,
        .height = 1080,
    };

    ESP_ERROR_CHECK(jpeg_encoder_process(jpeg_handle, &enc_config, raw_buf_1080p, raw_size_1080p, jpg_buf_1080p, rx_buffer_size, &jpg_size_1080p));

    FILE *file_jpg_1080p = fopen(s_outfile_1080p, "wb");
    ESP_LOGI(TAG, "outfile:%s",  s_outfile_1080p);
    if (file_jpg_1080p == NULL) {
        ESP_LOGE(TAG, "fopen file_jpg_1080p error");
        return;
    }

    fwrite(jpg_buf_1080p, 1, jpg_size_1080p, file_jpg_1080p);
    fclose(file_jpg_1080p);

    sdcard_deinit();
    ESP_LOGI(TAG, "Card unmounted");
}
