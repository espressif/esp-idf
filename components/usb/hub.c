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
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_private/critical_section.h"
#include "esp_log.h"
#include "usb_private.h"
#include "hcd.h"
#include "hub.h"
#include "usb/usb_helpers.h"

#if ENABLE_USB_HUBS
#include "ext_port.h"
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

<<<<<<< HEAD
#define SET_ADDR_RECOVERY_INTERVAL_MS               CONFIG_USB_HOST_SET_ADDR_RECOVERY_MS

#define ENUM_CTRL_TRANSFER_MAX_DATA_LEN             CONFIG_USB_HOST_CONTROL_TRANSFER_MAX_SIZE
#define ENUM_DEV_ADDR                               1       // Device address used in enumeration
#define ENUM_DEV_UID                                1       // Unique ID for device connected to root port
#define ENUM_CONFIG_INDEX_DEFAULT                   0       // Index used to get the first configuration descriptor of the device
#define ENUM_SHORT_DESC_REQ_LEN                     8       // Number of bytes to request when getting a short descriptor (just enough to get bMaxPacketSize0 or wTotalLength)
#define ENUM_WORST_CASE_MPS_LS                      8       // The worst case MPS of EP0 for a LS device
#define ENUM_WORST_CASE_MPS_FS                      64      // The worst case MPS of EP0 for a FS device
#define ENUM_LOW_SPEED_MPS                          8       // Worst case MPS for the default endpoint of a low-speed device
#define ENUM_FULL_SPEED_MPS                         64      // Worst case MPS for the default endpoint of a full-speed device
#define ENUM_LANGID                                 0x409   // Current enumeration only supports English (United States) string descriptors

// Hub driver action flags. LISTED IN THE ORDER THEY SHOULD BE HANDLED IN within hub_process(). Some actions are mutually exclusive
#define HUB_DRIVER_FLAG_ACTION_ROOT_EVENT           0x01
#define HUB_DRIVER_FLAG_ACTION_PORT_REQ             0x02
#define HUB_DRIVER_FLAG_ACTION_ENUM_EVENT           0x04
=======
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
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade

#define PORT_REQ_DISABLE                            0x01
#define PORT_REQ_RECOVER                            0x02

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

<<<<<<< HEAD
typedef struct {
    // Constant
    urb_t *urb;                     /**< URB used for enumeration control transfers. Max data length of ENUM_CTRL_TRANSFER_MAX_DATA_LEN */
    // Initialized at start of a particular enumeration
    usb_device_handle_t dev_hdl;    /**< Handle of device being enumerated */
    // Updated during enumeration
    enum_stage_t stage;             /**< Current enumeration stage */
    int expect_num_bytes;           /**< Expected number of bytes for IN transfers stages. Set to 0 for OUT transfer */
    uint8_t bMaxPacketSize0;        /**< Max packet size of the device's EP0. Read from bMaxPacketSize0 field of device descriptor */
    uint16_t wTotalLength;          /**< Total length of device's configuration descriptor. Read from wTotalLength field of config descriptor */
    uint8_t iManufacturer;          /**< Index of the Manufacturer string descriptor */
    uint8_t iProduct;               /**< Index of the Product string descriptor */
    uint8_t iSerialNumber;          /**< Index of the Serial Number string descriptor */
    uint8_t str_desc_bLength;       /**< Saved bLength from getting a short string descriptor */
    uint8_t bConfigurationValue;    /**< Device's current configuration number */
    uint8_t enum_config_index;      /**< Configuration index used during enumeration */
#ifdef ENABLE_ENUM_FILTER_CALLBACK
    usb_host_enum_filter_cb_t enum_filter_cb;   /**< Set device configuration callback */
    bool graceful_exit;                         /**< Exit enumeration by user's request from the callback function */
#endif // ENABLE_ENUM_FILTER_CALLBACK
} enum_ctrl_t;
=======
typedef struct dev_tree_node_s dev_tree_node_t;
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade

typedef struct {
    struct {
        union {
            struct {
                hub_flag_action_t actions: 8;       /**< Hub actions */
                uint32_t reserved24: 24;            /**< Reserved */
            };
<<<<<<< HEAD
            uint32_t val;
        } flags;
        root_port_state_t root_port_state;
        unsigned int port_reqs;
    } dynamic;
    // Single thread members don't require a critical section so long as they are never accessed from multiple threads
    struct {
        unsigned int root_dev_uid;  // UID of the device connected to root port. 0 if no device connected
        enum_ctrl_t enum_ctrl;
    } single_thread;
    // Constant members do no change after installation thus do not require a critical section
=======
            uint32_t val;                           /**< Hub flag action value */
        } flags;                                    /**< Hub flags */
        root_port_state_t root_port_state;          /**< Root port state */
        unsigned int port_reqs;                     /**< Root port request flag */
    } dynamic;                                      /**< Dynamic members. Require a critical section */

    struct {
        TAILQ_HEAD(tailhead_devs, dev_tree_node_s) dev_nodes_tailq;     /**< Tailq of attached devices */
        unsigned int next_uid;                      /**< Unique ID for next upcoming device */
    } single_thread;                                /**< Single thread members don't require a critical section so long as they are never accessed from multiple threads */

>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
    struct {
        hcd_port_handle_t root_port_hdl;            /**< Root port HCD handle */
        usb_proc_req_cb_t proc_req_cb;              /**< Process request callback */
        void *proc_req_cb_arg;                      /**< Process request callback argument */
        hub_event_cb_t event_cb;                    /**< Hub Driver event callback */
        void *event_cb_arg;                         /**< Event callback argument */
    } constant;                                     /**< Constant members. Do not change after installation thus do not require a critical section or mutex */
} hub_driver_t;

