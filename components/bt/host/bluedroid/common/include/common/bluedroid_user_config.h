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

#ifndef __BLUEDROID_USER_CONFIG_H__
#define __BLUEDROID_USER_CONFIG_H__

/* All the configuration from SDK defined here */
#include "bt_common.h"
#include "bt_user_config.h"

/**********************************************************
 * Thread/Task reference
 **********************************************************/
#ifdef CONFIG_BT_BTU_TASK_STACK_SIZE
#define UC_BTU_TASK_STACK_SIZE              CONFIG_BT_BTU_TASK_STACK_SIZE
#else
#define UC_BTU_TASK_STACK_SIZE              4096
#endif


/**********************************************************
 * Profile reference
 **********************************************************/
//Classic BT reference
#ifdef CONFIG_BT_CLASSIC_ENABLED
#define UC_BT_CLASSIC_ENABLED               CONFIG_BT_CLASSIC_ENABLED
#else
#define UC_BT_CLASSIC_ENABLED               FALSE
#endif

//A2DP
#ifdef CONFIG_BT_A2DP_ENABLE
#define UC_BT_A2DP_ENABLED                  CONFIG_BT_A2DP_ENABLE
#else
#define UC_BT_A2DP_ENABLED                  FALSE
#endif

//SPP
#ifdef CONFIG_BT_SPP_ENABLED
#define UC_BT_SPP_ENABLED                   CONFIG_BT_SPP_ENABLED
#else
#define UC_BT_SPP_ENABLED                   FALSE
#endif

//HFP
#ifdef CONFIG_BT_HFP_CLIENT_ENABLE
#define UC_BT_HFP_CLIENT_ENABLED            CONFIG_BT_HFP_CLIENT_ENABLE
#else
#define UC_BT_HFP_CLIENT_ENABLED            FALSE
#endif

//SSP
#ifdef CONFIG_BT_SSP_ENABLED
#define UC_BT_SSP_ENABLED                   CONFIG_BT_SSP_ENABLED
#else
#define UC_BT_SSP_ENABLED                   FALSE
#endif

//BLE
#ifdef CONFIG_BT_BLE_ENABLED
#define UC_BT_BLE_ENABLED                   CONFIG_BT_BLE_ENABLED
#else
#define UC_BT_BLE_ENABLED                   FALSE
#endif

//GATTS
#ifdef CONFIG_BT_GATTS_ENABLE
#define UC_BT_GATTS_ENABLE                  CONFIG_BT_GATTS_ENABLE
#else
#define UC_BT_GATTS_ENABLE                  FALSE
#endif

//GATTC
#ifdef CONFIG_BT_GATTC_ENABLE
#define UC_BT_GATTC_ENABLE                  CONFIG_BT_GATTC_ENABLE
#else
#define UC_BT_GATTC_ENABLE                  FALSE
#endif

//GATTC CACHE
#ifdef CONFIG_BT_GATTC_CACHE_NVS_FLASH
#define UC_BT_GATTC_CACHE_NVS_FLASH_ENABLED    CONFIG_BT_GATTC_CACHE_NVS_FLASH
#else
#define UC_BT_GATTC_CACHE_NVS_FLASH_ENABLED    FALSE
#endif

//SMP
#ifdef CONFIG_BT_SMP_ENABLE
#define UC_BT_SMP_ENABLE                       CONFIG_BT_SMP_ENABLE
#else
#define UC_BT_SMP_ENABLE                       FALSE
#endif

//SMP_SLAVE_CON_PARAMS_UPD_ENABLE
#ifdef CONFIG_BT_SMP_SLAVE_CON_PARAMS_UPD_ENABLE
#define UC_BT_SMP_SLAVE_CON_PARAMS_UPD_ENABLE  CONFIG_BT_SMP_SLAVE_CON_PARAMS_UPD_ENABLE
#else
#define UC_BT_SMP_SLAVE_CON_PARAMS_UPD_ENABLE  FALSE
#endif

//BTDM_BLE_ADV_REPORT_FLOW_CTRL_SUPP
#ifdef CONFIG_BTDM_BLE_ADV_REPORT_FLOW_CTRL_SUPP
#define UC_BTDM_BLE_ADV_REPORT_FLOW_CTRL_SUPP  CONFIG_BTDM_BLE_ADV_REPORT_FLOW_CTRL_SUPP
#else
#define UC_BTDM_BLE_ADV_REPORT_FLOW_CTRL_SUPP  FALSE
#endif

//SMP_SLAVE_CON_PARAMS_UPD_ENABLE
#ifdef CONFIG_BTDM_BLE_ADV_REPORT_FLOW_CTRL_NUM
#define UC_BTDM_BLE_ADV_REPORT_FLOW_CTRL_NUM   CONFIG_BTDM_BLE_ADV_REPORT_FLOW_CTRL_NUM
#else
#define UC_BTDM_BLE_ADV_REPORT_FLOW_CTRL_NUM   100
#endif

