/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of ep1 register
 *  FIFO access for the CDC-ACM data IN and OUT endpoints.
 */
typedef union {
    struct {
        /** rdwr_byte : R/W; bitpos: [7:0]; default: 0;
         *  Write and read byte data to/from UART Tx/Rx FIFO through this field. When
         *  USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT is set, then user can write data (up to 64
         *  bytes) into UART Tx FIFO. When USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT is set, user
         *  can check USB_SERIAL_JTAG_OUT_EP1_WR_ADDR USB_SERIAL_JTAG_OUT_EP0_RD_ADDR to know
         *  how many data is received, then read data from UART Rx FIFO.
         */
        uint32_t rdwr_byte:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} usb_serial_jtag_ep1_reg_t;

/** Type of ep1_conf register
 *  Configuration and control registers for the CDC-ACM FIFOs.
 */
typedef union {
    struct {
        /** wr_done : WT; bitpos: [0]; default: 0;
         *  Set this bit to indicate writing byte data to UART Tx FIFO is done.
         */
        uint32_t wr_done:1;
        /** serial_in_ep_data_free : RO; bitpos: [1]; default: 1;
         *  1'b1: Indicate UART Tx FIFO is not full and can write data into in. After writing
         *  USB_SERIAL_JTAG_WR_DONE, this bit would be 0 until data in UART Tx FIFO is read by
         *  USB Host.
         */
        uint32_t serial_in_ep_data_free:1;
        /** serial_out_ep_data_avail : RO; bitpos: [2]; default: 0;
         *  1'b1: Indicate there is data in UART Rx FIFO.
         */
        uint32_t serial_out_ep_data_avail:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} usb_serial_jtag_ep1_conf_reg_t;

/** Type of conf0 register
 *  PHY hardware configuration.
 */
typedef union {
    struct {
        /** phy_sel : R/W; bitpos: [0]; default: 0;
         *  Select internal/external PHY
         */
        uint32_t phy_sel:1;
        /** exchg_pins_override : R/W; bitpos: [1]; default: 0;
         *  Enable software control USB D+ D- exchange
         */
        uint32_t exchg_pins_override:1;
        /** exchg_pins : R/W; bitpos: [2]; default: 0;
         *  USB D+ D- exchange
         */
        uint32_t exchg_pins:1;
        /** vrefh : R/W; bitpos: [4:3]; default: 0;
         *  Control single-end input high threshold,1.76V to 2V, step 80mV
         */
        uint32_t vrefh:2;
        /** vrefl : R/W; bitpos: [6:5]; default: 0;
         *  Control single-end input low threshold,0.8V to 1.04V, step 80mV
         */
        uint32_t vrefl:2;
        /** vref_override : R/W; bitpos: [7]; default: 0;
         *  Enable software control input  threshold
         */
        uint32_t vref_override:1;
        /** pad_pull_override : R/W; bitpos: [8]; default: 0;
         *  Enable software control USB D+ D- pullup pulldown
         */
        uint32_t pad_pull_override:1;
        /** dp_pullup : R/W; bitpos: [9]; default: 1;
         *  Control USB D+ pull up.
         */
        uint32_t dp_pullup:1;
        /** dp_pulldown : R/W; bitpos: [10]; default: 0;
         *  Control USB D+ pull down.
         */
        uint32_t dp_pulldown:1;
        /** dm_pullup : R/W; bitpos: [11]; default: 0;
         *  Control USB D- pull up.
         */
        uint32_t dm_pullup:1;
        /** dm_pulldown : R/W; bitpos: [12]; default: 0;
         *  Control USB D- pull down.
         */
        uint32_t dm_pulldown:1;
        /** pullup_value : R/W; bitpos: [13]; default: 0;
         *  Control pull up value.
         */
        uint32_t pullup_value:1;
        /** usb_pad_enable : R/W; bitpos: [14]; default: 1;
         *  Enable USB pad function.
         */
        uint32_t usb_pad_enable:1;
        /** usb_jtag_bridge_en : R/W; bitpos: [15]; default: 0;
         *  Set this bit usb_jtag, the connection between usb_jtag and internal JTAG is
         *  disconnected, and MTMS, MTDI, MTCK are output through GPIO Matrix, MTDO is input
         *  through GPIO Matrix.
         */
        uint32_t usb_jtag_bridge_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_serial_jtag_conf0_reg_t;

/** Type of test register
 *  Registers used for debugging the PHY.
 */
typedef union {
    struct {
        /** test_enable : R/W; bitpos: [0]; default: 0;
         *  Enable test of the USB pad
         */
        uint32_t test_enable:1;
        /** test_usb_oe : R/W; bitpos: [1]; default: 0;
         *  USB pad oen in test
         */
        uint32_t test_usb_oe:1;
        /** test_tx_dp : R/W; bitpos: [2]; default: 0;
         *  USB D+ tx value in test
         */
        uint32_t test_tx_dp:1;
        /** test_tx_dm : R/W; bitpos: [3]; default: 0;
         *  USB D- tx value in test
         */
        uint32_t test_tx_dm:1;
        /** test_rx_rcv : RO; bitpos: [4]; default: 1;
         *  USB RCV value in test
         */
        uint32_t test_rx_rcv:1;
        /** test_rx_dp : RO; bitpos: [5]; default: 1;
         *  USB D+ rx value in test
         */
        uint32_t test_rx_dp:1;
        /** test_rx_dm : RO; bitpos: [6]; default: 0;
         *  USB D- rx value in test
         */
        uint32_t test_rx_dm:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} usb_serial_jtag_test_reg_t;

/** Type of misc_conf register
 *  Clock enable control
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  1'h1: Force clock on for register. 1'h0: Support clock only when application writes
         *  registers.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} usb_serial_jtag_misc_conf_reg_t;

/** Type of mem_conf register
 *  Memory power control
 */
typedef union {
    struct {
        /** usb_mem_pd : R/W; bitpos: [0]; default: 0;
         *  1: power down usb memory.
         */
        uint32_t usb_mem_pd:1;
        /** usb_mem_clk_en : R/W; bitpos: [1]; default: 1;
         *  1: Force clock on for usb memory.
         */
        uint32_t usb_mem_clk_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} usb_serial_jtag_mem_conf_reg_t;

/** Type of chip_rst register
 *  CDC-ACM chip reset control.
 */
typedef union {
    struct {
        /** rts : RO; bitpos: [0]; default: 0;
         *  1: Chip reset is detected from usb serial channel. Software write 1 to clear it.
         */
        uint32_t rts:1;
        /** dtr : RO; bitpos: [1]; default: 0;
         *  1: Chip reset is detected from usb jtag channel. Software write 1 to clear it.
         */
        uint32_t dtr:1;
        /** usb_uart_chip_rst_dis : R/W; bitpos: [2]; default: 0;
         *  Set this bit to disable chip reset from usb serial channel to reset chip.
         */
        uint32_t usb_uart_chip_rst_dis:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} usb_serial_jtag_chip_rst_reg_t;

/** Type of get_line_code_w0 register
 *  W0 of GET_LINE_CODING command.
 */
typedef union {
    struct {
        /** get_dw_dte_rate : R/W; bitpos: [31:0]; default: 0;
         *  The value of dwDTERate set by software which is requested by GET_LINE_CODING
         *  command.
         */
        uint32_t get_dw_dte_rate:32;
    };
    uint32_t val;
} usb_serial_jtag_get_line_code_w0_reg_t;

/** Type of get_line_code_w1 register
 *  W1 of GET_LINE_CODING command.
 */
typedef union {
    struct {
        /** get_bdata_bits : R/W; bitpos: [7:0]; default: 0;
         *  The value of bCharFormat set by software which is requested by GET_LINE_CODING
         *  command.
         */
        uint32_t get_bdata_bits:8;
        /** get_bparity_type : R/W; bitpos: [15:8]; default: 0;
         *  The value of bParityTpye set by software which is requested by GET_LINE_CODING
         *  command.
         */
        uint32_t get_bparity_type:8;
        /** get_bchar_format : R/W; bitpos: [23:16]; default: 0;
         *  The value of bDataBits set by software which is requested by GET_LINE_CODING
         *  command.
         */
        uint32_t get_bchar_format:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} usb_serial_jtag_get_line_code_w1_reg_t;

/** Type of config_update register
 *  Configuration registers' value update
 */
typedef union {
    struct {
        /** config_update : WT; bitpos: [0]; default: 0;
         *  Write 1 to this register would update the value of configure registers from APB
         *  clock domain to 48MHz clock domain.
         */
        uint32_t config_update:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} usb_serial_jtag_config_update_reg_t;

/** Type of ser_afifo_config register
 *  Serial AFIFO configure register
 */
typedef union {
    struct {
        /** serial_in_afifo_reset_wr : R/W; bitpos: [0]; default: 0;
         *  Write 1 to reset CDC_ACM IN async FIFO write clock domain.
         */
        uint32_t serial_in_afifo_reset_wr:1;
        /** serial_in_afifo_reset_rd : R/W; bitpos: [1]; default: 0;
         *  Write 1 to reset CDC_ACM IN async FIFO read clock domain.
         */
        uint32_t serial_in_afifo_reset_rd:1;
        /** serial_out_afifo_reset_wr : R/W; bitpos: [2]; default: 0;
         *  Write 1 to reset CDC_ACM OUT async FIFO write clock domain.
         */
        uint32_t serial_out_afifo_reset_wr:1;
        /** serial_out_afifo_reset_rd : R/W; bitpos: [3]; default: 0;
         *  Write 1 to reset CDC_ACM OUT async FIFO read clock domain.
         */
        uint32_t serial_out_afifo_reset_rd:1;
        /** serial_out_afifo_rempty : RO; bitpos: [4]; default: 1;
         *  CDC_ACM OUTOUT async FIFO empty signal in read clock domain.
         */
        uint32_t serial_out_afifo_rempty:1;
        /** serial_in_afifo_wfull : RO; bitpos: [5]; default: 0;
         *  CDC_ACM OUT IN async FIFO empty signal in write clock domain.
         */
        uint32_t serial_in_afifo_wfull:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} usb_serial_jtag_ser_afifo_config_reg_t;

/** Type of serial_ep_timeout0 register
 *  USB uart out endpoint timeout configuration.
 */
typedef union {
    struct {
        /** serial_timeout_en : R/W; bitpos: [0]; default: 0;
         *  USB serial out ep timeout enable. When a timeout event occurs, serial out ep buffer
         *  is automatically cleared and reg_serial_timeout_status is asserted.
         */
        uint32_t serial_timeout_en:1;
        /** serial_timeout_status : R/WTC/SS; bitpos: [1]; default: 0;
         *  Serial out ep triggers a timeout event.
         */
        uint32_t serial_timeout_status:1;
        /** serial_timeout_status_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear reg_serial_timeout_status.
         */
        uint32_t serial_timeout_status_clr:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} usb_serial_jtag_serial_ep_timeout0_reg_t;

/** Type of serial_ep_timeout1 register
 *  USB uart out endpoint timeout configuration.
 */
typedef union {
    struct {
        /** serial_timeout_max : R/W; bitpos: [31:0]; default: 4800768;
         *  USB serial out ep timeout max threshold value, indicates the maximum time that
         *  waiting for ESP to take away data in memory. This value is in steps of 20.83ns.
         */
        uint32_t serial_timeout_max:32;
    };
    uint32_t val;
} usb_serial_jtag_serial_ep_timeout1_reg_t;


/** Group: Interrupt Registers */
/** Type of int_raw register
 *  Interrupt raw status register.
 */
typedef union {
    struct {
        /** jtag_in_flush_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when flush cmd is received for IN
         *  endpoint 2 of JTAG.
         */
        uint32_t jtag_in_flush_int_raw:1;
        /** sof_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when SOF frame is received.
         */
        uint32_t sof_int_raw:1;
        /** serial_out_recv_pkt_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when Serial Port OUT Endpoint received
         *  one packet.
         */
        uint32_t serial_out_recv_pkt_int_raw:1;
        /** serial_in_empty_int_raw : R/WTC/SS; bitpos: [3]; default: 1;
         *  The raw interrupt bit turns to high level when Serial Port IN Endpoint is empty.
         */
        uint32_t serial_in_empty_int_raw:1;
        /** pid_err_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt bit turns to high level when pid error is detected.
         */
        uint32_t pid_err_int_raw:1;
        /** crc5_err_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt bit turns to high level when CRC5 error is detected.
         */
        uint32_t crc5_err_int_raw:1;
        /** crc16_err_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt bit turns to high level when CRC16 error is detected.
         */
        uint32_t crc16_err_int_raw:1;
        /** stuff_err_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt bit turns to high level when stuff error is detected.
         */
        uint32_t stuff_err_int_raw:1;
        /** in_token_rec_in_ep1_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when IN token for IN endpoint 1 is
         *  received.
         */
        uint32_t in_token_rec_in_ep1_int_raw:1;
        /** usb_bus_reset_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw interrupt bit turns to high level when usb bus reset is detected.
         */
        uint32_t usb_bus_reset_int_raw:1;
        /** out_ep1_zero_payload_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The raw interrupt bit turns to high level when OUT endpoint 1 received packet with
         *  zero palyload.
         */
        uint32_t out_ep1_zero_payload_int_raw:1;
        /** out_ep2_zero_payload_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw interrupt bit turns to high level when OUT endpoint 2 received packet with
         *  zero palyload.
         */
        uint32_t out_ep2_zero_payload_int_raw:1;
        /** rts_chg_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The raw interrupt bit turns to high level when level of RTS from usb serial channel
         *  is changed.
         */
        uint32_t rts_chg_int_raw:1;
        /** dtr_chg_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  The raw interrupt bit turns to high level when level of DTR from usb serial channel
         *  is changed.
         */
        uint32_t dtr_chg_int_raw:1;
        /** get_line_code_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  The raw interrupt bit turns to high level when level of GET LINE CODING request is
         *  received.
         */
        uint32_t get_line_code_int_raw:1;
        /** set_line_code_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  The raw interrupt bit turns to high level when level of SET LINE CODING request is
         *  received.
         */
        uint32_t set_line_code_int_raw:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_serial_jtag_int_raw_reg_t;

/** Type of int_st register
 *  Interrupt status register.
 */
typedef union {
    struct {
        /** jtag_in_flush_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT interrupt.
         */
        uint32_t jtag_in_flush_int_st:1;
        /** sof_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_SOF_INT interrupt.
         */
        uint32_t sof_int_st:1;
        /** serial_out_recv_pkt_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT
         *  interrupt.
         */
        uint32_t serial_out_recv_pkt_int_st:1;
        /** serial_in_empty_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT interrupt.
         */
        uint32_t serial_in_empty_int_st:1;
        /** pid_err_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_PID_ERR_INT interrupt.
         */
        uint32_t pid_err_int_st:1;
        /** crc5_err_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_CRC5_ERR_INT interrupt.
         */
        uint32_t crc5_err_int_st:1;
        /** crc16_err_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_CRC16_ERR_INT interrupt.
         */
        uint32_t crc16_err_int_st:1;
        /** stuff_err_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_STUFF_ERR_INT interrupt.
         */
        uint32_t stuff_err_int_st:1;
        /** in_token_rec_in_ep1_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT
         *  interrupt.
         */
        uint32_t in_token_rec_in_ep1_int_st:1;
        /** usb_bus_reset_int_st : RO; bitpos: [9]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_USB_BUS_RESET_INT interrupt.
         */
        uint32_t usb_bus_reset_int_st:1;
        /** out_ep1_zero_payload_int_st : RO; bitpos: [10]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT
         *  interrupt.
         */
        uint32_t out_ep1_zero_payload_int_st:1;
        /** out_ep2_zero_payload_int_st : RO; bitpos: [11]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT
         *  interrupt.
         */
        uint32_t out_ep2_zero_payload_int_st:1;
        /** rts_chg_int_st : RO; bitpos: [12]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_RTS_CHG_INT interrupt.
         */
        uint32_t rts_chg_int_st:1;
        /** dtr_chg_int_st : RO; bitpos: [13]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_DTR_CHG_INT interrupt.
         */
        uint32_t dtr_chg_int_st:1;
        /** get_line_code_int_st : RO; bitpos: [14]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_GET_LINE_CODE_INT interrupt.
         */
        uint32_t get_line_code_int_st:1;
        /** set_line_code_int_st : RO; bitpos: [15]; default: 0;
         *  The raw interrupt status bit for the USB_SERIAL_JTAG_SET_LINE_CODE_INT interrupt.
         */
        uint32_t set_line_code_int_st:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_serial_jtag_int_st_reg_t;

/** Type of int_ena register
 *  Interrupt enable status register.
 */
typedef union {
    struct {
        /** jtag_in_flush_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT interrupt.
         */
        uint32_t jtag_in_flush_int_ena:1;
        /** sof_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_SOF_INT interrupt.
         */
        uint32_t sof_int_ena:1;
        /** serial_out_recv_pkt_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT interrupt.
         */
        uint32_t serial_out_recv_pkt_int_ena:1;
        /** serial_in_empty_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT interrupt.
         */
        uint32_t serial_in_empty_int_ena:1;
        /** pid_err_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_PID_ERR_INT interrupt.
         */
        uint32_t pid_err_int_ena:1;
        /** crc5_err_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_CRC5_ERR_INT interrupt.
         */
        uint32_t crc5_err_int_ena:1;
        /** crc16_err_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_CRC16_ERR_INT interrupt.
         */
        uint32_t crc16_err_int_ena:1;
        /** stuff_err_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_STUFF_ERR_INT interrupt.
         */
        uint32_t stuff_err_int_ena:1;
        /** in_token_rec_in_ep1_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT interrupt.
         */
        uint32_t in_token_rec_in_ep1_int_ena:1;
        /** usb_bus_reset_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_USB_BUS_RESET_INT interrupt.
         */
        uint32_t usb_bus_reset_int_ena:1;
        /** out_ep1_zero_payload_int_ena : R/W; bitpos: [10]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT interrupt.
         */
        uint32_t out_ep1_zero_payload_int_ena:1;
        /** out_ep2_zero_payload_int_ena : R/W; bitpos: [11]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT interrupt.
         */
        uint32_t out_ep2_zero_payload_int_ena:1;
        /** rts_chg_int_ena : R/W; bitpos: [12]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_RTS_CHG_INT interrupt.
         */
        uint32_t rts_chg_int_ena:1;
        /** dtr_chg_int_ena : R/W; bitpos: [13]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_DTR_CHG_INT interrupt.
         */
        uint32_t dtr_chg_int_ena:1;
        /** get_line_code_int_ena : R/W; bitpos: [14]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_GET_LINE_CODE_INT interrupt.
         */
        uint32_t get_line_code_int_ena:1;
        /** set_line_code_int_ena : R/W; bitpos: [15]; default: 0;
         *  The interrupt enable bit for the USB_SERIAL_JTAG_SET_LINE_CODE_INT interrupt.
         */
        uint32_t set_line_code_int_ena:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_serial_jtag_int_ena_reg_t;

/** Type of int_clr register
 *  Interrupt clear status register.
 */
typedef union {
    struct {
        /** jtag_in_flush_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT interrupt.
         */
        uint32_t jtag_in_flush_int_clr:1;
        /** sof_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_JTAG_SOF_INT interrupt.
         */
        uint32_t sof_int_clr:1;
        /** serial_out_recv_pkt_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT interrupt.
         */
        uint32_t serial_out_recv_pkt_int_clr:1;
        /** serial_in_empty_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT interrupt.
         */
        uint32_t serial_in_empty_int_clr:1;
        /** pid_err_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_PID_ERR_INT interrupt.
         */
        uint32_t pid_err_int_clr:1;
        /** crc5_err_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_CRC5_ERR_INT interrupt.
         */
        uint32_t crc5_err_int_clr:1;
        /** crc16_err_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_CRC16_ERR_INT interrupt.
         */
        uint32_t crc16_err_int_clr:1;
        /** stuff_err_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_STUFF_ERR_INT interrupt.
         */
        uint32_t stuff_err_int_clr:1;
        /** in_token_rec_in_ep1_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_IN_TOKEN_IN_EP1_INT interrupt.
         */
        uint32_t in_token_rec_in_ep1_int_clr:1;
        /** usb_bus_reset_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_USB_BUS_RESET_INT interrupt.
         */
        uint32_t usb_bus_reset_int_clr:1;
        /** out_ep1_zero_payload_int_clr : WT; bitpos: [10]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT interrupt.
         */
        uint32_t out_ep1_zero_payload_int_clr:1;
        /** out_ep2_zero_payload_int_clr : WT; bitpos: [11]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT interrupt.
         */
        uint32_t out_ep2_zero_payload_int_clr:1;
        /** rts_chg_int_clr : WT; bitpos: [12]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_RTS_CHG_INT interrupt.
         */
        uint32_t rts_chg_int_clr:1;
        /** dtr_chg_int_clr : WT; bitpos: [13]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_DTR_CHG_INT interrupt.
         */
        uint32_t dtr_chg_int_clr:1;
        /** get_line_code_int_clr : WT; bitpos: [14]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_GET_LINE_CODE_INT interrupt.
         */
        uint32_t get_line_code_int_clr:1;
        /** set_line_code_int_clr : WT; bitpos: [15]; default: 0;
         *  Set this bit to clear the USB_SERIAL_JTAG_SET_LINE_CODE_INT interrupt.
         */
        uint32_t set_line_code_int_clr:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_serial_jtag_int_clr_reg_t;


/** Group: Status Registers */
/** Type of jfifo_st register
 *  JTAG FIFO status and control registers.
 */
typedef union {
    struct {
        /** in_fifo_cnt : RO; bitpos: [1:0]; default: 0;
         *  JTAT in fifo counter.
         */
        uint32_t in_fifo_cnt:2;
        /** in_fifo_empty : RO; bitpos: [2]; default: 1;
         *  1: JTAG in fifo is empty.
         */
        uint32_t in_fifo_empty:1;
        /** in_fifo_full : RO; bitpos: [3]; default: 0;
         *  1: JTAG in fifo is full.
         */
        uint32_t in_fifo_full:1;
        /** out_fifo_cnt : RO; bitpos: [5:4]; default: 0;
         *  JTAT out fifo counter.
         */
        uint32_t out_fifo_cnt:2;
        /** out_fifo_empty : RO; bitpos: [6]; default: 1;
         *  1: JTAG out fifo is empty.
         */
        uint32_t out_fifo_empty:1;
        /** out_fifo_full : RO; bitpos: [7]; default: 0;
         *  1: JTAG out fifo is full.
         */
        uint32_t out_fifo_full:1;
        /** in_fifo_reset : R/W; bitpos: [8]; default: 0;
         *  Write 1 to reset JTAG in fifo.
         */
        uint32_t in_fifo_reset:1;
        /** out_fifo_reset : R/W; bitpos: [9]; default: 0;
         *  Write 1 to reset JTAG out fifo.
         */
        uint32_t out_fifo_reset:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} usb_serial_jtag_jfifo_st_reg_t;

/** Type of fram_num register
 *  Last received SOF frame index register.
 */
typedef union {
    struct {
        /** sof_frame_index : RO; bitpos: [10:0]; default: 0;
         *  Frame index of received SOF frame.
         */
        uint32_t sof_frame_index:11;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} usb_serial_jtag_fram_num_reg_t;

/** Type of in_ep0_st register
 *  Control IN endpoint status information.
 */
typedef union {
    struct {
        /** in_ep0_state : RO; bitpos: [1:0]; default: 1;
         *  State of IN Endpoint 0.
         */
        uint32_t in_ep0_state:2;
        /** in_ep0_wr_addr : RO; bitpos: [8:2]; default: 0;
         *  Write data address of IN endpoint 0.
         */
        uint32_t in_ep0_wr_addr:7;
        /** in_ep0_rd_addr : RO; bitpos: [15:9]; default: 0;
         *  Read data address of IN endpoint 0.
         */
        uint32_t in_ep0_rd_addr:7;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_serial_jtag_in_ep0_st_reg_t;

/** Type of in_ep1_st register
 *  CDC-ACM IN endpoint status information.
 */
typedef union {
    struct {
        /** in_ep1_state : RO; bitpos: [1:0]; default: 1;
         *  State of IN Endpoint 1.
         */
        uint32_t in_ep1_state:2;
        /** in_ep1_wr_addr : RO; bitpos: [8:2]; default: 0;
         *  Write data address of IN endpoint 1.
         */
        uint32_t in_ep1_wr_addr:7;
        /** in_ep1_rd_addr : RO; bitpos: [15:9]; default: 0;
         *  Read data address of IN endpoint 1.
         */
        uint32_t in_ep1_rd_addr:7;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_serial_jtag_in_ep1_st_reg_t;

/** Type of in_ep2_st register
 *  CDC-ACM interrupt IN endpoint status information.
 */
typedef union {
    struct {
        /** in_ep2_state : RO; bitpos: [1:0]; default: 1;
         *  State of IN Endpoint 2.
         */
        uint32_t in_ep2_state:2;
        /** in_ep2_wr_addr : RO; bitpos: [8:2]; default: 0;
         *  Write data address of IN endpoint 2.
         */
        uint32_t in_ep2_wr_addr:7;
        /** in_ep2_rd_addr : RO; bitpos: [15:9]; default: 0;
         *  Read data address of IN endpoint 2.
         */
        uint32_t in_ep2_rd_addr:7;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_serial_jtag_in_ep2_st_reg_t;

/** Type of in_ep3_st register
 *  JTAG IN endpoint status information.
 */
typedef union {
    struct {
        /** in_ep3_state : RO; bitpos: [1:0]; default: 1;
         *  State of IN Endpoint 3.
         */
        uint32_t in_ep3_state:2;
        /** in_ep3_wr_addr : RO; bitpos: [8:2]; default: 0;
         *  Write data address of IN endpoint 3.
         */
        uint32_t in_ep3_wr_addr:7;
        /** in_ep3_rd_addr : RO; bitpos: [15:9]; default: 0;
         *  Read data address of IN endpoint 3.
         */
        uint32_t in_ep3_rd_addr:7;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_serial_jtag_in_ep3_st_reg_t;

/** Type of out_ep0_st register
 *  Control OUT endpoint status information.
 */
typedef union {
    struct {
        /** out_ep0_state : RO; bitpos: [1:0]; default: 0;
         *  State of OUT Endpoint 0.
         */
        uint32_t out_ep0_state:2;
        /** out_ep0_wr_addr : RO; bitpos: [8:2]; default: 0;
         *  Write data address of OUT endpoint 0. When USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT
         *  is detected, there are USB_SERIAL_JTAG_OUT_EP0_WR_ADDR-2 bytes data in OUT EP0.
         */
        uint32_t out_ep0_wr_addr:7;
        /** out_ep0_rd_addr : RO; bitpos: [15:9]; default: 0;
         *  Read data address of OUT endpoint 0.
         */
        uint32_t out_ep0_rd_addr:7;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_serial_jtag_out_ep0_st_reg_t;

/** Type of out_ep1_st register
 *  CDC-ACM OUT endpoint status information.
 */
typedef union {
    struct {
        /** out_ep1_state : RO; bitpos: [1:0]; default: 0;
         *  State of OUT Endpoint 1.
         */
        uint32_t out_ep1_state:2;
        /** out_ep1_wr_addr : RO; bitpos: [8:2]; default: 0;
         *  Write data address of OUT endpoint 1. When USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT
         *  is detected, there are USB_SERIAL_JTAG_OUT_EP1_WR_ADDR-2 bytes data in OUT EP1.
         */
        uint32_t out_ep1_wr_addr:7;
        /** out_ep1_rd_addr : RO; bitpos: [15:9]; default: 0;
         *  Read data address of OUT endpoint 1.
         */
        uint32_t out_ep1_rd_addr:7;
        /** out_ep1_rec_data_cnt : RO; bitpos: [22:16]; default: 0;
         *  Data count in OUT endpoint 1 when one packet is received.
         */
        uint32_t out_ep1_rec_data_cnt:7;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} usb_serial_jtag_out_ep1_st_reg_t;

/** Type of out_ep2_st register
 *  JTAG OUT endpoint status information.
 */
typedef union {
    struct {
        /** out_ep2_state : RO; bitpos: [1:0]; default: 0;
         *  State of OUT Endpoint 2.
         */
        uint32_t out_ep2_state:2;
        /** out_ep2_wr_addr : RO; bitpos: [8:2]; default: 0;
         *  Write data address of OUT endpoint 2. When USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT
         *  is detected, there are USB_SERIAL_JTAG_OUT_EP2_WR_ADDR-2 bytes data in OUT EP2.
         */
        uint32_t out_ep2_wr_addr:7;
        /** out_ep2_rd_addr : RO; bitpos: [15:9]; default: 0;
         *  Read data address of OUT endpoint 2.
         */
        uint32_t out_ep2_rd_addr:7;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} usb_serial_jtag_out_ep2_st_reg_t;

/** Type of set_line_code_w0 register
 *  W0 of SET_LINE_CODING command.
 */
typedef union {
    struct {
        /** dw_dte_rate : RO; bitpos: [31:0]; default: 0;
         *  The value of dwDTERate set by host through SET_LINE_CODING command.
         */
        uint32_t dw_dte_rate:32;
    };
    uint32_t val;
} usb_serial_jtag_set_line_code_w0_reg_t;

/** Type of set_line_code_w1 register
 *  W1 of SET_LINE_CODING command.
 */
typedef union {
    struct {
        /** bchar_format : RO; bitpos: [7:0]; default: 0;
         *  The value of bCharFormat set by host through SET_LINE_CODING command.
         */
        uint32_t bchar_format:8;
        /** bparity_type : RO; bitpos: [15:8]; default: 0;
         *  The value of bParityTpye set by host through SET_LINE_CODING command.
         */
        uint32_t bparity_type:8;
        /** bdata_bits : RO; bitpos: [23:16]; default: 0;
         *  The value of bDataBits set by host through SET_LINE_CODING command.
         */
        uint32_t bdata_bits:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} usb_serial_jtag_set_line_code_w1_reg_t;

/** Type of bus_reset_st register
 *  USB Bus reset status register
 */
typedef union {
    struct {
        /** usb_bus_reset_st : RO; bitpos: [0]; default: 1;
         *  USB bus reset status. 0: USB-Serial-JTAG is in usb bus reset status. 1: USB bus
         *  reset is released.
         */
        uint32_t usb_bus_reset_st:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} usb_serial_jtag_bus_reset_st_reg_t;


/** Group: Version Registers */
/** Type of date register
 *  Date register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 36770368;
         *  register version.
         */
        uint32_t date:32;
    };
    uint32_t val;
} usb_serial_jtag_date_reg_t;


typedef struct {
    volatile usb_serial_jtag_ep1_reg_t ep1;
    volatile usb_serial_jtag_ep1_conf_reg_t ep1_conf;
    volatile usb_serial_jtag_int_raw_reg_t int_raw;
    volatile usb_serial_jtag_int_st_reg_t int_st;
    volatile usb_serial_jtag_int_ena_reg_t int_ena;
    volatile usb_serial_jtag_int_clr_reg_t int_clr;
    volatile usb_serial_jtag_conf0_reg_t conf0;
    volatile usb_serial_jtag_test_reg_t test;
    volatile usb_serial_jtag_jfifo_st_reg_t jfifo_st;
    volatile usb_serial_jtag_fram_num_reg_t fram_num;
    volatile usb_serial_jtag_in_ep0_st_reg_t in_ep0_st;
    volatile usb_serial_jtag_in_ep1_st_reg_t in_ep1_st;
    volatile usb_serial_jtag_in_ep2_st_reg_t in_ep2_st;
    volatile usb_serial_jtag_in_ep3_st_reg_t in_ep3_st;
    volatile usb_serial_jtag_out_ep0_st_reg_t out_ep0_st;
    volatile usb_serial_jtag_out_ep1_st_reg_t out_ep1_st;
    volatile usb_serial_jtag_out_ep2_st_reg_t out_ep2_st;
    volatile usb_serial_jtag_misc_conf_reg_t misc_conf;
    volatile usb_serial_jtag_mem_conf_reg_t mem_conf;
    volatile usb_serial_jtag_chip_rst_reg_t chip_rst;
    volatile usb_serial_jtag_set_line_code_w0_reg_t set_line_code_w0;
    volatile usb_serial_jtag_set_line_code_w1_reg_t set_line_code_w1;
    volatile usb_serial_jtag_get_line_code_w0_reg_t get_line_code_w0;
    volatile usb_serial_jtag_get_line_code_w1_reg_t get_line_code_w1;
    volatile usb_serial_jtag_config_update_reg_t config_update;
    volatile usb_serial_jtag_ser_afifo_config_reg_t ser_afifo_config;
    volatile usb_serial_jtag_bus_reset_st_reg_t bus_reset_st;
    volatile usb_serial_jtag_serial_ep_timeout0_reg_t serial_ep_timeout0;
    volatile usb_serial_jtag_serial_ep_timeout1_reg_t serial_ep_timeout1;
    uint32_t reserved_074[3];
    volatile usb_serial_jtag_date_reg_t date;
} usb_serial_jtag_dev_t;

extern usb_serial_jtag_dev_t USB_SERIAL_JTAG;

#ifndef __cplusplus
_Static_assert(sizeof(usb_serial_jtag_dev_t) == 0x84, "Invalid size of usb_serial_jtag_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
