#include <string.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "audio_player.h"

static const char *TAG = "audio-player";

/* I2S設定 - マイク用 */
// #define I2S_BCK_IO      GPIO_NUM_6  // ビットクロック 
// #define I2S_WS_IO       GPIO_NUM_5  // ワードセレクト
// #define I2S_DI_IO       GPIO_NUM_4  // データイン（マイク入力）
// #define I2S_DO_IO       GPIO_NUM_16 // データアウト（スピーカー出力）

/* I2S設定 - マイク用 */
#define I2S_MIC_BCK_IO      GPIO_NUM_6  // ビットクロック（マイク用）
#define I2S_MIC_WS_IO       GPIO_NUM_5  // ワードセレクト（マイク用）
#define I2S_MIC_DI_IO       GPIO_NUM_4  // データイン（マイク入力）

/* I2S設定 - スピーカー用 */
#define I2S_SPK_BCK_IO      GPIO_NUM_3  // ビットクロック（スピーカー用）
#define I2S_SPK_WS_IO       GPIO_NUM_2  // ワードセレクト（スピーカー用）
#define I2S_SPK_DO_IO       GPIO_NUM_1  // データアウト（スピーカー出力）

/* オーディオ設定 */
#define SAMPLE_RATE     44100        // サンプリングレート (Hz)
#define SAMPLE_BITS     16          // サンプルビット数
#define AUDIO_CHANNELS  1           // モノラル
#define BUFFER_SIZE     1024        // バッファサイズ

/* I2Sハンドル */
static i2s_chan_handle_t i2s_rx_handle = NULL;
static i2s_chan_handle_t i2s_tx_handle = NULL;

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

/* オーディオプレーヤー初期化 */
esp_err_t audio_player_init(void) {
    ESP_LOGI(TAG, "オーディオプレーヤーを初期化しています...");
    
    // I2S受信チャンネル設定
    i2s_chan_config_t rx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&rx_chan_cfg, &i2s_rx_handle, NULL));
    
    // I2S送信チャンネル設定
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &i2s_tx_handle, NULL));
    
    // I2S標準モード設定（受信用）
    i2s_std_config_t rx_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = GPIO_NUM_NC,
            .bclk = I2S_MIC_BCK_IO,
            .ws = I2S_MIC_WS_IO,
            .dout = GPIO_NUM_NC,
            .din = I2S_MIC_DI_IO,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    
    // I2S標準モード設定（送信用）
    i2s_std_config_t tx_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = GPIO_NUM_NC,
            .bclk = I2S_SPK_BCK_IO,
            .ws = I2S_SPK_WS_IO,
            .dout = I2S_SPK_DO_IO,
            .din = GPIO_NUM_NC,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(i2s_rx_handle, &rx_std_cfg));
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(i2s_tx_handle, &tx_std_cfg));
    
    ESP_LOGI(TAG, "オーディオプレーヤー初期化完了");
    return ESP_OK;
}

/* ファイルに録音する関数 */
esp_err_t record_to_file(const char *filename, int duration_ms) {
    ESP_LOGI(TAG, "録音を開始します...");
    
    // I2S受信チャンネルを有効化
    ESP_ERROR_CHECK(i2s_channel_enable(i2s_rx_handle));
    
    // 録音ファイルを開く
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        ESP_LOGE(TAG, "録音ファイルを開けませんでした");
        i2s_channel_disable(i2s_rx_handle);
        return ESP_FAIL;
    }
    
    // WAVヘッダー用にスペースを確保
    wav_header_t header = {0};
    fseek(f, sizeof(wav_header_t), SEEK_SET);
    
    // 録音バッファ
    int16_t *samples = malloc(BUFFER_SIZE * sizeof(int16_t));
    if (samples == NULL) {
        ESP_LOGE(TAG, "メモリ割り当てに失敗しました");
        fclose(f);
        i2s_channel_disable(i2s_rx_handle);
        return ESP_FAIL;
    }
    
    size_t bytes_written = 0;
    uint32_t record_start_time = esp_timer_get_time() / 1000; // 現在時刻（ミリ秒）
    
    // 録音ループ
    while ((esp_timer_get_time() / 1000) - record_start_time < duration_ms) {
        // I2Sからデータを読み込む
        size_t bytes_read = 0;
        esp_err_t ret = i2s_channel_read(i2s_rx_handle, samples, BUFFER_SIZE * sizeof(int16_t), &bytes_read, portMAX_DELAY);
        
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
    
    // WAVヘッダーを準備して書き込む
    init_wav_header(&header, bytes_written);
    fseek(f, 0, SEEK_SET);
    fwrite(&header, sizeof(wav_header_t), 1, f);
    
    fclose(f);
    free(samples);
    
    // I2S受信チャンネルを無効化
    i2s_channel_disable(i2s_rx_handle);
    
    ESP_LOGI(TAG, "録音完了: %u バイトを保存しました", bytes_written);
    return ESP_OK;
}

/* ファイルから再生する関数 */
esp_err_t play_from_file(const char *filename) {
    ESP_LOGI(TAG, "音声ファイルを再生します: %s", filename);
    
    // ファイルを開く
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        ESP_LOGE(TAG, "ファイルを開けませんでした");
        return ESP_FAIL;
    }
    
    // WAVヘッダーを読み込む
    wav_header_t header;
    fread(&header, 1, sizeof(wav_header_t), f);
    ESP_LOGI(TAG, "音声ファイル情報: サンプルレート=%u Hz, チャンネル数=%u, ビット数=%u", 
          header.sample_rate, header.num_channels, header.bits_per_sample);
    
    // I2S送信チャンネルを有効化
    ESP_ERROR_CHECK(i2s_channel_enable(i2s_tx_handle));
    
    // バッファの割り当て
    const size_t buf_size = 1024;
    uint8_t *buffer = malloc(buf_size);
    if (!buffer) {
        ESP_LOGE(TAG, "メモリ割り当てに失敗しました");
        fclose(f);
        i2s_channel_disable(i2s_tx_handle);
        return ESP_FAIL;
    }
    
    // ファイルを読み込み、I2Sに送信
    size_t bytes_read;
    size_t bytes_written;
    while ((bytes_read = fread(buffer, 1, buf_size, f)) > 0) {
        ESP_ERROR_CHECK(i2s_channel_write(i2s_tx_handle, buffer, bytes_read, &bytes_written, portMAX_DELAY));
        if (bytes_written != bytes_read) {
            ESP_LOGE(TAG, "I2S書き込みエラー");
        }
    }
    
    // クリーンアップ
    fclose(f);
    free(buffer);
    
    // 少し待機して、すべてのデータがI2Sバッファから送信されるのを待つ
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // I2S送信チャンネルの無効化
    i2s_channel_disable(i2s_tx_handle);
    
    ESP_LOGI(TAG, "再生完了");
    return ESP_OK;
}
