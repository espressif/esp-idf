/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/usb_phy.h"
#include "soc/usb_dwc_periph.h"
#include "hal/usb_fsls_phy_hal.h"
#include "hal/usb_fsls_phy_ll.h"
#include "esp_rom_gpio.h"
#include "driver/gpio.h"
#include "hal/gpio_ll.h"
#include "soc/soc_caps.h"
#include "soc/usb_pins.h"

#if !SOC_RCC_IS_INDEPENDENT
#define USB_WRAP_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define USB_WRAP_RCC_ATOMIC()
#endif

static const char *USBPHY_TAG = "usb_phy";

#define USBPHY_NOT_INIT_ERR_STR    "USB_PHY is not initialized"

typedef struct phy_context_t phy_context_t;

struct phy_context_t {
    usb_phy_target_t target;                      /**< PHY target */
    usb_phy_controller_t controller;              /**< PHY controller */
    usb_phy_status_t status;                      /**< PHY status */
    usb_otg_mode_t otg_mode;                      /**< USB OTG mode */
    usb_phy_speed_t otg_speed;                    /**< USB speed */
    usb_phy_ext_io_conf_t *iopins;                /**< external PHY I/O pins */
    usb_fsls_phy_hal_context_t hal_context;            /**< USB_PHY hal context */
};

typedef struct {
    phy_context_t *internal_phy;                  /**< internal PHY context */
    phy_context_t *external_phy;                  /**< external PHY context */
    uint32_t ref_count;                           /**< reference count used to protect p_phy_ctrl_obj */
} phy_ctrl_obj_t;

/**
 * @brief A pin descriptor for initialize external PHY I/O pins
 */
typedef struct {
    int pin;                                      /**< GPIO pin num */
    const int func;                               /**< GPIO matrix signal */
    const bool is_output;                         /**< input/output signal */
} usb_iopin_dsc_t;

static phy_ctrl_obj_t *p_phy_ctrl_obj = NULL;
static portMUX_TYPE phy_spinlock = portMUX_INITIALIZER_UNLOCKED;

static esp_err_t phy_iopins_configure(const usb_iopin_dsc_t *usb_periph_iopins, int iopins_num)
{
    for (int i = 0; i < iopins_num; i++) {
        const usb_iopin_dsc_t iopin = usb_periph_iopins[i];
        if (iopin.pin != GPIO_NUM_NC) {
            ESP_RETURN_ON_FALSE((iopin.is_output && GPIO_IS_VALID_OUTPUT_GPIO(iopin.pin)) ||
                                (!iopin.is_output && GPIO_IS_VALID_GPIO(iopin.pin)),
                                ESP_ERR_INVALID_ARG, USBPHY_TAG, "io_num argument is invalid");
            esp_rom_gpio_pad_select_gpio(iopin.pin);
            if (iopin.is_output) {
                esp_rom_gpio_connect_out_signal(iopin.pin, iopin.func, false, false);
            } else {
                esp_rom_gpio_connect_in_signal(iopin.pin, iopin.func, false);
                gpio_ll_input_enable(&GPIO, iopin.pin);
            }
            esp_rom_gpio_pad_unhold(iopin.pin);
        }
    }
    return ESP_OK;
}

static esp_err_t phy_external_iopins_configure(const usb_phy_ext_io_conf_t *ext_io_conf)
{
    const usb_iopin_dsc_t usb_periph_iopins[] = {
        {ext_io_conf->vp_io_num, usb_otg_periph_signal.extphy_vp_in, false},
        {ext_io_conf->vm_io_num, usb_otg_periph_signal.extphy_vm_in, false},
        {ext_io_conf->rcv_io_num, usb_otg_periph_signal.extphy_rcv_in, false},
        {ext_io_conf->oen_io_num, usb_otg_periph_signal.extphy_oen_out, true},
        {ext_io_conf->vpo_io_num, usb_otg_periph_signal.extphy_vpo_out, true},
        {ext_io_conf->vmo_io_num, usb_otg_periph_signal.extphy_vmo_out, true},
    };

    return phy_iopins_configure(usb_periph_iopins, sizeof(usb_periph_iopins) / sizeof(usb_iopin_dsc_t));
}

