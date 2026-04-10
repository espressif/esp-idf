/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __AVRCP_ABS_VOL_SERVICE_H__
#define __AVRCP_ABS_VOL_SERVICE_H__

#include <stdint.h>
#include "esp_avrc_api.h"

typedef enum {
    VOLUME_SET_BY_CONTROLLER = 0,   /*!< volume is set by controller */
    VOLUME_SET_BY_LOCAL_HOST,       /*!< volume is set by local host */
} avrc_volume_set_t;

/**
 * @brief  open AVRCP absolute volume service
 */
void avrc_abs_vol_srv_open(void);

/**
 * @brief  close AVRCP absolute volume service
 */
void avrc_abs_vol_srv_close(void);

/**
 * @brief  get local volume value
 *
 * @return local volume value
 */
uint8_t avrc_abs_vol_srv_get_volume(void);

/**
 * @brief  set volume value
 *
 * @param [in] vol_set_t  volume setting type
 * @param [in] volume     volume value to be set
 */
void avrc_abs_vol_srv_set_volume(avrc_volume_set_t vol_set_t, uint8_t volume);

/**
 * @brief  register notification of volume change
 */
void avrc_abs_vol_srv_rn_volume_change(void);

#endif /* __AVRCP_ABS_VOL_SERVICE_H__ */
