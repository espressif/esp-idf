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
#include "esp_attr.h"
#include "driver/jpeg_decode.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"

static const char *TAG = "jpeg.example";
static sdmmc_card_t *s_card;
#define MOUNT_POINT "/sdcard"

const static char jpg_file_1080[] = "/sdcard/esp1080.jpg";
const static char raw_file_1080[] = "/sdcard/out.rgb";
const static char jpg_file_720[] = "/sdcard/esp720.jpg";
const static char raw_file_720[] = "/sdcard/out2.rgb";

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

    jpeg_decoder_handle_t jpgd_handle;

    jpeg_decode_engine_cfg_t decode_eng_cfg = {
        .timeout_ms = 40,
    };

    ESP_ERROR_CHECK(jpeg_new_decoder_engine(&decode_eng_cfg, &jpgd_handle));

    jpeg_decode_cfg_t decode_cfg_rgb = {
        .output_format = JPEG_DECODE_OUT_FORMAT_RGB888,
        .rgb_order = JPEG_DEC_RGB_ELEMENT_ORDER_BGR,
    };

    jpeg_decode_cfg_t decode_cfg_gray = {
        .output_format = JPEG_DECODE_OUT_FORMAT_GRAY,
    };

    jpeg_decode_memory_alloc_cfg_t rx_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_OUTPUT_BUFFER,
    };

    jpeg_decode_memory_alloc_cfg_t tx_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_INPUT_BUFFER,
    };

    FILE *file_jpg_1080p = fopen(jpg_file_1080, "rb");
    ESP_LOGI(TAG, "jpg_file_1080:%s", jpg_file_1080);
    if (file_jpg_1080p == NULL) {
        ESP_LOGE(TAG, "fopen file_jpg_1080p error");
        return;
    }

    fseek(file_jpg_1080p, 0, SEEK_END);
    int jpeg_size_1080p = ftell(file_jpg_1080p);
    fseek(file_jpg_1080p, 0, SEEK_SET);
    size_t tx_buffer_size_1080p = 0;
    uint8_t *tx_buf_1080p = (uint8_t*)jpeg_alloc_decoder_mem(jpeg_size_1080p, &tx_mem_cfg, &tx_buffer_size_1080p);
    if (tx_buf_1080p == NULL) {
        ESP_LOGE(TAG, "alloc 1080p tx buffer error");
        return;
    }
    fread(tx_buf_1080p, 1, jpeg_size_1080p, file_jpg_1080p);
    fclose(file_jpg_1080p);

    FILE *file_jpg_720p = fopen(jpg_file_720, "rb");
    ESP_LOGI(TAG, "jpg_file_1080:%s", jpg_file_720);
    if (file_jpg_720p == NULL) {
        ESP_LOGE(TAG, "fopen file_jpg_720p error");
        return;
    }
    fseek(file_jpg_720p, 0, SEEK_END);
    int jpeg_size_720p = ftell(file_jpg_720p);
    fseek(file_jpg_720p, 0, SEEK_SET);
    size_t tx_buffer_size_720p = 0;
    uint8_t *tx_buf_720p = (uint8_t*)jpeg_alloc_decoder_mem(jpeg_size_720p, &tx_mem_cfg, &tx_buffer_size_720p);
    if (tx_buf_720p == NULL) {
        ESP_LOGE(TAG, "alloc 720p tx buffer error");
        return;
    }
    fread(tx_buf_720p, 1, jpeg_size_720p, file_jpg_720p);
    fclose(file_jpg_720p);

    size_t rx_buffer_size_1080p = 0;
    size_t rx_buffer_size_720p = 0;
    uint8_t *rx_buf_1080p = (uint8_t*)jpeg_alloc_decoder_mem(1920 * 1088 * 3, &rx_mem_cfg, &rx_buffer_size_1080p);
    uint8_t *rx_buf_720p = (uint8_t*)jpeg_alloc_decoder_mem(720 * 1280, &rx_mem_cfg, &rx_buffer_size_720p);
    if (rx_buf_1080p == NULL) {
        ESP_LOGE(TAG, "alloc 1080p rx buffer error");
        return;
    }
    if (rx_buf_720p == NULL) {
        ESP_LOGE(TAG, "alloc 720p rx buffer error");
        return;
    }

    // Get the jpg header information (This step is optional)
    jpeg_decode_picture_info_t header_info;
    ESP_ERROR_CHECK(jpeg_decoder_get_info(tx_buf_1080p, jpeg_size_1080p, &header_info));
    ESP_LOGI(TAG, "header parsed, width is %" PRId32 ", height is %" PRId32, header_info.width, header_info.height);

    uint32_t out_size_1080p = 0;
    uint32_t out_size_720p = 0;
    ESP_ERROR_CHECK(jpeg_decoder_process(jpgd_handle, &decode_cfg_rgb, tx_buf_1080p, jpeg_size_1080p, rx_buf_1080p, rx_buffer_size_1080p, &out_size_1080p));
    ESP_ERROR_CHECK(jpeg_decoder_process(jpgd_handle, &decode_cfg_gray, tx_buf_720p, jpeg_size_720p, rx_buf_720p, rx_buffer_size_720p, &out_size_720p));

    // Write two pictures.
    FILE *file_rgb_1080p = fopen(raw_file_1080, "wb");
    ESP_LOGI(TAG, "raw_file_1080:%s", raw_file_1080);
    if (file_rgb_1080p == NULL) {
        ESP_LOGE(TAG, "fopen file_rgb_1080p error");
        return;
    }
    fwrite(rx_buf_1080p, 1, out_size_1080p, file_rgb_1080p);
    fclose(file_rgb_1080p);

    FILE *file_rgb_720p = fopen(raw_file_720, "wb");
    ESP_LOGI(TAG, "raw_file_720:%s", raw_file_720);
    if (file_rgb_720p == NULL) {
        ESP_LOGE(TAG, "fopen file_rgb_720p error");
        return;
    }
    fwrite(rx_buf_720p, 1, out_size_720p, file_rgb_720p);
    fclose(file_rgb_720p);

    sdcard_deinit();
    ESP_LOGI(TAG, "Card unmounted");

}
