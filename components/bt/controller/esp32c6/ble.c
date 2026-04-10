/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
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

int adv_stack_initEnv(void);
void adv_stack_deinitEnv(void);
int adv_stack_enable(void);
void adv_stack_disable(void);

int extAdv_stack_initEnv(void);
void extAdv_stack_deinitEnv(void);
int extAdv_stack_enable(void);
void extAdv_stack_disable(void);

int scan_stack_initEnv(void);
void scan_stack_deinitEnv(void);
int scan_stack_enable(void);
void scan_stack_disable(void);

int scan_stack_initEnv(void);
void scan_stack_deinitEnv(void);
int scan_stack_enable(void);
void scan_stack_disable(void);

int sync_stack_initEnv(void);
void sync_stack_deinitEnv(void);
int sync_stack_enable(void);
void sync_stack_disable(void);

#if CONFIG_BT_LE_DTM_ENABLED
int dtm_stack_initEnv(void);
void dtm_stack_deinitEnv(void);
int dtm_stack_enable(void);
void dtm_stack_disable(void);
#endif // CONFIG_BT_LE_DTM_ENABLED

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

#if DEFAULT_BT_LE_PAWR_SUPPORTED || CONFIG_BT_LE_ERROR_SIM_ENABLED
int ble_single_env_init(void);
void ble_single_env_deinit(void);
int ble_single_init(void);
void ble_single_deinit(void);
#endif // DEFAULT_BT_LE_PAWR_SUPPORTED || CONFIG_BT_LE_ERROR_SIM_ENABLED

#if DEFAULT_BT_LE_PAWR_SUPPORTED
int pawrBcast_stack_initEnv(void);
void pawrBcast_stack_deinitEnv(void);
int pawrBcast_stack_enable(void);
void pawrBcast_stack_disable(void);

int pawrSync_stack_initEnv(void);
void pawrSync_stack_deinitEnv(void);
int pawrSync_stack_enable(void);
void pawrSync_stack_disable(void);
#endif // DEFAULT_BT_LE_PAWR_SUPPORTED

#if CONFIG_BT_LE_CTRL_ADV_FAST_TX_EN
int advFastTx_stack_initEnv(void);
void advFastTx_stack_deinitEnv(void);
int advFastTx_stack_enable(void);
void advFastTx_stack_disable(void);
#endif // CONFIG_BT_LE_CTRL_ADV_FAST_TX_EN

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

void adv_stack_enableScanReqRxdVsEvent(bool en);
void conn_stack_enableChanMapUpdCompVsEvent(bool en);
void sleep_stack_enableWakeupVsEvent(bool en);
#if DEFAULT_BT_SCAN_ALLOW_ENH_ADI_FILTER
void scan_stack_enableSetScanADIOnlyFilterVsCmd(bool en);
#endif // DEFAULT_BT_SCAN_ALLOW_ENH_ADI_FILTER
#if DEFAULT_BT_ADV_SEND_CONSTANT_DID
void extAdv_stack_setExtAdvConstantDidVsCmd(bool en);
#endif // DEFAULT_BT_ADV_SEND_CONSTANT_DID
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
#if DEFAULT_BT_SCAN_ALLOW_ENH_ADI_FILTER
    scan_stack_enableSetScanADIOnlyFilterVsCmd(en);
#endif // DEFAULT_BT_SCAN_ALLOW_ENH_ADI_FILTER
#if DEFAULT_BT_ADV_SEND_CONSTANT_DID
    extAdv_stack_setExtAdvConstantDidVsCmd(en);
#endif // DEFAULT_BT_ADV_SEND_CONSTANT_DID
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

    rc = adv_stack_initEnv();
    if (rc) {
        return rc;
    }

    rc = extAdv_stack_initEnv();
    if (rc) {
        return rc;
    }

    rc = scan_stack_initEnv();
    if (rc) {
        return rc;
    }

    rc = sync_stack_initEnv();
    if (rc) {
        return rc;
    }

#if CONFIG_BT_LE_DTM_ENABLED
    rc = dtm_stack_initEnv();
    if (rc) {
        return rc;
    }
#endif // CONFIG_BT_LE_DTM_ENABLED

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

#if DEFAULT_BT_LE_PAWR_SUPPORTED || CONFIG_BT_LE_ERROR_SIM_ENABLED
    rc = ble_single_env_init();
    if (rc) {
        return rc;
    }

    rc = ble_single_init();
    if (rc) {
        return rc;
    }
