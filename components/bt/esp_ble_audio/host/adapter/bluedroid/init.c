/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "sdkconfig.h"

#include "bta/bta_gatt_common.h"

#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/audio/micp.h>
#include <zephyr/bluetooth/audio/vcp.h>

#include "bluedroid/init.h"
#include "common/init.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_BINIT, CONFIG_BT_ISO_LOG_LEVEL);

#if CONFIG_BT_CSIP_SET_MEMBER
#define CSIS_SVC_COUNT      CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT
#else /* CONFIG_BT_CSIP_SET_MEMBER */
#define CSIS_SVC_COUNT      0
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

/* 3 is reserved for other GATT services */
#define TOTAL_SERVICE_COUNT     (3 + \
                                 (IS_ENABLED(CONFIG_BT_ASCS) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_PACS) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_BAP_SCAN_DELEGATOR) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_TMAP) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_MCS) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_CSIP_SET_MEMBER) ? CSIS_SVC_COUNT : 0) + \
                                 (IS_ENABLED(CONFIG_BT_CAP_ACCEPTOR) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_VCP_VOL_REND) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_MICP_MIC_DEV) ? 1 : 0) + \
                                 CONFIG_BT_VOCS_MAX_INSTANCE_COUNT + \
                                 CONFIG_BT_AICS_MAX_INSTANCE_COUNT + \
                                 (IS_ENABLED(CONFIG_BT_TBS) ? 1 : 0) + \
                                 (IS_ENABLED(CONFIG_BT_HAS) ? 1 : 0))

_Static_assert(TOTAL_SERVICE_COUNT <= CONFIG_BT_GATT_MAX_SR_PROFILES, "Too small BT_GATT_MAX_SR_PROFILES");

int bt_le_bluedroid_audio_init(void)
{
    int err = 0;

    BTA_GATT_SetLocalMTU(BLE_AUDIO_ATT_MTU_MIN);

    bt_le_bluedroid_audio_gatts_init();

#if CONFIG_BT_PACS
    err = bt_le_bluedroid_pacs_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_PACS */

#if (BLE_AUDIO_SVC_DEFERRED_ADD == 0)
#if CONFIG_BT_ASCS
    err = bt_le_bluedroid_ascs_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_ASCS */

#if CONFIG_BT_BAP_SCAN_DELEGATOR
    err = bt_le_bluedroid_bass_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_BAP_SCAN_DELEGATOR */

#if CONFIG_BT_TMAP
    err = bt_le_bluedroid_tmas_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_TMAP */

#if CONFIG_BT_TBS
    err = bt_le_bluedroid_gtbs_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_TBS */

#if CONFIG_BT_HAS
    err = bt_le_bluedroid_has_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_HAS */
#endif /* (BLE_AUDIO_SVC_DEFERRED_ADD == 0) */

    return err;
}

#if CONFIG_BT_CSIP_SET_MEMBER
static int bluedroid_gatt_csis_init(struct bt_le_audio_start_info *info,
                                    struct bt_gatt_service **inc_csis_svc)
{
    struct bt_gatt_service *csis_svc[CONFIG_BT_CSIP_SET_MEMBER_MAX_INSTANCE_COUNT];
    uint8_t count;
    int err;

    if (info == NULL) {
        return 0;
    }

    *inc_csis_svc = NULL;
    count = 0;

    for (size_t i = 0; i < ARRAY_SIZE(info->csis_insts); i++) {
        if (info->csis_insts[i].svc_inst == NULL) {
            continue;
        }

        csis_svc[count] = lib_csip_set_member_svc_get(info->csis_insts[i].svc_inst);
        if (!csis_svc[count]) {
            LOG_ERR("[B]CsisSvcGetFail[%u]", i);
            return -ENODEV;
        }

        if (info->csis_insts[i].included_by_cas) {
            if (*inc_csis_svc == NULL) {
                *inc_csis_svc = csis_svc[count];
            } else {
                /* CAS may include at most one CSIS — caller misconfigured. */
                LOG_ERR("[B]CsisMultiIncByCas");
                return -EINVAL;
            }
        }

        count++;
    }

