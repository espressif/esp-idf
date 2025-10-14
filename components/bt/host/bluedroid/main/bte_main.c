/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  Filename:      bte_main.c
 *
 *  Description:   Contains BTE core stack initialization and shutdown code
 *
 ******************************************************************************/


#include "common/bt_defs.h"
#include "common/bt_common_types.h"
#include "common/bte.h"
#include "stack/btu.h"
#include "common/bt_trace.h"
#include "osi/osi.h"
#include "osi/alarm.h"
#include "osi/hash_map.h"
#include "osi/hash_functions.h"
#include "device/controller.h"
#include "hci/hci_layer.h"
#include "bta/bta_api.h"

/*******************************************************************************
**  Constants & Macros
*******************************************************************************/

/******************************************************************************
**  Variables
******************************************************************************/

/*******************************************************************************
**  Static variables
*******************************************************************************/
static const hci_t *hci;

/*******************************************************************************
**  Static functions
*******************************************************************************/
static void bte_main_disable(void);
static void bte_main_enable(void);

/*******************************************************************************
**  Externs
*******************************************************************************/

bluedroid_init_done_cb_t bluedroid_init_done_cb;

extern void osi_mem_dbg_init(void);
#if (BT_BLE_DYNAMIC_ENV_MEMORY == TRUE)
extern void free_controller_param(void);
#endif
/******************************************************************************
**
** Function         bte_main_boot_entry
**
** Description      BTE MAIN API - Entry point for BTE chip/stack initialization
**
** Returns          None
**
******************************************************************************/
int bte_main_boot_entry(bluedroid_init_done_cb_t cb)
{
    hci = hci_layer_get_interface();
    if (!hci) {
        APPL_TRACE_ERROR("%s could not get hci layer interface.\n", __func__);
        return -2;
    }

    bluedroid_init_done_cb = cb;

    osi_init();

    //Enable HCI
    bte_main_enable();

    return 0;
}

/******************************************************************************
**
** Function         bte_main_shutdown
**
** Description      BTE MAIN API - Shutdown code for BTE chip/stack
**
** Returns          None
**
******************************************************************************/
void bte_main_shutdown(void)
{
#if (BLE_INCLUDED == TRUE)
    BTA_VendorCleanup();
#endif

#if (BT_BLE_DYNAMIC_ENV_MEMORY == TRUE)
    free_controller_param();
#endif

    bte_main_disable();

    osi_deinit();
}

/******************************************************************************
**
** Function         bte_main_enable
**
** Description      BTE MAIN API - Creates all the BTE tasks. Should be called
**                  part of the Bluetooth stack enable sequence
**
** Returns          None
**
******************************************************************************/
static void bte_main_enable(void)
{
    APPL_TRACE_DEBUG("Enable HCI\n");
    if (hci_start_up()) {
        APPL_TRACE_ERROR("Start HCI Host Layer Failure\n");
        return;
    }

    //Now Test Case Not Supported BTU
    BTU_StartUp();
}

/******************************************************************************
**
** Function         bte_main_disable
**
** Description      BTE MAIN API - Destroys all the BTE tasks. Should be called
**                  part of the Bluetooth stack disable sequence
**
** Returns          None
**
******************************************************************************/
static void bte_main_disable(void)
{
    /*
        APPL_TRACE_DEBUG("%s", __FUNCTION__);

        module_shut_down(get_module(HCI_MODULE));
        module_shut_down(get_module(BTSNOOP_MODULE));
    */

    hci_shut_down();

    BTU_ShutDown();
}

/******************************************************************************
**
** Function         bte_main_postload_cfg
**
** Description      BTE MAIN API - Stack postload configuration
**
** Returns          None
**
******************************************************************************/
void bte_main_postload_cfg(void)
{
    hci->do_postload();
}

#if (defined(HCILP_INCLUDED) && HCILP_INCLUDED == TRUE)
/******************************************************************************
**
** Function         bte_main_enable_lpm
**
** Description      BTE MAIN API - Enable/Disable low power mode operation
**
** Returns          None
**
******************************************************************************/
void bte_main_enable_lpm(BOOLEAN enable)
{
    /*Enable Low power ?*/
    //hci->send_low_power_command(enable ? LPM_ENABLE : LPM_DISABLE);
}

/******************************************************************************
**
** Function         bte_main_lpm_allow_bt_device_sleep
**
** Description      BTE MAIN API - Allow BT controller goest to sleep
**
** Returns          None
**
******************************************************************************/
void bte_main_lpm_allow_bt_device_sleep(void)
{
    /**/
    //hci->send_low_power_command(LPM_WAKE_DEASSERT);
}

/******************************************************************************
**
** Function         bte_main_lpm_wake_bt_device
**
** Description      BTE MAIN API - Wake BT controller up if it is in sleep mode
**
** Returns          None
**
******************************************************************************/
void bte_main_lpm_wake_bt_device(void)
{
    //hci->send_low_power_command(LPM_WAKE_ASSERT);
}
#endif  // HCILP_INCLUDED

/******************************************************************************
**
** Function         bte_main_hci_send
**
** Description      BTE MAIN API - This function is called by the upper stack to
**                  send an HCI message. The function displays a protocol trace
**                  message (if enabled), and then calls the 'transmit' function
**                  associated with the currently selected HCI transport
**
** Returns          None
**
******************************************************************************/
void bte_main_hci_send (BT_HDR *p_msg, UINT16 event)
{
    UINT16 sub_event = event & BT_SUB_EVT_MASK;  /* local controller ID */

    p_msg->event = event;

    //counter_add("main.tx.packets", 1);
    //counter_add("main.tx.bytes", p_msg->len);

    if ((sub_event == LOCAL_BR_EDR_CONTROLLER_ID) || \
            (sub_event == LOCAL_BLE_CONTROLLER_ID)) {
        hci->transmit_downward(event, p_msg);
    } else {
        //APPL_TRACE_ERROR("Invalid Controller ID. Discarding message.");
        osi_free(p_msg);
    }
}
