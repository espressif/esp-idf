/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_private/critical_section.h"
#include "usb_private.h"
#include "ext_hub.h"
#include "ext_port.h"
#include "usb/usb_helpers.h"

#define EXT_HUB_MAX_STATUS_BYTES_SIZE               (sizeof(uint32_t))
#define EXT_HUB_STATUS_CHANGE_FLAG                  (1 << 0)
#define EXT_HUB_STATUS_PORT1_CHANGE_FLAG            (1 << 1)
#define EXT_HUB_CTRL_TRANSFER_MAX_DATA_LEN          CONFIG_USB_HOST_CONTROL_TRANSFER_MAX_SIZE

/**
 * @brief Device state
 *
 * Global state of the Device
 */
typedef enum {
    EXT_HUB_STATE_ATTACHED,                         /**< Device attached, but not state is unknown (no: Hub Descriptor, Device status and Hub status) */
    EXT_HUB_STATE_CONFIGURED,                       /**< Device attached and configured (has Hub Descriptor, Device status and Hub status were requested )*/
    EXT_HUB_STATE_SUSPENDED,                        /**< Device suspended */
    EXT_HUB_STATE_RELEASED,                         /**< Device released and its ports are not available */
} ext_hub_state_t;

/**
 * @brief Device stages
 *
 * During the lifecycle, Hub requires different actions. To implement interaction with external Hub the FSM, based on these stages is using.
 *
 * Entry:
 * - Every new attached external Hub should start from EXT_HUB_STAGE_GET_HUB_DESCRIPTOR. Without Fetching Hub Descriptor, device is not configured and doesn't have any ports.
 * - After handling the response during EXT_HUB_STAGE_CHECK_HUB_DESCRIPTOR, the External Hub Driver configures the Device according to the data from Hub Descriptor.
 * - After completion of any stage, the Device does back to the IDLE stage and waits for another request. Source of the request could be: EP1 INT callback (the Hub or Ports changes) or external call.
 * - Stages, that don't required response handling could not end up with fail.
 */
typedef enum {
    // Device in IDLE state
    EXT_HUB_STAGE_IDLE = 0,                         /**< Device in idle state and do not fulfill any actions  */
    // Stages, required response handling
    EXT_HUB_STAGE_GET_DEVICE_STATUS,                /**< Device requests Device Status. For more details, refer to 9.4.5 Get Status of usb_20 */
    EXT_HUB_STAGE_CHECK_DEVICE_STATUS,              /**< Device received the Device Status and required its' handling */
    EXT_HUB_STAGE_GET_HUB_DESCRIPTOR,               /**< Device requests Hub Descriptor. For more details, refer to 11.24.2.5 Get Hub Descriptor of usb_20 */
    EXT_HUB_STAGE_CHECK_HUB_DESCRIPTOR,             /**< Device received the Hub Descriptor and requires its' handling  */
    EXT_HUB_STAGE_GET_HUB_STATUS,                   /**< Device requests Hub Status. For more details, refer to 11.24.2.6 Get Hub Status of usb_20 */
    EXT_HUB_STAGE_CHECK_HUB_STATUS,                 /**< Device received the Hub Status and requires its' handling  */
    // Stages, don't required get stage handling
    EXT_HUB_STAGE_CHECK_PORT_FEATURE,               /**< Device completed the Port Feature class-specific request (Set Feature or Clear Feature). For more details, refer to 11.24.2 Class-specific Requests of usb_20 */
    EXT_HUB_STAGE_CHECK_PORT_STATUS,                /**< Device completed the Port Get Status class-specific request. For more details, refer to 11.24.2 Class-specific Requests of usb_20 */
    EXT_HUB_STAGE_ERROR                             /**< Device has internal error and requires handling */
} ext_hub_stage_t;

const char *const ext_hub_stage_strings[] = {
    "IDLE",
    "GET_DEVICE_STATUS",
    "CHECK_DEVICE_STATUS",
    "GET_HUB_DESCRIPTOR",
    "CHECK_HUB_DESCRIPTOR",
    "GET_HUB_STATUS",
    "CHECK_HUB_STATUS",
    "PORT_FEATURE",
    "PORT_STATUS_REQUEST",
    "FAILURE"
};

/**
 * @brief Device action flags
 */
typedef enum {
    DEV_ACTION_EP0_COMPLETE             = (1 << 1),  /**< Device's Control EP transfer completed */
    DEV_ACTION_EP1_FLUSH                = (1 << 2),  /**< Device's Interrupt EP needs to be flushed */
    DEV_ACTION_EP1_DEQUEUE              = (1 << 3),  /**< Device's Interrupt EP needs to be dequeued */
    DEV_ACTION_EP1_CLEAR                = (1 << 4),  /**< Device's Interrupt EP needs to be cleared */
    DEV_ACTION_REQ                      = (1 << 5),  /**< Device has new actions and requires handling */
    DEV_ACTION_ERROR                    = (1 << 6),  /**< Device encounters an error */
    DEV_ACTION_RELEASE                  = (1 << 7),  /**< Device should be released */
    DEV_ACTION_FREE                     = (1 << 8),  /**< Device should be freed */
} dev_action_t;

typedef struct ext_hub_s ext_hub_dev_t;

/**
 * @brief External Hub device configuration parameters for device allocation
 */
typedef struct {
    usb_device_handle_t dev_hdl;        /**< Device's handle */
    uint8_t dev_addr;                   /**< Device's bus address */
    const usb_intf_desc_t *iface_desc;  /**< Device's Interface Descriptor pointer */
    const usb_ep_desc_t *ep_in_desc;    /**< Device's IN Endpoint Descriptor pointer */
} device_config_t;

struct ext_hub_s {
    struct {
        TAILQ_ENTRY(ext_hub_s) tailq_entry;
        union {
            struct {
                uint32_t in_pending_list: 1;        /**< Device is in pending list */
                uint32_t waiting_release: 1;        /**< Device waiting to be released */
                uint32_t waiting_children: 1;       /**< Device is waiting children to be freed */
                uint32_t waiting_free: 1;           /**< Device waiting to be freed */
                uint32_t is_gone: 1;                /**< Device is gone */
                uint32_t reserved27: 27;            /**< Reserved */
            };
            uint32_t val;                           /**< Device's flags value */
        } flags;
        uint32_t action_flags;                      /**< Device's action flags */
    } dynamic;                                      /**< Dynamic members require a critical section */

    struct {
        ext_hub_stage_t stage;                      /**< Device's stage */
        uint8_t maxchild;                           /**< Amount of allocated ports. Could be 0 for some Hubs. Is increased when new port is added and decreased when port has been freed. */
        ext_hub_state_t state;                      /**< Device's state */
    } single_thread;                                /**< Single thread members don't require a critical section, as long as they are never accessed from multiple threads */

    struct {
        // For optimisation & debug only
        uint8_t iface_num;                          /**< Device's bInterfaceNum */
        // Driver purpose
        uint8_t dev_addr;                           /**< Device's bus address */
        usb_device_handle_t dev_hdl;                /**< Device's handle */
        urb_t *ctrl_urb;                            /**< Device's Control pipe transfer URB */
        urb_t *in_urb;                              /**< Device's Interrupt pipe URB */
        usbh_ep_handle_t ep_in_hdl;                 /**< Device's Interrupt EP handle */

        usb_hub_descriptor_t *hub_desc;             /**< Device's Hub descriptor pointer. Could be NULL when not requested */
        ext_port_hdl_t *ports;                      /**< Flexible array of Ports. Could be NULL, when maxchild is 0 */
    } constant;                                     /**< Constant members. Do not change after installation thus do not require a critical section or mutex */
};

typedef struct {
    struct {
        TAILQ_HEAD(ext_hubs, ext_hub_s)    ext_hubs_tailq;              /**< Idle tailq */
        TAILQ_HEAD(ext_hubs_cb, ext_hub_s) ext_hubs_pending_tailq;      /**< Pending tailq */
    } dynamic;                          /**< Dynamic members require a critical section */

    struct {
        ext_hub_cb_t proc_req_cb;                   /**< Process callback  */
        void *proc_req_cb_arg;                      /**< Process callback argument */
        const ext_port_driver_api_t* port_driver;   /**< External Port Driver */
    } constant;                         /**< Constant members. Do not change after installation thus do not require a critical section or mutex */
} ext_hub_driver_t;

static ext_hub_driver_t *p_ext_hub_driver = NULL;

const char *EXT_HUB_TAG = "EXT_HUB";

// -----------------------------------------------------------------------------
// ------------------------------- Helpers -------------------------------------
// -----------------------------------------------------------------------------

DEFINE_CRIT_SECTION_LOCK_STATIC(ext_hub_driver_lock);
#define EXT_HUB_ENTER_CRITICAL()           esp_os_enter_critical(&ext_hub_driver_lock)
#define EXT_HUB_EXIT_CRITICAL()            esp_os_exit_critical(&ext_hub_driver_lock)
#define EXT_HUB_ENTER_CRITICAL_SAFE()      esp_os_enter_critical_safe(&ext_hub_driver_lock)
#define EXT_HUB_EXIT_CRITICAL_SAFE()       esp_os_exit_critical_safe(&ext_hub_driver_lock)

#define EXT_HUB_CHECK(cond, ret_val) ({              \
            if (!(cond)) {                                  \
                return (ret_val);                           \
            }                                               \
})
#define EXT_HUB_CHECK_FROM_CRIT(cond, ret_val) ({    \
            if (!(cond)) {                                  \
                EXT_HUB_EXIT_CRITICAL();             \
                return ret_val;                             \
            }                                               \
})

// -----------------------------------------------------------------------------
// ----------------------- Forward declaration ---------------------------------
// -----------------------------------------------------------------------------
static bool _device_set_actions(ext_hub_dev_t *ext_hub_dev, uint32_t action_flags);
static void device_error(ext_hub_dev_t *ext_hub_dev);
static void device_status_change_handle(ext_hub_dev_t *ext_hub_dev, const uint8_t* data, const int length);

// -----------------------------------------------------------------------------
// ---------------------- Callbacks (implementation) ---------------------------
// -----------------------------------------------------------------------------

