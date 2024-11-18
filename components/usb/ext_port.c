/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "usb_private.h"
#include "usb/usb_types_ch9.h"
#include "usb/usb_types_ch11.h"
#include "usbh.h"
#include "ext_port.h"
#include "usb/usb_helpers.h"

// Amount of attempts to reset the port with a device in case of a failure
#if (CONFIG_USB_HOST_EXT_PORT_RESET_ATTEMPTS)
#define EXT_PORT_RESET_ATTEMPTS     CONFIG_USB_HOST_EXT_PORT_RESET_ATTEMPTS
#else
#define EXT_PORT_RESET_ATTEMPTS     1
#endif
// Delay in ms after sending the SetFeature() class specific request
#define EXT_PORT_RESET_RECOVERY_DELAY_MS       CONFIG_USB_HOST_EXT_PORT_RESET_RECOVERY_DELAY_MS
#define EXT_PORT_POWER_ON_CUSTOM_DELAY         CONFIG_USB_HOST_EXT_PORT_CUSTOM_POWER_ON_DELAY_ENABLE
#define EXT_PORT_POWER_ON_CUSTOM_DELAY_MS      CONFIG_USB_HOST_EXT_PORT_CUSTOM_POWER_ON_DELAY_MS

/**
 * @brief External Port driver action flags
 */
typedef enum {
    PORT_ACTION_HANDLE          = (1 << 0),     /**< Port requires handling */
    PORT_ACTION_DISABLE         = (1 << 1),     /**< Disable port */
    PORT_ACTION_RECYCLE         = (1 << 2),     /**< Recycle port */
    PORT_ACTION_RESET           = (1 << 3),     /**< Reset port */
    PORT_ACTION_GET_STATUS      = (1 << 4),     /**< Get status request */
} port_action_t;

/**
 * @brief State of the device, attached to the port
 */
typedef enum {
    PORT_DEV_NOT_PRESENT = 0,
    PORT_DEV_PRESENT
} port_dev_state_t;

/**
 * @brief Object representing a single External Port
 */
struct ext_port_s {
    /**< Single thread members don't require a critical section so long as they are never accessed from multiple threads */
    TAILQ_ENTRY(ext_port_s) tailq_entry;
    union {
        struct {
            uint32_t in_pending_list: 1;    /**< Port in pending list */
            uint32_t status_lock: 1;        /**< Ports' status lock. Parent Hub processes receiving new status */
            uint32_t status_outdated: 1;    /**< Ports' has changed and status is not valid. Get port status is required */
            uint32_t is_gone: 1;            /**< Ports' parent Hub is gone */
            uint32_t has_enum_device: 1;    /**< Port has an enumerated device */
            uint32_t waiting_recycle: 1;    /**< Port is waiting to be recycled */
            uint32_t waiting_free: 1;       /**< Port is waiting to be freed */
            uint32_t reserved25: 25;        /**< Reserved */
        };
        uint32_t val;                       /**< Ports' flags value */
    } flags;                                /**< Ports' flags */
    uint32_t action_flags;                  /**< Ports' action flags */
    usb_hub_port_state_t state;             /**< Ports' state */
    usb_port_status_t status;               /**< Ports' status data */
    port_dev_state_t dev_state;             /**< Ports' device state */
    uint8_t dev_reset_attempts;             /**< Ports' device reset failure */

    struct {
        // Ports' parent info for optimisation and better debug output
        usb_device_handle_t parent_dev_hdl;     /**< Ports' parent device handle */
        uint8_t parent_dev_addr;                /**< Ports' parent device bus address */
        // Port related constant members
        ext_hub_handle_t ext_hub_hdl;           /**< Ports' parent External Hub handle */
        uint8_t port_num;                       /**< Ports' parent External Hub Port number */
        int power_on_delay_ms;                  /**< Ports' Power on time to Power Good, ms */
    } constant;                                 /**< Constant members. Do not change after installation thus do not require a critical section or mutex */
};

/**
 * @brief Definition of the type for the struct, representing a single External Port
 */
typedef struct ext_port_s ext_port_t;

/**
 * @brief Object representing the External Port Driver
 */
typedef struct {
    struct {
        TAILQ_HEAD(ext_ports, ext_port_s)       pending_tailq;           /**< External Ports require handling */
    } single_thread;                            /**< Single thread members don't require a critical section so long as they are never accessed from multiple threads */

    struct {
        ext_port_cb_t proc_req_cb;              /**< External Port process callback */
        void *proc_req_cb_arg;                  /**< External Port process callback argument */
        ext_port_event_cb_t event_cb;           /**< External Port event callback */
        void *event_cb_arg;                     /**< External Port event callback argument */
    } constant;                                 /**< Constant members. Do not change after installation thus do not require a critical section or mutex */
} ext_port_driver_t;

static ext_port_driver_t *p_ext_port_driver = NULL;

const char *EXT_PORT_TAG = "EXT_PORT";

// -----------------------------------------------------------------------------
// --------------------------- Helpers & Macros --------------------------------
// -----------------------------------------------------------------------------

#define EXT_PORT_CHECK(cond, ret_val) ({              \
            if (!(cond)) {                                  \
                return (ret_val);                           \
            }                                               \
})

/**
 * @brief Port connection status
 *
 * @param[in] ext_port Port object
 * @return
 *    - true    Port has a connection
 *    - false   Port doesn't have a connection
 */
static inline bool port_has_connection(ext_port_t *ext_port)
{
    return ext_port->status.wPortStatus.PORT_CONNECTION ? true : false;
}

/**
 * @brief Port power status
 *
 * @param[in] ext_port Port object
 * @return
 *    - true    Port is powered
 *    - false   Port in not powered
 */
static inline bool port_is_powered(ext_port_t *ext_port)
{
    return ext_port->status.wPortStatus.PORT_POWER ? true : false;
}

/**
 * @brief Port reset status
 *
 * @param[in] ext_port Port object
 * @return
 *    - true    Port is in reset
 *    - false   Port is not in reset
 */
static inline bool port_is_in_reset(ext_port_t *ext_port)
{
    return ext_port->status.wPortStatus.PORT_RESET ? true : false;
}

/**
 * @brief Port enable status
 *
 * @param[in] ext_port Port object
 * @return
 *    - true    Port is enabled
 *    - false   Port is disabled
 */
