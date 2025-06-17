#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "nvs_flash.h"
#include "esp_vfs_fat.h"
#include "esp_timer.h"
#include "wear_levelling.h"
#include "sdmmc_cmd.h"
#include "driver/i2s_std.h"
#include "driver/i2s_pdm.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "cJSON.h"
#include "audio_player.h"
#include "wifi_manager.h"

// 一時追加
// #include "driver/rmt_tx.h"
// #include "led_strip.h"

/* グローバル変数として追加 */
SemaphoreHandle_t i2s_mutex = NULL;

static const char *TAG = "buzz-client";
// 192.168.1.24
/* APIサーバーの設定 */
#define API_SERVER_HOST "192.168.1.24"  // APIサーバーのIPアドレス
#define API_SERVER_PORT 8080             // APIサーバーのポート

/* I2S設定 - マイク用 */
// #define I2S_BCK_IO      GPIO_NUM_6       // ビットクロック 
// #define I2S_WS_IO       GPIO_NUM_5       // ワードセレクト
// #define I2S_DI_IO       GPIO_NUM_4       // データイン（マイク入力）
// #define I2S_DO_IO       GPIO_NUM_16      // データアウト（スピーカー出力）

/* I2S設定 - マイク用 */
#define I2S_MIC_BCK_IO      GPIO_NUM_6  // ビットクロック（マイク用）
#define I2S_MIC_WS_IO       GPIO_NUM_5  // ワードセレクト（マイク用）
#define I2S_MIC_DI_IO       GPIO_NUM_4  // データイン（マイク入力）

/* I2S設定 - スピーカー用 */
#define I2S_SPK_BCK_IO      GPIO_NUM_3  // ビットクロック（スピーカー用）
#define I2S_SPK_WS_IO       GPIO_NUM_2  // ワードセレクト（スピーカー用）
#define I2S_SPK_DO_IO       GPIO_NUM_1  // データアウト（スピーカー出力）

/* オーディオ設定 */
#define SAMPLE_RATE     16000            // サンプリングレート (Hz)
#define SAMPLE_BITS     16               // サンプルビット数
#define AUDIO_CHANNELS  1                // モノラル
#define BUFFER_SIZE     1024             // バッファサイズ

/* ボタンとLED設定 */
#define BUTTON_PIN      GPIO_NUM_15      // ボタンピン

#define LED_PIN         GPIO_NUM_7       // LEDピン
#define LED_COUNT       1                // LEDの数 (単一のNeoPixel)
#define CHASE_SPEED_MS  100              // アニメーション速度

/* 録音関連の設定 */
#define RECORD_TIME_MS  5000             // 録音時間 (ms)
#define RECORD_FILENAME "/sdcard/audio/rec.wav"
#define DOWNLOAD_FILENAME "/sdcard/audio/response.wav"

/* グローバル変数 */
// static i2s_chan_handle_t i2s_handle = NULL;
static QueueHandle_t button_event_queue;
static bool recording_active = false;
// // static bool emergency_stop = false;
static TaskHandle_t vad_task_handle = NULL; // VADタスクのハンドルを追加

// static led_strip_handle_t led_strip = NULL; // LEDストリップハンドル

/* 関数プロトタイプ */
void record_audio(void);
void send_audio_to_api(void);
void play_audio_file(const char *filename);
void download_and_play_audio(const char *audio_path);
void get_proactive_message(void);
// static void auto_record_task(void *pvParameters);

/* 関数プロトタイプ - 追加 */
static void led_blink_task(void *arg);
static void log_memory_info(const char* message);


esp_err_t spiffs_init(void) {
    ESP_LOGI(TAG, "FATファイルシステムを初期化しています...");
    
    // FATファイルシステムをマウント
    esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 5,
        .format_if_mount_failed = true,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };
    
    wl_handle_t wl_handle;
    
    esp_err_t ret = esp_vfs_fat_spiflash_mount_rw_wl("/spiffs", "storage", &mount_config, &wl_handle);
    
    
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "FATファイルシステムのマウントに失敗しました。パーティションが見つからないか、すでにマウントされています。");
        } else {
            ESP_LOGE(TAG, "FATファイルシステムのマウント中にエラーが発生しました: %s", esp_err_to_name(ret));
        }
        return ret;
    }
    
    ESP_LOGI(TAG, "FATファイルシステム初期化完了");
    return ESP_OK;
}

