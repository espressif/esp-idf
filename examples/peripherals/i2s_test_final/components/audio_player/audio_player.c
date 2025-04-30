#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "audio_player.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"

static const char *TAG = "audio-player";

/* SDカード関連の定義 */
#define SD_MOUNT_POINT "/sdcard"
#define SD_PIN_NUM_MISO 8   // XIAO ESP32S3 Sense MISOピン
#define SD_PIN_NUM_MOSI 9   // XIAO ESP32S3 Sense MOSIピン
#define SD_PIN_NUM_CLK  7   // XIAO ESP32S3 Sense SCKピン
#define SD_PIN_NUM_CS   21  // XIAO ESP32S3 Sense CSピン

/* SDカードハンドル */
static sdmmc_card_t *card = NULL;

/* I2S設定 - マイク用 */
// #define I2S_BCK_IO      GPIO_NUM_6  // ビットクロック 
// #define I2S_WS_IO       GPIO_NUM_5  // ワードセレクト
// #define I2S_DI_IO       GPIO_NUM_4  // データイン（マイク入力）
// #define I2S_DO_IO       GPIO_NUM_16 // データアウト（スピーカー出力）

/* PDM マイク設定 - XIAO ESP32S3 Sense用 */
#define PDM_MIC_CLK_IO      GPIO_NUM_42  // PDMクロック
#define PDM_MIC_DIN_IO      GPIO_NUM_41  // PDMデータイン（マイク入力）

/* I2S設定 - スピーカー用 */
#define I2S_SPK_BCK_IO      GPIO_NUM_3  // ビットクロック（スピーカー用）
#define I2S_SPK_WS_IO       GPIO_NUM_2  // ワードセレクト（スピーカー用）
#define I2S_SPK_DO_IO       GPIO_NUM_1  // データアウト（スピーカー出力）

/* オーディオ設定 */
#define MIC_SAMPLE_RATE    16000    // マイクのサンプリングレート (Hz) - PDMマイクに最適
#define SPK_SAMPLE_RATE    16000    // スピーカーのサンプリングレート (Hz)
#define SAMPLE_BITS        16       // サンプルビット数
#define AUDIO_CHANNELS     1        // モノラル
#define BUFFER_SIZE        1024     // バッファサイズ

/* I2Sハンドル */
i2s_chan_handle_t i2s_rx_handle = NULL;
i2s_chan_handle_t i2s_tx_handle = NULL;
bool i2s_rx_enabled = false; // チャンネルが有効化されているかを追跡するフラグ

/* PDMサンプル処理関数 - DCオフセット除去と異常値処理 */
static void process_pdm_samples(int16_t *samples, size_t sample_count) {
    // DCオフセット（平均値）を計算
    int32_t sum = 0;
    for (int i = 0; i < sample_count; i++) {
        sum += samples[i];
    }
    int16_t dc_offset = sum / sample_count;
    
    // DCオフセットを取り除き、値を正規化
    for (int i = 0; i < sample_count; i++) {
        // DCオフセットを除去
        samples[i] -= dc_offset;
        
        // 異常値をチェック（-32768など）
        if (samples[i] == INT16_MIN) {
            samples[i] = 0; // 異常値を0に
        }
    }
}

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
    header->sample_rate = MIC_SAMPLE_RATE; // PDMマイクのサンプルレート
    header->byte_rate = MIC_SAMPLE_RATE * AUDIO_CHANNELS * (SAMPLE_BITS / 8);
    header->block_align = AUDIO_CHANNELS * (SAMPLE_BITS / 8);
    header->bits_per_sample = SAMPLE_BITS;
    
    // data チャンク
    memcpy(header->data_header, "data", 4);
    header->data_bytes = data_size;
}

