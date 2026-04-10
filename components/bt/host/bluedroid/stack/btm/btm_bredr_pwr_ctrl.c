/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************
 *
 *  This file contains functions for BR/EDR power control.
 *
 ******************************************************************************/

#include <string.h>

#include "stack/bt_types.h"
#include "common/bt_target.h"
#include "stack/btu.h"
#include "stack/btm_api.h"
#include "btm_int.h"
#include "stack/hcidefs.h"

#if (CLASSIC_BT_INCLUDED == TRUE)

#define BTM_BREDR_PWR_CTRL_REPLY_TIMEOUT   3       /* 3 second timeout waiting for responses */

#if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
static void btm_read_new_conn_tx_pwr_lvl_vsc_cmpl_cb(tBTM_VSC_CMPL *p1)
{
    tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_read_new_conn_tx_pwr_lvl_cmpl_cb;
    tBTM_READ_NEW_CONN_TX_PWR_LVL_RESULTS result;

    btu_stop_timer(&btm_cb.devcb.read_new_conn_tx_pwr_lvl_timer);
    btm_cb.devcb.p_read_new_conn_tx_pwr_lvl_cmpl_cb = NULL;

    memset(&result, 0, sizeof(result));
    result.status = BTM_ERR_PROCESSING;
    result.hci_status = HCI_ERR_UNSPECIFIED;

    if (p1 == NULL || p1->p_param_buf == NULL || p1->param_len < 1) {
        goto out;
    }

    UINT8 *p = p1->p_param_buf;
    STREAM_TO_UINT8(result.hci_status, p);
    if (result.hci_status != HCI_SUCCESS) {
        goto out;
    }

    if (p1->param_len < 1 + 2) {
        goto out;
    }

    STREAM_TO_UINT8(result.pwr_lvl_min, p);
    STREAM_TO_UINT8(result.pwr_lvl_max, p);
    result.status = BTM_SUCCESS;

out:
    if (p_cb) {
        (*p_cb)(&result);
    }
}

tBTM_STATUS BTM_ReadNewConnTxPwrLvl(tBTM_CMPL_CB *p_cb)
{
    tBTM_STATUS status;
    tBTM_READ_NEW_CONN_TX_PWR_LVL_RESULTS result;

    if (btm_cb.devcb.p_read_new_conn_tx_pwr_lvl_cmpl_cb) {
        result.status = BTM_BUSY;
        (*p_cb)(&result);
        return BTM_BUSY;
    }

    btu_start_timer(&btm_cb.devcb.read_new_conn_tx_pwr_lvl_timer,
                    BTU_TTYPE_BTM_BREDR_PWR_CTRL,
                    BTM_BREDR_PWR_CTRL_REPLY_TIMEOUT);
    btm_cb.devcb.p_read_new_conn_tx_pwr_lvl_cmpl_cb = p_cb;

    status = BTM_VendorSpecificCommand(HCI_VENDOR_BT_RD_NEW_CONN_TX_PWR_LVL,
                                       0, NULL, btm_read_new_conn_tx_pwr_lvl_vsc_cmpl_cb);
    if (status != BTM_CMD_STARTED) {
        btm_cb.devcb.p_read_new_conn_tx_pwr_lvl_cmpl_cb = NULL;
        btu_stop_timer(&btm_cb.devcb.read_new_conn_tx_pwr_lvl_timer);
        result.status = status;
        (*p_cb)(&result);
        return BTM_NO_RESOURCES;
    }

    return BTM_CMD_STARTED;
}

static void btm_write_new_conn_tx_pwr_lvl_vsc_cmpl_cb(tBTM_VSC_CMPL *p1)
{
    tBTM_CMPL_CB *p_cb = btm_cb.devcb.p_write_new_conn_tx_pwr_lvl_cmpl_cb;
    tBTM_WRITE_NEW_CONN_TX_PWR_LVL_RESULTS result;

    btu_stop_timer(&btm_cb.devcb.write_new_conn_tx_pwr_lvl_timer);
    btm_cb.devcb.p_write_new_conn_tx_pwr_lvl_cmpl_cb = NULL;

    memset(&result, 0, sizeof(result));
    result.status = BTM_ERR_PROCESSING;
    result.hci_status = HCI_ERR_UNSPECIFIED;

    if (p1 == NULL || p1->p_param_buf == NULL || p1->param_len < 1) {
        goto out;
    }

    UINT8 *p = p1->p_param_buf;
    STREAM_TO_UINT8(result.hci_status, p);
    if (result.hci_status != HCI_SUCCESS) {
        goto out;
    }

    result.status = BTM_SUCCESS;

out:
    if (p_cb) {
        (*p_cb)(&result);
    }
}

