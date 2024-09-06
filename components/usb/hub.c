/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/queue.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "usb_private.h"
#include "hcd.h"
#include "hub.h"
#include "usb/usb_helpers.h"

#if ENABLE_USB_HUBS
#include "ext_hub.h"
#endif // ENABLE_USB_HUBS

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

#define PORT_REQ_DISABLE                            0x01
#define PORT_REQ_RECOVER                            0x02

/**
 * @brief Hub driver action flags
 */
typedef enum {
    HUB_DRIVER_ACTION_ROOT_EVENT          = (1 << 0),
    HUB_DRIVER_ACTION_ROOT_REQ            = (1 << 1),
#if ENABLE_USB_HUBS
    HUB_DRIVER_ACTION_EXT_HUB             = (1 << 6),
    HUB_DRIVER_ACTION_EXT_PORT            = (1 << 7)
#endif // ENABLE_USB_HUBS
} hub_flag_action_t;

/**
 * @brief Root port states
 */
typedef enum {
    ROOT_PORT_STATE_NOT_POWERED,    /**< Root port initialized and/or not powered */
    ROOT_PORT_STATE_POWERED,        /**< Root port is powered, device is not connected */
    ROOT_PORT_STATE_DISABLED,       /**< A device is connected but is disabled (i.e., not reset, no SOFs are sent) */
    ROOT_PORT_STATE_ENABLED,        /**< A device is connected, port has been reset, SOFs are sent */
    ROOT_PORT_STATE_RECOVERY,       /**< Root port encountered an error and needs to be recovered */
} root_port_state_t;

/**
 * @brief Hub device tree node
 *
 * Object type of a node in the USB device tree that is maintained by the Hub driver
 */
struct dev_tree_node_s {
    TAILQ_ENTRY(dev_tree_node_s) tailq_entry;   /**< Entry for the device tree node object tailq */
    unsigned int uid;                       /**< Device's unique ID */
    usb_device_handle_t parent_dev_hdl;     /**< Device's parent handle */
    uint8_t parent_port_num;                /**< Device's parent port number */
};

typedef struct dev_tree_node_s dev_tree_node_t;

