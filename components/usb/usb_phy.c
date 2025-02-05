/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/usb_phy.h"
#include "esp_private/critical_section.h"
#include "soc/usb_dwc_periph.h"
#include "hal/usb_wrap_hal.h"
#include "hal/usb_utmi_hal.h"
#include "esp_rom_gpio.h"
#include "driver/gpio.h"
#include "soc/soc_caps.h"

#if !SOC_RCC_IS_INDEPENDENT
#define USB_PHY_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define USB_PHY_RCC_ATOMIC()
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
    usb_wrap_hal_context_t wrap_hal;              /**< USB WRAP HAL context */
};

typedef struct {
    phy_context_t *fsls_phy;                      /**< internal FSLS PHY context */
    phy_context_t *utmi_phy;                      /**< internal UTMI PHY context */
    phy_context_t *external_phy;                  /**< external PHY context */
    uint32_t ref_count;                           /**< reference count used to protect p_phy_ctrl_obj */
} phy_ctrl_obj_t;

static phy_ctrl_obj_t *p_phy_ctrl_obj = NULL;
// Mapping of OTG1.1 peripheral in usb_dwc_info struct
#if CONFIG_IDF_TARGET_ESP32P4
static const int otg11_index = 1;
#else
static const int otg11_index = 0;
#endif

DEFINE_CRIT_SECTION_LOCK_STATIC(phy_spinlock);
#define PHY_ENTER_CRITICAL()           esp_os_enter_critical(&phy_spinlock)
#define PHY_EXIT_CRITICAL()            esp_os_exit_critical(&phy_spinlock)

static esp_err_t phy_configure_pin_input(int gpio_pin, int signal_idx)
{
    if (gpio_pin != GPIO_NUM_NC) {
        ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(gpio_pin),
                            ESP_ERR_INVALID_ARG, USBPHY_TAG, "io_num argument is invalid");
        esp_rom_gpio_pad_select_gpio(gpio_pin);
        esp_rom_gpio_connect_in_signal(gpio_pin, signal_idx, false);
        gpio_input_enable(gpio_pin);
        esp_rom_gpio_pad_unhold(gpio_pin);
    }
    return ESP_OK;
}

static esp_err_t phy_configure_pin_output(int gpio_pin, int signal_idx)
{
    if (gpio_pin != GPIO_NUM_NC) {
        ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(gpio_pin),
                            ESP_ERR_INVALID_ARG, USBPHY_TAG, "io_num argument is invalid");
        esp_rom_gpio_pad_select_gpio(gpio_pin);
        esp_rom_gpio_connect_out_signal(gpio_pin, signal_idx, false, false);
        esp_rom_gpio_pad_unhold(gpio_pin);
    }
    return ESP_OK;
}

static esp_err_t phy_external_iopins_configure(const usb_phy_ext_io_conf_t *ext_io_conf, const usb_fsls_serial_signal_conn_t *fsls_sig)
{
    ESP_RETURN_ON_FALSE(ext_io_conf && fsls_sig, ESP_ERR_INVALID_ARG, USBPHY_TAG, "argument is invalid");
    esp_err_t ret = ESP_OK;

    // Inputs
    ret |= phy_configure_pin_input(ext_io_conf->vp_io_num,  fsls_sig->rx_dp);
    ret |= phy_configure_pin_input(ext_io_conf->vm_io_num,  fsls_sig->rx_dm);
    ret |= phy_configure_pin_input(ext_io_conf->rcv_io_num, fsls_sig->rx_rcv);

    // Outputs
    ret |= phy_configure_pin_output(ext_io_conf->suspend_n_io_num,   fsls_sig->suspend_n);
    ret |= phy_configure_pin_output(ext_io_conf->oen_io_num,         fsls_sig->tx_enable_n);
    ret |= phy_configure_pin_output(ext_io_conf->vpo_io_num,         fsls_sig->tx_dp);
    ret |= phy_configure_pin_output(ext_io_conf->vmo_io_num,         fsls_sig->tx_dm);
    ret |= phy_configure_pin_output(ext_io_conf->fs_edge_sel_io_num, fsls_sig->fs_edge_sel);

    return ret;
}

