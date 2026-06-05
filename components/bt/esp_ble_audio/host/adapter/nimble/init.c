/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "sdkconfig.h"

#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/audio/vcp.h>

#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "nimble/init.h"
#include "common/init.h"

#include "../../../lib/include/audio.h"

LOG_MODULE_REGISTER(LEA_NINIT, CONFIG_BT_ISO_LOG_LEVEL);

#if CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT
#define ASCS_ASE_SNK_CCCD_COUNT                 CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT
#else /* CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT */
#define ASCS_ASE_SNK_CCCD_COUNT                 0
#endif /* CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT */

#if CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT
#define ASCS_ASE_SRC_CCCD_COUNT                 CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT
#else /* CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT */
#define ASCS_ASE_SRC_CCCD_COUNT                 0
#endif /* CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT */

#if CONFIG_BT_PAC_SNK_NOTIFIABLE
#define PAC_SNK_CCCD_COUNT                      1
#else /* CONFIG_BT_PAC_SNK_NOTIFIABLE */
#define PAC_SNK_CCCD_COUNT                      0
#endif /* CONFIG_BT_PAC_SNK_NOTIFIABLE */

#if CONFIG_BT_PAC_SNK_LOC_NOTIFIABLE
#define PAC_SNK_LOC_CCCD_COUNT                  1
#else /* CONFIG_BT_PAC_SNK_LOC_NOTIFIABLE */
#define PAC_SNK_LOC_CCCD_COUNT                  0
#endif /* CONFIG_BT_PAC_SNK_LOC_NOTIFIABLE */

#if CONFIG_BT_PAC_SRC_NOTIFIABLE
#define PAC_SRC_CCCD_COUNT                      1
#else /* CONFIG_BT_PAC_SRC_NOTIFIABLE */
#define PAC_SRC_CCCD_COUNT                      0
#endif /* CONFIG_BT_PAC_SRC_NOTIFIABLE */

#if CONFIG_BT_PAC_SRC_LOC_NOTIFIABLE
#define PAC_SRC_LOC_CCCD_COUNT                  1
#else /* CONFIG_BT_PAC_SRC_LOC_NOTIFIABLE */
#define PAC_SRC_LOC_CCCD_COUNT                  0
#endif /* CONFIG_BT_PAC_SRC_LOC_NOTIFIABLE */

#if CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE
#define PACS_SUPPORTED_CONTEXT_CCCD_COUNT       1
#else /* CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE */
#define PACS_SUPPORTED_CONTEXT_CCCD_COUNT       0
#endif /* CONFIG_BT_PACS_SUPPORTED_CONTEXT_NOTIFIABLE */

#if CONFIG_BT_BAP_SCAN_DELEGATOR_RECV_STATE_COUNT
#define SCAN_DELEGATOR_RECV_STATE_CCCD_COUNT    CONFIG_BT_BAP_SCAN_DELEGATOR_RECV_STATE_COUNT
#else /* CONFIG_BT_BAP_SCAN_DELEGATOR_RECV_STATE_COUNT */
#define SCAN_DELEGATOR_RECV_STATE_CCCD_COUNT    0
#endif /* CONFIG_BT_BAP_SCAN_DELEGATOR_RECV_STATE_COUNT */

#if CONFIG_BT_CSIP_SET_MEMBER_SIRK_NOTIFIABLE
#define CSIP_SET_MEMBER_CCCD_COUNT              1
#else /* CONFIG_BT_CSIP_SET_MEMBER_SIRK_NOTIFIABLE */
#define CSIP_SET_MEMBER_CCCD_COUNT              0
#endif /* CONFIG_BT_CSIP_SET_MEMBER_SIRK_NOTIFIABLE */

#if CONFIG_BT_VCP_VOL_REND_VOL_FLAGS_NOTIFIABLE
#define VCP_VOL_REND_VOL_FLAGS_CCCD_COUNT       1
#else /* CONFIG_BT_VCP_VOL_REND_VOL_FLAGS_NOTIFIABLE */
#define VCP_VOL_REND_VOL_FLAGS_CCCD_COUNT       0
#endif /* CONFIG_BT_VCP_VOL_REND_VOL_FLAGS_NOTIFIABLE */

#if CONFIG_BT_VOCS_MAX_INSTANCE_COUNT
#define VOCS_CCCD_COUNT                         (CONFIG_BT_VOCS_MAX_INSTANCE_COUNT * 3)
#else /* CONFIG_BT_VOCS_MAX_INSTANCE_COUNT */
#define VOCS_CCCD_COUNT                         0
#endif /* CONFIG_BT_VOCS_MAX_INSTANCE_COUNT */

#if CONFIG_BT_AICS_MAX_INSTANCE_COUNT
#define AICS_CCCD_COUNT                         (CONFIG_BT_AICS_MAX_INSTANCE_COUNT * 3)
#else /* CONFIG_BT_AICS_MAX_INSTANCE_COUNT */
#define AICS_CCCD_COUNT                         0
#endif /* CONFIG_BT_AICS_MAX_INSTANCE_COUNT */

