/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "btc/btc_task.h"
#include "osi/thread.h"
#include "esp_log.h"
#include "bt_common.h"
#include "osi/allocator.h"
#include "btc/btc_alarm.h"

#include "btc/btc_manage.h"
#include "btc_blufi_prf.h"
#include "blufi_int.h"
#ifdef CONFIG_BT_BLUEDROID_ENABLED
#include "common/bt_target.h"
#include "btc/btc_main.h"
#include "btc/btc_dev.h"
#include "btc_gatts.h"
#include "btc_gattc.h"
#include "btc_gatt_common.h"
#include "btc_gap_ble.h"
#include "btc/btc_dm.h"
#include "bta/bta_gatt_api.h"
#if CLASSIC_BT_INCLUDED
#include "btc/btc_profile_queue.h"
#if (BTC_GAP_BT_INCLUDED == TRUE)
#include "btc_gap_bt.h"
#endif /* BTC_GAP_BT_INCLUDED == TRUE */
#if BTC_AV_INCLUDED
#include "btc_av.h"
#include "btc_avrc.h"
#include "btc_av_co.h"
#endif /* #if BTC_AV_INCLUDED */
#if (BTC_SPP_INCLUDED == TRUE)
#include "btc_spp.h"
#endif /* #if (BTC_SPP_INCLUDED == TRUE) */
#if (BTC_L2CAP_INCLUDED == TRUE)
#include "btc_l2cap.h"
#endif /* #if (BTC_L2CAP_INCLUDED == TRUE) */
#if (BTC_SDP_INCLUDED == TRUE)
#include "btc_sdp.h"
#endif /* #if (BTC_SDP_INCLUDED == TRUE) */
#if BTC_HF_INCLUDED
#include "btc_hf_ag.h"
#endif/* #if BTC_HF_INCLUDED */
#if BTC_HF_CLIENT_INCLUDED
#include "btc_hf_client.h"
#endif  /* #if BTC_HF_CLIENT_INCLUDED */
#if BTC_HD_INCLUDED == TRUE
#include "btc_hd.h"
#endif /* BTC_HD_INCLUDED */
#if BTC_HH_INCLUDED == TRUE
#include "btc_hh.h"
#endif /* BTC_HH_INCLUDED */
#endif /* #if CLASSIC_BT_INCLUDED */
#endif

#if (BLE_INCLUDED == TRUE)
#include "btc_gap_ble.h"
#endif

#if CONFIG_BLE_MESH
#include "btc_ble_mesh_ble.h"
#include "btc_ble_mesh_prov.h"
#include "btc_ble_mesh_health_model.h"
#include "btc_ble_mesh_config_model.h"
#include "btc_ble_mesh_agg_model.h"
#include "btc_ble_mesh_brc_model.h"
#include "btc_ble_mesh_df_model.h"
#include "btc_ble_mesh_lcd_model.h"
#include "btc_ble_mesh_odp_model.h"
#include "btc_ble_mesh_prb_model.h"
#include "btc_ble_mesh_rpr_model.h"
#include "btc_ble_mesh_sar_model.h"
#include "btc_ble_mesh_srpl_model.h"
#include "btc_ble_mesh_generic_model.h"
#include "btc_ble_mesh_lighting_model.h"
#include "btc_ble_mesh_sensor_model.h"
#include "btc_ble_mesh_time_scene_model.h"
#include "btc_ble_mesh_mbt_model.h"
#endif /* #if CONFIG_BLE_MESH */

#define BTC_TASK_PINNED_TO_CORE         (TASK_PINNED_TO_CORE)
#define BTC_TASK_STACK_SIZE             (BT_BTC_TASK_STACK_SIZE + BT_TASK_EXTRA_STACK_SIZE)	//by menuconfig
#define BTC_TASK_NAME                   "BTC_TASK"
#define BTC_TASK_PRIO                   (BT_TASK_MAX_PRIORITIES - 6)
#define BTC_TASK_WORKQUEUE_NUM          (2)
#define BTC_TASK_WORKQUEUE0_LEN         (0)
#define BTC_TASK_WORKQUEUE1_LEN         (5)

osi_thread_t *btc_thread;