static esp_err_t phy_otg_iopins_configure(const usb_phy_otg_io_conf_t *otg_io_conf, const usb_otg_signal_conn_t *otg_sig)
{
    ESP_RETURN_ON_FALSE(otg_io_conf && otg_sig, ESP_ERR_INVALID_ARG, USBPHY_TAG, "argument is invalid");
    esp_err_t ret = ESP_OK;

    // Inputs
    ret |= phy_configure_pin_input(otg_io_conf->iddig_io_num,       otg_sig->iddig);
    ret |= phy_configure_pin_input(otg_io_conf->avalid_io_num,      otg_sig->avalid);
    ret |= phy_configure_pin_input(otg_io_conf->vbusvalid_io_num,   otg_sig->vbusvalid);
    ret |= phy_configure_pin_input(otg_io_conf->bvalid_io_num,      otg_sig->bvalid);
    ret |= phy_configure_pin_input(otg_io_conf->sessend_io_num,     otg_sig->sessend);

    // Outputs
    ret |= phy_configure_pin_output(otg_io_conf->idpullup_io_num,    otg_sig->idpullup);
    ret |= phy_configure_pin_output(otg_io_conf->dppulldown_io_num,  otg_sig->dppulldown);
    ret |= phy_configure_pin_output(otg_io_conf->dmpulldown_io_num,  otg_sig->dmpulldown);
    ret |= phy_configure_pin_output(otg_io_conf->drvvbus_io_num,     otg_sig->drvvbus);
    ret |= phy_configure_pin_output(otg_io_conf->chrgvbus_io_num,    otg_sig->chrgvbus);
    ret |= phy_configure_pin_output(otg_io_conf->dischrgvbus_io_num, otg_sig->dischrgvbus);

    return ret;
}

esp_err_t usb_phy_otg_set_mode(usb_phy_handle_t handle, usb_otg_mode_t mode)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, USBPHY_TAG, "handle argument is invalid");
    ESP_RETURN_ON_FALSE(mode < USB_OTG_MODE_MAX, ESP_ERR_INVALID_ARG, USBPHY_TAG, "mode argument is invalid");
    ESP_RETURN_ON_FALSE(handle->controller == USB_PHY_CTRL_OTG, ESP_FAIL, USBPHY_TAG, "phy source is not USB_OTG");

    handle->otg_mode = mode;
    // On targets with multiple internal PHYs (FSLS and UTMI)
    // we support only fixed PHY to USB-DWC mapping:
    // USB-DWC2.0 <-> UTMI PHY
    // USB-DWC1.1 <-> FSLS PHY
    if (handle->target == USB_PHY_TARGET_UTMI) {
        return ESP_OK; // No need to configure anything for UTMI PHY
    }

    const usb_otg_signal_conn_t *otg_sig = usb_dwc_info.controllers[otg11_index].otg_signals;
    assert(otg_sig);
    if (mode == USB_OTG_MODE_HOST) {
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, otg_sig->iddig, false);     // connected connector is A side
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, otg_sig->bvalid, false);
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT,  otg_sig->vbusvalid, false);  // receiving a valid Vbus from host
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT,  otg_sig->avalid, false);     // HIGH to force USB host mode
        if (handle->target == USB_PHY_TARGET_INT) {
            // Configure pull resistors for host
            usb_wrap_pull_override_vals_t vals = {
                .dp_pu = false,
                .dm_pu = false,
                .dp_pd = true,
                .dm_pd = true,
            };
            usb_wrap_hal_phy_enable_pull_override(&handle->wrap_hal, &vals);
        }
    } else if (mode == USB_OTG_MODE_DEVICE) {
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT,  otg_sig->iddig, false);      // connected connector is mini-B side
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT,  otg_sig->bvalid, false);     // HIGH to force USB device mode
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT,  otg_sig->vbusvalid, false);  // receiving a valid Vbus from device
        esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, otg_sig->avalid, false);
    }

    return ESP_OK;
}

