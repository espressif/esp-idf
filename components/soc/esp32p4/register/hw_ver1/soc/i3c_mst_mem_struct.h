/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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


/** Group: I3C DEV ADDR TABLE1 LOC REG */
/** Type of dev_addr_table1_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_dev1_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_dev1_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev1_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_dev1_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev1_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_dev1_nack_retry_cnt:2;
        /** reg_dat_dev1_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_dev1_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_table1_loc_reg_t;


/** Group: I3C DEV ADDR TABLE2 LOC REG */
/** Type of dev_addr_table2_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_dev2_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_dev2_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev2_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_dev2_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev2_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_dev2_nack_retry_cnt:2;
        /** reg_dat_dev2_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_dev2_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_table2_loc_reg_t;


/** Group: I3C DEV ADDR TABLE3 LOC REG */
/** Type of dev_addr_table3_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_dev3_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_dev3_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev3_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_dev3_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev3_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_dev3_nack_retry_cnt:2;
        /** reg_dat_dev3_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_dev3_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_table3_loc_reg_t;


/** Group: I3C DEV ADDR TABLE4 LOC REG */
/** Type of dev_addr_table4_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_dev4_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_dev4_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev4_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_dev4_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev4_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_dev4_nack_retry_cnt:2;
        /** reg_dat_dev4_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_dev4_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_table4_loc_reg_t;


/** Group: I3C DEV ADDR TABLE5 LOC REG */
/** Type of dev_addr_table5_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_dev5_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_dev5_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev5_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_dev5_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev5_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_dev5_nack_retry_cnt:2;
        /** reg_dat_dev5_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_dev5_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_table5_loc_reg_t;


/** Group: I3C DEV ADDR TABLE6 LOC REG */
/** Type of dev_addr_table6_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_dev6_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_dev6_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev6_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_dev6_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev6_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_dev6_nack_retry_cnt:2;
        /** reg_dat_dev6_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_dev6_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_table6_loc_reg_t;


/** Group: I3C DEV ADDR TABLE7 LOC REG */
/** Type of dev_addr_table7_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_dev7_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_dev7_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev7_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_dev7_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev7_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_dev7_nack_retry_cnt:2;
        /** reg_dat_dev7_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_dev7_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_table7_loc_reg_t;


/** Group: I3C DEV ADDR TABLE8 LOC REG */
/** Type of dev_addr_table8_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_dev8_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_dev8_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev8_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_dev8_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev8_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_dev8_nack_retry_cnt:2;
        /** reg_dat_dev8_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_dev8_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_table8_loc_reg_t;


/** Group: I3C DEV ADDR TABLE9 LOC REG */
/** Type of dev_addr_table9_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_dev9_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_dev9_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev9_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_dev9_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev9_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_dev9_nack_retry_cnt:2;
        /** reg_dat_dev9_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_dev9_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_table9_loc_reg_t;


/** Group: I3C DEV ADDR TABLE10 LOC REG */
/** Type of dev_addr_table10_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_dev10_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_dev10_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev10_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_dev10_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev10_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_dev10_nack_retry_cnt:2;
        /** reg_dat_dev10_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_dev10_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_table10_loc_reg_t;


/** Group: I3C DEV ADDR TABLE11 LOC REG */
/** Type of dev_addr_table11_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_dev11_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_dev11_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev11_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_dev11_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev11_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_dev11_nack_retry_cnt:2;
        /** reg_dat_dev11_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_dev11_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_table11_loc_reg_t;


/** Group: I3C DEV ADDR TABLE12 LOC REG */
/** Type of dev_addr_table12_loc register
 *  NA
 */