static const btc_func_t profile_tab[BTC_PID_NUM] = {
#ifdef CONFIG_BT_BLUEDROID_ENABLED
    [BTC_PID_MAIN_INIT]   = {btc_main_call_handler,       NULL                    },
    [BTC_PID_DEV]         = {btc_dev_call_handler,        NULL                    },
#if (GATTS_INCLUDED == TRUE)
    [BTC_PID_GATTS]       = {btc_gatts_call_handler,      btc_gatts_cb_handler    },
#endif  ///GATTS_INCLUDED == TRUE
#if (GATTC_INCLUDED == TRUE)
    [BTC_PID_GATTC]       = {btc_gattc_call_handler,      btc_gattc_cb_handler    },
#endif  ///GATTC_INCLUDED == TRUE
#if (GATTS_INCLUDED == TRUE || GATTC_INCLUDED == TRUE)
    [BTC_PID_GATT_COMMON] = {btc_gatt_com_call_handler,   NULL                    },
#endif //GATTC_INCLUDED == TRUE || GATTS_INCLUDED == TRUE
#if (BLE_INCLUDED == TRUE)
    [BTC_PID_GAP_BLE]     = {btc_gap_ble_call_handler,    btc_gap_ble_cb_handler  },
#else
    [BTC_PID_GAP_BLE]     = {NULL, NULL},
#endif  ///BLE_INCLUDED == TRUE
    [BTC_PID_BLE_HID]     = {NULL, NULL},
    [BTC_PID_SPPLIKE]     = {NULL, NULL},
    [BTC_PID_DM_SEC]      = {NULL,                        btc_dm_sec_cb_handler   },
#endif
#if (BLUFI_INCLUDED == TRUE)
    [BTC_PID_BLUFI]       = {btc_blufi_call_handler,      btc_blufi_cb_handler    },
#endif  ///BLUFI_INCLUDED == TRUE
    [BTC_PID_ALARM]       = {btc_alarm_handler,           NULL                    },
#ifdef CONFIG_BT_BLUEDROID_ENABLED
#if CLASSIC_BT_INCLUDED
#if (BTC_GAP_BT_INCLUDED == TRUE)
    [BTC_PID_GAP_BT]    = {btc_gap_bt_call_handler,     btc_gap_bt_cb_handler   },
#endif /* (BTC_GAP_BT_INCLUDED == TRUE) */
    [BTC_PID_PRF_QUE]     = {btc_profile_queue_handler,   NULL                    },
#if BTC_AV_INCLUDED
    [BTC_PID_A2DP]        = {btc_a2dp_call_handler,       btc_a2dp_cb_handler     },
    [BTC_PID_AVRC_CT]     = {btc_avrc_ct_call_handler,    NULL                    },
    [BTC_PID_AVRC_TG]     = {btc_avrc_tg_call_handler,    NULL                    },
#endif /* #if BTC_AV_INCLUDED */
#if (BTC_SPP_INCLUDED == TRUE)
    [BTC_PID_SPP]         = {btc_spp_call_handler,        btc_spp_cb_handler      },
#endif /* #if (BTC_SPP_INCLUDED == TRUE) */
#if (BTC_L2CAP_INCLUDED == TRUE)
    [BTC_PID_L2CAP]       = {btc_l2cap_call_handler,      btc_l2cap_cb_handler    },
#endif /* #if (BTC_L2CAP_INCLUDED == TRUE) */
#if (BTC_SDP_INCLUDED == TRUE)
    [BTC_PID_SDP]       = {btc_sdp_call_handler,          btc_sdp_cb_handler      },
#endif /* #if (BTC_SDP_INCLUDED == TRUE) */
#if BTC_HF_INCLUDED
    [BTC_PID_HF]   = {btc_hf_call_handler,  btc_hf_cb_handler},
#endif  /* #if BTC_HF_INCLUDED */
#if BTC_HF_CLIENT_INCLUDED
    [BTC_PID_HF_CLIENT]   = {btc_hf_client_call_handler,  btc_hf_client_cb_handler},
#endif  /* #if BTC_HF_CLIENT_INCLUDED */
#if BTC_HD_INCLUDED
    [BTC_PID_HD]          = {btc_hd_call_handler,          btc_hd_cb_handler      },
#endif
#if BTC_HH_INCLUDED
    [BTC_PID_HH]          = {btc_hh_call_handler,          btc_hh_cb_handler      },
#endif
#endif /* #if CLASSIC_BT_INCLUDED */
#endif
#if CONFIG_BLE_MESH
    [BTC_PID_PROV]              = {btc_ble_mesh_prov_call_handler,              btc_ble_mesh_prov_cb_handler             },
    [BTC_PID_MODEL]             = {btc_ble_mesh_model_call_handler,             btc_ble_mesh_model_cb_handler            },
#if CONFIG_BLE_MESH_HEALTH_CLI
    [BTC_PID_HEALTH_CLIENT]     = {btc_ble_mesh_health_client_call_handler,     btc_ble_mesh_health_client_cb_handler    },
#endif /* CONFIG_BLE_MESH_HEALTH_CLI */
#if CONFIG_BLE_MESH_HEALTH_SRV
    [BTC_PID_HEALTH_SERVER]     = {btc_ble_mesh_health_server_call_handler,     btc_ble_mesh_health_server_cb_handler    },
#endif /* CONFIG_BLE_MESH_HEALTH_SRV */
#if CONFIG_BLE_MESH_CFG_CLI
    [BTC_PID_CONFIG_CLIENT]     = {btc_ble_mesh_config_client_call_handler,     btc_ble_mesh_config_client_cb_handler    },
#endif /* CONFIG_BLE_MESH_CFG_CLI */
    [BTC_PID_CONFIG_SERVER]     = {NULL,                                        btc_ble_mesh_config_server_cb_handler    },
#if CONFIG_BLE_MESH_AGG_CLI
    [BTC_PID_AGG_CLIENT]        = {btc_ble_mesh_agg_client_call_handler,        btc_ble_mesh_agg_client_cb_handler       },
#endif /* CONFIG_BLE_MESH_AGG_CLI */
#if CONFIG_BLE_MESH_AGG_SRV
    [BTC_PID_AGG_SERVER]        = {NULL,                                        btc_ble_mesh_agg_server_cb_handler       },
#endif /* CONFIG_BLE_MESH_AGG_SRV */
#if CONFIG_BLE_MESH_BRC_CLI
    [BTC_PID_BRC_CLIENT]        = {btc_ble_mesh_brc_client_call_handler,        btc_ble_mesh_brc_client_cb_handler       },
#endif /* CONFIG_BLE_MESH_BRC_CLI */
#if CONFIG_BLE_MESH_BRC_SRV
    [BTC_PID_BRC_SERVER]        = {NULL,                                        btc_ble_mesh_brc_server_cb_handler       },
#endif  /* CONFIG_BLE_MESH_BRC_SRV */
#if CONFIG_BLE_MESH_DF_CLI
    [BTC_PID_DF_CLIENT]         = {btc_ble_mesh_df_client_call_handler,         btc_ble_mesh_df_client_cb_handler        },
#endif /* CONFIG_BLE_MESH_DF_CLI */
#if CONFIG_BLE_MESH_DF_SRV
    [BTC_PID_DF_SERVER]         = {NULL,                                        btc_ble_mesh_df_server_cb_handler        },
#endif /* CONFIG_BLE_MESH_DF_SRV */
#if CONFIG_BLE_MESH_LCD_CLI
    [BTC_PID_LCD_CLIENT]        = {btc_ble_mesh_lcd_client_call_handler,        btc_ble_mesh_lcd_client_cb_handler       },
#endif /* CONFIG_BLE_MESH_LCD_CLI */
#if CONFIG_BLE_MESH_LCD_SRV
    [BTC_PID_LCD_SERVER]        = {NULL,                                        btc_ble_mesh_lcd_server_cb_handler       },
#endif /* CONFIG_BLE_MESH_LCD_SRV */
#if CONFIG_BLE_MESH_ODP_CLI
    [BTC_PID_ODP_CLIENT]        = {btc_ble_mesh_odp_client_call_handler,        btc_ble_mesh_odp_client_cb_handler       },
#endif /* CONFIG_BLE_MESH_ODP_CLI */
#if CONFIG_BLE_MESH_ODP_SRV
    [BTC_PID_ODP_SERVER]        = {NULL,                                        btc_ble_mesh_odp_server_cb_handler       },
#endif /* CONFIG_BLE_MESH_ODP_SRV */
#if CONFIG_BLE_MESH_PRB_CLI
    [BTC_PID_PRB_CLIENT]        = {btc_ble_mesh_prb_client_call_handler,        btc_ble_mesh_prb_client_cb_handler       },
#endif  /* CONFIG_BLE_MESH_PRB_CLI */
#if CONFIG_BLE_MESH_PRB_SRV
    [BTC_PID_PRB_SERVER]        = {NULL,                                        btc_ble_mesh_prb_server_cb_handler       },
#endif  /*CONFIG_BLE_MESH_PRB_SRV*/
#if CONFIG_BLE_MESH_RPR_CLI
    [BTC_PID_RPR_CLIENT]        = {btc_ble_mesh_rpr_client_call_handler,        btc_ble_mesh_rpr_client_cb_handler       },
#endif /* CONFIG_BLE_MESH_RPR_CLI */
#if CONFIG_BLE_MESH_RPR_SRV
    [BTC_PID_RPR_SERVER]        = {NULL,                                        btc_ble_mesh_rpr_server_cb_handler       },
#endif /* CONFIG_BLE_MESH_RPR_SRV */
#if CONFIG_BLE_MESH_SAR_CLI
    [BTC_PID_SAR_CLIENT]        = {btc_ble_mesh_sar_client_call_handler,        btc_ble_mesh_sar_client_cb_handler       },
#endif /* CONFIG_BLE_MESH_SAR_CLI */
#if CONFIG_BLE_MESH_SAR_SRV
    [BTC_PID_SAR_SERVER]        = {NULL,                                        btc_ble_mesh_sar_server_cb_handler       },
#endif /* CONFIG_BLE_MESH_SAR_SRV */
#if CONFIG_BLE_MESH_SRPL_CLI
    [BTC_PID_SRPL_CLIENT]       = {btc_ble_mesh_srpl_client_call_handler,       btc_ble_mesh_srpl_client_cb_handler      },
#endif /* CONFIG_BLE_MESH_SRPL_CLI */
#if CONFIG_BLE_MESH_SRPL_SRV
    [BTC_PID_SRPL_SERVER]       = {NULL,                                        btc_ble_mesh_srpl_server_cb_handler      },
#endif /* CONFIG_BLE_MESH_SRPL_SRV */
#if CONFIG_BLE_MESH_GENERIC_CLIENT
    [BTC_PID_GENERIC_CLIENT]    = {btc_ble_mesh_generic_client_call_handler,    btc_ble_mesh_generic_client_cb_handler   },
#endif /* CONFIG_BLE_MESH_GENERIC_CLIENT */
#if CONFIG_BLE_MESH_LIGHTING_CLIENT
    [BTC_PID_LIGHTING_CLIENT]   = {btc_ble_mesh_lighting_client_call_handler,   btc_ble_mesh_lighting_client_cb_handler  },
#endif /* CONFIG_BLE_MESH_LIGHTING_CLIENT */
#if CONFIG_BLE_MESH_SENSOR_CLI
    [BTC_PID_SENSOR_CLIENT]     = {btc_ble_mesh_sensor_client_call_handler,     btc_ble_mesh_sensor_client_cb_handler    },
#endif /* CONFIG_BLE_MESH_SENSOR_CLI */
#if CONFIG_BLE_MESH_TIME_SCENE_CLIENT
    [BTC_PID_TIME_SCENE_CLIENT] = {btc_ble_mesh_time_scene_client_call_handler, btc_ble_mesh_time_scene_client_cb_handler},
#endif /* CONFIG_BLE_MESH_TIME_SCENE_CLIENT */
#if CONFIG_BLE_MESH_GENERIC_SERVER
    [BTC_PID_GENERIC_SERVER]    = {NULL,                                        btc_ble_mesh_generic_server_cb_handler   },
#endif /* CONFIG_BLE_MESH_GENERIC_SERVER */
#if CONFIG_BLE_MESH_LIGHTING_SERVER
    [BTC_PID_LIGHTING_SERVER]   = {NULL,                                        btc_ble_mesh_lighting_server_cb_handler  },
#endif /* CONFIG_BLE_MESH_LIGHTING_SERVER */
#if CONFIG_BLE_MESH_SENSOR_SERVER
    [BTC_PID_SENSOR_SERVER]     = {NULL,                                        btc_ble_mesh_sensor_server_cb_handler    },
#endif /* CONFIG_BLE_MESH_SENSOR_SERVER */
#if CONFIG_BLE_MESH_TIME_SCENE_SERVER
    [BTC_PID_TIME_SCENE_SERVER] = {NULL,                                        btc_ble_mesh_time_scene_server_cb_handler},
#endif /* CONFIG_BLE_MESH_TIME_SCENE_SERVER */
#if CONFIG_BLE_MESH_MBT_CLI
    [BTC_PID_MBT_CLIENT]        = {btc_ble_mesh_mbt_client_call_handler,        btc_ble_mesh_mbt_client_cb_handler       },
#endif /* CONFIG_BLE_MESH_MBT_CLI */
#if CONFIG_BLE_MESH_MBT_SRV
    [BTC_PID_MBT_SERVER]        = {btc_ble_mesh_mbt_server_call_handler,        btc_ble_mesh_mbt_server_cb_handler       },
#endif /* CONFIG_BLE_MESH_MBT_SRV */
#if CONFIG_BLE_MESH_BLE_COEX_SUPPORT
    [BTC_PID_BLE_MESH_BLE_COEX] = {btc_ble_mesh_ble_call_handler,               btc_ble_mesh_ble_cb_handler              },
#endif /* CONFIG_BLE_MESH_BLE_COEX_SUPPORT */
#endif /* #if CONFIG_BLE_MESH */
};

