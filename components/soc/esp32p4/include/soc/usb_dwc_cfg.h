/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
HS Instance:
Configuration Set ID: 11
*/

/* 3.1 Basic Config Parameters */
#define OTG20_MODE                0
#define OTG20_ARCHITECTURE        2
#define OTG20_SINGLE_POINT        1
#define OTG20_ENABLE_LPM          0
#define OTG20_EN_DED_TX_FIFO      1
#define OTG20_EN_DESC_DMA         1
#define OTG20_MULTI_PROC_INTRPT   1

/* 3.2 USB Physical Layer Interface Parameters */
#define OTG20_HSPHY_INTERFACE         3
#define OTG20_HSPHY_DWIDTH            2
#define OTG20_FSPHY_INTERFACE         2
#define OTG20_ENABLE_IC_USB           0
#define OTG20_ENABLE_HSIC             0
#define OTG20_I2C_INTERFACE           0
#define OTG20_ULPI_CARKIT             1
#define OTG20_ADP_SUPPORT             1
#define OTG20_BC_SUPPORT              0
#define OTG20_VENDOR_CTL_INTERFACE    1

/* 3.3 Device Endpoint Configuration Parameters */
#define OTG20_NUM_EPS         15
#define OTG20_NUM_IN_EPS      8
#define OTG20_NUM_CRL_EPS     1

/* 3.4 Host Endpoint Configuration Parameters */
#define OTG20_NUM_HOST_CHAN   16
#define OTG20_EN_PERIO_HOST   1

/* 3.5 Endpoint Channel FIFO Configuration Parameters */
#define OTG20_DFIFO_DEPTH             1024
#define OTG20_DFIFO_DYNAMIC           1
#define OTG20_RX_DFIFO_DEPTH          1024
#define OTG20_TX_HNPERIO_DFIFO_DEPTH  1024
#define OTG20_TX_HPERIO_DFIFO_DEPTH   1024
#define OTG20_NPERIO_TX_QUEUE_DEPTH   4
#define OTG20_PERIO_TX_QUEUE_DEPTH    4

/* 3.6 Additional Configuration Options Parameters */
#define OTG20_TRANS_COUNT_WIDTH       17
#define OTG20_PACKET_COUNT_WIDTH      8
#define OTG20_RM_OPT_FEATURES         1
#define OTG20_EN_PWROPT               1
#define OTG20_SYNC_RESET_TYPE         0
#define OTG20_EN_IDDIG_FILTER         1
#define OTG20_EN_VBUSVALID_FILTER     1
#define OTG20_EN_A_VALID_FILTER       1
#define OTG20_EN_B_VALID_FILTER       1
#define OTG20_EN_SESSIONEND_FILTER    1
#define OTG20_EXCP_CNTL_XFER_FLOW     1
#define OTG20_PWR_CLAMP               0
#define OTG20_PWR_SWITCH_POLARITY     0

/* 3.7 Endpoint Direction Parameters */
#define OTG20_EP_DIR_1    0
#define OTG20_EP_DIR_2    0
#define OTG20_EP_DIR_3    0
#define OTG20_EP_DIR_4    0
#define OTG20_EP_DIR_5    0
#define OTG20_EP_DIR_6    0
#define OTG20_EP_DIR_7    0
#define OTG20_EP_DIR_8    0
#define OTG20_EP_DIR_9    0
#define OTG20_EP_DIR_10   0
#define OTG20_EP_DIR_11   0
#define OTG20_EP_DIR_12   0
#define OTG20_EP_DIR_13   0
#define OTG20_EP_DIR_14   0
#define OTG20_EP_DIR_15   0

/* 3.8 Device Periodic FIFO Depth Parameters */

/* 3.9 Device IN Endpoint FIFO Depth Parameters */
#define OTG20_TX_DINEP_DFIFO_DEPTH_0  512
#define OTG20_TX_DINEP_DFIFO_DEPTH_1  512
#define OTG20_TX_DINEP_DFIFO_DEPTH_2  512
#define OTG20_TX_DINEP_DFIFO_DEPTH_3  512
#define OTG20_TX_DINEP_DFIFO_DEPTH_4  512
#define OTG20_TX_DINEP_DFIFO_DEPTH_5  512
#define OTG20_TX_DINEP_DFIFO_DEPTH_6  512
#define OTG20_TX_DINEP_DFIFO_DEPTH_7  512

