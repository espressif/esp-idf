/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//Commands for PSRAM chip

#pragma once

#include "soc/io_mux_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSRAM_READ                 0x03
#define PSRAM_FAST_READ            0x0B
#define PSRAM_FAST_READ_QUAD       0xEB
#define PSRAM_WRITE                0x02
#define PSRAM_QUAD_WRITE           0x38
#define PSRAM_ENTER_QMODE          0x35
#define PSRAM_EXIT_QMODE           0xF5
#define PSRAM_RESET_EN             0x66
#define PSRAM_RESET                0x99
#define PSRAM_SET_BURST_LEN        0xC0
#define PSRAM_DEVICE_ID            0x9F

#define PSRAM_FAST_READ_DUMMY      4
#define PSRAM_FAST_READ_QUAD_DUMMY 6

// ID
#define PSRAM_ID_KGD_M          0xff
#define PSRAM_ID_KGD_S             8
#define PSRAM_ID_KGD            0x5d
#define PSRAM_ID_EID_M          0xff
#define PSRAM_ID_EID_S            16

// Use the [7:5](bit7~bit5) of EID to distinguish the psram size:
//
//   BIT7  |  BIT6  |  BIT5  |  SIZE(MBIT)
//   -------------------------------------
//    0    |   0    |   0    |     16
//    0    |   0    |   1    |     32
//    0    |   1    |   0    |     64
#define PSRAM_EID_SIZE_M         0x07
#define PSRAM_EID_SIZE_S            5

#define PSRAM_KGD(id)         (((id) >> PSRAM_ID_KGD_S) & PSRAM_ID_KGD_M)
#define PSRAM_EID(id)         (((id) >> PSRAM_ID_EID_S) & PSRAM_ID_EID_M)
#define PSRAM_SIZE_ID(id)     ((PSRAM_EID(id) >> PSRAM_EID_SIZE_S) & PSRAM_EID_SIZE_M)
#define PSRAM_IS_VALID(id)    (PSRAM_KGD(id) == PSRAM_ID_KGD)

#define PSRAM_IS_64MBIT_TRIAL(id) (PSRAM_EID(id) == 0x26)
#define PSRAM_IS_2T_APS3204(id)   ((((id) >> 21) && 0xfffff) == 1)

// IO-pins for PSRAM.
// PSRAM clock and cs IO should be configured based on hardware design.
#define PSRAM_CLK_IO          SPI_CLK_GPIO_NUM
#define PSRAM_CS_IO           SPI_CS1_GPIO_NUM
#define PSRAM_SPIQ_SD0_IO     SPI_Q_GPIO_NUM
#define PSRAM_SPID_SD1_IO     SPI_D_GPIO_NUM
#define PSRAM_SPIWP_SD3_IO    SPI_WP_GPIO_NUM
#define PSRAM_SPIHD_SD2_IO    SPI_HD_GPIO_NUM

#define PSRAM_CMD_LENGTH      8
#define PSRAM_ADDR_LENGTH     24

#define PSRAM_CS_HOLD_VAL      1
#define PSRAM_CS_SETUP_VAL     1

#ifdef __cplusplus
}
#endif