//BTDM_BLE_ADV_REPORT_FLOW_CTRL_SUPP
#ifdef CONFIG_BTDM_BLE_ADV_REPORT_DISCARD_THRSHOLD
#define UC_BTDM_BLE_ADV_REPORT_DISCARD_THRSHOLD     CONFIG_BTDM_BLE_ADV_REPORT_DISCARD_THRSHOLD
#else
#define UC_BTDM_BLE_ADV_REPORT_DISCARD_THRSHOLD     20
#endif

//BT ACL CONNECTIONS
#ifdef CONFIG_BT_ACL_CONNECTIONS
#define UC_BT_ACL_CONNECTIONS                   CONFIG_BT_ACL_CONNECTIONS
#else
#define UC_BT_ACL_CONNECTIONS                   5
#endif

//BT_BLE_ESTAB_LINK_CONN_TOUT
#ifdef CONFIG_BT_BLE_ESTAB_LINK_CONN_TOUT
#define UC_BT_BLE_ESTAB_LINK_CONN_TOUT          CONFIG_BT_BLE_ESTAB_LINK_CONN_TOUT
#else
#define UC_BT_BLE_ESTAB_LINK_CONN_TOUT          30
#endif


//HOST QUEUE CONGEST CHECK
#ifdef CONFIG_BT_BLE_HOST_QUEUE_CONGESTION_CHECK
#define UC_BT_BLE_HOST_QUEUE_CONGESTION_CHECK   CONFIG_BT_BLE_HOST_QUEUE_CONGESTION_CHECK
#else
#define UC_BT_BLE_HOST_QUEUE_CONGESTION_CHECK   FALSE
#endif

#ifdef CONFIG_BT_GATTS_SEND_SERVICE_CHANGE_MODE
#define UC_BT_GATTS_SEND_SERVICE_CHANGE_MODE    CONFIG_BT_GATTS_SEND_SERVICE_CHANGE_MODE
#else
#define UC_BT_GATTS_SEND_SERVICE_CHANGE_MODE    0
#endif

#ifdef CONFIG_BT_BLE_ACT_SCAN_REP_ADV_SCAN
#define UC_BT_BLE_ACT_SCAN_REP_ADV_SCAN         CONFIG_BT_BLE_ACT_SCAN_REP_ADV_SCAN
#else
#define UC_BT_BLE_ACT_SCAN_REP_ADV_SCAN         FALSE
#endif

//SCO VOICE OVER HCI
#ifdef CONFIG_BT_HFP_AUDIO_DATA_PATH_HCI
#define UC_BT_HFP_AUDIO_DATA_PATH_HCI           CONFIG_BT_HFP_AUDIO_DATA_PATH_HCI
#else
#define UC_BT_HFP_AUDIO_DATA_PATH_HCI           FALSE
#endif

//Wide Band Speech
#ifdef CONFIG_BT_HFP_WBS_ENABLE
#define UC_BT_HFP_WBS_ENABLE                    CONFIG_BT_HFP_WBS_ENABLE
#else
#define UC_BT_HFP_WBS_ENABLE                    FALSE
#endif

/**********************************************************
 * Memory reference
 **********************************************************/

//MEMORY ALLOCATOR
#ifdef CONFIG_BT_ALLOCATION_FROM_SPIRAM_FIRST
#define UC_HEAP_ALLOCATION_FROM_SPIRAM_FIRST    CONFIG_BT_ALLOCATION_FROM_SPIRAM_FIRST
#else
#define UC_HEAP_ALLOCATION_FROM_SPIRAM_FIRST    FALSE
#endif

//MEMORY DEBUG
#ifdef CONFIG_BT_BLUEDROID_MEM_DEBUG
#define UC_BT_BLUEDROID_MEM_DEBUG               CONFIG_BT_BLUEDROID_MEM_DEBUG
#else
#define UC_BT_BLUEDROID_MEM_DEBUG               FALSE
#endif


/**********************************************************
 * Trace reference
 **********************************************************/

