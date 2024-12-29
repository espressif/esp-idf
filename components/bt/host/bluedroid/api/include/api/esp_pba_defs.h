/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_bt_defs.h"

/* Supported repositories bit mask */
#define ESP_PBA_SUPPORTED_REPO_LOCAL_PHONE_BOOK             0x01
#define ESP_PBA_SUPPORTED_REPO_SIM_CARD                     0x02
#define ESP_PBA_SUPPORTED_REPO_SPEED_DIAL                   0x04
#define ESP_PBA_SUPPORTED_REPO_FAVORITES                    0x08

/* Supported features bit mask */
#define ESP_PBA_SUPPORTED_FEAT_DOWNLOAD                     0x0001
#define ESP_PBA_SUPPORTED_FEAT_BROWSING                     0x0002
#define ESP_PBA_SUPPORTED_FEAT_DATABASE_IDENTIFIER          0x0004
#define ESP_PBA_SUPPORTED_FEAT_FOLDER_VERSION_COUNTERS      0x0008
#define ESP_PBA_SUPPORTED_FEAT_VCARD_SELECTING              0x0010
#define ESP_PBA_SUPPORTED_FEAT_ENHANCED_MISSED_CALLS        0x0020
#define ESP_PBA_SUPPORTED_FEAT_X_BT_UCI_VCARD_PROPERTY      0x0040
#define ESP_PBA_SUPPORTED_FEAT_X_BT_UID_VCARD_PROPERTY      0x0080
#define ESP_PBA_SUPPORTED_FEAT_CONTACT_REFERENCING          0x0100
#define ESP_PBA_SUPPORTED_FEAT_DEFAULT_CONTACT_IMAGE_FORMAT 0x0200