static bool interrupt_pipe_cb(usbh_ep_handle_t ep_hdl, usbh_ep_event_t ep_event, void *user_arg, bool in_isr)
{
    uint32_t action_flags;
    ext_hub_dev_t *ext_hub_dev = (ext_hub_dev_t *)user_arg;

    switch (ep_event) {
    case USBH_EP_EVENT_URB_DONE: {
        // A interrupt transfer completed on EP1's pipe . We need to dequeue it
        action_flags = DEV_ACTION_EP1_DEQUEUE;
        break;
        case USBH_EP_EVENT_ERROR_XFER:
        case USBH_EP_EVENT_ERROR_URB_NOT_AVAIL:
        case USBH_EP_EVENT_ERROR_OVERFLOW:
            // EP1's pipe has encountered an error. We need to retire all URBs, dequeue them, then make the pipe active again
            action_flags = DEV_ACTION_EP1_FLUSH |
                           DEV_ACTION_EP1_DEQUEUE |
                           DEV_ACTION_EP1_CLEAR;
            if (in_isr) {
                ESP_EARLY_LOGE(EXT_HUB_TAG, "Device %d EP1 Error", ext_hub_dev->constant.dev_addr);
            } else {
                ESP_LOGE(EXT_HUB_TAG, "Device %d EP1 Error", ext_hub_dev->constant.dev_addr);
            }
            break;
        case USBH_EP_EVENT_ERROR_STALL:
            // EP1's pipe encountered a "protocol stall". We just need to dequeue URBs then make the pipe active again
            action_flags = DEV_ACTION_EP1_DEQUEUE | DEV_ACTION_EP1_CLEAR;
            if (in_isr) {
                ESP_EARLY_LOGE(EXT_HUB_TAG, "Device %d EP1 STALL", ext_hub_dev->constant.dev_addr);
            } else {
                ESP_LOGE(EXT_HUB_TAG, "Device %d EP1 STALL", ext_hub_dev->constant.dev_addr);
            }
            break;
        }
    default:
        action_flags = 0;
        break;
    }

    EXT_HUB_ENTER_CRITICAL_SAFE();
    bool call_proc_req_cb = _device_set_actions(ext_hub_dev, action_flags);
    EXT_HUB_EXIT_CRITICAL_SAFE();

    bool yield = false;
    if (call_proc_req_cb) {
        yield = p_ext_hub_driver->constant.proc_req_cb(in_isr, p_ext_hub_driver->constant.proc_req_cb_arg);
    }
    return yield;
}

/**
 * @brief Control transfer completion callback
 *
 * Is called by lower logic when transfer is completed with or without error
 *
 * @param[in] ctrl_xfer Pointer to a transfer buffer
 */
static void control_transfer_complete_cb(usb_transfer_t *ctrl_xfer)
{
    bool call_proc_req_cb = false;
    ext_hub_dev_t *ext_hub_dev = (ext_hub_dev_t *) ctrl_xfer->context;

    EXT_HUB_ENTER_CRITICAL();
    call_proc_req_cb = _device_set_actions(ext_hub_dev, DEV_ACTION_EP0_COMPLETE);
    EXT_HUB_EXIT_CRITICAL();

    if (call_proc_req_cb) {
        p_ext_hub_driver->constant.proc_req_cb(false, p_ext_hub_driver->constant.proc_req_cb_arg);
    }
}

static void interrupt_transfer_complete_cb(usb_transfer_t *intr_xfer)
{
    assert(intr_xfer);
    ext_hub_dev_t *ext_hub_dev = (ext_hub_dev_t *)intr_xfer->context;
    assert(ext_hub_dev);

    switch (intr_xfer->status) {
    case USB_TRANSFER_STATUS_COMPLETED:
        ESP_LOG_BUFFER_HEXDUMP(EXT_HUB_TAG, intr_xfer->data_buffer, intr_xfer->actual_num_bytes, ESP_LOG_VERBOSE);
        device_status_change_handle(ext_hub_dev, intr_xfer->data_buffer, intr_xfer->actual_num_bytes);
        break;
    case USB_TRANSFER_STATUS_NO_DEVICE:
        // Device was removed, nothing to do
        break;
    case USB_TRANSFER_STATUS_CANCELED:
        // Cancellation due to USB Host uninstall routine
        break;
    default:
        // Any other error
        ESP_LOGE(EXT_HUB_TAG, "[%d] Interrupt transfer failed, status %d", ext_hub_dev->constant.dev_addr, intr_xfer->status);
        device_error(ext_hub_dev);
        break;
    }

}

// -----------------------------------------------------------------------------
// --------------------------- Internal Logic  ---------------------------------
// -----------------------------------------------------------------------------

static bool _device_set_actions(ext_hub_dev_t *ext_hub_dev, uint32_t action_flags)
{
    /*
    THIS FUNCTION MUST BE CALLED FROM A CRITICAL SECTION
    */
    if (action_flags == 0) {
        return false;
    }
    bool call_proc_req_cb;
    // Check if device is already on the callback list
    if (!ext_hub_dev->dynamic.flags.in_pending_list) {
        // Move device form idle device list to callback device list
        TAILQ_REMOVE(&p_ext_hub_driver->dynamic.ext_hubs_tailq, ext_hub_dev, dynamic.tailq_entry);
        TAILQ_INSERT_TAIL(&p_ext_hub_driver->dynamic.ext_hubs_pending_tailq, ext_hub_dev, dynamic.tailq_entry);
        ext_hub_dev->dynamic.flags.in_pending_list = 1;
        call_proc_req_cb = true;
    } else {
        // The device is already on the callback list, thus a processing request is already pending.
        call_proc_req_cb = false;
    }
    ext_hub_dev->dynamic.action_flags |= action_flags;
    return call_proc_req_cb;
}

static esp_err_t device_enable_int_ep(ext_hub_dev_t *ext_hub_dev)
{
    bool call_proc_req_cb = false;
    bool is_active = true;
    ESP_LOGD(EXT_HUB_TAG, "[%d] Device ports handle complete", ext_hub_dev->constant.dev_addr);

    // Check if the device should be released
    EXT_HUB_ENTER_CRITICAL();
    if (ext_hub_dev->dynamic.flags.waiting_release) {
        is_active = false;
        call_proc_req_cb = _device_set_actions(ext_hub_dev, DEV_ACTION_RELEASE);
    }
    EXT_HUB_EXIT_CRITICAL();

    if (is_active) {
        ESP_LOGD(EXT_HUB_TAG, "[%d] Enable IN EP", ext_hub_dev->constant.dev_addr);
        esp_err_t ret = usbh_ep_enqueue_urb(ext_hub_dev->constant.ep_in_hdl, ext_hub_dev->constant.in_urb);
        if (ret != ESP_OK) {
            ESP_LOGE(EXT_HUB_TAG, "[%d] Failed to submit in urb: %s", ext_hub_dev->constant.dev_addr, esp_err_to_name(ret));
            device_error(ext_hub_dev);
        }
        return ret;
    }

    if (call_proc_req_cb) {
        p_ext_hub_driver->constant.proc_req_cb(false, p_ext_hub_driver->constant.proc_req_cb_arg);
    }
    return ESP_OK;
}

static void device_has_changed(ext_hub_dev_t *ext_hub_dev)
{
    // TODO: IDF-10053 Hub status change handling
    // After getting the IRQ about Hub status change we need to request status
    // device_get_status(ext_hub_dev);
    ESP_LOGW(EXT_HUB_TAG, "Hub status change has not been implemented yet");
    device_enable_int_ep(ext_hub_dev);
}

//
// Figure 11-22. Hub and Port Status Change Bitmap
// Bit     | N | ... | 3 | 2 | 1 | 0 |
// Value   |1/0| ... |1/0|1/0|1/0|1/0|
//           |         |   |   |   |
//           |         |   |   |   +------ Hub change detected
//           |         |   |   +---------- Port 1 change detected
//           |         |   +-------------- Port 2 change detected
//           |         +------------------ Port 3 change detected
//           |                             ...
//           +---------------------------- Port N change detected
//
static void device_status_change_handle(ext_hub_dev_t *ext_hub_dev, const uint8_t* data, const int length)
{
    uint32_t device_status = 0;
    // Driver does not support Hubs with EP IN wMaxPacketSize > 4
    assert(length <= EXT_HUB_MAX_STATUS_BYTES_SIZE);

    for (uint32_t i = 0; i < length; i++) {
        device_status |= (uint32_t)(data[i] << i);
    }

    if (device_status) {
        // If device has a status change, we will re-trigger back the transfer
        // after all handling will be done
        if (device_status & EXT_HUB_STATUS_CHANGE_FLAG) {
            // Check device status
            device_has_changed(ext_hub_dev);
        }
        // HINTs:
        // - every byte of Data IN has 8 bits of possible port statuses bits: (length * 8)
        // - very first bit of status is used for Hub status: (length * 8) - 1
        for (uint8_t i = 0; i < (length * 8) - 1; i++) {
            // Check ports statuses
            if (device_status & (EXT_HUB_STATUS_PORT1_CHANGE_FLAG << i)) {
                assert(i < ext_hub_dev->single_thread.maxchild);        // Port should be in range
                assert(p_ext_hub_driver->constant.port_driver);         // Port driver call should be valid
                // Request Port status to handle changes
                ESP_ERROR_CHECK(p_ext_hub_driver->constant.port_driver->get_status(ext_hub_dev->constant.ports[i]));
            }
        }
    } else {
        // Device status is 0. Could happen when HS Hub with HS device is connected to HS Hub, connected to FS root port
        // Re-trigger transfer right now
        device_enable_int_ep(ext_hub_dev);
    }
}

static void device_error(ext_hub_dev_t *ext_hub_dev)
{
    bool call_proc_req_cb = false;

    EXT_HUB_ENTER_CRITICAL();
    ext_hub_dev->dynamic.flags.waiting_release = 1;
    call_proc_req_cb = _device_set_actions(ext_hub_dev, DEV_ACTION_ERROR |
                                           DEV_ACTION_RELEASE);
    EXT_HUB_EXIT_CRITICAL();

    if (call_proc_req_cb) {
        p_ext_hub_driver->constant.proc_req_cb(false, p_ext_hub_driver->constant.proc_req_cb_arg);
    }
}

