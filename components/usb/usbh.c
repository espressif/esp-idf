/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <sys/queue.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "hcd.h"
#include "usbh.h"
#include "usb/usb_helpers.h"
#include "usb/usb_types_ch9.h"

#define EP_NUM_MIN                  1   // The smallest possible non-default endpoint number
#define EP_NUM_MAX                  16  // The largest possible non-default endpoint number
#define NUM_NON_DEFAULT_EP          ((EP_NUM_MAX - 1) * 2)  // The total number of non-default endpoints a device can have.

// Device action flags. LISTED IN THE ORDER THEY SHOULD BE HANDLED IN within usbh_process(). Some actions are mutually exclusive
typedef enum {
    DEV_ACTION_EPn_HALT_FLUSH       = (1 << 0),     // Halt all non-default endpoints then flush them (called after a device gone is gone)
    DEV_ACTION_EP0_FLUSH            = (1 << 1),     // Retire all URBS submitted to EP0
    DEV_ACTION_EP0_DEQUEUE          = (1 << 2),     // Dequeue all URBs from EP0
    DEV_ACTION_EP0_CLEAR            = (1 << 3),     // Move EP0 to the the active state
    DEV_ACTION_PROP_GONE_EVT        = (1 << 4),     // Propagate a USBH_EVENT_DEV_GONE event
    DEV_ACTION_FREE_AND_RECOVER     = (1 << 5),     // Free the device object, but send a USBH_HUB_REQ_PORT_RECOVER request afterwards.
    DEV_ACTION_FREE                 = (1 << 6),     // Free the device object
    DEV_ACTION_PORT_DISABLE         = (1 << 7),     // Request the hub driver to disable the port of the device
    DEV_ACTION_PROP_NEW             = (1 << 8),     // Propagate a USBH_EVENT_DEV_NEW event
} dev_action_t;

typedef struct device_s device_t;

typedef struct {
    struct {
        usbh_ep_cb_t ep_cb;
        void *ep_cb_arg;
        hcd_pipe_handle_t pipe_hdl;
        device_t *dev;                  // Pointer to the device object that this endpoint is contained in
        const usb_ep_desc_t *ep_desc;   // This just stores a pointer endpoint descriptor inside the device's "config_desc"
    } constant;
} endpoint_t;

struct device_s {
    // Dynamic members require a critical section
    struct {
        TAILQ_ENTRY(device_s) tailq_entry;
        union {
            struct {
                uint32_t in_pending_list: 1;
                uint32_t is_gone: 1;
                uint32_t waiting_close: 1;
                uint32_t waiting_port_disable: 1;
                uint32_t waiting_free: 1;
                uint32_t reserved27: 27;
            };
            uint32_t val;
        } flags;
        uint32_t action_flags;
        int num_ctrl_xfers_inflight;
        usb_device_state_t state;
        uint32_t ref_count;
    } dynamic;
    // Mux protected members must be protected by the USBH mux_lock when accessed
    struct {
        /*
        - Endpoint object pointers for each possible non-default endpoint
        - All OUT EPs are listed before IN EPs (i.e., EP_NUM_MIN OUT ... EP_NUM_MAX OUT ... EP_NUM_MIN IN ... EP_NUM_MAX)
        */
        endpoint_t *endpoints[NUM_NON_DEFAULT_EP];
    } mux_protected;
    // Constant members do not change after device allocation and enumeration thus do not require a critical section
    struct {
        hcd_pipe_handle_t default_pipe;
        hcd_port_handle_t port_hdl;
        uint8_t address;
        usb_speed_t speed;
        const usb_device_desc_t *desc;
        const usb_config_desc_t *config_desc;
        const usb_str_desc_t *str_desc_manu;
        const usb_str_desc_t *str_desc_product;
        const usb_str_desc_t *str_desc_ser_num;
    } constant;
};

typedef struct {
    // Dynamic members require a critical section
    struct {
        TAILQ_HEAD(tailhead_devs, device_s) devs_idle_tailq;        // Tailq of all enum and configured devices
        TAILQ_HEAD(tailhead_devs_cb, device_s) devs_pending_tailq;  // Tailq of devices that need to have their cb called
    } dynamic;
    // Mux protected members must be protected by the USBH mux_lock when accessed
    struct {
        uint8_t num_device;     // Number of enumerated devices
    } mux_protected;
    // Constant members do no change after installation thus do not require a critical section
    struct {
        usb_proc_req_cb_t proc_req_cb;
        void *proc_req_cb_arg;
        usbh_hub_req_cb_t hub_req_cb;
        void *hub_req_cb_arg;
        usbh_event_cb_t event_cb;
        void *event_cb_arg;
        usbh_ctrl_xfer_cb_t ctrl_xfer_cb;
        void *ctrl_xfer_cb_arg;
        SemaphoreHandle_t mux_lock;
    } constant;
} usbh_t;

static usbh_t *p_usbh_obj = NULL;

static portMUX_TYPE usbh_lock = portMUX_INITIALIZER_UNLOCKED;

const char *USBH_TAG = "USBH";

#define USBH_ENTER_CRITICAL_ISR()       portENTER_CRITICAL_ISR(&usbh_lock)
#define USBH_EXIT_CRITICAL_ISR()        portEXIT_CRITICAL_ISR(&usbh_lock)
#define USBH_ENTER_CRITICAL()           portENTER_CRITICAL(&usbh_lock)
#define USBH_EXIT_CRITICAL()            portEXIT_CRITICAL(&usbh_lock)
#define USBH_ENTER_CRITICAL_SAFE()      portENTER_CRITICAL_SAFE(&usbh_lock)
#define USBH_EXIT_CRITICAL_SAFE()       portEXIT_CRITICAL_SAFE(&usbh_lock)

#define USBH_CHECK(cond, ret_val) ({                                        \
            if (!(cond)) {                                                  \
                return (ret_val);                                           \
            }                                                               \
})
#define USBH_CHECK_FROM_CRIT(cond, ret_val) ({                              \
            if (!(cond)) {                                                  \
                USBH_EXIT_CRITICAL();                                       \
                return ret_val;                                             \
            }                                                               \
})

// ------------------------------------------------- Forward Declare ---------------------------------------------------

static bool ep0_pipe_callback(hcd_pipe_handle_t pipe_hdl, hcd_pipe_event_t pipe_event, void *user_arg, bool in_isr);

static bool epN_pipe_callback(hcd_pipe_handle_t pipe_hdl, hcd_pipe_event_t pipe_event, void *user_arg, bool in_isr);

static bool _dev_set_actions(device_t *dev_obj, uint32_t action_flags);

// ----------------------------------------------------- Helpers -------------------------------------------------------

static inline bool check_ep_addr(uint8_t bEndpointAddress)
{
    /*
    Check that the bEndpointAddress is valid
    - Must be <= EP_NUM_MAX (e.g., 16)
    - Must be >= EP_NUM_MIN (e.g., 1).
    - EP0 is the owned/managed by USBH, thus must never by directly addressed by users (see USB 2.0 section 10.5.1.2)
    */
    uint8_t addr = bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK;
    return (addr >= EP_NUM_MIN) && (addr <= EP_NUM_MAX);
}