static hub_driver_t *p_hub_driver_obj = NULL;

const char *HUB_DRIVER_TAG = "HUB";

DEFINE_CRIT_SECTION_LOCK_STATIC(hub_driver_lock);
#define HUB_DRIVER_ENTER_CRITICAL_ISR()       esp_os_enter_critical_isr(&hub_driver_lock)
#define HUB_DRIVER_EXIT_CRITICAL_ISR()        esp_os_exit_critical_isr(&hub_driver_lock)
#define HUB_DRIVER_ENTER_CRITICAL()           esp_os_enter_critical(&hub_driver_lock)
#define HUB_DRIVER_EXIT_CRITICAL()            esp_os_exit_critical(&hub_driver_lock)
#define HUB_DRIVER_ENTER_CRITICAL_SAFE()      esp_os_enter_critical_safe(&hub_driver_lock)
#define HUB_DRIVER_EXIT_CRITICAL_SAFE()       esp_os_exit_critical_safe(&hub_driver_lock)

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
<<<<<<< HEAD
 * @brief Control transfer callback used for enumeration
 *
 * @param transfer Transfer object
 */
static void enum_transfer_callback(usb_transfer_t *transfer);

// ------------------------------------------------- Enum Functions ----------------------------------------------------

static bool enum_stage_start(enum_ctrl_t *enum_ctrl)
{
    // Open the newly added device (at address 0)
    ESP_ERROR_CHECK(usbh_devs_open(0, &p_hub_driver_obj->single_thread.enum_ctrl.dev_hdl));

    // Get the speed of the device to set the initial MPS of EP0
    usb_device_info_t dev_info;
    ESP_ERROR_CHECK(usbh_dev_get_info(p_hub_driver_obj->single_thread.enum_ctrl.dev_hdl, &dev_info));
    enum_ctrl->bMaxPacketSize0 = (dev_info.speed == USB_SPEED_LOW) ? ENUM_WORST_CASE_MPS_LS : ENUM_WORST_CASE_MPS_FS;

    // Lock the device for enumeration. This allows us call usbh_dev_set_...() functions during enumeration
    ESP_ERROR_CHECK(usbh_dev_enum_lock(p_hub_driver_obj->single_thread.enum_ctrl.dev_hdl));

    // Flag to gracefully exit the enumeration process if requested by the user in the enumeration filter cb
#ifdef ENABLE_ENUM_FILTER_CALLBACK
    enum_ctrl->graceful_exit = false;
#endif // ENABLE_ENUM_FILTER_CALLBACK
    return true;
}

static bool enum_stage_second_reset(enum_ctrl_t *enum_ctrl)
{
    if (hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_RESET) != ESP_OK) {
        ESP_LOGE(HUB_DRIVER_TAG, "Failed to issue second reset");
        return false;
    }
    return true;
}

static void get_string_desc_index_and_langid(enum_ctrl_t *enum_ctrl, uint8_t *index, uint16_t *langid)
{
    switch (enum_ctrl->stage) {
    case ENUM_STAGE_GET_SHORT_LANGID_TABLE:
    case ENUM_STAGE_GET_FULL_LANGID_TABLE:
        *index = 0;     // The LANGID table uses an index of 0
        *langid = 0;    // Getting the LANGID table itself should use a LANGID of 0
        break;
    case ENUM_STAGE_GET_SHORT_MANU_STR_DESC:
    case ENUM_STAGE_GET_FULL_MANU_STR_DESC:
        *index = enum_ctrl->iManufacturer;
        *langid = ENUM_LANGID;  // Use the default LANGID
        break;
    case ENUM_STAGE_GET_SHORT_PROD_STR_DESC:
    case ENUM_STAGE_GET_FULL_PROD_STR_DESC:
        *index = enum_ctrl->iProduct;
        *langid = ENUM_LANGID;  // Use the default LANGID
        break;
    case ENUM_STAGE_GET_SHORT_SER_STR_DESC:
    case ENUM_STAGE_GET_FULL_SER_STR_DESC:
        *index = enum_ctrl->iSerialNumber;
        *langid = ENUM_LANGID;  // Use the default LANGID
        break;
    default:
        // Should not occur
        abort();
        break;
    }
}