// // NeoPixel初期化関数
// static void neopixel_init(void)
// {
//     // LED_STRIPの設定
//     led_strip_config_t strip_config = {
//         .strip_gpio_num = LED_PIN,       // GPIO番号
//         .max_leds = LED_COUNT,           // LED数
//         .led_pixel_format = LED_PIXEL_FORMAT_GRB, // 一般的なNeoPixel形式
//         .led_model = LED_MODEL_WS2812,
//         .flags.invert_out = false,        // 信号を反転しない
//     };

//     // RMTの設定
//     led_strip_rmt_config_t rmt_config = {
// #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
//         .clk_src = RMT_CLK_SRC_DEFAULT,   // デフォルトクロックソース
//         .resolution_hz = 10 * 1000 * 1000, // 10MHz分解能 (100ns)
// #else
//         .rmt_channel = 0,
// #endif
//         .mem_block_symbols = 64,          // RMTメモリブロックサイズ
//         .flags.with_dma = false           // DMAを使用しない
//     };

//     // LED_STRIPドライバの作成
//         ESP_LOGE(TAG, "LED strip initialization failed");
//         return;
//     }

//     // RMTドライバの有効化
//     ESP_ERROR_CHECK(rmt_enable(rmt_chan));
// }

// // LED色設定関数の例
// static void set_led_color(uint8_t r, uint8_t g, uint8_t b)
// {
//     if (led_strip) {
//         // LEDの色を設定
//         ESP_ERROR_CHECK(led_strip->set_pixel(led_strip, 0, r, g, b));
//         // LEDの更新（実際に色を表示）
//         ESP_ERROR_CHECK(led_strip->refresh(led_strip, 100));
//     }
// }


/* ボタン割り込みハンドラ */
static void IRAM_ATTR button_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(button_event_queue, &gpio_num, NULL);
}

/* ボタンタスク - ボタン押下を処理 */
static void button_task(void* arg) {
    uint32_t gpio_num;
    
    for(;;) {
        if(xQueueReceive(button_event_queue, &gpio_num, portMAX_DELAY)) {
            // チャタリング防止（デバウンス）
            if (gpio_get_level(gpio_num) == 0) {
                // ボタンが押された
                vTaskDelay(pdMS_TO_TICKS(50)); // 50ms待機
                
                if (gpio_get_level(gpio_num) == 0 && !recording_active) {
                    ESP_LOGI(TAG, "ボタンが押されました");
                    
                    // LED点灯
                    gpio_set_level(LED_PIN, 1);
                    
                    // 録音開始
                    recording_active = true;
                    
                    // VADタスクを一時停止
                    if (vad_task_handle != NULL) {
                        vTaskSuspend(vad_task_handle);
                        ESP_LOGI(TAG, "VADタスクを一時停止しました");
                    }
                    
                    // 録音処理
                    record_audio();
                    
                    // APIに音声を送信
                    send_audio_to_api();
                    
                    // LED消灯
                    gpio_set_level(LED_PIN, 0);
                    
                    // 録音状態リセット
                    recording_active = false;
                    
                    // VADタスクを再開
                    if (vad_task_handle != NULL) {
                        vTaskResume(vad_task_handle);
                        ESP_LOGI(TAG, "VADタスクを再開しました");
                    }
                }
            }
        }
    }
}