esp_err_t usb_phy_otg_dev_set_speed(usb_phy_handle_t handle, usb_phy_speed_t speed)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, USBPHY_TAG, "handle argument is invalid");
    ESP_RETURN_ON_FALSE(speed < USB_PHY_SPEED_MAX, ESP_ERR_INVALID_ARG, USBPHY_TAG, "speed argument is invalid");
    ESP_RETURN_ON_FALSE(handle->controller == USB_PHY_CTRL_OTG, ESP_FAIL, USBPHY_TAG, "phy source is not USB_OTG");
    ESP_RETURN_ON_FALSE((handle->target != USB_PHY_TARGET_EXT && handle->otg_mode == USB_OTG_MODE_DEVICE), ESP_FAIL,
                        USBPHY_TAG, "set speed not supported");
    ESP_RETURN_ON_FALSE((handle->target == USB_PHY_TARGET_UTMI) == (speed == USB_PHY_SPEED_HIGH), ESP_ERR_NOT_SUPPORTED, USBPHY_TAG, "UTMI can be HighSpeed only"); // This is our software limitation

    handle->otg_speed = speed;
    if (handle->target == USB_PHY_TARGET_UTMI) {
        return ESP_OK; // No need to configure anything for UTMI PHY
    }

    // Configure pull resistors for device
    usb_wrap_pull_override_vals_t vals = {
        .dp_pd = false,
        .dm_pd = false,
    };
    if (speed == USB_PHY_SPEED_LOW) {
        vals.dp_pu = false;
        vals.dm_pu = true;
    } else {
        vals.dp_pu = true;
        vals.dm_pu = false;
    }
    usb_wrap_hal_phy_enable_pull_override(&handle->wrap_hal, &vals);
    return ESP_OK;
}

esp_err_t usb_phy_action(usb_phy_handle_t handle, usb_phy_action_t action)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, USBPHY_TAG, "handle argument is invalid");
    ESP_RETURN_ON_FALSE(handle->target != USB_PHY_TARGET_UTMI, ESP_ERR_NOT_SUPPORTED, USBPHY_TAG, "Operation not supported on UTMI PHY");
    ESP_RETURN_ON_FALSE(action < USB_PHY_ACTION_MAX, ESP_ERR_INVALID_ARG, USBPHY_TAG, "action argument is invalid");
    ESP_RETURN_ON_FALSE((action == USB_PHY_ACTION_HOST_ALLOW_CONN && handle->controller == USB_PHY_CTRL_OTG) ||
                        (action == USB_PHY_ACTION_HOST_FORCE_DISCONN && handle->controller == USB_PHY_CTRL_OTG),
                        ESP_ERR_INVALID_ARG, USBPHY_TAG, "wrong target for the action");

    esp_err_t ret = ESP_OK;
    const usb_fsls_serial_signal_conn_t *fsls_sig = usb_dwc_info.controllers[otg11_index].fsls_signals;
    assert(fsls_sig);

    switch (action) {
    case USB_PHY_ACTION_HOST_ALLOW_CONN:
        if (handle->target == USB_PHY_TARGET_INT) {
            usb_wrap_hal_phy_enable_test_mode(&handle->wrap_hal, false);
        } else {
            if (!handle->iopins) {
                ret = ESP_FAIL;
                ESP_LOGE(USBPHY_TAG, "no I/O pins provided for connection");
                break;
            }
            /*
            Allow for connections on the external PHY by connecting the VP and VM signals to the external PHY.
            */
            esp_rom_gpio_connect_in_signal(handle->iopins->vp_io_num, fsls_sig->rx_dp, false);
            esp_rom_gpio_connect_in_signal(handle->iopins->vm_io_num, fsls_sig->rx_dm, false);
        }
        break;

    case USB_PHY_ACTION_HOST_FORCE_DISCONN:
        if (handle->target == USB_PHY_TARGET_INT) {
            /*
            We mimic a disconnect by enabling the internal PHY's test mode,
            then forcing the output_enable to HIGH. This will cause the received
            VP and VM to be zero, thus mimicking a disconnection.
            */
            const usb_wrap_test_mode_vals_t vals = {
                .tx_enable_n = true,
                .tx_dp = false,
                .tx_dm = false,
                .rx_dp = false,
                .rx_dm = false,
                .rx_rcv = false,
            };
            usb_wrap_hal_phy_test_mode_set_signals(&handle->wrap_hal, &vals);
            usb_wrap_hal_phy_enable_test_mode(&handle->wrap_hal, true);
        } else {
            /*
            Disable connections on the external PHY by connecting the VP and VM signals to the constant LOW signal.
            */
            esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, fsls_sig->rx_dp, false);
            esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ZERO_INPUT, fsls_sig->rx_dm, false);
        }
        break;

    default:
        break;
    }

    return ret;
}

