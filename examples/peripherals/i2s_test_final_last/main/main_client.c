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
#include "esp_https_ota.h"
#include "nvs_flash.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/i2s_std.h"
#include "driver/i2s_pdm.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "cJSON.h"

static const char *TAG = "buzz-client";

/* WiFi設定 */
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"
#define WIFI_MAXIMUM_RETRY  5

/* APIサーバーの設定 */
#define API_SERVER_HOST "192.168.1.100"  // APIサーバーのIPアドレス
#define API_SERVER_PORT 8000             // APIサーバーのポート

/* I2S設定 - マイク用 */
#define I2S_BCK_IO      GPIO_NUM_6       // ビットクロック 
#define I2S_WS_IO       GPIO_NUM_5       // ワードセレクト
#define I2S_DI_IO       GPIO_NUM_4       // データイン（マイク入力）
#define I2S_DO_IO       GPIO_NUM_16      // データアウト（スピーカー出力）

/* オーディオ設定 */
#define SAMPLE_RATE     16000            // サンプリングレート (Hz)
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

/* FreeRTOSイベントグループビット */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

/* グローバル変数 */
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;
static i2s_chan_handle_t i2s_handle = NULL;
static QueueHandle_t button_event_queue;
static bool recording_active = false;

/* WAVファイルヘッダー構造体 */
typedef struct {
    // RIFF chunk
    char riff_header[4];     // "RIFF"
    uint32_t wav_size;       // ファイルサイズ - 8
    char wave_header[4];     // "WAVE"
    
    // fmt chunk
    char fmt_header[4];      // "fmt "
    uint32_t fmt_chunk_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    
    // data chunk
    char data_header[4];     // "data"
    uint32_t data_bytes;
} wav_header_t;

/* WAVヘッダーを初期化する関数 */
void init_wav_header(wav_header_t *header, uint32_t data_size) {
    // RIFF チャンク
    memcpy(header->riff_header, "RIFF", 4);
    header->wav_size = data_size + sizeof(wav_header_t) - 8;
    memcpy(header->wave_header, "WAVE", 4);
    
    // fmt チャンク
    memcpy(header->fmt_header, "fmt ", 4);
    header->fmt_chunk_size = 16;
    header->audio_format = 1; // PCM = 1
    header->num_channels = AUDIO_CHANNELS;
    header->sample_rate = SAMPLE_RATE;
    header->byte_rate = SAMPLE_RATE * AUDIO_CHANNELS * (SAMPLE_BITS / 8);
    header->block_align = AUDIO_CHANNELS * (SAMPLE_BITS / 8);
    header->bits_per_sample = SAMPLE_BITS;
    
    // data チャンク
    memcpy(header->data_header, "data", 4);
    header->data_bytes = data_size;
}

/* WiFiイベントハンドラ */
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < WIFI_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "WiFi接続をリトライ中...");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "WiFi接続に失敗しました");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "IPアドレス取得: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/* WiFi初期化関数 */
void wifi_init_sta(void) {
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi初期化完了");

    /* WiFiイベントビットを待機 */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "WiFi接続成功: %s", WIFI_SSID);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "WiFi接続失敗: %s", WIFI_SSID);
    } else {
        ESP_LOGE(TAG, "予期せぬイベント");
    }
}

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
    
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(i2s_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(i2s_handle));
    
    ESP_LOGI(TAG, "I2Sマイク初期化完了");
    return ESP_OK;
}

/* SPIFFS初期化関数 */
esp_err_t spiffs_init(void) {
    ESP_LOGI(TAG, "SPIFFSを初期化しています...");
    
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "SPIFFSのマウントに失敗しました。パーティションが見つからないか、すでにマウントされています。");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "SPIFFSパーティションが見つかりません");
        } else {
            ESP_LOGE(TAG, "SPIFFSのマウント中にエラーが発生しました: %s", esp_err_to_name(ret));
        }
        return ret;
    }
    
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "SPIFFS: 合計: %d KB, 使用中: %d KB, 空き: %d KB", 
                 total / 1024, used / 1024, (total - used) / 1024);
    }
    
    ESP_LOGI(TAG, "SPIFFS初期化完了");
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

