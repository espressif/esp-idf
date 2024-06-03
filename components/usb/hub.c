/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "usb_private.h"
#include "hcd.h"
#include "hub.h"
#include "usb/usb_helpers.h"

/*
Implementation of the HUB driver that only supports the Root Hub with a single port. Therefore, we currently don't
implement the bare minimum to control the root HCD port.
*/

#define HUB_ROOT_PORT_NUM                           1  // HCD only supports one port
#define HUB_ROOT_DEV_UID                            1  // Unique device ID

#ifdef CONFIG_USB_HOST_HW_BUFFER_BIAS_IN
#define HUB_ROOT_HCD_PORT_FIFO_BIAS                 HCD_PORT_FIFO_BIAS_RX
#elif CONFIG_USB_HOST_HW_BUFFER_BIAS_PERIODIC_OUT
#define HUB_ROOT_HCD_PORT_FIFO_BIAS                 HCD_PORT_FIFO_BIAS_PTX
#else   // CONFIG_USB_HOST_HW_BUFFER_BIAS_BALANCED
#define HUB_ROOT_HCD_PORT_FIFO_BIAS                 HCD_PORT_FIFO_BIAS_BALANCED
#endif

// Hub driver action flags. LISTED IN THE ORDER THEY SHOULD BE HANDLED IN within hub_process(). Some actions are mutually exclusive
#define HUB_DRIVER_FLAG_ACTION_ROOT_EVENT           0x01
#define HUB_DRIVER_FLAG_ACTION_PORT_REQ             0x02

#define PORT_REQ_DISABLE                            0x01
#define PORT_REQ_RECOVER                            0x02

/**
 * @brief Root port states
 *
 */
typedef enum {
    ROOT_PORT_STATE_NOT_POWERED,    /**< Root port initialized and/or not powered */
    ROOT_PORT_STATE_POWERED,        /**< Root port is powered, device is not connected */
    ROOT_PORT_STATE_DISABLED,       /**< A device is connected but is disabled (i.e., not reset, no SOFs are sent) */
    ROOT_PORT_STATE_ENABLED,        /**< A device is connected, port has been reset, SOFs are sent */
    ROOT_PORT_STATE_RECOVERY,       /**< Root port encountered an error and needs to be recovered */
} root_port_state_t;

typedef struct {
    // Dynamic members require a critical section
    struct {
        union {
            struct {
                uint32_t actions: 8;
                uint32_t reserved24: 24;
            };
            uint32_t val;
        } flags;
        root_port_state_t root_port_state;
        unsigned int port_reqs;
    } dynamic;
    // Single thread members don't require a critical section so long as they are never accessed from multiple threads
    struct {
        unsigned int root_dev_uid;  // UID of the device connected to root port. 0 if no device connected
    } single_thread;
    // Constant members do no change after installation thus do not require a critical section
    struct {
        hcd_port_handle_t root_port_hdl;
        usb_proc_req_cb_t proc_req_cb;
        void *proc_req_cb_arg;
        hub_event_cb_t event_cb;
        void *event_cb_arg;
    } constant;
} hub_driver_t;

static hub_driver_t *p_hub_driver_obj = NULL;
static portMUX_TYPE hub_driver_lock = portMUX_INITIALIZER_UNLOCKED;

const char *HUB_DRIVER_TAG = "HUB";

#define HUB_DRIVER_ENTER_CRITICAL_ISR()                 portENTER_CRITICAL_ISR(&hub_driver_lock)
#define HUB_DRIVER_EXIT_CRITICAL_ISR()                  portEXIT_CRITICAL_ISR(&hub_driver_lock)
#define HUB_DRIVER_ENTER_CRITICAL()                     portENTER_CRITICAL(&hub_driver_lock)
#define HUB_DRIVER_EXIT_CRITICAL()                      portEXIT_CRITICAL(&hub_driver_lock)
#define HUB_DRIVER_ENTER_CRITICAL_SAFE()                portENTER_CRITICAL_SAFE(&hub_driver_lock)
#define HUB_DRIVER_EXIT_CRITICAL_SAFE()                 portEXIT_CRITICAL_SAFE(&hub_driver_lock)

#define HUB_DRIVER_CHECK(cond, ret_val) ({              \
            if (!(cond)) {                              \
                return (ret_val);                       \
            }                                           \
})
#define HUB_DRIVER_CHECK_FROM_CRIT(cond, ret_val) ({    \
            if (!(cond)) {                              \
                HUB_DRIVER_EXIT_CRITICAL();             \
                return ret_val;                         \
            }                                           \
})