#ifdef CONFIG_BT_LOG_HCI_TRACE_LEVEL
#define UC_BT_LOG_HCI_TRACE_LEVEL           CONFIG_BT_LOG_HCI_TRACE_LEVEL
#else
#define UC_BT_LOG_HCI_TRACE_LEVEL           UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_BTM_TRACE_LEVEL
#define UC_BT_LOG_BTM_TRACE_LEVEL           CONFIG_BT_LOG_BTM_TRACE_LEVEL
#else
#define UC_BT_LOG_BTM_TRACE_LEVEL           UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_L2CAP_TRACE_LEVEL
#define UC_BT_LOG_L2CAP_TRACE_LEVEL         CONFIG_BT_LOG_L2CAP_TRACE_LEVEL
#else
#define UC_BT_LOG_L2CAP_TRACE_LEVEL         UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_RFCOMM_TRACE_LEVEL
#define UC_BT_LOG_RFCOMM_TRACE_LEVEL        CONFIG_BT_LOG_RFCOMM_TRACE_LEVEL
#else
#define UC_BT_LOG_RFCOMM_TRACE_LEVEL        UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_SDP_TRACE_LEVEL
#define UC_BT_LOG_SDP_TRACE_LEVEL           CONFIG_BT_LOG_SDP_TRACE_LEVEL
#else
#define UC_BT_LOG_SDP_TRACE_LEVEL           UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_GAP_TRACE_LEVEL
#define UC_BT_LOG_GAP_TRACE_LEVEL           CONFIG_BT_LOG_GAP_TRACE_LEVEL
#else
#define UC_BT_LOG_GAP_TRACE_LEVEL           UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_BNEP_TRACE_LEVEL
#define UC_BT_LOG_BNEP_TRACE_LEVEL          CONFIG_BT_LOG_BNEP_TRACE_LEVEL
#else
#define UC_BT_LOG_BNEP_TRACE_LEVEL          UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_PAN_TRACE_LEVEL
#define UC_BT_LOG_PAN_TRACE_LEVEL           CONFIG_BT_LOG_PAN_TRACE_LEVEL
#else
#define UC_BT_LOG_PAN_TRACE_LEVEL           UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_A2D_TRACE_LEVEL
#define UC_BT_LOG_A2D_TRACE_LEVEL           CONFIG_BT_LOG_A2D_TRACE_LEVEL
#else
#define UC_BT_LOG_A2D_TRACE_LEVEL           UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_AVDT_TRACE_LEVEL
#define UC_BT_LOG_AVDT_TRACE_LEVEL          CONFIG_BT_LOG_AVDT_TRACE_LEVEL
#else
#define UC_BT_LOG_AVDT_TRACE_LEVEL          UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_AVCT_TRACE_LEVEL
#define UC_BT_LOG_AVCT_TRACE_LEVEL          CONFIG_BT_LOG_AVCT_TRACE_LEVEL
#else
#define UC_BT_LOG_AVCT_TRACE_LEVEL          UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_AVRC_TRACE_LEVEL
#define UC_BT_LOG_AVRC_TRACE_LEVEL          CONFIG_BT_LOG_AVRC_TRACE_LEVEL
#else
#define UC_BT_LOG_AVRC_TRACE_LEVEL          UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_MCA_TRACE_LEVEL
#define UC_BT_LOG_MCA_TRACE_LEVEL           CONFIG_BT_LOG_MCA_TRACE_LEVEL
#else
#define UC_BT_LOG_MCA_TRACE_LEVEL           UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_HID_TRACE_LEVEL
#define UC_BT_LOG_HID_TRACE_LEVEL           CONFIG_BT_LOG_HID_TRACE_LEVEL
#else
#define UC_BT_LOG_HID_TRACE_LEVEL           UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_APPL_TRACE_LEVEL
#define UC_BT_LOG_APPL_TRACE_LEVEL          CONFIG_BT_LOG_APPL_TRACE_LEVEL
#else
#define UC_BT_LOG_APPL_TRACE_LEVEL          UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_GATT_TRACE_LEVEL
#define UC_BT_LOG_GATT_TRACE_LEVEL          CONFIG_BT_LOG_GATT_TRACE_LEVEL
#else
#define UC_BT_LOG_GATT_TRACE_LEVEL          UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_SMP_TRACE_LEVEL
#define UC_BT_LOG_SMP_TRACE_LEVEL           CONFIG_BT_LOG_SMP_TRACE_LEVEL
#else
#define UC_BT_LOG_SMP_TRACE_LEVEL           UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_BTIF_TRACE_LEVEL
#define UC_BT_LOG_BTIF_TRACE_LEVEL          CONFIG_BT_LOG_BTIF_TRACE_LEVEL
#else
#define UC_BT_LOG_BTIF_TRACE_LEVEL          UC_TRACE_LEVEL_WARNING
#endif

#ifdef CONFIG_BT_LOG_BLUFI_TRACE_LEVEL
#define UC_BT_LOG_BLUFI_TRACE_LEVEL         CONFIG_BT_LOG_BLUFI_TRACE_LEVEL
#else
#define UC_BT_LOG_BLUFI_TRACE_LEVEL         UC_TRACE_LEVEL_WARNING
#endif

#endif /* __BLUEDROID_USER_CONFIG_H__ */
