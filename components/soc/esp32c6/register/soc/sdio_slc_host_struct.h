/**
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: ********Registers */
/** Type of func2_0 register
 *  *******Description***********
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** slc_func2_int : R/W; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t slc_func2_int:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} slchost_func2_0_reg_t;

/** Type of func2_1 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc_func2_int_en : R/W; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc_func2_int_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} slchost_func2_1_reg_t;

/** Type of func2_2 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc_func1_mdstat : R/W; bitpos: [0]; default: 1;
         *  *******Description***********
         */
        uint32_t slc_func1_mdstat:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} slchost_func2_2_reg_t;

/** Type of gpio_status0 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** gpio_sdio_int0 : RO; bitpos: [31:0]; default: 0;
         *  *******Description***********
         */
        uint32_t gpio_sdio_int0:32;
    };
    uint32_t val;
} slchost_gpio_status0_reg_t;

/** Type of gpio_status1 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** gpio_sdio_int1 : RO; bitpos: [31:0]; default: 0;
         *  *******Description***********
         */
        uint32_t gpio_sdio_int1:32;
    };
    uint32_t val;
} slchost_gpio_status1_reg_t;

/** Type of gpio_in0 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** gpio_sdio_in0 : RO; bitpos: [31:0]; default: 0;
         *  *******Description***********
         */
        uint32_t gpio_sdio_in0:32;
    };
    uint32_t val;
} slchost_gpio_in0_reg_t;

/** Type of gpio_in1 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** gpio_sdio_in1 : RO; bitpos: [31:0]; default: 0;
         *  *******Description***********
         */
        uint32_t gpio_sdio_in1:32;
    };
    uint32_t val;
} slchost_gpio_in1_reg_t;

/** Type of slc0host_token_rdata register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc0_token0 : RO; bitpos: [11:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token0:12;
        /** slc0_rx_pf_valid : RO; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_pf_valid:1;
        uint32_t reserved_13:3;
        /** hostslchost_slc0_token1 : RO; bitpos: [27:16]; default: 0;
         *  *******Description***********
         */
        uint32_t hostslchost_slc0_token1:12;
        /** slc0_rx_pf_eof : RO; bitpos: [31:28]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_pf_eof:4;
    };
    uint32_t val;
} slchost_slc0host_token_rdata_reg_t;

/** Type of slc0_host_pf register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc0_pf_data : RO; bitpos: [31:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_pf_data:32;
    };
    uint32_t val;
} slchost_slc0_host_pf_reg_t;

/** Type of slc1_host_pf register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc1_pf_data : RO; bitpos: [31:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_pf_data:32;
    };
    uint32_t val;
} slchost_slc1_host_pf_reg_t;

/** Type of slc0host_int_raw register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc0_tohost_bit0_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit0_int_raw:1;
        /** slc0_tohost_bit1_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit1_int_raw:1;
        /** slc0_tohost_bit2_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit2_int_raw:1;
        /** slc0_tohost_bit3_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit3_int_raw:1;
        /** slc0_tohost_bit4_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit4_int_raw:1;
        /** slc0_tohost_bit5_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit5_int_raw:1;
        /** slc0_tohost_bit6_int_raw : R/WTC/SS/SC; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit6_int_raw:1;
        /** slc0_tohost_bit7_int_raw : R/WTC/SS/SC; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit7_int_raw:1;
        /** slc0_token0_1to0_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token0_1to0_int_raw:1;
        /** slc0_token1_1to0_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token1_1to0_int_raw:1;
        /** slc0_token0_0to1_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token0_0to1_int_raw:1;
        /** slc0_token1_0to1_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token1_0to1_int_raw:1;
        /** slc0host_rx_sof_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_sof_int_raw:1;
        /** slc0host_rx_eof_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_eof_int_raw:1;
        /** slc0host_rx_start_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_start_int_raw:1;
        /** slc0host_tx_start_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_tx_start_int_raw:1;
        /** slc0_rx_udf_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_udf_int_raw:1;
        /** slc0_tx_ovf_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tx_ovf_int_raw:1;
        /** slc0_rx_pf_valid_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_pf_valid_int_raw:1;
        /** slc0_ext_bit0_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit0_int_raw:1;
        /** slc0_ext_bit1_int_raw : R/WTC/SS; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit1_int_raw:1;
        /** slc0_ext_bit2_int_raw : R/WTC/SS; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit2_int_raw:1;
        /** slc0_ext_bit3_int_raw : R/WTC/SS; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit3_int_raw:1;
        /** slc0_rx_new_packet_int_raw : R/WTC/SS; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_new_packet_int_raw:1;
        /** slc0_host_rd_retry_int_raw : R/WTC/SS; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_host_rd_retry_int_raw:1;
        /** gpio_sdio_int_raw : R/WTC/SS/SC; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t gpio_sdio_int_raw:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc0host_int_raw_reg_t;

/** Type of slc1host_int_raw register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc1_tohost_bit0_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit0_int_raw:1;
        /** slc1_tohost_bit1_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit1_int_raw:1;
        /** slc1_tohost_bit2_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit2_int_raw:1;
        /** slc1_tohost_bit3_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit3_int_raw:1;
        /** slc1_tohost_bit4_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit4_int_raw:1;
        /** slc1_tohost_bit5_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit5_int_raw:1;
        /** slc1_tohost_bit6_int_raw : R/WTC/SS/SC; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit6_int_raw:1;
        /** slc1_tohost_bit7_int_raw : R/WTC/SS/SC; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit7_int_raw:1;
        /** slc1_token0_1to0_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token0_1to0_int_raw:1;
        /** slc1_token1_1to0_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token1_1to0_int_raw:1;
        /** slc1_token0_0to1_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token0_0to1_int_raw:1;
        /** slc1_token1_0to1_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token1_0to1_int_raw:1;
        /** slc1host_rx_sof_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_sof_int_raw:1;
        /** slc1host_rx_eof_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_eof_int_raw:1;
        /** slc1host_rx_start_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_start_int_raw:1;
        /** slc1host_tx_start_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_tx_start_int_raw:1;
        /** slc1_rx_udf_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_rx_udf_int_raw:1;
        /** slc1_tx_ovf_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tx_ovf_int_raw:1;
        /** slc1_rx_pf_valid_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_rx_pf_valid_int_raw:1;
        /** slc1_ext_bit0_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit0_int_raw:1;
        /** slc1_ext_bit1_int_raw : R/WTC/SS; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit1_int_raw:1;
        /** slc1_ext_bit2_int_raw : R/WTC/SS; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit2_int_raw:1;
        /** slc1_ext_bit3_int_raw : R/WTC/SS; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit3_int_raw:1;
        /** slc1_wifi_rx_new_packet_int_raw : R/WTC/SS; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_wifi_rx_new_packet_int_raw:1;
        /** slc1_host_rd_retry_int_raw : R/WTC/SS; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_host_rd_retry_int_raw:1;
        /** slc1_bt_rx_new_packet_int_raw : R/WTC/SS; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_bt_rx_new_packet_int_raw:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc1host_int_raw_reg_t;

/** Type of slc0host_int_st register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc0_tohost_bit0_int_st : RO; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit0_int_st:1;
        /** slc0_tohost_bit1_int_st : RO; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit1_int_st:1;
        /** slc0_tohost_bit2_int_st : RO; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit2_int_st:1;
        /** slc0_tohost_bit3_int_st : RO; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit3_int_st:1;
        /** slc0_tohost_bit4_int_st : RO; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit4_int_st:1;
        /** slc0_tohost_bit5_int_st : RO; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit5_int_st:1;
        /** slc0_tohost_bit6_int_st : RO; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit6_int_st:1;
        /** slc0_tohost_bit7_int_st : RO; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit7_int_st:1;
        /** slc0_token0_1to0_int_st : RO; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token0_1to0_int_st:1;
        /** slc0_token1_1to0_int_st : RO; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token1_1to0_int_st:1;
        /** slc0_token0_0to1_int_st : RO; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token0_0to1_int_st:1;
        /** slc0_token1_0to1_int_st : RO; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token1_0to1_int_st:1;
        /** slc0host_rx_sof_int_st : RO; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_sof_int_st:1;
        /** slc0host_rx_eof_int_st : RO; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_eof_int_st:1;
        /** slc0host_rx_start_int_st : RO; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_start_int_st:1;
        /** slc0host_tx_start_int_st : RO; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_tx_start_int_st:1;
        /** slc0_rx_udf_int_st : RO; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_udf_int_st:1;
        /** slc0_tx_ovf_int_st : RO; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tx_ovf_int_st:1;
        /** slc0_rx_pf_valid_int_st : RO; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_pf_valid_int_st:1;
        /** slc0_ext_bit0_int_st : RO; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit0_int_st:1;
        /** slc0_ext_bit1_int_st : RO; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit1_int_st:1;
        /** slc0_ext_bit2_int_st : RO; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit2_int_st:1;
        /** slc0_ext_bit3_int_st : RO; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit3_int_st:1;
        /** slc0_rx_new_packet_int_st : RO; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_new_packet_int_st:1;
        /** slc0_host_rd_retry_int_st : RO; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_host_rd_retry_int_st:1;
        /** gpio_sdio_int_st : RO; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t gpio_sdio_int_st:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc0host_int_st_reg_t;

/** Type of slc1host_int_st register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc1_tohost_bit0_int_st : RO; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit0_int_st:1;
        /** slc1_tohost_bit1_int_st : RO; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit1_int_st:1;
        /** slc1_tohost_bit2_int_st : RO; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit2_int_st:1;
        /** slc1_tohost_bit3_int_st : RO; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit3_int_st:1;
        /** slc1_tohost_bit4_int_st : RO; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit4_int_st:1;
        /** slc1_tohost_bit5_int_st : RO; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit5_int_st:1;
        /** slc1_tohost_bit6_int_st : RO; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit6_int_st:1;
        /** slc1_tohost_bit7_int_st : RO; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit7_int_st:1;
        /** slc1_token0_1to0_int_st : RO; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token0_1to0_int_st:1;
        /** slc1_token1_1to0_int_st : RO; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token1_1to0_int_st:1;
        /** slc1_token0_0to1_int_st : RO; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token0_0to1_int_st:1;
        /** slc1_token1_0to1_int_st : RO; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token1_0to1_int_st:1;
        /** slc1host_rx_sof_int_st : RO; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_sof_int_st:1;
        /** slc1host_rx_eof_int_st : RO; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_eof_int_st:1;
        /** slc1host_rx_start_int_st : RO; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_start_int_st:1;
        /** slc1host_tx_start_int_st : RO; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_tx_start_int_st:1;
        /** slc1_rx_udf_int_st : RO; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_rx_udf_int_st:1;
        /** slc1_tx_ovf_int_st : RO; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tx_ovf_int_st:1;
        /** slc1_rx_pf_valid_int_st : RO; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_rx_pf_valid_int_st:1;
        /** slc1_ext_bit0_int_st : RO; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit0_int_st:1;
        /** slc1_ext_bit1_int_st : RO; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit1_int_st:1;
        /** slc1_ext_bit2_int_st : RO; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit2_int_st:1;
        /** slc1_ext_bit3_int_st : RO; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit3_int_st:1;
        /** slc1_wifi_rx_new_packet_int_st : RO; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_wifi_rx_new_packet_int_st:1;
        /** slc1_host_rd_retry_int_st : RO; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_host_rd_retry_int_st:1;
        /** slc1_bt_rx_new_packet_int_st : RO; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_bt_rx_new_packet_int_st:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc1host_int_st_reg_t;

/** Type of pkt_len register
 *  *******Description***********
 */
