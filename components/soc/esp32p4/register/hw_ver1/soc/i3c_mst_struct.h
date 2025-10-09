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

/** Group: I3C DEVICE CTRL REG */
/** Type of device_ctrl register
 *  DEVICE_CTRL register controls the transfer properties and disposition of
 *  controllers capabilities.
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** reg_ba_include : R/W; bitpos: [1]; default: 0;
         *  This bit is used to include I3C broadcast address(0x7E) for private transfer.(If
         *  I3C broadcast address is not include for the private transfer, In-Band Interrupts
         *  driven from Slaves may not win address arbitration. Hence IBIs will get delayed)
         */
        uint32_t reg_ba_include:1;
        /** reg_trans_start : R/W; bitpos: [2]; default: 0;
         *  Transfer Start
         */
        uint32_t reg_trans_start:1;
        /** reg_clk_en : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t reg_clk_en:1;
        /** reg_ibi_rstart_trans_en : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t reg_ibi_rstart_trans_en:1;
        /** reg_auto_dis_ibi_en : R/W; bitpos: [5]; default: 1;
         *  NA
         */
        uint32_t reg_auto_dis_ibi_en:1;
        /** reg_dma_rx_en : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t reg_dma_rx_en:1;
        /** reg_dma_tx_en : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t reg_dma_tx_en:1;
        /** reg_multi_slv_single_ccc_en : R/W; bitpos: [8]; default: 0;
         *  0: rx high bit first, 1: rx low bit first
         */
        uint32_t reg_multi_slv_single_ccc_en:1;
        /** reg_rx_bit_order : R/W; bitpos: [9]; default: 0;
         *  0: rx low byte fist, 1: rx high byte first
         */
        uint32_t reg_rx_bit_order:1;
        /** reg_rx_byte_order : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t reg_rx_byte_order:1;
        /** reg_scl_pullup_force_en : R/W; bitpos: [11]; default: 0;
         *  This bit is used to force scl_pullup_en
         */
        uint32_t reg_scl_pullup_force_en:1;
        /** reg_scl_oe_force_en : R/W; bitpos: [12]; default: 1;
         *  This bit is used to force scl_oe
         */
        uint32_t reg_scl_oe_force_en:1;
        /** reg_sda_pp_rd_pullup_en : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t reg_sda_pp_rd_pullup_en:1;
        /** reg_sda_rd_tbit_hlvl_pullup_en : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t reg_sda_rd_tbit_hlvl_pullup_en:1;
        /** reg_sda_pp_wr_pullup_en : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t reg_sda_pp_wr_pullup_en:1;
        /** reg_data_byte_cnt_unlatch : R/W; bitpos: [16]; default: 0;
         *  1: read current real-time updated value  0: read latch data byte cnt value
         */
        uint32_t reg_data_byte_cnt_unlatch:1;
        /** reg_mem_clk_force_on : R/W; bitpos: [17]; default: 0;
         *  1: dev characteristic and address table memory clk  date force on . 0 :  clock
         *  gating by rd/wr.
         */
        uint32_t reg_mem_clk_force_on:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} i3c_mst_device_ctrl_reg_t;


/** Group: I3C BUFFER THLD CTRL REG */
/** Type of buffer_thld_ctrl register
 *  In-Band Interrupt Status Threshold Value . Every In Band Interrupt received by I3C
 *  controller generates an IBI status. This field controls the number of IBI status
 *  entries in the IBI buffer that trigger the IBI_STATUS_THLD_STAT interrupt.
 */
typedef union {
    struct {
        /** reg_cmd_buf_empty_thld : R/W; bitpos: [3:0]; default: 1;
         *  Command Buffer Empty Threshold Value is used to control the number of empty
         *  locations(or greater) in the Command Buffer that trigger CMD_BUFFER_READY_STAT
         *  interrupt.
         */
        uint32_t reg_cmd_buf_empty_thld:4;
        uint32_t reserved_4:2;
        /** reg_resp_buf_thld : R/W; bitpos: [8:6]; default: 1;
         *  Response Buffer Threshold Value is used to control the number of entries in the
         *  Response Buffer that trigger the RESP_READY_STAT_INTR.
         */
        uint32_t reg_resp_buf_thld:3;
        uint32_t reserved_9:3;
        /** reg_ibi_data_buf_thld : R/W; bitpos: [14:12]; default: 1;
         *  In-Band Interrupt Data Threshold Value . Every In Band Interrupt received by I3C
         *  controller generates an IBI status. This field controls the number of IBI data
         *  entries in the IBI buffer that trigger the IBI_DATA_THLD_STAT interrupt.
         */
        uint32_t reg_ibi_data_buf_thld:3;
        uint32_t reserved_15:3;
        /** reg_ibi_status_buf_thld : R/W; bitpos: [20:18]; default: 1;
         *  NA
         */
        uint32_t reg_ibi_status_buf_thld:3;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} i3c_mst_buffer_thld_ctrl_reg_t;


/** Group: I3C DATA BUFFER THLD CTRL REG */
/** Type of data_buffer_thld_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** reg_tx_data_buf_thld : R/W; bitpos: [2:0]; default: 1;
         *  Transmit Buffer Threshold Value. This field controls the number of empty locations
         *  in the Transmit FIFO that trigger the TX_THLD_STAT interrupt. Supports values:
         *  000:2  001:4  010:8  011:16  100:31, else:31
         */
        uint32_t reg_tx_data_buf_thld:3;
        /** reg_rx_data_buf_thld : R/W; bitpos: [5:3]; default: 1;
         *  Receive Buffer Threshold Value. This field controls the number of empty locations
         *  in the Receive FIFO that trigger the RX_THLD_STAT interrupt. Supports: 000:2  001:4
         *  010:8  011:16  100:31, else:31
         */
        uint32_t reg_rx_data_buf_thld:3;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} i3c_mst_data_buffer_thld_ctrl_reg_t;