typedef struct {
    struct {
        union {
            struct {
                hub_flag_action_t actions: 8;       /**< Hub actions */
                uint32_t reserved24: 24;            /**< Reserved */
            };
            uint32_t val;                           /**< Hub flag action value */
        } flags;                                    /**< Hub flags */
        root_port_state_t root_port_state;          /**< Root port state */
        unsigned int port_reqs;                     /**< Root port request flag */
    } dynamic;                                      /**< Dynamic members. Require a critical section */

    struct {
        TAILQ_HEAD(tailhead_devs, dev_tree_node_s) dev_nodes_tailq;     /**< Tailq of attached devices */
        unsigned int next_uid;                      /**< Unique ID for next upcoming device */
    } single_thread;                                /**< Single thread members don't require a critical section so long as they are never accessed from multiple threads */

    struct {
        hcd_port_handle_t root_port_hdl;            /**< Root port HCD handle */
        usb_proc_req_cb_t proc_req_cb;              /**< Process request callback */
        void *proc_req_cb_arg;                      /**< Process request callback argument */
        hub_event_cb_t event_cb;                    /**< Hub Driver event callback */
        void *event_cb_arg;                         /**< Event callback argument */
    } constant;                                     /**< Constant members. Do not change after installation thus do not require a critical section or mutex */
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

// ---------------------- Internal Logic ------------------------

/**
 * @brief Creates new device tree node and propagate HUB_EVENT_CONNECTED event
 *
 * @return esp_err_t
 */
static esp_err_t new_dev_tree_node(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num, usb_speed_t speed)
{
    esp_err_t ret;
    unsigned int node_uid = p_hub_driver_obj->single_thread.next_uid;

    dev_tree_node_t *dev_tree_node = heap_caps_calloc(1, sizeof(dev_tree_node_t), MALLOC_CAP_DEFAULT);
    if (dev_tree_node == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // Allocate a new USBH device
    usbh_dev_params_t params = {
        .uid = node_uid,
        .speed = speed,
        .root_port_hdl = p_hub_driver_obj->constant.root_port_hdl, // Always the same for all devices
        // TODO: IDF-10023 Move responsibility of parent-child tree building to Hub Driver instead of USBH
        .parent_dev_hdl = parent_dev_hdl,
        .parent_port_num = parent_port_num,
    };

    ret = usbh_devs_add(&params);
    if (ret != ESP_OK) {
        // USBH devs add could failed due to lack of free hcd channels
        // TODO: IDF-10044 Hub should recover after running out of hcd channels
        goto fail;
    }

    dev_tree_node->uid = node_uid;
    dev_tree_node->parent_dev_hdl = parent_dev_hdl;
    dev_tree_node->parent_port_num = parent_port_num;
    TAILQ_INSERT_TAIL(&p_hub_driver_obj->single_thread.dev_nodes_tailq, dev_tree_node, tailq_entry);

    p_hub_driver_obj->single_thread.next_uid++;
    if (p_hub_driver_obj->single_thread.next_uid == 0) {
        ESP_LOGW(HUB_DRIVER_TAG, "Counter overflowed, possibility of uid collisions");
        p_hub_driver_obj->single_thread.next_uid = HUB_ROOT_DEV_UID;
    }
    // Verify presence of a device with newly prepared uid in USBH
    // TODO: IDF-10022 Provide a mechanism to request presence status of a device with uid in USBH device object list
    // Return if device uid is not in USBH device object list, repeat until uid will be founded

    ESP_LOGD(HUB_DRIVER_TAG, "New device tree node (uid=%d)", node_uid);

    hub_event_data_t event_data = {
        .event = HUB_EVENT_CONNECTED,
        .connected = {
            .uid = node_uid,
        },
    };
    p_hub_driver_obj->constant.event_cb(&event_data, p_hub_driver_obj->constant.event_cb_arg);

    return ret;

fail:
    heap_caps_free(dev_tree_node);
    return ret;
}

static esp_err_t dev_tree_node_reset_completed(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num)
{
    dev_tree_node_t *dev_tree_node = NULL;
    dev_tree_node_t *dev_tree_iter;
    // Search the device tree nodes list for a device node with the specified parent
    TAILQ_FOREACH(dev_tree_iter, &p_hub_driver_obj->single_thread.dev_nodes_tailq, tailq_entry) {
        if (dev_tree_iter->parent_dev_hdl == parent_dev_hdl &&
                dev_tree_iter->parent_port_num == parent_port_num) {
            dev_tree_node = dev_tree_iter;
            break;
        }
    }

    if (dev_tree_node == NULL) {
        ESP_LOGE(HUB_DRIVER_TAG, "Reset completed, but device tree node with port=%d not found", parent_port_num);
        return ESP_ERR_NOT_FOUND;
    }

    hub_event_data_t event_data = {
        .event = HUB_EVENT_RESET_COMPLETED,
        .reset_completed = {
            .uid = dev_tree_node->uid,
        },
    };
    p_hub_driver_obj->constant.event_cb(&event_data, p_hub_driver_obj->constant.event_cb_arg);
    return ESP_OK;
}

static esp_err_t dev_tree_node_dev_gone(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num)
{
    dev_tree_node_t *dev_tree_node = NULL;
    dev_tree_node_t *dev_tree_iter;
    // Search the device tree nodes list for a device node with the specified parent
    TAILQ_FOREACH(dev_tree_iter, &p_hub_driver_obj->single_thread.dev_nodes_tailq, tailq_entry) {
        if (dev_tree_iter->parent_dev_hdl == parent_dev_hdl &&
                dev_tree_iter->parent_port_num == parent_port_num) {
            dev_tree_node = dev_tree_iter;
            break;
        }
    }

    if (dev_tree_node == NULL) {
        ESP_LOGE(HUB_DRIVER_TAG, "Device tree node with port=%d not found", parent_port_num);
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGD(HUB_DRIVER_TAG, "Device tree node (uid=%d): device gone", dev_tree_node->uid);

    hub_event_data_t event_data = {
        .event = HUB_EVENT_DISCONNECTED,
        .disconnected = {
            .uid = dev_tree_node->uid,
        },
    };
    p_hub_driver_obj->constant.event_cb(&event_data, p_hub_driver_obj->constant.event_cb_arg);

    return ESP_OK;
}

/**
 * @brief Frees device tree node
 *
 * @return esp_err_t
 */
static esp_err_t dev_tree_node_remove_by_parent(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num)
{
    dev_tree_node_t *dev_tree_node = NULL;
    dev_tree_node_t *dev_tree_iter;
    // Search the device tree nodes list for a device node with the specified parent
    TAILQ_FOREACH(dev_tree_iter, &p_hub_driver_obj->single_thread.dev_nodes_tailq, tailq_entry) {
        if (dev_tree_iter->parent_dev_hdl == parent_dev_hdl &&
                dev_tree_iter->parent_port_num == parent_port_num) {
            dev_tree_node = dev_tree_iter;
            break;
        }
    }

    if (dev_tree_node == NULL) {
        ESP_LOGE(HUB_DRIVER_TAG, "Device tree node with port=%d not found", parent_port_num);
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGD(HUB_DRIVER_TAG, "Device tree node freeing (uid=%d)", dev_tree_node->uid);

    TAILQ_REMOVE(&p_hub_driver_obj->single_thread.dev_nodes_tailq, dev_tree_node, tailq_entry);
    heap_caps_free(dev_tree_node);
    return ESP_OK;
}

// ---------------------- Callbacks ------------------------

static bool root_port_callback(hcd_port_handle_t port_hdl, hcd_port_event_t port_event, void *user_arg, bool in_isr)
{
    HUB_DRIVER_ENTER_CRITICAL_SAFE();
    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_ACTION_ROOT_EVENT;
    HUB_DRIVER_EXIT_CRITICAL_SAFE();
    assert(in_isr); // Currently, this callback should only ever be called from an ISR context
    return p_hub_driver_obj->constant.proc_req_cb(USB_PROC_REQ_SOURCE_HUB, in_isr, p_hub_driver_obj->constant.proc_req_cb_arg);
}

#ifdef ENABLE_USB_HUBS
static bool ext_hub_callback(bool in_isr, void *user_arg)
{
    HUB_DRIVER_ENTER_CRITICAL_SAFE();
    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_ACTION_EXT_HUB;
    HUB_DRIVER_EXIT_CRITICAL_SAFE();
    return p_hub_driver_obj->constant.proc_req_cb(USB_PROC_REQ_SOURCE_HUB, in_isr, p_hub_driver_obj->constant.proc_req_cb_arg);
}
#endif // ENABLE_USB_HUBS

// ---------------------- Handlers -------------------------
static void root_port_handle_events(hcd_port_handle_t root_port_hdl)
{
    hcd_port_event_t port_event = hcd_port_handle_event(root_port_hdl);
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

        if (new_dev_tree_node(NULL, 0, speed) != ESP_OK) {
            ESP_LOGE(HUB_DRIVER_TAG, "Failed to add new device");
            goto new_dev_err;
        }

        // Change Port state
        HUB_DRIVER_ENTER_CRITICAL();
        p_hub_driver_obj->dynamic.root_port_state = ROOT_PORT_STATE_ENABLED;
        HUB_DRIVER_EXIT_CRITICAL();
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
            p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_ACTION_ROOT_REQ;
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
            ESP_ERROR_CHECK(dev_tree_node_dev_gone(NULL, 0));
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
    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_ACTION_ROOT_REQ;
    HUB_DRIVER_EXIT_CRITICAL();

    ESP_ERROR_CHECK(dev_tree_node_remove_by_parent(NULL, 0));

    p_hub_driver_obj->constant.proc_req_cb(USB_PROC_REQ_SOURCE_HUB, false, p_hub_driver_obj->constant.proc_req_cb_arg);

    return ESP_OK;
}

static esp_err_t root_port_disable(void)
{
    hcd_port_state_t port_state = hcd_port_get_state(p_hub_driver_obj->constant.root_port_hdl);
    HUB_DRIVER_CHECK(port_state == HCD_PORT_STATE_ENABLED, ESP_ERR_INVALID_STATE);

    HUB_DRIVER_ENTER_CRITICAL();
    p_hub_driver_obj->dynamic.port_reqs |= PORT_REQ_DISABLE;
    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_ACTION_ROOT_REQ;
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

#if ENABLE_USB_HUBS
    // Install External HUB driver
    ext_hub_config_t ext_hub_config = {
        .proc_req_cb = ext_hub_callback,
        .port_driver = NULL,
    };
    ret = ext_hub_install(&ext_hub_config);
    if (ret != ESP_OK) {
        goto err_ext_hub;
    }
    *client_ret = ext_hub_get_client();
#else
    *client_ret = NULL;
#endif // ENABLE_USB_HUBS

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
    hub_driver_obj->single_thread.next_uid = HUB_ROOT_DEV_UID;
    TAILQ_INIT(&hub_driver_obj->single_thread.dev_nodes_tailq);
    // Driver is not installed, we can modify dynamic section outside of the critical section
    hub_driver_obj->dynamic.root_port_state = ROOT_PORT_STATE_NOT_POWERED;

    HUB_DRIVER_ENTER_CRITICAL();
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
#if ENABLE_USB_HUBS
    ext_hub_uninstall();
err_ext_hub:
#endif // ENABLE_USB_HUBS
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

#if ENABLE_USB_HUBS
    ESP_ERROR_CHECK(ext_hub_uninstall());
#endif // ENABLE_USB_HUBS

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
    esp_err_t ret;

    if (parent_port_num == 0) {
        ret = root_port_recycle();
    } else {
#if ENABLE_USB_HUBS
        ext_hub_handle_t ext_hub_hdl = NULL;
        ext_hub_get_handle(parent_dev_hdl, &ext_hub_hdl);
        ret = ext_hub_port_recycle(ext_hub_hdl, parent_port_num);
#else
        ESP_LOGW(HUB_DRIVER_TAG, "Recycling External Port is not available (External Hub support disabled)");
        ret = ESP_ERR_NOT_SUPPORTED;
#endif // ENABLE_USB_HUBS
    }

    return ret;
}

esp_err_t hub_port_reset(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj != NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_EXIT_CRITICAL();
    esp_err_t ret;

    if (parent_port_num == 0) {
        ret = hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_RESET);
        if (ret != ESP_OK) {
            ESP_LOGE(HUB_DRIVER_TAG, "Failed to issue root port reset");
        } else {
            ret = dev_tree_node_reset_completed(NULL, 0);
        }
    } else {
#if ENABLE_USB_HUBS
        ext_hub_handle_t ext_hub_hdl = NULL;
        ext_hub_get_handle(parent_dev_hdl, &ext_hub_hdl);
        ret = ext_hub_port_reset(ext_hub_hdl, parent_port_num);
#else
        ESP_LOGW(HUB_DRIVER_TAG, "Resetting External Port is not available (External Hub support disabled)");
        ret = ESP_ERR_NOT_SUPPORTED;
#endif // ENABLE_USB_HUBS
    }
    return ret;
}

esp_err_t hub_port_active(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num)
{
    esp_err_t ret;

    if (parent_port_num == 0) {
        // Root port no need to be activated
        ret = ESP_OK;
    } else {
#if ENABLE_USB_HUBS
        // External Hub port
        ext_hub_handle_t ext_hub_hdl = NULL;
        ext_hub_get_handle(parent_dev_hdl, &ext_hub_hdl);
        ret = ext_hub_port_active(ext_hub_hdl, parent_port_num);
#else
        ESP_LOGW(HUB_DRIVER_TAG, "Activating External Port is not available (External Hub support disabled)");
        ret = ESP_ERR_NOT_SUPPORTED;
#endif // ENABLE_USB_HUBS
    }
    return ret;
}

esp_err_t hub_port_disable(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num)
{
    esp_err_t ret;

    if (parent_port_num == 0) {
        ret = root_port_disable();
    } else {
#if ENABLE_USB_HUBS
        // External Hub port
        ext_hub_handle_t ext_hub_hdl = NULL;
        ext_hub_get_handle(parent_dev_hdl, &ext_hub_hdl);
        ret = ext_hub_port_disable(ext_hub_hdl, parent_port_num);
#else
        ESP_LOGW(HUB_DRIVER_TAG, "Activating External Port is not available (External Hub support disabled)");
        ret = ESP_ERR_NOT_SUPPORTED;
#endif // ENABLE_USB_HUBS
    }
    return ret;
}

#if ENABLE_USB_HUBS
esp_err_t hub_notify_new_dev(uint8_t dev_addr)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj != NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_EXIT_CRITICAL();

    return ext_hub_new_dev(dev_addr);
}

esp_err_t hub_notify_dev_gone(uint8_t dev_addr)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj != NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_EXIT_CRITICAL();

    return ext_hub_dev_gone(dev_addr);
}

esp_err_t hub_notify_all_free(void)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj != NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_EXIT_CRITICAL();

    return ext_hub_all_free();
}
#endif // ENABLE_USB_HUBS