typedef union {
    struct {
        /** reg_dat_dev12_static_addr : R/W; bitpos: [6:0]; default: 0;
         *  NA
         */
        uint32_t reg_dat_dev12_static_addr:7;
        uint32_t reserved_7:9;
        /** reg_dat_dev12_dynamic_addr : R/W; bitpos: [23:16]; default: 0;
         *  Device Dynamic Address with parity, The MSB,bit[23], should be programmed with
         *  parity of dynamic address.
         */
        uint32_t reg_dat_dev12_dynamic_addr:8;
        uint32_t reserved_24:5;
        /** reg_dat_dev12_nack_retry_cnt : R/W; bitpos: [30:29]; default: 0;
         *  This field is used to set the Device NACK Retry count for the particular device. If
         *  the Device NACK's for the device address, the controller automatically retries the
         *  same device until this count expires. If the Slave does not ACK for the mentioned
         *  number of retries, then controller generates an error response and move to the Halt
         *  state.
         */
        uint32_t reg_dat_dev12_nack_retry_cnt:2;
        /** reg_dat_dev12_i2c : R/W; bitpos: [31]; default: 0;
         *  Legacy I2C device or not. This bit should be set to 1 if the device is a legacy I2C
         *  device.
         */
        uint32_t reg_dat_dev12_i2c:1;
    };
    uint32_t val;
} i3c_mst_mem_dev_addr_table12_loc_reg_t;


