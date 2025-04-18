/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>

#include "sdkconfig.h"
#include "esp_bt_cfg.h"
#include "esp_bit_defs.h"

/* External functions or variables
 ************************************************************************
 */
int base_stack_initEnv(void);
void base_stack_deinitEnv(void);
int base_stack_enable(void);
void base_stack_disable(void);

int conn_stack_initEnv(void);
void conn_stack_deinitEnv(void);
int conn_stack_enable(void);
void conn_stack_disable(void);

#if CONFIG_BT_LE_ERROR_SIM_ENABLED
int conn_errorSim_initEnv(void);
void conn_errorSim_deinitEnv(void);
int conn_errorSim_enable(void);
void conn_errorSim_disable(void);
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED

#if (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)
void adv_stack_enableClearLegacyAdvVsCmd(bool en);
void scan_stack_enableAdvFlowCtrlVsCmd(bool en);
void advFilter_stack_enableDupExcListVsCmd(bool en);
void arr_stack_enableMultiConnVsCmd(bool en);
void pcl_stack_enableSetRssiThreshVsCmd(bool en);
void chanSel_stack_enableSetCsaVsCmd(bool en);
void log_stack_enableLogsRelatedVsCmd(bool en);
void hci_stack_enableSetVsEvtMaskVsCmd(bool en);
void winWiden_stack_enableSetConstPeerScaVsCmd(bool en);
#if CONFIG_IDF_TARGET_ESP32C61_ECO3
void conn_stack_enableSetPrefTxRxCntVsCmd(bool en);
#endif // CONFIG_IDF_TARGET_ESP32C61_ECO3

void adv_stack_enableScanReqRxdVsEvent(bool en);
void conn_stack_enableChanMapUpdCompVsEvent(bool en);
void sleep_stack_enableWakeupVsEvent(bool en);
#endif // (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)
#if CONFIG_BT_LE_RXBUF_OPT_ENABLED
extern void mmgmt_enableRxbufOptFeature(void);
#endif // CONFIG_BT_LE_RXBUF_OPT_ENABLED

/* Local functions definition
 ***************************************************************************
 */
#if (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)
void ble_stack_enableVsCmds(bool en)
{
    adv_stack_enableClearLegacyAdvVsCmd(en);
    advFilter_stack_enableDupExcListVsCmd(en);
    scan_stack_enableAdvFlowCtrlVsCmd(en);
    arr_stack_enableMultiConnVsCmd(en);
    pcl_stack_enableSetRssiThreshVsCmd(en);
    chanSel_stack_enableSetCsaVsCmd(en);
    log_stack_enableLogsRelatedVsCmd(en);
    hci_stack_enableSetVsEvtMaskVsCmd(en);
    winWiden_stack_enableSetConstPeerScaVsCmd(en);
#if CONFIG_IDF_TARGET_ESP32C61_ECO3
    conn_stack_enableSetPrefTxRxCntVsCmd(en);
#endif // CONFIG_IDF_TARGET_ESP32C61_ECO3
}

void ble_stack_enableVsEvents(bool en)
{
    adv_stack_enableScanReqRxdVsEvent(en);
    conn_stack_enableChanMapUpdCompVsEvent(en);

#if CONFIG_BT_LE_SLEEP_ENABLE
    sleep_stack_enableWakeupVsEvent(en);
#endif // CONFIG_BT_LE_SLEEP_ENABLE
}
#endif // (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)

int ble_stack_initEnv(void)
{
    int rc;

    rc = base_stack_initEnv();
    if (rc) {
        return rc;
    }

#if DEFAULT_BT_LE_MAX_CONNECTIONS
    rc = conn_stack_initEnv();
    if (rc) {
        return rc;
    }
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    rc = conn_errorSim_initEnv();
    if (rc) {
        return rc;
    }
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS

    return 0;
}

void ble_stack_deinitEnv(void)
{
#if DEFAULT_BT_LE_MAX_CONNECTIONS
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_errorSim_deinitEnv();
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_stack_deinitEnv();
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS

    base_stack_deinitEnv();
}

int ble_stack_enable(void)
{
    int rc;

    rc = base_stack_enable();
    if (rc) {
        return rc;
    }

#if DEFAULT_BT_LE_MAX_CONNECTIONS
    rc = conn_stack_enable();
    if (rc) {
        return rc;
    }
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    rc = conn_errorSim_enable();
    if (rc) {
        return rc;
    }
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS

#if (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)
    ble_stack_enableVsCmds(true);
    ble_stack_enableVsEvents(true);
#endif // (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)

#if CONFIG_BT_LE_RXBUF_OPT_ENABLED
    mmgmt_enableRxbufOptFeature();
#endif // CONFIG_BT_LE_RXBUF_OPT_ENABLED

    return 0;
}

void ble_stack_disable(void)
{
#if (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)
    ble_stack_enableVsEvents(false);
    ble_stack_enableVsCmds(false);
#endif // (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)

#if DEFAULT_BT_LE_MAX_CONNECTIONS
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_errorSim_disable();
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_stack_disable();
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS

    base_stack_disable();
}