tBTM_STATUS BTM_WriteNewConnTxPwrLvl(INT8 pwr_lvl_min, INT8 pwr_lvl_max, tBTM_CMPL_CB *p_cb)
{
    tBTM_STATUS status;
    tBTM_WRITE_NEW_CONN_TX_PWR_LVL_RESULTS result;
    UINT8 param[2];
    UINT8 *p = param;

    if (btm_cb.devcb.p_write_new_conn_tx_pwr_lvl_cmpl_cb) {
        result.status = BTM_BUSY;
        (*p_cb)(&result);
        return BTM_BUSY;
    }

    btu_start_timer(&btm_cb.devcb.write_new_conn_tx_pwr_lvl_timer,
                    BTU_TTYPE_BTM_BREDR_PWR_CTRL,
                    BTM_BREDR_PWR_CTRL_REPLY_TIMEOUT);
    btm_cb.devcb.p_write_new_conn_tx_pwr_lvl_cmpl_cb = p_cb;

    INT8_TO_STREAM(p, pwr_lvl_min);
    INT8_TO_STREAM(p, pwr_lvl_max);
    status = BTM_VendorSpecificCommand(HCI_VENDOR_BT_WR_NEW_CONN_TX_PWR_LVL,
                                       sizeof(param), param, btm_write_new_conn_tx_pwr_lvl_vsc_cmpl_cb);
    if (status != BTM_CMD_STARTED) {
        btm_cb.devcb.p_write_new_conn_tx_pwr_lvl_cmpl_cb = NULL;
        btu_stop_timer(&btm_cb.devcb.write_new_conn_tx_pwr_lvl_timer);
        result.status = status;
        (*p_cb)(&result);
        return BTM_NO_RESOURCES;
    }

    return BTM_CMD_STARTED;
}

static void btm_read_tx_pwr_lvl_vsc_cmpl_cb(tBTM_TX_PWR_LVL_TYPE type, tBTM_VSC_CMPL *p1,
                                            TIMER_LIST_ENT *timer, tBTM_CMPL_CB **pp_cb)
{
    tBTM_CMPL_CB *p_cb = *pp_cb;
    tBTM_READ_TX_PWR_LVL_RESULTS result;

    btu_stop_timer(timer);
    *pp_cb = NULL;

    memset(&result, 0, sizeof(result));
    result.type = type;
    result.status = BTM_ERR_PROCESSING;
    result.hci_status = HCI_ERR_UNSPECIFIED;

    if (p1 == NULL || p1->p_param_buf == NULL || p1->param_len < 1) {
        goto out;
    }

    UINT8 *p = p1->p_param_buf;
    STREAM_TO_UINT8(result.hci_status, p);
    if (result.hci_status != HCI_SUCCESS) {
        goto out;
    }

    if (p1->param_len < 1 + 1) {
        goto out;
    }

    STREAM_TO_UINT8(result.tx_power, p);
    result.status = BTM_SUCCESS;

out:
    if (p_cb) {
        (*p_cb)(&result);
    }
}

static void btm_read_inq_tx_pwr_lvl_vsc_cmpl_cb(tBTM_VSC_CMPL *p1)
{
    btm_read_tx_pwr_lvl_vsc_cmpl_cb(BTM_TX_PWR_LVL_INQ, p1,
                                    &btm_cb.devcb.read_inq_tx_pwr_lvl_timer,
                                    &btm_cb.devcb.p_read_inq_tx_pwr_lvl_cmpl_cb);
}

