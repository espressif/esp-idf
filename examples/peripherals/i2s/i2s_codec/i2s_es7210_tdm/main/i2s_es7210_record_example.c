/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_vfs_fat.h"
#include "driver/i2s_tdm.h"
#include "driver/i2c_master.h"
#include "esp_codec_dev_defaults.h"
#include "esp_codec_dev.h"
#include "esp_codec_dev_vol.h"
#include "format_wav.h"

#if CONFIG_IDF_TARGET_ESP32S3 // ESP32-S3-Korvo-1 pin out
/* I2C port and GPIOs */
#define EXAMPLE_I2C_NUM            (0)
#define EXAMPLE_I2C_SDA_IO         (1)
#define EXAMPLE_I2C_SCL_IO         (2)

/* I2S port and GPIOs */
#define EXAMPLE_I2S_NUM            (0)
#define EXAMPLE_I2S_MCK_IO         (20)
#define EXAMPLE_I2S_BCK_IO         (10)
#define EXAMPLE_I2S_WS_IO          (9)
#define EXAMPLE_I2S_DI_IO          (11)

/* SD card SPI GPIOs */
#define EXAMPLE_SD_SPI_CLK_IO      (18)
#define EXAMPLE_SD_SPI_MOSI_IO     (17)
#define EXAMPLE_SD_SPI_MISO_IO     (16)
#define EXAMPLE_SD_SPI_CS_IO       (15)
#elif CONFIG_IDF_TARGET_ESP32P4
#define EXAMPLE_I2C_NUM            (0)
#define EXAMPLE_I2C_SDA_IO         (3)
#define EXAMPLE_I2C_SCL_IO         (2)

/* I2S port and GPIOs */
#define EXAMPLE_I2S_NUM            (0)
#define EXAMPLE_I2S_MCK_IO         (4)
#define EXAMPLE_I2S_BCK_IO         (5)
#define EXAMPLE_I2S_WS_IO          (6)
#define EXAMPLE_I2S_DI_IO          (7)

/* SD card SPI GPIOs */
#define EXAMPLE_SD_SPI_CLK_IO      (18)
#define EXAMPLE_SD_SPI_MOSI_IO     (19)
#define EXAMPLE_SD_SPI_MISO_IO     (14)
#define EXAMPLE_SD_SPI_CS_IO       (17)
#else
#define EXAMPLE_I2C_NUM            (0)
#define EXAMPLE_I2C_SDA_IO         (3)
#define EXAMPLE_I2C_SCL_IO         (2)

/* I2S port and GPIOs */
#define EXAMPLE_I2S_NUM            (0)
#define EXAMPLE_I2S_MCK_IO         (0)
#define EXAMPLE_I2S_BCK_IO         (1)
#define EXAMPLE_I2S_WS_IO          (10)
#define EXAMPLE_I2S_DI_IO          (8)

/* SD card SPI GPIOs */
#define EXAMPLE_SD_SPI_CLK_IO      (5)
#define EXAMPLE_SD_SPI_MOSI_IO     (7)
#define EXAMPLE_SD_SPI_MISO_IO     (6)
#define EXAMPLE_SD_SPI_CS_IO       (4)
#endif

/* I2S configurations */
#define EXAMPLE_I2S_CHAN_NUM       (4)
#define EXAMPLE_I2S_SAMPLE_RATE    (48000)
#define EXAMPLE_I2S_MCLK_MULTIPLE  (I2S_MCLK_MULTIPLE_256)
#define EXAMPLE_I2S_SAMPLE_BITS    (I2S_DATA_BIT_WIDTH_16BIT)
#define EXAMPLE_I2S_TDM_SLOT_MASK  (I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3)

/* ES7210 configurations */
#define EXAMPLE_ES7210_I2C_ADDR    (0x40)
#define EXAMPLE_ES7210_MIC_GAIN    (30)  // 30db
#define EXAMPLE_ES7210_MIC_SELECTED (ES7120_SEL_MIC1 | ES7120_SEL_MIC2 | ES7120_SEL_MIC3 | ES7120_SEL_MIC4)

/* SD card & recording configurations */
#define EXAMPLE_RECORD_TIME_SEC    (10)
#define EXAMPLE_SD_MOUNT_POINT     "/sdcard"
#define EXAMPLE_RECORD_FILE_PATH   "/RECORD.WAV"

static const char *TAG = "example";