static inline bool port_is_enabled(ext_port_t *ext_port)
{
    return ext_port->status.wPortStatus.PORT_ENABLE ? true : false;
}

/**
 * @brief Port connection status changed
 *
 * @param[in] ext_port Port object
 * @return
 *    - true    Port connection changed
 *    - false   Port connection not changed
 */
static inline bool port_has_changed_connection(ext_port_t * ext_port)
{
    return ext_port->status.wPortChange.C_PORT_CONNECTION ? true : false;
}

/**
 * @brief Port enabled status changed
 *
 * @param[in] ext_port Port object
 * @return
 *    - true    Port has error state and was disabled
 *    - false   Port operates normally
 */
static inline bool port_has_changed_from_enable(ext_port_t *ext_port)
{
    return ext_port->status.wPortChange.C_PORT_ENABLE ? true : false;
}

/**
 * @brief Port reset status changed
 *
 * @param[in] ext_port Port object
 * @return
 *    - true    Port has finished the reset
 *    - false   Port hasn't finished the reset
 */
static inline bool port_has_finished_reset(ext_port_t *ext_port)
{
    return ext_port->status.wPortChange.C_PORT_RESET ? true : false;
}

// -----------------------------------------------------------------------------
// ------------------------ Parent Hub related logic ---------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Request the port status for the port object
 *
 * @note This call uses the External Hub Driver API
 *
 * @param[in] ext_port Port object
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Hub Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    The parent hub handle couldn't be NULL
 *    - ESP_ERR_INVALID_SIZE:   The port number should be in a range: [1, .. , bNbrPort]
 *    - ESP_ERR_INVALID_STATE:  The parent hub device wasn't configured
 *    - ESP_OK:                 Status has been requested
 */
static esp_err_t port_request_status(ext_port_t* ext_port)
{
    esp_err_t ret = ext_hub_get_port_status(ext_port->constant.ext_hub_hdl, ext_port->constant.port_num);
    if (ret != ESP_OK) {
        return ret;
    }
    // Port is requesting status, lock the status
    ext_port->flags.status_lock = 1;
    return ret;
}

/**
 * @brief Sets the feature to the port
 *
 * @note This call uses the External Hub Driver API
 *
 * @param[in] ext_port  Port object
 * @param[in] feature   Port feature to set
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Hub Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    The parent hub handle couldn't be NULL
 *    - ESP_ERR_INVALID_SIZE:   The port number should be in a range: [1, .. , bNbrPort]
 *    - ESP_ERR_INVALID_STATE:  The parent hub device wasn't configured
 *    - ESP_OK:                 SetPortFeature() has been requested
 */
static esp_err_t port_set_feature(ext_port_t *ext_port, const usb_hub_port_feature_t feature)
{
    esp_err_t ret = ext_hub_set_port_feature(ext_port->constant.ext_hub_hdl,
                                             ext_port->constant.port_num,
                                             feature);
    if (ret != ESP_OK) {
        return ret;
    }
    // Every set feature requires status update
    ext_port->flags.status_outdated = 1;
    switch (feature) {
    case USB_FEATURE_PORT_POWER:
        // PowerOn to PowerGood delay for port
        vTaskDelay(pdMS_TO_TICKS(ext_port->constant.power_on_delay_ms));
        break;
    case USB_FEATURE_PORT_RESET:
        // Port has reset, give the port some time to recover
        vTaskDelay(pdMS_TO_TICKS(EXT_PORT_RESET_RECOVERY_DELAY_MS));
        break;
    default:
        break;
    }
    return ret;
}

/**
 * @brief Clears the feature to the port
 *
 * @note This call uses the External Hub Driver API
 *
 * @param[in] ext_port  Port object
 * @param[in] feature   Port feature to set
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Hub Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    The parent hub handle couldn't be NULL
 *    - ESP_ERR_INVALID_SIZE:   The port number should be in a range: [1, .. , bNbrPort]
 *    - ESP_ERR_INVALID_STATE:  The parent hub device wasn't configured
 *    - ESP_OK:                 ClearPortFeature() has been requested
 */
static esp_err_t port_clear_feature(ext_port_t *ext_port, const usb_hub_port_feature_t feature)
{
    esp_err_t ret = ext_hub_clear_port_feature(ext_port->constant.ext_hub_hdl,
                                               ext_port->constant.port_num,
                                               feature);
    if (ret != ESP_OK) {
        return ret;
    }
    // Every clearing feature requires status update
    ext_port->flags.status_outdated = 1;
    return ret;
}

// -----------------------------------------------------------------------------
// ---------------------------- Internal logic ---------------------------------
// -----------------------------------------------------------------------------
/**
 * @brief Port set actions
 *
 * @note
 *    - The External Port Driver is a single threaded driver
 *    - All calls are made from USB Host related task
 *    - Does not need critical section
 *    - Always requires processing
 *
 * @param[in] ext_port      Port object
 * @param[in] action_flags  Action flags (could not be 0)
 */
static void port_set_actions(ext_port_t *ext_port, uint32_t action_flags)
{
    assert(action_flags != 0); // Sanity check

    // Check if port is not in the pending list
    if (!ext_port->flags.in_pending_list) {
        // Add port to pending list
        ext_port->flags.in_pending_list = 1;
        TAILQ_INSERT_TAIL(&p_ext_port_driver->single_thread.pending_tailq, ext_port, tailq_entry);
    }

    ext_port->action_flags |= action_flags;
    p_ext_port_driver->constant.proc_req_cb(p_ext_port_driver->constant.proc_req_cb_arg);
}

/**
 * @brief Stops the handling and remove the port from the pending list
 *
 * When the ports parent hub device has been removed, the port could be freed immediately
 * if it doesn't have an inserted device.
 * For this purpose port object should be removed from the pending
 * list and the action flags should be dropped.
 *
 * @note
 *    - The External Port Driver is a single threaded driver
 *    - All calls are made from USB Host related task
 *    - Does not need critical section
 *    - Does not require processing
 *
 * @param[in] ext_port      Port object
 */
static void port_stop_handling(ext_port_t *ext_port)
{
    // Check if port is in the pending list
    if (ext_port->flags.in_pending_list) {
        // Add port to pending list
        ext_port->flags.in_pending_list = 0;
        TAILQ_REMOVE(&p_ext_port_driver->single_thread.pending_tailq, ext_port, tailq_entry);
    }
    if (ext_port->action_flags) {
        // Port should be freed but has actions
        ESP_LOGD(EXT_PORT_TAG, "[%d:%d] Port stop handling. Dropped actions 0x%"PRIx32"",
                 ext_port->constant.parent_dev_addr,
                 ext_port->constant.port_num,
                 ext_port->action_flags);
    }
    ext_port->action_flags = 0;
}

