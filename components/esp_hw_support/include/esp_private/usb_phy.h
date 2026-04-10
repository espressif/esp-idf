/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "soc/soc_caps.h"

#define USB_PHY_SUPPORTS_P4_OTG11 1 // This version of usb_phy supports P4 OTG1.1 PHY

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief USB PHY target
 */
typedef enum {
    USB_PHY_TARGET_INT,              /**< USB target is internal FSLS PHY */
    USB_PHY_TARGET_UTMI,             /**< USB target is internal UTMI PHY */
    USB_PHY_TARGET_EXT,              /**< USB target is external PHY */
    USB_PHY_TARGET_MAX,
} usb_phy_target_t;

/**
 * @brief USB PHY source
 */
typedef enum {
    USB_PHY_CTRL_OTG,                /**< PHY controller is USB OTG */
#if SOC_USB_SERIAL_JTAG_SUPPORTED
    USB_PHY_CTRL_SERIAL_JTAG,        /**< PHY controller is USB Serial JTAG */
#endif
    USB_PHY_CTRL_MAX,
} usb_phy_controller_t;

/**
 * @brief USB OTG mode
 */
typedef enum {
    USB_PHY_MODE_DEFAULT,            /**< USB OTG default mode */
    USB_OTG_MODE_HOST,               /**< USB OTG host mode */
    USB_OTG_MODE_DEVICE,             /**< USB OTG device mode */
    USB_OTG_MODE_MAX,
} usb_otg_mode_t;

/**
 * @brief USB speed
 */
typedef enum {
    USB_PHY_SPEED_UNDEFINED,
    USB_PHY_SPEED_LOW,              /**< USB Low Speed (1.5 Mbit/s)  */
    USB_PHY_SPEED_FULL,             /**< USB Full Speed (12 Mbit/s)  */
    USB_PHY_SPEED_HIGH,             /**< USB High Speed (480 Mbit/s) */
    USB_PHY_SPEED_MAX,
} usb_phy_speed_t;

/**
 * @brief Initialization for usb_phy_otg_io_conf_t: Self-powered device
 */
#define USB_PHY_SELF_POWERED_DEVICE(vbus_monitor_io)                                                                          \
    {                                                                                                                  \
        .iddig_io_num = -1,                                                                                            \
        .avalid_io_num = -1,                                                                                           \
        .vbusvalid_io_num = -1,                                                                                        \
        .idpullup_io_num = -1,                                                                                         \
        .dppulldown_io_num = -1,                                                                                       \
        .dmpulldown_io_num = -1,                                                                                       \
        .drvvbus_io_num = -1,                                                                                          \
        .bvalid_io_num = vbus_monitor_io,                                                                              \
        .sessend_io_num = -1,                                                                                          \
        .chrgvbus_io_num = -1,                                                                                         \
        .dischrgvbus_io_num = -1,                                                                                      \
    };

/**
 * @brief USB PHY status
 */
typedef enum {
    USB_PHY_STATUS_FREE,             /**< PHY is not being used */
    USB_PHY_STATUS_IN_USE,           /**< PHY is in use */
} usb_phy_status_t;

/**
 * @brief USB external PHY IO pins configuration structure
 */
typedef struct {
    // Inputs
    int vp_io_num;             /**< GPIO pin to USB_EXTPHY_VP_IDX */
    int vm_io_num;             /**< GPIO pin to USB_EXTPHY_VM_IDX */
    int rcv_io_num;            /**< GPIO pin to USB_EXTPHY_RCV_IDX */
    // Outputs
    int suspend_n_io_num;      /**< GPIO pin to USB_EXTPHY_SUSPND_IDX */
    int oen_io_num;            /**< GPIO pin to USB_EXTPHY_OEN_IDX */
    int vpo_io_num;            /**< GPIO pin to USB_EXTPHY_VPO_IDX */
    int vmo_io_num;            /**< GPIO pin to USB_EXTPHY_VMO_IDX */
    int fs_edge_sel_io_num;    /**< GPIO pin to USB_EXTPHY_SPEED_IDX */
} usb_phy_ext_io_conf_t;