    err = bt_le_bluedroid_csis_init(csis_svc, count);
    if (err) {
        return err;
    }

    return 0;
}
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_MCS
int bt_le_bluedroid_media_proxy_pl_init(void)
{
    int err;

#if CONFIG_BT_MPL_OBJECTS

#endif /* CONFIG_BT_MPL_OBJECTS */

    err = bt_le_bluedroid_gmcs_init();
    if (err) {
        return err;
    }

    return 0;
}
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_VCP_VOL_REND
int bt_le_bluedroid_vcp_vol_rend_init(void)
{
    struct bt_vcp_included vcp_included;
    int err;

    memset(&vcp_included, 0, sizeof(vcp_included));

    err = bt_vcp_vol_rend_included_get_safe(&vcp_included);
    if (err) {
        return err;
    }

    err = bt_le_bluedroid_vcs_init(&vcp_included);
    if (err) {
        return err;
    }

    return 0;
}
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_MICP_MIC_DEV
int bt_le_bluedroid_micp_mic_dev_init(void)
{
    struct bt_micp_included micp_included;
    int err;

    memset(&micp_included, 0, sizeof(micp_included));

    err = bt_micp_mic_dev_included_get_safe(&micp_included);
    if (err) {
        return err;
    }

    err = bt_le_bluedroid_mics_init(&micp_included);
    if (err) {
        return err;
    }

    return 0;
}
#endif /* CONFIG_BT_MICP_MIC_DEV */

int bt_le_bluedroid_audio_start(void *info)
{
    struct bt_gatt_service *inc_csis_svc = NULL;
    int err = 0;

#if CONFIG_BT_CSIP_SET_MEMBER
    /* Note:
     * Should add CSIS before CAS in case one CSIS instance
     * is included by CAS.
     */
    err = bluedroid_gatt_csis_init(info, &inc_csis_svc);
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_CAP_ACCEPTOR
    err = bt_le_bluedroid_cas_init(inc_csis_svc);
    if (err) {
        return err;
    }
#else /* CONFIG_BT_CAP_ACCEPTOR */
    ARG_UNUSED(inc_csis_svc);
#endif /* CONFIG_BT_CAP_ACCEPTOR */

#if (BLE_AUDIO_SVC_DEFERRED_ADD == 0)
#if CONFIG_BT_MCS
    err = bt_le_bluedroid_media_proxy_pl_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_VCP_VOL_REND
    err = bt_le_bluedroid_vcp_vol_rend_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_MICP_MIC_DEV
    err = bt_le_bluedroid_micp_mic_dev_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_MICP_MIC_DEV */
#endif /* (BLE_AUDIO_SVC_DEFERRED_ADD == 0) */

#if CONFIG_BT_ASCS
    err = bt_le_bluedroid_ascs_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_ASCS */

#if CONFIG_BT_PACS
    err = bt_le_bluedroid_pacs_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_PACS */

#if CONFIG_BT_BAP_SCAN_DELEGATOR
    err = bt_le_bluedroid_bass_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_BAP_SCAN_DELEGATOR */

#if CONFIG_BT_TMAP
    err = bt_le_bluedroid_tmas_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_TMAP */

#if CONFIG_BT_CSIP_SET_MEMBER
    err = bt_le_bluedroid_csis_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_CAP_ACCEPTOR
    err = bt_le_bluedroid_cas_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_CAP_ACCEPTOR */

#if CONFIG_BT_VCP_VOL_REND
    err = bt_le_bluedroid_vcs_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_MICP_MIC_DEV
    err = bt_le_bluedroid_mics_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_MICP_MIC_DEV */

#if CONFIG_BT_MCS
    err = bt_le_bluedroid_gmcs_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_TBS
    err = bt_le_bluedroid_gtbs_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_TBS */

#if CONFIG_BT_HAS
    err = bt_le_bluedroid_has_start();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_HAS */

    return err;
}
