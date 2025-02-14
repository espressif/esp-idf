/**
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: I3C COMMAND BUF PORT REG */
/** Type of command_buf_port register
 *  NA
 */
typedef union {
    struct {
        /** reg_command : R/W; bitpos: [31:0]; default: 0;
         *  Contains a Command Descriptor structure that depends on the requested transfer
         *  type. Command Descriptor structure is used to schedule the transfers to devices on
         *  I3C bus.
         */
        uint32_t reg_command:32;
    };
    uint32_t val;
} i3c_mst_mem_command_buf_port_reg_t;


/** Group: I3C RESPONSE BUF PORT REG */
/** Type of response_buf_port register
 *  NA
 */
typedef union {
    struct {
        /** response : RO; bitpos: [31:0]; default: 0;
         *  The Response Buffer can be read through this register. The response status for each
         *  Command is written into the Response Buffer by the controller if ROC (Response On
         *  Completion) bit is set or if transfer error has occurred. The response buffer can
         *  be read through this register.
         */
        uint32_t response:32;
    };
    uint32_t val;
} i3c_mst_mem_response_buf_port_reg_t;


/** Group: I3C RX DATA PORT REG */
/** Type of rx_data_port register
 *  NA
 */
typedef union {
    struct {
        /** rx_data_port : RO; bitpos: [31:0]; default: 0;
         *  Receive Data Port. Receive data is mapped to the Rx-data buffer and receive data is
         *  always packed in 4-byte aligned data words.  If the length of data transfer is not
         *  aligned to 4-bytes boundary, then there will be extra(unused) bytes(the additional
         *  data bytes have to be ignored) at the end of the transferred data. The valid data
         *  must be identified using the DATA_LENGTH filed in the Response Descriptor.
         */
        uint32_t rx_data_port:32;
    };
    uint32_t val;
} i3c_mst_mem_rx_data_port_reg_t;


/** Group: I3C TX DATA PORT REG */
/** Type of tx_data_port register
 *  NA
 */
typedef union {
    struct {
        /** reg_tx_data_port : R/W; bitpos: [31:0]; default: 0;
         *  Transmit Data Port. Transmit data is mapped to the Tx-data  buffer and transmit
         *  data is always packed in 4-byte aligned data words.  If the length of data transfer
         *  is not aligned to 4-bytes boundary, then there will be extra(unused) bytes(the
         *  additional data bytes have to be ignored) at the end of the transferred data. The
         *  valid data must be identified using the DATA_LENGTH filed in the Response
         *  Descriptor.
         */
        uint32_t reg_tx_data_port:32;
    };
    uint32_t val;
} i3c_mst_mem_tx_data_port_reg_t;


/** Group: I3C IBI STATUS BUF REG */
/** Type of ibi_status_buf register
 *  In-Band Interrupt Buffer Status/Data Register. When receiving an IBI, IBI_PORT is
 *  used to both: Read the IBI Status Read the IBI Data(which is raw/opaque data)
 */
typedef union {
    struct {
        /** data_length : RO; bitpos: [7:0]; default: 0;
         *  This field represents the length of data received along with IBI, in bytes.
         */
        uint32_t data_length:8;
        /** ibi_id : RO; bitpos: [15:8]; default: 0;
         *  IBI Identifier. The byte received after START which includes the address the R/W
         *  bit: Device  address and R/W bit in case of Slave Interrupt or Master Request.
         */
        uint32_t ibi_id:8;
        uint32_t reserved_16:12;
        /** ibi_sts : RO; bitpos: [28]; default: 0;
         *  IBI received data/status. IBI Data register is mapped to the IBI Buffer. The IBI
         *  Data is always packed in4-byte aligned and put to the IBI Buffer. This register
         *  When read from, reads the data from the IBI buffer. IBI Status register when read
         *  from, returns the data from the IBI Buffer and indicates how the controller
         *  responded to incoming IBI(SIR, MR and HJ).
         */
        uint32_t ibi_sts:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} i3c_mst_mem_ibi_status_buf_reg_t;


/** Group: I3C IBI DATA BUF REG */
/** Type of ibi_data_buf register
 *  NA
 */
typedef union {
    struct {
        /** ibi_data : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t ibi_data:32;
    };
    uint32_t val;
} i3c_mst_mem_ibi_data_buf_reg_t;

/** Group: I3C DEV ADDR TABLEn LOC REG */
/** Type of dev_addr_tablen_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_devn_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_devn_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev12_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_devn_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev12_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_devn_nack_retry_cnt:2;
        /** reg_dat_dev12_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_devn_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_tablen_loc_reg_t;

typedef struct {
    volatile uint32_t loc1;
    volatile uint32_t loc2;
    volatile uint32_t loc3;
    volatile uint32_t loc4;
} i3c_mst_mem_dev_char_tablen_reg_t;

typedef struct {
    uint32_t reserved_000[2];
    volatile i3c_mst_mem_command_buf_port_reg_t command_buf_port;
    volatile i3c_mst_mem_response_buf_port_reg_t response_buf_port;
    volatile i3c_mst_mem_rx_data_port_reg_t rx_data_port;
    volatile i3c_mst_mem_tx_data_port_reg_t tx_data_port;
    volatile i3c_mst_mem_ibi_status_buf_reg_t ibi_status_buf;
    uint32_t reserved_01c[9];
    volatile i3c_mst_mem_ibi_data_buf_reg_t ibi_data_buf;
    uint32_t reserved_044[31];
    volatile i3c_mst_mem_dev_addr_tablen_loc_reg_t dev_addr_table[12];
    uint32_t reserved_0f0[4];
    volatile i3c_mst_mem_dev_char_tablen_reg_t dev_char_table[12];
} i3c_mst_mem_dev_t;

extern i3c_mst_mem_dev_t I3C_MST_MEM;

#ifndef __cplusplus
_Static_assert(sizeof(i3c_mst_mem_dev_t) == 0x1c0, "Invalid size of i3c_mst_mem_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