/* オーディオプレーヤー初期化 - 詳細デバッグ付き */
esp_err_t audio_player_init(void) {
    ESP_LOGI(TAG, "オーディオプレーヤーを初期化しています...");
    
    // I2S受信チャンネル設定 - DMAバッファサイズを最適化
    i2s_chan_config_t rx_chan_cfg = {
        .id = I2S_NUM_0,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 8,        // DMA記述子の数を増やす（デフォルトは3）
        .dma_frame_num = 512,     // DMAバッファのフレーム数を増やす
        .auto_clear = false,      // 自動クリアを無効化
        .intr_priority = 0,       // 割り込み優先度（デフォルト）
    };
    
    // 詳細設定のデバッグ出力
    ESP_LOGI(TAG, "I2S RX チャンネル設定:");
    ESP_LOGI(TAG, "  - ID: %d, ロール: MASTER, DMA バッファ数: %d, DMA バッファサイズ: %d",
             rx_chan_cfg.id, rx_chan_cfg.dma_desc_num, rx_chan_cfg.dma_frame_num);
    
    esp_err_t ret = i2s_new_channel(&rx_chan_cfg, NULL, &i2s_rx_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2S RX チャンネル作成エラー: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "I2S RX チャンネル作成成功: ハンドル = %p", i2s_rx_handle);
    
    // I2S送信チャンネル設定 - DMAバッファサイズを最適化
    i2s_chan_config_t tx_chan_cfg = {
        .id = I2S_NUM_1,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 8,        // DMA記述子の数を増やす（デフォルトは3）
        .dma_frame_num = 512,     // DMAバッファのフレーム数を増やす
        .auto_clear = false,      // 自動クリアを無効化
        .intr_priority = 0,       // 割り込み優先度（デフォルト）
    };
    
    ESP_LOGI(TAG, "I2S TX チャンネル設定:");
    ESP_LOGI(TAG, "  - ID: %d, ロール: MASTER, DMA バッファ数: %d, DMA バッファサイズ: %d",
             tx_chan_cfg.id, tx_chan_cfg.dma_desc_num, tx_chan_cfg.dma_frame_num);
    
    ret = i2s_new_channel(&tx_chan_cfg, &i2s_tx_handle, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2S TX チャンネル作成エラー: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "I2S TX チャンネル作成成功: ハンドル = %p", i2s_tx_handle);
    
    // PDMモード設定（受信用）- 最適化設定
    i2s_pdm_rx_config_t pdm_rx_cfg = {
        .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(MIC_SAMPLE_RATE),
        .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = PDM_MIC_CLK_IO,
            .din = PDM_MIC_DIN_IO,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };
    
    // スロットマスクを左チャンネルのみに設定
    pdm_rx_cfg.slot_cfg.slot_mask = I2S_PDM_SLOT_LEFT;
    
    // PDM RXクロックとスロット設定のデバッグ出力
    ESP_LOGI(TAG, "PDM RX クロック設定:");
    ESP_LOGI(TAG, "  - サンプルレート: %d Hz", 
             pdm_rx_cfg.clk_cfg.sample_rate_hz);
    ESP_LOGI(TAG, "PDM RX スロット設定:");
    ESP_LOGI(TAG, "  - データビット: %d, スロットモード: %s", 
             pdm_rx_cfg.slot_cfg.data_bit_width, 
             (pdm_rx_cfg.slot_cfg.slot_mode == I2S_SLOT_MODE_MONO) ? "MONO" : "STEREO");
    ESP_LOGI(TAG, "PDM RX GPIO設定:");
    ESP_LOGI(TAG, "  - CLK: GPIO%d, DIN: GPIO%d", 
             pdm_rx_cfg.gpio_cfg.clk, pdm_rx_cfg.gpio_cfg.din);
    
    // I2S標準モード設定（送信用）
    i2s_std_config_t tx_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SPK_SAMPLE_RATE),
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
    
    // TXクロックとスロット設定のデバッグ出力
    ESP_LOGI(TAG, "I2S TX クロック設定:");
    ESP_LOGI(TAG, "  - サンプルレート: %d Hz, マスタークロック倍率: %d", 
             tx_std_cfg.clk_cfg.sample_rate_hz, tx_std_cfg.clk_cfg.mclk_multiple);
    ESP_LOGI(TAG, "I2S TX スロット設定:");
    ESP_LOGI(TAG, "  - データビット: %d, スロットモード: %s, MSB/LSB: %s", 
             tx_std_cfg.slot_cfg.data_bit_width, 
             (tx_std_cfg.slot_cfg.slot_mode == I2S_SLOT_MODE_MONO) ? "MONO" : "STEREO",
             tx_std_cfg.slot_cfg.bit_shift ? "LSB" : "MSB");
    ESP_LOGI(TAG, "I2S TX GPIO設定:");
    ESP_LOGI(TAG, "  - BCLK: GPIO%d, WS: GPIO%d, DOUT: GPIO%d", 
             tx_std_cfg.gpio_cfg.bclk, tx_std_cfg.gpio_cfg.ws, tx_std_cfg.gpio_cfg.dout);
    
    // チャンネルモード初期化 - PDMモード
    ret = i2s_channel_init_pdm_rx_mode(i2s_rx_handle, &pdm_rx_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PDM RX チャンネルモード初期化エラー: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "PDM RX チャンネルモード初期化成功");
    
    ret = i2s_channel_init_std_mode(i2s_tx_handle, &tx_std_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2S TX チャンネルモード初期化エラー: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "I2S TX チャンネルモード初期化成功");
    
    // 初期化時にはチャンネルを有効化しない
    // 実際の使用時に有効化する
    i2s_rx_enabled = false;
    
    ESP_LOGI(TAG, "オーディオプレーヤー初期化完了");
    return ESP_OK;
}