static esp_err_t usb_phy_install(void)
{
    PHY_ENTER_CRITICAL();
    if (p_phy_ctrl_obj) {
        // p_phy_ctrl_obj already installed, return immediately
        PHY_EXIT_CRITICAL();
        return ESP_OK;
    }
    PHY_EXIT_CRITICAL();

    esp_err_t ret = ESP_OK;
    phy_ctrl_obj_t *phy_ctrl_obj = (phy_ctrl_obj_t *) calloc(1, sizeof(phy_ctrl_obj_t));
    ESP_GOTO_ON_FALSE(phy_ctrl_obj, ESP_ERR_NO_MEM, cleanup, USBPHY_TAG, "no mem for USB_PHY driver");

    PHY_ENTER_CRITICAL();
    if (!p_phy_ctrl_obj) {
        p_phy_ctrl_obj = phy_ctrl_obj;
        p_phy_ctrl_obj->ref_count = 0;
    } else {
        // p_phy_ctrl_obj already installed, need to free resource
        PHY_EXIT_CRITICAL();
        goto cleanup;
    }
    PHY_EXIT_CRITICAL();
    return ESP_OK;

cleanup:
    free(phy_ctrl_obj);
    return ret;
}

esp_err_t usb_new_phy(const usb_phy_config_t *config, usb_phy_handle_t *handle_ret)
{
    usb_phy_target_t phy_target = config->target;

    // Backward compatibility code:
    // Initial P4 device support was for USB-DWC HS and UTMI PHY.
    // To maintain backward compatibility on ESP32-P4 in USB Device mode, we select UTMI PHY
    // In case otg_speed is UNDEFINED or HIGH
#if CONFIG_IDF_TARGET_ESP32P4
    if (config->otg_mode == USB_OTG_MODE_DEVICE &&
            (config->otg_speed == USB_PHY_SPEED_UNDEFINED || config->otg_speed == USB_PHY_SPEED_HIGH)) {
        if (phy_target != USB_PHY_TARGET_UTMI) {
            ESP_LOGW(USBPHY_TAG, "Using UTMI PHY instead of requested %s PHY", (phy_target == USB_PHY_TARGET_INT) ? "internal" : "external");
            phy_target = USB_PHY_TARGET_UTMI;
        }
    }
#endif

    ESP_RETURN_ON_FALSE(config, ESP_ERR_INVALID_ARG, USBPHY_TAG, "config argument is invalid");
    ESP_RETURN_ON_FALSE(phy_target < USB_PHY_TARGET_MAX, ESP_ERR_INVALID_ARG, USBPHY_TAG, "specified PHY argument is invalid");
    ESP_RETURN_ON_FALSE(config->controller < USB_PHY_CTRL_MAX, ESP_ERR_INVALID_ARG, USBPHY_TAG, "specified source argument is invalid");
    ESP_RETURN_ON_FALSE(phy_target != USB_PHY_TARGET_EXT || config->ext_io_conf, ESP_ERR_INVALID_ARG, USBPHY_TAG, "ext_io_conf must be provided for ext PHY");
#if !USB_WRAP_LL_EXT_PHY_SUPPORTED
    ESP_RETURN_ON_FALSE(phy_target != USB_PHY_TARGET_EXT, ESP_ERR_NOT_SUPPORTED, USBPHY_TAG, "Ext PHY not supported on this target");
#endif
#if !SOC_USB_UTMI_PHY_NUM
    ESP_RETURN_ON_FALSE(phy_target != USB_PHY_TARGET_UTMI, ESP_ERR_NOT_SUPPORTED, USBPHY_TAG, "UTMI PHY not supported on this target");
#endif

    ESP_RETURN_ON_ERROR(usb_phy_install(), USBPHY_TAG, "usb_phy driver installation failed");
    esp_err_t ret = ESP_OK;
    bool new_phy = false;
    phy_context_t *phy_context = (phy_context_t *) calloc(1, sizeof(phy_context_t));
    ESP_GOTO_ON_FALSE(phy_context, ESP_ERR_NO_MEM, cleanup, USBPHY_TAG, "no mem for phy context");

    PHY_ENTER_CRITICAL();
    usb_phy_get_phy_status(phy_target, &phy_context->status);
    if (phy_context->status == USB_PHY_STATUS_FREE) {
        new_phy = true;
        p_phy_ctrl_obj->ref_count++;
        if (phy_target == USB_PHY_TARGET_EXT) {
            p_phy_ctrl_obj->external_phy = phy_context;
        } else if (phy_target == USB_PHY_TARGET_INT) {
            p_phy_ctrl_obj->fsls_phy = phy_context;
        } else { // USB_PHY_TARGET_UTMI
            p_phy_ctrl_obj->utmi_phy = phy_context;
        }
    }
    PHY_EXIT_CRITICAL();
    ESP_GOTO_ON_FALSE(new_phy, ESP_ERR_INVALID_STATE, cleanup, USBPHY_TAG, "selected PHY is in use");

    phy_context->target = phy_target;
    phy_context->controller = config->controller;
    phy_context->status = USB_PHY_STATUS_IN_USE;

    if (phy_target != USB_PHY_TARGET_UTMI) {
        USB_PHY_RCC_ATOMIC() {
            usb_wrap_hal_init(&phy_context->wrap_hal);
        }
    } else {
#if (SOC_USB_UTMI_PHY_NUM > 0)
        usb_utmi_hal_context_t utmi_hal_context; // Unused for now
        USB_PHY_RCC_ATOMIC() {
            usb_utmi_hal_init(&utmi_hal_context);
        }
#endif
    }
    if (config->controller == USB_PHY_CTRL_OTG) {
#if USB_WRAP_LL_EXT_PHY_SUPPORTED
        usb_wrap_hal_phy_set_external(&phy_context->wrap_hal, (phy_target == USB_PHY_TARGET_EXT));
#endif
    }

    // For FSLS PHY that shares pads with GPIO peripheral, we must set drive capability to 3 (40mA)
    if (phy_target == USB_PHY_TARGET_INT) {
        assert(usb_dwc_info.controllers[otg11_index].internal_phy_io);
        gpio_set_drive_capability(usb_dwc_info.controllers[otg11_index].internal_phy_io->dm, GPIO_DRIVE_CAP_3);
        gpio_set_drive_capability(usb_dwc_info.controllers[otg11_index].internal_phy_io->dp, GPIO_DRIVE_CAP_3);
    }

    *handle_ret = (usb_phy_handle_t) phy_context;
    if (phy_target == USB_PHY_TARGET_EXT) {
        phy_context->iopins = (usb_phy_ext_io_conf_t *) calloc(1, sizeof(usb_phy_ext_io_conf_t));
        ESP_GOTO_ON_FALSE(phy_context->iopins, ESP_ERR_NO_MEM, cleanup, USBPHY_TAG, "no mem for storing I/O pins");
        memcpy(phy_context->iopins, config->ext_io_conf, sizeof(usb_phy_ext_io_conf_t));
        const usb_fsls_serial_signal_conn_t *fsls_sig = usb_dwc_info.controllers[otg11_index].fsls_signals;
        ESP_ERROR_CHECK(phy_external_iopins_configure(phy_context->iopins, fsls_sig));
    }
    if (config->otg_mode != USB_PHY_MODE_DEFAULT) {
        ESP_ERROR_CHECK(usb_phy_otg_set_mode(*handle_ret, config->otg_mode));
    }
    if (config->otg_speed != USB_PHY_SPEED_UNDEFINED) {
        ESP_ERROR_CHECK(usb_phy_otg_dev_set_speed(*handle_ret, config->otg_speed));
    }
    if (config->otg_io_conf && (phy_context->controller == USB_PHY_CTRL_OTG)) {
        const usb_otg_signal_conn_t *otg_sig = usb_dwc_info.controllers[otg11_index].otg_signals;
        ESP_ERROR_CHECK(phy_otg_iopins_configure(config->otg_io_conf, otg_sig));
    }
    return ESP_OK;

cleanup:
    if (phy_context) {
        free(phy_context->iopins);
        free(phy_context);
    }
    if (p_phy_ctrl_obj->ref_count == 0) {
        free(p_phy_ctrl_obj);
        p_phy_ctrl_obj = NULL;
    }
    return ret;
}