typedef union {
    struct {
        /** hostslchost_slc0_len : RO; bitpos: [19:0]; default: 0;
         *  *******Description***********
         */
        uint32_t hostslchost_slc0_len:20;
        /** hostslchost_slc0_len_check : RO; bitpos: [31:20]; default: 0;
         *  *******Description***********
         */
        uint32_t hostslchost_slc0_len_check:12;
    };
    uint32_t val;
} slchost_pkt_len_reg_t;

/** Type of state_w0 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_state0 : RO; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_state0:8;
        /** slchost_state1 : RO; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_state1:8;
        /** slchost_state2 : RO; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_state2:8;
        /** slchost_state3 : RO; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_state3:8;
    };
    uint32_t val;
} slchost_state_w0_reg_t;

/** Type of state_w1 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_state4 : RO; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_state4:8;
        /** slchost_state5 : RO; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_state5:8;
        /** slchost_state6 : RO; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_state6:8;
        /** slchost_state7 : RO; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_state7:8;
    };
    uint32_t val;
} slchost_state_w1_reg_t;

/** Type of conf_w0 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf0 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf0:8;
        /** slchost_conf1 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf1:8;
        /** slchost_conf2 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf2:8;
        /** slchost_conf3 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf3:8;
    };
    uint32_t val;
} slchost_conf_w0_reg_t;

/** Type of conf_w1 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf4 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf4:8;
        /** slchost_conf5 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf5:8;
        /** slchost_conf6 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf6:8;
        /** slchost_conf7 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf7:8;
    };
    uint32_t val;
} slchost_conf_w1_reg_t;

/** Type of conf_w2 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf8 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf8:8;
        /** slchost_conf9 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf9:8;
        /** slchost_conf10 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf10:8;
        /** slchost_conf11 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf11:8;
    };
    uint32_t val;
} slchost_conf_w2_reg_t;

/** Type of conf_w3 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf12 : R/W; bitpos: [7:0]; default: 192;
         *  *******Description***********
         */
        uint32_t slchost_conf12:8;
        /** slchost_conf13 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf13:8;
        /** slchost_conf14 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf14:8;
        /** slchost_conf15 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf15:8;
    };
    uint32_t val;
} slchost_conf_w3_reg_t;

/** Type of conf_w4 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf16 : R/W; bitpos: [7:0]; default: 255;
         *  *******Description***********
         */
        uint32_t slchost_conf16:8;
        /** slchost_conf17 : R/W; bitpos: [15:8]; default: 1;
         *  *******Description***********
         */
        uint32_t slchost_conf17:8;
        /** slchost_conf18 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf18:8;
        /** slchost_conf19 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf19:8;
    };
    uint32_t val;
} slchost_conf_w4_reg_t;

/** Type of conf_w5 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf20 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf20:8;
        /** slchost_conf21 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf21:8;
        /** slchost_conf22 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf22:8;
        /** slchost_conf23 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf23:8;
    };
    uint32_t val;
} slchost_conf_w5_reg_t;

/** Type of win_cmd register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_win_cmd : R/W; bitpos: [15:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_win_cmd:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} slchost_win_cmd_reg_t;

/** Type of conf_w6 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf24 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf24:8;
        /** slchost_conf25 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf25:8;
        /** slchost_conf26 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf26:8;
        /** slchost_conf27 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf27:8;
    };
    uint32_t val;
} slchost_conf_w6_reg_t;

/** Type of conf_w7 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf28 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf28:8;
        /** slchost_conf29 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf29:8;
        /** slchost_conf30 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf30:8;
        /** slchost_conf31 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf31:8;
    };
    uint32_t val;
} slchost_conf_w7_reg_t;

/** Type of pkt_len0 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** hostslchost_slc0_len0 : RO; bitpos: [19:0]; default: 0;
         *  *******Description***********
         */
        uint32_t hostslchost_slc0_len0:20;
        /** hostslchost_slc0_len0_check : RO; bitpos: [31:20]; default: 0;
         *  *******Description***********
         */
        uint32_t hostslchost_slc0_len0_check:12;
    };
    uint32_t val;
} slchost_pkt_len0_reg_t;