esp_err_t hub_process(void)
{
    HUB_DRIVER_ENTER_CRITICAL();
    uint32_t action_flags = p_hub_driver_obj->dynamic.flags.actions;
    p_hub_driver_obj->dynamic.flags.actions = 0;
    HUB_DRIVER_EXIT_CRITICAL();

    while (action_flags) {
#if ENABLE_USB_HUBS
        if (action_flags & HUB_DRIVER_ACTION_EXT_PORT) {
            ESP_LOGW(HUB_DRIVER_TAG, "ext_port_process() has not been implemented yet");
            /*
            ESP_ERROR_CHECK(ext_port_process());
            */
        }
        if (action_flags & HUB_DRIVER_ACTION_EXT_HUB) {
            ESP_ERROR_CHECK(ext_hub_process());
        }
#endif // ENABLE_USB_HUBS
        if (action_flags & HUB_DRIVER_ACTION_ROOT_EVENT) {
            root_port_handle_events(p_hub_driver_obj->constant.root_port_hdl);
        }
        if (action_flags & HUB_DRIVER_ACTION_ROOT_REQ) {
            root_port_req(p_hub_driver_obj->constant.root_port_hdl);
        }

        HUB_DRIVER_ENTER_CRITICAL();
        action_flags = p_hub_driver_obj->dynamic.flags.actions;
        p_hub_driver_obj->dynamic.flags.actions = 0;
        HUB_DRIVER_EXIT_CRITICAL();
    }

    return ESP_OK;
}