static bool set_config_index(enum_ctrl_t *enum_ctrl, const usb_device_desc_t *device_desc)
{
#ifdef ENABLE_ENUM_FILTER_CALLBACK
    // Callback enabled in the menuncofig, but the callback function was not defined
    if (enum_ctrl->enum_filter_cb == NULL) {
        enum_ctrl->enum_config_index = ENUM_CONFIG_INDEX_DEFAULT;
        return true;
=======
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
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
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

<<<<<<< HEAD
static bool enum_stage_transfer(enum_ctrl_t *enum_ctrl)
{
    usb_transfer_t *transfer = &enum_ctrl->urb->transfer;
    switch (enum_ctrl->stage) {
    case ENUM_STAGE_GET_SHORT_DEV_DESC: {
        // Initialize a short device descriptor request
        USB_SETUP_PACKET_INIT_GET_DEVICE_DESC((usb_setup_packet_t *)transfer->data_buffer);
        ((usb_setup_packet_t *)transfer->data_buffer)->wLength = ENUM_SHORT_DESC_REQ_LEN;
        transfer->num_bytes = sizeof(usb_setup_packet_t) + usb_round_up_to_mps(ENUM_SHORT_DESC_REQ_LEN, enum_ctrl->bMaxPacketSize0);
        // IN data stage should return exactly ENUM_SHORT_DESC_REQ_LEN bytes
        enum_ctrl->expect_num_bytes = sizeof(usb_setup_packet_t) + ENUM_SHORT_DESC_REQ_LEN;
        break;
    }
    case ENUM_STAGE_SET_ADDR: {
        USB_SETUP_PACKET_INIT_SET_ADDR((usb_setup_packet_t *)transfer->data_buffer, ENUM_DEV_ADDR);
        transfer->num_bytes = sizeof(usb_setup_packet_t);   // No data stage
        enum_ctrl->expect_num_bytes = 0;    // OUT transfer. No need to check number of bytes returned
        break;
    }
    case ENUM_STAGE_GET_FULL_DEV_DESC: {
        USB_SETUP_PACKET_INIT_GET_DEVICE_DESC((usb_setup_packet_t *)transfer->data_buffer);
        transfer->num_bytes = sizeof(usb_setup_packet_t) + usb_round_up_to_mps(sizeof(usb_device_desc_t), enum_ctrl->bMaxPacketSize0);
        // IN data stage should return exactly sizeof(usb_device_desc_t) bytes
        enum_ctrl->expect_num_bytes = sizeof(usb_setup_packet_t) + sizeof(usb_device_desc_t);
        break;
    }
    case ENUM_STAGE_GET_SHORT_CONFIG_DESC: {
        // Get a short config descriptor at index 0
        USB_SETUP_PACKET_INIT_GET_CONFIG_DESC((usb_setup_packet_t *)transfer->data_buffer, enum_ctrl->enum_config_index, ENUM_SHORT_DESC_REQ_LEN);
        transfer->num_bytes = sizeof(usb_setup_packet_t) + usb_round_up_to_mps(ENUM_SHORT_DESC_REQ_LEN, enum_ctrl->bMaxPacketSize0);
        // IN data stage should return exactly ENUM_SHORT_DESC_REQ_LEN bytes
        enum_ctrl->expect_num_bytes = sizeof(usb_setup_packet_t) + ENUM_SHORT_DESC_REQ_LEN;
        break;
    }
    case ENUM_STAGE_GET_FULL_CONFIG_DESC: {
        // Get the full configuration descriptor at index 0, requesting its exact length.
        USB_SETUP_PACKET_INIT_GET_CONFIG_DESC((usb_setup_packet_t *)transfer->data_buffer, enum_ctrl->enum_config_index, enum_ctrl->wTotalLength);
        transfer->num_bytes = sizeof(usb_setup_packet_t) + usb_round_up_to_mps(enum_ctrl->wTotalLength, enum_ctrl->bMaxPacketSize0);
        // IN data stage should return exactly wTotalLength bytes
        enum_ctrl->expect_num_bytes = sizeof(usb_setup_packet_t) + enum_ctrl->wTotalLength;
        break;
    }
    case ENUM_STAGE_SET_CONFIG: {
        USB_SETUP_PACKET_INIT_SET_CONFIG((usb_setup_packet_t *)transfer->data_buffer, enum_ctrl->bConfigurationValue);
        transfer->num_bytes = sizeof(usb_setup_packet_t);   // No data stage
        enum_ctrl->expect_num_bytes = 0;    // OUT transfer. No need to check number of bytes returned
        break;
    }
    case ENUM_STAGE_GET_SHORT_LANGID_TABLE:
    case ENUM_STAGE_GET_SHORT_MANU_STR_DESC:
    case ENUM_STAGE_GET_SHORT_PROD_STR_DESC:
    case ENUM_STAGE_GET_SHORT_SER_STR_DESC: {
        uint8_t index;
        uint16_t langid;
        get_string_desc_index_and_langid(enum_ctrl, &index, &langid);
        // Get only the header of the string descriptor
        USB_SETUP_PACKET_INIT_GET_STR_DESC((usb_setup_packet_t *)transfer->data_buffer,
                                           index,
                                           langid,
                                           sizeof(usb_str_desc_t));
        transfer->num_bytes = sizeof(usb_setup_packet_t) + usb_round_up_to_mps(sizeof(usb_str_desc_t), enum_ctrl->bMaxPacketSize0);
        // IN data stage should return exactly sizeof(usb_str_desc_t) bytes
        enum_ctrl->expect_num_bytes = sizeof(usb_setup_packet_t) + sizeof(usb_str_desc_t);
        break;
    }
    case ENUM_STAGE_GET_FULL_LANGID_TABLE:
    case ENUM_STAGE_GET_FULL_MANU_STR_DESC:
    case ENUM_STAGE_GET_FULL_PROD_STR_DESC:
    case ENUM_STAGE_GET_FULL_SER_STR_DESC: {
        uint8_t index;
        uint16_t langid;
        get_string_desc_index_and_langid(enum_ctrl, &index, &langid);
        // Get the full string descriptor at a particular index, requesting the descriptors exact length
        USB_SETUP_PACKET_INIT_GET_STR_DESC((usb_setup_packet_t *)transfer->data_buffer,
                                           index,
                                           langid,
                                           enum_ctrl->str_desc_bLength);
        transfer->num_bytes = sizeof(usb_setup_packet_t) + usb_round_up_to_mps(enum_ctrl->str_desc_bLength, enum_ctrl->bMaxPacketSize0);
        // IN data stage should return exactly str_desc_bLength bytes
        enum_ctrl->expect_num_bytes = sizeof(usb_setup_packet_t) + enum_ctrl->str_desc_bLength;
        break;
    }
    default:    // Should never occur
        abort();
        break;
    }
    if (usbh_dev_submit_ctrl_urb(enum_ctrl->dev_hdl, enum_ctrl->urb) != ESP_OK) {
        ESP_LOGE(HUB_DRIVER_TAG, "Failed to submit: %s", enum_stage_strings[enum_ctrl->stage]);
        return false;
    }
    return true;
}
=======
    ESP_LOGD(HUB_DRIVER_TAG, "Device tree node (uid=%d): new", node_uid);
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade

    hub_event_data_t event_data = {
        .event = HUB_EVENT_CONNECTED,
        .connected = {
            .uid = node_uid,
        },
    };
    p_hub_driver_obj->constant.event_cb(&event_data, p_hub_driver_obj->constant.event_cb_arg);

    return ret;

<<<<<<< HEAD
    return false;
}

static bool enum_stage_transfer_check(enum_ctrl_t *enum_ctrl)
{
    // Check transfer status
    usb_transfer_t *transfer = &enum_ctrl->urb->transfer;
    if (transfer->status != USB_TRANSFER_STATUS_COMPLETED) {
        ESP_LOGE(HUB_DRIVER_TAG, "Bad transfer status %d: %s", transfer->status, enum_stage_strings[enum_ctrl->stage]);
        return false;
    }
    // Check IN transfer returned the expected correct number of bytes
    if (enum_ctrl->expect_num_bytes != 0 && transfer->actual_num_bytes != enum_ctrl->expect_num_bytes) {
        if (transfer->actual_num_bytes > enum_ctrl->expect_num_bytes) {
            // The device returned more bytes than requested.
            // This violates the USB specs chapter 9.3.5, but we can continue
            ESP_LOGW(HUB_DRIVER_TAG, "Incorrect number of bytes returned %d: %s", transfer->actual_num_bytes, enum_stage_strings[enum_ctrl->stage]);
        } else {
            // The device returned less bytes than requested. We cannot continue.
            ESP_LOGE(HUB_DRIVER_TAG, "Incorrect number of bytes returned %d: %s", transfer->actual_num_bytes, enum_stage_strings[enum_ctrl->stage]);
            return false;
        }
    }

    // Stage specific checks and updates
    bool ret;
    switch (enum_ctrl->stage) {
    case ENUM_STAGE_CHECK_SHORT_DEV_DESC: {
        const usb_device_desc_t *device_desc = (usb_device_desc_t *)(transfer->data_buffer + sizeof(usb_setup_packet_t));
        // Check if the returned descriptor is corrupted
        if (device_desc->bDescriptorType != USB_B_DESCRIPTOR_TYPE_DEVICE) {
            ESP_LOGE(HUB_DRIVER_TAG, "Short dev desc corrupt");
            ret = false;
            break;
        }
        // Update and save the MPS of the EP0
        if (usbh_dev_set_ep0_mps(enum_ctrl->dev_hdl, device_desc->bMaxPacketSize0) != ESP_OK) {
            ESP_LOGE(HUB_DRIVER_TAG, "Failed to update MPS");
            ret = false;
            break;
        }
        // Save the actual MPS of EP0
        enum_ctrl->bMaxPacketSize0 = device_desc->bMaxPacketSize0;
        ret = true;
        break;
    }
    case ENUM_STAGE_CHECK_ADDR: {
        // Update the device's address
        ESP_ERROR_CHECK(usbh_dev_set_addr(enum_ctrl->dev_hdl, ENUM_DEV_ADDR));
        ret = true;
        break;
    }
    case ENUM_STAGE_CHECK_FULL_DEV_DESC: {
        // Set the device's descriptor
        const usb_device_desc_t *device_desc = (const usb_device_desc_t *)(transfer->data_buffer + sizeof(usb_setup_packet_t));
        ESP_ERROR_CHECK(usbh_dev_set_desc(enum_ctrl->dev_hdl, device_desc));
        enum_ctrl->iManufacturer = device_desc->iManufacturer;
        enum_ctrl->iProduct = device_desc->iProduct;
        enum_ctrl->iSerialNumber = device_desc->iSerialNumber;
        ret = set_config_index(enum_ctrl, device_desc);
        break;
    }
    case ENUM_STAGE_CHECK_SHORT_CONFIG_DESC: {
        const usb_config_desc_t *config_desc = (usb_config_desc_t *)(transfer->data_buffer + sizeof(usb_setup_packet_t));
        // Check if the returned descriptor is corrupted
        if (config_desc->bDescriptorType != USB_B_DESCRIPTOR_TYPE_CONFIGURATION) {
            ESP_LOGE(HUB_DRIVER_TAG, "Short config desc corrupt");
            ret = false;
            break;
        }
#if (ENUM_CTRL_TRANSFER_MAX_DATA_LEN < UINT16_MAX)  // Suppress -Wtype-limits warning due to uint16_t wTotalLength
        // Check if the descriptor is too long to be supported
        if (config_desc->wTotalLength > ENUM_CTRL_TRANSFER_MAX_DATA_LEN) {
            ESP_LOGE(HUB_DRIVER_TAG, "Configuration descriptor larger than control transfer max length");
            ret = false;
            break;
        }
#endif
        // Save the configuration descriptors full length
        enum_ctrl->wTotalLength = config_desc->wTotalLength;
        ret = true;
        break;
    }
    case ENUM_STAGE_CHECK_FULL_CONFIG_DESC: {
        // Set the device's configuration descriptor
        const usb_config_desc_t *config_desc = (usb_config_desc_t *)(transfer->data_buffer + sizeof(usb_setup_packet_t));
        enum_ctrl->bConfigurationValue = config_desc->bConfigurationValue;
        ESP_ERROR_CHECK(usbh_dev_set_config_desc(enum_ctrl->dev_hdl, config_desc));
        ret = true;
        break;
    }
    case ENUM_STAGE_CHECK_CONFIG: {
        ret = true;
        // Nothing to do
        break;
    }
    case ENUM_STAGE_CHECK_SHORT_LANGID_TABLE:
    case ENUM_STAGE_CHECK_SHORT_MANU_STR_DESC:
    case ENUM_STAGE_CHECK_SHORT_PROD_STR_DESC:
    case ENUM_STAGE_CHECK_SHORT_SER_STR_DESC: {
        const usb_str_desc_t *str_desc = (usb_str_desc_t *)(transfer->data_buffer + sizeof(usb_setup_packet_t));
        // Check if the returned descriptor is supported or corrupted
        if (str_desc->bDescriptorType == 0) {
            ESP_LOGW(HUB_DRIVER_TAG, "String desc not supported");
            ret = false;
            break;
        } else if (str_desc->bDescriptorType != USB_B_DESCRIPTOR_TYPE_STRING) {
            ESP_LOGE(HUB_DRIVER_TAG, "Full string desc corrupt");
            ret = false;
            break;
        }
#if (ENUM_CTRL_TRANSFER_MAX_DATA_LEN < UINT8_MAX)   // Suppress -Wtype-limits warning due to uint8_t bLength
        // Check if the descriptor is too long to be supported
        if (str_desc->bLength > (uint32_t)ENUM_CTRL_TRANSFER_MAX_DATA_LEN) {
            ESP_LOGE(HUB_DRIVER_TAG, "String descriptor larger than control transfer max length");
            ret = false;
            break;
        }
#endif
        // Save the descriptors full length
        enum_ctrl->str_desc_bLength = str_desc->bLength;
        ret = true;
        break;
    }
    case ENUM_STAGE_CHECK_FULL_LANGID_TABLE:
    case ENUM_STAGE_CHECK_FULL_MANU_STR_DESC:
    case ENUM_STAGE_CHECK_FULL_PROD_STR_DESC:
    case ENUM_STAGE_CHECK_FULL_SER_STR_DESC: {
        const usb_str_desc_t *str_desc = (usb_str_desc_t *)(transfer->data_buffer + sizeof(usb_setup_packet_t));
        // Check if the returned descriptor is supported or corrupted
        if (str_desc->bDescriptorType == 0) {
            ESP_LOGW(HUB_DRIVER_TAG, "String desc not supported");
            ret = false;
            break;
        } else if (str_desc->bDescriptorType != USB_B_DESCRIPTOR_TYPE_STRING) {
            ESP_LOGE(HUB_DRIVER_TAG, "Full string desc corrupt");
            ret = false;
            break;
        }
        if (enum_ctrl->stage == ENUM_STAGE_CHECK_FULL_LANGID_TABLE) {
            // Scan the LANGID table for our target LANGID
            bool target_langid_found = false;
            int langid_table_num_entries = (str_desc->bLength - sizeof(usb_str_desc_t)) / 2;  // Each LANGID is 2 bytes
            for (int i = 0; i < langid_table_num_entries; i++) {  // Each LANGID is 2 bytes
                if (str_desc->wData[i] == ENUM_LANGID) {
                    target_langid_found = true;
                    break;
                }
            }
            if (!target_langid_found) {
                ESP_LOGE(HUB_DRIVER_TAG, "LANGID 0x%x not found", ENUM_LANGID);
            }
            ret = target_langid_found;
            break;
        } else {
            // Fill the string descriptor into the device object
            int select;
            if (enum_ctrl->stage == ENUM_STAGE_CHECK_FULL_MANU_STR_DESC) {
                select = 0;
            } else if (enum_ctrl->stage == ENUM_STAGE_CHECK_FULL_PROD_STR_DESC) {
                select = 1;
            } else {    // ENUM_STAGE_CHECK_FULL_PROD_STR_DESC
                select = 2;
            }
            ESP_ERROR_CHECK(usbh_dev_set_str_desc(enum_ctrl->dev_hdl, str_desc, select));
            ret = true;
            break;
        }
    }
    default:    // Should never occur
        ret = false;
        abort();
        break;
    }
=======
fail:
    heap_caps_free(dev_tree_node);
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
    return ret;
}

static esp_err_t dev_tree_node_reset_completed(usb_device_handle_t parent_dev_hdl, uint8_t parent_port_num)
{
<<<<<<< HEAD
    // Unlock the device as we are done with the enumeration
    ESP_ERROR_CHECK(usbh_dev_enum_unlock(enum_ctrl->dev_hdl));
    // Propagate a new device event
    ESP_ERROR_CHECK(usbh_devs_new_dev_event(enum_ctrl->dev_hdl));
    // We are done with using the device. Close it.
    ESP_ERROR_CHECK(usbh_devs_close(enum_ctrl->dev_hdl));
    // Clear values in enum_ctrl
    enum_ctrl->dev_hdl = NULL;
}

static void enum_stage_cleanup_failed(enum_ctrl_t *enum_ctrl)
{
    if (enum_ctrl->dev_hdl) {
        // Close the device and unlock it as we done with enumeration
        ESP_ERROR_CHECK(usbh_dev_enum_unlock(enum_ctrl->dev_hdl));
        ESP_ERROR_CHECK(usbh_devs_close(enum_ctrl->dev_hdl));
        // We allow this to fail in case the device object was already freed
        usbh_devs_remove(ENUM_DEV_UID);
    }
    // Clear values in enum_ctrl
    enum_ctrl->dev_hdl = NULL;
}

static enum_stage_t get_next_stage(enum_stage_t old_stage, enum_ctrl_t *enum_ctrl)
{
    enum_stage_t new_stage = old_stage + 1;
    // Skip the GET_DESCRIPTOR string type corresponding stages if a particular index is 0.
    while (((new_stage == ENUM_STAGE_GET_SHORT_MANU_STR_DESC ||
             new_stage == ENUM_STAGE_CHECK_SHORT_MANU_STR_DESC ||
             new_stage == ENUM_STAGE_GET_FULL_MANU_STR_DESC ||
             new_stage == ENUM_STAGE_CHECK_FULL_MANU_STR_DESC) && enum_ctrl->iManufacturer == 0) ||
            ((new_stage == ENUM_STAGE_GET_SHORT_PROD_STR_DESC ||
              new_stage == ENUM_STAGE_CHECK_SHORT_PROD_STR_DESC ||
              new_stage == ENUM_STAGE_GET_FULL_PROD_STR_DESC ||
              new_stage == ENUM_STAGE_CHECK_FULL_PROD_STR_DESC) && enum_ctrl->iProduct == 0) ||
            ((new_stage == ENUM_STAGE_GET_SHORT_SER_STR_DESC ||
              new_stage == ENUM_STAGE_CHECK_SHORT_SER_STR_DESC ||
              new_stage == ENUM_STAGE_GET_FULL_SER_STR_DESC ||
              new_stage == ENUM_STAGE_CHECK_FULL_SER_STR_DESC) && enum_ctrl->iSerialNumber == 0)) {
        new_stage++;
    }
    return new_stage;
}

static void enum_set_next_stage(enum_ctrl_t *enum_ctrl, bool last_stage_pass)
{
    // Set next stage
    if (last_stage_pass) {
        if (enum_ctrl->stage != ENUM_STAGE_NONE &&
                enum_ctrl->stage != ENUM_STAGE_CLEANUP &&
                enum_ctrl->stage != ENUM_STAGE_CLEANUP_FAILED) {
            enum_ctrl->stage = get_next_stage(enum_ctrl->stage, enum_ctrl);
        } else {
            enum_ctrl->stage = ENUM_STAGE_NONE;
        }
    } else {
        switch (enum_ctrl->stage) {
        case ENUM_STAGE_START:
            // Stage failed but clean up not required
            enum_ctrl->stage = ENUM_STAGE_NONE;
            break;
        case ENUM_STAGE_GET_SHORT_LANGID_TABLE:
        case ENUM_STAGE_CHECK_SHORT_LANGID_TABLE:
        case ENUM_STAGE_GET_FULL_LANGID_TABLE:
        case ENUM_STAGE_CHECK_FULL_LANGID_TABLE:
        case ENUM_STAGE_GET_SHORT_MANU_STR_DESC:
        case ENUM_STAGE_CHECK_SHORT_MANU_STR_DESC:
        case ENUM_STAGE_GET_FULL_MANU_STR_DESC:
        case ENUM_STAGE_CHECK_FULL_MANU_STR_DESC:
        case ENUM_STAGE_GET_SHORT_PROD_STR_DESC:
        case ENUM_STAGE_CHECK_SHORT_PROD_STR_DESC:
        case ENUM_STAGE_GET_FULL_PROD_STR_DESC:
        case ENUM_STAGE_CHECK_FULL_PROD_STR_DESC:
        case ENUM_STAGE_GET_SHORT_SER_STR_DESC:
        case ENUM_STAGE_CHECK_SHORT_SER_STR_DESC:
        case ENUM_STAGE_GET_FULL_SER_STR_DESC:
        case ENUM_STAGE_CHECK_FULL_SER_STR_DESC:
            // String descriptor stages are allow to fail. We just don't fetch them and treat enumeration as successful
            enum_ctrl->stage = ENUM_STAGE_CLEANUP;
            break;
        default:
            // Enumeration failed. Go to failure clean up
            enum_ctrl->stage = ENUM_STAGE_CLEANUP_FAILED;
=======
    dev_tree_node_t *dev_tree_node = NULL;
    dev_tree_node_t *dev_tree_iter;
    // Search the device tree nodes list for a device node with the specified parent
    TAILQ_FOREACH(dev_tree_iter, &p_hub_driver_obj->single_thread.dev_nodes_tailq, tailq_entry) {
        if (dev_tree_iter->parent_dev_hdl == parent_dev_hdl &&
                dev_tree_iter->parent_port_num == parent_port_num) {
            dev_tree_node = dev_tree_iter;
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
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
        ESP_LOGW(HUB_DRIVER_TAG, "Device tree node (parent_port=%d): not found", parent_port_num);
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
        ESP_LOGW(HUB_DRIVER_TAG, "Device tree node (parent_port=%d): not found", parent_port_num);
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGD(HUB_DRIVER_TAG, "Device tree node (uid=%d): freeing", dev_tree_node->uid);

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

<<<<<<< HEAD
static void enum_transfer_callback(usb_transfer_t *transfer)
{
    // We simply trigger a processing request to handle the completed enumeration control transfer
=======
#ifdef ENABLE_USB_HUBS
static bool ext_hub_callback(bool in_isr, void *user_arg)
{
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
    HUB_DRIVER_ENTER_CRITICAL_SAFE();
    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_ACTION_EXT_HUB;
    HUB_DRIVER_EXIT_CRITICAL_SAFE();
    p_hub_driver_obj->constant.proc_req_cb(USB_PROC_REQ_SOURCE_HUB, false, p_hub_driver_obj->constant.proc_req_cb_arg);
}

static void ext_port_callback(void *user_arg)
{
    HUB_DRIVER_ENTER_CRITICAL();
    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_ACTION_EXT_PORT;
    HUB_DRIVER_EXIT_CRITICAL();
    p_hub_driver_obj->constant.proc_req_cb(USB_PROC_REQ_SOURCE_HUB, false, p_hub_driver_obj->constant.proc_req_cb_arg);
}

static void ext_port_event_callback(ext_port_event_data_t *event_data, void *arg)
{
    switch (event_data->event) {
    case EXT_PORT_CONNECTED:
        // First reset is done by ext_port logic
        usb_speed_t port_speed;

        if (ext_hub_port_get_speed(event_data->connected.ext_hub_hdl,
                                   event_data->connected.parent_port_num,
                                   &port_speed) != ESP_OK) {
            goto new_ds_dev_err;
        }

        // TODO: IDF-10023 Move responsibility of parent-child tree building to Hub Driver instead of USBH
        usb_device_info_t parent_dev_info;
        ESP_ERROR_CHECK(usbh_dev_get_info(event_data->connected.parent_dev_hdl, &parent_dev_info));
        if (parent_dev_info.speed == USB_SPEED_HIGH) {
            if (port_speed != parent_dev_info.speed) {
                ESP_LOGE(HUB_DRIVER_TAG, "Connected device is %s, transaction translator (TT) is not supported",
                (char *[]) {
                    "LS", "FS", "HS"
                }[port_speed]);
                goto new_ds_dev_err;
            }
        }

#if (!CONFIG_USB_HOST_EXT_PORT_SUPPORT_LS)
        if (port_speed == USB_SPEED_LOW) {
            ESP_LOGE(HUB_DRIVER_TAG, "Connected %s-speed device, not supported",
            (char *[]) {
                "Low", "Full", "High"
            }[port_speed]);
            goto new_ds_dev_err;
        }
#endif // CONFIG_USB_HOST_EXT_PORT_SUPPORT_LS

        if (new_dev_tree_node(event_data->connected.parent_dev_hdl, event_data->connected.parent_port_num, port_speed) != ESP_OK) {
            ESP_LOGE(HUB_DRIVER_TAG, "Failed to add new downstream device");
            goto new_ds_dev_err;
        }
        break;
new_ds_dev_err:
        ext_hub_port_disable(event_data->connected.ext_hub_hdl, event_data->connected.parent_port_num);
        break;
    case EXT_PORT_RESET_COMPLETED:
        ESP_ERROR_CHECK(dev_tree_node_reset_completed(event_data->reset_completed.parent_dev_hdl, event_data->reset_completed.parent_port_num));
        break;
    case EXT_PORT_DISCONNECTED:
        // The node could be freed by now, no need to verify the result here
        dev_tree_node_dev_gone(event_data->disconnected.parent_dev_hdl, event_data->disconnected.parent_port_num);
        break;
    default:
        // Should never occur
        abort();
        break;
    }
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
<<<<<<< HEAD
        // Allocate a new device. We use a fixed ENUM_DEV_UID for now since we only support a single device
        if (usbh_devs_add(ENUM_DEV_UID, speed, p_hub_driver_obj->constant.root_port_hdl) != ESP_OK) {
            ESP_LOGE(HUB_DRIVER_TAG, "Failed to add device");
            goto new_dev_err;
        }
        p_hub_driver_obj->single_thread.root_dev_uid = ENUM_DEV_UID;
        // Start enumeration
        HUB_DRIVER_ENTER_CRITICAL();
        p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_ENUM_EVENT;
        p_hub_driver_obj->dynamic.root_port_state = ROOT_PORT_STATE_ENABLED;
        HUB_DRIVER_EXIT_CRITICAL();
        p_hub_driver_obj->single_thread.enum_ctrl.stage = ENUM_STAGE_START;
=======

        if (new_dev_tree_node(NULL, 0, speed) != ESP_OK) {
            ESP_LOGE(HUB_DRIVER_TAG, "Failed to add new device");
            goto new_dev_err;
        }

        // Change Port state
        HUB_DRIVER_ENTER_CRITICAL();
        p_hub_driver_obj->dynamic.root_port_state = ROOT_PORT_STATE_ENABLED;
        HUB_DRIVER_EXIT_CRITICAL();
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
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
<<<<<<< HEAD
            p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_PORT_REQ;
            break;
        case ROOT_PORT_STATE_ENABLED:
            // There is an enabled (active) device. We need to indicate to USBH that the device is gone
            pass_event_to_usbh = true;
=======
            p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_ACTION_ROOT_REQ;
            break;
        case ROOT_PORT_STATE_NOT_POWERED: // The user turned off ports' power. Indicate to USBH that the device is gone
        case ROOT_PORT_STATE_ENABLED: // There is an enabled (active) device. Indicate to USBH that the device is gone
            port_has_device = true;
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
            break;
        default:
            abort();    // Should never occur
            break;
        }
        HUB_DRIVER_EXIT_CRITICAL();
<<<<<<< HEAD
        if (pass_event_to_usbh) {
            // The port must have a device object
            assert(p_hub_driver_obj->single_thread.root_dev_uid != 0);
            // We allow this to fail in case the device object was already freed
            usbh_devs_remove(p_hub_driver_obj->single_thread.root_dev_uid);
=======

        if (port_has_device) {
            ESP_ERROR_CHECK(dev_tree_node_dev_gone(NULL, 0));
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
        }

        break;
    }
    default:
        abort();    // Should never occur
        break;
    }
}

static void root_port_req(hcd_port_handle_t root_port_hdl)
<<<<<<< HEAD
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

static void enum_handle_events(void)
=======
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
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

        // In case the port's power was turned off with usb_host_lib_set_root_port_power(false)
        // we will not turn on the power during port recovery
        HUB_DRIVER_ENTER_CRITICAL();
        const root_port_state_t root_state = p_hub_driver_obj->dynamic.root_port_state;
        HUB_DRIVER_EXIT_CRITICAL();

        if (root_state != ROOT_PORT_STATE_NOT_POWERED) {
            ESP_ERROR_CHECK(hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_POWER_ON));
            HUB_DRIVER_ENTER_CRITICAL();
            p_hub_driver_obj->dynamic.root_port_state = ROOT_PORT_STATE_POWERED;
            HUB_DRIVER_EXIT_CRITICAL();
        }
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
<<<<<<< HEAD
        stage_pass = true;
=======
        abort();    // Should never occur
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
        break;
    }
    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_ACTION_ROOT_REQ;
    HUB_DRIVER_EXIT_CRITICAL();

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
<<<<<<< HEAD
    enum_urb->usb_host_client = (void *)hub_driver_obj;
    enum_urb->transfer.callback = enum_transfer_callback;
=======

#if ENABLE_USB_HUBS
    // Install External Port driver
    ext_port_driver_config_t ext_port_config = {
        .proc_req_cb = ext_port_callback,
        .event_cb = ext_port_event_callback,
    };
    ret = ext_port_install(&ext_port_config);
    if (ret != ESP_OK) {
        goto err_ext_port;
    }

    // Install External HUB driver
    ext_hub_config_t ext_hub_config = {
        .proc_req_cb = ext_hub_callback,
        .port_driver = ext_port_get_driver(),
    };
    ret = ext_hub_install(&ext_hub_config);
    if (ret != ESP_OK) {
        ESP_LOGE(HUB_DRIVER_TAG, "Ext hub install error: %s", esp_err_to_name(ret));
        goto err_ext_hub;
    }
    *client_ret = ext_hub_get_client();
#else
    *client_ret = NULL;
#endif // ENABLE_USB_HUBS
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade

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
        ESP_LOGE(HUB_DRIVER_TAG, "HCD Port init error: %s", esp_err_to_name(ret));
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
<<<<<<< HEAD

    // Write-back client_ret pointer
    *client_ret = (void *)hub_driver_obj;
    ret = ESP_OK;
=======
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade

    return ret;

assign_err:
    ESP_ERROR_CHECK(hcd_port_deinit(root_port_hdl));
err:
#if ENABLE_USB_HUBS
    ext_hub_uninstall();
err_ext_hub:
    ext_port_uninstall();
err_ext_port:
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
    ESP_ERROR_CHECK(ext_port_uninstall());
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
    if (p_hub_driver_obj->dynamic.root_port_state == ROOT_PORT_STATE_NOT_POWERED) {
        // The HUB was already stopped by usb_host_lib_set_root_port_power(false)
        HUB_DRIVER_EXIT_CRITICAL();
        return ESP_OK;
    }
    p_hub_driver_obj->dynamic.root_port_state = ROOT_PORT_STATE_NOT_POWERED;
    HUB_DRIVER_EXIT_CRITICAL();

    // HCD_PORT_CMD_POWER_OFF will only fail if the port is already powered_off
    // This should never happen, so we assert ret == ESP_OK
    const esp_err_t ret = hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_POWER_OFF);
    assert(ret == ESP_OK);
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
        if (ret != ESP_OK) {
            ESP_LOGE(HUB_DRIVER_TAG, "Ext hub port recycle error: %s", esp_err_to_name(ret));
        }
#else
        ESP_LOGW(HUB_DRIVER_TAG, "Recycling External Port is not available (External Hub support disabled)");
        ret = ESP_ERR_NOT_SUPPORTED;
#endif // ENABLE_USB_HUBS
    }

    if (ret == ESP_OK) {
        ESP_ERROR_CHECK(dev_tree_node_remove_by_parent(parent_dev_hdl, parent_port_num));
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
        if (ret != ESP_OK) {
            ESP_LOGE(HUB_DRIVER_TAG, "Ext hub port reset error: %s", esp_err_to_name(ret));
        }
#else
        ESP_LOGW(HUB_DRIVER_TAG, "Resetting External Port is not available (External Hub support disabled)");
        ret = ESP_ERR_NOT_SUPPORTED;
#endif // ENABLE_USB_HUBS
    }
    return ret;
}