/** Group: I3C DEV CHAR TABLE1 LOC1 REG */
/** Type of dev_char_table1_loc1 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev1_loc1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev1_loc1:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table1_loc1_reg_t;


/** Group: I3C DEV CHAR TABLE1 LOC2 REG */
/** Type of dev_char_table1_loc2 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev1_loc2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev1_loc2:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table1_loc2_reg_t;


/** Group: I3C DEV CHAR TABLE1 LOC3 REG */
/** Type of dev_char_table1_loc3 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev1_loc3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev1_loc3:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table1_loc3_reg_t;


/** Group: I3C DEV CHAR TABLE1 LOC4 REG */
/** Type of dev_char_table1_loc4 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev1_loc4 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev1_loc4:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table1_loc4_reg_t;


/** Group: I3C DEV CHAR TABLE2 LOC1 REG */
/** Type of dev_char_table2_loc1 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev2_loc1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev2_loc1:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table2_loc1_reg_t;


/** Group: I3C DEV CHAR TABLE2 LOC2 REG */
/** Type of dev_char_table2_loc2 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev2_loc2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev2_loc2:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table2_loc2_reg_t;


/** Group: I3C DEV CHAR TABLE2 LOC3 REG */
/** Type of dev_char_table2_loc3 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev2_loc3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev2_loc3:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table2_loc3_reg_t;


/** Group: I3C DEV CHAR TABLE2 LOC4 REG */
/** Type of dev_char_table2_loc4 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev2_loc4 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev2_loc4:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table2_loc4_reg_t;


/** Group: I3C DEV CHAR TABLE3 LOC1 REG */
/** Type of dev_char_table3_loc1 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev3_loc1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev3_loc1:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table3_loc1_reg_t;


/** Group: I3C DEV CHAR TABLE3 LOC2 REG */
/** Type of dev_char_table3_loc2 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev3_loc2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev3_loc2:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table3_loc2_reg_t;


/** Group: I3C DEV CHAR TABLE3 LOC3 REG */
/** Type of dev_char_table3_loc3 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev3_loc3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev3_loc3:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table3_loc3_reg_t;


/** Group: I3C DEV CHAR TABLE3 LOC4 REG */
/** Type of dev_char_table3_loc4 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev3_loc4 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev3_loc4:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table3_loc4_reg_t;


/** Group: I3C DEV CHAR TABLE4 LOC1 REG */
/** Type of dev_char_table4_loc1 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev4_loc1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev4_loc1:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table4_loc1_reg_t;


/** Group: I3C DEV CHAR TABLE4 LOC2 REG */
/** Type of dev_char_table4_loc2 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev4_loc2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev4_loc2:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table4_loc2_reg_t;


/** Group: I3C DEV CHAR TABLE4 LOC3 REG */
/** Type of dev_char_table4_loc3 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev4_loc3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev4_loc3:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table4_loc3_reg_t;


/** Group: I3C DEV CHAR TABLE4 LOC4 REG */
/** Type of dev_char_table4_loc4 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev4_loc4 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev4_loc4:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table4_loc4_reg_t;


/** Group: I3C DEV CHAR TABLE5 LOC1 REG */
/** Type of dev_char_table5_loc1 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev5_loc1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev5_loc1:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table5_loc1_reg_t;


/** Group: I3C DEV CHAR TABLE5 LOC2 REG */
/** Type of dev_char_table5_loc2 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev5_loc2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev5_loc2:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table5_loc2_reg_t;


/** Group: I3C DEV CHAR TABLE5 LOC3 REG */
/** Type of dev_char_table5_loc3 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev5_loc3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev5_loc3:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table5_loc3_reg_t;


/** Group: I3C DEV CHAR TABLE5 LOC4 REG */
/** Type of dev_char_table5_loc4 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev5_loc4 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev5_loc4:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table5_loc4_reg_t;


/** Group: I3C DEV CHAR TABLE6 LOC1 REG */
/** Type of dev_char_table6_loc1 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev6_loc1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev6_loc1:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table6_loc1_reg_t;


/** Group: I3C DEV CHAR TABLE6 LOC2 REG */
/** Type of dev_char_table6_loc2 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev6_loc2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev6_loc2:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table6_loc2_reg_t;


/** Group: I3C DEV CHAR TABLE6 LOC3 REG */
/** Type of dev_char_table6_loc3 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev6_loc3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev6_loc3:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table6_loc3_reg_t;


/** Group: I3C DEV CHAR TABLE6 LOC4 REG */
/** Type of dev_char_table6_loc4 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev6_loc4 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev6_loc4:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table6_loc4_reg_t;


/** Group: I3C DEV CHAR TABLE7 LOC1 REG */
/** Type of dev_char_table7_loc1 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev7_loc1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev7_loc1:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table7_loc1_reg_t;


/** Group: I3C DEV CHAR TABLE7 LOC2 REG */
/** Type of dev_char_table7_loc2 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev7_loc2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev7_loc2:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table7_loc2_reg_t;


/** Group: I3C DEV CHAR TABLE7 LOC3 REG */
/** Type of dev_char_table7_loc3 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev7_loc3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev7_loc3:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table7_loc3_reg_t;


/** Group: I3C DEV CHAR TABLE7 LOC4 REG */
/** Type of dev_char_table7_loc4 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev7_loc4 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev7_loc4:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table7_loc4_reg_t;


/** Group: I3C DEV CHAR TABLE8 LOC1 REG */
/** Type of dev_char_table8_loc1 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev8_loc1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev8_loc1:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table8_loc1_reg_t;


/** Group: I3C DEV CHAR TABLE8 LOC2 REG */
/** Type of dev_char_table8_loc2 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev8_loc2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev8_loc2:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table8_loc2_reg_t;


/** Group: I3C DEV CHAR TABLE8 LOC3 REG */
/** Type of dev_char_table8_loc3 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev8_loc3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev8_loc3:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table8_loc3_reg_t;


/** Group: I3C DEV CHAR TABLE8 LOC4 REG */
/** Type of dev_char_table8_loc4 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev8_loc4 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev8_loc4:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table8_loc4_reg_t;


/** Group: I3C DEV CHAR TABLE9 LOC1 REG */
/** Type of dev_char_table9_loc1 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev9_loc1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev9_loc1:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table9_loc1_reg_t;


/** Group: I3C DEV CHAR TABLE9 LOC2 REG */
/** Type of dev_char_table9_loc2 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev9_loc2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev9_loc2:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table9_loc2_reg_t;


/** Group: I3C DEV CHAR TABLE9 LOC3 REG */
/** Type of dev_char_table9_loc3 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev9_loc3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev9_loc3:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table9_loc3_reg_t;


/** Group: I3C DEV CHAR TABLE9 LOC4 REG */
/** Type of dev_char_table9_loc4 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev9_loc4 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev9_loc4:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table9_loc4_reg_t;


/** Group: I3C DEV CHAR TABLE10 LOC1 REG */
/** Type of dev_char_table10_loc1 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev10_loc1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev10_loc1:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table10_loc1_reg_t;


/** Group: I3C DEV CHAR TABLE10 LOC2 REG */
/** Type of dev_char_table10_loc2 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev10_loc2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev10_loc2:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table10_loc2_reg_t;


/** Group: I3C DEV CHAR TABLE10 LOC3 REG */
/** Type of dev_char_table10_loc3 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev10_loc3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev10_loc3:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table10_loc3_reg_t;


/** Group: I3C DEV CHAR TABLE10 LOC4 REG */
/** Type of dev_char_table10_loc4 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev10_loc4 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev10_loc4:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table10_loc4_reg_t;


/** Group: I3C DEV CHAR TABLE11 LOC1 REG */
/** Type of dev_char_table11_loc1 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev11_loc1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev11_loc1:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table11_loc1_reg_t;


/** Group: I3C DEV CHAR TABLE11 LOC2 REG */
/** Type of dev_char_table11_loc2 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev11_loc2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev11_loc2:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table11_loc2_reg_t;


/** Group: I3C DEV CHAR TABLE11 LOC3 REG */
/** Type of dev_char_table11_loc3 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev11_loc3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev11_loc3:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table11_loc3_reg_t;


/** Group: I3C DEV CHAR TABLE11 LOC4 REG */
/** Type of dev_char_table11_loc4 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev11_loc4 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev11_loc4:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table11_loc4_reg_t;


/** Group: I3C DEV CHAR TABLE12 LOC1 REG */
/** Type of dev_char_table12_loc1 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev12_loc1 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev12_loc1:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table12_loc1_reg_t;


/** Group: I3C DEV CHAR TABLE12 LOC2 REG */
/** Type of dev_char_table12_loc2 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev12_loc2 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev12_loc2:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table12_loc2_reg_t;


/** Group: I3C DEV CHAR TABLE12 LOC3 REG */
/** Type of dev_char_table12_loc3 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev12_loc3 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev12_loc3:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table12_loc3_reg_t;


/** Group: I3C DEV CHAR TABLE12 LOC4 REG */
/** Type of dev_char_table12_loc4 register
 *  NA
 */