#endif // DEFAULT_BT_LE_PAWR_SUPPORTED || CONFIG_BT_LE_ERROR_SIM_ENABLED

#if DEFAULT_BT_LE_PAWR_SUPPORTED
    rc = pawrBcast_stack_initEnv();
    if (rc) {
        return rc;
    }

    rc = pawrSync_stack_initEnv();
    if (rc) {
        return rc;
    }
#endif // DEFAULT_BT_LE_PAWR_SUPPORTED
#if CONFIG_BT_LE_CTRL_ADV_FAST_TX_EN
    rc = advFastTx_stack_initEnv();
    if (rc) {
        return rc;
    }
#endif // CONFIG_BT_LE_CTRL_ADV_FAST_TX_EN
    return 0;
}

void ble_stack_deinitEnv(void)
{
#if CONFIG_BT_LE_CTRL_ADV_FAST_TX_EN
    advFastTx_stack_deinitEnv();
#endif // CONFIG_BT_LE_CTRL_ADV_FAST_TX_EN
#if DEFAULT_BT_LE_PAWR_SUPPORTED
    pawrSync_stack_deinitEnv();
    pawrBcast_stack_deinitEnv();
#endif // DEFAULT_BT_LE_PAWR_SUPPORTED
#if DEFAULT_BT_LE_PAWR_SUPPORTED || CONFIG_BT_LE_ERROR_SIM_ENABLED
    ble_single_deinit();
    ble_single_env_deinit();
#endif // DEFAULT_BT_LE_PAWR_SUPPORTED || CONFIG_BT_LE_ERROR_SIM_ENABLED

#if DEFAULT_BT_LE_MAX_CONNECTIONS
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_errorSim_deinitEnv();
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_stack_deinitEnv();
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS
#if CONFIG_BT_LE_DTM_ENABLED
    dtm_stack_deinitEnv();
#endif // CONFIG_BT_LE_DTM_ENABLED

    sync_stack_deinitEnv();
    scan_stack_deinitEnv();
    extAdv_stack_deinitEnv();
    adv_stack_deinitEnv();
    base_stack_deinitEnv();
}

int ble_stack_enable(void)
{
    int rc;

    rc = base_stack_enable();
    if (rc) {
        return rc;
    }

    rc = adv_stack_enable();
    if (rc) {
        return rc;
    }

    rc = extAdv_stack_enable();
    if (rc) {
        return rc;
    }

    rc = scan_stack_enable();
    if (rc) {
        return rc;
    }

    rc = sync_stack_enable();
    if (rc) {
        return rc;
    }

#if CONFIG_BT_LE_DTM_ENABLED
    rc = dtm_stack_enable();
    if (rc) {
        return rc;
    }
#endif // CONFIG_BT_LE_DTM_ENABLED

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
#if DEFAULT_BT_LE_PAWR_SUPPORTED
    rc = pawrBcast_stack_enable();
    if (rc) {
        return rc;
    }

    rc = pawrSync_stack_enable();
    if (rc) {
        return rc;
    }
#endif // DEFAULT_BT_LE_PAWR_SUPPORTED
#if CONFIG_BT_LE_CTRL_ADV_FAST_TX_EN
    rc = advFastTx_stack_enable();
    if (rc) {
        return rc;
    }
#endif // CONFIG_BT_LE_CTRL_ADV_FAST_TX_EN
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
#if CONFIG_BT_LE_CTRL_ADV_FAST_TX_EN
    advFastTx_stack_disable();
#endif // CONFIG_BT_LE_CTRL_ADV_FAST_TX_EN
#if DEFAULT_BT_LE_PAWR_SUPPORTED
    pawrSync_stack_disable();
    pawrBcast_stack_disable();
#endif // DEFAULT_BT_LE_PAWR_SUPPORTED
#if DEFAULT_BT_LE_MAX_CONNECTIONS
#if CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_errorSim_disable();
#endif // CONFIG_BT_LE_ERROR_SIM_ENABLED
    conn_stack_disable();
#endif // DEFAULT_BT_LE_MAX_CONNECTIONS
#if CONFIG_BT_LE_DTM_ENABLED
    dtm_stack_disable();
#endif // CONFIG_BT_LE_DTM_ENABLED
    sync_stack_disable();
    scan_stack_disable();
    extAdv_stack_disable();
    adv_stack_disable();
    base_stack_disable();
}