/* 録音と送信を行うタスク - VADタスク一時停止機能付き */
static void record_and_send_task(void *pvParameters) {
    ESP_LOGI(TAG, "録音開始: VADタスクを一時停止します");
    // VADタスクを一時停止
    if (vad_task_handle != NULL) {
        ESP_LOGI(TAG, "DEBUG: record_and_send_task - VADタスクハンドル: %p, 一時停止を試みます", vad_task_handle);
        
        // タスクがブロック状態から回復する時間を確保するために少し待機
        vTaskDelay(pdMS_TO_TICKS(50)); // 50ms待機
        
        // 安全のためにeTaskStateを確認
        eTaskState task_state = eTaskGetState(vad_task_handle);
        ESP_LOGI(TAG, "DEBUG: record_and_send_task - VADタスク状態: %d (0=実行中, 1=準備完了, 2=ブロック, 3=一時停止, 4=削除)", task_state);
        
        // タスクが有効な状態であることを確認
        if (task_state != eDeleted) {
            vTaskSuspend(vad_task_handle);
            ESP_LOGI(TAG, "VADタスクを一時停止しました");
        } else {
            ESP_LOGW(TAG, "VADタスクは既に削除されています。一時停止できません。");
        }
    } else {
        ESP_LOGW(TAG, "VADタスクのハンドルがNULLです。一時停止できません。");
    }

    // メモリ使用状況をログ出力
    log_memory_info("録音前");
    
    ESP_LOGI(TAG, "DEBUG: record_and_send_task - record_audio()を呼び出します");
    // 録音処理
    record_audio();
    
    ESP_LOGI(TAG, "DEBUG: record_and_send_task - record_audio()が完了しました");
    // メモリ使用状況をログ出力
    log_memory_info("録音後、送信前");
    
    ESP_LOGI(TAG, "DEBUG: record_and_send_task - send_audio_to_api()を呼び出します");
    // APIに音声を送信
    send_audio_to_api();
    ESP_LOGI(TAG, "DEBUG: record_and_send_task - send_audio_to_api()が完了しました");
    
    // メモリ使用状況をログ出力
    log_memory_info("送信後");
    
    // LED消灯
    gpio_set_level(LED_PIN, 0);
    
    // 録音状態リセット
    recording_active = false;

    vTaskDelay(pdMS_TO_TICKS(1000)); // 200ms待機
    
    // VADタスクを再開
    if (vad_task_handle != NULL) {
        vTaskResume(vad_task_handle);
        ESP_LOGI(TAG, "VADタスクを再開しました");
    }
    
    // タスク終了
    vTaskDelete(NULL);
}