/**
 * @brief Port event propagation
 *
 * Propagating the following events to the Hub Driver:
 * - EXT_PORT_CONNECTED
 * - EXT_PORT_RESET_COMPLETED
 * - EXT_PORT_DISCONNECTED
 *
 * Filling all the event data parameters, according to the event.
 *
 * @param[in] ext_port  Port object
 * @param[in] event     Port event
 */
static void port_event(ext_port_t *ext_port, ext_port_event_t event)
{
    ext_port_event_data_t event_data = {
        .event = event,
    };
    switch (event) {
    case EXT_PORT_CONNECTED:
        event_data.connected.ext_hub_hdl = ext_port->constant.ext_hub_hdl;
        event_data.connected.parent_dev_hdl = ext_port->constant.parent_dev_hdl;
        event_data.connected.parent_port_num = ext_port->constant.port_num;
        break;
    case EXT_PORT_RESET_COMPLETED:
        event_data.reset_completed.parent_dev_hdl = ext_port->constant.parent_dev_hdl;
        event_data.reset_completed.parent_port_num = ext_port->constant.port_num;
        break;
    case EXT_PORT_DISCONNECTED:
        event_data.disconnected.parent_dev_hdl = ext_port->constant.parent_dev_hdl;
        event_data.disconnected.parent_port_num = ext_port->constant.port_num;
        break;
    default:
        // Should never occur
        abort();
        break;
    }

    p_ext_port_driver->constant.event_cb(&event_data, p_ext_port_driver->constant.event_cb_arg);
}

/**
 * @brief Returns first port from the pending list
 *
 * @return
 *    - Port object pointer. NULL if no ports in pending list.
 */
static ext_port_t *get_port_from_pending_list(void)
{
    ext_port_t *ext_port = NULL;
    if (!TAILQ_EMPTY(&p_ext_port_driver->single_thread.pending_tailq)) {
        ext_port = TAILQ_FIRST(&p_ext_port_driver->single_thread.pending_tailq);
    }
    return ext_port;
}

/**
 * @brief Allocates port object
 *
 * Allocates new por object with following parameters:
 * - Port state:         USB_PORT_STATE_NOT_CONFIGURED
 * - Port device status: PORT_DEV_NOT_PRESENT
 *
 * @param[in] ext_hub_hdl       Ports' parent hub handle
 * @param[in] parent_dev_hdl    Ports' parent device handle
 * @param[in] parent_port_num   Ports' parent port number
 * @param[in] port_delay_ms     Ports' Power on time to Power Good, ms
 * @param[out] port_obj         Port objects' pointer
 * @return
 *    - ESP_ERR_INVALID_ARG:    Unable to allocate the port object: parent hub handle and parent device handle must be not NULL
 *    - ESP_ERR_NO_MEM:         Unable to allocate the port object: no memory
 *    - ESP_ERR_NOT_FINISHED:   Unable to allocate the port object: parent device is not available
 *    - ESP_OK:                 Port object created successfully
 */