/** Type of pkt_len1 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** hostslchost_slc0_len1 : RO; bitpos: [19:0]; default: 0;
         *  *******Description***********
         */
        uint32_t hostslchost_slc0_len1:20;
        /** hostslchost_slc0_len1_check : RO; bitpos: [31:20]; default: 0;
         *  *******Description***********
         */
        uint32_t hostslchost_slc0_len1_check:12;
    };
    uint32_t val;
} slchost_pkt_len1_reg_t;

/** Type of pkt_len2 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** hostslchost_slc0_len2 : RO; bitpos: [19:0]; default: 0;
         *  *******Description***********
         */
        uint32_t hostslchost_slc0_len2:20;
        /** hostslchost_slc0_len2_check : RO; bitpos: [31:20]; default: 0;
         *  *******Description***********
         */
        uint32_t hostslchost_slc0_len2_check:12;
    };
    uint32_t val;
} slchost_pkt_len2_reg_t;

/** Type of conf_w8 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf32 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf32:8;
        /** slchost_conf33 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf33:8;
        /** slchost_conf34 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf34:8;
        /** slchost_conf35 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf35:8;
    };
    uint32_t val;
} slchost_conf_w8_reg_t;

/** Type of conf_w9 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf36 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf36:8;
        /** slchost_conf37 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf37:8;
        /** slchost_conf38 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf38:8;
        /** slchost_conf39 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf39:8;
    };
    uint32_t val;
} slchost_conf_w9_reg_t;

/** Type of conf_w10 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf40 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf40:8;
        /** slchost_conf41 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf41:8;
        /** slchost_conf42 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf42:8;
        /** slchost_conf43 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf43:8;
    };
    uint32_t val;
} slchost_conf_w10_reg_t;

/** Type of conf_w11 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf44 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf44:8;
        /** slchost_conf45 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf45:8;
        /** slchost_conf46 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf46:8;
        /** slchost_conf47 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf47:8;
    };
    uint32_t val;
} slchost_conf_w11_reg_t;

/** Type of conf_w12 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf48 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf48:8;
        /** slchost_conf49 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf49:8;
        /** slchost_conf50 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf50:8;
        /** slchost_conf51 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf51:8;
    };
    uint32_t val;
} slchost_conf_w12_reg_t;

/** Type of conf_w13 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf52 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf52:8;
        /** slchost_conf53 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf53:8;
        /** slchost_conf54 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf54:8;
        /** slchost_conf55 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf55:8;
    };
    uint32_t val;
} slchost_conf_w13_reg_t;

/** Type of conf_w14 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf56 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf56:8;
        /** slchost_conf57 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf57:8;
        /** slchost_conf58 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf58:8;
        /** slchost_conf59 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf59:8;
    };
    uint32_t val;
} slchost_conf_w14_reg_t;

/** Type of conf_w15 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_conf60 : R/W; bitpos: [7:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf60:8;
        /** slchost_conf61 : R/W; bitpos: [15:8]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf61:8;
        /** slchost_conf62 : R/W; bitpos: [23:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf62:8;
        /** slchost_conf63 : R/W; bitpos: [31:24]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_conf63:8;
    };
    uint32_t val;
} slchost_conf_w15_reg_t;

/** Type of check_sum0 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_check_sum0 : RO; bitpos: [31:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slchost_check_sum0:32;
    };
    uint32_t val;
} slchost_check_sum0_reg_t;

/** Type of check_sum1 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_check_sum1 : RO; bitpos: [31:0]; default: 319;
         *  *******Description***********
         */
        uint32_t slchost_check_sum1:32;
    };
    uint32_t val;
} slchost_check_sum1_reg_t;

