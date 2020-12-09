// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <string.h>
#include "common/bt_target.h"
#include "btc/btc_task.h"
#include "common/bt_trace.h"
#include "osi/thread.h"
#include "common/bt_defs.h"
#include "osi/allocator.h"
#include "btc/btc_main.h"
#include "btc/btc_dev.h"
#include "btc_gatts.h"
#include "btc_gattc.h"
#include "btc_gatt_common.h"
#include "btc_gap_ble.h"
#include "btc_blufi_prf.h"
#include "btc/btc_dm.h"
#include "btc/btc_alarm.h"
#include "bta/bta_gatt_api.h"
#if CONFIG_CLASSIC_BT_ENABLED
#include "btc/btc_profile_queue.h"
#if (BTC_GAP_BT_INCLUDED == TRUE)
#include "btc_gap_bt.h"
#endif /* BTC_GAP_BT_INCLUDED == TRUE */
#if BTC_AV_INCLUDED
#include "btc_av.h"
#include "btc_avrc.h"
#endif /* #if BTC_AV_INCLUDED */
#if CONFIG_BT_SPP_ENABLED
#include "btc_spp.h"
#endif /* #if CONFIG_BT_SPP_ENABLED */
#if BTC_HF_CLIENT_INCLUDED
#include "btc_hf_client.h"
#endif  /* #if BTC_HF_CLIENT_INCLUDED */
#endif /* #if CONFIG_CLASSIC_BT_ENABLED */

#if CONFIG_BLE_MESH
#include "btc_ble_mesh_ble.h"
#include "btc_ble_mesh_prov.h"
#include "btc_ble_mesh_health_model.h"
#include "btc_ble_mesh_config_model.h"
#include "btc_ble_mesh_generic_model.h"
#include "btc_ble_mesh_lighting_model.h"
#include "btc_ble_mesh_sensor_model.h"
#include "btc_ble_mesh_time_scene_model.h"
#endif /* #if CONFIG_BLE_MESH */

static xTaskHandle  xBtcTaskHandle = NULL;
static xQueueHandle xBtcQueue = 0;

void bt_abort_with_coredump_log(uint16_t error);