static esp_err_t port_alloc(ext_hub_handle_t ext_hub_hdl, usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num, uint16_t port_delay_ms, ext_port_t **port_obj)
{
    uint8_t parent_dev_addr = 0;
    EXT_PORT_CHECK(ext_hub_hdl != NULL && parent_dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    // This is the one exception from the requirement to use only the Ext Hub Driver API.
    // TODO: IDF-10023 Move responsibility of parent-child tree building to Hub Driver instead of USBH
    EXT_PORT_CHECK(usbh_dev_get_addr(parent_dev_hdl, &parent_dev_addr) == ESP_OK, ESP_ERR_NOT_FINISHED);

    ext_port_t *ext_port = heap_caps_calloc(1, sizeof(ext_port_t), MALLOC_CAP_DEFAULT);
    if (ext_port == NULL) {
        return ESP_ERR_NO_MEM;
    }

    ext_port->constant.parent_dev_hdl = parent_dev_hdl;
    ext_port->constant.parent_dev_addr = parent_dev_addr;
    ext_port->constant.ext_hub_hdl = ext_hub_hdl;
    ext_port->constant.port_num = parent_port_num;
#if (EXT_PORT_POWER_ON_CUSTOM_DELAY)
    ext_port->constant.power_on_delay_ms = EXT_PORT_POWER_ON_CUSTOM_DELAY_MS;
#else
    // We don't need any additional delay in case port_delay_ms == 0, because this usually means
    // that parent Hub device has no power switches
    ext_port->constant.power_on_delay_ms = port_delay_ms;
#endif // EXT_PORT_POWER_ON_CUSTOM_DELAY

    ext_port->state = USB_PORT_STATE_NOT_CONFIGURED;
    ext_port->dev_state = PORT_DEV_NOT_PRESENT;

    ESP_LOGD(EXT_PORT_TAG, "[%d:%d] Port has been added (PwrOn2PwrGood=%d ms)",
             ext_port->constant.parent_dev_addr,
             ext_port->constant.port_num,
             ext_port->constant.power_on_delay_ms);

    *port_obj = ext_port;
    return ESP_OK;
}

/**
 * @brief Port object handling complete
 *
 * @note This is the final stage of port object handling.
 * After port being handled, the port object:
 * - is removed from the pending list
 * - is freed, is the port gone or port object waiting to be freed
 *
 * The final stage is parent notification about port object handling completion to enable the EP IN
 *
 * @param[in] ext_port  Port object
 * @return
 *    - ESP_OK:         Port object handle completed
 */
static esp_err_t handle_complete(ext_port_t *ext_port)
{
    bool all_ports_were_handled = true;
    bool has_pending_ports = false;

    ESP_LOGD(EXT_PORT_TAG, "[%d:%d] Port is %s",
             ext_port->constant.parent_dev_addr,
             ext_port->constant.port_num,
             ext_port->flags.is_gone ? "gone" :
             (ext_port->dev_state == PORT_DEV_PRESENT) ? "active" : "idle");

    assert(ext_port->flags.is_gone == 0);           // Handle complete could be called only on port that is still available
    assert(ext_port->flags.waiting_free == 0);      // Handle completion could be called only on port that should be freed
    assert(ext_port->action_flags == 0);            // Port should not have any further actions
    assert(ext_port->flags.waiting_recycle == 0);   // Port should not await to be recycled
    assert(ext_port->flags.in_pending_list == 1);   // Port should be in pending list

    // Remove port from it
    port_stop_handling(ext_port);

    // Verify, that list has more ports in pending list
    if (!TAILQ_EMPTY(&p_ext_port_driver->single_thread.pending_tailq)) {
        has_pending_ports = true;
    }

    // Port handling complete
#if ENABLE_MULTIPLE_HUBS
    // When multiply Hubs are attached, we can have ports in pending list, but for another parent
    ext_port_t *port = NULL;
    TAILQ_FOREACH(port, &p_ext_port_driver->single_thread.pending_tailq, tailq_entry) {
        if (port->constant.ext_hub_hdl == ext_port->constant.ext_hub_hdl) {
            // Port with same parent has been found
            all_ports_were_handled = false;
            break;
        }
    }
#else
    // When only one hub is supported - all ports have the same parent
    // all_port_were_handled should be triggered if no more ports in pending list
    all_ports_were_handled = !has_pending_ports;
#endif //

    if (all_ports_were_handled) {
        // Notify parent to enable Interrupt EP
        ext_hub_status_handle_complete(ext_port->constant.ext_hub_hdl);
    }

    if (has_pending_ports) {
        // Re-trigger the processing if there are more ports in pending list
        p_ext_port_driver->constant.proc_req_cb(p_ext_port_driver->constant.proc_req_cb_arg);
    }

    return ESP_OK;
}

/**
 * @brief Handles port status
 *
 * @param[in] ext_port  Port object
 * @return
 *    - true:       Port has a status, that requires processing
 *    - false:      Port has a status, that doesn't require processing
 */
static bool handle_port_status(ext_port_t *ext_port)
{
    bool need_processing = false;
    if (port_is_in_reset(ext_port)) {
        ESP_LOGD(EXT_PORT_TAG, "[%d:%d] Port still in reset, wait and repeat get status...",
                 ext_port->constant.parent_dev_addr,
                 ext_port->constant.port_num);
        port_request_status(ext_port);
        need_processing = true;
    }

    return need_processing;
}

/**
 * @brief Handles port connection
 *
 * @param[in] ext_port  Port object
 */
static void handle_port_connection(ext_port_t *ext_port)
{
    bool has_device = false;

    switch (ext_port->state) {
    case USB_PORT_STATE_POWERED_OFF:
        if (!port_is_powered(ext_port)) {
            ext_port->state = USB_PORT_STATE_DISCONNECTED;
            port_set_feature(ext_port, USB_FEATURE_PORT_POWER);
        }
        break;
    case USB_PORT_STATE_DISCONNECTED:
    case USB_PORT_STATE_DISABLED:
        if (port_has_connection(ext_port)) {
            if (ext_port->dev_state == PORT_DEV_PRESENT) {
                ESP_LOGE(EXT_PORT_TAG, "[%d:%d] Mismatch port state and device status",
                         ext_port->constant.parent_dev_addr,
                         ext_port->constant.port_num);
                has_device = true;
            } else {
                // New device connected, flush reset attempts
                ext_port->dev_reset_attempts = 0;
                ext_port->state = USB_PORT_STATE_RESETTING;
                // New device has not been enumerated yet, reset the flag
                ext_port->flags.has_enum_device = 0;
            }
        }
        break;
    case USB_PORT_STATE_RESETTING:
        if (!port_has_connection(ext_port)) {
            if (ext_port->dev_state == PORT_DEV_PRESENT) {
                ESP_LOGE(EXT_PORT_TAG, "[%d:%d] Failed to issue downstream port reset",
                         ext_port->constant.parent_dev_addr,
                         ext_port->constant.port_num);
                has_device = true;
            } else {
                ext_port->state = USB_PORT_STATE_DISCONNECTED;
            }
        }
        break;
    case USB_PORT_STATE_ENABLED:
        // TODO: IDF-10071 Port debounce mechanism
        if (ext_port->dev_state == PORT_DEV_PRESENT) {
            ext_port->flags.waiting_recycle = 1;
        }
        break;

    default:
        // Should never occur
        abort();
        break;
    }

    if (has_device) {
        ext_port->flags.waiting_recycle = 1;
        ext_port->dev_state = PORT_DEV_NOT_PRESENT;
        port_event(ext_port, EXT_PORT_DISCONNECTED);
    }
}

/**
 * @brief Handles port changes
 *
 * @param[in] ext_port  Port object
 * @return
 *    - true:   Port has a change, that requires processing
 *    - false:  Port has no changes and doesn't require processing
 */
static bool handle_port_changes(ext_port_t *ext_port)
{
    bool need_processing = false;
    if (port_has_changed_connection(ext_port)) {
        handle_port_connection(ext_port);
        port_clear_feature(ext_port, USB_FEATURE_C_PORT_CONNECTION);
        need_processing = true;
    } else if (port_has_changed_from_enable(ext_port)) {
        // For more information, refer to section 11.8.1 Port Error of usb_2.0 specification
        ESP_LOGE(EXT_PORT_TAG, "[%d:%d] Error on port (state=%d, dev=%d)",
                 ext_port->constant.parent_dev_addr,
                 ext_port->constant.port_num,
                 ext_port->state,
                 ext_port->dev_state == PORT_DEV_PRESENT);
        port_clear_feature(ext_port, USB_FEATURE_C_PORT_ENABLE);
        need_processing = true;
    } else if (port_has_finished_reset(ext_port)) {
        if (port_has_connection(ext_port)) {
            ext_port->state = USB_PORT_STATE_ENABLED;
        }
        port_clear_feature(ext_port, USB_FEATURE_C_PORT_RESET);
        need_processing = true;
    }

    return need_processing;
}

/**
 * @brief Handles port state
 *
 * @param[in] ext_port  Port object
 */
static void handle_port_state(ext_port_t *ext_port)
{
    bool need_handling = false;
    usb_hub_port_state_t curr_state = ext_port->state;
    usb_hub_port_state_t new_state = ext_port->state;

    switch (curr_state) {
    case USB_PORT_STATE_NOT_CONFIGURED:
        new_state = USB_PORT_STATE_POWERED_OFF;
        port_request_status(ext_port);
        need_handling = true;
        break;
    case USB_PORT_STATE_POWERED_OFF:
        // Port power state depends on the wHubCharacteristics.power_switching
        new_state = USB_PORT_STATE_DISCONNECTED;
        port_set_feature(ext_port, USB_FEATURE_PORT_POWER);
        need_handling = true;
        break;
    case USB_PORT_STATE_DISCONNECTED:
        if (port_has_connection(ext_port)) {
            if (ext_port->dev_reset_attempts < EXT_PORT_RESET_ATTEMPTS) {
                // Available, it EXT_PORT_RESET_ATTEMPTS > 1
                ext_port->dev_reset_attempts++;
                port_set_feature(ext_port, USB_FEATURE_PORT_RESET);
                need_handling = true;
            } else {
                ESP_LOGE(EXT_PORT_TAG, "[%d:%d] Unable to reset the device",
                         ext_port->constant.parent_dev_addr,
                         ext_port->constant.port_num);
            }
        }
        break;
    case USB_PORT_STATE_DISABLED:
        if (port_has_connection(ext_port)) {
            // This logic does depend on the moment, when we propagate the EXT_PORT_DISCONNECTED event
            // during the port disable.
            if (!ext_port->flags.has_enum_device && ext_port->flags.waiting_recycle) {
                // Port was disabled before enumeration, so the USBH device object was not created.
                // Clean the recycle flag and complete port handling with device attached.
                ext_port->flags.waiting_recycle = 0;
            }
        }
        break;
    case USB_PORT_STATE_RESETTING:
        if (port_has_connection(ext_port)) {
            // Port in resetting state and has connection
            if (ext_port->dev_state == PORT_DEV_NOT_PRESENT) {
                assert(ext_port->dev_reset_attempts == 0); // First reset, attempts always should be 0
                ext_port->dev_reset_attempts++;
                port_set_feature(ext_port, USB_FEATURE_PORT_RESET);
            } else {
                assert(port_is_enabled(ext_port)); // Port should be enabled
                port_event(ext_port, EXT_PORT_RESET_COMPLETED);
                new_state = USB_PORT_STATE_ENABLED;
            }
            need_handling = true;
        } else {
            // Port in resetting state and doesn't have connection
            // Error case, could be, when device was removed during port reset
            ESP_LOGE(EXT_PORT_TAG, "[%d:%d] Device gone during port reset, recover port",
                     ext_port->constant.parent_dev_addr,
                     ext_port->constant.port_num);
            new_state = USB_PORT_STATE_DISCONNECTED;
        }
        break;
    case USB_PORT_STATE_ENABLED:
        if (port_is_enabled(ext_port)) {
            if (port_has_connection(ext_port)) {
                if (ext_port->dev_state == PORT_DEV_NOT_PRESENT) {
                    usb_speed_t dev_speed = USB_SPEED_LOW;
                    if (ext_port->status.wPortStatus.PORT_LOW_SPEED == 0) {
                        dev_speed = ext_port->status.wPortStatus.PORT_HIGH_SPEED
                                    ? USB_SPEED_HIGH
                                    : USB_SPEED_FULL;
                    }
                    ESP_LOGD(EXT_PORT_TAG, "Device speed %s", (char *[]) {
                        "Low", "Full", "High"
                    }[dev_speed]);
                    ext_port->dev_state = PORT_DEV_PRESENT;
                    port_event(ext_port, EXT_PORT_CONNECTED);
                } else {
                    // Port enabled, device present, reset completed
                    ext_port->dev_reset_attempts = 0;
                    port_event(ext_port, EXT_PORT_RESET_COMPLETED);
                }
                need_handling = true;
            } else {
                // TODO: IDF-10071 Port debounce mechanism
                ESP_LOGE(EXT_PORT_TAG, "[%d:%d] Enabled, but doesn't have connection",
                         ext_port->constant.parent_dev_addr,
                         ext_port->constant.port_num);
            }
        } else {
            // If port was enabled, there should be an active device
            if (ext_port->dev_state == PORT_DEV_PRESENT) {
                ext_port->dev_state = PORT_DEV_NOT_PRESENT;
                port_event(ext_port, EXT_PORT_DISCONNECTED);
                ext_port->flags.waiting_recycle = 1;
            } else {
                // Error state
                ESP_LOGE(EXT_PORT_TAG, "[%d:%d] Enabled, but doesn't have a device",
                         ext_port->constant.parent_dev_addr,
                         ext_port->constant.port_num);
                new_state = USB_PORT_STATE_DISCONNECTED;
            }
        }
        break;
    default:
        // Should never occur
        abort();
        break;
    }

    if (curr_state != new_state) {
        ESP_LOGD(EXT_PORT_TAG, "New state: %d", new_state);
        ext_port->state = new_state;
    }

    if (!ext_port->flags.waiting_recycle && !need_handling) {
        handle_complete(ext_port);
    }
}

/**
 * @brief Port object handling action
 *
 * @note Handles the port in the following order:
 * - Port changes (C_CONNECTION, C_ENABLE, C_RESET)
 * - Port status (Port still in RESET state)
 * - Port state
 *
 * When ports' handling has been completed, removes the port object from the pending list.
 *
 * @param[in] ext_port  Port object
 */
static void handle_port(ext_port_t *ext_port)
{
    ESP_LOGD(EXT_PORT_TAG, "[%d:%d] change=0x%04x, status=0x%04x, state=%d",
             ext_port->constant.parent_dev_addr,
             ext_port->constant.port_num,
             ext_port->status.wPortChange.val,
             ext_port->status.wPortStatus.val,
             ext_port->state);

    if (handle_port_changes(ext_port)) {
        return;
    } else if (handle_port_status(ext_port)) {
        return;
    }
    handle_port_state(ext_port);
}

/**
 * @brief Port object handling recycle action
 *
 * @note Port should undergo a recycle procedure when:
 * - Device has been detached and freed by all clients (port is present)
 * - Parent device been detached (port is gone)
 * - Port has run an error during the handling
 *
 * @param[in] ext_port  Port object
 */
static void handle_recycle(ext_port_t *ext_port)
{
    assert(ext_port->flags.waiting_recycle == 1);

    ESP_LOGD(EXT_PORT_TAG, "[%d:%d] Port recycle (state=%d, dev=%d)",
             ext_port->constant.parent_dev_addr,
             ext_port->constant.port_num,
             ext_port->state,
             ext_port->dev_state);

    ext_port->flags.waiting_recycle = 0;

    if (ext_port->flags.status_lock) {
        // Port is awaiting the status
        return;
    }

    // Port should not have any changes
    assert(ext_port->status.wPortChange.val == 0);
    switch (ext_port->state) {
    case USB_PORT_STATE_DISABLED:
        // We don't need to do anything, as port will be handled after completing USB_FEATURE_PORT_ENABLE
        if (ext_port->flags.is_gone) {
            handle_complete(ext_port);
        }
        break;
    default:
        ext_port->state = USB_PORT_STATE_DISCONNECTED;
        if (ext_port->flags.is_gone) {
            handle_complete(ext_port);
        } else {
            handle_port(ext_port);
        }
        break;
    }
}

/**
 * @brief Port object handling disable action
 *
 * @param[in] ext_port  Port object
 */
static void handle_disable(ext_port_t *ext_port)
{
    ESP_LOGD(EXT_PORT_TAG, "[%d:%d] Disable (state=%d, dev=%d)",
             ext_port->constant.parent_dev_addr,
             ext_port->constant.port_num,
             ext_port->state,
             ext_port->dev_state);

    assert(ext_port->state != USB_PORT_STATE_POWERED_OFF);
    assert(ext_port->state != USB_PORT_STATE_DISCONNECTED);
    assert(ext_port->state != USB_PORT_STATE_NOT_CONFIGURED);

    if (ext_port->state == USB_PORT_STATE_ENABLED) {
        if (port_has_connection(ext_port)) {
            ESP_LOGE(EXT_PORT_TAG, "[%d:%d] Port disabled, reset attempts=%d",
                     ext_port->constant.parent_dev_addr,
                     ext_port->constant.port_num,
                     ext_port->dev_reset_attempts);

            // Do not try to reset port anymore
            ext_port->dev_reset_attempts = EXT_PORT_RESET_ATTEMPTS;

            if (ext_port->dev_state == PORT_DEV_PRESENT) {
                ext_port->dev_state = PORT_DEV_NOT_PRESENT;
                // Propagate device disconnection if device present
                port_event(ext_port, EXT_PORT_DISCONNECTED);
                ext_port->flags.waiting_recycle = 1;
            }
        }
    }

    ext_port->state = USB_PORT_STATE_DISABLED;

    if (!ext_port->flags.is_gone) {
        // Port not gone, disable port
        port_clear_feature(ext_port, USB_FEATURE_PORT_ENABLE);
    }
}

// -----------------------------------------------------------------------------
// ------------------------ External Port API ----------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Creates new port object
 *
 * @note This function should only be called from the External Hub Driver
 *
 * @param[in] port_cfg              Port configuration
 * @param[out] port_hdl             Port object handle
 * @return
 *    - ESP_ERR_INVALID_STATE:      The External Port Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:        Unable to create the port, arguments couldn't be NULL
 *    - ESP_ERR_NO_MEM:             Unable to allocate the port object: no memory
 *    - ESP_ERR_NOT_FINISHED:       Unable to allocate the port object: parent device is not available
 *    - ESP_OK:                     Port has been created and added to the pending list
 */
static esp_err_t port_new(void *port_cfg, void **port_hdl)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_INVALID_STATE);
    EXT_PORT_CHECK(port_cfg != NULL && port_hdl != NULL, ESP_ERR_INVALID_ARG);

    ext_port_t *port = NULL;
    ext_port_config_t *config = (ext_port_config_t *)port_cfg;
    esp_err_t ret = port_alloc(config->ext_hub_hdl,
                               config->parent_dev_hdl,
                               config->parent_port_num,
                               config->port_power_delay_ms,
                               &port);

    if (ret != ESP_OK) {
        *port_hdl = NULL;
        goto exit;
    }

    port_set_actions(port, PORT_ACTION_HANDLE);
    *port_hdl = (ext_port_hdl_t) port;
