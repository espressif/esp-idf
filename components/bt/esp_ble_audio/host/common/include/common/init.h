/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_INIT_H_
#define HOST_COMMON_INIT_H_

#include <stdint.h>

#include "sdkconfig.h"

#include "../../../lib/include/audio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_AUDIO_SVC_SEP_ADD   0   /* Indicate if adding LE Audio services separately */

/* Minimum value required by BAP.
 * 64 octets is not enough for running 4 ASEs.
 * Hence use 128 as the default value here.
 */
#define BLE_AUDIO_ATT_MTU_MIN   128

struct bt_csip_set_member_svc_inst;

struct bt_le_audio_start_info {
#if CONFIG_BT_CSIP_SET_MEMBER
    struct {
        struct bt_csip_set_member_svc_inst *svc_inst;
        bool included_by_cas;
    } csis_insts[CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT];
#endif /* CONFIG_BT_CSIP_SET_MEMBER */
    uint8_t dummy;  /*!< Dummy field to avoid empty struct */
};

int bt_le_audio_init(void);

int bt_le_ascs_init(void);

int bt_le_bass_init(void);

int bt_le_tmas_init(void);

int bt_le_gtbs_init(void);

int bt_le_has_init(void);

int bt_le_media_proxy_pl_init(void);

int bt_le_vcp_vol_rend_init(void);

int bt_le_micp_mic_dev_init(void);

int bt_le_audio_start(void *info);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_INIT_H_ */
