/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "hal/usb_phy_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief USB PHY available actions
 */
typedef enum {
    USB_PHY_ACTION_HOST_ALLOW_CONN,             /**< Enable physical connection when operating as an OTG Host */
    USB_PHY_ACTION_HOST_FORCE_DISCONN,          /**< Disable physical connection when operating as an OTG Host */
    USB_PHY_ACTION_MAX,
} usb_phy_action_t;

/**
 * @brief USB external PHY IO pins configuration structure
 */
typedef struct {
    int vp_io_num;             /**< GPIO pin to USB_EXTPHY_VP_IDX */
    int vm_io_num;             /**< GPIO pin to USB_EXTPHY_VM_IDX */
    int rcv_io_num;            /**< GPIO pin to USB_EXTPHY_RCV_IDX */
    int oen_io_num;            /**< GPIO pin to USB_EXTPHY_OEN_IDX */
    int vpo_io_num;            /**< GPIO pin to USB_EXTPHY_VPO_IDX */
    int vmo_io_num;            /**< GPIO pin to USB_EXTPHY_VMO_IDX */
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
 * @param[in] config USB PHY configurtion struct
 * @param[out] handle_ret USB PHY context handle
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL USB PHY init error.
 *     - ESP_ERR_INVALID_STATE USB PHY not installed.
 *     - ESP_ERR_NO_MEM USB_OTG installation failed due to no mem.
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
 * @brief Configure USB speed for a USB PHY that is operating as an OTG Device
 *
 * @param handle Pointer of USB PHY context handle
 * @param mode USB speed
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error.
 *     - ESP_FAIL OTG set speed fail.
 */
esp_err_t usb_phy_otg_dev_set_speed(usb_phy_handle_t handle, usb_phy_speed_t speed);

/**
 * @brief Take a action for a USB PHY
 *
 * @param handle Pointer of USB PHY context handle
 * @param action USB PHY action
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error.
 *     - ESP_FAIL Action cannot be performed.
 */
esp_err_t usb_phy_action(usb_phy_handle_t handle, usb_phy_action_t action);

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