static endpoint_t *get_ep_from_addr(device_t *dev_obj, uint8_t bEndpointAddress)
{
    /*
    CALLER IS RESPONSIBLE FOR TAKING THE mux_lock
    */

    // Calculate index to the device's endpoint object list
    int index;
    // EP_NUM_MIN should map to an index of 0
    index = (bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK) - EP_NUM_MIN;
    assert(index >= 0);  // Endpoint address is not supported
    if (bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) {
        // OUT EPs are listed before IN EPs, so add an offset
        index += (EP_NUM_MAX - EP_NUM_MIN);
    }
    return dev_obj->mux_protected.endpoints[index];
}

static inline void set_ep_from_addr(device_t *dev_obj, uint8_t bEndpointAddress, endpoint_t *ep_obj)
{
    /*
    CALLER IS RESPONSIBLE FOR TAKING THE mux_lock
    */

    // Calculate index to the device's endpoint object list
    int index;
    // EP_NUM_MIN should map to an index of 0
    index = (bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK) - EP_NUM_MIN;
    assert(index >= 0);  // Endpoint address is not supported
    if (bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) {
        // OUT EPs are listed before IN EPs, so add an offset
        index += (EP_NUM_MAX - EP_NUM_MIN);
    }
    dev_obj->mux_protected.endpoints[index] = ep_obj;
}

static bool urb_check_args(urb_t *urb)
{
    if (urb->transfer.callback == NULL) {
        ESP_LOGE(USBH_TAG, "usb_transfer_t callback is NULL");
        return false;
    }
    if (urb->transfer.num_bytes > urb->transfer.data_buffer_size) {
        ESP_LOGE(USBH_TAG, "usb_transfer_t num_bytes > data_buffer_size");
        return false;
    }
    return true;
}

static bool transfer_check_usb_compliance(usb_transfer_t *transfer, usb_transfer_type_t type, int mps, bool is_in)
{
    if (type == USB_TRANSFER_TYPE_CTRL) {
        // Check that num_bytes and wLength are set correctly
        usb_setup_packet_t *setup_pkt = (usb_setup_packet_t *)transfer->data_buffer;
        if (transfer->num_bytes != sizeof(usb_setup_packet_t) + setup_pkt->wLength) {
            ESP_LOGE(USBH_TAG, "usb_transfer_t num_bytes and usb_setup_packet_t wLength mismatch");
            return false;
        }
    } else if (type == USB_TRANSFER_TYPE_ISOCHRONOUS) {
        // Check that there is at least one isochronous packet descriptor
        if (transfer->num_isoc_packets <= 0) {
            ESP_LOGE(USBH_TAG, "usb_transfer_t num_isoc_packets is 0");
            return false;
        }
        // Check that sum of all packet lengths add up to transfer length
        // If IN, check that each packet length is integer multiple of MPS
        int total_num_bytes = 0;
        bool mod_mps_all_zero = true;
        for (int i = 0; i < transfer->num_isoc_packets; i++) {
            total_num_bytes += transfer->isoc_packet_desc[i].num_bytes;
            if (transfer->isoc_packet_desc[i].num_bytes % mps != 0) {
                mod_mps_all_zero = false;
            }
        }
        if (transfer->num_bytes != total_num_bytes) {
            ESP_LOGE(USBH_TAG, "ISOC transfer num_bytes != num_bytes of all packets");
            return false;
        }
        if (is_in && !mod_mps_all_zero) {
            ESP_LOGE(USBH_TAG, "ISOC IN num_bytes not integer multiple of MPS");
            return false;
        }
    } else {
        // Check that IN transfers are integer multiple of MPS
        if (is_in && (transfer->num_bytes % mps != 0)) {
            ESP_LOGE(USBH_TAG, "IN transfer num_bytes not integer multiple of MPS");
            return false;
        }
    }
    return true;
}

// --------------------------------------------------- Allocation ------------------------------------------------------

static esp_err_t endpoint_alloc(device_t *dev_obj, const usb_ep_desc_t *ep_desc, usbh_ep_config_t *ep_config, endpoint_t **ep_obj_ret)
{
    esp_err_t ret;
    endpoint_t *ep_obj;
    hcd_pipe_handle_t pipe_hdl;

    ep_obj = heap_caps_calloc(1, sizeof(endpoint_t), MALLOC_CAP_DEFAULT);
    if (ep_obj == NULL) {
        return ESP_ERR_NO_MEM;
    }
    // Allocate the EP's underlying pipe
    hcd_pipe_config_t pipe_config = {
        .callback = epN_pipe_callback,
        .callback_arg = (void *)ep_obj,
        .context = ep_config->context,
        .ep_desc = ep_desc,
        .dev_speed = dev_obj->constant.speed,
        .dev_addr = dev_obj->constant.address,
    };
    ret = hcd_pipe_alloc(dev_obj->constant.port_hdl, &pipe_config, &pipe_hdl);
    if (ret != ESP_OK) {
        goto pipe_err;
    }
    // Initialize the endpoint object
    ep_obj->constant.pipe_hdl = pipe_hdl;
    ep_obj->constant.ep_cb = ep_config->ep_cb;
    ep_obj->constant.ep_cb_arg = ep_config->ep_cb_arg;
    ep_obj->constant.dev = dev_obj;
    ep_obj->constant.ep_desc = ep_desc;
    // Return the endpoint object
    *ep_obj_ret = ep_obj;

    ret = ESP_OK;
    return ret;

pipe_err:
    heap_caps_free(ep_obj);
    return ret;
}

static void endpoint_free(endpoint_t *ep_obj)
{
    if (ep_obj == NULL) {
        return;
    }
    // Deallocate the EP's underlying pipe
    ESP_ERROR_CHECK(hcd_pipe_free(ep_obj->constant.pipe_hdl));
    // Free the heap object
    heap_caps_free(ep_obj);
}