/* 録音関数 */
void record_audio(void) {
    ESP_LOGI(TAG, "録音を開始します...");
    
    // 録音ファイルを開く
    FILE *f = fopen(RECORD_FILENAME, "wb");
    if (f == NULL) {
        ESP_LOGE(TAG, "録音ファイルを開けませんでした");
        return;
    }
    
    // WAVヘッダー用にスペースを確保
    wav_header_t header = {0};
    fseek(f, sizeof(wav_header_t), SEEK_SET);
    
    // 録音バッファ
    int16_t *samples = malloc(BUFFER_SIZE * sizeof(int16_t));
    if (samples == NULL) {
        ESP_LOGE(TAG, "メモリ割り当てに失敗しました");
        fclose(f);
        return;
    }
    
    size_t bytes_written = 0;
    uint32_t record_start_time = esp_timer_get_time() / 1000; // 現在時刻（ミリ秒）
    
    // 録音ループ
    while ((esp_timer_get_time() / 1000) - record_start_time < RECORD_TIME_MS) {
        // LEDを点滅（録音中の視覚的フィードバック）
        if (((esp_timer_get_time() / 1000) - record_start_time) % 500 < 250) {
            gpio_set_level(LED_PIN, 1);
        } else {
            gpio_set_level(LED_PIN, 0);
        }
        
        // I2Sからデータを読み込む
        size_t bytes_read = 0;
        esp_err_t ret = i2s_channel_read(i2s_handle, samples, BUFFER_SIZE * sizeof(int16_t), &bytes_read, portMAX_DELAY);
        
        if (ret == ESP_OK && bytes_read > 0) {
            // マイクの音量レベルを計算
            int16_t max_sample = 0;
            for (int i = 0; i < bytes_read / 2; i++) {
                if (abs(samples[i]) > abs(max_sample)) {
                    max_sample = samples[i];
                }
            }
            
            // 音量レベルを0-100%で表示
            int volume_percent = (abs(max_sample) * 100) / 32768;
            if (volume_percent > 5) { // 小さすぎる音は表示しない
                ESP_LOGI(TAG, "音量: %d%%", volume_percent);
            }
            
            // ファイルに書き込む
            size_t written = fwrite(samples, 1, bytes_read, f);
            if (written != bytes_read) {
                ESP_LOGE(TAG, "ファイルへの書き込みエラー");
            }
            bytes_written += written;
        }
    }
    
    // 録音終了
    gpio_set_level(LED_PIN, 1); // 処理中は点灯
    ESP_LOGI(TAG, "録音完了");
    
    // WAVヘッダーを準備して書き込む
    init_wav_header(&header, bytes_written);
    fseek(f, 0, SEEK_SET);
    fwrite(&header, sizeof(wav_header_t), 1, f);
    
    fclose(f);
    free(samples);
    
    ESP_LOGI(TAG, "録音完了: %u バイトを保存しました", bytes_written);
}

/* HTTPイベントハンドラ */
esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            // ここでデータ受信処理
            if (!esp_http_client_is_chunked_response(evt->client)) {
                ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
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
    
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,
        .method = HTTP_METHOD_POST,
        .buffer_size = 2048, // バッファサイズを増やす
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
        esp_http_client_cleanup(client);
        free(file_buffer);
        free(header);
        free(footer);
        free(content_type);
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
        return;
    }
    
    // レスポンスの読み取り
    int content_length_response = esp_http_client_fetch_headers(client);
    if (content_length_response < 0) {
        ESP_LOGE(TAG, "HTTPヘッダーの読み取りに失敗しました");
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
                        }
                        
                        cJSON_Delete(json);
                    } else {
                        ESP_LOGE(TAG, "JSONの解析に失敗しました");
                    }
                }
                free(response_buffer);
            }
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