<<<<<<< HEAD
esp_err_t hub_port_recycle(unsigned int dev_uid)
{
    if (dev_uid == p_hub_driver_obj->single_thread.root_dev_uid) {
        // Device is free, we can now request its port be recycled
        hcd_port_state_t port_state = hcd_port_get_state(p_hub_driver_obj->constant.root_port_hdl);
        p_hub_driver_obj->single_thread.root_dev_uid = 0;
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
    }

    return ESP_OK;
=======
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
        if (ret != ESP_OK) {
            ESP_LOGE(HUB_DRIVER_TAG, "Ext hub port activation error: %s", esp_err_to_name(ret));
        }
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
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
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
            ESP_ERROR_CHECK(ext_port_process());
        }
        if (action_flags & HUB_DRIVER_ACTION_EXT_HUB) {
            ESP_ERROR_CHECK(ext_hub_process());
        }
#endif // ENABLE_USB_HUBS
        if (action_flags & HUB_DRIVER_ACTION_ROOT_EVENT) {
            root_port_handle_events(p_hub_driver_obj->constant.root_port_hdl);
        }
<<<<<<< HEAD
        if (action_flags & HUB_DRIVER_FLAG_ACTION_PORT_REQ) {
            root_port_req(p_hub_driver_obj->constant.root_port_hdl);
        }
        if (action_flags & HUB_DRIVER_FLAG_ACTION_ENUM_EVENT) {
            enum_handle_events();
=======
        if (action_flags & HUB_DRIVER_ACTION_ROOT_REQ) {
            root_port_req(p_hub_driver_obj->constant.root_port_hdl);
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
        }

        HUB_DRIVER_ENTER_CRITICAL();
        action_flags = p_hub_driver_obj->dynamic.flags.actions;
        p_hub_driver_obj->dynamic.flags.actions = 0;
        HUB_DRIVER_EXIT_CRITICAL();
    }

    return ESP_OK;
}
