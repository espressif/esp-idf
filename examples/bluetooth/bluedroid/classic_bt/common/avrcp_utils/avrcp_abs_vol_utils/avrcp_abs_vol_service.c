/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <string.h>
#include "sys/lock.h"
#include "esp_log.h"
#include "esp_avrc_api.h"
#include "avrcp_abs_vol_service.h"

/* tags */
#define RC_VC_SRV_TAG  "RC_VC_SRV"

typedef struct {
    _lock_t volume_lock;          /* lock local volume value */
    uint8_t volume;               /* local volume value */
    bool volume_notify;           /* notify volume change or not */
} avrc_abs_vol_srv_cb_t;

/*******************************
 * STATIC VARIABLE DEFINITIONS
 ******************************/

/* AVRCP absolute volume service control block */
static avrc_abs_vol_srv_cb_t s_avrc_abs_vol_srv_cb;

/********************************
 * EXTERNAL FUNCTION DEFINITIONS
 *******************************/

void avrc_abs_vol_srv_open(void)
{
    memset(&s_avrc_abs_vol_srv_cb, 0, sizeof(avrc_abs_vol_srv_cb_t));
}

void avrc_abs_vol_srv_close(void)
{
    memset(&s_avrc_abs_vol_srv_cb, 0, sizeof(avrc_abs_vol_srv_cb_t));
}

uint8_t avrc_abs_vol_srv_get_volume(void)
{
    return s_avrc_abs_vol_srv_cb.volume;
}

void avrc_abs_vol_srv_set_volume(avrc_volume_set_t vol_set_t, uint8_t volume)
{
    switch (vol_set_t) {
    case VOLUME_SET_BY_CONTROLLER: {
        ESP_LOGI(RC_VC_SRV_TAG, "Volume is set by remote controller to: %"PRIu32"%%", (uint32_t)volume * 100 / 0x7f);
        /* set the volume in protection of lock */
        _lock_acquire(&s_avrc_abs_vol_srv_cb.volume_lock);
        s_avrc_abs_vol_srv_cb.volume = volume;
        _lock_release(&s_avrc_abs_vol_srv_cb.volume_lock);
        break;
    }
    case VOLUME_SET_BY_LOCAL_HOST: {
        ESP_LOGI(RC_VC_SRV_TAG, "Volume is set locally to: %"PRIu32"%%", (uint32_t)volume * 100 / 0x7f);
        /* set the volume in protection of lock */
        _lock_acquire(&s_avrc_abs_vol_srv_cb.volume_lock);
        s_avrc_abs_vol_srv_cb.volume = volume;
        _lock_release(&s_avrc_abs_vol_srv_cb.volume_lock);

        /* send notification response to remote AVRCP controller */
        if (s_avrc_abs_vol_srv_cb.volume_notify) {
            esp_avrc_rn_param_t rn_param;
            rn_param.volume = s_avrc_abs_vol_srv_cb.volume;
            esp_avrc_tg_send_rn_rsp(ESP_AVRC_RN_VOLUME_CHANGE, ESP_AVRC_RN_RSP_CHANGED, &rn_param);
            s_avrc_abs_vol_srv_cb.volume_notify = false;
        }
        break;
    }
    default:
        ESP_LOGE(RC_VC_SRV_TAG, "Invalid volume setting type");
        break;
    }
}

void avrc_abs_vol_srv_rn_volume_change(void)
{
    s_avrc_abs_vol_srv_cb.volume_notify = true;
    esp_avrc_rn_param_t rn_param;
    rn_param.volume = s_avrc_abs_vol_srv_cb.volume;
    esp_avrc_tg_send_rn_rsp(ESP_AVRC_RN_VOLUME_CHANGE, ESP_AVRC_RN_RSP_INTERIM, &rn_param);
}