static btc_func_t profile_tab[BTC_PID_NUM] = {
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
    [BTC_PID_GAP_BLE]     = {btc_gap_ble_call_handler,    btc_gap_ble_cb_handler  },
    [BTC_PID_BLE_HID]     = {NULL, NULL},
    [BTC_PID_SPPLIKE]     = {NULL, NULL},
#if (GATTS_INCLUDED == TRUE)
    [BTC_PID_BLUFI]       = {btc_blufi_call_handler,      btc_blufi_cb_handler    },
#endif  ///GATTS_INCLUDED == TRUE
    [BTC_PID_DM_SEC]      = {NULL,                        btc_dm_sec_cb_handler   },
    [BTC_PID_ALARM]       = {btc_alarm_handler,           NULL                    },
#if CONFIG_CLASSIC_BT_ENABLED
#if (BTC_GAP_BT_INCLUDED == TRUE)
    [BTC_PID_GAP_BT]    = {btc_gap_bt_call_handler,     btc_gap_bt_cb_handler   },
#endif /* (BTC_GAP_BT_INCLUDED == TRUE) */
    [BTC_PID_PRF_QUE]     = {btc_profile_queue_handler,   NULL                    },
#if BTC_AV_INCLUDED
    [BTC_PID_A2DP]        = {btc_a2dp_call_handler,       btc_a2dp_cb_handler     },
    [BTC_PID_AVRC_CT]     = {btc_avrc_ct_call_handler,    NULL                    },
    [BTC_PID_AVRC_TG]     = {btc_avrc_tg_call_handler,    NULL                    },
#endif /* #if BTC_AV_INCLUDED */
#if CONFIG_BT_SPP_ENABLED
    [BTC_PID_SPP]         = {btc_spp_call_handler,        btc_spp_cb_handler      },
#endif /* #if CONFIG_BT_SPP_ENABLED */
#if BTC_HF_CLIENT_INCLUDED
    [BTC_PID_HF_CLIENT]   = {btc_hf_client_call_handler,  btc_hf_client_cb_handler},
#endif  /* #if BTC_HF_CLIENT_INCLUDED */
#endif /* #if CONFIG_CLASSIC_BT_ENABLED */
#if CONFIG_BLE_MESH
    [BTC_PID_PROV]              = {btc_ble_mesh_prov_call_handler,              btc_ble_mesh_prov_cb_handler             },
    [BTC_PID_MODEL]             = {btc_ble_mesh_model_call_handler,             btc_ble_mesh_model_cb_handler            },
    [BTC_PID_HEALTH_CLIENT]     = {btc_ble_mesh_health_client_call_handler,     btc_ble_mesh_health_client_cb_handler    },
    [BTC_PID_HEALTH_SERVER]     = {btc_ble_mesh_health_server_call_handler,     btc_ble_mesh_health_server_cb_handler    },
    [BTC_PID_CONFIG_CLIENT]     = {btc_ble_mesh_config_client_call_handler,     btc_ble_mesh_config_client_cb_handler    },
    [BTC_PID_CONFIG_SERVER]     = {NULL,                                        btc_ble_mesh_config_server_cb_handler    },
    [BTC_PID_GENERIC_CLIENT]    = {btc_ble_mesh_generic_client_call_handler,    btc_ble_mesh_generic_client_cb_handler   },
    [BTC_PID_LIGHTING_CLIENT]   = {btc_ble_mesh_lighting_client_call_handler,   btc_ble_mesh_lighting_client_cb_handler  },
    [BTC_PID_SENSOR_CLIENT]     = {btc_ble_mesh_sensor_client_call_handler,     btc_ble_mesh_sensor_client_cb_handler    },
    [BTC_PID_TIME_SCENE_CLIENT] = {btc_ble_mesh_time_scene_client_call_handler, btc_ble_mesh_time_scene_client_cb_handler},
    [BTC_PID_GENERIC_SERVER]    = {NULL,                                        btc_ble_mesh_generic_server_cb_handler   },
    [BTC_PID_LIGHTING_SERVER]   = {NULL,                                        btc_ble_mesh_lighting_server_cb_handler  },
    [BTC_PID_SENSOR_SERVER]     = {NULL,                                        btc_ble_mesh_sensor_server_cb_handler    },
    [BTC_PID_TIME_SCENE_SERVER] = {NULL,                                        btc_ble_mesh_time_scene_server_cb_handler},
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
    [BTC_PID_BLE_MESH_BLE_COEX] = {btc_ble_mesh_ble_call_handler,               btc_ble_mesh_ble_cb_handler              },
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */
#endif /* #if CONFIG_BLE_MESH */
};

/*****************************************************************************
**
** Function         btc_task
**
** Description      Process profile Task Thread.
******************************************************************************/
static void btc_task(void *arg)
{
    btc_msg_t msg;

    for (;;) {
        if (pdTRUE == xQueueReceive(xBtcQueue, &msg, (portTickType)portMAX_DELAY)) {
            BTC_TRACE_DEBUG("%s msg %u %u %u %p\n", __func__, msg.sig, msg.pid, msg.act, msg.arg);
            switch (msg.sig) {
            case BTC_SIG_API_CALL:
                profile_tab[msg.pid].btc_call(&msg);
                break;
            case BTC_SIG_API_CB:
                profile_tab[msg.pid].btc_cb(&msg);
                break;
            default:
                break;
            }
            if (msg.arg) {
                osi_free(msg.arg);
            }
        }
    }
}