#if CONFIG_BT_HAS_FEATURES_NOTIFIABLE
#define HAS_FEATURES_CCCD_COUNT                 1
#else /* CONFIG_BT_HAS_FEATURES_NOTIFIABLE */
#define HAS_FEATURES_CCCD_COUNT                 0
#endif /* CONFIG_BT_HAS_FEATURES_NOTIFIABLE */

#if CONFIG_BT_HAS_PRESET_CONTROL_POINT_NOTIFIABLE
#define HAS_PRESET_CONTROL_POINT_CCCD_COUNT     1
#else /* CONFIG_BT_HAS_PRESET_CONTROL_POINT_NOTIFIABLE */
#define HAS_PRESET_CONTROL_POINT_CCCD_COUNT     0
#endif /* CONFIG_BT_HAS_PRESET_CONTROL_POINT_NOTIFIABLE */

#if CONFIG_BT_HAS_ACTIVE_PRESET_INDEX
#define HAS_ACTIVE_PRESET_INDEX_CCCD_COUNT      1
#else /* CONFIG_BT_HAS_ACTIVE_PRESET_INDEX */
#define HAS_ACTIVE_PRESET_INDEX_CCCD_COUNT      0
#endif /* CONFIG_BT_HAS_ACTIVE_PRESET_INDEX */

#if CONFIG_BT_OTS
#define OTS_CCCD_COUNT                          (CONFIG_BT_OTS_MAX_INST_CNT * 2)
#else /* CONFIG_BT_OTS */
#define OTS_CCCD_COUNT                          0
#endif /* CONFIG_BT_OTS */

/* 6 is reserved for other GATT services.
 * TODO:
 * - OTS
 * - MCS (GMCS is used for now)
 * - TBS (GTBS is used for now)
 */
#define TOTAL_CCCDS_COUNT   (6 + \
                             (IS_ENABLED(CONFIG_BT_ASCS) ? (1 + ASCS_ASE_SNK_CCCD_COUNT + \
                                                            ASCS_ASE_SRC_CCCD_COUNT) : 0) + \
                             (IS_ENABLED(CONFIG_BT_PACS) ? (PAC_SNK_CCCD_COUNT + PAC_SNK_LOC_CCCD_COUNT + \
                                                            PAC_SRC_CCCD_COUNT + PAC_SRC_LOC_CCCD_COUNT + \
                                                            1 + PACS_SUPPORTED_CONTEXT_CCCD_COUNT) : 0) + \
                             (IS_ENABLED(CONFIG_BT_BAP_SCAN_DELEGATOR) ? SCAN_DELEGATOR_RECV_STATE_CCCD_COUNT : 0) + \
                             (IS_ENABLED(CONFIG_BT_MCS) ? 11 : 0) + \
                             (IS_ENABLED(CONFIG_BT_CSIP_SET_MEMBER) ? (CSIP_SET_MEMBER_CCCD_COUNT + 2) : 0) + \
                             (IS_ENABLED(CONFIG_BT_VCP_VOL_REND) ? (1 + VCP_VOL_REND_VOL_FLAGS_CCCD_COUNT) : 0) + \
                             VOCS_CCCD_COUNT + \
                             AICS_CCCD_COUNT + \
                             (IS_ENABLED(CONFIG_BT_TBS) ? 12 : 0) + \
                             (IS_ENABLED(CONFIG_BT_HAS) ? (HAS_FEATURES_CCCD_COUNT + \
                                                           HAS_PRESET_CONTROL_POINT_CCCD_COUNT + \
                                                           HAS_ACTIVE_PRESET_INDEX_CCCD_COUNT) : 0) + \
                             (IS_ENABLED(CONFIG_BT_OTS) ? OTS_CCCD_COUNT : 0))

_Static_assert(TOTAL_CCCDS_COUNT <= CONFIG_BT_NIMBLE_MAX_CCCDS, "Too small BT_NIMBLE_MAX_CCCDS");

int bt_le_nimble_audio_init(void)
{
    int err = 0;

    err = ble_att_set_preferred_mtu(BLE_AUDIO_ATT_MTU_MIN);
    if (err) {
        LOG_ERR("[N]SetPrefMtuFail[%d]", err);
        return err;
    }

    ble_svc_gap_init();

    ble_svc_gatt_init();

#if CONFIG_BT_PACS
    err = bt_le_nimble_pacs_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_PACS */

#if (BLE_AUDIO_SVC_DEFERRED_ADD == 0)
#if CONFIG_BT_ASCS
    err = bt_le_nimble_ascs_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_ASCS */

#if CONFIG_BT_BAP_SCAN_DELEGATOR
    err = bt_le_nimble_bass_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_BAP_SCAN_DELEGATOR */

#if CONFIG_BT_TMAP
    err = bt_le_nimble_tmas_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_TMAP */

#if CONFIG_BT_TBS
    err = bt_le_nimble_gtbs_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_TBS */

#if CONFIG_BT_HAS
    err = bt_le_nimble_has_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_HAS */
#endif /* (BLE_AUDIO_SVC_DEFERRED_ADD == 0) */

    return err;
}

