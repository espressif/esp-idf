/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/* PBAP supported repositories */
#define BTA_PBAP_REPO_LOCAL_PHONEBOOK                   0x01
#define BTA_PBAP_REPO_SIM_CARD                          0x02
#define BTA_PBAP_REPO_SPEED_DIAL                        0x04
#define BTA_PBAP_REPO_FAVORITES                         0x08

/* PBAP supported features */
#define BTA_PBAP_FEAT_DOWNLOAD                          0x0001
#define BTA_PBAP_FEAT_BROWSING                          0x0002
#define BTA_PBAP_FEAT_DATABASE_IDENTIFIER               0x0004
#define BTA_PBAP_FEAT_FLODER_VERSION_COUNTER            0x0008
#define BTA_PBAP_FEAT_VCARD_SELECTING                   0x0010
#define BTA_PBAP_FEAT_ENHANCED_MISSED_CALLS             0x0020
#define BTA_PBAP_FEAT_X_BT_UCI_VCARD_PROPERTY           0x0040
#define BTA_PBAP_FEAT_X_BT_UID_VCARD_PROPERTY           0x0080
#define BTA_PBAP_FEAT_CONTACT_REFERENCING               0x0100
#define BTA_PBAP_FEAT_DEFAULT_CONTACT_IMAGE_FORMAT      0x0200

/* PBAP default supported features */
#define BTA_PBAP_DEFAULT_SUPPORTED_FEATURES             0x0003

/* Application parameters tag id */
#define BTA_PBAP_APP_PARAM_ORDER                        0x01
#define BTA_PBAP_APP_PARAM_SEARCH_VALUE                 0x02
#define BTA_PBAP_APP_PARAM_SEARCH_PROPERTY              0x03
#define BTA_PBAP_APP_PARAM_MAX_LIST_COUNT               0x04
#define BTA_PBAP_APP_PARAM_LIST_START_OFFSET            0x05
#define BTA_PBAP_APP_PARAM_PROPERTY_SELECTOR            0x06
#define BTA_PBAP_APP_PARAM_FORMAT                       0x07
#define BTA_PBAP_APP_PARAM_PHONE_BOOK_SIZE              0x08
#define BTA_PBAP_APP_PARAM_NEW_MISSED_CALLS             0x09
#define BTA_PBAP_APP_PARAM_PRIMARY_FOLDER_VERSION       0x0A
#define BTA_PBAP_APP_PARAM_SECONDARY_FOLDER_VERSION     0x0B
#define BTA_PBAP_APP_PARAM_VCARD_SELECTOR               0x0C
#define BTA_PBAP_APP_PARAM_DATABASE_IDENTIFIER          0x0D
#define BTA_PBAP_APP_PARAM_VCARD_SELECTOR_OPERATOR      0x0E
#define BTA_PBAP_APP_PARAM_RESET_NEW_MISSED_CALLS       0x0F
#define BTA_PBAP_APP_PARAM_PBAP_SUPPORTED_FEATURES      0x10

/* Application parameters length (except SearchValue) */
#define BTA_PBAP_APP_PARAM_LENGTH_ORDER                         1
#define BTA_PBAP_APP_PARAM_LENGTH_SEARCH_PROPERTY               1
#define BTA_PBAP_APP_PARAM_LENGTH_MAX_LIST_COUNT                2
#define BTA_PBAP_APP_PARAM_LENGTH_LIST_START_OFFSET             2
#define BTA_PBAP_APP_PARAM_LENGTH_PROPERTY_SELECTOR             8
#define BTA_PBAP_APP_PARAM_LENGTH_FORMAT                        1
#define BTA_PBAP_APP_PARAM_LENGTH_PHONE_BOOK_SIZE               2
#define BTA_PBAP_APP_PARAM_LENGTH_NEW_MISSED_CALLS              1
#define BTA_PBAP_APP_PARAM_LENGTH_PRIMARY_FOLDER_VERSION        16
#define BTA_PBAP_APP_PARAM_LENGTH_SECONDARY_FOLDER_VERSION      16
#define BTA_PBAP_APP_PARAM_LENGTH_VCARD_SELECTOR                8
#define BTA_PBAP_APP_PARAM_LENGTH_DATABASE_IDENTIFIER           16
#define BTA_PBAP_APP_PARAM_LENGTH_VCARD_SELECTOR_OPERATOR       1
#define BTA_PBAP_APP_PARAM_LENGTH_RESET_NEW_MISSED_CALLS        1
#define BTA_PBAP_APP_PARAM_LENGTH_PBAP_SUPPORTED_FEATURES       4

/* Application parameter tag (1 byte) + Application parameter length (1 byte) */
#define BTA_PBAP_APP_PARAM_HEADER_LENGTH                        2

/* The minimal buff size that can hold all pull phone book application parameters */
#define BTA_PBAP_PULL_PHONE_BOOK_APP_PARAM_BUFF_SIZE_MIN        37

/* The minimal buff size that can hold pull vCard listing application parameters, except the value of SearchValue */
#define BTA_PBAP_PULL_VCARD_LISTING_APP_PARAM_BUFF_SIZE_MIN     34

/* The minimal buff size that can hold all pull vCard entry application parameters */
#define BTA_PBAP_PULL_VCARD_ENTRY_APP_PARAM_BUFF_SIZE_MIN       13

/* Application parameters bit mask */
#define PBAP_APP_PARAM_BIT_MASK_ORDER                           0x0001
#define PBAP_APP_PARAM_BIT_MASK_SEARCH_VALUE                    0x0002
#define PBAP_APP_PARAM_BIT_MASK_SEARCH_PROPERTY                 0x0004
#define PBAP_APP_PARAM_BIT_MASK_MAX_LIST_COUNT                  0x0008
#define PBAP_APP_PARAM_BIT_MASK_LIST_START_OFFSET               0x0010
#define PBAP_APP_PARAM_BIT_MASK_PROPERTY_SELECTOR               0x0020
#define PBAP_APP_PARAM_BIT_MASK_FORMAT                          0x0040
#define PBAP_APP_PARAM_BIT_MASK_PHONE_BOOK_SIZE                 0x0080
#define PBAP_APP_PARAM_BIT_MASK_NEW_MISSED_CALLS                0x0100
#define PBAP_APP_PARAM_BIT_MASK_PRIMARY_FOLDER_VERSION          0x0200
#define PBAP_APP_PARAM_BIT_MASK_SECONDARY_FOLDER_VERSION        0x0400
#define PBAP_APP_PARAM_BIT_MASK_VCARD_SELECTOR                  0x0800
#define PBAP_APP_PARAM_BIT_MASK_DATABASE_IDENTIFIER             0x1000
#define PBAP_APP_PARAM_BIT_MASK_VCARD_SELECTOR_OPERATOR         0x2000
#define PBAP_APP_PARAM_BIT_MASK_RESET_NEW_MISSED_CALLS          0x4000
#define PBAP_APP_PARAM_BIT_MASK_PBAP_SUPPORTED_FEATURES         0x8000
