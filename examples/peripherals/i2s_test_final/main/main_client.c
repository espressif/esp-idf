#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
#define SAMPLE_RATE     44100            // サンプリングレート (Hz)
#define SAMPLE_BITS     16               // サンプルビット数
#define AUDIO_CHANNELS  1                // モノラル
#define BUFFER_SIZE     1024             // バッファサイズ

/* ボタンとLED設定 */
#define BUTTON_PIN      GPIO_NUM_15      // ボタンピン
#define LED_PIN         GPIO_NUM_2       // LEDピン

/* 録音関連の設定 */
#define RECORD_TIME_MS  5000             // 録音時間 (ms)
#define RECORD_FILENAME "/spiffs/rec.wav"
#define DOWNLOAD_FILENAME "/spiffs/response.wav"

/* グローバル変数 */
static i2s_chan_handle_t i2s_handle = NULL;
static QueueHandle_t button_event_queue;
static bool recording_active = false;
// static bool emergency_stop = false;

/* 関数プロトタイプ */
void record_audio(void);
void send_audio_to_api(void);
void play_audio_file(const char *filename);
void download_and_play_audio(const char *audio_path);
void get_proactive_message(void);
// static void auto_record_task(void *pvParameters);

/* 関数プロトタイプ - 追加 */
static void led_blink_task(void *arg);


/* I2S初期化関数 */
esp_err_t i2s_init(void) {
    ESP_LOGI(TAG, "I2Sマイクを初期化しています...");
    
    // I2Sチャンネル設定
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &i2s_handle));
    
    // I2S標準モード設定
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = GPIO_NUM_NC,
            .bclk = I2S_MIC_BCK_IO,  // 新しいマイク用ピン定義
            .ws = I2S_MIC_WS_IO,     // 新しいマイク用ピン定義
            .dout = GPIO_NUM_NC,     // マイクでは出力を使わないのでNC
            .din = I2S_MIC_DI_IO,    // 新しいマイク用ピン定義
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(i2s_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(i2s_handle));
    
    ESP_LOGI(TAG, "I2Sマイク初期化完了");
    return ESP_OK;
}

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
                    
                    // 録音処理
                    record_audio();
                    
                    // APIに音声を送信
                    send_audio_to_api();
                    
                    // LED消灯
                    gpio_set_level(LED_PIN, 0);
                    
                    // 録音状態リセット
                    recording_active = false;
                }
            }
        }
    }
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
    for (int i = 0; i < 3; i++) {
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    // タスクを作成して録音中のLED点滅を処理
    TaskHandle_t led_task_handle = NULL;
    xTaskCreate(led_blink_task, "led_blink_task", 2048, NULL, 5, &led_task_handle);
    
    // audio_playerコンポーネントを使用して録音
    esp_err_t ret = record_to_file(RECORD_FILENAME, RECORD_TIME_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "録音に失敗しました");
    } else {
        ESP_LOGI(TAG, "録音完了");
    }
    
    // LED点灯（処理中）
    gpio_set_level(LED_PIN, 1);
}

