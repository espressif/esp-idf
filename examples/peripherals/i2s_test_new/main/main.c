/* ESP32-S3 ハードウェア診断および音楽出力テスト
 * 
 * このプログラムは以下を行います:
 * 1. 利用可能なGPIOピンの状態をスキャン
 * 2. I2Sを使用して簡単な音を出力
 */

 #include <stdio.h>
 #include <string.h>
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "driver/gpio.h"
 #include "driver/i2s_std.h"
 #include "esp_log.h"
 #include "esp_system.h"
 #include "math.h"
 #include "sdkconfig.h"
 
 #define TAG "HW-TEST"
 
 // テスト対象のGPIOピン
 // 注: ESP32-S3では一部のピンが特別な用途に予約されている場合があります
 #define GPIO_START 1
 #define GPIO_END   45
 
 // 音楽出力設定
 #define SAMPLE_RATE     CONFIG_EXAMPLE_SAMPLE_RATE
 #define I2S_BCK_IO      CONFIG_EXAMPLE_I2S_BCK_IO  // ビットクロック
 #define I2S_WS_IO       CONFIG_EXAMPLE_I2S_WS_IO   // ワードセレクト(LRCLK)
 #define I2S_DO_IO       CONFIG_EXAMPLE_I2S_DO_IO   // データ出力
 #define I2S_DI_IO       CONFIG_EXAMPLE_I2S_DI_IO   // データ入力（使用しない場合は-1）
 
 // サイン波生成用
 #define PI 3.14159265358979323846
 
 // GPIO診断タスク
 void gpio_diagnostic_task(void *pvParameters) {
     ESP_LOGI(TAG, "GPIO診断を開始します...");
 
     for (int pin = GPIO_START; pin <= GPIO_END; pin++) {
         // 一部のピンはスキップ (フラッシュに接続されているなど)
         if (pin >= 6 && pin <= 11) {
             ESP_LOGI(TAG, "GPIO %d: フラッシュメモリに接続されている可能性があるためスキップ", pin);
             continue;
         }
 
         gpio_config_t io_conf = {
             .pin_bit_mask = (1ULL << pin),
             .mode = GPIO_MODE_INPUT,
             .pull_up_en = GPIO_PULLUP_DISABLE,
             .pull_down_en = GPIO_PULLDOWN_DISABLE,
             .intr_type = GPIO_INTR_DISABLE
         };
         
         esp_err_t ret = gpio_config(&io_conf);
         if (ret != ESP_OK) {
             ESP_LOGW(TAG, "GPIO %d: 設定できません (エラー %d)", pin, ret);
             continue;
         }
         
         // 入力として読み取り
         int level = gpio_get_level(pin);
         ESP_LOGI(TAG, "GPIO %d: レベル = %d", pin, level);
         
         // 少し待機
         vTaskDelay(10 / portTICK_PERIOD_MS);
     }
     
     ESP_LOGI(TAG, "GPIO診断が完了しました");
     vTaskDelete(NULL);
 }
 
 // 簡単な音楽を生成する関数
 void generate_sine_wave(int16_t* buffer, size_t buffer_size, float frequency, int sample_rate) {
     // 簡単なサイン波を生成
     for (size_t i = 0; i < buffer_size / 2; i++) {
         float sample = sinf(2 * PI * frequency * i / sample_rate);
         int16_t sample_int = (int16_t)(sample * 32767.0f); // int16_tに変換
         
         // ステレオのため、左右チャンネルに同じ値をセット
         buffer[i * 2] = sample_int;      // 左チャンネル
         buffer[i * 2 + 1] = sample_int;  // 右チャンネル
     }
 }
 
 // 音楽出力タスク
 void audio_output_task(void *pvParameters) {
     ESP_LOGI(TAG, "音楽出力タスクを開始します...");
     
     // I2S標準モードの設定
     i2s_chan_handle_t tx_handle;
     i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
     ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));
     
     i2s_std_config_t std_cfg = {
         .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
         .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
         .gpio_cfg = {
             .mclk = GPIO_NUM_NC,
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
     
     ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
     ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));
     
     // オーディオバッファ準備
     const size_t buffer_size = 1024;
     int16_t *buffer = malloc(buffer_size * sizeof(int16_t));
     if (buffer == NULL) {
         ESP_LOGE(TAG, "メモリ割り当てに失敗しました");
         vTaskDelete(NULL);
         return;
     }
     
     // 異なる周波数でサイン波を生成し再生
     const float frequencies[] = {262.0f, 330.0f, 392.0f, 523.0f}; // ドミソド'の周波数
     const int freq_count = sizeof(frequencies) / sizeof(frequencies[0]);
     
     size_t bytes_written = 0;
     
     while (1) {
         for (int i = 0; i < freq_count; i++) {
             // 現在の周波数のサイン波を生成
             generate_sine_wave(buffer, buffer_size, frequencies[i], SAMPLE_RATE);
             
             // 生成した音を出力
             ESP_LOGI(TAG, "周波数 %.1f Hz の音を再生中...", frequencies[i]);
             ESP_ERROR_CHECK(i2s_channel_write(tx_handle, buffer, buffer_size * sizeof(int16_t), &bytes_written, 1000));
             
             // 500ms再生
             vTaskDelay(500 / portTICK_PERIOD_MS);
         }
     }
     
     // 後処理（ここには到達しませんが、完全性のために含めています）
     free(buffer);
     ESP_ERROR_CHECK(i2s_channel_disable(tx_handle));
     ESP_ERROR_CHECK(i2s_del_channel(tx_handle));
     vTaskDelete(NULL);
 }
 
 void app_main(void) {
     ESP_LOGI(TAG, "ESP32-S3 ハードウェア診断を開始します");
     
     // GPIO診断タスク作成
     xTaskCreate(gpio_diagnostic_task, "gpio_diagnostic", 4096, NULL, 5, NULL);
     
     // 少し待ってからオーディオ出力タスクを開始
     vTaskDelay(2000 / portTICK_PERIOD_MS);
     
     // オーディオ出力タスク作成
     xTaskCreate(audio_output_task, "audio_output", 4096, NULL, 5, NULL);
 }