static bt_status_t btc_task_post(btc_msg_t *msg, task_post_t timeout)
{
    if (msg == NULL) {
        return BT_STATUS_PARM_INVALID;
    }

    if (xQueueSend(xBtcQueue, msg, timeout) != pdTRUE) {
        BTC_TRACE_ERROR("Btc Post failed\n");
    #ifdef TASK_MONITOR_MODE
        ets_printf("!! Btc Post failed.Timeout Abort !!");
        bt_abort_with_coredump_log(0);
    #endif
        return BT_STATUS_BUSY;
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t btc_transfer_context(btc_msg_t *msg, void *arg, int arg_len, btc_arg_deep_copy_t copy_func)
{
    btc_msg_t lmsg;

    if (msg == NULL) {
        return BT_STATUS_PARM_INVALID;
    }

    BTC_TRACE_DEBUG("%s msg %u %u %u %p\n", __func__, msg->sig, msg->pid, msg->act, arg);

    memcpy(&lmsg, msg, sizeof(btc_msg_t));
    if (arg) {
        lmsg.arg = (void *)osi_malloc(arg_len);
        if (lmsg.arg == NULL) {
            return BT_STATUS_NOMEM;
        }
        memset(lmsg.arg, 0x00, arg_len);    //important, avoid arg which have no length
        memcpy(lmsg.arg, arg, arg_len);
        if (copy_func) {
            copy_func(&lmsg, lmsg.arg, arg);
        }
    } else {
        lmsg.arg = NULL;
    }
#ifdef TASK_MONITOR_MODE
    return btc_task_post(&lmsg, TASK_POST_BLOCKING_WITH_TO);
#else
    return btc_task_post(&lmsg, TASK_POST_BLOCKING);
#endif

}

#if (CONFIG_SPIRAM_USE_MALLOC && CONFIG_BT_ALLOCATION_FROM_SPIRAM_FIRST)
    StaticQueue_t *btc_queue_buffer = NULL;
    uint8_t *btc_queue_storage = NULL;
#endif

int btc_init(void)
{
#if (CONFIG_SPIRAM_USE_MALLOC && CONFIG_BT_ALLOCATION_FROM_SPIRAM_FIRST)

    btc_queue_buffer = heap_caps_malloc(sizeof(StaticQueue_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (!btc_queue_buffer) {
        BTC_TRACE_ERROR("Btc Queue malloc fail in PSRAM.\n");
        return BT_STATUS_NOMEM;
    }

    btc_queue_storage = heap_caps_malloc((BTC_TASK_QUEUE_LEN*sizeof(btc_msg_t)), MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
    if (!btc_queue_storage ) {
        BTC_TRACE_ERROR("Btc Queue malloc fail in PSRAM.\n");
        return BT_STATUS_NOMEM;
    }

    xBtcQueue = xQueueCreateStatic(BTC_TASK_QUEUE_LEN, sizeof(btc_msg_t), btc_queue_storage, btc_queue_buffer);
    BTC_TRACE_API("Btc Queue malloc in PSRAM Success.\n");
#else
    BTC_TRACE_API("xBtcQueue in internal RAM.");
    xBtcQueue = xQueueCreate(BTC_TASK_QUEUE_LEN, sizeof(btc_msg_t));
#endif
    xTaskCreatePinnedToCore(btc_task, "Btc_task", BTC_TASK_STACK_SIZE, NULL, BTC_TASK_PRIO, &xBtcTaskHandle, BTC_TASK_PINNED_TO_CORE);
    if (xBtcTaskHandle == NULL || xBtcQueue == 0){
        return BT_STATUS_NOMEM;
    }
    btc_gap_callback_init();
#if SCAN_QUEUE_CONGEST_CHECK
    btc_adv_list_init();
#endif
    /* TODO: initial the profile_tab */
    return BT_STATUS_SUCCESS;
}

void btc_deinit(void)
{
    vTaskDelete(xBtcTaskHandle);
    vQueueDelete(xBtcQueue);
#if SCAN_QUEUE_CONGEST_CHECK
    btc_adv_list_deinit();
#endif

#if (CONFIG_SPIRAM_USE_MALLOC && CONFIG_BT_ALLOCATION_FROM_SPIRAM_FIRST)
    if(btc_queue_buffer){
        heap_caps_free(btc_queue_buffer);
        btc_queue_buffer = NULL;
    }
    if(btc_queue_storage){
        heap_caps_free(btc_queue_storage);
        btc_queue_storage = NULL;
    }
#endif
    xBtcTaskHandle = NULL;
    xBtcQueue = 0;
}

bool btc_check_queue_is_congest(void)
{
    UBaseType_t wait_size = uxQueueMessagesWaiting(xBtcQueue);
    if(wait_size >= QUEUE_CONGEST_SIZE) {
        return true;
    }
    return false;
}