// ------------------------------------------------- Forward Declare ---------------------------------------------------

/**
 * @brief HCD port callback for the root port
 *
 * - This callback is called from the context of the HCD, so any event handling should be deferred to hub_process()
 * - Under the current HCD implementation, this callback should only be ever be called in an ISR
 * - This callback needs to call proc_req_cb to ensure that hub_process() gets a chance to run
 *
 * @param port_hdl HCD port handle
 * @param port_event HCD port event
 * @param user_arg Callback argument
 * @param in_isr Whether callback is in an ISR context
 * @return Whether a yield is required
 */
static bool root_port_callback(hcd_port_handle_t port_hdl, hcd_port_event_t port_event, void *user_arg, bool in_isr);

// ---------------------- Callbacks ------------------------

static bool root_port_callback(hcd_port_handle_t port_hdl, hcd_port_event_t port_event, void *user_arg, bool in_isr)
{
    HUB_DRIVER_ENTER_CRITICAL_SAFE();
    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_ROOT_EVENT;
    HUB_DRIVER_EXIT_CRITICAL_SAFE();
    assert(in_isr); // Currently, this callback should only ever be called from an ISR context
    return p_hub_driver_obj->constant.proc_req_cb(USB_PROC_REQ_SOURCE_HUB, in_isr, p_hub_driver_obj->constant.proc_req_cb_arg);
}

// ---------------------- Handlers -------------------------

static void root_port_handle_events(hcd_port_handle_t root_port_hdl)
{
    hcd_port_event_t port_event = hcd_port_handle_event(root_port_hdl);
    hub_event_data_t event_data = { 0 };

    switch (port_event) {
    case HCD_PORT_EVENT_NONE:
        // Nothing to do
        break;
    case HCD_PORT_EVENT_CONNECTION: {
        if (hcd_port_command(root_port_hdl, HCD_PORT_CMD_RESET) != ESP_OK) {
            ESP_LOGE(HUB_DRIVER_TAG, "Root port reset failed");
            goto reset_err;
        }
        ESP_LOGD(HUB_DRIVER_TAG, "Root port reset");
        usb_speed_t speed;
        if (hcd_port_get_speed(p_hub_driver_obj->constant.root_port_hdl, &speed) != ESP_OK) {
            goto new_dev_err;
        }

        // Allocate a new device. We use a fixed HUB_ROOT_DEV_UID for now since we only support a single device
        usbh_dev_params_t params = {
            .uid = HUB_ROOT_DEV_UID,
            .speed = speed,
            .root_port_hdl = p_hub_driver_obj->constant.root_port_hdl,
        };

        if (usbh_devs_add(&params) != ESP_OK) {
            ESP_LOGE(HUB_DRIVER_TAG, "Failed to add device");
            goto new_dev_err;
        }
        // Save uid to Port
        p_hub_driver_obj->single_thread.root_dev_uid = HUB_ROOT_DEV_UID;
        // Change Port state
        HUB_DRIVER_ENTER_CRITICAL();
        p_hub_driver_obj->dynamic.root_port_state = ROOT_PORT_STATE_ENABLED;
        HUB_DRIVER_EXIT_CRITICAL();

        event_data.event = HUB_EVENT_CONNECTED;
        event_data.connected.uid = p_hub_driver_obj->single_thread.root_dev_uid;
        p_hub_driver_obj->constant.event_cb(&event_data, p_hub_driver_obj->constant.event_cb_arg);
        break;
new_dev_err:
        // We allow this to fail in case a disconnect/port error happens while disabling.
        hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_DISABLE);
reset_err:
        break;
    }
    case HCD_PORT_EVENT_DISCONNECTION:
    case HCD_PORT_EVENT_ERROR:
    case HCD_PORT_EVENT_OVERCURRENT: {
        bool port_has_device = false;
        HUB_DRIVER_ENTER_CRITICAL();
        switch (p_hub_driver_obj->dynamic.root_port_state) {
        case ROOT_PORT_STATE_POWERED: // This occurred before enumeration
        case ROOT_PORT_STATE_DISABLED: // This occurred after the device has already been disabled
            // Therefore, there's no device object to clean up, and we can go straight to port recovery
            p_hub_driver_obj->dynamic.port_reqs |= PORT_REQ_RECOVER;
            p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_PORT_REQ;
            break;
        case ROOT_PORT_STATE_ENABLED:
            // There is an enabled (active) device. We need to indicate to USBH that the device is gone
            port_has_device = true;
            break;
        default:
            abort();    // Should never occur
            break;
        }
        HUB_DRIVER_EXIT_CRITICAL();
        if (port_has_device) {
            // The port must have a device object
            assert(p_hub_driver_obj->single_thread.root_dev_uid != 0);

            event_data.event = HUB_EVENT_DISCONNECTED;
            event_data.disconnected.uid = p_hub_driver_obj->single_thread.root_dev_uid;
            p_hub_driver_obj->constant.event_cb(&event_data, p_hub_driver_obj->constant.event_cb_arg);
        }
        break;
    }
    default:
        abort();    // Should never occur
        break;
    }
}

