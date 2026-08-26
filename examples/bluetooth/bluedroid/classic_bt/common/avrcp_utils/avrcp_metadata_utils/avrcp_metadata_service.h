/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __AVRCP_METADATA_SERVICE_H__
#define __AVRCP_METADATA_SERVICE_H__

#include <stdint.h>
#include "esp_avrc_api.h"

typedef struct {
    uint8_t attr_id;               /*!< id of metadata attribute */
    uint8_t *attr_text;            /*!< attribute itself */
    int attr_length;               /*!< attribute character length */
} avrc_metadata_srv_param_t;

/**
 * @brief  open AVRCP metadata service
 */
void avrc_metadata_srv_open(void);

/**
 * @brief  close AVRCP metadata service
 */
void avrc_metadata_srv_close(void);

/**
 * @brief  request AVRCP metadata
 */
void avrc_metadata_srv_md_req(void);

/**
 * @brief  save AVRCP metadata response data from ESP_AVRC_CT_METADATA_RSP_EVT
 *
 * @param [in] param  AVRCP metadata response data
 */
void avrc_metadata_srv_md_save(avrc_metadata_srv_param_t *param);

/**
 * @brief  free AVRCP metadata response data from ESP_AVRC_CT_METADATA_RSP_EVT
 *
 * @param [in] attr_id  attribute id
 */
void avrc_metadata_srv_md_free(uint8_t attr_id);

#endif /* __AVRCP_METADATA_SERVICE_H__ */