exit:
    return ret;
}

/**
 * @brief Indicates to the External Port driver that a device's port can be recycled
 *
 * The device connected to the port has been freed. The External Port driver can now
 * recycle the port.
 *
 * @note This function should only be called from the External Hub Driver
 *
 * @param[in] port_hdl          Port object handle
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Port Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    Unable to recycle the port, port handle couldn't be NULL
 *    - ESP_OK:
 */
static esp_err_t port_recycle(void *port_hdl)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_PORT_CHECK(port_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_port_t *ext_port = (ext_port_t *) port_hdl;

    ESP_LOGD(EXT_PORT_TAG, "Port %d request recycle, state=%d", ext_port->constant.port_num, ext_port->state);

    port_set_actions(ext_port, PORT_ACTION_RECYCLE);
    return ESP_OK;
}

/**
 * @brief Indicate to the External Port driver that reset of a device's port is required
 *
 * @note This function should only be called from the External Hub Driver
 *
 * @param[in] port_hdl          Port object handle
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Port Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    Port handle couldn't be NULL
 *    - ESP_ERR_INVALID_STATE:  Port doesn't have a connection or not enabled
 *    - ESP_OK:                 Port reset requested
 */
static esp_err_t port_reset(void *port_hdl)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_PORT_CHECK(port_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_port_t *ext_port = (ext_port_t *) port_hdl;

    EXT_PORT_CHECK(port_has_connection(ext_port), ESP_ERR_INVALID_STATE);

    ESP_LOGD(EXT_PORT_TAG, "[%d:%d] Port reset request",
             ext_port->constant.parent_dev_addr,
             ext_port->constant.port_num);

    // Reset can be triggered only when port is enabled
    EXT_PORT_CHECK(ext_port->state == USB_PORT_STATE_ENABLED, ESP_ERR_INVALID_STATE);

    port_set_actions(ext_port, PORT_ACTION_RESET);
    return ESP_OK;
}

