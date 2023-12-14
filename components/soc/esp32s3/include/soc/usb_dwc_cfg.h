/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
Configuration Set ID: 1
*/

/* 3.1 Basic Config Parameters */
#define OTG_MODE                0
#define OTG_ARCHITECTURE        2
#define OTG_SINGLE_POINT        1
#define OTG_ENABLE_LPM          0
#define OTG_EN_DED_TX_FIFO      1
#define OTG_EN_DESC_DMA         1
#define OTG_MULTI_PROC_INTRPT   0

/* 3.2 USB Physical Layer Interface Parameters */
#define OTG_HSPHY_INTERFACE     0
#define OTG_FSPHY_INTERFACE     1
#define OTG_ENABLE_IC_USB       0
#define OTG_I2C_INTERFACE       0
#define OTG_ADP_SUPPORT         0
#define OTG_BC_SUPPORT          0

/* 3.3 Device Endpoint Configuration Parameters */
#define OTG_NUM_EPS         6
#define OTG_NUM_IN_EPS      5
#define OTG_NUM_CRL_EPS     0

/* 3.4 Host Endpoint Configuration Parameters */
#define OTG_NUM_HOST_CHAN   8
#define OTG_EN_PERIO_HOST   1

/* 3.5 Endpoint Channel FIFO Configuration Parameters */
#define OTG_DFIFO_DEPTH             256
#define OTG_DFIFO_DYNAMIC           1
#define OTG_RX_DFIFO_DEPTH          256
#define OTG_TX_HNPERIO_DFIFO_DEPTH  256
#define OTG_TX_NPERIO_DFIFO_DEPTH   256
#define OTG_TX_HPERIO_DFIFO_DEPTH   256
#define OTG_NPERIO_TX_QUEUE_DEPTH   4
#define OTG_PERIO_TX_QUEUE_DEPTH    8

/* 3.6 Additional Configuration Options Parameters */
#define OTG_TRANS_COUNT_WIDTH       16
#define OTG_PACKET_COUNT_WIDTH      7
#define OTG_RM_OPT_FEATURES         1
#define OTG_EN_PWROPT               1
#define OTG_SYNC_RESET_TYPE         0
#define OTG_EN_IDDIG_FILTER         1
#define OTG_EN_VBUSVALID_FILTER     1
#define OTG_EN_A_VALID_FILTER       1
#define OTG_EN_B_VALID_FILTER       1
#define OTG_EN_SESSIONEND_FILTER    1
#define OTG_EXCP_CNTL_XFER_FLOW     1
#define OTG_PWR_CLAMP               0
#define OTG_PWR_SWITCH_POLARITY     0

/* 3.7 Endpoint Direction Parameters */
#define OTG_EP_DIR_1    0
#define OTG_EP_DIR_2    0
#define OTG_EP_DIR_3    0
#define OTG_EP_DIR_4    0
#define OTG_EP_DIR_5    0
#define OTG_EP_DIR_6    0

/* 3.8 Device Periodic FIFO Depth Parameters */

/* 3.9 Device IN Endpoint FIFO Depth Parameters */
#define OTG_TX_DINEP_DFIFO_DEPTH_1  256
#define OTG_TX_DINEP_DFIFO_DEPTH_2  256
#define OTG_TX_DINEP_DFIFO_DEPTH_3  256
#define OTG_TX_DINEP_DFIFO_DEPTH_4  256

/* 3.10 UTMI-To-UTMI Bridge Component Parameters */
#define U2UB_EN     0

#ifdef __cplusplus
}
#endif