/* LED点滅タスク */
static void led_blink_task(void *arg) {
    uint32_t start_time = esp_timer_get_time() / 1000;
    while ((esp_timer_get_time() / 1000) - start_time < RECORD_TIME_MS) {
        // LEDを点滅
        if (((esp_timer_get_time() / 1000) - start_time) % 500 < 250) {
            gpio_set_level(LED_PIN, 1);
        } else {
            gpio_set_level(LED_PIN, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    vTaskDelete(NULL);
}


/* 録音関数 */
void record_audio(void) {
    ESP_LOGI(TAG, "録音を開始します...");
    
    // LEDを点滅させて録音中であることを示す
    ESP_LOGI(TAG, "DEBUG: record_audio - LED点滅開始");
    for (int i = 0; i < 3; i++) {
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    // LED点灯（録音中）
    ESP_LOGI(TAG, "DEBUG: record_audio - LED点灯（録音中）");
    gpio_set_level(LED_PIN, 1);
    
    // audio_playerコンポーネントを使用して録音
    ESP_LOGI(TAG, "DEBUG: record_audio - record_to_file()を呼び出します");
    esp_err_t ret = record_to_file(RECORD_FILENAME, RECORD_TIME_MS);
    ESP_LOGI(TAG, "DEBUG: record_audio - record_to_file()が戻りました: %s", esp_err_to_name(ret));
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "録音に失敗しました");
    } else {
        ESP_LOGI(TAG, "録音完了");
    }
}

/* 音声検出（VAD）タスク - 録音中一時停止機能付き */
static void vad_task(void *pvParameters) {
    ESP_LOGI(TAG, "音声検出タスクを開始します...");
    
    // ログ出力制御用のカウンター
    uint32_t log_counter = 0;
    const uint32_t log_interval = 50; // 50回に1回のみログを出力 (調整可能)
    
    // audio_player.cで定義されたi2s_rx_enabledフラグを使用
    if (!i2s_rx_enabled) {
        ESP_LOGI(TAG, "I2S受信チャンネルを有効化します");
        ESP_ERROR_CHECK(i2s_channel_enable(i2s_rx_handle));
        i2s_rx_enabled = true;
    } else {
        ESP_LOGI(TAG, "I2S受信チャンネルは既に有効化されています");
    }
    
    // バッファの割り当て
    const size_t buf_size = 1024;
    int16_t *samples = malloc(buf_size * sizeof(int16_t));
    if (!samples) {
        ESP_LOGE(TAG, "VADバッファのメモリ割り当てに失敗しました");
        vTaskDelete(NULL);
        return;
    }
    
    if (i2s_rx_handle == NULL) {
        ESP_LOGE(TAG, "i2s_rx_handleがNULLです。I2Sの初期化が正しく行われていません。");
        vTaskDelete(NULL);
        return;
    }
    
    // 音声検出のしきい値を調整
    const int voice_threshold = 2500;  // 音声検出しきい値
    const int activation_count = 4;    // 連続検出カウント（ノイズ除去用）
    
    int voice_count = 0;
    
    for (;;) {
        // 録音中はタスクが一時停止されるため、ここには到達しない
        
        // ミューテックスを取得してからI2S操作
        if (xSemaphoreTake(i2s_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            // マイクからデータを読み込む
            size_t bytes_read = 0;
            i2s_channel_read(i2s_rx_handle, samples, buf_size * sizeof(int16_t), &bytes_read, pdMS_TO_TICKS(100));
            
            if (bytes_read > 0) {
                // マイクの音量レベルを計算
                int16_t max_sample = 0;
                int16_t min_sample = 0;
                int32_t sum = 0;
                
                for (int i = 0; i < bytes_read / sizeof(int16_t); i++) {
                    if (samples[i] > max_sample) {
                        max_sample = samples[i];
                    }
                    if (samples[i] < min_sample) {
                        min_sample = samples[i];
                    }
                    sum += samples[i];
                }
                
                // 音声検出処理
                // 閾値を超えた場合、または一定間隔でのみログを出力
                bool should_log = false;  // ここで変数を宣言
                
                if (abs(max_sample) > voice_threshold) {
                    // 閾値を超えた場合は必ずログを出力
                    should_log = true;
                } else {
                    // 閾値以下の場合は一定間隔でログを出力
                    log_counter++;
                    if (log_counter >= log_interval) {
                        should_log = true;
                        log_counter = 0;
                    }
                }
                
                if (should_log) {
                    ESP_LOGI(TAG, "音声検出しきい値: %d, 現在の最大値: %d", voice_threshold, abs(max_sample));
                }
                
                if (abs(max_sample) > voice_threshold) {
                    // 音声を検出
                    voice_count++;

                    // 連続してN回以上音声が検出され、かつ録音中でない場合は録音を開始
                    if (voice_count >= activation_count && !recording_active) {
                        ESP_LOGI(TAG, "音声を検出しました - 録音を開始します");
                        
                        // 録音開始の前に状態をクリアに設定
                        recording_active = true;
                                                
                        // ミューテックスを解放
                        xSemaphoreGive(i2s_mutex);
                        
                        // LED点灯
                        gpio_set_level(LED_PIN, 1);
                        
                        ESP_LOGI(TAG, "録音タスクを開始します");
                        
                        // 別タスクで録音と送信を実行
                        BaseType_t result = xTaskCreate(record_and_send_task, "record_task", 8192, NULL, 10, NULL);
                        if (result != pdPASS) {
                            ESP_LOGE(TAG, "録音タスクの作成に失敗しました: %d", result);
                            // 失敗した場合は録音状態をリセット
                            recording_active = false;
                        } else {
                            ESP_LOGI(TAG, "録音タスクを作成しました");
                        }
                        
                        // 確実に録音タスクが開始するまで待機
                        vTaskDelay(pdMS_TO_TICKS(200));
                        continue; // 次のループへ
                    }
                } else {
                    // 無音が続いたらカウントをリセット
                    if (voice_count > 0) {
                        voice_count--;
                    }
                }
            }
            
            // ミューテックスを解放
            xSemaphoreGive(i2s_mutex);
        }

        // 少し待機
        vTaskDelay(pdMS_TO_TICKS(20)); // 20msごとに処理
    }
    
    // ここには到達しない
    free(samples);
    vTaskDelete(NULL);
}

/* HTTPイベントハンドラ - データ収集機能付き */
static char *output_buffer = NULL;  // レスポンス用のバッファ
static int output_len = 0;          // バッファの現在の長さ
static char next_audio_to_download[256] = {0}; // ダウンロード予定の音声ファイルパス

esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            // 新しいリクエストの開始時にバッファをリセット
            if (output_buffer != NULL) {
                free(output_buffer);
            }
            output_buffer = NULL;
            output_len = 0;
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            // データ受信時にバッファに追加
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // バッファが未初期化なら確保
                if (output_buffer == NULL) {
                    output_buffer = (char *)malloc(evt->data_len + 1);
                    if (output_buffer == NULL) {
                        ESP_LOGE(TAG, "メモリ割り当て失敗");
                        return ESP_FAIL;
                    }
                    memcpy(output_buffer, evt->data, evt->data_len);
                    output_len = evt->data_len;
                    output_buffer[output_len] = 0; // NULL終端
                } else {
                    // バッファを拡張して新しいデータを追加
                    char *tmp = (char *)realloc(output_buffer, output_len + evt->data_len + 1);
                    if (tmp == NULL) {
                        ESP_LOGE(TAG, "メモリ再割り当て失敗");
                        free(output_buffer);
                        output_buffer = NULL;
                        output_len = 0;
                        return ESP_FAIL;
                    }
                    output_buffer = tmp;
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                    output_len += evt->data_len;
                    output_buffer[output_len] = 0; // NULL終端
                }
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL && output_len > 0) {
                // レスポンスデータを表示
                ESP_LOGI(TAG, "レスポンスデータ: %s", output_buffer);
                
                // JSONの解析
                cJSON *json = cJSON_Parse(output_buffer);
                if (json) {
                    cJSON *success = cJSON_GetObjectItem(json, "success");
                    cJSON *message = cJSON_GetObjectItem(json, "message");
                    cJSON *audio_path = cJSON_GetObjectItem(json, "audio_path");
                    
                    if (cJSON_IsTrue(success)) {
                        if (message) {
                            ESP_LOGI(TAG, "バズの自発的なメッセージ: %s", message->valuestring);
                        }
                        
                        if (audio_path) {
                            ESP_LOGI(TAG, "音声ファイルのパス: %s", audio_path->valuestring);
                            // 音声ファイルのパスを保存
                            strncpy(next_audio_to_download, audio_path->valuestring, sizeof(next_audio_to_download) - 1);
                            next_audio_to_download[sizeof(next_audio_to_download) - 1] = 0; // 念のためNULL終端を確保
                        }
                    } else {
                        ESP_LOGE(TAG, "サーバーからエラーレスポンスを受信しました");
                    }
                    cJSON_Delete(json);
                } else {
                    ESP_LOGE(TAG, "JSONの解析に失敗しました");
                }
                
                // バッファをクリーンアップ
                free(output_buffer);
                output_buffer = NULL;
                output_len = 0;
            }
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
                output_len = 0;
            }
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
            break;
    }
    return ESP_OK;
}