static esp_err_t device_alloc(hcd_port_handle_t port_hdl, usb_speed_t speed, device_t **dev_obj_ret)
{
    esp_err_t ret;
    device_t *dev_obj = heap_caps_calloc(1, sizeof(device_t), MALLOC_CAP_DEFAULT);
    usb_device_desc_t *dev_desc = heap_caps_calloc(1, sizeof(usb_device_desc_t), MALLOC_CAP_DEFAULT);
    if (dev_obj == NULL || dev_desc == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
    // Allocate a pipe for EP0. We set the pipe callback to NULL for now
    hcd_pipe_config_t pipe_config = {
        .callback = NULL,
        .callback_arg = NULL,
        .context = (void *)dev_obj,
        .ep_desc = NULL,    // No endpoint descriptor means we're allocating a pipe for EP0
        .dev_speed = speed,
        .dev_addr = 0,
    };
    hcd_pipe_handle_t default_pipe_hdl;
    ret = hcd_pipe_alloc(port_hdl, &pipe_config, &default_pipe_hdl);
    if (ret != ESP_OK) {
        goto err;
    }
    // Initialize device object
    dev_obj->dynamic.state = USB_DEVICE_STATE_DEFAULT;
    dev_obj->constant.default_pipe = default_pipe_hdl;
    dev_obj->constant.port_hdl = port_hdl;
    // Note: dev_obj->constant.address is assigned later during enumeration
    dev_obj->constant.speed = speed;
    dev_obj->constant.desc = dev_desc;
    *dev_obj_ret = dev_obj;
    ret = ESP_OK;
    return ret;

err:
    heap_caps_free(dev_desc);
    heap_caps_free(dev_obj);
    return ret;
}

static void device_free(device_t *dev_obj)
{
    if (dev_obj == NULL) {
        return;
    }
    // Configuration might not have been allocated (in case of early enumeration failure)
    if (dev_obj->constant.config_desc) {
        heap_caps_free((usb_config_desc_t *)dev_obj->constant.config_desc);
    }
    // String descriptors might not have been allocated (in case of early enumeration failure)
    if (dev_obj->constant.str_desc_manu) {
        heap_caps_free((usb_str_desc_t *)dev_obj->constant.str_desc_manu);
    }
    if (dev_obj->constant.str_desc_product) {
        heap_caps_free((usb_str_desc_t *)dev_obj->constant.str_desc_product);
    }
    if (dev_obj->constant.str_desc_ser_num) {
        heap_caps_free((usb_str_desc_t *)dev_obj->constant.str_desc_ser_num);
    }
    heap_caps_free((usb_device_desc_t *)dev_obj->constant.desc);
    ESP_ERROR_CHECK(hcd_pipe_free(dev_obj->constant.default_pipe));
    heap_caps_free(dev_obj);
}

// ---------------------------------------------------- Callbacks ------------------------------------------------------

static bool ep0_pipe_callback(hcd_pipe_handle_t pipe_hdl, hcd_pipe_event_t pipe_event, void *user_arg, bool in_isr)
{
    uint32_t action_flags;
    device_t *dev_obj = (device_t *)user_arg;
    switch (pipe_event) {
    case HCD_PIPE_EVENT_URB_DONE:
        // A control transfer completed on EP0's pipe . We need to dequeue it
        action_flags = DEV_ACTION_EP0_DEQUEUE;
        break;
    case HCD_PIPE_EVENT_ERROR_XFER:
    case HCD_PIPE_EVENT_ERROR_URB_NOT_AVAIL:
    case HCD_PIPE_EVENT_ERROR_OVERFLOW:
        // EP0's pipe has encountered an error. We need to retire all URBs, dequeue them, then make the pipe active again
        action_flags = DEV_ACTION_EP0_FLUSH |
                       DEV_ACTION_EP0_DEQUEUE |
                       DEV_ACTION_EP0_CLEAR;
        if (in_isr) {
            ESP_EARLY_LOGE(USBH_TAG, "Dev %d EP 0 Error", dev_obj->constant.address);
        } else {
            ESP_LOGE(USBH_TAG, "Dev %d EP 0 Error", dev_obj->constant.address);
        }
        break;
    case HCD_PIPE_EVENT_ERROR_STALL:
        // EP0's pipe encountered a "protocol stall". We just need to dequeue URBs then make the pipe active again
        action_flags = DEV_ACTION_EP0_DEQUEUE | DEV_ACTION_EP0_CLEAR;
        if (in_isr) {
            ESP_EARLY_LOGE(USBH_TAG, "Dev %d EP 0 STALL", dev_obj->constant.address);
        } else {
            ESP_LOGE(USBH_TAG, "Dev %d EP 0 STALL", dev_obj->constant.address);
        }
        break;
    default:
        action_flags = 0;
        break;
    }

    USBH_ENTER_CRITICAL_SAFE();
    bool call_proc_req_cb = _dev_set_actions(dev_obj, action_flags);
    USBH_EXIT_CRITICAL_SAFE();

    bool yield = false;
    if (call_proc_req_cb) {
        yield = p_usbh_obj->constant.proc_req_cb(USB_PROC_REQ_SOURCE_USBH, in_isr, p_usbh_obj->constant.proc_req_cb_arg);
    }
    return yield;
}

static bool epN_pipe_callback(hcd_pipe_handle_t pipe_hdl, hcd_pipe_event_t pipe_event, void *user_arg, bool in_isr)
{
    endpoint_t *ep_obj = (endpoint_t *)user_arg;
    return ep_obj->constant.ep_cb((usbh_ep_handle_t)ep_obj,
                                  (usbh_ep_event_t)pipe_event,
                                  ep_obj->constant.ep_cb_arg,
                                  in_isr);
}

// -------------------------------------------------- Event Related ----------------------------------------------------

static bool _dev_set_actions(device_t *dev_obj, uint32_t action_flags)
{
    if (action_flags == 0) {
        return false;
    }
    bool call_proc_req_cb;
    // Check if device is already on the callback list
    if (!dev_obj->dynamic.flags.in_pending_list) {
        // Move device form idle device list to callback device list
        TAILQ_REMOVE(&p_usbh_obj->dynamic.devs_idle_tailq, dev_obj, dynamic.tailq_entry);
        TAILQ_INSERT_TAIL(&p_usbh_obj->dynamic.devs_pending_tailq, dev_obj, dynamic.tailq_entry);
        dev_obj->dynamic.action_flags |= action_flags;
        dev_obj->dynamic.flags.in_pending_list = 1;
        call_proc_req_cb = true;
    } else {
        call_proc_req_cb = false;
    }
    return call_proc_req_cb;
}

static inline void handle_epn_halt_flush(device_t *dev_obj)
{
    // We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    // Halt then flush all non-default EPs
    for (int i = 0; i < NUM_NON_DEFAULT_EP; i++) {
        if (dev_obj->mux_protected.endpoints[i] != NULL) {
            ESP_ERROR_CHECK(hcd_pipe_command(dev_obj->mux_protected.endpoints[i]->constant.pipe_hdl, HCD_PIPE_CMD_HALT));
            ESP_ERROR_CHECK(hcd_pipe_command(dev_obj->mux_protected.endpoints[i]->constant.pipe_hdl, HCD_PIPE_CMD_FLUSH));
        }
    }
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);
}

static inline void handle_ep0_flush(device_t *dev_obj)
{
    ESP_ERROR_CHECK(hcd_pipe_command(dev_obj->constant.default_pipe, HCD_PIPE_CMD_HALT));
    ESP_ERROR_CHECK(hcd_pipe_command(dev_obj->constant.default_pipe, HCD_PIPE_CMD_FLUSH));
}

static inline void handle_ep0_dequeue(device_t *dev_obj)
{
    // Empty URBs from EP0's pipe and call the control transfer callback
    ESP_LOGD(USBH_TAG, "Default pipe device %d", dev_obj->constant.address);
    int num_urbs = 0;
    urb_t *urb = hcd_urb_dequeue(dev_obj->constant.default_pipe);
    while (urb != NULL) {
        num_urbs++;
        p_usbh_obj->constant.ctrl_xfer_cb((usb_device_handle_t)dev_obj, urb, p_usbh_obj->constant.ctrl_xfer_cb_arg);
        urb = hcd_urb_dequeue(dev_obj->constant.default_pipe);
    }
    USBH_ENTER_CRITICAL();
    dev_obj->dynamic.num_ctrl_xfers_inflight -= num_urbs;
    USBH_EXIT_CRITICAL();
}