static esp_err_t device_port_new(ext_hub_dev_t *ext_hub_dev, uint8_t port_idx)
{
    ext_port_config_t port_config = {
        .context = (void*) ext_hub_dev,
        .parent_dev_hdl =  ext_hub_dev->constant.dev_hdl,
        .parent_port_num = port_idx + 1,
        .port_power_delay_ms = ext_hub_dev->constant.hub_desc->bPwrOn2PwrGood * 2,
    };

    assert(p_ext_hub_driver->constant.port_driver);
    esp_err_t ret = p_ext_hub_driver->constant.port_driver->new (&port_config, (void**) &ext_hub_dev->constant.ports[port_idx]);
    if (ret != ESP_OK) {
        ESP_LOGE(EXT_HUB_TAG, "[%d:%d] Port allocation error: %s", ext_hub_dev->constant.dev_addr, port_idx + 1, esp_err_to_name(ret));
        goto fail;
    }

    ext_hub_dev->single_thread.maxchild++;

fail:
    return ret;
}

static esp_err_t device_port_free(ext_hub_dev_t *ext_hub_dev, uint8_t port_idx)
{
    EXT_HUB_CHECK(ext_hub_dev->constant.ports[port_idx] != NULL, ESP_ERR_INVALID_STATE);

    bool call_proc_req_cb = false;

    assert(ext_hub_dev->single_thread.maxchild != 0);
    assert(p_ext_hub_driver->constant.port_driver);
    esp_err_t ret = p_ext_hub_driver->constant.port_driver->del(ext_hub_dev->constant.ports[port_idx]);

    if (ret != ESP_OK) {
        ESP_LOGE(EXT_HUB_TAG, "[%d:%d] Unable to free port: %s", ext_hub_dev->constant.dev_addr, port_idx + 1, esp_err_to_name(ret));
        goto exit;
    }

    ext_hub_dev->constant.ports[port_idx] = NULL;
    ext_hub_dev->single_thread.maxchild--;

    EXT_HUB_ENTER_CRITICAL();
    if (ext_hub_dev->dynamic.flags.waiting_free && ext_hub_dev->single_thread.maxchild == 0) {
        call_proc_req_cb = _device_set_actions(ext_hub_dev, DEV_ACTION_FREE);
    }
    EXT_HUB_EXIT_CRITICAL();

    if (call_proc_req_cb) {
        p_ext_hub_driver->constant.proc_req_cb(false, p_ext_hub_driver->constant.proc_req_cb_arg);
    }

    ret = ESP_OK;
exit:
    return ret;
}

static esp_err_t device_release_port(ext_hub_dev_t *ext_hub_dev, uint8_t port_idx)
{
    assert(ext_hub_dev->constant.ports[port_idx] != NULL);
    assert(ext_hub_dev->single_thread.maxchild != 0);
    assert(p_ext_hub_driver->constant.port_driver);

    // Mark port as gone
    esp_err_t ret = p_ext_hub_driver->constant.port_driver->gone(ext_hub_dev->constant.ports[port_idx]);
    if (ret == ESP_OK) {
        // Port doesn't have a device and can be freed right now
        ESP_ERROR_CHECK(device_port_free(ext_hub_dev, port_idx));
    } else if (ret == ESP_ERR_NOT_FINISHED) {
        // Port has a device and will be freed on recycle
        EXT_HUB_ENTER_CRITICAL();
        ext_hub_dev->dynamic.flags.waiting_children = 1;
        EXT_HUB_EXIT_CRITICAL();
        ret = ESP_OK;
    } else {
        ESP_LOGE(EXT_HUB_TAG, "[%d:%d] Unable to mark port as gone: %s",
                 ext_hub_dev->constant.dev_addr, port_idx + 1, esp_err_to_name(ret));
        return ret;
    }

    return ret;
}

static void device_release_ports(ext_hub_dev_t *ext_hub_dev)
{
    assert(ext_hub_dev->constant.hub_desc->bNbrPorts); // Sanity check
    for (uint8_t i = 0; i < ext_hub_dev->constant.hub_desc->bNbrPorts; i++) {
        ESP_ERROR_CHECK(device_release_port(ext_hub_dev, i));
    }
}

static void device_release(ext_hub_dev_t *ext_hub_dev)
{
    ESP_LOGD(EXT_HUB_TAG, "[%d] Device release", ext_hub_dev->constant.dev_addr);

    EXT_HUB_ENTER_CRITICAL();
    assert(ext_hub_dev->dynamic.flags.waiting_release); // Sanity check
    ext_hub_dev->dynamic.flags.waiting_release = 0;
    ext_hub_dev->dynamic.flags.waiting_free = 1;
    EXT_HUB_EXIT_CRITICAL();

    switch (ext_hub_dev->single_thread.state) {
    case EXT_HUB_STATE_CONFIGURED:
    case EXT_HUB_STATE_SUSPENDED:
        assert(ext_hub_dev->constant.hub_desc != NULL); // Device should have a Hub descriptor
        assert(p_ext_hub_driver->constant.port_driver); // Port driver should be available
        device_release_ports(ext_hub_dev);
        break;
    default:
        break;
    }
    ext_hub_dev->single_thread.state = EXT_HUB_STATE_RELEASED;
}

static esp_err_t device_alloc_desc(ext_hub_dev_t *ext_hub_hdl, const usb_hub_descriptor_t *hub_desc)
{
    // Allocate memory to store the configuration descriptor
    usb_hub_descriptor_t *desc = heap_caps_malloc(hub_desc->bDescLength, MALLOC_CAP_DEFAULT);  // Buffer to copy over full configuration descriptor (wTotalLength)
    if (desc == NULL) {
        return ESP_ERR_NO_MEM;
    }
    // Copy the hub descriptor
    memcpy(desc, hub_desc, hub_desc->bDescLength);
    // Assign the hub descriptor to the device object
    assert(ext_hub_hdl->constant.hub_desc == NULL);
    ext_hub_hdl->constant.hub_desc = desc;
    return ESP_OK;
}

static esp_err_t device_alloc(device_config_t *config, ext_hub_dev_t **ext_hub_dev)
{
    EXT_HUB_CHECK(config != NULL, ESP_ERR_INVALID_ARG);
    EXT_HUB_CHECK(config->dev_addr != 0, ESP_ERR_NOT_ALLOWED);

    esp_err_t ret;
    urb_t *ctrl_urb = NULL;
    urb_t *in_urb = NULL;

#if !ENABLE_MULTIPLE_HUBS
    usb_device_info_t dev_info;
    ESP_ERROR_CHECK(usbh_dev_get_info(config->dev_hdl, &dev_info));
    if (dev_info.parent.dev_hdl) {
        ESP_LOGW(EXT_HUB_TAG, "Multiple Hubs support disabled, Hub device was not initialized");
        ret = ESP_ERR_NOT_SUPPORTED;
        goto fail;
    }
#endif // ENABLE_MULTIPLE_HUBS

    ext_hub_dev_t *hub_dev = heap_caps_calloc(1, sizeof(ext_hub_dev_t), MALLOC_CAP_DEFAULT);

    if (hub_dev == NULL) {
        ESP_LOGE(EXT_HUB_TAG, "[%d] Unable to allocate device",
                 config->dev_addr);
        ret = ESP_ERR_NO_MEM;
        goto fail;
    }

    // Allocate Control transfer URB
    ctrl_urb = urb_alloc(sizeof(usb_setup_packet_t) + EXT_HUB_CTRL_TRANSFER_MAX_DATA_LEN, 0);
    if (ctrl_urb == NULL) {
        ESP_LOGE(EXT_HUB_TAG, "[%d] Unable to allocate Control URB",
                 config->dev_addr);
        ret = ESP_ERR_NO_MEM;
        goto ctrl_urb_fail;
    }

    if (config->ep_in_desc->wMaxPacketSize > EXT_HUB_MAX_STATUS_BYTES_SIZE) {
        ESP_LOGE(EXT_HUB_TAG, "[%d] wMaxPacketSize=%d is not supported",
                 config->dev_addr,
                 config->ep_in_desc->wMaxPacketSize);
        ret = ESP_ERR_NOT_SUPPORTED;
        goto in_urb_fail;
    }

    in_urb = urb_alloc(config->ep_in_desc->wMaxPacketSize, 0);
    // Allocate Interrupt transfer URB
    if (in_urb == NULL) {
        ESP_LOGE(EXT_HUB_TAG, "[%d] Unable to allocate Interrupt URB",
                 config->dev_addr);
        ret =  ESP_ERR_NO_MEM;
        goto in_urb_fail;
    }

    usbh_ep_handle_t ep_hdl;
    usbh_ep_config_t ep_config = {
        .bInterfaceNumber = config->iface_desc->bInterfaceNumber,
        .bAlternateSetting = config->iface_desc->bAlternateSetting,
        .bEndpointAddress = config->ep_in_desc->bEndpointAddress,
        .ep_cb = interrupt_pipe_cb,
        .ep_cb_arg = (void *)hub_dev,
        .context = (void *)hub_dev,
    };

    ret = usbh_ep_alloc(config->dev_hdl, &ep_config, &ep_hdl);
    if (ret != ESP_OK) {
        ESP_LOGE(EXT_HUB_TAG, "[%d] Interrupt EP allocation failure: %s",
                 config->dev_addr,
                 esp_err_to_name(ret));
        goto ep_fail;
    }
    // Configure Control transfer URB
    ctrl_urb->usb_host_client = (void *) p_ext_hub_driver;
    ctrl_urb->transfer.callback = control_transfer_complete_cb;
    ctrl_urb->transfer.context = (void *) hub_dev;

    // Client is a memory address of the p_ext_hub_driver driver object
    in_urb->usb_host_client = (void *) p_ext_hub_driver;
    in_urb->transfer.callback = interrupt_transfer_complete_cb;
    in_urb->transfer.context = (void *) hub_dev;
    in_urb->transfer.num_bytes = config->ep_in_desc->wMaxPacketSize;

    hub_dev->dynamic.flags.val = 0;
    hub_dev->dynamic.action_flags = 0;
    // Mutex protected
    hub_dev->single_thread.state = EXT_HUB_STATE_ATTACHED;
    // Constant members
    hub_dev->constant.ep_in_hdl = ep_hdl;
    hub_dev->constant.ctrl_urb = ctrl_urb;
    hub_dev->constant.in_urb = in_urb;
    hub_dev->constant.dev_hdl = config->dev_hdl;
    hub_dev->constant.dev_addr = config->dev_addr;
    hub_dev->constant.iface_num = config->iface_desc->bInterfaceNumber;
    // Single thread members
    hub_dev->single_thread.stage = EXT_HUB_STAGE_IDLE;
    // We will update number of ports during Hub Descriptor handling stage
    hub_dev->single_thread.maxchild = 0;

    EXT_HUB_ENTER_CRITICAL();
    TAILQ_INSERT_TAIL(&p_ext_hub_driver->dynamic.ext_hubs_tailq, hub_dev, dynamic.tailq_entry);
    EXT_HUB_EXIT_CRITICAL();

    ESP_LOGD(EXT_HUB_TAG, "[%d] New device (iface %d)", config->dev_addr, hub_dev->constant.iface_num);

    *ext_hub_dev = hub_dev;
    return ret;

ep_fail:
    urb_free(in_urb);
in_urb_fail:
    urb_free(ctrl_urb);
ctrl_urb_fail:
    heap_caps_free(hub_dev);
fail:
    return ret;
}