/*****************************************************************************
**
** Function         btc_task
**
** Description      Process profile Task Thread.
******************************************************************************/
static void btc_thread_handler(void *arg)
{
    btc_msg_t *msg = (btc_msg_t *)arg;

    BTC_TRACE_DEBUG("%s msg %u %u %u %p\n", __func__, msg->sig, msg->pid, msg->act, msg->arg);
    switch (msg->sig) {
    case BTC_SIG_API_CALL:
        profile_tab[msg->pid].btc_call(msg);
        break;
    case BTC_SIG_API_CB:
        profile_tab[msg->pid].btc_cb(msg);
        break;
    default:
        break;
    }

    osi_free(msg);
}

static bt_status_t btc_task_post(btc_msg_t *msg, uint32_t timeout)
{
    if (osi_thread_post(btc_thread, btc_thread_handler, msg, 0, timeout) == false) {
        return BT_STATUS_BUSY;
    }

    return BT_STATUS_SUCCESS;
}

/**
 * transfer an message to another module in the different task.
 * @param  msg       message
 * @param  arg       paramter
 * @param  arg_len   length of paramter
 * @param  copy_func deep copy function
 * @param  free_func deep free function
 * @return           BT_STATUS_SUCCESS: success
 *                   others: fail
 */
bt_status_t btc_transfer_context(btc_msg_t *msg, void *arg, int arg_len, btc_arg_deep_copy_t copy_func,
                                    btc_arg_deep_free_t free_func)
{
    btc_msg_t* lmsg;
    bt_status_t ret;
    //                              arg XOR arg_len
    if ((msg == NULL) || ((arg == NULL) == !(arg_len == 0))) {
        BTC_TRACE_WARNING("%s Invalid parameters\n", __func__);
        return BT_STATUS_PARM_INVALID;
    }

    BTC_TRACE_DEBUG("%s msg %u %u %u %p\n", __func__, msg->sig, msg->pid, msg->act, arg);

    lmsg = (btc_msg_t *)osi_malloc(sizeof(btc_msg_t) + arg_len);
    if (lmsg == NULL) {
        BTC_TRACE_WARNING("%s No memory\n", __func__);
        return BT_STATUS_NOMEM;
    }

    memcpy(lmsg, msg, sizeof(btc_msg_t));
    if (arg) {
        memset(lmsg->arg, 0x00, arg_len);    //important, avoid arg which have no length
        memcpy(lmsg->arg, arg, arg_len);
        if (copy_func) {
            copy_func(lmsg, lmsg->arg, arg);
        }
    }

    ret = btc_task_post(lmsg, OSI_THREAD_MAX_TIMEOUT);
    if (ret != BT_STATUS_SUCCESS) {
        if (copy_func && free_func) {
            free_func(lmsg);
        }
        osi_free(lmsg);
    }

    return ret;
}