/* 音声ファイルをAPIに送信する関数 */
void send_audio_to_api(void) {
    ESP_LOGI(TAG, "APIに音声を送信しています...");
    
    // APIサーバーを使用して音声を送信
    
    // ファイルを開く
    FILE *file = fopen(RECORD_FILENAME, "rb");
    if (!file) {
        ESP_LOGE(TAG, "録音ファイルを開けませんでした");
        return;
    }
    
    // ファイルサイズを取得
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // チャンク方式でファイルを処理
    // 全体を一度に読み込むのではなく、小さなバッファで少しずつ読み込み、送信する
    size_t chunk_size = 4096; // 4KBずつ読み込む
    char *chunk_buffer = malloc(chunk_size);
    if (!chunk_buffer) {
        ESP_LOGE(TAG, "チャンクバッファの割り当てに失敗しました");
        fclose(file);
        return;
    }
    
    ESP_LOGI(TAG, "チャンク方式でファイル転送を開始します（ファイルサイズ: %ld バイト）", file_size);
    
    // マルチパートフォームデータの境界
    const char *boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
    
    // マルチパートヘッダーの構築
    char *header = malloc(512);
    if (!header) {
        ESP_LOGE(TAG, "ヘッダーバッファの割り当てに失敗しました");
        free(chunk_buffer);
        fclose(file);
        return;
    }
    
    // マルチパートボディの構築
    sprintf(header, 
        "--%s\r\n"
        "Content-Disposition: form-data; name=\"file\"; filename=\"audio.wav\"\r\n"
        "Content-Type: audio/wav\r\n\r\n",
        boundary);
    
    // 終了境界
    char *footer = malloc(128);
    if (!footer) {
        ESP_LOGE(TAG, "フッターバッファの割り当てに失敗しました");
        free(chunk_buffer);
        free(header);
        fclose(file);
        return;
    }
    
    sprintf(footer, "\r\n--%s--\r\n", boundary);
    
    // Content-Typeヘッダーの構築
    char *content_type = malloc(128);
    if (!content_type) {
        ESP_LOGE(TAG, "Content-Typeバッファの割り当てに失敗しました");
        free(chunk_buffer);
        free(header);
        free(footer);
        fclose(file);
        return;
    }
    
    sprintf(content_type, "multipart/form-data; boundary=%s", boundary);
    
    // HTTPクライアント設定
    char url[128];
    sprintf(url, "http://%s:%d/upload_audio", API_SERVER_HOST, API_SERVER_PORT);
    
    ESP_LOGI(TAG, "APIサーバーURL: %s", url);
    
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,
        .method = HTTP_METHOD_POST,
        .buffer_size = 2048, // バッファサイズを増やす
        .timeout_ms = 30000, // タイムアウト時間
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", content_type);
    
    // コンテンツ長の設定
    int header_len = strlen(header);
    int footer_len = strlen(footer);
    int total_len = header_len + file_size + footer_len;
    
    char content_length[32];
    sprintf(content_length, "%d", total_len);
    esp_http_client_set_header(client, "Content-Length", content_length);
    
    // POSTリクエストの開始
    esp_err_t err = esp_http_client_open(client, total_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP接続の開始に失敗しました: %s", esp_err_to_name(err));
        ESP_LOGW(TAG, "APIサーバーに接続できません。ローカルで録音した音声を再生します。");
        esp_http_client_cleanup(client);
        free(chunk_buffer);
        free(header);
        free(footer);
        free(content_type);
        
        // APIサーバーに接続できない場合は、ローカルで再生する
        play_audio_file(RECORD_FILENAME);
        fclose(file);
        return;
    }
    
    // ヘッダーの送信
    int write_len = esp_http_client_write(client, header, header_len);
    if (write_len < 0) {
        ESP_LOGE(TAG, "ヘッダーの送信に失敗しました");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        free(chunk_buffer);
        free(header);
        free(footer);
        free(content_type);
        
        // 送信に失敗した場合は、ローカルで再生する
        play_audio_file(RECORD_FILENAME);
        fclose(file);
        return;
    }
    
    // ファイルデータをチャンクごとに読み込んで送信
    size_t bytes_read = 0;
    size_t total_read = 0;
    int chunks_sent = 0;
    
    // ファイルポインタを先頭に戻す
    fseek(file, 0, SEEK_SET);
    
    // チャンクごとに読み込んで送信
    while ((bytes_read = fread(chunk_buffer, 1, chunk_size, file)) > 0) {
        write_len = esp_http_client_write(client, chunk_buffer, bytes_read);
        
        if (write_len < 0) {
            ESP_LOGE(TAG, "ファイルデータの送信に失敗しました");
            break;
        }
        
        total_read += bytes_read;
        chunks_sent++;
        
        if (chunks_sent % 10 == 0) {
            ESP_LOGI(TAG, "転送進捗: %d/%ld バイト (%.1f%%)", 
                    (int)total_read, file_size, 
                    ((float)total_read / file_size) * 100.0f);
        }
        
        // チャンク間で少し遅延を入れてスタックオーバーフローを防止
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    
    fclose(file);
    
    if (total_read != file_size) {
        ESP_LOGW(TAG, "予想サイズとは異なるバイト数を読み込みました: %d vs %ld", (int)total_read, file_size);
    }
    
    // フッターの送信
    write_len = esp_http_client_write(client, footer, footer_len);
    if (write_len < 0) {
        ESP_LOGE(TAG, "フッターの送信に失敗しました");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        free(chunk_buffer);
        free(header);
        free(footer);
        free(content_type);
        
        // 送信に失敗した場合は、ローカルで再生する
        play_audio_file(RECORD_FILENAME);
        return;
    }
    
    // レスポンスの読み取り
    int content_length_response = esp_http_client_fetch_headers(client);
    if (content_length_response < 0) {
        ESP_LOGE(TAG, "HTTPヘッダーの読み取りに失敗しました");
        
        // ヘッダー読み取りに失敗した場合は、ローカルで再生する
        play_audio_file(RECORD_FILENAME);
    } else {
        // レスポンスボディを読み取る
        int status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTP ステータスコード: %d", status_code);
        
        if (status_code == 200) {
            // レスポンスボディバッファの割り当て
            char *response_buffer = malloc(content_length_response + 1);
            if (response_buffer) {
                int read_len = esp_http_client_read(client, response_buffer, content_length_response);
                if (read_len > 0) {
                    response_buffer[read_len] = 0;
                    ESP_LOGI(TAG, "レスポンス: %s", response_buffer);
                    
                    // JSONレスポンスの解析
                    cJSON *json = cJSON_Parse(response_buffer);
                    if (json) {
                        cJSON *success = cJSON_GetObjectItem(json, "success");
                        cJSON *message = cJSON_GetObjectItem(json, "message");
                        cJSON *audio_path = cJSON_GetObjectItem(json, "audio_path");
                        
                        if (cJSON_IsTrue(success)) {
                            ESP_LOGI(TAG, "バズの応答: %s", message->valuestring);
                            
                            if (audio_path) {
                                // 音声ファイルをダウンロードして再生
                                download_and_play_audio(audio_path->valuestring);
                            }
                        } else {
                            ESP_LOGE(TAG, "エラー: %s", message->valuestring);
                            
                            // エラーの場合は、ローカルで再生する
                            play_audio_file(RECORD_FILENAME);
                        }
                        
                        cJSON_Delete(json);
                    } else {
                        ESP_LOGE(TAG, "JSONの解析に失敗しました");
                        
                        // JSON解析に失敗した場合は、ローカルで再生する
                        play_audio_file(RECORD_FILENAME);
                    }
                } else {
                    // レスポンスの読み取りに失敗した場合は、ローカルで再生する
                    play_audio_file(RECORD_FILENAME);
                }
                free(response_buffer);
            } else {
                // メモリ割り当てに失敗した場合は、ローカルで再生する
                play_audio_file(RECORD_FILENAME);
            }
        } else {
            // ステータスコードが200以外の場合は、ローカルで再生する
            ESP_LOGE(TAG, "APIサーバーからエラーレスポンスを受信しました: %d", status_code);
            play_audio_file(RECORD_FILENAME);
        }
    }
    
    // クリーンアップ
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    free(chunk_buffer);
    free(header);
    free(footer);
    free(content_type);
}

/* 音声ファイルを再生する関数 */
void download_and_play_audio(const char *audio_path) {
    ESP_LOGI(TAG, "音声ファイルをダウンロードしています: %s", audio_path);
    
    // 完全なURLを構築
    char url[256];
    sprintf(url, "http://%s:%d%s", API_SERVER_HOST, API_SERVER_PORT, audio_path);
    ESP_LOGI(TAG, "ダウンロードURL: %s", url);
    
    // ダウンロードファイルを開く
    FILE *f = fopen(DOWNLOAD_FILENAME, "wb");
    if (f == NULL) {
        ESP_LOGE(TAG, "ダウンロードファイルを作成できませんでした");
        return;
    }
    
    // HTTPクライアント設定
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 30000,
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    
    // クライアントを直接開いてデータを読み込む
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTPクライアントを開くのに失敗しました: %s", esp_err_to_name(err));
        fclose(f);
        esp_http_client_cleanup(client);
        return;
    }
    
    // レスポンスヘッダーを読み取る
    int content_length = esp_http_client_fetch_headers(client);
    ESP_LOGI(TAG, "音声ファイルのサイズ: %d バイト", content_length);
    
    if (content_length > 0) {
        // データを読み込んでファイルに書き込む
        char *buffer = malloc(1024);
        if (buffer != NULL) {
            int read_len;
            size_t total_read = 0;
            while ((read_len = esp_http_client_read(client, buffer, 1024)) > 0) {
                fwrite(buffer, 1, read_len, f);
                total_read += read_len;
                ESP_LOGI(TAG, "ダウンロード進捗: %d / %d バイト", total_read, content_length);
            }
            ESP_LOGI(TAG, "合計 %d バイトをダウンロードしました", total_read);
            free(buffer);
        } else {
            ESP_LOGE(TAG, "バッファのメモリ割り当てに失敗しました");
        }
    } else {
        ESP_LOGE(TAG, "コンテンツ長が無効です: %d", content_length);
    }
    
    fclose(f);
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    
    ESP_LOGI(TAG, "ダウンロード完了、ファイル: %s", DOWNLOAD_FILENAME);
    
    // ダウンロードした音声ファイルを再生
    play_audio_file(DOWNLOAD_FILENAME);
}