static i2s_chan_handle_t es7210_i2s_init(void)
{
    i2s_chan_handle_t i2s_rx_chan = NULL;
    ESP_LOGI(TAG, "Create I2S receive channel");
    i2s_chan_config_t i2s_rx_conf = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&i2s_rx_conf, NULL, &i2s_rx_chan));

    ESP_LOGI(TAG, "Configure I2S receive channel to TDM mode");
    i2s_tdm_config_t i2s_tdm_rx_conf = {
        // es7210 driver is default to use philips format in esp_codec_dev component
        .slot_cfg = I2S_TDM_PHILIPS_SLOT_DEFAULT_CONFIG(
            EXAMPLE_I2S_SAMPLE_BITS,
            I2S_SLOT_MODE_STEREO,
            EXAMPLE_I2S_TDM_SLOT_MASK),
        .clk_cfg  = {
            .clk_src = I2S_CLK_SRC_DEFAULT,
            .sample_rate_hz = EXAMPLE_I2S_SAMPLE_RATE,
            .mclk_multiple = EXAMPLE_I2S_MCLK_MULTIPLE
        },
        .gpio_cfg = {
            .mclk = EXAMPLE_I2S_MCK_IO,
            .bclk = EXAMPLE_I2S_BCK_IO,
            .ws   = EXAMPLE_I2S_WS_IO,
            .dout = -1, // ES7210 only has ADC capability
            .din  = EXAMPLE_I2S_DI_IO
        },
    };

    ESP_ERROR_CHECK(i2s_channel_init_tdm_mode(i2s_rx_chan, &i2s_tdm_rx_conf));

    return i2s_rx_chan;
}

sdmmc_card_t * mount_sdcard(void)
{
    sdmmc_host_t sdmmc_host = SDSPI_HOST_DEFAULT();
    sdmmc_card_t *sdmmc_card = NULL;

    ESP_LOGI(TAG, "Initializing SPI bus for SD card");
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = EXAMPLE_SD_SPI_MOSI_IO,
        .miso_io_num = EXAMPLE_SD_SPI_MISO_IO,
        .sclk_io_num = EXAMPLE_SD_SPI_CLK_IO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(sdmmc_host.slot, &bus_cfg, SPI_DMA_CH_AUTO));

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = EXAMPLE_SD_SPI_CS_IO;
    slot_config.host_id = sdmmc_host.slot;

    ESP_LOGI(TAG, "Mounting SD card");
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 2,
        .allocation_unit_size = 8 * 1024
    };

    esp_err_t ret;
    while (1) {
        ret = esp_vfs_fat_sdspi_mount(EXAMPLE_SD_MOUNT_POINT, &sdmmc_host, &slot_config, &mount_config, &sdmmc_card);
        if (ret == ESP_OK) {
            break;
        } else if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_LOGI(TAG, "Card size: %lluMB, speed: %dMHz",
             (((uint64_t)sdmmc_card->csd.capacity) * sdmmc_card->csd.sector_size) >> 20,
             (int)(sdmmc_card->max_freq_khz / 1000));

    return sdmmc_card;
}

static esp_codec_dev_handle_t es7210_codec_init(i2s_chan_handle_t i2s_rx_chan)
{
    ESP_LOGI(TAG, "Init I2C used to configure ES7210");
    /* Initialize I2C peripheral */
    i2c_master_bus_handle_t i2c_bus_handle = NULL;
    i2c_master_bus_config_t i2c_mst_cfg = {
        .i2c_port = EXAMPLE_I2C_NUM,
        .sda_io_num = EXAMPLE_I2C_SDA_IO,
        .scl_io_num = EXAMPLE_I2C_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        /* Pull-up internally for no external pull-up case.
        Suggest to use external pull-up to ensure a strong enough pull-up. */
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_cfg, &i2c_bus_handle));

    /* Create control interface with I2C bus handle */
    audio_codec_i2c_cfg_t i2c_cfg = {
        .port = EXAMPLE_I2C_NUM,
        .addr = ES7210_CODEC_DEFAULT_ADDR,
        .bus_handle = i2c_bus_handle,
    };
    const audio_codec_ctrl_if_t *ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    assert(ctrl_if);

    /* Create data interface with I2S bus handle */
    audio_codec_i2s_cfg_t i2s_cfg = {
        .port = EXAMPLE_I2S_NUM,
        .rx_handle = i2s_rx_chan,
        .tx_handle = NULL,
    };
    const audio_codec_data_if_t *data_if = audio_codec_new_i2s_data(&i2s_cfg);
    assert(data_if);

    /* Create ES7210 interface handle */
    ESP_LOGI(TAG, "Configure ES7210 codec parameters");
    es7210_codec_cfg_t es7210_cfg = {
        .ctrl_if = ctrl_if,
        .master_mode = false,
        .mic_selected = EXAMPLE_ES7210_MIC_SELECTED,
        .mclk_src = ES7210_MCLK_FROM_PAD,
        .mclk_div = EXAMPLE_I2S_MCLK_MULTIPLE,
    };
    const audio_codec_if_t *es7210_if = es7210_codec_new(&es7210_cfg);
    assert(es7210_if);

    /* Create the top codec handle with ES7210 interface handle and data interface */
    esp_codec_dev_cfg_t dev_cfg = {
        .dev_type = ESP_CODEC_DEV_TYPE_IN,
        .codec_if = es7210_if,
        .data_if = data_if,
    };
    esp_codec_dev_handle_t codec_handle = esp_codec_dev_new(&dev_cfg);
    assert(codec_handle);

    /* Specify the sample configurations and open the device */
    esp_codec_dev_sample_info_t sample_cfg = {
        .bits_per_sample = EXAMPLE_I2S_SAMPLE_BITS,
        .channel = EXAMPLE_I2S_CHAN_NUM,
        .channel_mask = EXAMPLE_ES7210_MIC_SELECTED,
        .sample_rate = EXAMPLE_I2S_SAMPLE_RATE,
    };
    if (esp_codec_dev_open(codec_handle, &sample_cfg) != ESP_CODEC_DEV_OK) {
        ESP_LOGE(TAG, "Open codec device failed");
        assert(false);
    }

    /* Set the initial gain */
    if (esp_codec_dev_set_in_gain(codec_handle, EXAMPLE_ES7210_MIC_GAIN) != ESP_CODEC_DEV_OK) {
        ESP_LOGE(TAG, "set input gain failed");
        assert(false);
    }

    return codec_handle;
}