static esp_err_t device_configure(ext_hub_dev_t *ext_hub_dev)
{
    esp_err_t ret;
    EXT_HUB_CHECK(ext_hub_dev->constant.hub_desc != NULL, ESP_ERR_INVALID_STATE);
    usb_hub_descriptor_t *hub_desc = ext_hub_dev->constant.hub_desc;

    ESP_LOGD(EXT_HUB_TAG, "[%d] Device configure (iface %d)",
             ext_hub_dev->constant.dev_addr,
             ext_hub_dev->constant.iface_num);

    if (hub_desc->wHubCharacteristics.compound) {
        ESP_LOGD(EXT_HUB_TAG, "\tCompound device");
    } else {
        ESP_LOGD(EXT_HUB_TAG, "\tStandalone HUB");
    }

    ESP_LOGD(EXT_HUB_TAG, "\t%d external port%s",
             ext_hub_dev->constant.hub_desc->bNbrPorts,
             (ext_hub_dev->constant.hub_desc->bNbrPorts == 1) ? "" : "s");

    switch (hub_desc->wHubCharacteristics.power_switching) {
    case USB_W_HUB_CHARS_PORT_PWR_CTRL_NO:
        ESP_LOGD(EXT_HUB_TAG, "\tNo power switching (usb 1.0)");
        break;
    case USB_W_HUB_CHARS_PORT_PWR_CTRL_INDV:
        ESP_LOGD(EXT_HUB_TAG, "\tIndividual port power switching");
        break;
    default:
        // USB_W_HUB_CHARS_PORT_PWR_CTRL_ALL
        ESP_LOGD(EXT_HUB_TAG, "\tAll ports power at once");
        break;
    }

    switch (hub_desc->wHubCharacteristics.ovr_current_protect) {
    case USB_W_HUB_CHARS_PORT_OVER_CURR_NO:
        ESP_LOGD(EXT_HUB_TAG, "\tNo over-current protection");
        break;
    case USB_W_HUB_CHARS_PORT_OVER_CURR_INDV:
        ESP_LOGD(EXT_HUB_TAG, "\tIndividual port over-current protection");
        break;
    default:
        // USB_W_HUB_CHARS_PORT_OVER_CURR_ALL
        ESP_LOGD(EXT_HUB_TAG, "\tGlobal over-current protection");
        break;
    }

    if (hub_desc->wHubCharacteristics.indicator_support) {
        ESP_LOGD(EXT_HUB_TAG, "\tPort indicators are supported");
    }

    ESP_LOGD(EXT_HUB_TAG, "\tPower on to power good time: %dms", hub_desc->bPwrOn2PwrGood * 2);
    ESP_LOGD(EXT_HUB_TAG, "\tMaximum current: %d mA", hub_desc->bHubContrCurrent);

    if (hub_desc->bNbrPorts == 0) {
        ESP_LOGW(EXT_HUB_TAG, "[%d] Device doesn't have any ports", ext_hub_dev->constant.dev_addr);
        // Nothing to configure, keep the device in EXT_HUB_STATE_ATTACHED
        return ESP_OK;
    }

    // Device has external ports
    // Create flexible array for port object pointers
    ext_hub_dev->constant.ports = heap_caps_calloc(ext_hub_dev->constant.hub_desc->bNbrPorts, sizeof(ext_port_hdl_t), MALLOC_CAP_DEFAULT);
    if (ext_hub_dev->constant.ports == NULL) {
        ESP_LOGE(EXT_HUB_TAG, "[%d] Ports list allocation error", ext_hub_dev->constant.dev_addr);
        ret = ESP_ERR_NO_MEM;
        goto fail;
    }

    // Create each port object
    for (uint8_t i = 0; i < ext_hub_dev->constant.hub_desc->bNbrPorts; i++) {
        ext_hub_dev->constant.ports[i] = NULL;
        ret = device_port_new(ext_hub_dev, i);
        if (ret != ESP_OK) {
            goto fail;
        }
    }

    ext_hub_dev->single_thread.state = EXT_HUB_STATE_CONFIGURED;
    return ESP_OK;

fail:
    // Free allocated resources
    for (uint8_t i = 0; i < ext_hub_dev->constant.hub_desc->bNbrPorts; i++) {
        if (ext_hub_dev->constant.ports[i]) {
            device_port_free(ext_hub_dev, i);
        }
    }

    if (ext_hub_dev->constant.ports) {
        assert(ext_hub_dev->single_thread.maxchild == 0);
        heap_caps_free(ext_hub_dev->constant.ports);
    }

    return ret;
}

static void device_free(ext_hub_dev_t *ext_hub_dev)
{
    assert(ext_hub_dev->single_thread.maxchild == 0); // All ports should be freed by now

    ESP_LOGD(EXT_HUB_TAG, "[%d] Freeing device", ext_hub_dev->constant.dev_addr);

    EXT_HUB_ENTER_CRITICAL();
    assert(ext_hub_dev->dynamic.flags.waiting_free == 1); // Hub should await being freed
    ext_hub_dev->dynamic.flags.waiting_free = 0;
    TAILQ_REMOVE(&p_ext_hub_driver->dynamic.ext_hubs_tailq, ext_hub_dev, dynamic.tailq_entry);
    EXT_HUB_EXIT_CRITICAL();

    ESP_ERROR_CHECK(usbh_ep_free(ext_hub_dev->constant.ep_in_hdl));
    ESP_ERROR_CHECK(usbh_dev_close(ext_hub_dev->constant.dev_hdl));

    if (ext_hub_dev->constant.hub_desc) {
        heap_caps_free(ext_hub_dev->constant.hub_desc);
    }
    if (ext_hub_dev->constant.ports) {
        heap_caps_free(ext_hub_dev->constant.ports);
    }

    urb_free(ext_hub_dev->constant.ctrl_urb);
    urb_free(ext_hub_dev->constant.in_urb);
    heap_caps_free(ext_hub_dev);
}

static esp_err_t get_dev_by_hdl(usb_device_handle_t dev_hdl, ext_hub_dev_t **ext_hub_hdl)
{
    esp_err_t ret = ESP_OK;
    // Go through the Hubs lists to find the hub with the specified device address
    ext_hub_dev_t *found_hub = NULL;
    ext_hub_dev_t *hub = NULL;

    EXT_HUB_ENTER_CRITICAL();
    TAILQ_FOREACH(hub, &p_ext_hub_driver->dynamic.ext_hubs_pending_tailq, dynamic.tailq_entry) {
        if (hub->constant.dev_hdl == dev_hdl) {
            found_hub = hub;
            goto exit;
        }
    }

    TAILQ_FOREACH(hub, &p_ext_hub_driver->dynamic.ext_hubs_tailq, dynamic.tailq_entry) {
        if (hub->constant.dev_hdl == dev_hdl) {
            found_hub = hub;
            goto exit;
        }
    }

exit:
    if (found_hub == NULL) {
        ret = ESP_ERR_NOT_FOUND;
    }
    EXT_HUB_EXIT_CRITICAL();

    *ext_hub_hdl = found_hub;
    return ret;
}

static esp_err_t get_dev_by_addr(uint8_t dev_addr, ext_hub_dev_t **ext_hub_hdl)
{
    esp_err_t ret = ESP_OK;
    // Go through the Hubs lists to find the Hub with the specified device address
    ext_hub_dev_t *found_hub = NULL;
    ext_hub_dev_t *hub = NULL;

    EXT_HUB_ENTER_CRITICAL();
    TAILQ_FOREACH(hub, &p_ext_hub_driver->dynamic.ext_hubs_pending_tailq, dynamic.tailq_entry) {
        if (hub->constant.dev_addr == dev_addr) {
            found_hub = hub;
            goto exit;
        }
    }

    TAILQ_FOREACH(hub, &p_ext_hub_driver->dynamic.ext_hubs_tailq, dynamic.tailq_entry) {
        if (hub->constant.dev_addr == dev_addr) {
            found_hub = hub;
            goto exit;
        }
    }

exit:
    if (found_hub == NULL) {
        ret = ESP_ERR_NOT_FOUND;
    }
    EXT_HUB_EXIT_CRITICAL();

    *ext_hub_hdl = found_hub;
    return ret;
}

// -----------------------------------------------------------------------------
// -------------------------- Device handling  ---------------------------------
// -----------------------------------------------------------------------------
static void handle_error(ext_hub_dev_t *ext_hub_dev)
{
    ESP_LOGE(EXT_HUB_TAG, "[%d] Device is not working properly, wait device removal",
             ext_hub_dev->constant.dev_addr);
    // Force change the stage
    ext_hub_dev->single_thread.stage = EXT_HUB_STAGE_ERROR;
}

static bool handle_hub_descriptor(ext_hub_dev_t *ext_hub_dev)
{
    esp_err_t ret;
    bool pass;
    usb_transfer_t *ctrl_xfer = &ext_hub_dev->constant.ctrl_urb->transfer;
    const usb_hub_descriptor_t *hub_desc = (const usb_hub_descriptor_t *)(ctrl_xfer->data_buffer + sizeof(usb_setup_packet_t));

    ESP_LOG_BUFFER_HEXDUMP(EXT_HUB_TAG, ctrl_xfer->data_buffer, ctrl_xfer->actual_num_bytes, ESP_LOG_VERBOSE);

    if (hub_desc->bDescriptorType != USB_CLASS_DESCRIPTOR_TYPE_HUB) {
        ESP_LOGE(EXT_HUB_TAG, "[%d] Hub Descriptor has wrong bDescriptorType", ext_hub_dev->constant.dev_addr);
        return false;
    }

    ret = device_alloc_desc(ext_hub_dev, hub_desc);
    if (ret != ESP_OK) {
        pass = false;
        goto exit;
    }

    ret = device_configure(ext_hub_dev);
    if (ret != ESP_OK) {
        pass = false;
        goto exit;
    }

    pass = true;
exit:
    return pass;
}