/**
 * transfer an message to another module in tha same task.
 * @param  msg       message
 * @return           BT_STATUS_SUCCESS: success
 *                   others: fail
 */
bt_status_t btc_inter_profile_call(btc_msg_t *msg)
{
    if (msg == NULL) {
        return BT_STATUS_PARM_INVALID;
    }

    switch (msg->sig) {
    case BTC_SIG_API_CALL:
        profile_tab[msg->pid].btc_call(msg);
        break;
    case BTC_SIG_API_CB:
        profile_tab[msg->pid].btc_cb(msg);
        break;
    default:
        break;
    }
    return BT_STATUS_SUCCESS;
}


#if BTC_DYNAMIC_MEMORY

static void btc_deinit_mem(void) {
    if (btc_dm_cb_ptr) {
        osi_free(btc_dm_cb_ptr);
        btc_dm_cb_ptr = NULL;
    }

    if (btc_profile_cb_tab) {
        osi_free(btc_profile_cb_tab);
        btc_profile_cb_tab = NULL;
    }

#if (BLE_INCLUDED == TRUE)
    if (gl_bta_adv_data_ptr) {
        osi_free(gl_bta_adv_data_ptr);
        gl_bta_adv_data_ptr = NULL;
    }

    if (gl_bta_scan_rsp_data_ptr) {
        osi_free(gl_bta_scan_rsp_data_ptr);
        gl_bta_scan_rsp_data_ptr = NULL;
    }
#endif  ///BLE_INCLUDED == TRUE

#if GATTS_INCLUDED == TRUE && GATT_DYNAMIC_MEMORY == TRUE
    if (btc_creat_tab_env_ptr) {
        osi_free(btc_creat_tab_env_ptr);
        btc_creat_tab_env_ptr = NULL;
    }
#if (BLUFI_INCLUDED == TRUE)
    if (blufi_env_ptr) {
        osi_free(blufi_env_ptr);
        blufi_env_ptr = NULL;
    }
#endif
#endif

#if BTC_HF_CLIENT_INCLUDED == TRUE && HFP_DYNAMIC_MEMORY == TRUE
    if (hf_client_local_param_ptr) {
        osi_free(hf_client_local_param_ptr);
        hf_client_local_param_ptr = NULL;
    }
#endif

#if BTC_AV_INCLUDED == TRUE && AVRC_DYNAMIC_MEMORY == TRUE
    if (btc_rc_cb_ptr) {
        osi_free(btc_rc_cb_ptr);
        btc_rc_cb_ptr = NULL;
    }
    if (bta_av_co_cb_ptr) {
        osi_free(bta_av_co_cb_ptr);
        bta_av_co_cb_ptr = NULL;
    }
#endif
}