/* 自発的なメッセージを取得する関数 */
void get_proactive_message(void) {
    ESP_LOGI(TAG, "自発的なメッセージを取得しています...");
    
    // URLを構築
    char url[128];
    sprintf(url, "http://%s:%d/generate_proactive", API_SERVER_HOST, API_SERVER_PORT);
    
    ESP_LOGI(TAG, "自発的メッセージURL: %s", url);
    
    // 念のためパスをクリア
    next_audio_to_download[0] = 0;
    
    // HTTPクライアント設定
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 30000, 
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    
    // GETリクエストの実行（レスポンスはイベントハンドラで処理）
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP GETリクエストに失敗: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
    
    // イベントハンドラで音声ファイルのパスが設定された場合、ダウンロードして再生
    if (strlen(next_audio_to_download) > 0) {
        ESP_LOGI(TAG, "音声ファイルのダウンロードを開始します: %s", next_audio_to_download);
        download_and_play_audio(next_audio_to_download);
    } else {
        ESP_LOGI(TAG, "ダウンロードする音声ファイルがありません");
    }
}

/* 音声ファイルを再生する関数 */
void play_audio_file(const char *filename) {
    ESP_LOGI(TAG, "音声ファイルを再生します: %s", filename);
    
    // LED点灯（再生中）
    gpio_set_level(LED_PIN, 0);
    
    // audio_playerコンポーネントを使用して再生
    esp_err_t ret = play_from_file(filename);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "再生に失敗しました");
    } else {
        ESP_LOGI(TAG, "再生完了");
    }
    
    // LEDを消灯
    gpio_set_level(LED_PIN, 0);
}