static void phy_uninstall(void)
{
    phy_ctrl_obj_t *p_phy_ctrl_obj_free = NULL;
    PHY_ENTER_CRITICAL();
    if (p_phy_ctrl_obj->ref_count == 0) {
        p_phy_ctrl_obj_free = p_phy_ctrl_obj;
        p_phy_ctrl_obj = NULL;
        USB_PHY_RCC_ATOMIC() {
            // Disable USB peripheral without reset the module
            usb_wrap_hal_disable();
#if (SOC_USB_UTMI_PHY_NUM > 0)
            usb_utmi_hal_disable();
#endif
        }
    }
    PHY_EXIT_CRITICAL();
    free(p_phy_ctrl_obj_free);
}

esp_err_t usb_del_phy(usb_phy_handle_t handle)
{
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, USBPHY_TAG, "handle argument is invalid");

    PHY_ENTER_CRITICAL();
    p_phy_ctrl_obj->ref_count--;
    if (handle->target == USB_PHY_TARGET_EXT) {
        p_phy_ctrl_obj->external_phy = NULL;
    } else if (handle->target == USB_PHY_TARGET_INT) {
        // Clear pullup and pulldown loads on D+ / D-, and disable the pads
        usb_wrap_hal_phy_disable_pull_override(&handle->wrap_hal);
        p_phy_ctrl_obj->fsls_phy = NULL;
    } else { // USB_PHY_TARGET_UTMI
        p_phy_ctrl_obj->utmi_phy = NULL;
    }
    PHY_EXIT_CRITICAL();
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
    } else if (target == USB_PHY_TARGET_INT && p_phy_ctrl_obj->fsls_phy) {
        *status = p_phy_ctrl_obj->fsls_phy->status;
    } else if (target == USB_PHY_TARGET_UTMI && p_phy_ctrl_obj->utmi_phy) {
        *status = p_phy_ctrl_obj->utmi_phy->status;
    } else {
        *status = USB_PHY_STATUS_FREE;
    }
    return ESP_OK;
}