static bt_status_t btc_init_mem(void) {
    if ((btc_dm_cb_ptr = (btc_dm_cb_t *)osi_malloc(sizeof(btc_dm_cb_t))) == NULL) {
        goto error_exit;
    }
    memset((void *)btc_dm_cb_ptr, 0, sizeof(btc_dm_cb_t));

    if ((btc_profile_cb_tab = (void **)osi_malloc(sizeof(void *) * BTC_PID_NUM)) == NULL) {
        goto error_exit;
    }
    memset((void *)btc_profile_cb_tab, 0, sizeof(void *) * BTC_PID_NUM);

#if (BLE_INCLUDED == TRUE)
    if ((gl_bta_adv_data_ptr = (tBTA_BLE_ADV_DATA *)osi_malloc(sizeof(tBTA_BLE_ADV_DATA))) == NULL) {
        goto error_exit;
    }
    memset((void *)gl_bta_adv_data_ptr, 0, sizeof(tBTA_BLE_ADV_DATA));

    if ((gl_bta_scan_rsp_data_ptr = (tBTA_BLE_ADV_DATA *)osi_malloc(sizeof(tBTA_BLE_ADV_DATA))) == NULL) {
        goto error_exit;
    }
    memset((void *)gl_bta_scan_rsp_data_ptr, 0, sizeof(tBTA_BLE_ADV_DATA));
#endif  ///BLE_INCLUDED == TRUE

#if GATTS_INCLUDED == TRUE && GATT_DYNAMIC_MEMORY == TRUE
    if ((btc_creat_tab_env_ptr = (esp_btc_creat_tab_t *)osi_malloc(sizeof(esp_btc_creat_tab_t))) == NULL) {
        goto error_exit;
    }
    memset((void *)btc_creat_tab_env_ptr, 0, sizeof(esp_btc_creat_tab_t));
#if (BLUFI_INCLUDED == TRUE)
    if ((blufi_env_ptr = (tBLUFI_ENV *)osi_malloc(sizeof(tBLUFI_ENV))) == NULL) {
        goto error_exit;
    }
    memset((void *)blufi_env_ptr, 0, sizeof(tBLUFI_ENV));
#endif
#endif

#if BTC_HF_CLIENT_INCLUDED == TRUE && HFP_DYNAMIC_MEMORY == TRUE
    if ((hf_client_local_param_ptr = (hf_client_local_param_t *)osi_malloc(sizeof(hf_client_local_param_t))) == NULL) {
        goto error_exit;
    }
    memset((void *)hf_client_local_param_ptr, 0, sizeof(hf_client_local_param_t));
#endif

#if BTC_AV_INCLUDED == TRUE && AVRC_DYNAMIC_MEMORY == TRUE
    if ((btc_rc_cb_ptr = (btc_rc_cb_t *)osi_malloc(sizeof(btc_rc_cb_t))) == NULL) {
        goto error_exit;
    }
    memset((void *)btc_rc_cb_ptr, 0, sizeof(btc_rc_cb_t));
    if ((bta_av_co_cb_ptr = (tBTA_AV_CO_CB *)osi_malloc(sizeof(tBTA_AV_CO_CB))) == NULL) {
        goto error_exit;
    }
    memset((void *)bta_av_co_cb_ptr, 0, sizeof(tBTA_AV_CO_CB));
#endif

    return BT_STATUS_SUCCESS;

error_exit:;
    btc_deinit_mem();
    return BT_STATUS_NOMEM;
}
#endif ///BTC_DYNAMIC_MEMORY

