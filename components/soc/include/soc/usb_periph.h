/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_pins.h"
#include "soc/soc_caps.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_USB_OTG_SUPPORTED

/* ---------------------------------- Types --------------------------------- */

/**
 * @brief USB PHY Instance Type
 */
typedef enum {
    USB_PHY_INST_FSLS_INTERN_0 = (1 << 0),
    USB_PHY_INST_FSLS_INTERN_1 = (1 << 1),
    USB_PHY_INST_UTMI_0        = (1 << 2),
    USB_PHY_INST_EXTERN        = (1 << 3),
} usb_phy_inst_t;

/**
 * @brief USB PHY FSLS Serial Interface Signals
 *
 * Structure to store the GPIO matrix signal indexes for a USB PHY FSLS Serial
 * interface's signals.
 *
 * @note Refer to section "2.2.1.13 FsLsSerialMode" of the UTMI+ for more
 *       details regarding the FSLS Serial Interface.
 */
typedef struct {
    // Inputs
    int rx_dp;
    int rx_dm;
    int rx_rcv;
    // Outputs
    int suspend_n;
    int tx_enable_n;
    int tx_dp;
    int tx_dm;
    int fs_edge_sel;
} usb_fsls_serial_signal_conn_t;

/**
 * @brief USB PHY UTMI OTG Interface Signal Index Type
 *
 * Structure to store the GPIO matrix signal indexes for a UTMI PHY interface's
 * OTG signals.
 */
typedef struct {
    // Inputs
    int iddig;
    int avalid;
    int bvalid;
    int vbusvalid;
    int sessend;
    // Outputs
    int idpullup;
    int dppulldown;
    int dmpulldown;
    int drvvbus;
    int chrgvbus;
    int dischrgvbus;
} usb_otg_signal_conn_t;

/**
 * @brief Internal USB PHY IO
 *
 * Structure to store the IO numbers for a particular internal USB PHY
 */
typedef struct {
    int dp;
    int dm;
} usb_internal_phy_io_t;

/**
 * @brief USB Controller Information
 *
 * Structure to store information for all USB-DWC instances
 *
 * For targets with multiple USB controllers, we support only fixed mapping of the PHYs.
 * This is a software limitation; the hardware supports swapping Controllers and PHYs.
 */
typedef struct {
    struct {
        const usb_fsls_serial_signal_conn_t * const fsls_signals;   // Must be set if external PHY is supported by controller
        const usb_otg_signal_conn_t * const otg_signals;
        const usb_internal_phy_io_t * const internal_phy_io;        // Must be set for internal FSLS PHY(s)
        const usb_phy_inst_t supported_phys;                        // Bitmap of supported PHYs by this controller
        const int irq;
        const int irq_2nd_cpu;  // The USB-DWC can provide 2nd interrupt so each CPU can have its own interrupt line. Set to -1 if not supported
    } controllers [SOC_USB_OTG_PERIPH_NUM];
} usb_dwc_info_t;

extern const usb_dwc_info_t usb_dwc_info;

#endif // SOC_USB_OTG_SUPPORTED

#ifdef __cplusplus
}
#endif