static esp_err_t phy_otg_iopins_configure(const usb_phy_otg_io_conf_t *otg_io_conf)
{
    const usb_iopin_dsc_t usb_periph_iopins[] = {
        {otg_io_conf->iddig_io_num, usb_otg_periph_signal.otg_iddig_in, false},
        {otg_io_conf->avalid_io_num, usb_otg_periph_signal.otg_avalid_in, false},
        {otg_io_conf->vbusvalid_io_num, usb_otg_periph_signal.otg_vbusvalid_in, false},
        {otg_io_conf->idpullup_io_num, usb_otg_periph_signal.otg_idpullup_out, true},
        {otg_io_conf->dppulldown_io_num, usb_otg_periph_signal.otg_dppulldown_out, true},
        {otg_io_conf->dmpulldown_io_num, usb_otg_periph_signal.otg_dmpulldown_out, true},
        {otg_io_conf->drvvbus_io_num, usb_otg_periph_signal.otg_drvvbus_out, true},
        {otg_io_conf->bvalid_io_num, usb_otg_periph_signal.srp_bvalid_in, false},
        {otg_io_conf->sessend_io_num, usb_otg_periph_signal.srp_sessend_in, false},
        {otg_io_conf->chrgvbus_io_num, usb_otg_periph_signal.srp_chrgvbus_out, true},
        {otg_io_conf->dischrgvbus_io_num, usb_otg_periph_signal.srp_dischrgvbus_out, true},
    };
    return phy_iopins_configure(usb_periph_iopins, sizeof(usb_periph_iopins) / sizeof(usb_iopin_dsc_t));
}

esp_err_t usb_phy_otg_set_mode(usb_phy_handle_t handle, usb_otg_mode_t mode)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, USBPHY_TAG, "handle argument is invalid");
    ESP_RETURN_ON_FALSE(mode < USB_OTG_MODE_MAX, ESP_ERR_INVALID_ARG, USBPHY_TAG, "mode argument is invalid");
    ESP_RETURN_ON_FALSE(handle->controller == USB_PHY_CTRL_OTG, ESP_FAIL, USBPHY_TAG, "phy source is not USB_OTG");

    handle->otg_mode = mode;
    if (mode == USB_OTG_MODE_HOST) {
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, USB_OTG_IDDIG_IN_IDX, false);     // connected connector is A side
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, USB_SRP_BVALID_IN_IDX, false);
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, USB_OTG_VBUSVALID_IN_IDX, false);  // receiving a valid Vbus from host
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, USB_OTG_AVALID_IN_IDX, false);     // HIGH to force USB host mode
        if (handle->target == USB_PHY_TARGET_INT) {
            usb_fsls_phy_hal_int_load_conf_host(&(handle->hal_context));
        }
    } else if (mode == USB_OTG_MODE_DEVICE) {
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, USB_OTG_IDDIG_IN_IDX, false);      // connected connector is mini-B side
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, USB_SRP_BVALID_IN_IDX, false);     // HIGH to force USB device mode
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, USB_OTG_VBUSVALID_IN_IDX, false);  // receiving a valid Vbus from device
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, USB_OTG_AVALID_IN_IDX, false);
    }

    return ESP_OK;
}

esp_err_t usb_phy_otg_dev_set_speed(usb_phy_handle_t handle, usb_phy_speed_t speed)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, USBPHY_TAG, "handle argument is invalid");
    ESP_RETURN_ON_FALSE(speed < USB_PHY_SPEED_MAX, ESP_ERR_INVALID_ARG, USBPHY_TAG, "speed argument is invalid");
    ESP_RETURN_ON_FALSE(handle->controller == USB_PHY_CTRL_OTG, ESP_FAIL, USBPHY_TAG, "phy source is not USB_OTG");
    ESP_RETURN_ON_FALSE((handle->target == USB_PHY_TARGET_INT && handle->otg_mode == USB_OTG_MODE_DEVICE), ESP_FAIL,
                        USBPHY_TAG, "set speed not supported");

    handle->otg_speed = speed;
    usb_fsls_phy_hal_int_load_conf_dev(&(handle->hal_context), speed);
    return ESP_OK;
}

esp_err_t usb_phy_action(usb_phy_handle_t handle, usb_phy_action_t action)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, USBPHY_TAG, "handle argument is invalid");
    ESP_RETURN_ON_FALSE(action < USB_PHY_ACTION_MAX, ESP_ERR_INVALID_ARG, USBPHY_TAG, "action argument is invalid");
    ESP_RETURN_ON_FALSE((action == USB_PHY_ACTION_HOST_ALLOW_CONN && handle->controller == USB_PHY_CTRL_OTG) ||
                        (action == USB_PHY_ACTION_HOST_FORCE_DISCONN && handle->controller == USB_PHY_CTRL_OTG),
                        ESP_ERR_INVALID_ARG, USBPHY_TAG, "wrong target for the action");

    esp_err_t ret = ESP_OK;
    switch (action) {
    case USB_PHY_ACTION_HOST_ALLOW_CONN:
        if (handle->target == USB_PHY_TARGET_INT) {
            usb_fsls_phy_hal_int_mimick_disconn(&(handle->hal_context), false);
        } else {
            if (!handle->iopins) {
                ret = ESP_FAIL;
                ESP_LOGE(USBPHY_TAG, "no I/O pins provided for connection");
                break;
            }
            /*
            Allow for connections on the external PHY by connecting the VP and VM signals to the external PHY.
            */
            esp_rom_gpio_connect_in_signal(handle->iopins->vp_io_num, USB_EXTPHY_VP_IDX, false);
            esp_rom_gpio_connect_in_signal(handle->iopins->vm_io_num, USB_EXTPHY_VM_IDX, false);
        }
        break;

    case USB_PHY_ACTION_HOST_FORCE_DISCONN:
        if (handle->target == USB_PHY_TARGET_INT) {
            usb_fsls_phy_hal_int_mimick_disconn(&(handle->hal_context), true);
        } else {
            /*
            Disable connections on the external PHY by connecting the VP and VM signals to the constant LOW signal.
            */
            esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, USB_EXTPHY_VP_IDX, false);
            esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, USB_EXTPHY_VM_IDX, false);
        }
        break;

    default:
        break;
    }

    return ret;
}