static bool handle_device_status(ext_hub_dev_t *ext_hub_dev)
{
    usb_transfer_t *ctrl_xfer = &ext_hub_dev->constant.ctrl_urb->transfer;
    const usb_device_status_t *dev_status = (const usb_device_status_t *)(ctrl_xfer->data_buffer + sizeof(usb_setup_packet_t));

    ESP_LOGD(EXT_HUB_TAG, "[%d] Device status: ", ext_hub_dev->constant.dev_addr);
    ESP_LOGD(EXT_HUB_TAG, "\tPower: %s", dev_status->self_powered ? "self-powered" : "bus-powered");
    ESP_LOGD(EXT_HUB_TAG, "\tRemoteWakeup: %s", dev_status->remote_wakeup ? "yes" : "no");

    if (dev_status->remote_wakeup) {
        // Device in remote_wakeup, we need send command Clear Device Feature: USB_W_VALUE_FEATURE_DEVICE_REMOTE_WAKEUP
        // HEX codes of command: 00 01 01 00 00 00 00 00
        // TODO: IDF-10055 Hub Support remote_wakeup feature
        ESP_LOGW(EXT_HUB_TAG, "Remote Wakeup feature has not been implemented yet");
    }
    return true;
}

static bool handle_hub_status(ext_hub_dev_t *ext_hub_dev)
{
    usb_transfer_t *ctrl_xfer = &ext_hub_dev->constant.ctrl_urb->transfer;
    const usb_hub_status_t *hub_status = (const usb_hub_status_t *)(ctrl_xfer->data_buffer + sizeof(usb_setup_packet_t));

    ESP_LOGD(EXT_HUB_TAG, "[%d] Hub status: ", ext_hub_dev->constant.dev_addr);
    ESP_LOGD(EXT_HUB_TAG, "\tExternal power supply: %s", hub_status->wHubStatus.HUB_LOCAL_POWER ? "yes" : "no");
    ESP_LOGD(EXT_HUB_TAG, "\tOvercurrent: %s", hub_status->wHubStatus.HUB_OVER_CURRENT ? "yes" : "no");

    if (hub_status->wHubStatus.HUB_OVER_CURRENT) {
        ESP_LOGE(EXT_HUB_TAG, "Device has overcurrent!");
        // Hub has an overcurrent, we need to disable all port and/or disable parent port
        // TODO: IDF-10056 Hubs overcurrent handling
        ESP_LOGW(EXT_HUB_TAG, "Feature has not been implemented yet");
        return false;
    }

    return true;
}

static void handle_port_feature(ext_hub_dev_t *ext_hub_dev)
{
    usb_transfer_t *ctrl_xfer = &ext_hub_dev->constant.ctrl_urb->transfer;

    uint8_t port_num = USB_SETUP_PACKET_GET_PORT((usb_setup_packet_t *)ctrl_xfer->data_buffer);
    uint8_t port_idx = port_num - 1;

    assert(port_idx < ext_hub_dev->constant.hub_desc->bNbrPorts);
    assert(p_ext_hub_driver->constant.port_driver);
    // [TODO: IDF-12174] Revisit the External Hub Driver to ensure consistent error handling.
    ESP_ERROR_CHECK(p_ext_hub_driver->constant.port_driver->req_process(ext_hub_dev->constant.ports[port_idx]));
}

static void handle_port_status(ext_hub_dev_t *ext_hub_dev)
{
    usb_transfer_t *ctrl_xfer = &ext_hub_dev->constant.ctrl_urb->transfer;

    uint8_t port_num = USB_SETUP_PACKET_GET_PORT((usb_setup_packet_t *)ctrl_xfer->data_buffer);
    uint8_t port_idx = port_num - 1;
    const usb_port_status_t *new_status = (const usb_port_status_t *)(ctrl_xfer->data_buffer + sizeof(usb_setup_packet_t));

    assert(port_idx < ext_hub_dev->constant.hub_desc->bNbrPorts);
    assert(p_ext_hub_driver->constant.port_driver);
    // [TODO: IDF-12174] Revisit the External Hub Driver to ensure consistent error handling.
    ESP_ERROR_CHECK(p_ext_hub_driver->constant.port_driver->set_status(ext_hub_dev->constant.ports[port_idx], new_status));
}

static bool device_control_request(ext_hub_dev_t *ext_hub_dev)
{
    esp_err_t ret;
    usb_transfer_t *transfer = &ext_hub_dev->constant.ctrl_urb->transfer;

    switch (ext_hub_dev->single_thread.stage) {
    case EXT_HUB_STAGE_GET_DEVICE_STATUS:
        USB_SETUP_PACKET_INIT_GET_STATUS((usb_setup_packet_t *)transfer->data_buffer);
        transfer->num_bytes = sizeof(usb_setup_packet_t) + sizeof(usb_device_status_t);
        break;
    case EXT_HUB_STAGE_GET_HUB_DESCRIPTOR:
        USB_SETUP_PACKET_INIT_GET_HUB_DESCRIPTOR((usb_setup_packet_t *)transfer->data_buffer);
        transfer->num_bytes = sizeof(usb_setup_packet_t) + sizeof(usb_hub_descriptor_t);
        break;
    case EXT_HUB_STAGE_GET_HUB_STATUS:
        USB_SETUP_PACKET_INIT_GET_HUB_STATUS((usb_setup_packet_t *)transfer->data_buffer);
        transfer->num_bytes = sizeof(usb_setup_packet_t) + sizeof(usb_hub_status_t);
        break;
    default:
        // Should never occur
        abort();
        break;
    }

    ret = usbh_dev_submit_ctrl_urb(ext_hub_dev->constant.dev_hdl, ext_hub_dev->constant.ctrl_urb);
    if (ret != ESP_OK) {
        ESP_LOGE(EXT_HUB_TAG, "Failed to submit ctrl urb: %s", esp_err_to_name(ret));
        return false;
    }

    return true;
}

static bool device_control_response_handling(ext_hub_dev_t *ext_hub_dev)
{
    bool stage_pass = false;
    usb_transfer_t *ctrl_xfer = &ext_hub_dev->constant.ctrl_urb->transfer;

    // Check transfer status
    if (ctrl_xfer->status != USB_TRANSFER_STATUS_COMPLETED) {
        ESP_LOGE(EXT_HUB_TAG, "[%d] Control request bad transfer status %d",
                 ext_hub_dev->constant.dev_addr,
                 ctrl_xfer->status);
        return stage_pass;
    }

    // Transfer completed, verbose data in ESP_LOG_VERBOSE is set
    ESP_LOG_BUFFER_HEXDUMP(EXT_HUB_TAG, ctrl_xfer->data_buffer, ctrl_xfer->actual_num_bytes, ESP_LOG_VERBOSE);

    switch (ext_hub_dev->single_thread.stage) {
    case EXT_HUB_STAGE_CHECK_DEVICE_STATUS:
        stage_pass = handle_device_status(ext_hub_dev);
        break;
    case EXT_HUB_STAGE_CHECK_HUB_DESCRIPTOR:
        stage_pass = handle_hub_descriptor(ext_hub_dev);
        break;
    case EXT_HUB_STAGE_CHECK_HUB_STATUS:
        stage_pass = handle_hub_status(ext_hub_dev);
        break;
    case EXT_HUB_STAGE_CHECK_PORT_FEATURE:
        handle_port_feature(ext_hub_dev);
        stage_pass =  true;
        break;
    case EXT_HUB_STAGE_CHECK_PORT_STATUS:
        handle_port_status(ext_hub_dev);
        stage_pass = true;
        break;
    default:
        // Should never occur
        abort();
        break;
    }

    return stage_pass;
}

static bool stage_need_process(ext_hub_stage_t stage)
{
    bool need_process_cb = false;

    switch (stage) {
    // Stages, required control transfer
    case EXT_HUB_STAGE_GET_DEVICE_STATUS:
    case EXT_HUB_STAGE_GET_HUB_DESCRIPTOR:
    case EXT_HUB_STAGE_GET_HUB_STATUS:
        need_process_cb = true;
        break;
    default:
        break;
    }

    return need_process_cb;
}

// return
// true - next stage requires the processing
// false - terminal stage
static bool device_set_next_stage(ext_hub_dev_t *ext_hub_dev, bool last_stage_pass)
{
    bool call_proc_req_cb = false;
    ext_hub_stage_t last_stage = ext_hub_dev->single_thread.stage;
    ext_hub_stage_t next_stage;

    if (last_stage_pass) {
        // Device doesn't have an error
        ESP_LOGD(EXT_HUB_TAG, "Stage %s OK", ext_hub_stage_strings[last_stage]);
        if (last_stage == EXT_HUB_STAGE_GET_DEVICE_STATUS ||
                last_stage == EXT_HUB_STAGE_GET_HUB_DESCRIPTOR ||
                last_stage == EXT_HUB_STAGE_GET_HUB_STATUS) {
            // Simply increment to get the next stage
            next_stage = last_stage + 1;
        } else {
            // Terminal stages, move to IDLE
            next_stage = EXT_HUB_STAGE_IDLE;
            EXT_HUB_ENTER_CRITICAL();
            if (ext_hub_dev->dynamic.flags.waiting_release) {
                call_proc_req_cb = _device_set_actions(ext_hub_dev, DEV_ACTION_RELEASE);
            }
            EXT_HUB_EXIT_CRITICAL();
        }
        ext_hub_dev->single_thread.stage = next_stage;
        call_proc_req_cb = stage_need_process(next_stage);
    } else {
        // Device has an error
        ESP_LOGE(EXT_HUB_TAG, "Stage %s FAILED", ext_hub_stage_strings[last_stage]);
        // Set error and wait device to be removed
        EXT_HUB_ENTER_CRITICAL();
        call_proc_req_cb = _device_set_actions(ext_hub_dev, DEV_ACTION_ERROR);
        EXT_HUB_EXIT_CRITICAL();
    }

    return call_proc_req_cb;
}

