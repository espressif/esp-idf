/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer of the USB-serial-jtag controller

#pragma once
#include <stdbool.h>
#include "esp_attr.h"
#include "soc/usb_serial_jtag_reg.h"
#include "soc/usb_serial_jtag_struct.h"
#include "soc/system_struct.h"

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
 * @brief  Get the USB_SERIAL_JTAG raw interrupt status.
 *
 * @return The USB_SERIAL_JTAG raw interrupt status.
 */
static inline __attribute__((always_inline)) uint32_t usb_serial_jtag_ll_get_intraw_mask(void)
{
    return USB_SERIAL_JTAG.int_raw.val;
}

/**
 * @brief  Clear the USB_SERIAL_JTAG interrupt status based on the given mask.
 *
 * @param  mask The bitmap of the interrupts need to be cleared.
 *
 * @return None
 */
static inline __attribute__((always_inline)) void usb_serial_jtag_ll_clr_intsts_mask(uint32_t mask)
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

/**
 * @brief Disable usb serial jtag pad during light sleep to avoid current leakage
 *
 * @return Initial configuration of usb serial jtag pad enable before light sleep
 */
FORCE_INLINE_ATTR bool usb_serial_jtag_ll_pad_backup_and_disable(void)
{
    bool pad_enabled = USB_SERIAL_JTAG.conf0.usb_pad_enable;

    // Disable USB pad function
    USB_SERIAL_JTAG.conf0.usb_pad_enable = 0;

    return pad_enabled;
}

/**
 * @brief Enable the internal USJ PHY control to D+/D- pad
 *
 * @param enable_pad Enable the USJ PHY control to D+/D- pad
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_enable_pad(bool enable_pad)
{
    USB_SERIAL_JTAG.conf0.usb_pad_enable = enable_pad;
}

/**
 * @brief Enable the bus clock for  USB Serial_JTAG module
 * @param clk_en True if enable the clock of USB Serial_JTAG module
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_enable_bus_clock(bool clk_en)
{
    SYSTEM.perip_clk_en0.reg_usb_device_clk_en = clk_en;
}

// SYSTEM.perip_clk_enx are shared registers, so this function must be used in an atomic way
#define usb_serial_jtag_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; usb_serial_jtag_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the usb serial jtag module
 */
FORCE_INLINE_ATTR void usb_serial_jtag_ll_reset_register(void)
{
    SYSTEM.perip_rst_en0.reg_usb_device_rst = 1;
    SYSTEM.perip_rst_en0.reg_usb_device_rst = 0;
}

// SYSTEM.perip_clk_enx are shared registers, so this function must be used in an atomic way
#define usb_serial_jtag_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; usb_serial_jtag_ll_reset_register(__VA_ARGS__)

/**
 * Get the enable status USB Serial_JTAG module
 *
 * @return Return true if USB Serial_JTAG module is enabled
 */
FORCE_INLINE_ATTR bool usb_serial_jtag_ll_module_is_enabled(void)
{
    return (SYSTEM.perip_clk_en0.reg_usb_device_clk_en && !SYSTEM.perip_rst_en0.reg_usb_device_rst);
}


#ifdef __cplusplus
}
#endif