static void btm_read_page_tx_pwr_lvl_vsc_cmpl_cb(tBTM_VSC_CMPL *p1)
{
    btm_read_tx_pwr_lvl_vsc_cmpl_cb(BTM_TX_PWR_LVL_PAGE, p1,
                                    &btm_cb.devcb.read_page_tx_pwr_lvl_timer,
                                    &btm_cb.devcb.p_read_page_tx_pwr_lvl_cmpl_cb);
}

static void btm_read_pscan_tx_pwr_lvl_vsc_cmpl_cb(tBTM_VSC_CMPL *p1)
{
    btm_read_tx_pwr_lvl_vsc_cmpl_cb(BTM_TX_PWR_LVL_PSCAN, p1,
                                    &btm_cb.devcb.read_pscan_tx_pwr_lvl_timer,
                                    &btm_cb.devcb.p_read_pscan_tx_pwr_lvl_cmpl_cb);
}
#endif // #if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)

tBTM_STATUS BTM_ReadBredrTxPwrLvl(tBTM_TX_PWR_LVL_TYPE type, tBTM_CMPL_CB *p_cb)
{
#if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
    UINT16 opcode;
    TIMER_LIST_ENT *timer;
    tBTM_CMPL_CB **pp_cb;
    tBTM_VSC_CMPL_CB *p_vsc_cb;

    tBTM_STATUS status;
    tBTM_READ_TX_PWR_LVL_RESULTS result;
#endif // #if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)

    switch (type) {
    case BTM_TX_PWR_LVL_ISCAN:
        // standard HCI
        return BTM_ReadInquiryRspTxPower(p_cb);
#if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
    case BTM_TX_PWR_LVL_INQ:
        opcode = HCI_VENDOR_BT_RD_INQ_TX_PWR_LVL;
        timer = &btm_cb.devcb.read_inq_tx_pwr_lvl_timer;
        pp_cb = &btm_cb.devcb.p_read_inq_tx_pwr_lvl_cmpl_cb;
        p_vsc_cb = btm_read_inq_tx_pwr_lvl_vsc_cmpl_cb;
        break;
    case BTM_TX_PWR_LVL_PAGE:
        opcode = HCI_VENDOR_BT_RD_PAGE_TX_PWR_LVL;
        timer = &btm_cb.devcb.read_page_tx_pwr_lvl_timer;
        pp_cb = &btm_cb.devcb.p_read_page_tx_pwr_lvl_cmpl_cb;
        p_vsc_cb = btm_read_page_tx_pwr_lvl_vsc_cmpl_cb;
        break;
    case BTM_TX_PWR_LVL_PSCAN:
        opcode = HCI_VENDOR_BT_RD_PSCAN_TX_PWR_LVL;
        timer = &btm_cb.devcb.read_pscan_tx_pwr_lvl_timer;
        pp_cb = &btm_cb.devcb.p_read_pscan_tx_pwr_lvl_cmpl_cb;
        p_vsc_cb = btm_read_pscan_tx_pwr_lvl_vsc_cmpl_cb;
        break;
#endif // #if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
    default:
        BTM_TRACE_ERROR("BTM_ReadBredrTxPwrLvl invalid type: %d", type);
        return BTM_ILLEGAL_VALUE;
    }

#if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
    if (*pp_cb) {
        memset(&result, 0, sizeof(result));
        result.type = type;
        result.status = BTM_BUSY;
        if (p_cb) {
            (*p_cb)(&result);
        }
        return BTM_BUSY;
    }

    btu_start_timer(timer, BTU_TTYPE_BTM_BREDR_PWR_CTRL, BTM_BREDR_PWR_CTRL_REPLY_TIMEOUT);
    *pp_cb = p_cb;

    status = BTM_VendorSpecificCommand(opcode, 0, NULL, p_vsc_cb);
    if (status != BTM_CMD_STARTED) {
        *pp_cb = NULL;
        btu_stop_timer(timer);
        memset(&result, 0, sizeof(result));
        result.type = type;
        result.status = status;
        if (p_cb) {
            (*p_cb)(&result);
        }
        return BTM_NO_RESOURCES;
    }

    return BTM_CMD_STARTED;
#endif // #if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
}