static void handle_device(ext_hub_dev_t *ext_hub_dev)
{
    bool call_proc_req_cb;
    bool stage_pass = false;
    // FSM for external Hub
    switch (ext_hub_dev->single_thread.stage) {
    case EXT_HUB_STAGE_GET_DEVICE_STATUS:
    case EXT_HUB_STAGE_GET_HUB_DESCRIPTOR:
    case EXT_HUB_STAGE_GET_HUB_STATUS:
        stage_pass = device_control_request(ext_hub_dev);
        break;
    case EXT_HUB_STAGE_CHECK_DEVICE_STATUS:
    case EXT_HUB_STAGE_CHECK_HUB_DESCRIPTOR:
    case EXT_HUB_STAGE_CHECK_HUB_STATUS:
    case EXT_HUB_STAGE_CHECK_PORT_FEATURE:
    case EXT_HUB_STAGE_CHECK_PORT_STATUS:
        stage_pass = device_control_response_handling(ext_hub_dev);
        break;
    default:
        // No one must handle the device when EXT_HUB_STAGE_IDLE, so it should never occur
        abort();
        break;
    }

    call_proc_req_cb = device_set_next_stage(ext_hub_dev, stage_pass);

    if (call_proc_req_cb) {
        p_ext_hub_driver->constant.proc_req_cb(false, p_ext_hub_driver->constant.proc_req_cb_arg);
    }
}

static void handle_ep1_flush(ext_hub_dev_t *ext_hub_dev)
{
    ESP_ERROR_CHECK(usbh_ep_command(ext_hub_dev->constant.ep_in_hdl, USBH_EP_CMD_HALT));
    ESP_ERROR_CHECK(usbh_ep_command(ext_hub_dev->constant.ep_in_hdl, USBH_EP_CMD_FLUSH));
}

static void handle_ep1_dequeue(ext_hub_dev_t *ext_hub_dev)
{
    // Dequeue all URBs and run their transfer callback
    ESP_LOGD(EXT_HUB_TAG, "[%d] Interrupt dequeue", ext_hub_dev->constant.dev_addr);

    urb_t *urb;
    usbh_ep_dequeue_urb(ext_hub_dev->constant.ep_in_hdl, &urb);
    while (urb != NULL) {
        // Clear the transfer's in-flight flag to indicate the transfer is no longer in-flight
        urb->usb_host_inflight = false;
        urb->transfer.callback(&urb->transfer);
        usbh_ep_dequeue_urb(ext_hub_dev->constant.ep_in_hdl, &urb);
    }
}

static void handle_ep1_clear(ext_hub_dev_t *ext_hub_dev)
{
    // We allow the pipe command to fail just in case the pipe becomes invalid mid command
    usbh_ep_command(ext_hub_dev->constant.ep_in_hdl, USBH_EP_CMD_CLEAR);
}

// -----------------------------------------------------------------------------
// ------------------------------ Driver ---------------------------------------
// -----------------------------------------------------------------------------

esp_err_t ext_hub_install(const ext_hub_config_t *config)
{
    esp_err_t ret;
    ext_hub_driver_t *ext_hub_drv = heap_caps_calloc(1, sizeof(ext_hub_driver_t), MALLOC_CAP_DEFAULT);
    if (ext_hub_drv == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
    // Save callbacks
    ext_hub_drv->constant.proc_req_cb = config->proc_req_cb;
    ext_hub_drv->constant.proc_req_cb_arg = config->proc_req_cb_arg;
    // Copy Port driver pointer
    ext_hub_drv->constant.port_driver = config->port_driver;

    if (ext_hub_drv->constant.port_driver == NULL) {
        ESP_LOGE(EXT_HUB_TAG, "Port Driver has not been implemented yet");
        ret = ESP_ERR_NOT_FINISHED;
        goto err;
    }

    TAILQ_INIT(&ext_hub_drv->dynamic.ext_hubs_tailq);
    TAILQ_INIT(&ext_hub_drv->dynamic.ext_hubs_pending_tailq);

    EXT_HUB_ENTER_CRITICAL();
    if (p_ext_hub_driver != NULL) {
        EXT_HUB_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto err;
    }
    p_ext_hub_driver = ext_hub_drv;
    EXT_HUB_EXIT_CRITICAL();

    ESP_LOGD(EXT_HUB_TAG, "Driver installed");
    return ESP_OK;

err:
    heap_caps_free(ext_hub_drv);
    return ret;
}

esp_err_t ext_hub_uninstall(void)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_INVALID_STATE);
    EXT_HUB_CHECK_FROM_CRIT(TAILQ_EMPTY(&p_ext_hub_driver->dynamic.ext_hubs_tailq), ESP_ERR_INVALID_STATE);
    EXT_HUB_CHECK_FROM_CRIT(TAILQ_EMPTY(&p_ext_hub_driver->dynamic.ext_hubs_pending_tailq), ESP_ERR_INVALID_STATE);
    ext_hub_driver_t *ext_hub_drv = p_ext_hub_driver;
    p_ext_hub_driver = NULL;
    EXT_HUB_EXIT_CRITICAL();

    // Free resources
    heap_caps_free(ext_hub_drv);
    ESP_LOGD(EXT_HUB_TAG, "Driver uninstalled");
    return ESP_OK;
}

void *ext_hub_get_client(void)
{
    bool driver_installed = false;
    EXT_HUB_ENTER_CRITICAL();
    driver_installed = (p_ext_hub_driver != NULL);
    EXT_HUB_EXIT_CRITICAL();
    return (driver_installed) ? (void*) p_ext_hub_driver : NULL;
}

// -----------------------------------------------------------------------------
// -------------------------- External Hub API ---------------------------------
// -----------------------------------------------------------------------------

esp_err_t ext_hub_get_handle(usb_device_handle_t dev_hdl, ext_hub_handle_t *ext_hub_hdl)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_INVALID_STATE);
    EXT_HUB_EXIT_CRITICAL();
    return get_dev_by_hdl(dev_hdl, ext_hub_hdl);
}

static esp_err_t find_first_intf_desc(const usb_config_desc_t *config_desc, device_config_t *hub_config)
{
    bool iface_found = false;
    const usb_ep_desc_t *ep_in_desc = NULL;

    int offset = 0;
    const usb_intf_desc_t *next_intf_desc = (const usb_intf_desc_t *)usb_parse_next_descriptor_of_type(
                                                (const usb_standard_desc_t *)config_desc,
                                                config_desc->wTotalLength,
                                                USB_B_DESCRIPTOR_TYPE_INTERFACE,
                                                &offset);

    while (next_intf_desc != NULL) {
        if (iface_found) {
            // TODO: IDF-10058 Hubs support interface selection (HS)
            ESP_LOGW(EXT_HUB_TAG, "Device has several Interfaces, selection has not been implemented yet. Using first.");
            break;
        }
        // Parse all interfaces
        if (USB_CLASS_HUB == next_intf_desc->bInterfaceClass) {
            // We have found the first interface descriptor with matching bInterfaceNumber

            if (next_intf_desc->bInterfaceProtocol != USB_B_DEV_PROTOCOL_HUB_FS) {
                // TODO: IDF-10059 Hubs support multiple TT (HS)
                if (next_intf_desc->bInterfaceProtocol != USB_B_DEV_PROTOCOL_HUB_HS_NO_TT) {
                    ESP_LOGW(EXT_HUB_TAG, "Transaction Translator has not been implemented yet");
                }

                switch (next_intf_desc->bInterfaceProtocol) {
                case USB_B_DEV_PROTOCOL_HUB_HS_NO_TT:
                    ESP_LOGD(EXT_HUB_TAG, "\tNo TT");
                    break;
                case USB_B_DEV_PROTOCOL_HUB_HS_SINGLE_TT:
                    ESP_LOGD(EXT_HUB_TAG, "\tSingle TT");
                    break;
                case USB_B_DEV_PROTOCOL_HUB_HS_MULTI_TT:
                    ESP_LOGD(EXT_HUB_TAG, "\tMulti TT");
                    break;
                default:
                    ESP_LOGE(EXT_HUB_TAG, "\tInterface Protocol (%#x) not supported", next_intf_desc->bInterfaceProtocol);
                    goto next_iface;
                }
            }

            // Hub Interface always should have only one Interrupt endpoint
            if (next_intf_desc->bNumEndpoints != 1) {
                ESP_LOGE(EXT_HUB_TAG, "Unexpected number of endpoints (%d)", next_intf_desc->bNumEndpoints);
                goto next_iface;
            }
            // Get related IN EP
            ep_in_desc = usb_parse_endpoint_descriptor_by_index(next_intf_desc, 0, config_desc->wTotalLength, &offset);
            if (ep_in_desc == NULL) {
                ESP_LOGE(EXT_HUB_TAG, "EP descriptor not found (iface=%d)", next_intf_desc->bInterfaceNumber);
                goto next_iface;
            }

            if (!USB_EP_DESC_GET_EP_DIR(ep_in_desc) ||
                    (USB_EP_DESC_GET_XFERTYPE(ep_in_desc) != USB_TRANSFER_TYPE_INTR)) {
                ESP_LOGE(EXT_HUB_TAG, "Interrupt EP not found (iface=%d)", next_intf_desc->bInterfaceNumber);
                goto next_iface;
            }

            // Interface found, fill the config
            hub_config->iface_desc = next_intf_desc;
            hub_config->ep_in_desc = ep_in_desc;
            iface_found = true;
        }

next_iface:
        next_intf_desc = (const usb_intf_desc_t *)usb_parse_next_descriptor_of_type(
                             (const usb_standard_desc_t *)next_intf_desc,
                             config_desc->wTotalLength,
                             USB_B_DESCRIPTOR_TYPE_INTERFACE,
                             &offset);
    }

    return (iface_found) ? ESP_OK : ESP_ERR_NOT_FOUND;
}

