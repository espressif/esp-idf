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

typedef enum {
    ESP_A2D_CONNECTION_STATE_EVT = 0,           /*!< connection state changed */
    ESP_A2D_AUDIO_STATE_EVT = 1,                /*!< audio stream state changed */
    ESP_A2D_AUDIO_CFG_EVT = 2                   /*!< audio codec configuration received */
} esp_a2d_cb_event_t;

typedef union {
    /*< ESP_A2D_CONNECTION_STATE_EVT */
    struct a2d_conn_stat_param {
        esp_a2d_connection_state_t state;       /*!< one of values from esp_a2d_connection_state_t */
        esp_bd_addr_t remote_bda;
    } conn_stat;
    
    /*< ESP_A2D_AUDIO_STATE_EVT */
    struct a2d_audio_stat_param {
        esp_a2d_audio_state_t state;            /*!< one of the values from esp_a2d_audio_state_t */
        esp_bd_addr_t remote_bda;
    } audio_stat;
    
    /*< ESP_A2D_AUDIO_CFG_EVT */
    struct a2d_audio_cfg_param {
        esp_bd_addr_t remote_bda;
        esp_a2d_mcc_t mcc;
    } audio_cfg;
} esp_a2d_cb_param_t;

typedef void (* esp_a2d_data_cb_t)(uint8_t *buf, uint32_t len);

/**
 * NOTE:
 * A2DP data path is handled via below function sets, between the Audio HAL
 *    and the Bluetooth stack.
 */

/**
 * Represents the A2DP sink interface.
 */
esp_err_t esp_a2d_register_callback(esp_profile_cb_t callback);

esp_err_t esp_a2d_register_data_callback(esp_a2d_data_cb_t cb);

esp_err_t esp_a2d_sink_init(void);

esp_err_t esp_a2d_sink_connect(esp_bd_addr_t remote_bda);

esp_err_t esp_a2d_sink_disconnect(esp_bd_addr_t remote_bda);

void esp_a2d_sink_deinit(void);

#endif /* __ESP_A2DP_API_H__ */
