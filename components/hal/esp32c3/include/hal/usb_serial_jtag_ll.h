// Copyright 2021 Espressif Systems (Shanghai)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// The LL layer of the USB-serial-jtag controller

#pragma once
#include "soc/usb_serial_jtag_reg.h"
#include "soc/usb_serial_jtag_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

//The in and out endpoints are this long.
#define USB_SERIAL_JTAG_PACKET_SZ_BYTES 64

#define USB_SERIAL_JTAG_LL_INTR_MASK         (0x7ffff) //All interrupt mask

// Define USB_SERIAL_JTAG interrupts
// Note the hardware has more interrupts, but they're only useful for debugging
// the hardware.
typedef enum {
    USB_SERIAL_JTAG_INTR_SOF                    = (1 << 1),
    USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT    = (1 << 2),
    USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY        = (1 << 3),
    USB_SERIAL_JTAG_INTR_TOKEN_REC_IN_EP1       = (1 << 8),
    USB_SERIAL_JTAG_INTR_BUS_RESET              = (1 << 9),
    USB_SERIAL_JTAG_INTR_EP1_ZERO_PAYLOAD       = (1 << 10),
} usb_serial_jtag_ll_intr_t;

/**
 * @brief  Enable the USB_SERIAL_JTAG interrupt based on the given mask.
 *
 * @param  mask The bitmap of the interrupts need to be enabled.
 *
 * @return None
 */
static inline void usb_serial_jtag_ll_ena_intr_mask(uint32_t mask)
{
    USB_SERIAL_JTAG.int_ena.val |= mask;
}

/**
 * @brief  Clear the USB_SERIAL_JTAG interrupt based on the given mask.
 *
 * @param  mask The bitmap of the interrupts bits need to be cleared.
 *
 * @return None
 */
static inline void usb_serial_jtag_ll_clr_intr_sts_mask(uint32_t mask)
{
    USB_SERIAL_JTAG.int_clr.val = mask;
}

/**
 * @brief  Disable the USB_SERIAL_JTAG interrupt based on the given mask.
 *
 * @param  mask The bitmap of the interrupts need to be disabled.
 *
 * @return None
 */
static inline void usb_serial_jtag_ll_disable_intr_mask(uint32_t mask)
{
    USB_SERIAL_JTAG.int_ena.val &= (~mask);
}

/**
 * @brief  Get the USB_SERIAL_JTAG interrupt status.
 *
 * @return The USB_SERIAL_JTAG interrupt status.
 */
static inline uint32_t usb_serial_jtag_ll_get_intsts_mask(void)
{
    return USB_SERIAL_JTAG.int_st.val;
}

/**
 * @brief  Clear the USB_SERIAL_JTAG interrupt status based on the given mask.
 *
 * @param  mask The bitmap of the interrupts need to be cleared.
 *
 * @return None
 */
static inline void usb_serial_jtag_ll_clr_intsts_mask(uint32_t mask)
{
    USB_SERIAL_JTAG.int_clr.val = mask;
}

/**
 * @brief  Get status of enabled interrupt.
 *
 * @return interrupt enable value
 */
static inline uint32_t usb_serial_jtag_ll_get_intr_ena_status(void)
{
    return USB_SERIAL_JTAG.int_ena.val;
}

/**
 * @brief  Read the bytes from the USB_SERIAL_JTAG rxfifo.
 *
 * @param  buf The data buffer.
 * @param  rd_len The data length needs to be read.
 *
 * @return amount of bytes read
 */
static inline int usb_serial_jtag_ll_read_rxfifo(uint8_t *buf, uint32_t rd_len)
{
    int i;
    for (i = 0; i < (int)rd_len; i++) {
        if (!USB_SERIAL_JTAG.ep1_conf.serial_out_ep_data_avail) break;
        buf[i] = USB_SERIAL_JTAG.ep1.rdwr_byte;
    }
    return i;
}

/**
 * @brief  Write byte to the USB_SERIAL_JTAG txfifo. Only writes bytes as long / if there
 *         is room in the buffer.
 *
 * @param  buf The data buffer.
 * @param  wr_len The data length needs to be writen.
 *
 * @return Amount of bytes actually written. May be less than wr_len.
 */
static inline int usb_serial_jtag_ll_write_txfifo(const uint8_t *buf, uint32_t wr_len)
{
    int i;
    for (i = 0; i < (int)wr_len; i++) {
        if (!USB_SERIAL_JTAG.ep1_conf.serial_in_ep_data_free) break;
        USB_SERIAL_JTAG.ep1.rdwr_byte = buf[i];
    }
    return i;
}

/**
 * @brief  Returns 1 if the USB_SERIAL_JTAG rxfifo has data available.
 *
 * @return 0 if no data available, 1 if data available
 */
static inline int usb_serial_jtag_ll_rxfifo_data_available(void)
{
    return USB_SERIAL_JTAG.ep1_conf.serial_out_ep_data_avail;
}

/**
 * @brief  Returns 1 if the USB_SERIAL_JTAG txfifo has room.
 *
 * @return 0 if no data available, 1 if data available
 */
static inline int usb_serial_jtag_ll_txfifo_writable(void)
{
    return USB_SERIAL_JTAG.ep1_conf.serial_in_ep_data_free;
}

/**
 * @brief  Flushes the TX buffer, that is, make it available for the
 *         host to pick up.
 *
 * @note  When fifo is full (with 64 byte), HW will flush the buffer automatically.
 *        It won't be executed if there is nothing in the fifo.
 *
 * @return na
 */
static inline void usb_serial_jtag_ll_txfifo_flush(void)
{
    USB_SERIAL_JTAG.ep1_conf.wr_done=1;
}


#ifdef __cplusplus
}
#endif