static void root_port_req(hcd_port_handle_t root_port_hdl)
{
    unsigned int port_reqs;

    HUB_DRIVER_ENTER_CRITICAL();
    port_reqs = p_hub_driver_obj->dynamic.port_reqs;
    p_hub_driver_obj->dynamic.port_reqs = 0;
    HUB_DRIVER_EXIT_CRITICAL();

    if (port_reqs & PORT_REQ_DISABLE) {
        ESP_LOGD(HUB_DRIVER_TAG, "Disabling root port");
        // We allow this to fail in case a disconnect/port error happens while disabling.
        hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_DISABLE);
    }
    if (port_reqs & PORT_REQ_RECOVER) {
        ESP_LOGD(HUB_DRIVER_TAG, "Recovering root port");
        ESP_ERROR_CHECK(hcd_port_recover(p_hub_driver_obj->constant.root_port_hdl));
        ESP_ERROR_CHECK(hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_POWER_ON));
        HUB_DRIVER_ENTER_CRITICAL();
        p_hub_driver_obj->dynamic.root_port_state = ROOT_PORT_STATE_POWERED;
        HUB_DRIVER_EXIT_CRITICAL();
    }
}

static esp_err_t root_port_recycle(void)
{
    // Device is free, we can now request its port be recycled
    hcd_port_state_t port_state = hcd_port_get_state(p_hub_driver_obj->constant.root_port_hdl);
    HUB_DRIVER_ENTER_CRITICAL();
    // How the port is recycled will depend on the port's state
    switch (port_state) {
    case HCD_PORT_STATE_ENABLED:
        p_hub_driver_obj->dynamic.port_reqs |= PORT_REQ_DISABLE;
        break;
    case HCD_PORT_STATE_RECOVERY:
        p_hub_driver_obj->dynamic.port_reqs |= PORT_REQ_RECOVER;
        break;
    default:
        abort();    // Should never occur
        break;
    }
    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_PORT_REQ;
    HUB_DRIVER_EXIT_CRITICAL();

    p_hub_driver_obj->constant.proc_req_cb(USB_PROC_REQ_SOURCE_HUB, false, p_hub_driver_obj->constant.proc_req_cb_arg);

    return ESP_OK;
}

// ---------------------------------------------- Hub Driver Functions -------------------------------------------------

esp_err_t hub_install(hub_config_t *hub_config, void **client_ret)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj == NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_EXIT_CRITICAL();
    esp_err_t ret;

    // Allocate Hub driver object
    hub_driver_t *hub_driver_obj = heap_caps_calloc(1, sizeof(hub_driver_t), MALLOC_CAP_DEFAULT);
    if (hub_driver_obj == NULL) {
        return ESP_ERR_NO_MEM;
    }

    *client_ret = NULL;

    // Install HCD port
    hcd_port_config_t port_config = {
        .fifo_bias = HUB_ROOT_HCD_PORT_FIFO_BIAS,
        .callback = root_port_callback,
        .callback_arg = NULL,
        .context = NULL,
    };
    hcd_port_handle_t root_port_hdl;
    ret = hcd_port_init(HUB_ROOT_PORT_NUM, &port_config, &root_port_hdl);
    if (ret != ESP_OK) {
        goto err;
    }

    // Initialize Hub driver object
    hub_driver_obj->constant.root_port_hdl = root_port_hdl;
    hub_driver_obj->constant.proc_req_cb = hub_config->proc_req_cb;
    hub_driver_obj->constant.proc_req_cb_arg = hub_config->proc_req_cb_arg;
    hub_driver_obj->constant.event_cb = hub_config->event_cb;
    hub_driver_obj->constant.event_cb_arg = hub_config->event_cb_arg;

    HUB_DRIVER_ENTER_CRITICAL();
    hub_driver_obj->dynamic.root_port_state = ROOT_PORT_STATE_NOT_POWERED;
    if (p_hub_driver_obj != NULL) {
        HUB_DRIVER_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto assign_err;
    }
    p_hub_driver_obj = hub_driver_obj;
    HUB_DRIVER_EXIT_CRITICAL();

    return ret;