esp_err_t ext_hub_new_dev(uint8_t dev_addr)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_HUB_EXIT_CRITICAL();
    esp_err_t ret;
    ext_hub_dev_t *hub_dev = NULL;
    usb_device_handle_t dev_hdl = NULL;
    const usb_config_desc_t *config_desc = NULL;
    bool call_proc_req_cb = false;

    // Open device
    ret = usbh_devs_open(dev_addr, &dev_hdl);
    if (ret != ESP_OK) {
        ESP_LOGE(EXT_HUB_TAG, "USBH device opening error: %s", esp_err_to_name(ret));
        return ret;
    }

    // Get Configuration Descriptor
    ret = usbh_dev_get_config_desc(dev_hdl, &config_desc);
    if (ret != ESP_OK) {
        ESP_LOGE(EXT_HUB_TAG, "Getting config desc error %s", esp_err_to_name(ret));
        goto exit;
    }

    // Find related Hub Interface descriptor
    device_config_t hub_config = {
        .dev_hdl = dev_hdl,
        .dev_addr = dev_addr,
        .iface_desc = NULL,
        .ep_in_desc = NULL,
    };

    ret = find_first_intf_desc(config_desc, &hub_config);
    if (ret != ESP_OK) {
        goto exit;
    }

    // Create External Hub device
    ret = device_alloc(&hub_config, &hub_dev);
    if (ret != ESP_OK) {
        ESP_LOGE(EXT_HUB_TAG, "[%d] Unable to add device, error %s", dev_addr, esp_err_to_name(ret));
        goto exit;
    }

    hub_dev->single_thread.stage = EXT_HUB_STAGE_GET_HUB_DESCRIPTOR;

    EXT_HUB_ENTER_CRITICAL();
    call_proc_req_cb = _device_set_actions(hub_dev, DEV_ACTION_REQ);
    EXT_HUB_EXIT_CRITICAL();

    if (call_proc_req_cb) {
        p_ext_hub_driver->constant.proc_req_cb(false, p_ext_hub_driver->constant.proc_req_cb_arg);
    }

    return ret;

exit:
    ESP_ERROR_CHECK(usbh_dev_close(dev_hdl));
    return ret;
}

esp_err_t ext_hub_dev_gone(uint8_t dev_addr)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_HUB_EXIT_CRITICAL();
    esp_err_t ret;
    ext_hub_dev_t *ext_hub_dev = NULL;
    bool call_proc_req_cb = false;

    EXT_HUB_CHECK(dev_addr != 0, ESP_ERR_INVALID_ARG);
    // Find device with dev_addr in the devices TAILQ
    // TODO: IDF-10058 Hubs support interface selection (HS)
    // All objects which are linked to dev_addr should be freed, but before IDF-10058, one address - one device
    ret = get_dev_by_addr(dev_addr, &ext_hub_dev);
    if (ret != ESP_OK) {
        ESP_LOGD(EXT_HUB_TAG, "No device with address %d was found", dev_addr);
        goto exit;
    }

    ESP_LOGD(EXT_HUB_TAG, "[%d] Device gone", ext_hub_dev->constant.dev_addr);

    EXT_HUB_ENTER_CRITICAL();
    ext_hub_dev->dynamic.flags.is_gone = 1;
    if (ext_hub_dev->dynamic.flags.waiting_release ||
            ext_hub_dev->dynamic.flags.waiting_children) {
        EXT_HUB_EXIT_CRITICAL();
        // External Hub was already released or waiting children to be freed
        ret = ESP_ERR_INVALID_STATE;
        goto exit;
    }

    if ((ext_hub_dev->single_thread.maxchild == 0) && !ext_hub_dev->dynamic.flags.in_pending_list) {
        // Not in pending list and doesn't have any children release and free right away
        ext_hub_dev->dynamic.flags.waiting_free = 1;
        ext_hub_dev->dynamic.flags.waiting_release = 1;
        call_proc_req_cb = _device_set_actions(ext_hub_dev, DEV_ACTION_RELEASE |
                                               DEV_ACTION_FREE);
    } else {
        // If external Hub was configured and enabled, its EP In is active
        // After detachment the Hub, the driver handles the EP In interrupt and added the device to the list
        // Usually, when device was detached, device object is already in pending list
        // with DEV_ACTION_EP1_DEQUEUE action. Then just add the RELEASE action
        ext_hub_dev->dynamic.flags.waiting_release = 1;
        call_proc_req_cb = _device_set_actions(ext_hub_dev, DEV_ACTION_RELEASE);
    }
    EXT_HUB_EXIT_CRITICAL();

    if (call_proc_req_cb) {
        p_ext_hub_driver->constant.proc_req_cb(false, p_ext_hub_driver->constant.proc_req_cb_arg);
    }

    ret = ESP_OK;
exit:
    return ret;
}

void ext_hub_mark_all_free(void)
{
    bool call_proc_req_cb = false;

    EXT_HUB_ENTER_CRITICAL();
    for (int i = 0; i < 2; i++) {
        ext_hub_dev_t *hub_curr;
        ext_hub_dev_t *hub_next;
        // Go through pending list first
        if (i == 0) {
            hub_curr = TAILQ_FIRST(&p_ext_hub_driver->dynamic.ext_hubs_pending_tailq);
        } else {
            hub_curr = TAILQ_FIRST(&p_ext_hub_driver->dynamic.ext_hubs_tailq);
        }
        while (hub_curr != NULL) {
            hub_next = TAILQ_NEXT(hub_curr, dynamic.tailq_entry);
            hub_curr->dynamic.flags.waiting_release = 1;
            uint32_t action_flags = DEV_ACTION_EP1_FLUSH | DEV_ACTION_EP1_DEQUEUE;
            // If device in the IDLE stage, add the release action
            // otherwise, device will be released when the stage changed to IDLE
            if (hub_curr->single_thread.stage == EXT_HUB_STAGE_IDLE) {
                action_flags |= DEV_ACTION_RELEASE;
            }
            call_proc_req_cb = _device_set_actions(hub_curr, action_flags);
            hub_curr = hub_next;
        }
    }
    EXT_HUB_EXIT_CRITICAL();

    if (call_proc_req_cb) {
        p_ext_hub_driver->constant.proc_req_cb(false, p_ext_hub_driver->constant.proc_req_cb_arg);
    }
}

esp_err_t ext_hub_status_handle_complete(ext_hub_handle_t ext_hub_hdl)
{
    EXT_HUB_CHECK(ext_hub_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_hub_dev_t *ext_hub_dev = (ext_hub_dev_t *)ext_hub_hdl;

    EXT_HUB_CHECK(ext_hub_dev->single_thread.state == EXT_HUB_STATE_CONFIGURED, ESP_ERR_INVALID_STATE);

    return device_enable_int_ep(ext_hub_dev);
}

esp_err_t ext_hub_process(void)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_NOT_ALLOWED);
    // Keep processing until all device's with pending events have been handled
    while (!TAILQ_EMPTY(&p_ext_hub_driver->dynamic.ext_hubs_pending_tailq)) {
        // Move the device back into the idle device list,
        ext_hub_dev_t *ext_hub_dev = TAILQ_FIRST(&p_ext_hub_driver->dynamic.ext_hubs_pending_tailq);
        TAILQ_REMOVE(&p_ext_hub_driver->dynamic.ext_hubs_pending_tailq, ext_hub_dev, dynamic.tailq_entry);
        TAILQ_INSERT_TAIL(&p_ext_hub_driver->dynamic.ext_hubs_tailq, ext_hub_dev, dynamic.tailq_entry);
        // Clear the device's flags
        uint32_t action_flags = ext_hub_dev->dynamic.action_flags;
        ext_hub_dev->dynamic.action_flags = 0;
        ext_hub_dev->dynamic.flags.in_pending_list = 0;

        /* ---------------------------------------------------------------------
        Exit critical section to handle device action flags in their listed order
        --------------------------------------------------------------------- */
        EXT_HUB_EXIT_CRITICAL();
        ESP_LOGD(EXT_HUB_TAG, "[%d] Processing actions 0x%"PRIx32"", ext_hub_dev->constant.dev_addr, action_flags);

        if (action_flags & DEV_ACTION_REQ ||
                action_flags & DEV_ACTION_EP0_COMPLETE) {
            handle_device(ext_hub_dev);
        }
        if (action_flags & DEV_ACTION_EP1_FLUSH) {
            handle_ep1_flush(ext_hub_dev);
        }
        if (action_flags & DEV_ACTION_EP1_DEQUEUE) {
            handle_ep1_dequeue(ext_hub_dev);
        }
        if (action_flags & DEV_ACTION_EP1_CLEAR) {
            handle_ep1_clear(ext_hub_dev);
        }
        if (action_flags & DEV_ACTION_ERROR) {
            handle_error(ext_hub_dev);
        }
        if (action_flags & DEV_ACTION_RELEASE) {
            device_release(ext_hub_dev);
        }
        if (action_flags & DEV_ACTION_FREE) {
            device_free(ext_hub_dev);
        }
        EXT_HUB_ENTER_CRITICAL();
        /* ---------------------------------------------------------------------
        Re-enter critical sections. All device action flags should have been handled.
        --------------------------------------------------------------------- */

    }
    EXT_HUB_EXIT_CRITICAL();

    return ESP_OK;
}

// -----------------------------------------------------------------------------
// --------------------- External Hub - Device related -------------------------
// -----------------------------------------------------------------------------

esp_err_t ext_hub_get_hub_status(ext_hub_handle_t ext_hub_hdl)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_INVALID_STATE);
    EXT_HUB_EXIT_CRITICAL();
    EXT_HUB_CHECK(ext_hub_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_hub_dev_t *ext_hub_dev = (ext_hub_dev_t *)ext_hub_hdl;

    ext_hub_dev->single_thread.stage = EXT_HUB_STAGE_GET_HUB_STATUS;

    EXT_HUB_ENTER_CRITICAL();
    bool call_proc_req_cb = _device_set_actions(ext_hub_dev, DEV_ACTION_REQ);
    EXT_HUB_EXIT_CRITICAL();

    if (call_proc_req_cb) {
        p_ext_hub_driver->constant.proc_req_cb(false, p_ext_hub_driver->constant.proc_req_cb_arg);
    }

    return ESP_OK;
}