// /* HTTPイベントハンドラ */
// esp_err_t http_event_handler(esp_http_client_event_t *evt) {
//     switch(evt->event_id) {
//         case HTTP_EVENT_ERROR:
//             ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
//             break;
//         case HTTP_EVENT_ON_CONNECTED:
//             ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
//             break;
//         case HTTP_EVENT_HEADER_SENT:
//             ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
//             break;
//         case HTTP_EVENT_ON_HEADER:
//             ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
//             break;
//         case HTTP_EVENT_ON_DATA:
//             // ここでデータ受信処理
//             if (!esp_http_client_is_chunked_response(evt->client)) {
//                 ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
//             }
//             break;
//         case HTTP_EVENT_ON_FINISH:
//             ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
//             break;
//         case HTTP_EVENT_DISCONNECTED:
//             ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
//             break;
//         case HTTP_EVENT_REDIRECT:
//             ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
//             break;
//     }
//     return ESP_OK;
// }

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
    
    // ファイルを読み込む
    char *file_buffer = malloc(file_size);
    if (!file_buffer) {
        ESP_LOGE(TAG, "メモリ割り当てに失敗しました");
        fclose(file);
        return;
    }
    
    fread(file_buffer, 1, file_size, file);
    fclose(file);
    
    // マルチパートフォームデータの境界
    const char *boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
    
    // マルチパートヘッダーの構築
    char *header = malloc(512);
    if (!header) {
        ESP_LOGE(TAG, "メモリ割り当てに失敗しました");
        free(file_buffer);
        return;
    }
    
    // マルチパートボディの構築
    sprintf(header, 
        "--%s\r\n"
        "Content-Disposition: form-data; name=\"audio\"; filename=\"audio.wav\"\r\n"
        "Content-Type: audio/wav\r\n\r\n",
        boundary);
    
    // 終了境界
    char *footer = malloc(128);
    if (!footer) {
        ESP_LOGE(TAG, "メモリ割り当てに失敗しました");
        free(file_buffer);
        free(header);
        return;
    }
    
    sprintf(footer, "\r\n--%s--\r\n", boundary);
    
    // 完全なコンテンツ長の計算
    int header_len = strlen(header);
    int footer_len = strlen(footer);
    int total_len = header_len + file_size + footer_len;
    
    // Content-Typeヘッダーの構築
    char *content_type = malloc(128);
    if (!content_type) {
        ESP_LOGE(TAG, "メモリ割り当てに失敗しました");
        free(file_buffer);
        free(header);
        free(footer);
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
    char content_length[32];
    sprintf(content_length, "%d", total_len);
    esp_http_client_set_header(client, "Content-Length", content_length);
    
    // POSTリクエストの開始
    esp_err_t err = esp_http_client_open(client, total_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP接続の開始に失敗しました: %s", esp_err_to_name(err));
        ESP_LOGW(TAG, "APIサーバーに接続できません。ローカルで録音した音声を再生します。");
        esp_http_client_cleanup(client);
        free(file_buffer);
        free(header);
        free(footer);
        free(content_type);
        
        // APIサーバーに接続できない場合は、ローカルで再生する
        play_audio_file(RECORD_FILENAME);
        return;
    }
    
    // ヘッダーの送信
    int write_len = esp_http_client_write(client, header, header_len);
    if (write_len < 0) {
        ESP_LOGE(TAG, "ヘッダーの送信に失敗しました");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        free(file_buffer);
        free(header);
        free(footer);
        free(content_type);
        
        // 送信に失敗した場合は、ローカルで再生する
        play_audio_file(RECORD_FILENAME);
        return;
    }
    
    // ファイルデータの送信（チャンクごと）
    const size_t chunk_size = 1024;
    for (size_t i = 0; i < file_size; i += chunk_size) {
        size_t current_chunk_size = ((i + chunk_size) > file_size) ? (file_size - i) : chunk_size;
        write_len = esp_http_client_write(client, &file_buffer[i], current_chunk_size);
        if (write_len < 0) {
            ESP_LOGE(TAG, "ファイルデータの送信に失敗しました");
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            free(file_buffer);
            free(header);
            free(footer);
            free(content_type);
            
            // 送信に失敗した場合は、ローカルで再生する
            play_audio_file(RECORD_FILENAME);
            return;
        }
    }
    
    // フッターの送信
    write_len = esp_http_client_write(client, footer, footer_len);
    if (write_len < 0) {
        ESP_LOGE(TAG, "フッターの送信に失敗しました");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        free(file_buffer);
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
    free(file_buffer);
    free(header);
    free(footer);
    free(content_type);
}

// /* 音声ファイルをダウンロードして再生する関数 */
// void download_and_play_audio(const char *audio_path) {
//     ESP_LOGI(TAG, "音声ファイルをダウンロードしています: %s", audio_path);
    
//     // 完全なURLを構築
//     char url[256];
//     sprintf(url, "http://%s:%d%s", API_SERVER_HOST, API_SERVER_PORT, audio_path);
    
//     // HTTPクライアント設定
//     esp_http_client_config_t config = {
//         .url = url,
//         .event_handler = http_event_handler,
//         .method = HTTP_METHOD_GET,
//         .timeout_ms = 30000, // タイムアウト時間
//     };
    
//     esp_http_client_handle_t client = esp_http_client_init(&config);
    
//     // GETリクエストの実行
//     esp_err_t err = esp_http_client_perform(client);
//     if (err == ESP_OK) {
//         int status_code = esp_http_client_get_status_code(client);
//         int content_length = esp_http_client_get_content_length(client);
        
//         ESP_LOGI(TAG, "HTTP GET ステータスコード = %d, コンテンツ長 = %d", status_code, content_length);
        
//         if (status_code == 200) {
//             // ダウンロードファイルを開く
//             FILE *f = fopen(DOWNLOAD_FILENAME, "wb");
//             if (f != NULL) {
//                 // ダウンロードバッファ
//                 char *buffer = malloc(1024);
//                 if (buffer != NULL) {
//                     // ファイルに書き込む
//                     esp_http_client_set_url(client, url);
//                     err = esp_http_client_open(client, 0);
//                     if (err == ESP_OK) {
//                         int read_len;
//                         while ((read_len = esp_http_client_read(client, buffer, 1024)) > 0) {
//                             fwrite(buffer, 1, read_len, f);
//                         }
//                     }
//                     free(buffer);
//                 }
//                 fclose(f);
                
//                 ESP_LOGI(TAG, "ダウンロード完了");
                
//                 // ダウンロードした音声ファイルを再生
//                 play_audio_file(DOWNLOAD_FILENAME);
//             } else {
//                 ESP_LOGE(TAG, "ダウンロードファイルを作成できませんでした");
//             }
//         }
//     } else {
//         ESP_LOGE(TAG, "HTTP GETリクエストに失敗: %s", esp_err_to_name(err));
//     }
    
//     esp_http_client_cleanup(client);
// }

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

// /* 自発的なメッセージを取得して再生する関数 */
// void get_proactive_message(void) {
//     ESP_LOGI(TAG, "自発的なメッセージを取得しています...");
    
//     // APIサーバーから自発的なメッセージを取得
    
//     // URLを構築
//     char url[128];
//     sprintf(url, "http://%s:%d/generate_proactive", API_SERVER_HOST, API_SERVER_PORT);
    
//     ESP_LOGI(TAG, "自発的メッセージURL: %s", url);
    
//     // HTTPクライアント設定
//     esp_http_client_config_t config = {
//         .url = url,
//         .event_handler = http_event_handler,
//         .method = HTTP_METHOD_GET,
//         .timeout_ms = 30000, 
//     };
    
//     esp_http_client_handle_t client = esp_http_client_init(&config);
    
//     // GETリクエストの実行
//     esp_err_t err = esp_http_client_perform(client);
//     if (err == ESP_OK) {
//         int status_code = esp_http_client_get_status_code(client);
//         int content_length = esp_http_client_get_content_length(client);
        
//         ESP_LOGI(TAG, "HTTP GET ステータスコード = %d, コンテンツ長 = %d", status_code, content_length);
        
//         if (status_code == 200 && content_length > 0) {
//             // レスポンスを読み取る
//             char *response_buffer = malloc(content_length + 1);
//             if (response_buffer) {
//                 int read_len = esp_http_client_read_response(client, response_buffer, content_length);
//                 if (read_len > 0) {
//                     response_buffer[read_len] = 0;
                    
//                     // JSONレスポンスの解析
//                     cJSON *json = cJSON_Parse(response_buffer);
//                     if (json) {
//                         cJSON *success = cJSON_GetObjectItem(json, "success");
//                         cJSON *message = cJSON_GetObjectItem(json, "message");
//                         cJSON *audio_path = cJSON_GetObjectItem(json, "audio_path");
                        
//                         if (cJSON_IsTrue(success)) {
//                             ESP_LOGI(TAG, "バズの自発的なメッセージ: %s", message->valuestring);
                            
//                             if (audio_path) {
//                                 // 音声ファイルをダウンロードして再生
//                                 download_and_play_audio(audio_path->valuestring);
//                             }
//                         }
                        
//                         cJSON_Delete(json);
//                     } else {
//                         ESP_LOGE(TAG, "JSONの解析に失敗しました");
//                     }
//                 }
//                 free(response_buffer);
//             }
//         }
//     } else {
//         ESP_LOGE(TAG, "HTTP GETリクエストに失敗: %s", esp_err_to_name(err));
//     }
    
//     esp_http_client_cleanup(client);
// }

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
    gpio_set_level(LED_PIN, 1);
    
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

void app_main(void) {
    /* 初期化処理 */
    // NVS初期化
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // GPIOピンの設定
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    
    // ボタンGPIOピン設定
    io_conf.pin_bit_mask = (1ULL << BUTTON_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;  // 立ち下がりエッジで割り込み
    gpio_config(&io_conf);
    
    // ボタン割り込みキューの作成
    button_event_queue = xQueueCreate(10, sizeof(uint32_t));
    
    // ボタン割り込みハンドラの登録
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, (void*)BUTTON_PIN);
    
    // WiFi初期化と接続 (wifi_manager コンポーネントを使用)
    ESP_LOGI(TAG, "ESP32-S3 バズ・ライトイヤー WiFi接続中...");
    ESP_ERROR_CHECK(wifi_init_sta());
    
    // オーディオプレーヤー初期化
    ESP_ERROR_CHECK(audio_player_init());
    
    // SPIFFSの初期化
    spiffs_init();
    
    // LEDを点滅させて準備完了を示す
    for (int i = 0; i < 3; i++) {
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    // ボタンタスクの作成
    xTaskCreate(button_task, "button_task", 4096, NULL, 10, NULL);
    
    // メインタスクの作成 - APIサーバーからの自発的なメッセージを取得
    xTaskCreate(main_task, "main_task", 8192, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "バズライトイヤーボット準備完了！ボタンを押して録音と再生を開始してください。");
}