#if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
static void btm_write_tx_pwr_lvl_vsc_cmpl_cb(tBTM_TX_PWR_LVL_TYPE type, tBTM_VSC_CMPL *p1,
                                             TIMER_LIST_ENT *timer, tBTM_CMPL_CB **pp_cb)
{
    tBTM_CMPL_CB *p_cb = *pp_cb;
    tBTM_WRITE_TX_PWR_LVL_RESULTS result;

    btu_stop_timer(timer);
    *pp_cb = NULL;

    memset(&result, 0, sizeof(result));
    result.type = type;
    result.status = BTM_ERR_PROCESSING;
    result.hci_status = HCI_ERR_UNSPECIFIED;

    if (p1 == NULL || p1->p_param_buf == NULL || p1->param_len < 1) {
        goto out;
    }

    UINT8 *p = p1->p_param_buf;
    STREAM_TO_UINT8(result.hci_status, p);
    if (result.hci_status != HCI_SUCCESS) {
        goto out;
    }

    result.status = BTM_SUCCESS;

out:
    if (p_cb) {
        (*p_cb)(&result);
    }
}

static void btm_write_iscan_tx_pwr_lvl_vsc_cmpl_cb(tBTM_VSC_CMPL *p1)
{
    btm_write_tx_pwr_lvl_vsc_cmpl_cb(BTM_TX_PWR_LVL_ISCAN, p1,
                                     &btm_cb.devcb.write_iscan_tx_pwr_lvl_timer,
                                     &btm_cb.devcb.p_write_iscan_tx_pwr_lvl_cmpl_cb);
}

static void btm_write_page_tx_pwr_lvl_vsc_cmpl_cb(tBTM_VSC_CMPL *p1)
{
    btm_write_tx_pwr_lvl_vsc_cmpl_cb(BTM_TX_PWR_LVL_PAGE, p1,
                                     &btm_cb.devcb.write_page_tx_pwr_lvl_timer,
                                     &btm_cb.devcb.p_write_page_tx_pwr_lvl_cmpl_cb);
}

static void btm_write_pscan_tx_pwr_lvl_vsc_cmpl_cb(tBTM_VSC_CMPL *p1)
{
    btm_write_tx_pwr_lvl_vsc_cmpl_cb(BTM_TX_PWR_LVL_PSCAN, p1,
                                     &btm_cb.devcb.write_pscan_tx_pwr_lvl_timer,
                                     &btm_cb.devcb.p_write_pscan_tx_pwr_lvl_cmpl_cb);
}
#endif // #if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)

