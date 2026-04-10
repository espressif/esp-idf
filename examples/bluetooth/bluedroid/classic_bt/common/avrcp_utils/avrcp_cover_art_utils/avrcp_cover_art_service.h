/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __AVRCP_COVER_ART_SERVICE_H__
#define __AVRCP_COVER_ART_SERVICE_H__

#include <stdint.h>
#include "esp_avrc_api.h"
#include "esp_err.h"

/**
 * @brief  open AVRCP cover art service
 */
void avrc_cover_art_srv_open(void);

/**
 * @brief  close AVRCP cover art service
 */
void avrc_cover_art_srv_close(void);

/**
 * @brief  start the cover art connection
 *
 * @param [in] mtu: maximum transmission unit
 */
void avrc_cover_art_srv_connect(uint16_t mtu);

/**
 * @brief  set AVRCP cover art image final, and then if image final is true, display the image
 *
 * @param [in] final: true if image reception has been completed, false otherwise
 */
void avrc_cover_art_srv_set_image_final(bool final);

/**
 * @brief  set AVRCP cover art connected
 *
 * @param [in] connected: true if connected, false otherwise
 */
void avrc_cover_art_srv_set_connected(bool connected);

/**
 * @brief  request AVRCP cover art
 */
void avrc_cover_art_srv_ca_req(void);

/**
 * @brief  save cover art data
 *
 * @param [in] p_data: pointer to the cover art data
 * @param [in] data_len: length of the cover art data
 */
void avrc_cover_art_srv_save_image_data(uint8_t *p_data, uint16_t data_len);

/**
 * @brief  handle the cover art update when metadata response
 *
 * @param [in] image_handle: pointer to the image handle
 * @param [in] len: length of the image handle
 */
void avrc_cover_art_srv_ct_metadata_update(uint8_t *image_handle, int len);

#endif /* __AVRCP_COVER_ART_SERVICE_H__ */