/* ファイルに録音する関数 - 無音検出機能付き、ミューテックス使用 */
esp_err_t record_to_file(const char *filename, int duration_ms) {
    ESP_LOGI(TAG, "録音を開始します...");
    
    // // ミューテックスを取得
    // extern SemaphoreHandle_t i2s_mutex;
    // if (xSemaphoreTake(i2s_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
    //     ESP_LOGE(TAG, "録音のためのI2Sアクセス権を取得できませんでした");
    //     return ESP_FAIL;
    // }
    // ミューテックスを取得
    extern SemaphoreHandle_t i2s_mutex;
    ESP_LOGI(TAG, "録音関数: ミューテックス取得を試みます");
    
    if (xSemaphoreTake(i2s_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGE(TAG, "録音関数: ミューテックス取得失敗");
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "録音関数: ミューテックス取得成功");
    
    // I2S状態確認
    ESP_LOGI(TAG, "録音関数: I2S状態 - i2s_rx_handle=%p, i2s_rx_enabled=%d", i2s_rx_handle, i2s_rx_enabled);
    
    // I2Sが有効でない場合は有効化
    if (!i2s_rx_enabled) {
        ESP_LOGI(TAG, "録音関数: I2S受信チャンネルを有効化します");
        esp_err_t err = i2s_channel_enable(i2s_rx_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "録音関数: I2Sチャンネル有効化エラー: %s", esp_err_to_name(err));
            xSemaphoreGive(i2s_mutex);
            return err;
        }
        i2s_rx_enabled = true;
        ESP_LOGI(TAG, "録音関数: I2S受信チャンネルを有効化しました");
        
        // I2Sチャネルが完全に初期化されるのを待つ
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP_LOGI(TAG, "録音関数: I2S初期化待機完了");
    }
    
    
    // 録音ファイルを開く前にディレクトリが存在することを確認
    char dir_path[64] = {0};
    const char *last_slash = strrchr(filename, '/');
    if (last_slash) {
        strncpy(dir_path, filename, last_slash - filename);
        // ディレクトリが存在するか確認し、なければ作成
        struct stat st;
        if (stat(dir_path, &st) != 0) {
            ESP_LOGI(TAG, "ディレクトリを作成します: %s", dir_path);
            if (mkdir(dir_path, 0755) != 0) {
                ESP_LOGE(TAG, "ディレクトリの作成に失敗しました: %s", dir_path);
                xSemaphoreGive(i2s_mutex);
                return ESP_FAIL;
            }
        }
    }
    
    // 録音ファイルを開く
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        ESP_LOGE(TAG, "録音ファイルを開けませんでした: %s", filename);
        // ミューテックスを解放
        xSemaphoreGive(i2s_mutex);
        return ESP_FAIL;
    }
    
    // WAVヘッダー用にスペースを確保
    wav_header_t header = {0};
    fseek(f, sizeof(wav_header_t), SEEK_SET);
    
    // 録音バッファ - サイズを大きくして最適化
    const size_t chunk_size = 4096; // チャンクサイズを大きくする (4096 サンプル)
    int16_t *samples = malloc(chunk_size * sizeof(int16_t));
    if (samples == NULL) {
        ESP_LOGE(TAG, "メモリ割り当てに失敗しました");
        fclose(f);
        // ミューテックスを解放
        xSemaphoreGive(i2s_mutex);
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "録音バッファを割り当てました: %d バイト", chunk_size * sizeof(int16_t));
    
    size_t bytes_written = 0;
    uint32_t record_start_time = esp_timer_get_time() / 1000; // 現在時刻（ミリ秒）
    
    // 無音検出のためのパラメータ
    const int silence_threshold = 1000;  // 無音と判断する閾値
    const int silence_duration_ms = 1500; // 無音が続く時間（ミリ秒）
    uint32_t last_sound_time = esp_timer_get_time() / 1000; // 最後に音が検出された時間
    bool sound_detected = false; // 一度でも音が検出されたかのフラグ

    // ログ出力制御用のカウンター
    uint32_t log_counter = 0;
    const uint32_t log_interval = 50; // 50回に1回のみログを出力 (調整可能)
    
    // 録音ループ
    ESP_LOGI(TAG, "録音ループ開始");
    int loop_count = 0;
    while ((esp_timer_get_time() / 1000) - record_start_time < duration_ms) {
        loop_count++;
        if (loop_count % 50 == 0) {
            ESP_LOGI(TAG, "録音中... %d ms経過", (int)((esp_timer_get_time() / 1000) - record_start_time));
        }
        
        // I2Sからデータを読み込む - 小さなチャンクで処理
        size_t bytes_read = 0;
        // タイムアウト値を増やす (200ms → 500ms)
        esp_err_t ret = i2s_channel_read(i2s_rx_handle, samples, chunk_size * sizeof(int16_t), &bytes_read, pdMS_TO_TICKS(500));
        
        if (ret == ESP_OK && bytes_read > 0) {
            // PDMサンプルの処理 - DCオフセット除去と異常値処理
            process_pdm_samples(samples, bytes_read / sizeof(int16_t));
            
            // マイクの音量レベルと統計情報を計算
            int16_t max_sample = INT16_MIN;
            int16_t min_sample = INT16_MAX;
            int32_t sum = 0;
            int count = bytes_read / sizeof(int16_t);
            int saturated_samples = 0;  // 飽和したサンプルの数
            
            // サンプル統計計算
            for (int i = 0; i < count; i++) {
                if (samples[i] > max_sample) {
                    max_sample = samples[i];
                }
                if (samples[i] < min_sample) {
                    min_sample = samples[i];
                }
                sum += samples[i];
                
                // 飽和判定（最大値または最小値に近い）
                if (abs(samples[i]) > 32700) {  // INT16_MAX=32767に近い
                    saturated_samples++;
                }
            }
            
            // 平均値
            int32_t avg = (count > 0) ? (sum / count) : 0;
            
            // 飽和率（パーセンテージ）
            int saturation_percent = (count > 0) ? ((saturated_samples * 100) / count) : 0;
            
            // 音量レベルを0-100%で表示 - main_client.cと同じ計算方法を使用
            int volume_percent = (abs(max_sample) * 100) / 10000;
            if (volume_percent > 100) volume_percent = 100; // 100%を超えないように
            
            bool should_log = false;  // ここで変数を宣言

            if (log_counter >= log_interval) {
                should_log = true;
                log_counter = 0; // ログ出力後にカウンターをリセット
            } else {
                log_counter++;
            }

            if (should_log) {
                ESP_LOGI(TAG, "音量: %d%%, 最大値: %d, 最小値: %d, 平均値: %d", 
                         volume_percent, max_sample, min_sample, avg);
                
                // 飽和検出
                if (saturation_percent > 5) {  // 5%以上のサンプルが飽和している場合
                    ESP_LOGW(TAG, "警告: 音声サンプルの飽和検出 - 飽和率: %d%%", saturation_percent);
                }
            }
            
            // 音声検出処理
            if (abs(max_sample) > silence_threshold) {
                // 音声を検出
                last_sound_time = esp_timer_get_time() / 1000;
                sound_detected = true; // 音声が検出されたことを記録
            } else if (sound_detected) {
                // 無音状態が一定時間続いたら録音を終了
                uint32_t current_time = esp_timer_get_time() / 1000;
                if (current_time - last_sound_time > silence_duration_ms) {
                    ESP_LOGI(TAG, "無音を検出 - 録音を終了します");
                    break; // 録音ループを終了
                }
            }
            
            // ファイルに書き込む
            if (f != NULL) {
                size_t written = fwrite(samples, 1, bytes_read, f);
                if (written != bytes_read) {
                    ESP_LOGE(TAG, "ファイルへの書き込みエラー: %d/%d バイト", (int)written, (int)bytes_read);
                } else {
                    bytes_written += written;
                }
            }
        } else if (ret != ESP_OK) {
            ESP_LOGE(TAG, "I2S読み取りエラー: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(10)); // エラー時は少し待機
        }
    }
    
    // WAVヘッダーを準備して書き込む
    ESP_LOGI(TAG, "DEBUG: record_to_file - WAVヘッダー書き込み開始");
    if (f != NULL && bytes_written > 0) {
        ESP_LOGI(TAG, "DEBUG: record_to_file - WAVヘッダー初期化 (データサイズ: %d バイト)", bytes_written);
        init_wav_header(&header, bytes_written);
        ESP_LOGI(TAG, "DEBUG: record_to_file - ファイル先頭に移動");
        fseek(f, 0, SEEK_SET);
        ESP_LOGI(TAG, "DEBUG: record_to_file - WAVヘッダー書き込み");
        size_t header_written = fwrite(&header, 1, sizeof(wav_header_t), f);
        if (header_written != sizeof(wav_header_t)) {
            ESP_LOGE(TAG, "DEBUG: record_to_file - WAVヘッダーの書き込みに失敗しました: %d/%d バイト", 
                    (int)header_written, (int)sizeof(wav_header_t));
        } else {
            ESP_LOGI(TAG, "DEBUG: record_to_file - WAVヘッダー書き込み成功: %d バイト", (int)header_written);
        }
        ESP_LOGI(TAG, "DEBUG: record_to_file - ファイルを閉じます");
        fclose(f);
        ESP_LOGI(TAG, "DEBUG: record_to_file - ファイルを閉じました");
    } else if (f != NULL) {
        ESP_LOGI(TAG, "DEBUG: record_to_file - データなし、ファイルを閉じます");
        fclose(f);
        // 録音データがない場合はファイルを削除
        if (bytes_written == 0) {
            ESP_LOGW(TAG, "DEBUG: record_to_file - 録音データがありません。ファイルを削除します: %s", filename);
            unlink(filename);
            ESP_LOGI(TAG, "DEBUG: record_to_file - ファイルを削除しました");
        }
    }
    
    free(samples);
    
    // 録音終了時にミューテックスを解放
    xSemaphoreGive(i2s_mutex);

    // // 録音終了時
    // ESP_ERROR_CHECK(i2s_channel_disable(i2s_rx_handle));
    
    if (bytes_written > 0) {
        ESP_LOGI(TAG, "録音完了: %u バイトを保存しました", bytes_written);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "録音に失敗しました: データが取得できませんでした");
        return ESP_FAIL;
    }
}