/**
 * @brief Get the speed of the External Port
 *
 * The speed of the port is determined by the speed of the device connected to it.
 *
 * @note This function is only when a device connected to the port and has been reset
 * @note This function should only be called from the External Hub Driver
 *
 * @param[in] port_hdl      Port object handle
 * @param[out] speed        Speed of the port
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Port Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    The port handle can't be NULL, speed pointer can't be NULL
 *    - ESP_ERR_INVALID_STATE:  No valid device connected to the port
 *    - ESP_OK:                 Device speed obtained
 */
esp_err_t port_get_speed(void *port_hdl, usb_speed_t *speed)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_PORT_CHECK(port_hdl != NULL && speed != NULL, ESP_ERR_INVALID_ARG);
    ext_port_t *ext_port = (ext_port_t *) port_hdl;

    usb_speed_t dev_speed = USB_SPEED_LOW;
    EXT_PORT_CHECK(port_has_connection(ext_port), ESP_ERR_INVALID_STATE);
    // Full-speed or High-speed device attached to this port (determined by bit PORT_HIGH_SPEED).
    if (ext_port->status.wPortStatus.PORT_LOW_SPEED == 0) {
        dev_speed = ext_port->status.wPortStatus.PORT_HIGH_SPEED
                    ? USB_SPEED_HIGH
                    : USB_SPEED_FULL;
    }
    // Otherwise, Low-speed
    *speed = dev_speed;
    return ESP_OK;
}

