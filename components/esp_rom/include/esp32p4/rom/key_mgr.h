/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc_caps.h"

#if SOC_KEY_MANAGER_SUPPORTED
#include <stdint.h>
#include "esp_attr.h"
#include "ets_sys.h"
#include "km.h"

#if __cplusplus
extern "C" {
#endif

// store huk info, occupy 96 words
struct huk_info {
#define HUK_INFO_LEN 384
    uint8_t info[HUK_INFO_LEN];
    uint32_t crc;
} PACKED_ATTR;

// store key info, occupy 512 bits
struct key_info {
#define KEY_INFO_LEN 64
    uint8_t info[KEY_INFO_LEN];
    uint32_t crc;
} PACKED_ATTR;

struct huk_key_block {
#define KEY_HUK_SECTOR_MAGIC 0xDEA5CE5A
    uint32_t magic;
    uint32_t version; // for backward compatibility
    uint8_t key_type;
    uint8_t reserved[15];
    struct huk_info huk_info;
    struct key_info key_info[2]; // at most 2 key info (XTS-512_1 and XTS-512_2), at least use 1
} WORD_ALIGNED_ATTR PACKED_ATTR;

/*
 * We define two info sectors "active" and "backup" here
 * Most rom code would rely only on the "active" sector for the key information
 *
 * But there could be a situation where the huk and key information must be regenerated
 * based on ageing and other factors. For that scenario, we need a "backup" sector
 */
#define KEY_HUK_SECTOR_OFFSET(i) ((i)*0x1000)
#define ACTIVE_SECTOR_OFFSET     KEY_HUK_SECTOR_OFFSET(0)
#define BACKUP_SECTOR_OFFSET     KEY_HUK_SECTOR_OFFSET(1)

#define KM_PERI_ECDSA (BIT(0))
#define KM_PERI_XTS   (BIT(1))

struct km_deploy_ops {
#define KM_KEY_PURPOSE_ECDSA     1
#define KM_KEY_PURPOSE_XTS_256_1 2
#define KM_KEY_PURPOSE_XTS_256_2 3
#define KM_KEY_PURPOSE_XTS_128   4
    int km_key_purpose;
#define KM_DEPLOY_MODE_RANDOM  0
#define KM_DEPLOY_MODE_AES     1
#define KM_DEPLOY_MODE_ECDH0   2
#define KM_DEPLOY_MODE_ECDH1   3
#define KM_DEPLOY_MODE_RECOVER 4
#define KM_DEPLOY_MODE_EXPORT  5
    int deploy_mode;
    uint8_t *init_key; // 256 bits, only used in aes and ecdh1 deploy mode
    int deploy_only_once;
    int force_use_km_key;
    int km_use_efuse_key;
    uint32_t efuse_km_rnd_switch_cycle; // 0 means use default
    uint32_t km_rnd_switch_cycle;       // 0 means use default
    int km_use_sw_init_key;
    struct huk_info *huk_info;
    struct key_info *key_info;
};

/* state of km */
#define KM_STATE_IDLE    0
#define KM_STATE_LOAD    1
#define KM_STATE_GAIN    2
#define KM_STATE_BUSY    3
#define KM_STATE_INVALID 4

/* state of huk generator
 * values defined same as km
 */
#define HUK_STATE_IDLE 0
#define HUK_STATE_LOAD 1
#define HUK_STATE_GAIN 2
#define HUK_STATE_BUSY 3

#define HUK_NOT_GENERATED 0
#define HUK_GEN_VALID     1
#define HUK_GEN_INVALID   2

#if __cplusplus
}
#endif
#endif
