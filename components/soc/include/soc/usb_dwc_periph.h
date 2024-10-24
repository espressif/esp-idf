/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_pins.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_USB_OTG_SUPPORTED

/* ---------------------------------- Types --------------------------------- */

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
} usb_utmi_otg_signal_conn_t;

/**
 * @brief USB Controller Information
 *
 * Structure to store information for all USB-DWC instances
 */
typedef struct {
    struct {
        const usb_fsls_serial_signal_conn_t * const fsls_signals;   // Must be set if external PHY is supported by controller
        const usb_utmi_otg_signal_conn_t * const otg_signals;
        const int irq;
        const int irq_2nd_cpu;  // The USB-DWC can provide 2nd interrupt so each CPU can have its own interrupt line. Set to -1 if not supported
    } controllers [SOC_USB_OTG_PERIPH_NUM];
} usb_dwc_info_t;

extern const usb_dwc_info_t usb_dwc_info;

#endif // SOC_USB_OTG_SUPPORTED

/* ------------------------------- Deprecated ------------------------------- */
/* Todo: Remove in ESP-IDF v6.0 (IDF-9052) */

#if SOC_USB_OTG_SUPPORTED

/*
 Stores a bunch of USB-peripheral data.
*/
typedef struct {
    const uint8_t extphy_vp_in;
    const uint8_t extphy_vm_in;
    const uint8_t extphy_rcv_in;
    const uint8_t extphy_oen_out;
    const uint8_t extphy_vpo_out;
    const uint8_t extphy_vmo_out;
    const uint8_t extphy_suspend_in;
    const uint8_t extphy_speed_in;
    const uint8_t srp_bvalid_in;
    const uint8_t srp_sessend_in;
    const uint8_t srp_chrgvbus_out;
    const uint8_t srp_dischrgvbus_out;
    const uint8_t otg_iddig_in;
    const uint8_t otg_avalid_in;
    const uint8_t otg_vbusvalid_in;
    const uint8_t otg_idpullup_out;
    const uint8_t otg_dppulldown_out;
    const uint8_t otg_dmpulldown_out;
    const uint8_t otg_drvvbus_out;
    const periph_module_t module;
} usb_phy_signal_conn_t;

extern const usb_phy_signal_conn_t usb_otg_periph_signal;

#endif // SOC_USB_OTG_SUPPORTED

#ifdef __cplusplus
}
#endif