/**
 * @brief Indicate to the External Port Driver that port has an active device
 *
 * The device:
 * - has been enumerated
 * - still present
 * - is in configured state
 *
 * @note This function should only be called from the External Hub Driver
 *
 * @param[in] port_hdl          Port object handle
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Port Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    The port handle can't be NULL
 *    - ESP_OK:                 Port has a device and the device completed enumeration process
 */
static esp_err_t port_active(void *port_hdl)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_PORT_CHECK(port_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_port_t *ext_port = (ext_port_t *) port_hdl;

    ESP_LOGD(EXT_PORT_TAG, "[%d:%d] Port has an enumerated device",
             ext_port->constant.parent_dev_addr,
             ext_port->constant.port_num);

    ext_port->flags.has_enum_device = 1;

    return handle_complete(ext_port);
}

/**
 * @brief Indicate to the External Port Driver that port should be disabled
 *
 * @note This function should only be called from the External Hub Driver
 *
 * @param[in] port_hdl      Port object handle
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Port Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    The port handle can't be NULL
 *    - ESP_ERR_INVALID_STATE:  The port is not enabled
 *    - ESP_OK:                 Port disable requested
 */
static esp_err_t port_disable(void *port_hdl)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_PORT_CHECK(port_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_port_t *ext_port = (ext_port_t *) port_hdl;

    ESP_LOGD(EXT_PORT_TAG, "[%d:%d] Disable",
             ext_port->constant.parent_dev_addr,
             ext_port->constant.port_num);

    EXT_PORT_CHECK(ext_port->state == USB_PORT_STATE_ENABLED, ESP_ERR_INVALID_STATE);

    port_set_actions(ext_port, PORT_ACTION_DISABLE);
    return ESP_OK;
}

/**
 * @brief Deletes the port object
 *
 * @note This function should only be called from the External Hub Driver
 *
 * @param[in] port_hdl      Port object handle
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Port Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    The port handle can't be NULL
 *    - ESP_OK:                 Port object has been freed and deleted
 */
static esp_err_t port_delete(void *port_hdl)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_PORT_CHECK(port_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_port_t *ext_port = (ext_port_t *) port_hdl;
    // Sanity checks
    assert(ext_port->dev_state == PORT_DEV_NOT_PRESENT);    // Port should not have a device
    assert(ext_port->flags.in_pending_list == 0);           // Port should not be in pending list

    ESP_LOGD(EXT_PORT_TAG, "[%d:%d] Freeing",
             ext_port->constant.parent_dev_addr,
             ext_port->constant.port_num);

    heap_caps_free(ext_port);

    return ESP_OK;
}

/**
 * @brief Marks the Port as gone and initialize process of freeing the Port
 *
 * @note This function should only be called from the External Hub Driver
 *
 * @param[in] port_hdl Port object handle
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Port Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    The port handle can't be NULL
 *    - ESP_ERR_NOT_FINISHED:   The port has a device and should be recycled
 *    - ESP_OK:                 Port object has been marked as gone and could be freed
 */
static esp_err_t port_gone(void *port_hdl)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_PORT_CHECK(port_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_port_t *ext_port = (ext_port_t *)port_hdl;

    ESP_LOGD(EXT_PORT_TAG, "[%d:%d] Port is gone (state=%d, dev=%d)",
             ext_port->constant.parent_dev_addr,
             ext_port->constant.port_num,
             ext_port->state,
             ext_port->dev_state);

    bool has_device = false;

    ext_port->flags.is_gone = 1;
    ext_port->flags.waiting_free = 1;

    switch (ext_port->state) {
    case USB_PORT_STATE_ENABLED:
    case USB_PORT_STATE_DISABLED:
    case USB_PORT_STATE_RESETTING:
        // Port could have a device
        if (ext_port->dev_state == PORT_DEV_PRESENT) {
            ext_port->dev_state = PORT_DEV_NOT_PRESENT;
            ext_port->flags.waiting_recycle = 1;
            port_event(ext_port, EXT_PORT_DISCONNECTED);
            has_device = true;
        }
        break;
    case USB_PORT_STATE_NOT_CONFIGURED:
    // Port has been added, but the Driver has not started the handling yet
    case USB_PORT_STATE_POWERED_OFF:
    case USB_PORT_STATE_DISCONNECTED:
        // Port has been added, but doesn't have a device
        // We can remove the port from pending list and could be freed by the External Hub Driver
        break;
    default:
        // Should never occur
        abort();
        break;
    }

    // If the port is in handling list, stop it
    if (ext_port->flags.in_pending_list) {
        port_stop_handling(ext_port);
    }

    return (has_device) ? ESP_ERR_NOT_FINISHED : ESP_OK;
}

/**
 * @brief Indicate to the External Port Driver that Port request has been finished and the Port requires handling
 *
 * @note This function should only be called from the External Hub Driver
 *
 * @param[in] port_hdl      Port object handle
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Port Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    The port handle can't be NULL
 *    - ESP_OK:                 Port action was requested
 */
static esp_err_t port_get_status(void* port_hdl)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_PORT_CHECK(port_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_port_t *ext_port = (ext_port_t *)port_hdl;

    port_set_actions(ext_port, PORT_ACTION_GET_STATUS);
    return ESP_OK;
}

/**
 * @brief Indicate to the External Port Driver that the Port has new status
 *
 * @note This function should only be called from the External Hub Driver
 *
 * @param[in] port_hdl      Port object handle
 * @param[in] port_status   New status data
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Port Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    The port handle can't be NULL, the status data can't be NULL
 *    - ESP_OK:                 Port action was requested
 */
