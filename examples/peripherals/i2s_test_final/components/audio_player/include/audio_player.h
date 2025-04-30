#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <stdio.h>
#include "driver/i2s_std.h"
#include "driver/i2s_pdm.h"  // PDMマイク用のヘッダーを追加
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

/* WAVファイルヘッダー構造体 */
typedef struct {
    // RIFF chunk
    char riff_header[4];
    uint32_t wav_size;
    char wave_header[4];
    
    // fmt chunk
    char fmt_header[4];
    uint32_t fmt_chunk_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    
    // data chunk
    char data_header[4];
    uint32_t data_bytes;
} wav_header_t;

extern i2s_chan_handle_t i2s_rx_handle; // ハンドルをアクセス可能にするために追加
extern i2s_chan_handle_t i2s_tx_handle; // ハンドルをアクセス可能にするために追加
extern bool i2s_rx_enabled; // チャンネルが有効化されているかを追跡するフラグ

/* 関数プロトタイプ */
void init_wav_header(wav_header_t *header, uint32_t data_size);
esp_err_t audio_player_init(void);
esp_err_t sd_card_init(void);
esp_err_t record_to_file(const char *filename, int duration_ms);
esp_err_t play_from_file(const char *filename);

#endif /* AUDIO_PLAYER_H */