static esp_err_t usb_phy_install(void)
{
    portENTER_CRITICAL(&phy_spinlock);
    if (p_phy_ctrl_obj) {
        // p_phy_ctrl_obj already installed, return immediately
        portEXIT_CRITICAL(&phy_spinlock);
        return ESP_OK;
    }
    portEXIT_CRITICAL(&phy_spinlock);

    esp_err_t ret = ESP_OK;
    phy_ctrl_obj_t *phy_ctrl_obj = (phy_ctrl_obj_t *) calloc(1, sizeof(phy_ctrl_obj_t));
    ESP_GOTO_ON_FALSE(phy_ctrl_obj, ESP_ERR_NO_MEM, cleanup, USBPHY_TAG, "no mem for USB_PHY driver");

    portENTER_CRITICAL(&phy_spinlock);
    if (!p_phy_ctrl_obj) {
        p_phy_ctrl_obj = phy_ctrl_obj;
        p_phy_ctrl_obj->ref_count = 0;
    } else {
        // p_phy_ctrl_obj already installed, need to free resource
        portEXIT_CRITICAL(&phy_spinlock);
        goto cleanup;
    }
    // Enable USB peripheral and reset the register
    portEXIT_CRITICAL(&phy_spinlock);
    USB_WRAP_RCC_ATOMIC() {
        usb_fsls_phy_ll_usb_wrap_enable_bus_clock(true);
        usb_fsls_phy_ll_usb_wrap_reset_register();
    }
    return ESP_OK;

cleanup:
    free(phy_ctrl_obj);
    return ret;
}

esp_err_t usb_new_phy(const usb_phy_config_t *config, usb_phy_handle_t *handle_ret)
{
    ESP_RETURN_ON_FALSE(config, ESP_ERR_INVALID_ARG, USBPHY_TAG, "config argument is invalid");
    ESP_RETURN_ON_FALSE(config->target < USB_PHY_TARGET_MAX, ESP_ERR_INVALID_ARG, USBPHY_TAG, "specified PHY argument is invalid");
    ESP_RETURN_ON_FALSE(config->controller < USB_PHY_CTRL_MAX, ESP_ERR_INVALID_ARG, USBPHY_TAG, "specified source argument is invalid");

    ESP_RETURN_ON_ERROR(usb_phy_install(), USBPHY_TAG, "usb_phy driver installation failed");
    esp_err_t ret = ESP_OK;
    bool new_phy = false;
    phy_context_t *phy_context = (phy_context_t *) calloc(1, sizeof(phy_context_t));
    ESP_GOTO_ON_FALSE(phy_context, ESP_ERR_NO_MEM, cleanup, USBPHY_TAG, "no mem for phy context");

    portENTER_CRITICAL(&phy_spinlock);
    usb_phy_get_phy_status(config->target, &phy_context->status);
    if (phy_context->status == USB_PHY_STATUS_FREE) {
        new_phy = true;
        p_phy_ctrl_obj->ref_count++;
        if (config->target == USB_PHY_TARGET_EXT) {
            p_phy_ctrl_obj->external_phy = phy_context;
        } else {
            p_phy_ctrl_obj->internal_phy = phy_context;
        }
    }
    portEXIT_CRITICAL(&phy_spinlock);
    ESP_GOTO_ON_FALSE(new_phy, ESP_ERR_INVALID_STATE, cleanup, USBPHY_TAG, "selected PHY is in use");

    phy_context->target = config->target;
    phy_context->controller = config->controller;
    phy_context->status = USB_PHY_STATUS_IN_USE;

    usb_fsls_phy_hal_init(&(phy_context->hal_context));
    if (config->controller == USB_PHY_CTRL_OTG) {
        usb_fsls_phy_hal_otg_conf(&(phy_context->hal_context), config->target == USB_PHY_TARGET_EXT);
    }
#if SOC_USB_SERIAL_JTAG_SUPPORTED
    else if (config->controller == USB_PHY_CTRL_SERIAL_JTAG) {
        usb_fsls_phy_hal_jtag_conf(&(phy_context->hal_context), config->target == USB_PHY_TARGET_EXT);
        phy_context->otg_mode = USB_OTG_MODE_DEVICE;
        phy_context->otg_speed = USB_PHY_SPEED_FULL;
    }
#endif

    if (config->target == USB_PHY_TARGET_INT) {
        gpio_set_drive_capability(USBPHY_DM_NUM, GPIO_DRIVE_CAP_3);
        gpio_set_drive_capability(USBPHY_DP_NUM, GPIO_DRIVE_CAP_3);
    }

    *handle_ret = (usb_phy_handle_t) phy_context;
    if (config->ext_io_conf && config->target == USB_PHY_TARGET_EXT) {
        phy_context->iopins = (usb_phy_ext_io_conf_t *) calloc(1, sizeof(usb_phy_ext_io_conf_t));
        ESP_GOTO_ON_FALSE(phy_context->iopins, ESP_ERR_NO_MEM, cleanup, USBPHY_TAG, "no mem for storing I/O pins");
        memcpy(phy_context->iopins, config->ext_io_conf, sizeof(usb_phy_ext_io_conf_t));
        ESP_ERROR_CHECK(phy_external_iopins_configure(phy_context->iopins));
    }
    if (config->otg_mode != USB_PHY_MODE_DEFAULT) {
        ESP_ERROR_CHECK(usb_phy_otg_set_mode(*handle_ret, config->otg_mode));
    }
    if (config->otg_speed != USB_PHY_SPEED_UNDEFINED) {
        ESP_ERROR_CHECK(usb_phy_otg_dev_set_speed(*handle_ret, config->otg_speed));
    }
    if (config->otg_io_conf && (phy_context->controller == USB_PHY_CTRL_OTG)) {
        ESP_ERROR_CHECK(phy_otg_iopins_configure(config->otg_io_conf));
    }
    return ESP_OK;

cleanup:
    free(phy_context->iopins);
    free(phy_context);
    if (p_phy_ctrl_obj->ref_count == 0) {
        free(p_phy_ctrl_obj);
        p_phy_ctrl_obj = NULL;
    }
    return ret;
}