static esp_err_t port_set_status(void* port_hdl, const usb_port_status_t *port_status)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_PORT_CHECK(port_hdl != NULL && port_status != NULL, ESP_ERR_INVALID_ARG);
    ext_port_t *ext_port = (ext_port_t *)port_hdl;

    // Update status
    ext_port->status.wPortChange.val = port_status->wPortChange.val;
    ext_port->status.wPortStatus.val = port_status->wPortStatus.val;
    // Status valid
    ext_port->flags.status_outdated = 0;
    // Remove status lock
    ext_port->flags.status_lock = 0;
    // Request port handling
    port_set_actions(ext_port, PORT_ACTION_HANDLE);
    return ESP_OK;
}

/**
 * @brief Indicate to the External Port Driver that the Port requires processing
 *
 * @note This function should only be called from the External Hub Driver
 *
 * @param[in] port_hdl      Port object handle
 * @return
 *    - ESP_ERR_NOT_ALLOWED:    The External Port Driver has not been installed
 *    - ESP_ERR_INVALID_ARG:    The port handle can't be NULL
 *    - ESP_OK:                 Port action was requested
 */
static esp_err_t port_req_process(void* port_hdl)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_PORT_CHECK(port_hdl != NULL, ESP_ERR_INVALID_ARG);
    ext_port_t *ext_port = (ext_port_t *)port_hdl;

    if (ext_port->flags.status_outdated) {
        port_set_actions(ext_port, PORT_ACTION_GET_STATUS);
    } else {
        port_set_actions(ext_port, PORT_ACTION_HANDLE);
    }
    return ESP_OK;
}

// -----------------------------------------------------------------------------
// ------------------ External Port Processing Functions -----------------------
// -----------------------------------------------------------------------------

/**
 * @brief External Port Driver API
 */
const ext_hub_port_driver_t ext_port_driver = {
    .new = port_new,
    .reset = port_reset,
    .recycle = port_recycle,
    .active = port_active,
    .disable = port_disable,
    .gone = port_gone,
    .del = port_delete,
    .get_speed = port_get_speed,
    .get_status = port_get_status,
    .set_status = port_set_status,
    .req_process = port_req_process,
};

esp_err_t ext_port_install(const ext_port_driver_config_t *config)
{
    EXT_PORT_CHECK(p_ext_port_driver == NULL, ESP_ERR_NOT_ALLOWED);

    ext_port_driver_t *ext_port_drv = heap_caps_calloc(1, sizeof(ext_port_driver_t), MALLOC_CAP_DEFAULT);
    EXT_PORT_CHECK(ext_port_drv != NULL, ESP_ERR_NO_MEM);

    // Save callbacks
    ext_port_drv->constant.proc_req_cb = config->proc_req_cb;
    ext_port_drv->constant.proc_req_cb_arg = config->proc_req_cb_arg;
    ext_port_drv->constant.event_cb = config->event_cb;
    ext_port_drv->constant.event_cb_arg = config->event_cb_arg;
    TAILQ_INIT(&ext_port_drv->single_thread.pending_tailq);

    p_ext_port_driver = ext_port_drv;

    ESP_LOGD(EXT_PORT_TAG, "Driver installed");
    return ESP_OK;
}

esp_err_t ext_port_uninstall(void)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_NOT_ALLOWED);
    EXT_PORT_CHECK(TAILQ_EMPTY(&p_ext_port_driver->single_thread.pending_tailq), ESP_ERR_INVALID_STATE);
    ext_port_driver_t *ext_port_drv = p_ext_port_driver;
    p_ext_port_driver = NULL;

    heap_caps_free(ext_port_drv);
    ESP_LOGD(EXT_PORT_TAG, "Driver uninstalled");
    return ESP_OK;
}

esp_err_t ext_port_process(void)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, ESP_ERR_NOT_ALLOWED);

    ext_port_t *ext_port = get_port_from_pending_list();
    if (ext_port == NULL) {
        // No more ports in list to handle
        // NOTE:
        // This is possible, when an external Hub detached sooner than being
        // configured and handled by the Driver. This is not an error case,
        // because the processing requests by the HUB_DRIVER_ACTION_EXT_PORT
        // flag in the Hub Driver and there is no way to clean it.
        ESP_LOGD(EXT_PORT_TAG, "No more ports to handle");
        return ESP_OK;
    }

    uint32_t action_flags = ext_port->action_flags;
    ext_port->action_flags = 0;

    while (action_flags) {
        // Keep processing until all port's action have been handled
        ESP_LOGD(EXT_PORT_TAG, "[%d:%d] Processing actions 0x%"PRIx32"",
                 ext_port->constant.parent_dev_addr,
                 ext_port->constant.port_num,
                 action_flags);

        if (action_flags & PORT_ACTION_HANDLE) {
            handle_port(ext_port);
        }
        if (action_flags & PORT_ACTION_DISABLE) {
            handle_disable(ext_port);
        }
        if (action_flags & PORT_ACTION_RECYCLE) {
            handle_recycle(ext_port);
        }

        /*
        * Feature related actions are mutual exclusive and require:
        * - transfer completion callback
        * - further request of new port status via get_status(), except PORT_ACTION_GET_STATUS itself
        */
        if (action_flags & PORT_ACTION_GET_STATUS) {
            port_request_status(ext_port);
        } else if (action_flags & PORT_ACTION_RESET) {
            if (ext_port->state != USB_PORT_STATE_RESETTING) {
                /*
                * IMPORTANT NOTE
                * This is possible, when the reset is requested via port_reset()
                * Port reset is possible only in two states:
                * - USB_PORT_STATE_DISCONNECTED (mainly, first reset)
                * - USB_PORT_STATE_ENABLED      (mainly, second reset)
                */
                assert(ext_port->state ==  USB_PORT_STATE_DISCONNECTED ||
                       ext_port->state == USB_PORT_STATE_ENABLED);
                ext_port->state = USB_PORT_STATE_RESETTING;
            }
            port_set_feature(ext_port, USB_FEATURE_PORT_RESET);
        }

        action_flags = ext_port->action_flags;
        ext_port->action_flags = 0;
    }

    return ESP_OK;
}

const ext_hub_port_driver_t *ext_port_get_driver(void)
{
    EXT_PORT_CHECK(p_ext_port_driver != NULL, NULL);
    return &ext_port_driver;
}
