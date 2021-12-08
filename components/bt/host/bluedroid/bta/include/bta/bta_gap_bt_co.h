/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/******************************************************************************
 *
 *  This is the interface file for BT GAP call-out functions.
 *
 ******************************************************************************/
#ifndef BTA_GAP_BT_CO_H
#define BTA_GAP_BT_CO_H

#if (BTC_GAP_BT_INCLUDED == TRUE)

extern void btc_gap_bt_config_eir_cmpl_callback (uint8_t status, uint8_t eir_type_num, uint8_t *eir_type);

#endif /// (BTC_GAP_BT_INCLUDED == TRUE)
#endif /// BTA_GAP_BT_CO_H