/* SDカード初期化関数 */
esp_err_t sd_card_init(void) {
    ESP_LOGI(TAG, "SDカードを初期化しています...");
    
    // SDカードマウントポイントの設定
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,  // マウント失敗時にフォーマットする
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    
    // SDカードのSPI設定
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SD_PIN_NUM_MOSI,
        .miso_io_num = SD_PIN_NUM_MISO,
        .sclk_io_num = SD_PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    
    // SPIバスの初期化
    esp_err_t ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIバスの初期化に失敗しました: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // SDカードのSPIデバイス設定
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SD_PIN_NUM_CS;
    slot_config.host_id = host.slot;
    
    // SDカードのマウント
    ret = esp_vfs_fat_sdspi_mount(SD_MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "SDカードのマウントに失敗しました。カードがフォーマットされていないか、パーティションテーブルが無効です。");
        } else {
            ESP_LOGE(TAG, "SDカードの初期化に失敗しました: %s", esp_err_to_name(ret));
        }
        return ret;
    }
    
    // SDカード情報の表示
    sdmmc_card_print_info(stdout, card);
    
    // SDカードのディレクトリ作成（音声ファイル用）
    struct stat st;
    if (stat(SD_MOUNT_POINT "/audio", &st) != 0) {
        if (mkdir(SD_MOUNT_POINT "/audio", 0755) != 0) {
            ESP_LOGE(TAG, "audioディレクトリの作成に失敗しました");
        } else {
            ESP_LOGI(TAG, "audioディレクトリを作成しました");
        }
    }
    
    ESP_LOGI(TAG, "SDカード初期化完了");
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
    
    // バッファの割り当て - サイズを小さく最適化
    const size_t buf_size = 512;
    uint8_t *buffer = malloc(buf_size);
    if (!buffer) {
        ESP_LOGE(TAG, "メモリ割り当てに失敗しました");
        fclose(f);
        i2s_channel_disable(i2s_tx_handle);
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "再生バッファを割り当てました: %d バイト", buf_size);
    
    // サンプルレート変換が必要かチェック
    bool need_resampling = (header.sample_rate != SPK_SAMPLE_RATE);
    if (need_resampling) {
        ESP_LOGI(TAG, "サンプルレート変換が必要です: %d Hz -> %d Hz", 
                 header.sample_rate, SPK_SAMPLE_RATE);
    }
    
    // ファイルを読み込み、I2Sに送信
    size_t bytes_read;
    size_t bytes_written;
    
    // 再生ループ
    while ((bytes_read = fread(buffer, 1, buf_size, f)) > 0) {
        // バッファ内の最大値を計算（デバッグ用）
        int16_t max_sample = 0;
        int count = bytes_read / sizeof(int16_t);
        int16_t *samples = (int16_t*)buffer;
        
        for (int i = 0; i < count; i++) {
            if (abs(samples[i]) > abs(max_sample)) {
                max_sample = samples[i];
            }
        }
        
        // 音量レベルを0-100%で表示 - main_client.cと同じ計算方法を使用
        int volume_percent = (abs(max_sample) * 100) / 10000;
        if (volume_percent > 100) volume_percent = 100; // 100%を超えないように
        
        // 音量情報を表示（頻度を減らす）
        static int log_counter = 0;
        if (log_counter++ % 10 == 0) {
            ESP_LOGI(TAG, "再生音量: %d%%, 最大値: %d", volume_percent, max_sample);
        }
        
        // I2Sに書き込む
        esp_err_t ret = i2s_channel_write(i2s_tx_handle, buffer, bytes_read, &bytes_written, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "I2S書き込みエラー: %s", esp_err_to_name(ret));
        } else if (bytes_written != bytes_read) {
            ESP_LOGW(TAG, "I2S書き込み不完全: %d/%d バイト", (int)bytes_written, (int)bytes_read);
        }
    }
    
    // クリーンアップ - バッファを確実に解放
    fclose(f);
    if (buffer) {
        free(buffer);
        buffer = NULL;
    }
    
    // メモリ使用状況をログに出力
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);
    ESP_LOGI(TAG, "再生後のメモリ状況 - 空き: %d バイト, 最小空き: %d バイト", 
             info.total_free_bytes, info.minimum_free_bytes);
    
    // 少し待機して、すべてのデータがI2Sバッファから送信されるのを待つ
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // I2S送信チャンネルの無効化
    i2s_channel_disable(i2s_tx_handle);
    
    ESP_LOGI(TAG, "再生完了");
    return ESP_OK;
}
