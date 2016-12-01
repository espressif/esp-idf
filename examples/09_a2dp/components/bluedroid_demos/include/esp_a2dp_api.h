#ifndef __ESP_A2DP_API_H__
#define __ESP_A2DP_API_H__

#include "esp_err.h"
#include "esp_bt_defs.h"

/* Codec types */
#define ESP_A2DP_CODEC_SBC         (0)
#define ESP_A2DP_CODEC_M12         (0x01)
#define ESP_A2DP_CODEC_M24         (0x02)
#define ESP_A2DP_CODEC_ATRAC       (0x04)
#define ESP_A2DP_CODEC_NON_A2DP    (0xff)
typedef uint8_t esp_a2dp_codec_type_t;

/* codec specific information element */
typedef struct {
    uint8_t oct[4];
} esp_sbc_cie_t;

typedef struct {
    uint8_t oct[4];
} esp_mpeg12_audio_cie_t;

typedef struct {
    uint8_t oct[6];
} esp_mpeg24_aac_cie_t;

typedef struct {
    uint8_t oct[7];
} esp_atrac_cie_t;

typedef struct {
    esp_a2dp_codec_type_t type;         /* A2DP codec type*/
    union {
        esp_sbc_cie_t sbc;
        esp_mpeg12_audio_cie_t mpeg12;
        esp_mpeg24_aac_cie_t mpeg24;
        esp_atrac_cie_t atrac;
    } cie;
} esp_a2dp_codec_cfg_t;

/* Bluetooth A2DP connection states */
typedef enum {
    ESP_A2DP_CONNECTION_STATE_DISCONNECTED = 0,
    ESP_A2DP_CONNECTION_STATE_CONNECTING,
    ESP_A2DP_CONNECTION_STATE_CONNECTED,
    ESP_A2DP_CONNECTION_STATE_DISCONNECTING
} esp_a2dp_connection_state_t;

/* Bluetooth A2DP datapath states */
typedef enum {
    ESP_A2DP_AUDIO_STATE_REMOTE_SUSPEND = 0,
    ESP_A2DP_AUDIO_STATE_STOPPED,
    ESP_A2DP_AUDIO_STATE_STARTED,
} esp_a2dp_audio_state_t;

/** Callback for connection state change.
 *  state will have one of the values from btav_connection_state_t
 */
typedef void (* esp_a2dp_connection_state_cb_t)(esp_a2dp_connection_state_t state,
    esp_bd_addr_t *remote_bda);

/** Callback for audiopath state change.
 *  state will have one of the values from btav_audio_state_t
 */
typedef void (* esp_a2dp_audio_state_cb_t)(esp_a2dp_audio_state_t state,
    esp_bd_addr_t *remote_bda);

/** Callback for audio configuration change.
 *  Used only for the A2DP sink interface.
 *  state will have one of the values from btav_audio_state_t
 *  sample_rate: sample rate in Hz
 *  channel_count: number of channels (1 for mono, 2 for stereo)
 */
/*
typedef void (* esp_a2dp_audio_config_cb_t)(esp_bd_addr_t *remote_bda,
    esp_a2dp_codec_cfg_t *cfg);
*/
typedef void (* esp_a2dp_audio_config_cb_t)(esp_bd_addr_t *remote_bda,
					    uint32_t sample_rate, uint8_t channel_count); // TODO: to be modified

/** BT-a2dp callback structure. */
typedef struct {
    esp_a2dp_connection_state_cb_t connection_state_cb;
    esp_a2dp_audio_state_cb_t audio_state_cb;
    esp_a2dp_audio_config_cb_t audio_config_cb;
} esp_a2dp_callbacks_t;


/**
 * NOTE:
 * A2DP data path is handled via below function sets, between the Audio HAL
 *    and the Bluetooth stack.
 */

typedef struct {
    int (* open)(void);
    void (* close)(void);
    int (* ioctl)(int cmd, void *param); // not used for now
    uint32_t (* write)(uint8_t *buf, uint32_t len);
} esp_a2dp_sink_audio_hal_t;

#define ESP_A2DP_AUDIO_HAL_IOC_WRITE_FLUSH            (1)   // ioctl command, which is not used for now


/** Represents the A2DP sink interface.
 */
esp_err_t esp_a2dp_sink_init(esp_a2dp_callbacks_t *callbacks);

esp_err_t esp_a2dp_sink_connect(esp_bd_addr_t *remote_bda);

esp_err_t esp_a2dp_sink_disconnect(esp_bd_addr_t *remote_bda);

void esp_a2dp_sink_deinit(void);

#endif /* __ESP_A2DP_API_H__ */