/* 3.10 UTMI-To-UTMI Bridge Component Parameters */
#define OTG20_U2UB_EN     0

/*
FS Instance:
Configuration Set ID: 1
*/

/* 3.1 Basic Config Parameters */
#define OTG11_MODE                0
#define OTG11_ARCHITECTURE        2
#define OTG11_SINGLE_POINT        1
#define OTG11_ENABLE_LPM          0
#define OTG11_EN_DED_TX_FIFO      1
#define OTG11_EN_DESC_DMA         1
#define OTG11_MULTI_PROC_INTRPT   0

/* 3.2 USB Physical Layer Interface Parameters */
#define OTG11_HSPHY_INTERFACE     0
#define OTG11_FSPHY_INTERFACE     1
#define OTG11_ENABLE_IC_USB       0
#define OTG11_I2C_INTERFACE       0
#define OTG11_ADP_SUPPORT         0
#define OTG11_BC_SUPPORT          0

/* 3.3 Device Endpoint Configuration Parameters */
#define OTG11_NUM_EPS         6
#define OTG11_NUM_IN_EPS      5
#define OTG11_NUM_CRL_EPS     0

/* 3.4 Host Endpoint Configuration Parameters */
#define OTG11_NUM_HOST_CHAN   8
#define OTG11_EN_PERIO_HOST   1

/* 3.5 Endpoint Channel FIFO Configuration Parameters */
#define OTG11_DFIFO_DEPTH             256
#define OTG11_DFIFO_DYNAMIC           1
#define OTG11_RX_DFIFO_DEPTH          256
#define OTG11_TX_HNPERIO_DFIFO_DEPTH  256
#define OTG11_TX_NPERIO_DFIFO_DEPTH   256
#define OTG11_TX_HPERIO_DFIFO_DEPTH   256
#define OTG11_NPERIO_TX_QUEUE_DEPTH   4
#define OTG11_PERIO_TX_QUEUE_DEPTH    8

/* 3.6 Additional Configuration Options Parameters */
#define OTG11_TRANS_COUNT_WIDTH       16
#define OTG11_PACKET_COUNT_WIDTH      7
#define OTG11_RM_OPT_FEATURES         1
#define OTG11_EN_PWROPT               1
#define OTG11_SYNC_RESET_TYPE         0
#define OTG11_EN_IDDIG_FILTER         1
#define OTG11_EN_VBUSVALID_FILTER     1
#define OTG11_EN_A_VALID_FILTER       1
#define OTG11_EN_B_VALID_FILTER       1
#define OTG11_EN_SESSIONEND_FILTER    1
#define OTG11_EXCP_CNTL_XFER_FLOW     1
#define OTG11_PWR_CLAMP               0
#define OTG11_PWR_SWITCH_POLARITY     0

/* 3.7 Endpoint Direction Parameters */
#define OTG11_EP_DIR_1    0
#define OTG11_EP_DIR_2    0
#define OTG11_EP_DIR_3    0
#define OTG11_EP_DIR_4    0
#define OTG11_EP_DIR_5    0
#define OTG11_EP_DIR_6    0

/* 3.8 Device Periodic FIFO Depth Parameters */

/* 3.9 Device IN Endpoint FIFO Depth Parameters */
#define OTG11_TX_DINEP_DFIFO_DEPTH_1  256
#define OTG11_TX_DINEP_DFIFO_DEPTH_2  256
#define OTG11_TX_DINEP_DFIFO_DEPTH_3  256
#define OTG11_TX_DINEP_DFIFO_DEPTH_4  256

/* 3.10 UTMI-To-UTMI Bridge Component Parameters */
#define OTG11_U2UB_EN     0

#ifdef __cplusplus
}
#endif