/** Type of slc1host_token_rdata register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc1_token0 : RO; bitpos: [11:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token0:12;
        /** slc1_rx_pf_valid : RO; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_rx_pf_valid:1;
        uint32_t reserved_13:3;
        /** hostslchost_slc1_token1 : RO; bitpos: [27:16]; default: 0;
         *  *******Description***********
         */
        uint32_t hostslchost_slc1_token1:12;
        /** slc1_rx_pf_eof : RO; bitpos: [31:28]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_rx_pf_eof:4;
    };
    uint32_t val;
} slchost_slc1host_token_rdata_reg_t;

/** Type of slc0host_token_wdata register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc0host_token0_wd : R/W; bitpos: [11:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_token0_wd:12;
        uint32_t reserved_12:4;
        /** slc0host_token1_wd : R/W; bitpos: [27:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_token1_wd:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} slchost_slc0host_token_wdata_reg_t;

/** Type of slc1host_token_wdata register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc1host_token0_wd : R/W; bitpos: [11:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_token0_wd:12;
        uint32_t reserved_12:4;
        /** slc1host_token1_wd : R/W; bitpos: [27:16]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_token1_wd:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} slchost_slc1host_token_wdata_reg_t;

/** Type of token_con register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc0host_token0_dec : WT; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_token0_dec:1;
        /** slc0host_token1_dec : WT; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_token1_dec:1;
        /** slc0host_token0_wr : WT; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_token0_wr:1;
        /** slc0host_token1_wr : WT; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_token1_wr:1;
        /** slc1host_token0_dec : WT; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_token0_dec:1;
        /** slc1host_token1_dec : WT; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_token1_dec:1;
        /** slc1host_token0_wr : WT; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_token0_wr:1;
        /** slc1host_token1_wr : WT; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_token1_wr:1;
        /** slc0host_len_wr : WT; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_len_wr:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} slchost_token_con_reg_t;

/** Type of slc0host_int_clr register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc0_tohost_bit0_int_clr : WT; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit0_int_clr:1;
        /** slc0_tohost_bit1_int_clr : WT; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit1_int_clr:1;
        /** slc0_tohost_bit2_int_clr : WT; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit2_int_clr:1;
        /** slc0_tohost_bit3_int_clr : WT; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit3_int_clr:1;
        /** slc0_tohost_bit4_int_clr : WT; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit4_int_clr:1;
        /** slc0_tohost_bit5_int_clr : WT; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit5_int_clr:1;
        /** slc0_tohost_bit6_int_clr : WT; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit6_int_clr:1;
        /** slc0_tohost_bit7_int_clr : WT; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit7_int_clr:1;
        /** slc0_token0_1to0_int_clr : WT; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token0_1to0_int_clr:1;
        /** slc0_token1_1to0_int_clr : WT; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token1_1to0_int_clr:1;
        /** slc0_token0_0to1_int_clr : WT; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token0_0to1_int_clr:1;
        /** slc0_token1_0to1_int_clr : WT; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token1_0to1_int_clr:1;
        /** slc0host_rx_sof_int_clr : WT; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_sof_int_clr:1;
        /** slc0host_rx_eof_int_clr : WT; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_eof_int_clr:1;
        /** slc0host_rx_start_int_clr : WT; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_start_int_clr:1;
        /** slc0host_tx_start_int_clr : WT; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_tx_start_int_clr:1;
        /** slc0_rx_udf_int_clr : WT; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_udf_int_clr:1;
        /** slc0_tx_ovf_int_clr : WT; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tx_ovf_int_clr:1;
        /** slc0_rx_pf_valid_int_clr : WT; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_pf_valid_int_clr:1;
        /** slc0_ext_bit0_int_clr : WT; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit0_int_clr:1;
        /** slc0_ext_bit1_int_clr : WT; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit1_int_clr:1;
        /** slc0_ext_bit2_int_clr : WT; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit2_int_clr:1;
        /** slc0_ext_bit3_int_clr : WT; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit3_int_clr:1;
        /** slc0_rx_new_packet_int_clr : WT; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_new_packet_int_clr:1;
        /** slc0_host_rd_retry_int_clr : WT; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_host_rd_retry_int_clr:1;
        /** gpio_sdio_int_clr : WT; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t gpio_sdio_int_clr:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc0host_int_clr_reg_t;

/** Type of slc1host_int_clr register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc1_tohost_bit0_int_clr : WT; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit0_int_clr:1;
        /** slc1_tohost_bit1_int_clr : WT; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit1_int_clr:1;
        /** slc1_tohost_bit2_int_clr : WT; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit2_int_clr:1;
        /** slc1_tohost_bit3_int_clr : WT; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit3_int_clr:1;
        /** slc1_tohost_bit4_int_clr : WT; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit4_int_clr:1;
        /** slc1_tohost_bit5_int_clr : WT; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit5_int_clr:1;
        /** slc1_tohost_bit6_int_clr : WT; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit6_int_clr:1;
        /** slc1_tohost_bit7_int_clr : WT; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit7_int_clr:1;
        /** slc1_token0_1to0_int_clr : WT; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token0_1to0_int_clr:1;
        /** slc1_token1_1to0_int_clr : WT; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token1_1to0_int_clr:1;
        /** slc1_token0_0to1_int_clr : WT; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token0_0to1_int_clr:1;
        /** slc1_token1_0to1_int_clr : WT; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token1_0to1_int_clr:1;
        /** slc1host_rx_sof_int_clr : WT; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_sof_int_clr:1;
        /** slc1host_rx_eof_int_clr : WT; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_eof_int_clr:1;
        /** slc1host_rx_start_int_clr : WT; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_start_int_clr:1;
        /** slc1host_tx_start_int_clr : WT; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_tx_start_int_clr:1;
        /** slc1_rx_udf_int_clr : WT; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_rx_udf_int_clr:1;
        /** slc1_tx_ovf_int_clr : WT; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tx_ovf_int_clr:1;
        /** slc1_rx_pf_valid_int_clr : WT; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_rx_pf_valid_int_clr:1;
        /** slc1_ext_bit0_int_clr : WT; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit0_int_clr:1;
        /** slc1_ext_bit1_int_clr : WT; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit1_int_clr:1;
        /** slc1_ext_bit2_int_clr : WT; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit2_int_clr:1;
        /** slc1_ext_bit3_int_clr : WT; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit3_int_clr:1;
        /** slc1_wifi_rx_new_packet_int_clr : WT; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_wifi_rx_new_packet_int_clr:1;
        /** slc1_host_rd_retry_int_clr : WT; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_host_rd_retry_int_clr:1;
        /** slc1_bt_rx_new_packet_int_clr : WT; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_bt_rx_new_packet_int_clr:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc1host_int_clr_reg_t;

/** Type of slc0host_func1_int_ena register
 *  *******Description***********
 */