static inline void handle_ep0_clear(device_t *dev_obj)
{
    // We allow the pipe command to fail just in case the pipe becomes invalid mid command
    hcd_pipe_command(dev_obj->constant.default_pipe, HCD_PIPE_CMD_CLEAR);
}

static inline void handle_prop_gone_evt(device_t *dev_obj)
{
    // Flush EP0's pipe. Then propagate a USBH_EVENT_DEV_GONE event
    ESP_LOGE(USBH_TAG, "Device %d gone", dev_obj->constant.address);
    p_usbh_obj->constant.event_cb((usb_device_handle_t)dev_obj, USBH_EVENT_DEV_GONE, p_usbh_obj->constant.event_cb_arg);
}

static void handle_free_and_recover(device_t *dev_obj, bool recover_port)
{
    // Cache a copy of the port handle as we are about to free the device object
    bool all_free;
    hcd_port_handle_t port_hdl = dev_obj->constant.port_hdl;
    ESP_LOGD(USBH_TAG, "Freeing device %d", dev_obj->constant.address);

    // We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    USBH_ENTER_CRITICAL();
    // Remove the device object for it's containing list
    if (dev_obj->dynamic.flags.in_pending_list) {
        dev_obj->dynamic.flags.in_pending_list = 0;
        TAILQ_REMOVE(&p_usbh_obj->dynamic.devs_pending_tailq, dev_obj, dynamic.tailq_entry);
    } else {
        TAILQ_REMOVE(&p_usbh_obj->dynamic.devs_idle_tailq, dev_obj, dynamic.tailq_entry);
    }
    USBH_EXIT_CRITICAL();
    p_usbh_obj->mux_protected.num_device--;
    all_free = (p_usbh_obj->mux_protected.num_device == 0);
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);
    device_free(dev_obj);

    // If all devices have been freed, propagate a USBH_EVENT_DEV_ALL_FREE event
    if (all_free) {
        ESP_LOGD(USBH_TAG, "Device all free");
        p_usbh_obj->constant.event_cb((usb_device_handle_t)NULL, USBH_EVENT_DEV_ALL_FREE, p_usbh_obj->constant.event_cb_arg);
    }
    // Check if we need to recover the device's port
    if (recover_port) {
        p_usbh_obj->constant.hub_req_cb(port_hdl, USBH_HUB_REQ_PORT_RECOVER, p_usbh_obj->constant.hub_req_cb_arg);
    }
}

static inline void handle_port_disable(device_t *dev_obj)
{
    // Request that the HUB disables this device's port
    ESP_LOGD(USBH_TAG, "Disable device port %d", dev_obj->constant.address);
    p_usbh_obj->constant.hub_req_cb(dev_obj->constant.port_hdl, USBH_HUB_REQ_PORT_DISABLE, p_usbh_obj->constant.hub_req_cb_arg);
}

static inline void handle_prop_new_evt(device_t *dev_obj)
{
    ESP_LOGD(USBH_TAG, "New device %d", dev_obj->constant.address);
    p_usbh_obj->constant.event_cb((usb_device_handle_t)dev_obj, USBH_EVENT_DEV_NEW, p_usbh_obj->constant.event_cb_arg);
}

// ------------------------------------------------- USBH Functions ----------------------------------------------------

esp_err_t usbh_install(const usbh_config_t *usbh_config)
{
    USBH_CHECK(usbh_config != NULL, ESP_ERR_INVALID_ARG);
    USBH_ENTER_CRITICAL();
    USBH_CHECK_FROM_CRIT(p_usbh_obj == NULL, ESP_ERR_INVALID_STATE);
    USBH_EXIT_CRITICAL();

    esp_err_t ret;
    usbh_t *usbh_obj = heap_caps_calloc(1, sizeof(usbh_t), MALLOC_CAP_DEFAULT);
    SemaphoreHandle_t mux_lock = xSemaphoreCreateMutex();
    if (usbh_obj == NULL || mux_lock == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
    // Initialize USBH object
    TAILQ_INIT(&usbh_obj->dynamic.devs_idle_tailq);
    TAILQ_INIT(&usbh_obj->dynamic.devs_pending_tailq);
    usbh_obj->constant.proc_req_cb = usbh_config->proc_req_cb;
    usbh_obj->constant.proc_req_cb_arg = usbh_config->proc_req_cb_arg;
    usbh_obj->constant.event_cb = usbh_config->event_cb;
    usbh_obj->constant.event_cb_arg = usbh_config->event_cb_arg;
    usbh_obj->constant.ctrl_xfer_cb = usbh_config->ctrl_xfer_cb;
    usbh_obj->constant.ctrl_xfer_cb_arg = usbh_config->ctrl_xfer_cb_arg;
    usbh_obj->constant.mux_lock = mux_lock;

    // Assign USBH object pointer
    USBH_ENTER_CRITICAL();
    if (p_usbh_obj != NULL) {
        USBH_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto err;
    }
    p_usbh_obj = usbh_obj;
    USBH_EXIT_CRITICAL();

    ret = ESP_OK;
    return ret;

err:
    if (mux_lock != NULL) {
        vSemaphoreDelete(mux_lock);
    }
    heap_caps_free(usbh_obj);
    return ret;
}

esp_err_t usbh_uninstall(void)
{
    // Check that USBH is in a state to be uninstalled
    USBH_ENTER_CRITICAL();
    USBH_CHECK_FROM_CRIT(p_usbh_obj != NULL, ESP_ERR_INVALID_STATE);
    usbh_t *usbh_obj = p_usbh_obj;
    USBH_EXIT_CRITICAL();

    esp_err_t ret;
    // We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(usbh_obj->constant.mux_lock, portMAX_DELAY);
    if (p_usbh_obj->mux_protected.num_device > 0) {
        // There are still devices allocated. Can't uninstall right now.
        ret = ESP_ERR_INVALID_STATE;
        goto exit;
    }
    // Check again if we can uninstall
    USBH_ENTER_CRITICAL();
    assert(p_usbh_obj == usbh_obj);
    p_usbh_obj = NULL;
    USBH_EXIT_CRITICAL();
    xSemaphoreGive(usbh_obj->constant.mux_lock);

    // Free resources
    vSemaphoreDelete(usbh_obj->constant.mux_lock);
    heap_caps_free(usbh_obj);
    ret = ESP_OK;
    return ret;

exit:
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);
    return ret;
}