/** Group: I3C IBI NOTIFY CTRL REG */
/** Type of ibi_notify_ctrl register
 *  NA
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** reg_notify_sir_rejected : R/W; bitpos: [2]; default: 0;
         *  Notify Rejected Slave Interrupt Request Control. This bit is used to suppress
         *  reporting to the application about Slave Interrupt Request. 0:Suppress passing the
         *  IBI Status to the IBI FIFO(hence not notifying the application) when a SIR request
         *  is NACKed and auto-disabled base on the IBI_SIR_REQ_REJECT register. 1: Writes IBI
         *  Status to the IBI FIFO(hence notifying the application) when SIR request is NACKed
         *  and auto-disabled based on the IBI_SIR_REQ_REJECT registerl.
         */
        uint32_t reg_notify_sir_rejected:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} i3c_mst_ibi_notify_ctrl_reg_t;


/** Group: I3C IBI SIR REQ PAYLOAD REG */
/** Type of ibi_sir_req_payload register
 *  NA
 */
typedef union {
    struct {
        /** reg_sir_req_payload : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t reg_sir_req_payload:32;
    };
    uint32_t val;
} i3c_mst_ibi_sir_req_payload_reg_t;


/** Group: I3C IBI SIR REQ REJECT REG */
/** Type of ibi_sir_req_reject register
 *  NA
 */
typedef union {
    struct {
        /** reg_sir_req_reject : R/W; bitpos: [31:0]; default: 0;
         *  The application of controller can decide whether to send ACK or NACK for Slave
         *  request received from any I3C device. A device specific response control bit is
         *  provided to select the response option, Master will ACK/NACK the Master Request
         *  based on programming of control bit, corresponding to the interrupting device.
         *  0:ACK the SIR Request  1:NACK and send direct auto disable CCC
         */
        uint32_t reg_sir_req_reject:32;
    };
    uint32_t val;
} i3c_mst_ibi_sir_req_reject_reg_t;


/** Group: I3C INT CLR REG */
/** Type of int_clr register
 *  NA
 */
typedef union {
    struct {
        /** tx_data_buf_thld_int_clr : WT; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t tx_data_buf_thld_int_clr:1;
        /** rx_data_buf_thld_int_clr : WT; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t rx_data_buf_thld_int_clr:1;
        /** ibi_status_thld_int_clr : WT; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ibi_status_thld_int_clr:1;
        /** cmd_buf_empty_thld_int_clr : WT; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t cmd_buf_empty_thld_int_clr:1;
        /** resp_ready_int_clr : WT; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t resp_ready_int_clr:1;
        /** nxt_cmd_req_err_int_clr : WT; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t nxt_cmd_req_err_int_clr:1;
        /** transfer_err_int_clr : WT; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t transfer_err_int_clr:1;
        /** transfer_complete_int_clr : WT; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t transfer_complete_int_clr:1;
        /** command_done_int_clr : WT; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t command_done_int_clr:1;
        /** detect_start_int_clr : WT; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t detect_start_int_clr:1;
        /** resp_buf_ovf_int_clr : WT; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t resp_buf_ovf_int_clr:1;
        /** ibi_data_buf_ovf_int_clr : WT; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ibi_data_buf_ovf_int_clr:1;
        /** ibi_status_buf_ovf_int_clr : WT; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ibi_status_buf_ovf_int_clr:1;
        /** ibi_handle_done_int_clr : WT; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ibi_handle_done_int_clr:1;
        /** ibi_detect_int_clr : WT; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ibi_detect_int_clr:1;
        /** cmd_ccc_mismatch_int_clr : WT; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t cmd_ccc_mismatch_int_clr:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} i3c_mst_int_clr_reg_t;


/** Group: I3C INT RAW REG */
/** Type of int_raw register
 *  NA
 */
typedef union {
    struct {
        /** tx_data_buf_thld_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t tx_data_buf_thld_int_raw:1;
        /** rx_data_buf_thld_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t rx_data_buf_thld_int_raw:1;
        /** ibi_status_thld_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t ibi_status_thld_int_raw:1;
        /** cmd_buf_empty_thld_int_raw : R/WTC/SS; bitpos: [3]; default: 1;
         *  NA
         */
        uint32_t cmd_buf_empty_thld_int_raw:1;
        /** resp_ready_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t resp_ready_int_raw:1;
        /** nxt_cmd_req_err_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t nxt_cmd_req_err_int_raw:1;
        /** transfer_err_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t transfer_err_int_raw:1;
        /** transfer_complete_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t transfer_complete_int_raw:1;
        /** command_done_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t command_done_int_raw:1;
        /** detect_start_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t detect_start_int_raw:1;
        /** resp_buf_ovf_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t resp_buf_ovf_int_raw:1;
        /** ibi_data_buf_ovf_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ibi_data_buf_ovf_int_raw:1;
        /** ibi_status_buf_ovf_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ibi_status_buf_ovf_int_raw:1;
        /** ibi_handle_done_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ibi_handle_done_int_raw:1;
        /** ibi_detect_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ibi_detect_int_raw:1;
        /** cmd_ccc_mismatch_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t cmd_ccc_mismatch_int_raw:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} i3c_mst_int_raw_reg_t;


/** Group: I3C INT ST REG */
/** Type of int_st register
 *  NA
 */
typedef union {
    struct {
        /** tx_data_buf_thld_int_st : RO; bitpos: [0]; default: 0;
         *  This interrupt is generated when number of empty locations in transmit buffer is
         *  greater than or equal to threshold value specified by TX_EMPTY_BUS_THLD field in
         *  DATA_BUFFER_THLD_CTRL register. This interrupt will be cleared automatically when
         *  number of empty locations in transmit buffer is less than threshold value.
         */
        uint32_t tx_data_buf_thld_int_st:1;
        /** rx_data_buf_thld_int_st : RO; bitpos: [1]; default: 0;
         *  This interrupt is generated when number of entries in receive buffer is greater
         *  than or equal to threshold value specified by RX_BUF_THLD field in
         *  DATA_BUFFER_THLD_CTRL register. This interrupt will be cleared automatically when
         *  number of entries in receive buffer is less than threshold value.
         */
        uint32_t rx_data_buf_thld_int_st:1;
        /** ibi_status_thld_int_st : RO; bitpos: [2]; default: 0;
         *  Only used in master mode. This interrupt is generated when number of entries in IBI
         *  buffer is greater than or equal to threshold value specified by IBI_BUF_THLD field
         *  in BUFFER_THLD_CTRL register. This interrupt will be cleared automatically when
         *  number of entries in IBI buffer is less than threshold value.
         */
        uint32_t ibi_status_thld_int_st:1;
        /** cmd_buf_empty_thld_int_st : RO; bitpos: [3]; default: 0;
         *  This interrupt is generated when number of empty locations in command buffer is
         *  greater than or equal to threshold value specified by CMD_EMPTY_BUF_THLD field in
         *  BUFFER_THLD_CTRL register. This interrupt will be cleared automatically when number
         *  of empty locations in command buffer is less than threshold value.
         */
        uint32_t cmd_buf_empty_thld_int_st:1;
        /** resp_ready_int_st : RO; bitpos: [4]; default: 0;
         *  This interrupt is generated when number of entries in response buffer is greater
         *  than or equal to threshold value specified by RESP_BUF_THLD field in
         *  BUFFER_THLD_CTRL register. This interrupt will be cleared automatically when number
         *  of entries in response buffer is less than threshold value.
         */
        uint32_t resp_ready_int_st:1;
        /** nxt_cmd_req_err_int_st : RO; bitpos: [5]; default: 0;
         *  This interrupt is generated if toc is 0(master will restart next command), but
         *  command buf is empty.
         */
        uint32_t nxt_cmd_req_err_int_st:1;
        /** transfer_err_int_st : RO; bitpos: [6]; default: 0;
         *  This interrupt is generated if any error occurs during transfer. The error type
         *  will be specified in the response packet associated with the command (in ERR_STATUS
         *  field of RESPONSE_BUFFER_PORT register). This bit can be cleared by writing 1'h1.
         */
        uint32_t transfer_err_int_st:1;
        /** transfer_complete_int_st : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t transfer_complete_int_st:1;
        /** command_done_int_st : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t command_done_int_st:1;
        /** detect_start_int_st : RO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t detect_start_int_st:1;
        /** resp_buf_ovf_int_st : RO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t resp_buf_ovf_int_st:1;
        /** ibi_data_buf_ovf_int_st : RO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ibi_data_buf_ovf_int_st:1;
        /** ibi_status_buf_ovf_int_st : RO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ibi_status_buf_ovf_int_st:1;
        /** ibi_handle_done_int_st : RO; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ibi_handle_done_int_st:1;
        /** ibi_detect_int_st : RO; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ibi_detect_int_st:1;
        /** cmd_ccc_mismatch_int_st : RO; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t cmd_ccc_mismatch_int_st:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} i3c_mst_int_st_reg_t;


/** Group: I3C INT ST ENA REG */
/** Type of int_st_ena register
 *  The Interrupt status will be updated in INTR_STATUS register if corresponding
 *  Status Enable bit set.
 */
typedef union {
    struct {
        /** tx_data_buf_thld_int_ena : R/W; bitpos: [0]; default: 0;
         *  Transmit Buffer threshold status enable.
         */
        uint32_t tx_data_buf_thld_int_ena:1;
        /** rx_data_buf_thld_int_ena : R/W; bitpos: [1]; default: 0;
         *  Receive Buffer threshold status enable.
         */
        uint32_t rx_data_buf_thld_int_ena:1;
        /** ibi_status_thld_int_ena : R/W; bitpos: [2]; default: 0;
         *  Only used in master mode. IBI Buffer threshold status enable.
         */
        uint32_t ibi_status_thld_int_ena:1;
        /** cmd_buf_empty_thld_int_ena : R/W; bitpos: [3]; default: 0;
         *  Command buffer ready status enable.
         */
        uint32_t cmd_buf_empty_thld_int_ena:1;
        /** resp_ready_int_ena : R/W; bitpos: [4]; default: 0;
         *  Response buffer ready status enable.
         */
        uint32_t resp_ready_int_ena:1;
        /** nxt_cmd_req_err_int_ena : R/W; bitpos: [5]; default: 0;
         *  next command request error status enable
         */
        uint32_t nxt_cmd_req_err_int_ena:1;
        /** transfer_err_int_ena : R/W; bitpos: [6]; default: 0;
         *  Transfer error status enable
         */
        uint32_t transfer_err_int_ena:1;
        /** transfer_complete_int_ena : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t transfer_complete_int_ena:1;
        /** command_done_int_ena : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t command_done_int_ena:1;
        /** detect_start_int_ena : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t detect_start_int_ena:1;
        /** resp_buf_ovf_int_ena : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t resp_buf_ovf_int_ena:1;
        /** ibi_data_buf_ovf_int_ena : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t ibi_data_buf_ovf_int_ena:1;
        /** ibi_status_buf_ovf_int_ena : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t ibi_status_buf_ovf_int_ena:1;
        /** ibi_handle_done_int_ena : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t ibi_handle_done_int_ena:1;
        /** ibi_detect_int_ena : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t ibi_detect_int_ena:1;
        /** cmd_ccc_mismatch_int_ena : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t cmd_ccc_mismatch_int_ena:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} i3c_mst_int_st_ena_reg_t;


/** Group: I3C RESET CTRL REG */
/** Type of reset_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** reg_core_soft_rst : WT; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_core_soft_rst:1;
        /** reg_cmd_buf_rst : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_cmd_buf_rst:1;
        /** reg_resp_buf_rst : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t reg_resp_buf_rst:1;
        /** reg_tx_data_buf_buf_rst : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t reg_tx_data_buf_buf_rst:1;
        /** reg_rx_data_buf_rst : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t reg_rx_data_buf_rst:1;
        /** reg_ibi_data_buf_rst : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t reg_ibi_data_buf_rst:1;
        /** reg_ibi_status_buf_rst : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t reg_ibi_status_buf_rst:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} i3c_mst_reset_ctrl_reg_t;


/** Group: I3C BUFFER STATUS LEVEL REG */
/** Type of buffer_status_level register
 *  BUFFER_STATUS_LEVEL reflects the status level of Buffers in the controller.
 */
typedef union {
    struct {
        /** cmd_buf_empty_cnt : RO; bitpos: [4:0]; default: 16;
         *  Command Buffer Empty Locations contains the number of empty locations in the
         *  command buffer.
         */
        uint32_t cmd_buf_empty_cnt:5;
        uint32_t reserved_5:3;
        /** resp_buf_cnt : RO; bitpos: [11:8]; default: 0;
         *  Response Buffer Level Value contains the number of valid data entries in the
         *  response buffer.
         */
        uint32_t resp_buf_cnt:4;
        uint32_t reserved_12:4;
        /** ibi_data_buf_cnt : RO; bitpos: [19:16]; default: 0;
         *  IBI Buffer Level Value contains the number of valid entries in the IBI Buffer. This
         *  is field is used in master mode.
         */
        uint32_t ibi_data_buf_cnt:4;
        uint32_t reserved_20:4;
        /** ibi_status_buf_cnt : RO; bitpos: [27:24]; default: 0;
         *  IBI Buffer Status Count contains the number of IBI status entries in the IBI
         *  Buffer. This field is used in master mode.
         */
        uint32_t ibi_status_buf_cnt:4;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} i3c_mst_buffer_status_level_reg_t;


/** Group: I3C DATA BUFFER STATUS LEVEL REG */
/** Type of data_buffer_status_level register
 *  DATA_BUFFER_STATUS_LEVEL reflects the status level of the Buffers in the controller.
 */
typedef union {
    struct {
        /** tx_data_buf_empty_cnt : RO; bitpos: [5:0]; default: 32;
         *  Transmit Buffer Empty Level Value contains the number of empty locations in the
         *  transmit Buffer.
         */
        uint32_t tx_data_buf_empty_cnt:6;
        uint32_t reserved_6:10;
        /** rx_data_buf_cnt : RO; bitpos: [21:16]; default: 0;
         *  Receive Buffer Level value contains the number of valid data entries in the receive
         *  buffer.
         */
        uint32_t rx_data_buf_cnt:6;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} i3c_mst_data_buffer_status_level_reg_t;


/** Group: I3C PRESENT STATE0 REG */
/** Type of present_state0 register
 *  NA
 */
typedef union {
    struct {
        /** sda_lvl : RO; bitpos: [0]; default: 1;
         *  This bit is used to check the SCL line level to recover from error and  for
         *  debugging. This bit reflects the value of synchronized scl_in_a.
         */
        uint32_t sda_lvl:1;
        /** scl_lvl : RO; bitpos: [1]; default: 1;
         *  This bit is used to check the SDA line level to recover from error and  for
         *  debugging. This bit reflects the value of synchronized sda_in_a.
         */
        uint32_t scl_lvl:1;
        /** bus_busy : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t bus_busy:1;
        /** bus_free : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t bus_free:1;
        uint32_t reserved_4:5;
        /** cmd_tid : RO; bitpos: [12:9]; default: 0;
         *  NA
         */
        uint32_t cmd_tid:4;
        /** scl_gen_fsm_state : RO; bitpos: [15:13]; default: 0;
         *  NA
         */
        uint32_t scl_gen_fsm_state:3;
        /** ibi_ev_handle_fsm_state : RO; bitpos: [18:16]; default: 0;
         *  NA
         */
        uint32_t ibi_ev_handle_fsm_state:3;
        /** i2c_mode_fsm_state : RO; bitpos: [21:19]; default: 0;
         *  NA
         */
        uint32_t i2c_mode_fsm_state:3;
        /** sdr_mode_fsm_state : RO; bitpos: [25:22]; default: 0;
         *  NA
         */
        uint32_t sdr_mode_fsm_state:4;
        /** daa_mode_fsm_state : RO; bitpos: [28:26]; default: 0;
         *  Reflects whether the Master Controller is in IDLE or not. This bit will be set when
         *  all the buffer(Command, Response, IBI, Transmit, Receive) are empty along with the
         *  Master State machine is in idle state. 0X0: not in idle  0x1: in idle
         */
        uint32_t daa_mode_fsm_state:3;
        /** main_fsm_state : RO; bitpos: [31:29]; default: 0;
         *  NA
         */
        uint32_t main_fsm_state:3;
    };
    uint32_t val;
} i3c_mst_present_state0_reg_t;


/** Group: I3C PRESENT STATE1 REG */
/** Type of present_state1 register
 *  NA
 */
typedef union {
    struct {
        /** data_byte_cnt : RO; bitpos: [15:0]; default: 0;
         *  Present transfer data byte cnt: tx data byte cnt if write  rx data byte cnt if read
         *  ibi data byte cnt if IBI handle.
         */
        uint32_t data_byte_cnt:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} i3c_mst_present_state1_reg_t;


/** Group: I3C DEVICE TABLE REG */
/** Type of device_table register
 *  Pointer for Device Address Table
 */
typedef union {
    struct {
        /** reg_dct_daa_init_index : R/W; bitpos: [3:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_dct_daa_init_index:4;
        /** reg_dat_daa_init_index : R/W; bitpos: [7:4]; default: 0;
         *  NA
         */
        uint32_t reg_dat_daa_init_index:4;
        /** present_dct_index : RO; bitpos: [11:8]; default: 0;
         *  NA
         */
        uint32_t present_dct_index:4;
        /** present_dat_index : RO; bitpos: [15:12]; default: 0;
         *  NA
         */
        uint32_t present_dat_index:4;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} i3c_mst_device_table_reg_t;


/** Group: I3C TIME OUT VALUE REG */
/** Type of time_out_value register
 *  NA
 */
typedef union {
    struct {
        /** reg_resp_buf_to_value : R/W; bitpos: [4:0]; default: 16;
         *  NA
         */
        uint32_t reg_resp_buf_to_value:5;
        /** reg_resp_buf_to_en : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t reg_resp_buf_to_en:1;
        /** reg_ibi_data_buf_to_value : R/W; bitpos: [10:6]; default: 16;
         *  NA
         */
        uint32_t reg_ibi_data_buf_to_value:5;
        /** reg_ibi_data_buf_to_en : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t reg_ibi_data_buf_to_en:1;
        /** reg_ibi_status_buf_to_value : R/W; bitpos: [16:12]; default: 16;
         *  NA
         */
        uint32_t reg_ibi_status_buf_to_value:5;
        /** reg_ibi_status_buf_to_en : R/W; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t reg_ibi_status_buf_to_en:1;
        /** reg_rx_data_buf_to_value : R/W; bitpos: [22:18]; default: 16;
         *  NA
         */
        uint32_t reg_rx_data_buf_to_value:5;
        /** reg_rx_data_buf_to_en : R/W; bitpos: [23]; default: 0;
         *  NA
         */
        uint32_t reg_rx_data_buf_to_en:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} i3c_mst_time_out_value_reg_t;


/** Group: I3C SCL I3C OD TIME REG */
/** Type of scl_i3c_mst_od_time register
 *  NA
 */
typedef union {
    struct {
        /** reg_i3c_mst_od_low_period : R/W; bitpos: [15:0]; default: 25;
         *  SCL Open-Drain low count for I3C transfers targeted to I3C devices.
         */
        uint32_t reg_i3c_mst_od_low_period:16;
        /** reg_i3c_mst_od_high_period : R/W; bitpos: [31:16]; default: 5;
         *  SCL Open-Drain High count for I3C transfers targeted to I3C devices.
         */
        uint32_t reg_i3c_mst_od_high_period:16;
    };
    uint32_t val;
} i3c_mst_scl_i3c_mst_od_time_reg_t;


/** Group: I3C SCL I3C PP TIME REG */
/** Type of scl_i3c_mst_pp_time register
 *  NA
 */
typedef union {
    struct {
        /** reg_i3c_mst_pp_low_period : R/W; bitpos: [7:0]; default: 5;
         *  NA
         */
        uint32_t reg_i3c_mst_pp_low_period:8;
        uint32_t reserved_8:8;
        /** reg_i3c_mst_pp_high_period : R/W; bitpos: [23:16]; default: 5;
         *  NA
         */
        uint32_t reg_i3c_mst_pp_high_period:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} i3c_mst_scl_i3c_mst_pp_time_reg_t;


/** Group: I3C SCL I2C FM TIME REG */
/** Type of scl_i2c_fm_time register
 *  NA
 */
typedef union {
    struct {
        /** reg_i2c_fm_low_period : R/W; bitpos: [15:0]; default: 163;
         *  NA
         */
        uint32_t reg_i2c_fm_low_period:16;
        /** reg_i2c_fm_high_period : R/W; bitpos: [31:16]; default: 75;
         *  The SCL open-drain low count timing for I2C Fast Mode transfers.
         */
        uint32_t reg_i2c_fm_high_period:16;
    };
    uint32_t val;
} i3c_mst_scl_i2c_fm_time_reg_t;


/** Group: I3C SCL I2C FMP TIME REG */
/** Type of scl_i2c_fmp_time register
 *  NA
 */
typedef union {
    struct {
        /** reg_i2c_fmp_low_period : R/W; bitpos: [15:0]; default: 63;
         *  NA
         */
        uint32_t reg_i2c_fmp_low_period:16;
        /** reg_i2c_fmp_high_period : R/W; bitpos: [23:16]; default: 33;
         *  NA
         */
        uint32_t reg_i2c_fmp_high_period:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} i3c_mst_scl_i2c_fmp_time_reg_t;


/** Group: I3C SCL EXT LOW TIME REG */
/** Type of scl_ext_low_time register
 *  NA
 */
typedef union {
    struct {
        /** reg_i3c_mst_ext_low_period1 : R/W; bitpos: [7:0]; default: 0;
         *  NA
         */
        uint32_t reg_i3c_mst_ext_low_period1:8;
        /** reg_i3c_mst_ext_low_period2 : R/W; bitpos: [15:8]; default: 0;
         *  NA
         */
        uint32_t reg_i3c_mst_ext_low_period2:8;
        /** reg_i3c_mst_ext_low_period3 : R/W; bitpos: [23:16]; default: 0;
         *  NA
         */
        uint32_t reg_i3c_mst_ext_low_period3:8;
        /** reg_i3c_mst_ext_low_period4 : R/W; bitpos: [31:24]; default: 0;
         *  NA
         */
        uint32_t reg_i3c_mst_ext_low_period4:8;
    };
    uint32_t val;
} i3c_mst_scl_ext_low_time_reg_t;


/** Group: I3C SDA SAMPLE TIME REG */
/** Type of sda_sample_time register
 *  NA
 */
typedef union {
    struct {
        /** reg_sda_od_sample_time : R/W; bitpos: [8:0]; default: 0;
         *  It is used to adjust sda sample point when scl high under open drain speed
         */
        uint32_t reg_sda_od_sample_time:9;
        /** reg_sda_pp_sample_time : R/W; bitpos: [13:9]; default: 0;
         *  It is used to adjust sda sample point when scl high under push pull speed
         */
        uint32_t reg_sda_pp_sample_time:5;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} i3c_mst_sda_sample_time_reg_t;


/** Group: I3C SDA HOLD TIME REG */
/** Type of sda_hold_time register
 *  NA
 */
typedef union {
    struct {
        /** reg_sda_od_tx_hold_time : R/W; bitpos: [8:0]; default: 1;
         *  It is used to adjust sda drive point after scl neg under open drain speed
         */
        uint32_t reg_sda_od_tx_hold_time:9;
        /** reg_sda_pp_tx_hold_time : R/W; bitpos: [13:9]; default: 0;
         *  It is used to adjust sda dirve point after scl neg under push pull speed
         */
        uint32_t reg_sda_pp_tx_hold_time:5;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} i3c_mst_sda_hold_time_reg_t;


/** Group: I3C SCL START HOLD REG */
/** Type of scl_start_hold register
 *  NA
 */
typedef union {
    struct {
        /** reg_scl_start_hold_time : R/W; bitpos: [8:0]; default: 8;
         *  I2C_SCL_START_HOLD_TIME
         */
        uint32_t reg_scl_start_hold_time:9;
        /** reg_start_det_hold_time : R/W; bitpos: [10:9]; default: 0;
         *  NA
         */
        uint32_t reg_start_det_hold_time:2;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} i3c_mst_scl_start_hold_reg_t;


/** Group: I3C SCL RSTART SETUP REG */
/** Type of scl_rstart_setup register
 *  NA
 */
typedef union {
    struct {
        /** reg_scl_rstart_setup_time : R/W; bitpos: [8:0]; default: 8;
         *  I2C_SCL_RSTART_SETUP_TIME
         */
        uint32_t reg_scl_rstart_setup_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} i3c_mst_scl_rstart_setup_reg_t;


/** Group: I3C SCL STOP HOLD REG */
/** Type of scl_stop_hold register
 *  NA
 */
typedef union {
    struct {
        /** reg_scl_stop_hold_time : R/W; bitpos: [8:0]; default: 8;
         *  I2C_SCL_STOP_HOLD_TIME
         */
        uint32_t reg_scl_stop_hold_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} i3c_mst_scl_stop_hold_reg_t;


/** Group: I3C SCL STOP SETUP REG */
/** Type of scl_stop_setup register
 *  NA
 */
typedef union {
    struct {
        /** reg_scl_stop_setup_time : R/W; bitpos: [8:0]; default: 8;
         *  I2C_SCL_STOP_SETUP_TIME
         */
        uint32_t reg_scl_stop_setup_time:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} i3c_mst_scl_stop_setup_reg_t;


/** Group: I3C BUS FREE TIME REG */
/** Type of bus_free_time register
 *  NA
 */
typedef union {
    struct {
        /** reg_bus_free_time : R/W; bitpos: [15:0]; default: 5;
         *  I3C Bus Free Count Value. This field is used only in Master mode. In pure Bus
         *  System, this field represents tCAS.  In Mixed Bus System, this field is expected to
         *  be programmed to tLOW of I2C Timing.
         */
        uint32_t reg_bus_free_time:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} i3c_mst_bus_free_time_reg_t;


/** Group: I3C SCL TERMN T EXT LOW TIME REG */
/** Type of scl_termn_t_ext_low_time register
 *  NA
 */
typedef union {
    struct {
        /** reg_i3c_mst_termn_t_ext_low_time : R/W; bitpos: [7:0]; default: 2;
         *  NA
         */
        uint32_t reg_i3c_mst_termn_t_ext_low_time:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} i3c_mst_scl_termn_t_ext_low_time_reg_t;


/** Group: I3C VER ID REG */
/** Type of ver_id register
 *  NA
 */
typedef union {
    struct {
        /** reg_i3c_mst_ver_id : R/W; bitpos: [31:0]; default: 539165956;
         *  This field indicates the controller current release number that is read by an
         *  application.
         */
        uint32_t reg_i3c_mst_ver_id:32;
    };
    uint32_t val;
} i3c_mst_ver_id_reg_t;


/** Group: I3C VER TYPE REG */
/** Type of ver_type register
 *  NA
 */
typedef union {
    struct {
        /** reg_i3c_mst_ver_type : R/W; bitpos: [31:0]; default: 0;
         *  This field indicates the controller current release type that is read by an
         *  application.
         */
        uint32_t reg_i3c_mst_ver_type:32;
    };
    uint32_t val;
} i3c_mst_ver_type_reg_t;


/** Group: I3C FPGA DEBUG PROBE REG */
/** Type of fpga_debug_probe register
 *  NA
 */
typedef union {
    struct {
        /** reg_i3c_mst_fpga_debug_probe : R/W; bitpos: [31:0]; default: 1;
         *  For Debug Probe Test on FPGA
         */
        uint32_t reg_i3c_mst_fpga_debug_probe:32;
    };
    uint32_t val;
} i3c_mst_fpga_debug_probe_reg_t;


/** Group: I3C RND ECO CS REG */
/** Type of rnd_eco_cs register
 *  NA
 */
typedef union {
    struct {
        /** reg_rnd_eco_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_rnd_eco_en:1;
        /** rnd_eco_result : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t rnd_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} i3c_mst_rnd_eco_cs_reg_t;


/** Group: I3C RND ECO LOW REG */
/** Type of rnd_eco_low register
 *  NA
 */
typedef union {
    struct {
        /** reg_rnd_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t reg_rnd_eco_low:32;
    };
    uint32_t val;
} i3c_mst_rnd_eco_low_reg_t;


/** Group: I3C RND ECO HIGH REG */
/** Type of rnd_eco_high register
 *  NA
 */
typedef union {
    struct {
        /** reg_rnd_eco_high : R/W; bitpos: [31:0]; default: 65535;
         *  NA
         */
        uint32_t reg_rnd_eco_high:32;
    };
    uint32_t val;
} i3c_mst_rnd_eco_high_reg_t;


typedef struct {
    volatile i3c_mst_device_ctrl_reg_t device_ctrl;
    uint32_t reserved_004[6];
    volatile i3c_mst_buffer_thld_ctrl_reg_t buffer_thld_ctrl;
    volatile i3c_mst_data_buffer_thld_ctrl_reg_t data_buffer_thld_ctrl;
    volatile i3c_mst_ibi_notify_ctrl_reg_t ibi_notify_ctrl;
    volatile i3c_mst_ibi_sir_req_payload_reg_t ibi_sir_req_payload;
    volatile i3c_mst_ibi_sir_req_reject_reg_t ibi_sir_req_reject;
    volatile i3c_mst_int_clr_reg_t int_clr;
    volatile i3c_mst_int_raw_reg_t int_raw;
    volatile i3c_mst_int_st_reg_t int_st;
    volatile i3c_mst_int_st_ena_reg_t int_st_ena;
    uint32_t reserved_040;
    volatile i3c_mst_reset_ctrl_reg_t reset_ctrl;
    volatile i3c_mst_buffer_status_level_reg_t buffer_status_level;
    volatile i3c_mst_data_buffer_status_level_reg_t data_buffer_status_level;
    volatile i3c_mst_present_state0_reg_t present_state0;
    volatile i3c_mst_present_state1_reg_t present_state1;
    volatile i3c_mst_device_table_reg_t device_table;
    volatile i3c_mst_time_out_value_reg_t time_out_value;
    volatile i3c_mst_scl_i3c_mst_od_time_reg_t scl_i3c_mst_od_time;
    volatile i3c_mst_scl_i3c_mst_pp_time_reg_t scl_i3c_mst_pp_time;
    volatile i3c_mst_scl_i2c_fm_time_reg_t scl_i2c_fm_time;
    volatile i3c_mst_scl_i2c_fmp_time_reg_t scl_i2c_fmp_time;
    volatile i3c_mst_scl_ext_low_time_reg_t scl_ext_low_time;
    volatile i3c_mst_sda_sample_time_reg_t sda_sample_time;
    volatile i3c_mst_sda_hold_time_reg_t sda_hold_time;
    volatile i3c_mst_scl_start_hold_reg_t scl_start_hold;
    volatile i3c_mst_scl_rstart_setup_reg_t scl_rstart_setup;
    volatile i3c_mst_scl_stop_hold_reg_t scl_stop_hold;
    volatile i3c_mst_scl_stop_setup_reg_t scl_stop_setup;
    uint32_t reserved_08c;
    volatile i3c_mst_bus_free_time_reg_t bus_free_time;
    volatile i3c_mst_scl_termn_t_ext_low_time_reg_t scl_termn_t_ext_low_time;
    uint32_t reserved_098[2];
    volatile i3c_mst_ver_id_reg_t ver_id;
    volatile i3c_mst_ver_type_reg_t ver_type;
    uint32_t reserved_0a8;
    volatile i3c_mst_fpga_debug_probe_reg_t fpga_debug_probe;
    volatile i3c_mst_rnd_eco_cs_reg_t rnd_eco_cs;
    volatile i3c_mst_rnd_eco_low_reg_t rnd_eco_low;
    volatile i3c_mst_rnd_eco_high_reg_t rnd_eco_high;
} i3c_mst_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(i3c_mst_dev_t) == 0xbc, "Invalid size of i3c_mst_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