typedef union {
    struct {
        /** dct_dev12_loc4 : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t dct_dev12_loc4:32;
    };
    uint32_t val;
} i3c_mst_mem_dev_char_table12_loc4_reg_t;


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
    volatile i3c_mst_mem_dev_addr_table1_loc_reg_t dev_addr_table1_loc;
    volatile i3c_mst_mem_dev_addr_table2_loc_reg_t dev_addr_table2_loc;
    volatile i3c_mst_mem_dev_addr_table3_loc_reg_t dev_addr_table3_loc;
    volatile i3c_mst_mem_dev_addr_table4_loc_reg_t dev_addr_table4_loc;
    volatile i3c_mst_mem_dev_addr_table5_loc_reg_t dev_addr_table5_loc;
    volatile i3c_mst_mem_dev_addr_table6_loc_reg_t dev_addr_table6_loc;
    volatile i3c_mst_mem_dev_addr_table7_loc_reg_t dev_addr_table7_loc;
    volatile i3c_mst_mem_dev_addr_table8_loc_reg_t dev_addr_table8_loc;
    volatile i3c_mst_mem_dev_addr_table9_loc_reg_t dev_addr_table9_loc;
    volatile i3c_mst_mem_dev_addr_table10_loc_reg_t dev_addr_table10_loc;
    volatile i3c_mst_mem_dev_addr_table11_loc_reg_t dev_addr_table11_loc;
    volatile i3c_mst_mem_dev_addr_table12_loc_reg_t dev_addr_table12_loc;
    uint32_t reserved_0f0[4];
    volatile i3c_mst_mem_dev_char_table1_loc1_reg_t dev_char_table1_loc1;
    volatile i3c_mst_mem_dev_char_table1_loc2_reg_t dev_char_table1_loc2;
    volatile i3c_mst_mem_dev_char_table1_loc3_reg_t dev_char_table1_loc3;
    volatile i3c_mst_mem_dev_char_table1_loc4_reg_t dev_char_table1_loc4;
    volatile i3c_mst_mem_dev_char_table2_loc1_reg_t dev_char_table2_loc1;
    volatile i3c_mst_mem_dev_char_table2_loc2_reg_t dev_char_table2_loc2;
    volatile i3c_mst_mem_dev_char_table2_loc3_reg_t dev_char_table2_loc3;
    volatile i3c_mst_mem_dev_char_table2_loc4_reg_t dev_char_table2_loc4;
    volatile i3c_mst_mem_dev_char_table3_loc1_reg_t dev_char_table3_loc1;
    volatile i3c_mst_mem_dev_char_table3_loc2_reg_t dev_char_table3_loc2;
    volatile i3c_mst_mem_dev_char_table3_loc3_reg_t dev_char_table3_loc3;
    volatile i3c_mst_mem_dev_char_table3_loc4_reg_t dev_char_table3_loc4;
    volatile i3c_mst_mem_dev_char_table4_loc1_reg_t dev_char_table4_loc1;
    volatile i3c_mst_mem_dev_char_table4_loc2_reg_t dev_char_table4_loc2;
    volatile i3c_mst_mem_dev_char_table4_loc3_reg_t dev_char_table4_loc3;
    volatile i3c_mst_mem_dev_char_table4_loc4_reg_t dev_char_table4_loc4;
    volatile i3c_mst_mem_dev_char_table5_loc1_reg_t dev_char_table5_loc1;
    volatile i3c_mst_mem_dev_char_table5_loc2_reg_t dev_char_table5_loc2;
    volatile i3c_mst_mem_dev_char_table5_loc3_reg_t dev_char_table5_loc3;
    volatile i3c_mst_mem_dev_char_table5_loc4_reg_t dev_char_table5_loc4;
    volatile i3c_mst_mem_dev_char_table6_loc1_reg_t dev_char_table6_loc1;
    volatile i3c_mst_mem_dev_char_table6_loc2_reg_t dev_char_table6_loc2;
    volatile i3c_mst_mem_dev_char_table6_loc3_reg_t dev_char_table6_loc3;
    volatile i3c_mst_mem_dev_char_table6_loc4_reg_t dev_char_table6_loc4;
    volatile i3c_mst_mem_dev_char_table7_loc1_reg_t dev_char_table7_loc1;
    volatile i3c_mst_mem_dev_char_table7_loc2_reg_t dev_char_table7_loc2;
    volatile i3c_mst_mem_dev_char_table7_loc3_reg_t dev_char_table7_loc3;
    volatile i3c_mst_mem_dev_char_table7_loc4_reg_t dev_char_table7_loc4;
    volatile i3c_mst_mem_dev_char_table8_loc1_reg_t dev_char_table8_loc1;
    volatile i3c_mst_mem_dev_char_table8_loc2_reg_t dev_char_table8_loc2;
    volatile i3c_mst_mem_dev_char_table8_loc3_reg_t dev_char_table8_loc3;
    volatile i3c_mst_mem_dev_char_table8_loc4_reg_t dev_char_table8_loc4;
    volatile i3c_mst_mem_dev_char_table9_loc1_reg_t dev_char_table9_loc1;
    volatile i3c_mst_mem_dev_char_table9_loc2_reg_t dev_char_table9_loc2;
    volatile i3c_mst_mem_dev_char_table9_loc3_reg_t dev_char_table9_loc3;
    volatile i3c_mst_mem_dev_char_table9_loc4_reg_t dev_char_table9_loc4;
    volatile i3c_mst_mem_dev_char_table10_loc1_reg_t dev_char_table10_loc1;
    volatile i3c_mst_mem_dev_char_table10_loc2_reg_t dev_char_table10_loc2;
    volatile i3c_mst_mem_dev_char_table10_loc3_reg_t dev_char_table10_loc3;
    volatile i3c_mst_mem_dev_char_table10_loc4_reg_t dev_char_table10_loc4;
    volatile i3c_mst_mem_dev_char_table11_loc1_reg_t dev_char_table11_loc1;
    volatile i3c_mst_mem_dev_char_table11_loc2_reg_t dev_char_table11_loc2;
    volatile i3c_mst_mem_dev_char_table11_loc3_reg_t dev_char_table11_loc3;
    volatile i3c_mst_mem_dev_char_table11_loc4_reg_t dev_char_table11_loc4;
    volatile i3c_mst_mem_dev_char_table12_loc1_reg_t dev_char_table12_loc1;
    volatile i3c_mst_mem_dev_char_table12_loc2_reg_t dev_char_table12_loc2;
    volatile i3c_mst_mem_dev_char_table12_loc3_reg_t dev_char_table12_loc3;
    volatile i3c_mst_mem_dev_char_table12_loc4_reg_t dev_char_table12_loc4;
} i3c_mst_mem_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(i3c_mst_mem_dev_t) == 0x1c0, "Invalid size of i3c_mst_mem_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
