/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

 #include <stdio.h>
 #include <inttypes.h>
 #include <math.h>
 #include <string.h>
 #include "sdkconfig.h"
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "esp_chip_info.h"
 #include "esp_flash.h"
 #include "esp_system.h"
 #include "esp_log.h"
 
 // Additional includes for audio output
 #include "driver/i2s_std.h"
 #include "driver/gpio.h"
 #include "esp_check.h"
 
 static const char *TAG = "audio_example";
 
 // Define M_PI if not available
 #ifndef M_PI
 #define M_PI 3.14159265358979323846
 #endif
 
 // Audio configuration - using pins from working example
 #define SAMPLE_RATE     (16000)
 #define I2S_NUM         (I2S_NUM_0)
 #define I2S_BCK_IO      (3)     // BCLK - Pin 3
 #define I2S_WS_IO       (2)     // LRC/WS - Pin 2
 #define I2S_DO_IO       (1)     // DOUT - Pin 1
 #define I2S_DI_IO       (I2S_GPIO_UNUSED)  // Not used for output only
 
 // Simple sine wave tone (C4 = 261.63 Hz)
 #define TONE_FREQ       (500)   // Simpler frequency
 #define SAMPLE_COUNT    (256)   // Smaller buffer
 
 // Create pre-calculated sine wave table to avoid computation
 static int16_t sine_wave[SAMPLE_COUNT];
 
 // I2S channel handle
 static i2s_chan_handle_t tx_handle = NULL;
 
 // Pre-calculate sine wave to avoid floating point in task
 static void generate_sine_wave(void) 
 {
     for (int i = 0; i < SAMPLE_COUNT; i++) {
         float angle = (float)i / SAMPLE_COUNT * 2 * M_PI;
         sine_wave[i] = (int16_t)(sinf(angle) * 16000); // Lower amplitude for safety
     }
 }
 
 // Initialize I2S for audio output
 static esp_err_t audio_init(void)
 {
     ESP_LOGI(TAG, "Initializing I2S");
     
     // Generate sine wave table
     generate_sine_wave();
     
     // Configure I2S driver
     i2s_chan_config_t chan_cfg = {
         .id = I2S_NUM,
         .role = I2S_ROLE_MASTER,
         .dma_desc_num = 4,
         .dma_frame_num = 64,
         .auto_clear = true,
     };
     
     esp_err_t ret = ESP_OK;
     
     // Create I2S channel
     ret = i2s_new_channel(&chan_cfg, &tx_handle, NULL);
     if (ret != ESP_OK) {
         ESP_LOGE(TAG, "Failed to create I2S channel: %d", ret);
         return ret;
     }
     
     // I2S standard configuration - using basic settings that should work with most ESP-IDF versions
     i2s_std_config_t std_cfg = {
         .clk_cfg = {
             .sample_rate_hz = SAMPLE_RATE,
             .clk_src = I2S_CLK_SRC_DEFAULT,
             .mclk_multiple = I2S_MCLK_MULTIPLE_256,
         },
         .slot_cfg = {
             .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
             .slot_bit_width = I2S_SLOT_BIT_WIDTH_16BIT,
             .slot_mode = I2S_SLOT_MODE_STEREO,
             .slot_mask = I2S_STD_SLOT_BOTH,
             // Removed problematic fields:
             // - ws_width 
             // - msb_right
             .ws_pol = false,
             .bit_shift = false,
         },
         .gpio_cfg = {
             .mclk = I2S_GPIO_UNUSED,
             .bclk = I2S_BCK_IO,
             .ws = I2S_WS_IO,
             .dout = I2S_DO_IO,
             .din = I2S_DI_IO,
             .invert_flags = {
                 .mclk_inv = false,
                 .bclk_inv = false,
                 .ws_inv = false,
             },
         },
     };
     
     // Initialize channel with standard mode
     ret = i2s_channel_init_std_mode(tx_handle, &std_cfg);
     if (ret != ESP_OK) {
         ESP_LOGE(TAG, "Failed to init I2S channel: %d", ret);
         return ret;
     }
     
     // Start I2S channel
     ret = i2s_channel_enable(tx_handle);
     if (ret != ESP_OK) {
         ESP_LOGE(TAG, "Failed to enable I2S channel: %d", ret);
         return ret;
     }
     
     ESP_LOGI(TAG, "I2S initialized successfully");
     return ESP_OK;
 }
 
 // Task to play continuous audio
 void audio_task(void *pvParameters)
 {
     // Allocate buffer for stereo samples
     const int stereo_sample_count = SAMPLE_COUNT;
     int16_t *stereo_samples = (int16_t*)malloc(stereo_sample_count * 2 * sizeof(int16_t));
     
     if (stereo_samples == NULL) {
         ESP_LOGE(TAG, "Failed to allocate memory for audio samples");
         vTaskDelete(NULL);
         return;
     }
     
     // Prepare stereo samples (left and right channels)
     for (int i = 0; i < SAMPLE_COUNT; i++) {
         stereo_samples[i*2] = sine_wave[i];      // Left channel
         stereo_samples[i*2+1] = sine_wave[i];    // Right channel
     }
     
     size_t bytes_written = 0;
     
     // Main audio loop
     while (1) {
         // Write the entire buffer at once
         esp_err_t ret = i2s_channel_write(tx_handle, stereo_samples, 
                                          stereo_sample_count * 2 * sizeof(int16_t),
                                          &bytes_written, portMAX_DELAY);
                                         
         if (ret != ESP_OK) {
             ESP_LOGE(TAG, "I2S write failed: %d", ret);
             vTaskDelay(100 / portTICK_PERIOD_MS);
         }
         
         // Small delay to prevent watchdog issues
         vTaskDelay(1 / portTICK_PERIOD_MS);
     }
     
     // Cleanup
     free(stereo_samples);
     vTaskDelete(NULL);
 }
 
 void app_main(void)
 {
     printf("Hello world!\n");
 
     /* Print chip information */
     esp_chip_info_t chip_info;
     uint32_t flash_size;
     esp_chip_info(&chip_info);
     printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
            (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
            (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");
 
     unsigned major_rev = chip_info.revision / 100;
     unsigned minor_rev = chip_info.revision % 100;
     printf("silicon revision v%d.%d, ", major_rev, minor_rev);
     if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
         printf("Get flash size failed");
         return;
     }
 
     printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
 
     printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
 
     // Initialize audio
     printf("Initializing audio...\n");
     esp_err_t ret = audio_init();
     if (ret != ESP_OK) {
         printf("Audio initialization failed: %d\n", ret);
         return;
     }
     
     // Create audio task with sufficient stack
     printf("Starting audio task...\n");
     TaskHandle_t audio_task_handle = NULL;
     BaseType_t task_created = xTaskCreate(
         audio_task,
         "audio_task",
         8192,    // Increased stack size
         NULL,
         5,
         &audio_task_handle
     );
     
     if (task_created != pdPASS) {
         printf("Failed to create audio task\n");
         return;
     }
     
     for (int i = 10; i >= 0; i--) {
         printf("Restarting in %d seconds...\n", i);
         vTaskDelay(1000 / portTICK_PERIOD_MS);
     }
     printf("Restarting now.\n");
     fflush(stdout);
     esp_restart();
 }