static void phy_uninstall(void)
{
    phy_ctrl_obj_t *p_phy_ctrl_obj_free = NULL;
    portENTER_CRITICAL(&phy_spinlock);
    if (p_phy_ctrl_obj->ref_count == 0) {
        p_phy_ctrl_obj_free = p_phy_ctrl_obj;
        p_phy_ctrl_obj = NULL;
        USB_WRAP_RCC_ATOMIC() {
            // Disable USB peripheral without reset the module
            usb_fsls_phy_ll_usb_wrap_enable_bus_clock(false);
        }
    }
    portEXIT_CRITICAL(&phy_spinlock);
    free(p_phy_ctrl_obj_free);
}

esp_err_t usb_del_phy(usb_phy_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, USBPHY_TAG, "handle argument is invalid");

    portENTER_CRITICAL(&phy_spinlock);
    p_phy_ctrl_obj->ref_count--;
    if (handle->target == USB_PHY_TARGET_EXT) {
        p_phy_ctrl_obj->external_phy = NULL;
    } else {
        // Clear pullup and pulldown loads on D+ / D-, and disable the pads
        usb_fsls_phy_ll_int_load_conf(handle->hal_context.wrap_dev, false, false, false, false);
        usb_fsls_phy_ll_usb_wrap_pad_enable(handle->hal_context.wrap_dev, false);
        p_phy_ctrl_obj->internal_phy = NULL;
    }
    portEXIT_CRITICAL(&phy_spinlock);
    free(handle->iopins);
    free(handle);
    phy_uninstall();
    return ESP_OK;
}

esp_err_t usb_phy_get_phy_status(usb_phy_target_t target, usb_phy_status_t *status)
{
    ESP_RETURN_ON_FALSE(target < USB_PHY_TARGET_MAX, ESP_ERR_INVALID_ARG, USBPHY_TAG, "argument is invalid");
    ESP_RETURN_ON_FALSE(p_phy_ctrl_obj, ESP_ERR_INVALID_STATE, USBPHY_TAG, USBPHY_NOT_INIT_ERR_STR);

    if (target == USB_PHY_TARGET_EXT && p_phy_ctrl_obj->external_phy) {
        *status = p_phy_ctrl_obj->external_phy->status;
    } else if (target == USB_PHY_TARGET_INT && p_phy_ctrl_obj->internal_phy) {
        *status = p_phy_ctrl_obj->internal_phy->status;
    } else {
        *status = USB_PHY_STATUS_FREE;
    }
    return ESP_OK;
}