esp_err_t ext_hub_get_status(ext_hub_handle_t ext_hub_hdl)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_INVALID_STATE);
    EXT_HUB_EXIT_CRITICAL();

    EXT_HUB_CHECK(ext_hub_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_hub_dev_t *ext_hub_dev = (ext_hub_dev_t *)ext_hub_hdl;

    ext_hub_dev->single_thread.stage = EXT_HUB_STAGE_GET_DEVICE_STATUS;
    EXT_HUB_ENTER_CRITICAL();
    bool call_proc_req_cb = _device_set_actions(ext_hub_dev, DEV_ACTION_REQ);
    EXT_HUB_EXIT_CRITICAL();

    if (call_proc_req_cb) {
        p_ext_hub_driver->constant.proc_req_cb(false, p_ext_hub_driver->constant.proc_req_cb_arg);
    }

    return ESP_OK;
}

// -----------------------------------------------------------------------------
// --------------------- External Hub - Port related ---------------------------
// -----------------------------------------------------------------------------

esp_err_t ext_hub_port_recycle(ext_hub_handle_t ext_hub_hdl, uint8_t port_num)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_HUB_EXIT_CRITICAL();
    EXT_HUB_CHECK(ext_hub_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_hub_dev_t *ext_hub_dev = (ext_hub_dev_t *)ext_hub_hdl;

    uint8_t port_idx = port_num - 1;
    bool release_port = false;
    EXT_HUB_CHECK(port_idx < ext_hub_dev->constant.hub_desc->bNbrPorts, ESP_ERR_INVALID_SIZE);
    EXT_HUB_CHECK(p_ext_hub_driver->constant.port_driver != NULL, ESP_ERR_NOT_SUPPORTED);
    EXT_HUB_CHECK(ext_hub_dev->single_thread.state == EXT_HUB_STATE_CONFIGURED ||
                  ext_hub_dev->single_thread.state == EXT_HUB_STATE_RELEASED, ESP_ERR_INVALID_STATE);

    EXT_HUB_ENTER_CRITICAL();
    if (ext_hub_dev->dynamic.flags.waiting_release ||
            ext_hub_dev->dynamic.flags.waiting_children) {
        release_port = true;
    }
    EXT_HUB_EXIT_CRITICAL();

    esp_err_t ret;
    if (release_port) {
        ret = device_release_port(ext_hub_dev, port_idx);
    } else {
        // Parent still present, recycle the port
        ret = p_ext_hub_driver->constant.port_driver->recycle(ext_hub_dev->constant.ports[port_idx]);
        if (ret != ESP_OK) {
            ESP_LOGE(EXT_HUB_TAG, "[%d:%d] Unable to recycle the port: %s", ext_hub_dev->constant.dev_addr, port_num, esp_err_to_name(ret));
        }
    }
    return ret;
}

esp_err_t ext_hub_port_reset(ext_hub_handle_t ext_hub_hdl, uint8_t port_num)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_HUB_EXIT_CRITICAL();

    EXT_HUB_CHECK(ext_hub_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_hub_dev_t *ext_hub_dev = (ext_hub_dev_t *)ext_hub_hdl;
    uint8_t port_idx = port_num - 1;
    EXT_HUB_CHECK(port_idx < ext_hub_dev->constant.hub_desc->bNbrPorts, ESP_ERR_INVALID_SIZE);
    EXT_HUB_CHECK(p_ext_hub_driver->constant.port_driver != NULL, ESP_ERR_NOT_SUPPORTED);

    return p_ext_hub_driver->constant.port_driver->reset(ext_hub_dev->constant.ports[port_idx]);
}

esp_err_t ext_hub_port_active(ext_hub_handle_t ext_hub_hdl, uint8_t port_num)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_HUB_EXIT_CRITICAL();

    EXT_HUB_CHECK(ext_hub_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_hub_dev_t *ext_hub_dev = (ext_hub_dev_t *)ext_hub_hdl;
    uint8_t port_idx = port_num - 1;
    EXT_HUB_CHECK(port_idx < ext_hub_dev->constant.hub_desc->bNbrPorts, ESP_ERR_INVALID_SIZE);
    EXT_HUB_CHECK(p_ext_hub_driver->constant.port_driver != NULL, ESP_ERR_NOT_SUPPORTED);

    return p_ext_hub_driver->constant.port_driver->active(ext_hub_dev->constant.ports[port_idx]);
}

esp_err_t ext_hub_port_disable(ext_hub_handle_t ext_hub_hdl, uint8_t port_num)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_HUB_EXIT_CRITICAL();

    EXT_HUB_CHECK(ext_hub_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_hub_dev_t *ext_hub_dev = (ext_hub_dev_t *)ext_hub_hdl;
    uint8_t port_idx = port_num - 1;
    EXT_HUB_CHECK(port_idx < ext_hub_dev->constant.hub_desc->bNbrPorts, ESP_ERR_INVALID_SIZE);
    EXT_HUB_CHECK(p_ext_hub_driver->constant.port_driver != NULL, ESP_ERR_NOT_SUPPORTED);
    EXT_HUB_CHECK(ext_hub_dev->single_thread.state == EXT_HUB_STATE_CONFIGURED ||
                  ext_hub_dev->single_thread.state == EXT_HUB_STATE_SUSPENDED, ESP_ERR_INVALID_STATE);

    return p_ext_hub_driver->constant.port_driver->disable(ext_hub_dev->constant.ports[port_idx]);
}

esp_err_t ext_hub_port_get_speed(ext_hub_handle_t ext_hub_hdl, uint8_t port_num, usb_speed_t *speed)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_HUB_EXIT_CRITICAL();

    EXT_HUB_CHECK(ext_hub_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_hub_dev_t *ext_hub_dev = (ext_hub_dev_t *)ext_hub_hdl;
    uint8_t port_idx = port_num - 1;
    EXT_HUB_CHECK(port_idx < ext_hub_dev->constant.hub_desc->bNbrPorts, ESP_ERR_INVALID_SIZE);
    EXT_HUB_CHECK(p_ext_hub_driver->constant.port_driver != NULL, ESP_ERR_NOT_SUPPORTED);
    EXT_HUB_CHECK(ext_hub_dev->single_thread.state == EXT_HUB_STATE_CONFIGURED ||
                  ext_hub_dev->single_thread.state == EXT_HUB_STATE_SUSPENDED, ESP_ERR_INVALID_STATE);

    return p_ext_hub_driver->constant.port_driver->get_speed(ext_hub_dev->constant.ports[port_idx], speed);
}

// -------------------------------------------------------------------------------------------------
// ---------------------------------- USB Chapter 11 -----------------------------------------------
// -------------------------------------------------------------------------------------------------

static esp_err_t ext_hub_control_request(ext_hub_dev_t *ext_hub_dev, uint8_t port1, uint8_t request, uint8_t feature)
{
    esp_err_t ret;
    usb_transfer_t *transfer = &ext_hub_dev->constant.ctrl_urb->transfer;
    switch (request) {
    case USB_B_REQUEST_HUB_GET_PORT_STATUS:
        USB_SETUP_PACKET_INIT_GET_PORT_STATUS((usb_setup_packet_t *)transfer->data_buffer, port1);
        transfer->num_bytes = sizeof(usb_setup_packet_t) + sizeof(usb_port_status_t);
        ext_hub_dev->single_thread.stage = EXT_HUB_STAGE_CHECK_PORT_STATUS;
        break;
    case USB_B_REQUEST_HUB_SET_PORT_FEATURE:
        USB_SETUP_PACKET_INIT_SET_PORT_FEATURE((usb_setup_packet_t *)transfer->data_buffer, port1, feature);
        transfer->num_bytes = sizeof(usb_setup_packet_t);
        ext_hub_dev->single_thread.stage = EXT_HUB_STAGE_CHECK_PORT_FEATURE;
        break;
    case USB_B_REQUEST_HUB_CLEAR_FEATURE:
        USB_SETUP_PACKET_INIT_CLEAR_PORT_FEATURE((usb_setup_packet_t *)transfer->data_buffer, port1, feature);
        transfer->num_bytes = sizeof(usb_setup_packet_t);
        ext_hub_dev->single_thread.stage = EXT_HUB_STAGE_CHECK_PORT_FEATURE;
        break;
    default:
        ESP_LOGE(EXT_HUB_TAG, "Request %X not supported", request);
        return ESP_ERR_NOT_SUPPORTED;
    }

    ret = usbh_dev_submit_ctrl_urb(ext_hub_dev->constant.dev_hdl, ext_hub_dev->constant.ctrl_urb);
    if (ret != ESP_OK) {
        ESP_LOGE(EXT_HUB_TAG, "Request %X, port %d, feature %d: failed to submit ctrl urb: %s",
                 request, port1, feature,
                 esp_err_to_name(ret));
        device_error(ext_hub_dev);
    }
    return ret;
}

// -------------------------------------------------------------------------------------------------
// ---------------------------------- Parent Request -----------------------------------------------
// -------------------------------------------------------------------------------------------------

esp_err_t ext_hub_request(ext_port_hdl_t port_hdl, ext_port_parent_request_data_t *data, void *user_arg)
{
    EXT_HUB_ENTER_CRITICAL();
    EXT_HUB_CHECK_FROM_CRIT(p_ext_hub_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_HUB_EXIT_CRITICAL();
    EXT_HUB_CHECK(data != NULL && port_hdl != NULL, ESP_ERR_INVALID_ARG);

    uint8_t port1;
    ext_hub_handle_t ext_hub_hdl = (ext_hub_handle_t)ext_port_get_context(port_hdl);
    ext_hub_dev_t *ext_hub_dev = (ext_hub_dev_t *)ext_hub_hdl;

    ESP_ERROR_CHECK(ext_port_get_port_num(port_hdl, &port1));
    EXT_HUB_CHECK(port1 != 0 && port1 <= ext_hub_dev->constant.hub_desc->bNbrPorts, ESP_ERR_INVALID_SIZE);
    EXT_HUB_CHECK(ext_hub_dev->single_thread.state == EXT_HUB_STATE_CONFIGURED ||
                  ext_hub_dev->single_thread.state == EXT_HUB_STATE_SUSPENDED, ESP_ERR_INVALID_STATE);

    switch (data->type) {
    case EXT_PORT_PARENT_REQ_CONTROL:
        return ext_hub_control_request(ext_hub_dev, port1, data->control.req, data->control.feature);

    case EXT_PORT_PARENT_REQ_PROC_COMPLETED:
        return ext_hub_status_handle_complete(ext_hub_hdl);

    default:
        break;
    }

    ESP_LOGE(EXT_HUB_TAG, "Request type %d not supported", data->type);
    return ESP_ERR_NOT_SUPPORTED;
}