/* メインタスク - 自発的なメッセージを定期的に取得 */
static void main_task(void *pvParameters) {
    // 最初の自発的なメッセージを取得
    get_proactive_message();
    
    uint32_t last_proactive_check = 0;
    
    for (;;) {
        // 現在の時間を取得
        uint32_t current_time = esp_timer_get_time() / 1000000; // 秒単位
        
        // 10分ごとに自発的なメッセージをチェック（600秒 = 10分）
        if (current_time - last_proactive_check > 600) {
            get_proactive_message();
            last_proactive_check = current_time;
        }
        
        // 少し待機
        vTaskDelay(pdMS_TO_TICKS(5000)); // 5秒ごとにチェック
    }
}


// メモリ使用状況をログ出力する関数
static void log_memory_info(const char* message) {
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);
    ESP_LOGI(TAG, "%s - Free: %d bytes, Minimum free: %d bytes, Total allocated: %d bytes",
        message, info.total_free_bytes, info.minimum_free_bytes, info.total_allocated_bytes);
}

void app_main(void) {
    /* メモリ使用状況を確認 */
    log_memory_info("アプリケーション起動時");
    
    /* 初期化処理 */
    // NVS初期化
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // LEDピンのみ設定
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    
    // I2Sアクセス用のミューテックスを作成
    i2s_mutex = xSemaphoreCreateMutex();
    if (i2s_mutex == NULL) {
        ESP_LOGE(TAG, "ミューテックスの作成に失敗しました");
        return;
    }
    
    // WiFi初期化と接続
    ESP_LOGI(TAG, "ESP32-S3 バズ・ライトイヤー WiFi接続中...");
    ESP_ERROR_CHECK(wifi_init_sta());
    
    // オーディオプレーヤー初期化
    ESP_ERROR_CHECK(audio_player_init());
    
    // SDカードの初期化
    ESP_LOGI(TAG, "SDカードを初期化しています...");
    ret = sd_card_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SDカードの初期化に失敗しました。SPIFFSを代わりに使用します。");
        // SPIFFSの初期化（SDカード初期化失敗時のフォールバック）
        spiffs_init();
    } else {
        ESP_LOGI(TAG, "SDカード初期化完了");
    }

    // // NeoPixel初期化
    // neopixel_init();    

    // // LEDを赤色に設定
    // set_led_color(255, 0, 0);  // RGB: 赤色


    // // LEDを点滅させて準備完了を示す
    // for (int i = 0; i < 3; i++) {
    //     // 赤色に点灯 (R, G, B)
    //     set_led_color(255, 0, 0);
    //     vTaskDelay(pdMS_TO_TICKS(500));
    //     // 消灯
    //     set_led_color(0, 0, 0);
    //     vTaskDelay(pdMS_TO_TICKS(500));
    // }
    
    // メインタスクの作成 - プロアクティブなメッセージを取得
    xTaskCreate(main_task, "main_task", 8192, NULL, 5, NULL);
    
    // 音声検出タスクの作成（タスクハンドルを保存）
    xTaskCreate(vad_task, "vad_task", 4096, NULL, 10, &vad_task_handle);
    ESP_LOGI(TAG, "i2s_rx_handle: %p", i2s_rx_handle);
    ESP_LOGI(TAG, "vad_task_handle: %p", vad_task_handle);
    ESP_LOGI(TAG, "バズライトイヤーボット準備完了！音声検出モードで動作中...");
}