assign_err:
    ESP_ERROR_CHECK(hcd_port_deinit(root_port_hdl));
err:
    heap_caps_free(hub_driver_obj);
    return ret;
}

esp_err_t hub_uninstall(void)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj != NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj->dynamic.root_port_state == ROOT_PORT_STATE_NOT_POWERED, ESP_ERR_INVALID_STATE);
    hub_driver_t *hub_driver_obj = p_hub_driver_obj;
    p_hub_driver_obj = NULL;
    HUB_DRIVER_EXIT_CRITICAL();

    ESP_ERROR_CHECK(hcd_port_deinit(hub_driver_obj->constant.root_port_hdl));
    // Free Hub driver resources
    heap_caps_free(hub_driver_obj);
    return ESP_OK;
}

esp_err_t hub_root_start(void)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj != NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj->dynamic.root_port_state == ROOT_PORT_STATE_NOT_POWERED, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_EXIT_CRITICAL();
    // Power ON the root port
    esp_err_t ret;
    ret = hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_POWER_ON);
    if (ret == ESP_OK) {
        HUB_DRIVER_ENTER_CRITICAL();
        p_hub_driver_obj->dynamic.root_port_state = ROOT_PORT_STATE_POWERED;
        HUB_DRIVER_EXIT_CRITICAL();
    }
    return ret;
}

esp_err_t hub_root_stop(void)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj != NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj->dynamic.root_port_state != ROOT_PORT_STATE_NOT_POWERED, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_EXIT_CRITICAL();
    esp_err_t ret;
    ret = hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_POWER_OFF);
    if (ret == ESP_OK) {
        HUB_DRIVER_ENTER_CRITICAL();
        p_hub_driver_obj->dynamic.root_port_state = ROOT_PORT_STATE_NOT_POWERED;
        HUB_DRIVER_EXIT_CRITICAL();
    }
    return ret;
}

esp_err_t hub_port_recycle(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num, unsigned int dev_uid)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj != NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_EXIT_CRITICAL();

    esp_err_t ret = ESP_FAIL;

    if (parent_port_num == 0) {
        if (p_hub_driver_obj->single_thread.root_dev_uid) {
            // If root port has a device, it should be with correct uid
            assert(dev_uid == p_hub_driver_obj->single_thread.root_dev_uid);
            p_hub_driver_obj->single_thread.root_dev_uid = 0;
        }
        ret = root_port_recycle();
    } else {
        ESP_LOGW(HUB_DRIVER_TAG, "Recycling External Port has not been implemented yet");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ret;
}

esp_err_t hub_port_reset(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj != NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_EXIT_CRITICAL();

    esp_err_t ret = ESP_FAIL;

    if (parent_port_num == 0) {
        // The port must have a device object
        assert(p_hub_driver_obj->single_thread.root_dev_uid != 0);

        ret = hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_RESET);
        if (ret != ESP_OK) {
            ESP_LOGE(HUB_DRIVER_TAG, "Failed to issue root port reset");
        }

        hub_event_data_t event_data = {
            .event = HUB_EVENT_RESET_COMPLETED,
            .disconnected = {
                .uid = p_hub_driver_obj->single_thread.root_dev_uid,
            },
        };
        p_hub_driver_obj->constant.event_cb(&event_data, p_hub_driver_obj->constant.event_cb_arg);
    } else {
        ESP_LOGW(HUB_DRIVER_TAG, "Reset External Port has not been implemented yet");
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ret;
}

esp_err_t hub_process(void)
{
    HUB_DRIVER_ENTER_CRITICAL();
    uint32_t action_flags = p_hub_driver_obj->dynamic.flags.actions;
    p_hub_driver_obj->dynamic.flags.actions = 0;
    HUB_DRIVER_EXIT_CRITICAL();

    while (action_flags) {
        if (action_flags & HUB_DRIVER_FLAG_ACTION_ROOT_EVENT) {
            root_port_handle_events(p_hub_driver_obj->constant.root_port_hdl);
        }
        if (action_flags & HUB_DRIVER_FLAG_ACTION_PORT_REQ) {
            root_port_req(p_hub_driver_obj->constant.root_port_hdl);
        }

        HUB_DRIVER_ENTER_CRITICAL();
        action_flags = p_hub_driver_obj->dynamic.flags.actions;
        p_hub_driver_obj->dynamic.flags.actions = 0;
        HUB_DRIVER_EXIT_CRITICAL();
    }
    return ESP_OK;
}