/**
 * @brief USB OTG IO pins configuration structure
 */
typedef struct {
    int iddig_io_num;          /**< GPIO pin to USB_OTG_IDDIG_IN_IDX */
    int avalid_io_num;         /**< GPIO pin to USB_OTG_AVALID_IN_IDX */
    int vbusvalid_io_num;      /**< GPIO pin to USB_OTG_VBUSVALID_IN_IDX */
    int idpullup_io_num;       /**< GPIO pin to USB_OTG_IDPULLUP_IDX */
    int dppulldown_io_num;     /**< GPIO pin to USB_OTG_DPPULLDOWN_IDX */
    int dmpulldown_io_num;     /**< GPIO pin to USB_OTG_DMPULLDOWN_IDX */
    int drvvbus_io_num;        /**< GPIO pin to USB_OTG_DRVVBUS_IDX */
    int bvalid_io_num;         /**< GPIO pin to USB_SRP_BVALID_IN_IDX */
    int sessend_io_num;        /**< GPIO pin to USB_SRP_SESSEND_IN_IDX */
    int chrgvbus_io_num;       /**< GPIO pin to USB_SRP_CHRGVBUS_IDX */
    int dischrgvbus_io_num;    /**< GPIO pin to USB_SRP_DISCHRGVBUS_IDX */
} usb_phy_otg_io_conf_t;

/**
 * @brief USB PHY configure struct
 *
 * At minimum the PHY controller and PHY target must be initialized.
 */
typedef struct {
    usb_phy_controller_t controller;          /**< USB PHY controller */
    usb_phy_target_t target;                  /**< USB PHY target INT/EXT */
    usb_otg_mode_t otg_mode;                  /**< USB OTG mode */
    usb_phy_speed_t otg_speed;                /**< USB OTG speed */
    const usb_phy_ext_io_conf_t *ext_io_conf; /**< USB external PHY IO pins configuration */
    const usb_phy_otg_io_conf_t *otg_io_conf; /**< USB OTG IO pins configuration */
} usb_phy_config_t;

typedef struct phy_context_t *usb_phy_handle_t;    /**< USB PHY context handle */

/**
 * @brief Initialize a new USB PHY
 *        Configure at least PHY source.
 *
 * This function will enable the OTG Controller
 *
 * @param[in]  config     USB PHY configuration struct
 * @param[out] handle_ret USB PHY context handle
 *
 * @return
 *     - ESP_OK                 Success
 *     - ESP_ERR_INVALID_STATE  USB PHY already initialized.
 *     - ESP_ERR_NO_MEM USB_OTG Installation failed due to no mem.
 *     - ESP_ERR_NOT_SUPPORTED  Selected PHY is not supported on this target.
 *     - ESP_ERR_INVALID_ARG    Invalid input argument.
 */
esp_err_t usb_new_phy(const usb_phy_config_t *config, usb_phy_handle_t *handle_ret);

/**
 * @brief Configure OTG mode for a USB PHY
 *
 * @param handle Pointer of USB PHY context handle
 * @param mode USB OTG mode
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error.
 *     - ESP_FAIL OTG set mode fail.
 */
esp_err_t usb_phy_otg_set_mode(usb_phy_handle_t handle, usb_otg_mode_t mode);

/**
 * @brief Delete a USB PHY
 *
 * @param handle Pointer of USB PHY context handle
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error.
 */
esp_err_t usb_del_phy(usb_phy_handle_t handle);

/**
 * @brief Get status of a USB PHY
 *
 * @param[in] target The specific PHY target to check
 * @param[out] status Status of the PHY
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error.
 *     - ESP_ERR_INVALID_STATE USB PHY not installed.
 */
esp_err_t usb_phy_get_phy_status(usb_phy_target_t target, usb_phy_status_t *status);

#ifdef __cplusplus
}
#endif