/* 音声ファイルをダウンロードして再生する関数 */
void download_and_play_audio(const char *audio_path) {
    ESP_LOGI(TAG, "音声ファイルをダウンロードしています: %s", audio_path);
    
    // 完全なURLを構築
    char url[256];
    sprintf(url, "http://%s:%d%s", API_SERVER_HOST, API_SERVER_PORT, audio_path);
    
    // HTTPクライアント設定
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,
        .method = HTTP_METHOD_GET,
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    
    // GETリクエストの実行
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        int content_length = esp_http_client_get_content_length(client);
        
        ESP_LOGI(TAG, "HTTP GET ステータスコード = %d, コンテンツ長 = %d", status_code, content_length);
        
        if (status_code == 200) {
            // ダウンロードファイルを開く
            FILE *f = fopen(DOWNLOAD_FILENAME, "wb");
            if (f != NULL) {
                // ダウンロードバッファ
                char *buffer = malloc(1024);
                if (buffer != NULL) {
                    // ファイルに書き込む
                    esp_http_client_set_url(client, url);
                    err = esp_http_client_open(client, 0);
                    if (err == ESP_OK) {
                        int read_len;
                        while ((read_len = esp_http_client_read(client, buffer, 1024)) > 0) {
                            fwrite(buffer, 1, read_len, f);
                        }
                    }
                    free(buffer);
                }
                fclose(f);
                
                ESP_LOGI(TAG, "ダウンロード完了");
                
                // ダウンロードした音声ファイルを再生
                play_audio_file(DOWNLOAD_FILENAME);
            } else {
                ESP_LOGE(TAG, "ダウンロードファイルを作成できませんでした");
            }
        }
    } else {
        ESP_LOGE(TAG, "HTTP GETリクエストに失敗: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
}

/* 自発的なメッセージを取得して再生する関数 */
void get_proactive_message(void) {
    ESP_LOGI(TAG, "自発的なメッセージを取得しています...");
    
    // URLを構築
    char url[128];
    sprintf(url, "http://%s:%d/generate_proactive", API_SERVER_HOST, API_SERVER_PORT);
    
    // HTTPクライアント設定
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,
        .method = HTTP_METHOD_GET,
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    
    // GETリクエストの実行
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        int content_length = esp_http_client_get_content_length(client);
        
        ESP_LOGI(TAG, "HTTP GET ステータスコード = %d, コンテンツ長 = %d", status_code, content_length);
        
        if (status_code == 200 && content_length > 0) {
            // レスポンスを読み取る
            char *response_buffer = malloc(content_length + 1);
            if (response_buffer) {
                int read_len = esp_http_client_read_response(client, response_buffer, content_length);
                if (read_len > 0) {
                    response_buffer[read_len] = 0;
                    
                    // JSONレスポンスの解析
                    cJSON *json = cJSON_Parse(response_buffer);
                    if (json) {
                        cJSON *success = cJSON_GetObjectItem(json, "success");
                        cJSON *message = cJSON_GetObjectItem(json, "message");
                        cJSON *audio_path = cJSON_GetObjectItem(json, "audio_path");
                        
                        if (cJSON_IsTrue(success)) {
                            ESP_LOGI(TAG, "バズの自発的なメッセージ: %s", message->valuestring);
                            
                            if (audio_path) {
                                // 音声ファイルをダウンロードして再生
                                download_and_play_audio(audio_path->valuestring);
                            }
                        }
                        
                        cJSON_Delete(json);
                    }
                }
                free(response_buffer);
            }
        }
    } else {
        ESP_LOGE(TAG, "HTTP GETリクエストに失敗: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
}

/* 音声ファイルを再生する関数 */
void play_audio_file(const char *filename) {
    ESP_LOGI(TAG, "音声ファイルを再生します: %s", filename);
    
    // ここでは、I2S出力を使った実際の再生処理を実装します
    // この実装はハードウェア依存となるため、必要に応じて調整してください
    
    // ファイルを開く
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        ESP_LOGE(TAG, "ファイルを開けませんでした");
        return;
    }
    
    // WAVヘッダーをスキップ
    wav_header_t header;
    fread(&header, 1, sizeof(wav_header_t), f);
    
    // I2Sチャンネルを送信モードに設定
    // 注: 実際の実装では、既存のI2Sチャンネルを閉じて新しく設定し直す必要があるかもしれません
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_MASTER);
    i2s_chan_handle_t tx_handle;
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &tx_handle, NULL));
    
    i2s_std_config_t tx_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(header.sample_rate),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(header.bits_per_sample, 
                                                        header.num_channels == 1 ? I2S_SLOT_MODE_MONO : I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = GPIO_NUM_NC,
            .bclk = I2S_BCK_IO,
            .ws = I2S_WS_IO,
            .dout = I2S_DO_IO,
            .din = GPIO_NUM_NC,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, &tx_std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));
    
    // バッファの割り当て
    const size_t buf_size = 1024;
    uint8_t *buffer = malloc(buf_size);
    if (!buffer) {
        ESP_LOGE(TAG, "メモリ割り当てに失敗しました");
        fclose(f);
        i2s_channel_disable(tx_handle);
        i2s_del_channel(tx_handle);
        return;
    }
    
    // LED点灯（再生中）
    gpio_set_level(LED_PIN, 1);
    
    // ファイルを読み込み、I2Sに送信
    size_t bytes_read;
    size_t bytes_written;
    while ((bytes_read = fread(buffer, 1, buf_size, f)) > 0) {
        ESP_ERROR_CHECK(i2s_channel_write(tx_handle, buffer, bytes_read, &bytes_written, portMAX_DELAY));
        if (bytes_written != bytes_read) {
            ESP_LOGE(TAG, "I2S書き込みエラー");
        }
    }
    
    // クリーンアップ
    fclose(f);
    free(buffer);
    
    // 少し待機して、すべてのデータがI2Sバッファから送信されるのを待つ
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // I2Sチャンネルの無効化と削除
    i2s_channel_disable(tx_handle);
    i2s_del_channel(tx_handle);
    
    // LEDを消灯
    gpio_set_level(LED_PIN, 0);
    
    ESP_LOGI(TAG, "再生完了");
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
    
    // WiFi初期化と接続
    ESP_LOGI(TAG, "ESP32-S3 バズ・ライトイヤーAPI通信テスト");
    wifi_init_sta();
    
    // I2S初期化
    i2s_init();
    
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
    
    // メインタスクの作成
    xTaskCreate(main_task, "main_task", 8192, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "バズライトイヤーボット準備完了！ボタンを押して会話を開始してください。");
}