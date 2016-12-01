#ifndef __ESP_A2DP_API_H__
#define __ESP_A2DP_API_H__

#include "esp_err.h"
#include "esp_bt_defs.h"

/* Media codec types */
#define ESP_A2D_MCT_SBC         (0)
#define ESP_A2D_MCT_M12         (0x01)
#define ESP_A2D_MCT_M24         (0x02)
#define ESP_A2D_MCT_ATRAC       (0x04)
#define ESP_A2D_MCT_NON_A2DP    (0xff)
typedef uint8_t esp_a2d_mct_t;

/* codec specific information element */
#define ESP_A2D_CIE_LEN_SBC          (4)
#define ESP_A2D_CIE_LEN_M12          (4)
#define ESP_A2D_CIE_LEN_M24          (6)
#define ESP_A2D_CIE_LEN_ATRAC        (7)

typedef struct {
    uint8_t oct[ESP_A2D_CIE_LEN_SBC];
} esp_a2d_cie_sbc_t;

typedef struct {
    uint8_t oct[ESP_A2D_CIE_LEN_M12];
} esp_a2d_cie_m12_t;

typedef struct {
    uint8_t oct[ESP_A2D_CIE_LEN_M24];
} esp_a2d_cie_m24_t;

typedef struct {
    uint8_t oct[ESP_A2D_CIE_LEN_ATRAC];
} esp_a2d_cie_atrac_t;

/* media codec capabilities */
typedef struct {
    esp_a2d_mct_t type;         /* A2DP media codec type*/
    union {
        esp_a2d_cie_sbc_t sbc;
        esp_a2d_cie_m12_t m12;
        esp_a2d_cie_m24_t m24;
        esp_a2d_cie_atrac_t atrac;
    } cie;
} esp_a2d_mcc_t;

/* Bluetooth A2DP connection states */
typedef enum {
    ESP_A2D_CONNECTION_STATE_DISCONNECTED = 0,
    ESP_A2D_CONNECTION_STATE_CONNECTING,
    ESP_A2D_CONNECTION_STATE_CONNECTED,
    ESP_A2D_CONNECTION_STATE_DISCONNECTING
} esp_a2d_connection_state_t;

/* Bluetooth A2DP datapath states */
typedef enum {
    ESP_A2D_AUDIO_STATE_REMOTE_SUSPEND = 0,
    ESP_A2D_AUDIO_STATE_STOPPED,
    ESP_A2D_AUDIO_STATE_STARTED,
} esp_a2d_audio_state_t;

/** Callback for connection state change.
 *  state will have one of the values from btav_connection_state_t
 */
typedef void (* esp_a2d_connection_state_cb_t)(esp_a2d_connection_state_t state,
    esp_bd_addr_t *remote_bda);

/** Callback for audiopath state change.
 *  state will have one of the values from btav_audio_state_t
 */
typedef void (* esp_a2d_audio_state_cb_t)(esp_a2d_audio_state_t state,
    esp_bd_addr_t *remote_bda);

/** Callback for audio configuration change.
 *  Used only for the A2DP sink interface.
 *  state will have one of the values from btav_audio_state_t
 */
typedef void (* esp_a2d_audio_config_cb_t)(esp_bd_addr_t *remote_bda,
    esp_a2d_mcc_t *mcc);

/** BT-a2dp callback structure. */
typedef struct {
    esp_a2d_connection_state_cb_t connection_state_cb;
    esp_a2d_audio_state_cb_t audio_state_cb;
    esp_a2d_audio_config_cb_t audio_config_cb;
} esp_a2d_callbacks_t;

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
} esp_a2d_sink_audio_hal_t;

#define ESP_A2D_AUDIO_HAL_IOC_WRITE_FLUSH            (1)   // ioctl command, which is not used for now


/** Represents the A2DP sink interface.
 */
esp_err_t esp_a2d_sink_init(esp_a2d_callbacks_t *callbacks);

esp_err_t esp_a2d_sink_connect(esp_bd_addr_t *remote_bda);

esp_err_t esp_a2d_sink_disconnect(esp_bd_addr_t *remote_bda);

void esp_a2d_sink_deinit(void);

#endif /* __ESP_A2DP_API_H__ */