esp_err_t usbh_process(void)
{
    USBH_ENTER_CRITICAL();
    USBH_CHECK_FROM_CRIT(p_usbh_obj != NULL, ESP_ERR_INVALID_STATE);
    // Keep processing until all device's with pending events have been handled
    while (!TAILQ_EMPTY(&p_usbh_obj->dynamic.devs_pending_tailq)) {
        // Move the device back into the idle device list,
        device_t *dev_obj = TAILQ_FIRST(&p_usbh_obj->dynamic.devs_pending_tailq);
        TAILQ_REMOVE(&p_usbh_obj->dynamic.devs_pending_tailq, dev_obj, dynamic.tailq_entry);
        TAILQ_INSERT_TAIL(&p_usbh_obj->dynamic.devs_idle_tailq, dev_obj, dynamic.tailq_entry);
        // Clear the device's flags
        uint32_t action_flags = dev_obj->dynamic.action_flags;
        dev_obj->dynamic.action_flags = 0;
        dev_obj->dynamic.flags.in_pending_list = 0;

        /* ---------------------------------------------------------------------
        Exit critical section to handle device action flags in their listed order
        --------------------------------------------------------------------- */
        USBH_EXIT_CRITICAL();
        ESP_LOGD(USBH_TAG, "Processing actions 0x%"PRIx32"", action_flags);
        // Sanity check. If the device is being freed, there must not be any other action flags set
        assert(!(action_flags & DEV_ACTION_FREE) || action_flags == DEV_ACTION_FREE);

        if (action_flags & DEV_ACTION_EPn_HALT_FLUSH) {
            handle_epn_halt_flush(dev_obj);
        }
        if (action_flags & DEV_ACTION_EP0_FLUSH) {
            handle_ep0_flush(dev_obj);
        }
        if (action_flags & DEV_ACTION_EP0_DEQUEUE) {
            handle_ep0_dequeue(dev_obj);
        }
        if (action_flags & DEV_ACTION_EP0_CLEAR) {
            handle_ep0_clear(dev_obj);
        }
        if (action_flags & DEV_ACTION_PROP_GONE_EVT) {
            handle_prop_gone_evt(dev_obj);
        }
        /*
        Note: We make these action flags mutually exclusive in case they happen in rapid succession. They are handled
        in the order of precedence
        For example
        - New device event is requested followed immediately by a disconnection
        - Port disable requested followed immediately by a disconnection
        */
        if (action_flags & DEV_ACTION_FREE_AND_RECOVER) {
            handle_free_and_recover(dev_obj, true);
        } else if (action_flags & DEV_ACTION_FREE) {
            handle_free_and_recover(dev_obj, false);
        } else if (action_flags & DEV_ACTION_PORT_DISABLE) {
            handle_port_disable(dev_obj);
        } else if (action_flags & DEV_ACTION_PROP_NEW) {
            handle_prop_new_evt(dev_obj);
        }
        USBH_ENTER_CRITICAL();
        /* ---------------------------------------------------------------------
        Re-enter critical sections. All device action flags should have been handled.
        --------------------------------------------------------------------- */

    }
    USBH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t usbh_num_devs(int *num_devs_ret)
{
    USBH_CHECK(num_devs_ret != NULL, ESP_ERR_INVALID_ARG);
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    *num_devs_ret = p_usbh_obj->mux_protected.num_device;
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);
    return ESP_OK;
}

// ------------------------------------------------ Device Functions ---------------------------------------------------

// --------------------- Device Pool -----------------------

esp_err_t usbh_dev_addr_list_fill(int list_len, uint8_t *dev_addr_list, int *num_dev_ret)
{
    USBH_CHECK(dev_addr_list != NULL && num_dev_ret != NULL, ESP_ERR_INVALID_ARG);
    USBH_ENTER_CRITICAL();
    int num_filled = 0;
    device_t *dev_obj;
    // Fill list with devices from idle tailq
    TAILQ_FOREACH(dev_obj, &p_usbh_obj->dynamic.devs_idle_tailq, dynamic.tailq_entry) {
        if (num_filled < list_len) {
            dev_addr_list[num_filled] = dev_obj->constant.address;
            num_filled++;
        } else {
            break;
        }
    }
    // Fill list with devices from pending tailq
    TAILQ_FOREACH(dev_obj, &p_usbh_obj->dynamic.devs_pending_tailq, dynamic.tailq_entry) {
        if (num_filled < list_len) {
            dev_addr_list[num_filled] = dev_obj->constant.address;
            num_filled++;
        } else {
            break;
        }
    }
    USBH_EXIT_CRITICAL();
    // Write back number of devices filled
    *num_dev_ret = num_filled;
    return ESP_OK;
}