static int nimble_gatt_attr_handle_set(void)
{
    int err = 0;

#if CONFIG_BT_ASCS
    err = bt_le_nimble_ascs_attr_handle_set();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_ASCS */

#if CONFIG_BT_PACS
    err = bt_le_nimble_pacs_attr_handle_set();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_PACS */

#if CONFIG_BT_BAP_SCAN_DELEGATOR
    err = bt_le_nimble_bass_attr_handle_set();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_BAP_SCAN_DELEGATOR */

#if CONFIG_BT_CAP_ACCEPTOR
    err = bt_le_nimble_cas_attr_handle_set();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_CAP_ACCEPTOR */

#if CONFIG_BT_TMAP
    err = bt_le_nimble_tmas_attr_handle_set();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_TMAP */

#if CONFIG_BT_CSIP_SET_MEMBER
    err = bt_le_nimble_csis_attr_handle_set();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_VCP_VOL_REND
    err = bt_le_nimble_vcs_attr_handle_set();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_MICP_MIC_DEV
    err = bt_le_nimble_mics_attr_handle_set();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_MICP_MIC_DEV */

#if CONFIG_BT_MCS
    err = bt_le_nimble_gmcs_attr_handle_set();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_TBS
    err = bt_le_nimble_gtbs_attr_handle_set();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_TBS */

#if CONFIG_BT_HAS
    err = bt_le_nimble_has_attr_handle_set();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_HAS */

    return err;
}

#if CONFIG_BT_CSIP_SET_MEMBER
static int nimble_gatt_csis_init(struct bt_le_audio_start_info *info,
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
            LOG_ERR("[N]CsisSvcGetFail[%u]", i);
            return -ENODEV;
        }

        if (info->csis_insts[i].included_by_cas) {
            if (*inc_csis_svc == NULL) {
                *inc_csis_svc = csis_svc[count];
            } else {
                /* CAS may include at most one CSIS — caller misconfigured. */
                LOG_ERR("[N]CsisMultiIncByCas");
                return -EINVAL;
            }
        }

        count++;
    }

    err = bt_le_nimble_csis_init(csis_svc, count);
    if (err) {
        return err;
    }

    return 0;
}
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_MCS
int bt_le_nimble_media_proxy_pl_init(void)
{
    /* Note:
     * Currently the existence of some characteristics within GMCS
     * are determined by the enabling of OTS.
     */
#if CONFIG_BT_OTS
    return bt_le_nimble_gmcs_init(true);
#else /* CONFIG_BT_OTS */
    return bt_le_nimble_gmcs_init(false);
#endif /* CONFIG_BT_OTS */
}
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_VCP_VOL_REND
int bt_le_nimble_vcp_vol_rend_init(void)
{
    struct bt_vcp_included vcp_included;
    int err;

    memset(&vcp_included, 0, sizeof(vcp_included));

    err = bt_vcp_vol_rend_included_get_safe(&vcp_included);
    if (err) {
        return err;
    }

    err = bt_le_nimble_vcs_init(&vcp_included);
    if (err) {
        return err;
    }

    return 0;
}
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_MICP_MIC_DEV
int bt_le_nimble_micp_mic_dev_init(void)
{
    struct bt_micp_included micp_included;
    int err;

    memset(&micp_included, 0, sizeof(micp_included));

    err = bt_micp_mic_dev_included_get_safe(&micp_included);
    if (err) {
        return err;
    }

    err = bt_le_nimble_mics_init(&micp_included);
    if (err) {
        return err;
    }

    return 0;
}
#endif /* CONFIG_BT_MICP_MIC_DEV */

int bt_le_nimble_audio_start(void *info)
{
    struct bt_gatt_service *inc_csis_svc = NULL;
    int err = 0;

#if CONFIG_BT_CSIP_SET_MEMBER
    /* Note:
     * Should add CSIS before CAS in case one CSIS instance
     * is included by CAS.
     */
    err = nimble_gatt_csis_init(info, &inc_csis_svc);
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_CSIP_SET_MEMBER */

#if CONFIG_BT_CAP_ACCEPTOR
    err = bt_le_nimble_cas_init(inc_csis_svc);
    if (err) {
        return err;
    }
#else /* CONFIG_BT_CAP_ACCEPTOR */
    ARG_UNUSED(inc_csis_svc);
#endif /* CONFIG_BT_CAP_ACCEPTOR */

#if (BLE_AUDIO_SVC_DEFERRED_ADD == 0)
#if CONFIG_BT_MCS
    err = bt_le_nimble_media_proxy_pl_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_MCS */

#if CONFIG_BT_VCP_VOL_REND
    err = bt_le_nimble_vcp_vol_rend_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_VCP_VOL_REND */

#if CONFIG_BT_MICP_MIC_DEV
    err = bt_le_nimble_micp_mic_dev_init();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_MICP_MIC_DEV */
#endif /* (BLE_AUDIO_SVC_DEFERRED_ADD == 0) */

    err = ble_gatts_start();
    if (err) {
        LOG_ERR("[N]GattsStartFail[%d]", err);
        return err;
    }

    err = nimble_gatt_attr_handle_set();
    if (err) {
        return err;
    }

    return err;
}
