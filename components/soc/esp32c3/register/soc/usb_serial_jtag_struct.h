/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_USB_SERIAL_JTAG_STRUCT_H_
#define _SOC_USB_SERIAL_JTAG_STRUCT_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc/soc.h"

typedef volatile struct usb_serial_jtag_dev_s {
    union {
        struct {
            uint32_t rdwr_byte                     :    32;  /*Although only low 8-bits is valid, but change it to 32bits to avoid there's no read/modify/write behaviour*/ /*Write and read byte data to/from UART Tx/Rx FIFO through this field. When USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT is set  then user can write data (up to 64 bytes) into UART Tx FIFO. When USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT is set, user can check USB_SERIAL_JTAG_OUT_EP1_WR_ADDR and USB_SERIAL_JTAG_OUT_EP0_RD_ADDR to know how many data is received, then read that amount of data from UART Rx FIFO. */
        };
        uint32_t val;
    } ep1;
    union {
        struct {
            uint32_t wr_done                       :    1;  /*Set this bit to indicate writing byte data to UART Tx FIFO is done.  This bit then stays 0 until data in UART Tx FIFO is read by the USB Host.*/
            uint32_t serial_in_ep_data_free        :    1;  /*1'b1: Indicate UART Tx FIFO is not full and data can be written into in. After writing USB_SERIAL_JTAG_WR_DONE, this will be 1’b0 until the data is sent to the USB Host.*/
            uint32_t serial_out_ep_data_avail      :    1;  /*1'b1: Indicate there is data in UART Rx FIFO.*/
            uint32_t reserved3                     :    29;  /*reserved*/
        };
        uint32_t val;
    } ep1_conf;
    union {
        struct {
            uint32_t jtag_in_flush_int_raw         :    1;  /*The raw interrupt bit turns to high level when a flush command is received for IN endpoint 2 of JTAG.*/
            uint32_t sof_int_raw                   :    1;  /*The raw interrupt bit turns to high level when a SOF frame is received.*/
            uint32_t serial_out_recv_pkt_int_raw   :    1;  /*The raw interrupt bit turns to high level when the Serial Port OUT Endpoint received one packet.*/
            uint32_t serial_in_empty_int_raw       :    1;  /*The raw interrupt bit turns to high level when the Serial Port IN Endpoint is empty.*/
            uint32_t pid_err_int_raw               :    1;  /*The raw interrupt bit turns to high level when a PID error is detected.*/
            uint32_t crc5_err_int_raw              :    1;  /*The raw interrupt bit turns to high level when a CRC5 error is detected.*/
            uint32_t crc16_err_int_raw             :    1;  /*The raw interrupt bit turns to high level when a CRC16 error is detected.*/
            uint32_t stuff_err_int_raw             :    1;  /*The raw interrupt bit turns to high level when a bit stuffing error is detected.*/
            uint32_t in_token_rec_in_ep1_int_raw   :    1;  /*The raw interrupt bit turns to high level when an IN token for IN endpoint 1 is received.*/
            uint32_t usb_bus_reset_int_raw         :    1;  /*The raw interrupt bit turns to high level when a USB bus reset is detected.*/
            uint32_t out_ep1_zero_payload_int_raw  :    1;  /*The raw interrupt bit turns to high level when OUT endpoint 1 received packet with zero payload.*/
            uint32_t out_ep2_zero_payload_int_raw  :    1;  /*The raw interrupt bit turns to high level when OUT endpoint 2 received packet with zero payload.*/
            uint32_t reserved12                    :    20;  /*reserved*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t jtag_in_flush_int_st          :    1;  /*The raw interrupt status bit for the USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT interrupt.*/
            uint32_t sof_int_st                    :    1;  /*The raw interrupt status bit for the USB_SERIAL_JTAG_SOF_INT interrupt.*/
            uint32_t serial_out_recv_pkt_int_st    :    1;  /*The raw interrupt status bit for the USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT interrupt.*/
            uint32_t serial_in_empty_int_st        :    1;  /*The raw interrupt status bit for the USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT interrupt.*/
            uint32_t pid_err_int_st                :    1;  /*The raw interrupt status bit for the USB_SERIAL_JTAG_PID_ERR_INT interrupt.*/
            uint32_t crc5_err_int_st               :    1;  /*The raw interrupt status bit for the USB_SERIAL_JTAG_CRC5_ERR_INT interrupt.*/
            uint32_t crc16_err_int_st              :    1;  /*The raw interrupt status bit for the USB_SERIAL_JTAG_CRC16_ERR_INT interrupt.*/
            uint32_t stuff_err_int_st              :    1;  /*The raw interrupt status bit for the USB_SERIAL_JTAG_STUFF_ERR_INT interrupt.*/
            uint32_t in_token_rec_in_ep1_int_st    :    1;  /*The raw interrupt status bit for the USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT interrupt.*/
            uint32_t usb_bus_reset_int_st          :    1;  /*The raw interrupt status bit for the USB_SERIAL_JTAG_USB_BUS_RESET_INT interrupt.*/
            uint32_t out_ep1_zero_payload_int_st   :    1;  /*The raw interrupt status bit for the USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT interrupt.*/
            uint32_t out_ep2_zero_payload_int_st   :    1;  /*The raw interrupt status bit for the USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT interrupt.*/
            uint32_t reserved12                    :    20;  /*reserved*/
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t jtag_in_flush_int_ena         :    1;  /*The interrupt enable bit for the USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT interrupt.*/
            uint32_t sof_int_ena                   :    1;  /*The interrupt enable bit for the USB_SERIAL_JTAG_SOF_INT interrupt.*/
            uint32_t serial_out_recv_pkt_int_ena   :    1;  /*The interrupt enable bit for the USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT interrupt.*/
            uint32_t serial_in_empty_int_ena       :    1;  /*The interrupt enable bit for the USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT interrupt.*/
            uint32_t pid_err_int_ena               :    1;  /*The interrupt enable bit for the USB_SERIAL_JTAG_PID_ERR_INT interrupt.*/
            uint32_t crc5_err_int_ena              :    1;  /*The interrupt enable bit for the USB_SERIAL_JTAG_CRC5_ERR_INT interrupt.*/
            uint32_t crc16_err_int_ena             :    1;  /*The interrupt enable bit for the USB_SERIAL_JTAG_CRC16_ERR_INT interrupt.*/
            uint32_t stuff_err_int_ena             :    1;  /*The interrupt enable bit for the USB_SERIAL_JTAG_STUFF_ERR_INT interrupt.*/
            uint32_t in_token_rec_in_ep1_int_ena   :    1;  /*The interrupt enable bit for the USB_SERIAL_JTAG_IN_TOKEN_REC_IN_EP1_INT interrupt.*/
            uint32_t usb_bus_reset_int_ena         :    1;  /*The interrupt enable bit for the USB_SERIAL_JTAG_USB_BUS_RESET_INT interrupt.*/
            uint32_t out_ep1_zero_payload_int_ena  :    1;  /*The interrupt enable bit for the USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT interrupt.*/
            uint32_t out_ep2_zero_payload_int_ena  :    1;  /*The interrupt enable bit for the USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT interrupt.*/
            uint32_t reserved12                    :    20;  /*reserved*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t jtag_in_flush_int_clr         :    1;  /*Set this bit to clear the USB_SERIAL_JTAG_JTAG_IN_FLUSH_INT interrupt.*/
            uint32_t sof_int_clr                   :    1;  /*Set this bit to clear the USB_SERIAL_JTAG_JTAG_SOF_INT interrupt.*/
            uint32_t serial_out_recv_pkt_int_clr   :    1;  /*Set this bit to clear the USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT interrupt.*/
            uint32_t serial_in_empty_int_clr       :    1;  /*Set this bit to clear the USB_SERIAL_JTAG_SERIAL_IN_EMPTY_INT interrupt.*/
            uint32_t pid_err_int_clr               :    1;  /*Set this bit to clear the USB_SERIAL_JTAG_PID_ERR_INT interrupt.*/
            uint32_t crc5_err_int_clr              :    1;  /*Set this bit to clear the USB_SERIAL_JTAG_CRC5_ERR_INT interrupt.*/
            uint32_t crc16_err_int_clr             :    1;  /*Set this bit to clear the USB_SERIAL_JTAG_CRC16_ERR_INT interrupt.*/
            uint32_t stuff_err_int_clr             :    1;  /*Set this bit to clear the USB_SERIAL_JTAG_STUFF_ERR_INT interrupt.*/
            uint32_t in_token_rec_in_ep1_int_clr   :    1;  /*Set this bit to clear the USB_SERIAL_JTAG_IN_TOKEN_IN_EP1_INT interrupt.*/
            uint32_t usb_bus_reset_int_clr         :    1;  /*Set this bit to clear the USB_SERIAL_JTAG_USB_BUS_RESET_INT interrupt.*/
            uint32_t out_ep1_zero_payload_int_clr  :    1;  /*Set this bit to clear the USB_SERIAL_JTAG_OUT_EP1_ZERO_PAYLOAD_INT interrupt.*/
            uint32_t out_ep2_zero_payload_int_clr  :    1;  /*Set this bit to clear the USB_SERIAL_JTAG_OUT_EP2_ZERO_PAYLOAD_INT interrupt.*/
            uint32_t reserved12                    :    20;  /*reserved*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t phy_sel                       :    1;  /*Select internal/external PHY. 1’b0: internal PHY, 1’b1: external PHY*/
            uint32_t exchg_pins_override           :    1;  /*Enable software control USB D+ D- exchange*/
            uint32_t exchg_pins                    :    1;  /*USB D+ D- exchange*/
            uint32_t vrefh                         :    2;  /*Control single-end input high threshold. 1.76V to 2V, step 80mV */
            uint32_t vrefl                         :    2;  /*Control single-end input low threshold. 0.8V to 1.04V, step 80mV*/
            uint32_t vref_override                 :    1;  /*Enable software control input  threshold*/
            uint32_t pad_pull_override             :    1;  /*Enable software control USB D+ D- pullup pulldown*/
            uint32_t dp_pullup                     :    1;  /*Control USB D+ pull up.*/
            uint32_t dp_pulldown                   :    1;  /*Control USB D+ pull down.*/
            uint32_t dm_pullup                     :    1;  /*Control USB D- pull up.*/
            uint32_t dm_pulldown                   :    1;  /*Control USB D- pull down.*/
            uint32_t pullup_value                  :    1;  /*Control pull up value.*/
            uint32_t usb_pad_enable                :    1;  /*Enable USB pad function.*/
            uint32_t reserved15                    :    17;
        };
        uint32_t val;
    } conf0;
    union {
        struct {
            uint32_t test_enable                   :    1;  /*Enable test of the USB pad*/
            uint32_t test_usb_oe                   :    1;  /*USB pad oen in test*/
            uint32_t test_tx_dp                    :    1;  /*USB D+ tx value in test*/
            uint32_t test_tx_dm                    :    1;  /*USB D- tx value in test*/
            uint32_t reserved4                     :    28;
        };
        uint32_t val;
    } test;
    union {
        struct {
            uint32_t in_fifo_cnt                   :    2;  /*JTAG in fifo counter.*/
            uint32_t in_fifo_empty                 :    1;  /*1: JTAG in fifo is empty.*/
            uint32_t in_fifo_full                  :    1;  /*1: JTAG in fifo is full.*/
            uint32_t out_fifo_cnt                  :    2;  /*JTAT out fifo counter.*/
            uint32_t out_fifo_empty                :    1;  /*1: JTAG out fifo is empty.*/
            uint32_t out_fifo_full                 :    1;  /*1: JTAG out fifo is full.*/
            uint32_t in_fifo_reset                 :    1;  /*Write 1 to reset JTAG in fifo.*/
            uint32_t out_fifo_reset                :    1;  /*Write 1 to reset JTAG out fifo.*/
            uint32_t reserved10                    :    22;
        };
        uint32_t val;
    } jfifo_st;
    union {
        struct {
            uint32_t sof_frame_index               :    11;  /*Frame index of received SOF frame.*/
            uint32_t reserved11                    :    21;
        };
        uint32_t val;
    } fram_num;
    union {
        struct {
            uint32_t in_ep0_state                  :    2;  /*State of IN Endpoint 0.*/
            uint32_t in_ep0_wr_addr                :    7;  /*Write data address of IN endpoint 0.*/
            uint32_t in_ep0_rd_addr                :    7;  /*Read data address of IN endpoint 0.*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } in_ep0_st;
    union {
        struct {
            uint32_t in_ep1_state                  :    2;  /*State of IN Endpoint 1.*/
            uint32_t in_ep1_wr_addr                :    7;  /*Write data address of IN endpoint 1.*/
            uint32_t in_ep1_rd_addr                :    7;  /*Read data address of IN endpoint 1.*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } in_ep1_st;
    union {
        struct {
            uint32_t in_ep2_state                  :    2;  /*State of IN Endpoint 2.*/
            uint32_t in_ep2_wr_addr                :    7;  /*Write data address of IN endpoint 2.*/
            uint32_t in_ep2_rd_addr                :    7;  /*Read data address of IN endpoint 2.*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } in_ep2_st;
    union {
        struct {
            uint32_t in_ep3_state                  :    2;  /*State of IN Endpoint 3.*/
            uint32_t in_ep3_wr_addr                :    7;  /*Write data address of IN endpoint 3.*/
            uint32_t in_ep3_rd_addr                :    7;  /*Read data address of IN endpoint 3.*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } in_ep3_st;
    union {
        struct {
            uint32_t out_ep0_state                 :    2;  /*State of OUT Endpoint 0.*/
            uint32_t out_ep0_wr_addr               :    7;  /*Write data address of OUT endpoint 0. When USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT is detected.  there are USB_SERIAL_JTAG_OUT_EP0_WR_ADDR-2 bytes data in OUT EP0. */
            uint32_t out_ep0_rd_addr               :    7;  /*Read data address of OUT endpoint 0.*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } out_ep0_st;
    union {
        struct {
            uint32_t out_ep1_state                 :    2;  /*State of OUT Endpoint 1.*/
            uint32_t out_ep1_wr_addr               :    7;  /*Write data address of OUT endpoint 1. When USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT is detected.  there are USB_SERIAL_JTAG_OUT_EP1_WR_ADDR-2 bytes data in OUT EP1.*/
            uint32_t out_ep1_rd_addr               :    7;  /*Read data address of OUT endpoint 1.*/
            uint32_t out_ep1_rec_data_cnt          :    7;  /*Data count in OUT endpoint 1 when one packet is received.*/
            uint32_t reserved23                    :    9;  /*reserved*/
        };
        uint32_t val;
    } out_ep1_st;
    union {
        struct {
            uint32_t out_ep2_state                 :    2;  /*State of OUT Endpoint 2.*/
            uint32_t out_ep2_wr_addr               :    7;  /*Write data address of OUT endpoint 2. When USB_SERIAL_JTAG_SERIAL_OUT_RECV_PKT_INT is detected.  there are USB_SERIAL_JTAG_OUT_EP2_WR_ADDR-2 bytes data in OUT EP2.*/
            uint32_t out_ep2_rd_addr               :    7;  /*Read data address of OUT endpoint 2.*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } out_ep2_st;
    union {
        struct {
            uint32_t clk_en                        :    1;  /*1'h1: Force clock on for register. 1'h0: Support clock only when application writes registers.*/
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } misc_conf;
    union {
        struct {
            uint32_t usb_mem_pd                    :    1;  /*1: power down usb memory.*/
            uint32_t usb_mem_clk_en                :    1;  /*1: Force clock on for usb memory.*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } mem_conf;
    uint32_t reserved_4c;
    uint32_t reserved_50;
    uint32_t reserved_54;
    uint32_t reserved_58;
    uint32_t reserved_5c;
    uint32_t reserved_60;
    uint32_t reserved_64;
    uint32_t reserved_68;
    uint32_t reserved_6c;
    uint32_t reserved_70;
    uint32_t reserved_74;
    uint32_t reserved_78;
    uint32_t reserved_7c;
    uint32_t date;
} usb_serial_jtag_dev_t;
extern usb_serial_jtag_dev_t USB_SERIAL_JTAG;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_USB_SERIAL_JTAG_STRUCT_H_ */
