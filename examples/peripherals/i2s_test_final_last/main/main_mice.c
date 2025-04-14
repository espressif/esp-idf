/*
 * ESP32-S3 マイク入力テスト
 * I2Sインターフェースを使用してマイクから音声を取得するサンプル
 */

 #include <stdio.h>
 #include <string.h>
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "esp_system.h"
 #include "esp_log.h"
 #include "driver/i2s_std.h"
 #include "driver/gpio.h"
 #include "esp_check.h"
 
 static const char *TAG = "mic-test";
 
 // I2S設定 - マイク用
 // 共有いただいたArduinoコードからピン設定を採用
 #define I2S_BCK_IO          GPIO_NUM_6  // ビットクロック 
 #define I2S_WS_IO           GPIO_NUM_5  // ワードセレクト
 #define I2S_DI_IO           GPIO_NUM_4  // データイン（マイク入力）
 #define I2S_DO_IO           GPIO_NUM_NC // 出力なし
 
 // オーディオ設定
 #define SAMPLE_RATE         16000       // サンプリングレート (Hz)
 #define SAMPLE_BITS         16          // サンプルビット数
 #define AUDIO_CHANNELS      1           // モノラル
 
 // I2Sハンドル
 i2s_chan_handle_t i2s_handle = NULL;
 
 // I2S初期化
 static esp_err_t init_i2s(void)
 {
     ESP_LOGI(TAG, "I2S初期化を開始します");
     esp_err_t ret = ESP_OK;
 
     // I2Sチャンネル設定
     i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
     ret = i2s_new_channel(&chan_cfg, NULL, &i2s_handle); // RXモードでチャンネル作成
     if (ret != ESP_OK) {
         ESP_LOGE(TAG, "I2Sチャンネルの作成に失敗しました: %d", ret);
         return ret;
     }
 
     // I2S標準モード設定
     i2s_std_config_t std_cfg = {
         .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
         .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_MONO),
         .gpio_cfg = {
             .mclk = GPIO_NUM_NC,    // マスタークロックは使用しない
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
 
     ret = i2s_channel_init_std_mode(i2s_handle, &std_cfg);
     if (ret != ESP_OK) {
         ESP_LOGE(TAG, "I2SのSTDモード初期化に失敗しました: %d", ret);
         return ret;
     }
 
     // I2Sチャンネルを有効化
     ret = i2s_channel_enable(i2s_handle);
     if (ret != ESP_OK) {
         ESP_LOGE(TAG, "I2Sチャンネルの有効化に失敗しました: %d", ret);
         return ret;
     }
 
     ESP_LOGI(TAG, "I2S初期化が完了しました");
     return ESP_OK;
 }
 
 // マイクからデータを読み取るタスク
 static void mic_task(void *arg)
 {
     ESP_LOGI(TAG, "マイク入力タスクを開始します");
     
     const int buf_size = 1024;
     int16_t *samples = malloc(buf_size * sizeof(int16_t));
     if (samples == NULL) {
         ESP_LOGE(TAG, "メモリ割り当てに失敗しました");
         vTaskDelete(NULL);
     }
 
     ESP_LOGI(TAG, "マイクから音声データを読み取ります");
     while (1) {
         // マイクから音声データを読み取り
         size_t bytes_read = 0;
         esp_err_t ret = i2s_channel_read(i2s_handle, samples, buf_size * sizeof(int16_t), &bytes_read, portMAX_DELAY);
         if (ret != ESP_OK) {
             ESP_LOGE(TAG, "I2S読み取りエラー: %d", ret);
         } else if (bytes_read > 0) {
             // 最初の10サンプルだけ表示（ログのためとデバッグ用）
             printf("音声データ: ");
             for (int i = 0; i < 10 && i < bytes_read / sizeof(int16_t); i++) {
                 printf("%d ", samples[i]);
             }
             printf("... (合計 %d サンプル)\n", bytes_read / sizeof(int16_t));
             
             // ここで音量レベルを計算して表示
             int max_value = 0;
             for (int i = 0; i < bytes_read / sizeof(int16_t); i++) {
                 int abs_val = abs(samples[i]);
                 if (abs_val > max_value) {
                     max_value = abs_val;
                 }
             }
             // 音量レベルを％で表示（ログプレベルをINFOに設定）
             int volume_percent = (max_value * 100) / 32768;
             ESP_LOGI(TAG, "音量レベル: %d%% (最大値: %d)", volume_percent, max_value);
         }
         
         // 少し待機
         vTaskDelay(pdMS_TO_TICKS(100));
     }
 
     free(samples);
     vTaskDelete(NULL);
 }
 
 void app_main(void)
 {
     printf("ESP32-S3 マイク入力テストを開始します\n");
     
     // I2S初期化
     if (init_i2s() != ESP_OK) {
         ESP_LOGE(TAG, "I2Sの初期化に失敗しました");
         return;
     }
     
     // 設定情報を表示
     printf("\nI2S設定:\n");
     printf("- BCK (ビットクロック): GPIO %d\n", I2S_BCK_IO);
     printf("- WS (ワードセレクト): GPIO %d\n", I2S_WS_IO);
     printf("- DI (データイン): GPIO %d\n", I2S_DI_IO);
     printf("- サンプリングレート: %d Hz\n", SAMPLE_RATE);
     printf("- ビット幅: %d bits\n", SAMPLE_BITS);
     printf("- チャンネル: %s\n", (AUDIO_CHANNELS == 1) ? "モノラル" : "ステレオ");
     
     // マイク入力タスクを作成
     xTaskCreate(mic_task, "mic_task", 4096, NULL, 5, NULL);
     
     printf("\nマイクからの音声データを読み取っています\n");
     printf("近くで話したり音を立てたりして、マイクの反応を確認してください\n");
 }