typedef union {
    struct {
        /** fn1_slc0_tohost_bit0_int_ena : R/W; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_tohost_bit0_int_ena:1;
        /** fn1_slc0_tohost_bit1_int_ena : R/W; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_tohost_bit1_int_ena:1;
        /** fn1_slc0_tohost_bit2_int_ena : R/W; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_tohost_bit2_int_ena:1;
        /** fn1_slc0_tohost_bit3_int_ena : R/W; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_tohost_bit3_int_ena:1;
        /** fn1_slc0_tohost_bit4_int_ena : R/W; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_tohost_bit4_int_ena:1;
        /** fn1_slc0_tohost_bit5_int_ena : R/W; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_tohost_bit5_int_ena:1;
        /** fn1_slc0_tohost_bit6_int_ena : R/W; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_tohost_bit6_int_ena:1;
        /** fn1_slc0_tohost_bit7_int_ena : R/W; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_tohost_bit7_int_ena:1;
        /** fn1_slc0_token0_1to0_int_ena : R/W; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_token0_1to0_int_ena:1;
        /** fn1_slc0_token1_1to0_int_ena : R/W; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_token1_1to0_int_ena:1;
        /** fn1_slc0_token0_0to1_int_ena : R/W; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_token0_0to1_int_ena:1;
        /** fn1_slc0_token1_0to1_int_ena : R/W; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_token1_0to1_int_ena:1;
        /** fn1_slc0host_rx_sof_int_ena : R/W; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0host_rx_sof_int_ena:1;
        /** fn1_slc0host_rx_eof_int_ena : R/W; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0host_rx_eof_int_ena:1;
        /** fn1_slc0host_rx_start_int_ena : R/W; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0host_rx_start_int_ena:1;
        /** fn1_slc0host_tx_start_int_ena : R/W; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0host_tx_start_int_ena:1;
        /** fn1_slc0_rx_udf_int_ena : R/W; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_rx_udf_int_ena:1;
        /** fn1_slc0_tx_ovf_int_ena : R/W; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_tx_ovf_int_ena:1;
        /** fn1_slc0_rx_pf_valid_int_ena : R/W; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_rx_pf_valid_int_ena:1;
        /** fn1_slc0_ext_bit0_int_ena : R/W; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_ext_bit0_int_ena:1;
        /** fn1_slc0_ext_bit1_int_ena : R/W; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_ext_bit1_int_ena:1;
        /** fn1_slc0_ext_bit2_int_ena : R/W; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_ext_bit2_int_ena:1;
        /** fn1_slc0_ext_bit3_int_ena : R/W; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_ext_bit3_int_ena:1;
        /** fn1_slc0_rx_new_packet_int_ena : R/W; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_rx_new_packet_int_ena:1;
        /** fn1_slc0_host_rd_retry_int_ena : R/W; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc0_host_rd_retry_int_ena:1;
        /** fn1_gpio_sdio_int_ena : R/W; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_gpio_sdio_int_ena:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc0host_func1_int_ena_reg_t;

/** Type of slc1host_func1_int_ena register
 *  *******Description***********
 */
typedef union {
    struct {
        /** fn1_slc1_tohost_bit0_int_ena : R/W; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_tohost_bit0_int_ena:1;
        /** fn1_slc1_tohost_bit1_int_ena : R/W; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_tohost_bit1_int_ena:1;
        /** fn1_slc1_tohost_bit2_int_ena : R/W; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_tohost_bit2_int_ena:1;
        /** fn1_slc1_tohost_bit3_int_ena : R/W; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_tohost_bit3_int_ena:1;
        /** fn1_slc1_tohost_bit4_int_ena : R/W; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_tohost_bit4_int_ena:1;
        /** fn1_slc1_tohost_bit5_int_ena : R/W; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_tohost_bit5_int_ena:1;
        /** fn1_slc1_tohost_bit6_int_ena : R/W; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_tohost_bit6_int_ena:1;
        /** fn1_slc1_tohost_bit7_int_ena : R/W; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_tohost_bit7_int_ena:1;
        /** fn1_slc1_token0_1to0_int_ena : R/W; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_token0_1to0_int_ena:1;
        /** fn1_slc1_token1_1to0_int_ena : R/W; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_token1_1to0_int_ena:1;
        /** fn1_slc1_token0_0to1_int_ena : R/W; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_token0_0to1_int_ena:1;
        /** fn1_slc1_token1_0to1_int_ena : R/W; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_token1_0to1_int_ena:1;
        /** fn1_slc1host_rx_sof_int_ena : R/W; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1host_rx_sof_int_ena:1;
        /** fn1_slc1host_rx_eof_int_ena : R/W; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1host_rx_eof_int_ena:1;
        /** fn1_slc1host_rx_start_int_ena : R/W; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1host_rx_start_int_ena:1;
        /** fn1_slc1host_tx_start_int_ena : R/W; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1host_tx_start_int_ena:1;
        /** fn1_slc1_rx_udf_int_ena : R/W; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_rx_udf_int_ena:1;
        /** fn1_slc1_tx_ovf_int_ena : R/W; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_tx_ovf_int_ena:1;
        /** fn1_slc1_rx_pf_valid_int_ena : R/W; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_rx_pf_valid_int_ena:1;
        /** fn1_slc1_ext_bit0_int_ena : R/W; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_ext_bit0_int_ena:1;
        /** fn1_slc1_ext_bit1_int_ena : R/W; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_ext_bit1_int_ena:1;
        /** fn1_slc1_ext_bit2_int_ena : R/W; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_ext_bit2_int_ena:1;
        /** fn1_slc1_ext_bit3_int_ena : R/W; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_ext_bit3_int_ena:1;
        /** fn1_slc1_wifi_rx_new_packet_int_ena : R/W; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_wifi_rx_new_packet_int_ena:1;
        /** fn1_slc1_host_rd_retry_int_ena : R/W; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_host_rd_retry_int_ena:1;
        /** fn1_slc1_bt_rx_new_packet_int_ena : R/W; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t fn1_slc1_bt_rx_new_packet_int_ena:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc1host_func1_int_ena_reg_t;

/** Type of slc0host_func2_int_ena register
 *  *******Description***********
 */
typedef union {
    struct {
        /** fn2_slc0_tohost_bit0_int_ena : R/W; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_tohost_bit0_int_ena:1;
        /** fn2_slc0_tohost_bit1_int_ena : R/W; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_tohost_bit1_int_ena:1;
        /** fn2_slc0_tohost_bit2_int_ena : R/W; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_tohost_bit2_int_ena:1;
        /** fn2_slc0_tohost_bit3_int_ena : R/W; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_tohost_bit3_int_ena:1;
        /** fn2_slc0_tohost_bit4_int_ena : R/W; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_tohost_bit4_int_ena:1;
        /** fn2_slc0_tohost_bit5_int_ena : R/W; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_tohost_bit5_int_ena:1;
        /** fn2_slc0_tohost_bit6_int_ena : R/W; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_tohost_bit6_int_ena:1;
        /** fn2_slc0_tohost_bit7_int_ena : R/W; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_tohost_bit7_int_ena:1;
        /** fn2_slc0_token0_1to0_int_ena : R/W; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_token0_1to0_int_ena:1;
        /** fn2_slc0_token1_1to0_int_ena : R/W; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_token1_1to0_int_ena:1;
        /** fn2_slc0_token0_0to1_int_ena : R/W; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_token0_0to1_int_ena:1;
        /** fn2_slc0_token1_0to1_int_ena : R/W; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_token1_0to1_int_ena:1;
        /** fn2_slc0host_rx_sof_int_ena : R/W; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0host_rx_sof_int_ena:1;
        /** fn2_slc0host_rx_eof_int_ena : R/W; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0host_rx_eof_int_ena:1;
        /** fn2_slc0host_rx_start_int_ena : R/W; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0host_rx_start_int_ena:1;
        /** fn2_slc0host_tx_start_int_ena : R/W; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0host_tx_start_int_ena:1;
        /** fn2_slc0_rx_udf_int_ena : R/W; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_rx_udf_int_ena:1;
        /** fn2_slc0_tx_ovf_int_ena : R/W; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_tx_ovf_int_ena:1;
        /** fn2_slc0_rx_pf_valid_int_ena : R/W; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_rx_pf_valid_int_ena:1;
        /** fn2_slc0_ext_bit0_int_ena : R/W; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_ext_bit0_int_ena:1;
        /** fn2_slc0_ext_bit1_int_ena : R/W; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_ext_bit1_int_ena:1;
        /** fn2_slc0_ext_bit2_int_ena : R/W; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_ext_bit2_int_ena:1;
        /** fn2_slc0_ext_bit3_int_ena : R/W; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_ext_bit3_int_ena:1;
        /** fn2_slc0_rx_new_packet_int_ena : R/W; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_rx_new_packet_int_ena:1;
        /** fn2_slc0_host_rd_retry_int_ena : R/W; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc0_host_rd_retry_int_ena:1;
        /** fn2_gpio_sdio_int_ena : R/W; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_gpio_sdio_int_ena:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc0host_func2_int_ena_reg_t;

/** Type of slc1host_func2_int_ena register
 *  *******Description***********
 */
typedef union {
    struct {
        /** fn2_slc1_tohost_bit0_int_ena : R/W; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_tohost_bit0_int_ena:1;
        /** fn2_slc1_tohost_bit1_int_ena : R/W; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_tohost_bit1_int_ena:1;
        /** fn2_slc1_tohost_bit2_int_ena : R/W; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_tohost_bit2_int_ena:1;
        /** fn2_slc1_tohost_bit3_int_ena : R/W; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_tohost_bit3_int_ena:1;
        /** fn2_slc1_tohost_bit4_int_ena : R/W; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_tohost_bit4_int_ena:1;
        /** fn2_slc1_tohost_bit5_int_ena : R/W; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_tohost_bit5_int_ena:1;
        /** fn2_slc1_tohost_bit6_int_ena : R/W; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_tohost_bit6_int_ena:1;
        /** fn2_slc1_tohost_bit7_int_ena : R/W; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_tohost_bit7_int_ena:1;
        /** fn2_slc1_token0_1to0_int_ena : R/W; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_token0_1to0_int_ena:1;
        /** fn2_slc1_token1_1to0_int_ena : R/W; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_token1_1to0_int_ena:1;
        /** fn2_slc1_token0_0to1_int_ena : R/W; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_token0_0to1_int_ena:1;
        /** fn2_slc1_token1_0to1_int_ena : R/W; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_token1_0to1_int_ena:1;
        /** fn2_slc1host_rx_sof_int_ena : R/W; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1host_rx_sof_int_ena:1;
        /** fn2_slc1host_rx_eof_int_ena : R/W; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1host_rx_eof_int_ena:1;
        /** fn2_slc1host_rx_start_int_ena : R/W; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1host_rx_start_int_ena:1;
        /** fn2_slc1host_tx_start_int_ena : R/W; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1host_tx_start_int_ena:1;
        /** fn2_slc1_rx_udf_int_ena : R/W; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_rx_udf_int_ena:1;
        /** fn2_slc1_tx_ovf_int_ena : R/W; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_tx_ovf_int_ena:1;
        /** fn2_slc1_rx_pf_valid_int_ena : R/W; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_rx_pf_valid_int_ena:1;
        /** fn2_slc1_ext_bit0_int_ena : R/W; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_ext_bit0_int_ena:1;
        /** fn2_slc1_ext_bit1_int_ena : R/W; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_ext_bit1_int_ena:1;
        /** fn2_slc1_ext_bit2_int_ena : R/W; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_ext_bit2_int_ena:1;
        /** fn2_slc1_ext_bit3_int_ena : R/W; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_ext_bit3_int_ena:1;
        /** fn2_slc1_wifi_rx_new_packet_int_ena : R/W; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_wifi_rx_new_packet_int_ena:1;
        /** fn2_slc1_host_rd_retry_int_ena : R/W; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_host_rd_retry_int_ena:1;
        /** fn2_slc1_bt_rx_new_packet_int_ena : R/W; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t fn2_slc1_bt_rx_new_packet_int_ena:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc1host_func2_int_ena_reg_t;

/** Type of slc0host_int_ena register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc0_tohost_bit0_int_ena : R/W; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit0_int_ena:1;
        /** slc0_tohost_bit1_int_ena : R/W; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit1_int_ena:1;
        /** slc0_tohost_bit2_int_ena : R/W; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit2_int_ena:1;
        /** slc0_tohost_bit3_int_ena : R/W; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit3_int_ena:1;
        /** slc0_tohost_bit4_int_ena : R/W; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit4_int_ena:1;
        /** slc0_tohost_bit5_int_ena : R/W; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit5_int_ena:1;
        /** slc0_tohost_bit6_int_ena : R/W; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit6_int_ena:1;
        /** slc0_tohost_bit7_int_ena : R/W; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit7_int_ena:1;
        /** slc0_token0_1to0_int_ena : R/W; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token0_1to0_int_ena:1;
        /** slc0_token1_1to0_int_ena : R/W; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token1_1to0_int_ena:1;
        /** slc0_token0_0to1_int_ena : R/W; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token0_0to1_int_ena:1;
        /** slc0_token1_0to1_int_ena : R/W; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token1_0to1_int_ena:1;
        /** slc0host_rx_sof_int_ena : R/W; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_sof_int_ena:1;
        /** slc0host_rx_eof_int_ena : R/W; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_eof_int_ena:1;
        /** slc0host_rx_start_int_ena : R/W; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_start_int_ena:1;
        /** slc0host_tx_start_int_ena : R/W; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_tx_start_int_ena:1;
        /** slc0_rx_udf_int_ena : R/W; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_udf_int_ena:1;
        /** slc0_tx_ovf_int_ena : R/W; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tx_ovf_int_ena:1;
        /** slc0_rx_pf_valid_int_ena : R/W; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_pf_valid_int_ena:1;
        /** slc0_ext_bit0_int_ena : R/W; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit0_int_ena:1;
        /** slc0_ext_bit1_int_ena : R/W; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit1_int_ena:1;
        /** slc0_ext_bit2_int_ena : R/W; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit2_int_ena:1;
        /** slc0_ext_bit3_int_ena : R/W; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit3_int_ena:1;
        /** slc0_rx_new_packet_int_ena : R/W; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_new_packet_int_ena:1;
        /** slc0_host_rd_retry_int_ena : R/W; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_host_rd_retry_int_ena:1;
        /** gpio_sdio_int_ena : R/W; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t gpio_sdio_int_ena:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc0host_int_ena_reg_t;

/** Type of slc1host_int_ena register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc1_tohost_bit0_int_ena : R/W; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit0_int_ena:1;
        /** slc1_tohost_bit1_int_ena : R/W; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit1_int_ena:1;
        /** slc1_tohost_bit2_int_ena : R/W; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit2_int_ena:1;
        /** slc1_tohost_bit3_int_ena : R/W; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit3_int_ena:1;
        /** slc1_tohost_bit4_int_ena : R/W; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit4_int_ena:1;
        /** slc1_tohost_bit5_int_ena : R/W; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit5_int_ena:1;
        /** slc1_tohost_bit6_int_ena : R/W; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit6_int_ena:1;
        /** slc1_tohost_bit7_int_ena : R/W; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit7_int_ena:1;
        /** slc1_token0_1to0_int_ena : R/W; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token0_1to0_int_ena:1;
        /** slc1_token1_1to0_int_ena : R/W; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token1_1to0_int_ena:1;
        /** slc1_token0_0to1_int_ena : R/W; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token0_0to1_int_ena:1;
        /** slc1_token1_0to1_int_ena : R/W; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token1_0to1_int_ena:1;
        /** slc1host_rx_sof_int_ena : R/W; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_sof_int_ena:1;
        /** slc1host_rx_eof_int_ena : R/W; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_eof_int_ena:1;
        /** slc1host_rx_start_int_ena : R/W; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_start_int_ena:1;
        /** slc1host_tx_start_int_ena : R/W; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_tx_start_int_ena:1;
        /** slc1_rx_udf_int_ena : R/W; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_rx_udf_int_ena:1;
        /** slc1_tx_ovf_int_ena : R/W; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tx_ovf_int_ena:1;
        /** slc1_rx_pf_valid_int_ena : R/W; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_rx_pf_valid_int_ena:1;
        /** slc1_ext_bit0_int_ena : R/W; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit0_int_ena:1;
        /** slc1_ext_bit1_int_ena : R/W; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit1_int_ena:1;
        /** slc1_ext_bit2_int_ena : R/W; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit2_int_ena:1;
        /** slc1_ext_bit3_int_ena : R/W; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit3_int_ena:1;
        /** slc1_wifi_rx_new_packet_int_ena : R/W; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_wifi_rx_new_packet_int_ena:1;
        /** slc1_host_rd_retry_int_ena : R/W; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_host_rd_retry_int_ena:1;
        /** slc1_bt_rx_new_packet_int_ena : R/W; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_bt_rx_new_packet_int_ena:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc1host_int_ena_reg_t;

/** Type of slc0host_rx_infor register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc0host_rx_infor : R/W; bitpos: [19:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_infor:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} slchost_slc0host_rx_infor_reg_t;

/** Type of slc1host_rx_infor register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc1host_rx_infor : R/W; bitpos: [19:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_infor:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} slchost_slc1host_rx_infor_reg_t;

/** Type of slc0host_len_wd register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc0host_len_wd : R/W; bitpos: [31:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_len_wd:32;
    };
    uint32_t val;
} slchost_slc0host_len_wd_reg_t;

/** Type of slc_apbwin_wdata register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc_apbwin_wdata : R/W; bitpos: [31:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc_apbwin_wdata:32;
    };
    uint32_t val;
} slchost_slc_apbwin_wdata_reg_t;

/** Type of slc_apbwin_conf register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc_apbwin_addr : R/W; bitpos: [27:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc_apbwin_addr:28;
        /** slc_apbwin_wr : R/W; bitpos: [28]; default: 0;
         *  *******Description***********
         */
        uint32_t slc_apbwin_wr:1;
        /** slc_apbwin_start : R/W/SC; bitpos: [29]; default: 0;
         *  *******Description***********
         */
        uint32_t slc_apbwin_start:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} slchost_slc_apbwin_conf_reg_t;

/** Type of slc_apbwin_rdata register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc_apbwin_rdata : RO; bitpos: [31:0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc_apbwin_rdata:32;
    };
    uint32_t val;
} slchost_slc_apbwin_rdata_reg_t;

/** Type of rdclr0 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_slc0_bit7_clraddr : R/W; bitpos: [8:0]; default: 68;
         *  *******Description***********
         */
        uint32_t slchost_slc0_bit7_clraddr:9;
        /** slchost_slc0_bit6_clraddr : R/W; bitpos: [17:9]; default: 480;
         *  *******Description***********
         */
        uint32_t slchost_slc0_bit6_clraddr:9;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} slchost_rdclr0_reg_t;

/** Type of rdclr1 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_slc1_bit7_clraddr : R/W; bitpos: [8:0]; default: 480;
         *  *******Description***********
         */
        uint32_t slchost_slc1_bit7_clraddr:9;
        /** slchost_slc1_bit6_clraddr : R/W; bitpos: [17:9]; default: 480;
         *  *******Description***********
         */
        uint32_t slchost_slc1_bit6_clraddr:9;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} slchost_rdclr1_reg_t;

/** Type of slc0host_int_ena1 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc0_tohost_bit0_int_ena1 : R/W; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit0_int_ena1:1;
        /** slc0_tohost_bit1_int_ena1 : R/W; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit1_int_ena1:1;
        /** slc0_tohost_bit2_int_ena1 : R/W; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit2_int_ena1:1;
        /** slc0_tohost_bit3_int_ena1 : R/W; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit3_int_ena1:1;
        /** slc0_tohost_bit4_int_ena1 : R/W; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit4_int_ena1:1;
        /** slc0_tohost_bit5_int_ena1 : R/W; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit5_int_ena1:1;
        /** slc0_tohost_bit6_int_ena1 : R/W; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit6_int_ena1:1;
        /** slc0_tohost_bit7_int_ena1 : R/W; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tohost_bit7_int_ena1:1;
        /** slc0_token0_1to0_int_ena1 : R/W; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token0_1to0_int_ena1:1;
        /** slc0_token1_1to0_int_ena1 : R/W; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token1_1to0_int_ena1:1;
        /** slc0_token0_0to1_int_ena1 : R/W; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token0_0to1_int_ena1:1;
        /** slc0_token1_0to1_int_ena1 : R/W; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_token1_0to1_int_ena1:1;
        /** slc0host_rx_sof_int_ena1 : R/W; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_sof_int_ena1:1;
        /** slc0host_rx_eof_int_ena1 : R/W; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_eof_int_ena1:1;
        /** slc0host_rx_start_int_ena1 : R/W; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_rx_start_int_ena1:1;
        /** slc0host_tx_start_int_ena1 : R/W; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0host_tx_start_int_ena1:1;
        /** slc0_rx_udf_int_ena1 : R/W; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_udf_int_ena1:1;
        /** slc0_tx_ovf_int_ena1 : R/W; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_tx_ovf_int_ena1:1;
        /** slc0_rx_pf_valid_int_ena1 : R/W; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_pf_valid_int_ena1:1;
        /** slc0_ext_bit0_int_ena1 : R/W; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit0_int_ena1:1;
        /** slc0_ext_bit1_int_ena1 : R/W; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit1_int_ena1:1;
        /** slc0_ext_bit2_int_ena1 : R/W; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit2_int_ena1:1;
        /** slc0_ext_bit3_int_ena1 : R/W; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_ext_bit3_int_ena1:1;
        /** slc0_rx_new_packet_int_ena1 : R/W; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_rx_new_packet_int_ena1:1;
        /** slc0_host_rd_retry_int_ena1 : R/W; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t slc0_host_rd_retry_int_ena1:1;
        /** gpio_sdio_int_ena1 : R/W; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t gpio_sdio_int_ena1:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc0host_int_ena1_reg_t;

/** Type of slc1host_int_ena1 register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slc1_tohost_bit0_int_ena1 : R/W; bitpos: [0]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit0_int_ena1:1;
        /** slc1_tohost_bit1_int_ena1 : R/W; bitpos: [1]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit1_int_ena1:1;
        /** slc1_tohost_bit2_int_ena1 : R/W; bitpos: [2]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit2_int_ena1:1;
        /** slc1_tohost_bit3_int_ena1 : R/W; bitpos: [3]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit3_int_ena1:1;
        /** slc1_tohost_bit4_int_ena1 : R/W; bitpos: [4]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit4_int_ena1:1;
        /** slc1_tohost_bit5_int_ena1 : R/W; bitpos: [5]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit5_int_ena1:1;
        /** slc1_tohost_bit6_int_ena1 : R/W; bitpos: [6]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit6_int_ena1:1;
        /** slc1_tohost_bit7_int_ena1 : R/W; bitpos: [7]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tohost_bit7_int_ena1:1;
        /** slc1_token0_1to0_int_ena1 : R/W; bitpos: [8]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token0_1to0_int_ena1:1;
        /** slc1_token1_1to0_int_ena1 : R/W; bitpos: [9]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token1_1to0_int_ena1:1;
        /** slc1_token0_0to1_int_ena1 : R/W; bitpos: [10]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token0_0to1_int_ena1:1;
        /** slc1_token1_0to1_int_ena1 : R/W; bitpos: [11]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_token1_0to1_int_ena1:1;
        /** slc1host_rx_sof_int_ena1 : R/W; bitpos: [12]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_sof_int_ena1:1;
        /** slc1host_rx_eof_int_ena1 : R/W; bitpos: [13]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_eof_int_ena1:1;
        /** slc1host_rx_start_int_ena1 : R/W; bitpos: [14]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_rx_start_int_ena1:1;
        /** slc1host_tx_start_int_ena1 : R/W; bitpos: [15]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1host_tx_start_int_ena1:1;
        /** slc1_rx_udf_int_ena1 : R/W; bitpos: [16]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_rx_udf_int_ena1:1;
        /** slc1_tx_ovf_int_ena1 : R/W; bitpos: [17]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_tx_ovf_int_ena1:1;
        /** slc1_rx_pf_valid_int_ena1 : R/W; bitpos: [18]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_rx_pf_valid_int_ena1:1;
        /** slc1_ext_bit0_int_ena1 : R/W; bitpos: [19]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit0_int_ena1:1;
        /** slc1_ext_bit1_int_ena1 : R/W; bitpos: [20]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit1_int_ena1:1;
        /** slc1_ext_bit2_int_ena1 : R/W; bitpos: [21]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit2_int_ena1:1;
        /** slc1_ext_bit3_int_ena1 : R/W; bitpos: [22]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_ext_bit3_int_ena1:1;
        /** slc1_wifi_rx_new_packet_int_ena1 : R/W; bitpos: [23]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_wifi_rx_new_packet_int_ena1:1;
        /** slc1_host_rd_retry_int_ena1 : R/W; bitpos: [24]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_host_rd_retry_int_ena1:1;
        /** slc1_bt_rx_new_packet_int_ena1 : R/W; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t slc1_bt_rx_new_packet_int_ena1:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} slchost_slc1host_int_ena1_reg_t;

/** Type of slchostdate register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_date : R/W; bitpos: [31:0]; default: 554043136;
         *  *******Description***********
         */
        uint32_t slchost_date:32;
    };
    uint32_t val;
} slchost_slchostdate_reg_t;

/** Type of slchostid register
 *  *******Description***********
 */
typedef union {
    struct {
        /** slchost_id : R/W; bitpos: [31:0]; default: 1536;
         *  *******Description***********
         */
        uint32_t slchost_id:32;
    };
    uint32_t val;
} slchost_slchostid_reg_t;

/** Type of conf register
 *  *******Description***********
 */
typedef union {
    struct {
        /** frc_sdio11 : R/W; bitpos: [4:0]; default: 0;
         *  *******Description***********
         */
        uint32_t frc_sdio11:5;
        /** frc_sdio20 : R/W; bitpos: [9:5]; default: 0;
         *  *******Description***********
         */
        uint32_t frc_sdio20:5;
        /** frc_neg_samp : R/W; bitpos: [14:10]; default: 0;
         *  *******Description***********
         */
        uint32_t frc_neg_samp:5;
        /** frc_pos_samp : R/W; bitpos: [19:15]; default: 0;
         *  *******Description***********
         */
        uint32_t frc_pos_samp:5;
        /** frc_quick_in : R/W; bitpos: [24:20]; default: 0;
         *  *******Description***********
         */
        uint32_t frc_quick_in:5;
        /** sdio20_int_delay : R/W; bitpos: [25]; default: 0;
         *  *******Description***********
         */
        uint32_t sdio20_int_delay:1;
        /** sdio_pad_pullup : R/W; bitpos: [26]; default: 0;
         *  *******Description***********
         */
        uint32_t sdio_pad_pullup:1;
        /** hspeed_con_en : R/W; bitpos: [27]; default: 0;
         *  *******Description***********
         */
        uint32_t hspeed_con_en:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} slchost_conf_reg_t;

/** Type of inf_st register
 *  *******Description***********
 */
typedef union {
    struct {
        /** sdio20_mode : RO; bitpos: [4:0]; default: 0;
         *  *******Description***********
         */
        uint32_t sdio20_mode:5;
        /** sdio_neg_samp : RO; bitpos: [9:5]; default: 0;
         *  *******Description***********
         */
        uint32_t sdio_neg_samp:5;
        /** sdio_quick_in : RO; bitpos: [14:10]; default: 0;
         *  *******Description***********
         */
        uint32_t sdio_quick_in:5;
        /** dll_on_sw : R/W; bitpos: [15]; default: 0;
         *  dll is controlled by software
         */
        uint32_t dll_on_sw:1;
        /** dll_on : R/W; bitpos: [16]; default: 0;
         *  Software dll on
         */
        uint32_t dll_on:1;
        /** clk_mode_sw : R/W; bitpos: [17]; default: 0;
         *  dll clock mode is controlled by software
         */
        uint32_t clk_mode_sw:1;
        /** clk_mode : R/W; bitpos: [19:18]; default: 0;
         *  Software set clock mode
         */
        uint32_t clk_mode:2;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} slchost_inf_st_reg_t;


typedef struct host_dev_t {
    uint32_t reserved_000[4];
    volatile slchost_func2_0_reg_t func2_0;
    volatile slchost_func2_1_reg_t func2_1;
    uint32_t reserved_018[2];
    volatile slchost_func2_2_reg_t func2_2;
    uint32_t reserved_024[4];
    volatile slchost_gpio_status0_reg_t gpio_status0;
    volatile slchost_gpio_status1_reg_t gpio_status1;
    volatile slchost_gpio_in0_reg_t gpio_in0;
    volatile slchost_gpio_in1_reg_t gpio_in1;
    volatile slchost_slc0host_token_rdata_reg_t slc0host_token_rdata;
    volatile slchost_slc0_host_pf_reg_t slc0_host_pf;
    volatile slchost_slc1_host_pf_reg_t slc1_host_pf;
    volatile slchost_slc0host_int_raw_reg_t slc0host_int_raw;
    volatile slchost_slc1host_int_raw_reg_t slc1host_int_raw;
    volatile slchost_slc0host_int_st_reg_t slc0host_int_st;
    volatile slchost_slc1host_int_st_reg_t slc1host_int_st;
    volatile slchost_pkt_len_reg_t pkt_len;
    volatile slchost_state_w0_reg_t state_w0;
    volatile slchost_state_w1_reg_t state_w1;
    volatile slchost_conf_w0_reg_t conf_w0;
    volatile slchost_conf_w1_reg_t conf_w1;
    volatile slchost_conf_w2_reg_t conf_w2;
    volatile slchost_conf_w3_reg_t conf_w3;
    volatile slchost_conf_w4_reg_t conf_w4;
    volatile slchost_conf_w5_reg_t conf_w5;
    volatile slchost_win_cmd_reg_t win_cmd;
    volatile slchost_conf_w6_reg_t conf_w6;
    volatile slchost_conf_w7_reg_t conf_w7;
    volatile slchost_pkt_len0_reg_t pkt_len0;
    volatile slchost_pkt_len1_reg_t pkt_len1;
    volatile slchost_pkt_len2_reg_t pkt_len2;
    volatile slchost_conf_w8_reg_t conf_w8;
    volatile slchost_conf_w9_reg_t conf_w9;
    volatile slchost_conf_w10_reg_t conf_w10;
    volatile slchost_conf_w11_reg_t conf_w11;
    volatile slchost_conf_w12_reg_t conf_w12;
    volatile slchost_conf_w13_reg_t conf_w13;
    volatile slchost_conf_w14_reg_t conf_w14;
    volatile slchost_conf_w15_reg_t conf_w15;
    volatile slchost_check_sum0_reg_t check_sum0;
    volatile slchost_check_sum1_reg_t check_sum1;
    volatile slchost_slc1host_token_rdata_reg_t slc1host_token_rdata;
    volatile slchost_slc0host_token_wdata_reg_t slc0host_token_wdata;
    volatile slchost_slc1host_token_wdata_reg_t slc1host_token_wdata;
    volatile slchost_token_con_reg_t token_con;
    volatile slchost_slc0host_int_clr_reg_t slc0host_int_clr;
    volatile slchost_slc1host_int_clr_reg_t slc1host_int_clr;
    volatile slchost_slc0host_func1_int_ena_reg_t slc0host_func1_int_ena;
    volatile slchost_slc1host_func1_int_ena_reg_t slc1host_func1_int_ena;
    volatile slchost_slc0host_func2_int_ena_reg_t slc0host_func2_int_ena;
    volatile slchost_slc1host_func2_int_ena_reg_t slc1host_func2_int_ena;
    volatile slchost_slc0host_int_ena_reg_t slc0host_int_ena;
    volatile slchost_slc1host_int_ena_reg_t slc1host_int_ena;
    volatile slchost_slc0host_rx_infor_reg_t slc0host_rx_infor;
    volatile slchost_slc1host_rx_infor_reg_t slc1host_rx_infor;
    volatile slchost_slc0host_len_wd_reg_t slc0host_len_wd;
    volatile slchost_slc_apbwin_wdata_reg_t slc_apbwin_wdata;
    volatile slchost_slc_apbwin_conf_reg_t slc_apbwin_conf;
    volatile slchost_slc_apbwin_rdata_reg_t slc_apbwin_rdata;
    volatile slchost_rdclr0_reg_t rdclr0;
    volatile slchost_rdclr1_reg_t rdclr1;
    volatile slchost_slc0host_int_ena1_reg_t slc0host_int_ena1;
    volatile slchost_slc1host_int_ena1_reg_t slc1host_int_ena1;
    uint32_t reserved_11c[23];
    volatile slchost_slchostdate_reg_t slchostdate;
    volatile slchost_slchostid_reg_t slchostid;
    uint32_t reserved_180[28];
    volatile slchost_conf_reg_t conf;
    volatile slchost_inf_st_reg_t inf_st;
} host_dev_t;

extern host_dev_t HOST;

#ifndef __cplusplus
_Static_assert(sizeof(host_dev_t) == 0x1f8, "Invalid size of host_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