tBTM_STATUS BTM_WriteBredrTxPwrLvl(tBTM_TX_PWR_LVL_TYPE type, INT8 tx_power, tBTM_CMPL_CB *p_cb)
{
#if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
    UINT16 opcode;
    TIMER_LIST_ENT *timer;
    tBTM_CMPL_CB **pp_cb;
    tBTM_VSC_CMPL_CB *p_vsc_cb;

    tBTM_STATUS status;
    tBTM_WRITE_TX_PWR_LVL_RESULTS result;
    UINT8 param[1];
    UINT8 *p = param;
#endif // #if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)

    switch (type) {
    case BTM_TX_PWR_LVL_INQ:
        // standard HCI
        return BTM_WriteInquiryTxPower(tx_power, p_cb);
#if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
    case BTM_TX_PWR_LVL_ISCAN:
        opcode = HCI_VENDOR_BT_WR_ISCAN_TX_PWR_LVL;
        timer = &btm_cb.devcb.write_iscan_tx_pwr_lvl_timer;
        pp_cb = &btm_cb.devcb.p_write_iscan_tx_pwr_lvl_cmpl_cb;
        p_vsc_cb = btm_write_iscan_tx_pwr_lvl_vsc_cmpl_cb;
        break;
    case BTM_TX_PWR_LVL_PAGE:
        opcode = HCI_VENDOR_BT_WR_PAGE_TX_PWR_LVL;
        timer = &btm_cb.devcb.write_page_tx_pwr_lvl_timer;
        pp_cb = &btm_cb.devcb.p_write_page_tx_pwr_lvl_cmpl_cb;
        p_vsc_cb = btm_write_page_tx_pwr_lvl_vsc_cmpl_cb;
        break;
    case BTM_TX_PWR_LVL_PSCAN:
        opcode = HCI_VENDOR_BT_WR_PSCAN_TX_PWR_LVL;
        timer = &btm_cb.devcb.write_pscan_tx_pwr_lvl_timer;
        pp_cb = &btm_cb.devcb.p_write_pscan_tx_pwr_lvl_cmpl_cb;
        p_vsc_cb = btm_write_pscan_tx_pwr_lvl_vsc_cmpl_cb;
        break;
#endif // #if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
    default:
        BTM_TRACE_ERROR("BTM_WriteBredrTxPwrLvl invalid type: %d", type);
        return BTM_ILLEGAL_VALUE;
    }

#if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
    if (*pp_cb) {
        memset(&result, 0, sizeof(result));
        result.type = type;
        result.status = BTM_BUSY;
        if (p_cb) {
            (*p_cb)(&result);
        }
        return BTM_BUSY;
    }

    btu_start_timer(timer, BTU_TTYPE_BTM_BREDR_PWR_CTRL, BTM_BREDR_PWR_CTRL_REPLY_TIMEOUT);
    *pp_cb = p_cb;

    INT8_TO_STREAM(p, tx_power);
    status = BTM_VendorSpecificCommand(opcode, sizeof(param), param, p_vsc_cb);
    if (status != BTM_CMD_STARTED) {
        *pp_cb = NULL;
        btu_stop_timer(timer);
        memset(&result, 0, sizeof(result));
        result.type = type;
        result.status = status;
        if (p_cb) {
            (*p_cb)(&result);
        }
        return BTM_NO_RESOURCES;
    }

    return BTM_CMD_STARTED;
#endif // #if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
}

/*******************************************************************************
**
** Function         btm_bredr_pwr_ctrl_timeout
**
** Description      This function processes a BR/EDR power control vsc timeout.
**
** Returns          void
**
*******************************************************************************/
void btm_bredr_pwr_ctrl_timeout(TIMER_LIST_ENT *p_tle)
{
    if (p_tle == &btm_cb.devcb.write_inq_txpwer_timer) {
        btm_write_inq_tx_power_complete(NULL);
    } else if (p_tle == &btm_cb.devcb.read_iscan_txpwer_timer) {
        btm_read_iscan_tx_power_complete(NULL);

#if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
    } else if (p_tle == &btm_cb.devcb.read_new_conn_tx_pwr_lvl_timer) {
        btm_read_new_conn_tx_pwr_lvl_vsc_cmpl_cb(NULL);
    } else if (p_tle == &btm_cb.devcb.write_new_conn_tx_pwr_lvl_timer) {
        btm_write_new_conn_tx_pwr_lvl_vsc_cmpl_cb(NULL);
    } else if (p_tle == &btm_cb.devcb.read_inq_tx_pwr_lvl_timer) {
        btm_read_inq_tx_pwr_lvl_vsc_cmpl_cb(NULL);
    } else if (p_tle == &btm_cb.devcb.read_page_tx_pwr_lvl_timer) {
        btm_read_page_tx_pwr_lvl_vsc_cmpl_cb(NULL);
    } else if (p_tle == &btm_cb.devcb.read_pscan_tx_pwr_lvl_timer) {
        btm_read_pscan_tx_pwr_lvl_vsc_cmpl_cb(NULL);
    } else if (p_tle == &btm_cb.devcb.write_iscan_tx_pwr_lvl_timer) {
        btm_write_iscan_tx_pwr_lvl_vsc_cmpl_cb(NULL);
    } else if (p_tle == &btm_cb.devcb.write_page_tx_pwr_lvl_timer) {
        btm_write_page_tx_pwr_lvl_vsc_cmpl_cb(NULL);
    } else if (p_tle == &btm_cb.devcb.write_pscan_tx_pwr_lvl_timer) {
        btm_write_pscan_tx_pwr_lvl_vsc_cmpl_cb(NULL);

#endif // #if (ESP_BT_CLASSIC_ENABLE_POWER_CTRL_VSC == TRUE)
    }
}

#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