bt_status_t btc_init(void)
{
    const size_t workqueue_len[] = {BTC_TASK_WORKQUEUE0_LEN, BTC_TASK_WORKQUEUE1_LEN};
    btc_thread = osi_thread_create(BTC_TASK_NAME, BTC_TASK_STACK_SIZE, BTC_TASK_PRIO, BTC_TASK_PINNED_TO_CORE,
                                   BTC_TASK_WORKQUEUE_NUM, workqueue_len);
    if (btc_thread == NULL) {
        return BT_STATUS_NOMEM;
    }

#if BTC_DYNAMIC_MEMORY
    if (btc_init_mem() != BT_STATUS_SUCCESS){
        return BT_STATUS_NOMEM;
    }
#endif

#if (BLE_INCLUDED == TRUE)
    btc_gap_callback_init();
    btc_gap_ble_init();
#endif  ///BLE_INCLUDED == TRUE

#if SCAN_QUEUE_CONGEST_CHECK
    btc_adv_list_init();
#endif
    /* TODO: initial the profile_tab */
    return BT_STATUS_SUCCESS;
}

void btc_deinit(void)
{
#if BTC_DYNAMIC_MEMORY
    btc_deinit_mem();
#endif

    osi_thread_free(btc_thread);
    btc_thread = NULL;
#if (BLE_INCLUDED == TRUE)
    btc_gap_ble_deinit();
#endif  ///BLE_INCLUDED == TRUE
#if SCAN_QUEUE_CONGEST_CHECK
    btc_adv_list_deinit();
#endif
}

bool btc_check_queue_is_congest(void)
{
    if (osi_thread_queue_wait_size(btc_thread, 0) >= BT_QUEUE_CONGEST_SIZE) {
        return true;
    }

    return false;
}

int get_btc_work_queue_size(void)
{
    return osi_thread_queue_wait_size(btc_thread, 0);
}

osi_thread_t *btc_get_current_thread(void)
{
    return btc_thread;
}