esp_err_t usbh_dev_open(uint8_t dev_addr, usb_device_handle_t *dev_hdl)
{
    USBH_CHECK(dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    esp_err_t ret;

    USBH_ENTER_CRITICAL();
    // Go through the device lists to find the device with the specified address
    device_t *found_dev_obj = NULL;
    device_t *dev_obj;
    TAILQ_FOREACH(dev_obj, &p_usbh_obj->dynamic.devs_idle_tailq, dynamic.tailq_entry) {
        if (dev_obj->constant.address == dev_addr) {
            found_dev_obj = dev_obj;
            goto exit;
        }
    }
    TAILQ_FOREACH(dev_obj, &p_usbh_obj->dynamic.devs_pending_tailq, dynamic.tailq_entry) {
        if (dev_obj->constant.address == dev_addr) {
            found_dev_obj = dev_obj;
            goto exit;
        }
    }
exit:
    if (found_dev_obj != NULL) {
        // The device is not in a state to be referenced
        if (dev_obj->dynamic.flags.is_gone || dev_obj->dynamic.flags.waiting_port_disable || dev_obj->dynamic.flags.waiting_free) {
            ret = ESP_ERR_INVALID_STATE;
        } else {
            dev_obj->dynamic.ref_count++;
            *dev_hdl = (usb_device_handle_t)found_dev_obj;
            ret = ESP_OK;
        }
    } else {
        ret = ESP_ERR_NOT_FOUND;
    }
    USBH_EXIT_CRITICAL();

    return ret;
}

esp_err_t usbh_dev_close(usb_device_handle_t dev_hdl)
{
    USBH_CHECK(dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;

    USBH_ENTER_CRITICAL();
    dev_obj->dynamic.ref_count--;
    bool call_proc_req_cb = false;
    if (dev_obj->dynamic.ref_count == 0) {
        // Sanity check.
        assert(dev_obj->dynamic.num_ctrl_xfers_inflight == 0);  // There cannot be any control transfer in-flight
        assert(!dev_obj->dynamic.flags.waiting_free);   // This can only be set when ref count reaches 0
        if (dev_obj->dynamic.flags.is_gone) {
            // Device is already gone so it's port is already disabled. Trigger the USBH process to free the device
            dev_obj->dynamic.flags.waiting_free = 1;
            call_proc_req_cb = _dev_set_actions(dev_obj, DEV_ACTION_FREE_AND_RECOVER); // Port error occurred so we need to recover it
        } else if (dev_obj->dynamic.flags.waiting_close) {
            // Device is still connected but is no longer needed. Trigger the USBH process to request device's port be disabled
            dev_obj->dynamic.flags.waiting_port_disable = 1;
            call_proc_req_cb = _dev_set_actions(dev_obj, DEV_ACTION_PORT_DISABLE);
        }
        // Else, there's nothing to do. Leave the device allocated
    }
    USBH_EXIT_CRITICAL();

    if (call_proc_req_cb) {
        p_usbh_obj->constant.proc_req_cb(USB_PROC_REQ_SOURCE_USBH, false, p_usbh_obj->constant.proc_req_cb_arg);
    }
    return ESP_OK;
}

esp_err_t usbh_dev_mark_all_free(void)
{
    USBH_ENTER_CRITICAL();
    /*
    Go through the device list and mark each device as waiting to be closed. If the device is not opened at all, we can
    disable it immediately.
    Note: We manually traverse the list because we need to add/remove items while traversing
    */
    bool call_proc_req_cb = false;
    bool wait_for_free = false;
    for (int i = 0; i < 2; i++) {
        device_t *dev_obj_cur;
        device_t *dev_obj_next;
        // Go through pending list first as it's more efficient
        if (i == 0) {
            dev_obj_cur = TAILQ_FIRST(&p_usbh_obj->dynamic.devs_pending_tailq);
        } else {
            dev_obj_cur = TAILQ_FIRST(&p_usbh_obj->dynamic.devs_idle_tailq);
        }
        while (dev_obj_cur != NULL) {
            assert(!dev_obj_cur->dynamic.flags.waiting_close);  // Sanity check
            // Keep a copy of the next item first in case we remove the current item
            dev_obj_next = TAILQ_NEXT(dev_obj_cur, dynamic.tailq_entry);
            if (dev_obj_cur->dynamic.ref_count == 0 && !dev_obj_cur->dynamic.flags.is_gone) {
                // Device is not opened as is not gone, so we can disable it now
                dev_obj_cur->dynamic.flags.waiting_port_disable = 1;
                call_proc_req_cb |= _dev_set_actions(dev_obj_cur, DEV_ACTION_PORT_DISABLE);
            } else {
                // Device is still opened. Just mark it as waiting to be closed
                dev_obj_cur->dynamic.flags.waiting_close = 1;
            }
            wait_for_free = true;   // As long as there is still a device, we need to wait for an event indicating it is freed
            dev_obj_cur = dev_obj_next;
        }
    }
    USBH_EXIT_CRITICAL();

    if (call_proc_req_cb) {
        p_usbh_obj->constant.proc_req_cb(USB_PROC_REQ_SOURCE_USBH, false, p_usbh_obj->constant.proc_req_cb_arg);
    }
    return (wait_for_free) ? ESP_ERR_NOT_FINISHED : ESP_OK;
}

// ------------------- Single Device  ----------------------

esp_err_t usbh_dev_get_addr(usb_device_handle_t dev_hdl, uint8_t *dev_addr)
{
    USBH_CHECK(dev_hdl != NULL && dev_addr != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;

    USBH_ENTER_CRITICAL();
    USBH_CHECK_FROM_CRIT(dev_obj->constant.address > 0, ESP_ERR_INVALID_STATE);
    *dev_addr = dev_obj->constant.address;
    USBH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t usbh_dev_get_info(usb_device_handle_t dev_hdl, usb_device_info_t *dev_info)
{
    USBH_CHECK(dev_hdl != NULL && dev_info != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;

    esp_err_t ret;
    // Device must be configured, or not attached (if it suddenly disconnected)
    USBH_ENTER_CRITICAL();
    if (!(dev_obj->dynamic.state == USB_DEVICE_STATE_CONFIGURED || dev_obj->dynamic.state == USB_DEVICE_STATE_NOT_ATTACHED)) {
        USBH_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto exit;
    }
    // Critical section for the dynamic members
    dev_info->speed = dev_obj->constant.speed;
    dev_info->dev_addr = dev_obj->constant.address;
    dev_info->bMaxPacketSize0 = dev_obj->constant.desc->bMaxPacketSize0;
    USBH_EXIT_CRITICAL();
    assert(dev_obj->constant.config_desc);
    dev_info->bConfigurationValue = dev_obj->constant.config_desc->bConfigurationValue;
    // String descriptors are allowed to be NULL as not all devices support them
    dev_info->str_desc_manufacturer = dev_obj->constant.str_desc_manu;
    dev_info->str_desc_product = dev_obj->constant.str_desc_product;
    dev_info->str_desc_serial_num = dev_obj->constant.str_desc_ser_num;
    ret = ESP_OK;
exit:
    return ret;
}

esp_err_t usbh_dev_get_desc(usb_device_handle_t dev_hdl, const usb_device_desc_t **dev_desc_ret)
{
    USBH_CHECK(dev_hdl != NULL && dev_desc_ret != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;

    USBH_ENTER_CRITICAL();
    USBH_CHECK_FROM_CRIT(dev_obj->dynamic.state == USB_DEVICE_STATE_CONFIGURED, ESP_ERR_INVALID_STATE);
    USBH_EXIT_CRITICAL();

    *dev_desc_ret = dev_obj->constant.desc;
    return ESP_OK;
}

esp_err_t usbh_dev_get_config_desc(usb_device_handle_t dev_hdl, const usb_config_desc_t **config_desc_ret)
{
    USBH_CHECK(dev_hdl != NULL && config_desc_ret != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;

    esp_err_t ret;
    // Device must be in the configured state
    USBH_ENTER_CRITICAL();
    if (dev_obj->dynamic.state != USB_DEVICE_STATE_CONFIGURED) {
        USBH_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto exit;
    }
    USBH_EXIT_CRITICAL();
    assert(dev_obj->constant.config_desc);
    *config_desc_ret = dev_obj->constant.config_desc;
    ret = ESP_OK;
exit:
    return ret;
}

esp_err_t usbh_dev_submit_ctrl_urb(usb_device_handle_t dev_hdl, urb_t *urb)
{
    USBH_CHECK(dev_hdl != NULL && urb != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;
    USBH_CHECK(urb_check_args(urb), ESP_ERR_INVALID_ARG);
    bool xfer_is_in = ((usb_setup_packet_t *)urb->transfer.data_buffer)->bmRequestType & USB_BM_REQUEST_TYPE_DIR_IN;
    USBH_CHECK(transfer_check_usb_compliance(&(urb->transfer), USB_TRANSFER_TYPE_CTRL, dev_obj->constant.desc->bMaxPacketSize0, xfer_is_in), ESP_ERR_INVALID_ARG);

    USBH_ENTER_CRITICAL();
    USBH_CHECK_FROM_CRIT(dev_obj->dynamic.state == USB_DEVICE_STATE_CONFIGURED, ESP_ERR_INVALID_STATE);
    // Increment the control transfer count first
    dev_obj->dynamic.num_ctrl_xfers_inflight++;
    USBH_EXIT_CRITICAL();

    esp_err_t ret;
    if (hcd_pipe_get_state(dev_obj->constant.default_pipe) != HCD_PIPE_STATE_ACTIVE) {
        ret = ESP_ERR_INVALID_STATE;
        goto hcd_err;
    }
    ret = hcd_urb_enqueue(dev_obj->constant.default_pipe, urb);
    if (ret != ESP_OK) {
        goto hcd_err;
    }
    ret = ESP_OK;
    return ret;

hcd_err:
    USBH_ENTER_CRITICAL();
    dev_obj->dynamic.num_ctrl_xfers_inflight--;
    USBH_EXIT_CRITICAL();
    return ret;
}

// ----------------------------------------------- Interface Functions -------------------------------------------------

esp_err_t usbh_ep_alloc(usb_device_handle_t dev_hdl, usbh_ep_config_t *ep_config, usbh_ep_handle_t *ep_hdl_ret)
{
    USBH_CHECK(dev_hdl != NULL && ep_config != NULL && ep_hdl_ret != NULL, ESP_ERR_INVALID_ARG);
    uint8_t bEndpointAddress = ep_config->bEndpointAddress;
    USBH_CHECK(check_ep_addr(bEndpointAddress), ESP_ERR_INVALID_ARG);

    esp_err_t ret;
    device_t *dev_obj = (device_t *)dev_hdl;
    endpoint_t *ep_obj;

    // Find the endpoint descriptor from the device's current configuration descriptor
    const usb_ep_desc_t *ep_desc = usb_parse_endpoint_descriptor_by_address(dev_obj->constant.config_desc, ep_config->bInterfaceNumber, ep_config->bAlternateSetting, ep_config->bEndpointAddress, NULL);
    if (ep_desc == NULL) {
        return ESP_ERR_NOT_FOUND;
    }
    // Allocate the endpoint object
    ret = endpoint_alloc(dev_obj, ep_desc, ep_config, &ep_obj);
    if (ret != ESP_OK) {
        goto alloc_err;
    }

    // We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    USBH_ENTER_CRITICAL();
    // Check the device's state before we assign the a pipe to the allocated endpoint
    if (dev_obj->dynamic.state != USB_DEVICE_STATE_CONFIGURED) {
        USBH_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto dev_state_err;
    }
    USBH_EXIT_CRITICAL();
    // Check if the endpoint has already been allocated
    if (get_ep_from_addr(dev_obj, bEndpointAddress) == NULL) {
        set_ep_from_addr(dev_obj, bEndpointAddress, ep_obj);
        // Write back the endpoint handle
        *ep_hdl_ret = (usbh_ep_handle_t)ep_obj;
        ret = ESP_OK;
    } else {
        // Endpoint is already allocated
        ret = ESP_ERR_INVALID_STATE;
    }
dev_state_err:
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);

    // If the endpoint was not assigned, free it
    if (ret != ESP_OK) {
        endpoint_free(ep_obj);
    }
alloc_err:
    return ret;
}

esp_err_t usbh_ep_free(usbh_ep_handle_t ep_hdl)
{
    USBH_CHECK(ep_hdl != NULL, ESP_ERR_INVALID_ARG);

    esp_err_t ret;
    endpoint_t *ep_obj = (endpoint_t *)ep_hdl;
    device_t *dev_obj = (device_t *)ep_obj->constant.dev;
    uint8_t bEndpointAddress = ep_obj->constant.ep_desc->bEndpointAddress;

    // Todo: Check that the EP's underlying pipe is halted before allowing the EP to be freed (IDF-7273)
    // Check that the the EP's underlying pipe has no more in-flight URBs
    if (hcd_pipe_get_num_urbs(ep_obj->constant.pipe_hdl) != 0) {
        ret = ESP_ERR_INVALID_STATE;
        goto exit;
    }

    // We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    // Check if the endpoint was allocated on this device
    if (ep_obj == get_ep_from_addr(dev_obj, bEndpointAddress)) {
        // Clear the endpoint from the device's endpoint object list
        set_ep_from_addr(dev_obj, bEndpointAddress, NULL);
        ret = ESP_OK;
    } else {
        ret = ESP_ERR_NOT_FOUND;
    }
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);

    // Finally, we free the endpoint object
    if (ret == ESP_OK) {
        endpoint_free(ep_obj);
    }
exit:
    return ret;
}

esp_err_t usbh_ep_get_handle(usb_device_handle_t dev_hdl, uint8_t bEndpointAddress, usbh_ep_handle_t *ep_hdl_ret)
{
    USBH_CHECK(dev_hdl != NULL && ep_hdl_ret != NULL, ESP_ERR_INVALID_ARG);
    USBH_CHECK(check_ep_addr(bEndpointAddress), ESP_ERR_INVALID_ARG);

    esp_err_t ret;
    device_t *dev_obj = (device_t *)dev_hdl;
    endpoint_t *ep_obj;

    // We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    ep_obj = get_ep_from_addr(dev_obj, bEndpointAddress);
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);
    if (ep_obj) {
        *ep_hdl_ret = (usbh_ep_handle_t)ep_obj;
        ret = ESP_OK;
    } else {
        ret = ESP_ERR_NOT_FOUND;
    }

    return ret;
}

esp_err_t usbh_ep_enqueue_urb(usbh_ep_handle_t ep_hdl, urb_t *urb)
{
    USBH_CHECK(ep_hdl != NULL && urb != NULL, ESP_ERR_INVALID_ARG);
    USBH_CHECK(urb_check_args(urb), ESP_ERR_INVALID_ARG);

    endpoint_t *ep_obj = (endpoint_t *)ep_hdl;

    USBH_CHECK(transfer_check_usb_compliance(&(urb->transfer),
                                             USB_EP_DESC_GET_XFERTYPE(ep_obj->constant.ep_desc),
                                             USB_EP_DESC_GET_MPS(ep_obj->constant.ep_desc),
                                             USB_EP_DESC_GET_EP_DIR(ep_obj->constant.ep_desc)),
               ESP_ERR_INVALID_ARG);
    // Check that the EP's underlying pipe is in the active state before submitting the URB
    if (hcd_pipe_get_state(ep_obj->constant.pipe_hdl) != HCD_PIPE_STATE_ACTIVE) {
        return ESP_ERR_INVALID_STATE;
    }
    // Enqueue the URB to the EP's underlying pipe
    return hcd_urb_enqueue(ep_obj->constant.pipe_hdl, urb);
}

esp_err_t usbh_ep_dequeue_urb(usbh_ep_handle_t ep_hdl, urb_t **urb_ret)
{
    USBH_CHECK(ep_hdl != NULL && urb_ret != NULL, ESP_ERR_INVALID_ARG);

    endpoint_t *ep_obj = (endpoint_t *)ep_hdl;
    // Enqueue the URB to the EP's underlying pipe
    *urb_ret = hcd_urb_dequeue(ep_obj->constant.pipe_hdl);
    return ESP_OK;
}

esp_err_t usbh_ep_command(usbh_ep_handle_t ep_hdl, usbh_ep_cmd_t command)
{
    USBH_CHECK(ep_hdl != NULL, ESP_ERR_INVALID_ARG);

    endpoint_t *ep_obj = (endpoint_t *)ep_hdl;
    // Send the command to the EP's underlying pipe
    return hcd_pipe_command(ep_obj->constant.pipe_hdl, (hcd_pipe_cmd_t)command);
}

void *usbh_ep_get_context(usbh_ep_handle_t ep_hdl)
{
    assert(ep_hdl);
    endpoint_t *ep_obj = (endpoint_t *)ep_hdl;
    return hcd_pipe_get_context(ep_obj->constant.pipe_hdl);
}

// -------------------------------------------------- Hub Functions ----------------------------------------------------

// ------------------- Device Related ----------------------

esp_err_t usbh_hub_is_installed(usbh_hub_req_cb_t hub_req_callback, void *callback_arg)
{
    USBH_CHECK(hub_req_callback != NULL, ESP_ERR_INVALID_ARG);

    USBH_ENTER_CRITICAL();
    // Check that USBH is already installed
    USBH_CHECK_FROM_CRIT(p_usbh_obj != NULL, ESP_ERR_INVALID_STATE);
    // Check that Hub has not be installed yet
    USBH_CHECK_FROM_CRIT(p_usbh_obj->constant.hub_req_cb == NULL, ESP_ERR_INVALID_STATE);
    p_usbh_obj->constant.hub_req_cb = hub_req_callback;
    p_usbh_obj->constant.hub_req_cb_arg = callback_arg;
    USBH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t usbh_hub_add_dev(hcd_port_handle_t port_hdl, usb_speed_t dev_speed, usb_device_handle_t *new_dev_hdl, hcd_pipe_handle_t *default_pipe_hdl)
{
    // Note: Parent device handle can be NULL if it's connected to the root hub
    USBH_CHECK(new_dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    esp_err_t ret;
    device_t *dev_obj;
    ret = device_alloc(port_hdl, dev_speed, &dev_obj);
    if (ret != ESP_OK) {
        return ret;
    }
    // Write-back device handle
    *new_dev_hdl = (usb_device_handle_t)dev_obj;
    *default_pipe_hdl = dev_obj->constant.default_pipe;
    ret = ESP_OK;
    return ret;
}

esp_err_t usbh_hub_pass_event(usb_device_handle_t dev_hdl, usbh_hub_event_t hub_event)
{
    USBH_CHECK(dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;

    bool call_proc_req_cb;
    switch (hub_event) {
    case USBH_HUB_EVENT_PORT_ERROR: {
        USBH_ENTER_CRITICAL();
        dev_obj->dynamic.flags.is_gone = 1;
        // Check if the device can be freed now
        if (dev_obj->dynamic.ref_count == 0) {
            dev_obj->dynamic.flags.waiting_free = 1;
            // Device is already waiting free so none of it's EP's will be in use. Can free immediately.
            call_proc_req_cb = _dev_set_actions(dev_obj, DEV_ACTION_FREE_AND_RECOVER); // Port error occurred so we need to recover it
        } else {
            call_proc_req_cb = _dev_set_actions(dev_obj,
                                                DEV_ACTION_EPn_HALT_FLUSH |
                                                DEV_ACTION_EP0_FLUSH |
                                                DEV_ACTION_EP0_DEQUEUE |
                                                DEV_ACTION_PROP_GONE_EVT);
        }
        USBH_EXIT_CRITICAL();
        break;
    }
    case USBH_HUB_EVENT_PORT_DISABLED: {
        USBH_ENTER_CRITICAL();
        assert(dev_obj->dynamic.ref_count == 0);    // At this stage, the device should have been closed by all users
        dev_obj->dynamic.flags.waiting_free = 1;
        call_proc_req_cb = _dev_set_actions(dev_obj, DEV_ACTION_FREE);
        USBH_EXIT_CRITICAL();
        break;
    }
    default:
        return ESP_ERR_INVALID_ARG;
    }

    if (call_proc_req_cb) {
        p_usbh_obj->constant.proc_req_cb(USB_PROC_REQ_SOURCE_USBH, false, p_usbh_obj->constant.proc_req_cb_arg);
    }
    return ESP_OK;
}

// ----------------- Enumeration Related -------------------

esp_err_t usbh_hub_enum_fill_dev_addr(usb_device_handle_t dev_hdl, uint8_t dev_addr)
{
    USBH_CHECK(dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;

    USBH_ENTER_CRITICAL();
    dev_obj->dynamic.state = USB_DEVICE_STATE_ADDRESS;
    USBH_EXIT_CRITICAL();

    // We can modify the info members outside the critical section
    dev_obj->constant.address = dev_addr;
    return ESP_OK;
}

esp_err_t usbh_hub_enum_fill_dev_desc(usb_device_handle_t dev_hdl, const usb_device_desc_t *device_desc)
{
    USBH_CHECK(dev_hdl != NULL && device_desc != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;
    // We can modify the info members outside the critical section
    memcpy((usb_device_desc_t *)dev_obj->constant.desc, device_desc, sizeof(usb_device_desc_t));
    return ESP_OK;
}

esp_err_t usbh_hub_enum_fill_config_desc(usb_device_handle_t dev_hdl, const usb_config_desc_t *config_desc_full)
{
    USBH_CHECK(dev_hdl != NULL && config_desc_full != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;
    // Allocate memory to store the configuration descriptor
    usb_config_desc_t *config_desc = heap_caps_malloc(config_desc_full->wTotalLength, MALLOC_CAP_DEFAULT);  // Buffer to copy over full configuration descriptor (wTotalLength)
    if (config_desc == NULL) {
        return ESP_ERR_NO_MEM;
    }
    // Copy the configuration descriptor
    memcpy(config_desc, config_desc_full, config_desc_full->wTotalLength);
    // Assign the config desc to the device object
    assert(dev_obj->constant.config_desc == NULL);
    dev_obj->constant.config_desc = config_desc;
    return ESP_OK;
}

esp_err_t usbh_hub_enum_fill_str_desc(usb_device_handle_t dev_hdl, const usb_str_desc_t *str_desc, int select)
{
    USBH_CHECK(dev_hdl != NULL && str_desc != NULL && (select >= 0 && select < 3), ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;
    // Allocate memory to store the manufacturer string descriptor
    usb_str_desc_t *str_desc_fill = heap_caps_malloc(str_desc->bLength, MALLOC_CAP_DEFAULT);
    if (str_desc_fill == NULL) {
        return ESP_ERR_NO_MEM;
    }
    // Copy the string descriptor
    memcpy(str_desc_fill, str_desc, str_desc->bLength);
    // Assign filled string descriptor to the device object
    switch (select) {
    case 0:
        assert(dev_obj->constant.str_desc_manu == NULL);
        dev_obj->constant.str_desc_manu = str_desc_fill;
        break;
    case 1:
        assert(dev_obj->constant.str_desc_product == NULL);
        dev_obj->constant.str_desc_product = str_desc_fill;
        break;
    default:    // 2
        assert(dev_obj->constant.str_desc_ser_num == NULL);
        dev_obj->constant.str_desc_ser_num = str_desc_fill;
        break;
    }
    return ESP_OK;
}

esp_err_t usbh_hub_enum_done(usb_device_handle_t dev_hdl)
{
    USBH_CHECK(dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;

    // We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    USBH_ENTER_CRITICAL();
    dev_obj->dynamic.state = USB_DEVICE_STATE_CONFIGURED;
    // Add the device to list of devices, then trigger a device event
    TAILQ_INSERT_TAIL(&p_usbh_obj->dynamic.devs_idle_tailq, dev_obj, dynamic.tailq_entry);   // Add it to the idle device list first
    bool call_proc_req_cb = _dev_set_actions(dev_obj, DEV_ACTION_PROP_NEW);
    USBH_EXIT_CRITICAL();
    p_usbh_obj->mux_protected.num_device++;
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);

    // Update the EP0's underlying pipe's callback
    ESP_ERROR_CHECK(hcd_pipe_update_callback(dev_obj->constant.default_pipe, ep0_pipe_callback, (void *)dev_obj));
    // Call the processing request callback
    if (call_proc_req_cb) {
        p_usbh_obj->constant.proc_req_cb(USB_PROC_REQ_SOURCE_USBH, false, p_usbh_obj->constant.proc_req_cb_arg);
    }
    return ESP_OK;
}

esp_err_t usbh_hub_enum_failed(usb_device_handle_t dev_hdl)
{
    USBH_CHECK(dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;
    device_free(dev_obj);
    return ESP_OK;
}