static esp_err_t record_wav(i2s_chan_handle_t i2s_rx_chan)
{
    ESP_RETURN_ON_FALSE(i2s_rx_chan, ESP_FAIL, TAG, "invalid i2s channel handle pointer");
    esp_err_t ret = ESP_OK;

    uint32_t byte_rate = EXAMPLE_I2S_SAMPLE_RATE * EXAMPLE_I2S_CHAN_NUM * EXAMPLE_I2S_SAMPLE_BITS / 8;
    uint32_t wav_size = byte_rate * EXAMPLE_RECORD_TIME_SEC;

    const wav_header_t wav_header =
        WAV_HEADER_PCM_DEFAULT(wav_size, EXAMPLE_I2S_SAMPLE_BITS, EXAMPLE_I2S_SAMPLE_RATE, EXAMPLE_I2S_CHAN_NUM);

    ESP_LOGI(TAG, "Opening file %s", EXAMPLE_RECORD_FILE_PATH);
    FILE *f = fopen(EXAMPLE_SD_MOUNT_POINT EXAMPLE_RECORD_FILE_PATH, "w");
    ESP_RETURN_ON_FALSE(f, ESP_FAIL, TAG, "error while opening wav file");

    /* Write wav header */
    ESP_GOTO_ON_FALSE(fwrite(&wav_header, sizeof(wav_header_t), 1, f), ESP_FAIL, err,
                      TAG, "error while writing wav header");

    /* Start recording */
    size_t wav_written = 0;
    static int16_t i2s_readraw_buff[4096];
    while (wav_written < wav_size) {
        if (wav_written % byte_rate < sizeof(i2s_readraw_buff)) {
            ESP_LOGI(TAG, "Recording: %"PRIu32"/%ds", wav_written / byte_rate + 1, (int)EXAMPLE_RECORD_TIME_SEC);
        }
        size_t bytes_read = 0;
        /* Read RAW samples from ES7210 */
        ESP_GOTO_ON_ERROR(i2s_channel_read(i2s_rx_chan, i2s_readraw_buff, sizeof(i2s_readraw_buff), &bytes_read,
                                           pdMS_TO_TICKS(1000)), err, TAG, "error while reading samples from i2s");
        /* Write the samples to the WAV file */
        ESP_GOTO_ON_FALSE(fwrite(i2s_readraw_buff, bytes_read, 1, f), ESP_FAIL, err,
                          TAG, "error while writing samples to wav file");
        wav_written += bytes_read;
    }

err:
    ESP_LOGI(TAG, "Recording done! Flushing file buffer");
    fclose(f);

    return ret;
}

void app_main(void)
{
    /* Init I2C bus to configure ES7210 and I2S bus to receive audio data from ES7210 */
    i2s_chan_handle_t i2s_rx_chan = es7210_i2s_init();
    /* Create ES7210 device handle and configure codec parameters */
    esp_codec_dev_handle_t codec_handle = es7210_codec_init(i2s_rx_chan);
    /* Mount SD card, the recorded audio file will be saved into it */
    sdmmc_card_t *sdmmc_card = mount_sdcard();
    /* Start to record wav audio */
    esp_err_t err = record_wav(i2s_rx_chan);
    /* Unmount SD card */
    esp_vfs_fat_sdcard_unmount(EXAMPLE_SD_MOUNT_POINT, sdmmc_card);
    /* Close codec device */
    esp_codec_dev_close(codec_handle);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Audio was successfully recorded into "EXAMPLE_RECORD_FILE_PATH
                 ". You can now remove the SD card safely");
    } else {
        ESP_LOGE(TAG, "Record failed, "EXAMPLE_RECORD_FILE_PATH" on SD card may not be playable.");
    }
}
