/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "usb_private.h"
#include "usbh.h"
#include "enum.h"
#include "usb/usb_helpers.h"

#define SET_ADDR_RECOVERY_INTERVAL_MS               CONFIG_USB_HOST_SET_ADDR_RECOVERY_MS

#define ENUM_CTRL_TRANSFER_MAX_DATA_LEN             CONFIG_USB_HOST_CONTROL_TRANSFER_MAX_SIZE
#define ENUM_INIT_VALUE_DEV_ADDR                    1       // Init value for device address
#define ENUM_DEFAULT_CONFIGURATION_VALUE            1       // Default configuration value for SetConfiguration() request
#define ENUM_SHORT_DESC_REQ_LEN                     8       // Number of bytes to request when getting a short descriptor (just enough to get bMaxPacketSize0 or wTotalLength)
#define ENUM_WORST_CASE_MPS_LS                      8       // The worst case MPS of EP0 for a LS device
#define ENUM_WORST_CASE_MPS_FS_HS                   64      // The worst case MPS of EP0 for a FS/HS device
#define ENUM_LANGID                                 0x409   // Current enumeration only supports English (United States) string descriptors
#define ENUM_MAX_ADDRESS                            (127)   // Maximal device address value

/**
 * @brief Stages of device enumeration listed in their order of execution
 *
 * Entry:
 * - These stages MUST BE LISTED IN THE ORDER OF THEIR EXECUTION as the enumeration will simply increment the current stage
 * - If an error occurs at any stage, ENUM_STAGE_CANCEL acts as a common exit stage on failure
 * - Must start with 0 as enum is also used as an index
 * - The short descriptor stages are used to fetch the start particular descriptors that don't have a fixed length in order to determine the full descriptors length
 * - Any state of Get String Descriptor could be STALLed by the device. In that case we just don't fetch them and treat enumeration as successful
 */
typedef enum {
    ENUM_STAGE_IDLE = 0,                    /**< There is no device awaiting enumeration */
    // Basic Device enumeration
    ENUM_STAGE_GET_SHORT_DEV_DESC,          /**< Getting short dev desc (wLength is ENUM_SHORT_DESC_REQ_LEN) */
    ENUM_STAGE_CHECK_SHORT_DEV_DESC,        /**< Save bMaxPacketSize0 from the short dev desc. Update the MPS of the enum pipe */
    ENUM_STAGE_SECOND_RESET,                /**< Reset the device again (Workaround for old USB devices that get confused by the previous short dev desc request). */
    ENUM_STAGE_SECOND_RESET_COMPLETE,       /**< Reset completed, re-trigger the FSM  */
    ENUM_STAGE_SET_ADDR,                    /**< Send SET_ADDRESS request */
    ENUM_STAGE_CHECK_ADDR,                  /**< Update the enum pipe's target address */
    ENUM_STAGE_SET_ADDR_RECOVERY,           /**< Wait SET ADDRESS recovery interval at least for 2ms due to usb_20, chapter 9.2.6.3 */
    ENUM_STAGE_GET_FULL_DEV_DESC,           /**< Get the full dev desc */
    ENUM_STAGE_CHECK_FULL_DEV_DESC,         /**< Check the full dev desc, fill it into the device object in USBH. Save the string descriptor indexes*/
    ENUM_STAGE_SELECT_CONFIG,               /**< Select configuration: select default ENUM_DEFAULT_CONFIGURATION_VALUE value or use callback if ENABLE_ENUM_FILTER_CALLBACK enabled */
    ENUM_STAGE_GET_SHORT_CONFIG_DESC,       /**< Getting a short config desc (wLength is ENUM_SHORT_DESC_REQ_LEN) */
    ENUM_STAGE_CHECK_SHORT_CONFIG_DESC,     /**< Save wTotalLength of the short config desc */
    ENUM_STAGE_GET_FULL_CONFIG_DESC,        /**< Get the full config desc (wLength is the saved wTotalLength) */
    ENUM_STAGE_CHECK_FULL_CONFIG_DESC,      /**< Check the full config desc, fill it into the device object in USBH */
    // Get String Descriptors
    ENUM_STAGE_GET_SHORT_LANGID_TABLE,      /**< Get the header of the LANGID table string descriptor */
    ENUM_STAGE_CHECK_SHORT_LANGID_TABLE,    /**< Save the bLength of the LANGID table string descriptor */
    ENUM_STAGE_GET_FULL_LANGID_TABLE,       /**< Get the full LANGID table string descriptor */
    ENUM_STAGE_CHECK_FULL_LANGID_TABLE,     /**< Check whether ENUM_LANGID is in the LANGID table */
    ENUM_STAGE_GET_SHORT_MANU_STR_DESC,     /**< Get the header of the iManufacturer string descriptor */
    ENUM_STAGE_CHECK_SHORT_MANU_STR_DESC,   /**< Save the bLength of the iManufacturer string descriptor */
    ENUM_STAGE_GET_FULL_MANU_STR_DESC,      /**< Get the full iManufacturer string descriptor */
    ENUM_STAGE_CHECK_FULL_MANU_STR_DESC,    /**< Check and fill the full iManufacturer string descriptor */
    ENUM_STAGE_GET_SHORT_PROD_STR_DESC,     /**< Get the header of the string descriptor at index iProduct */
    ENUM_STAGE_CHECK_SHORT_PROD_STR_DESC,   /**< Save the bLength of the iProduct string descriptor */
    ENUM_STAGE_GET_FULL_PROD_STR_DESC,      /**< Get the full iProduct string descriptor */
    ENUM_STAGE_CHECK_FULL_PROD_STR_DESC,    /**< Check and fill the full iProduct string descriptor */
    ENUM_STAGE_GET_SHORT_SER_STR_DESC,      /**< Get the header of the string descriptor at index iSerialNumber */
    ENUM_STAGE_CHECK_SHORT_SER_STR_DESC,    /**< Save the bLength of the iSerialNumber string descriptor */
    ENUM_STAGE_GET_FULL_SER_STR_DESC,       /**< Get the full iSerialNumber string descriptor */
    ENUM_STAGE_CHECK_FULL_SER_STR_DESC,     /**< Check and fill the full iSerialNumber string descriptor */
    // Set Configuration
    ENUM_STAGE_SET_CONFIG,                  /**< Send SET_CONFIGURATION request */
    ENUM_STAGE_CHECK_CONFIG,                /**< Check that SET_CONFIGURATION request was successful */
    // Terminal stages
    ENUM_STAGE_COMPLETE,                    /**< Successful enumeration complete. */
    ENUM_STAGE_CANCEL,                      /**< Cancel enumeration. Free device resources */
} enum_stage_t;

const char *const enum_stage_strings[] = {
    "IDLE",
    "GET_SHORT_DEV_DESC",
    "CHECK_SHORT_DEV_DESC",
    "SECOND_RESET",
    "SECOND_RESET_COMPLETE",
    "SET_ADDR",
    "CHECK_ADDR",
    "SET_ADDR_RECOVERY",
    "GET_FULL_DEV_DESC",
    "CHECK_FULL_DEV_DESC",
    "SELECT_CONFIG",
    "GET_SHORT_CONFIG_DESC",
    "CHECK_SHORT_CONFIG_DESC",
    "GET_FULL_CONFIG_DESC",
    "CHECK_FULL_CONFIG_DESC",
    "GET_SHORT_LANGID_TABLE",
    "CHECK_SHORT_LANGID_TABLE",
    "GET_FULL_LANGID_TABLE",
    "CHECK_FULL_LANGID_TABLE",
    "GET_SHORT_MANU_STR_DESC",
    "CHECK_SHORT_MANU_STR_DESC",
    "GET_FULL_MANU_STR_DESC",
    "CHECK_FULL_MANU_STR_DESC",
    "GET_SHORT_PROD_STR_DESC",
    "CHECK_SHORT_PROD_STR_DESC",
    "GET_FULL_PROD_STR_DESC",
    "CHECK_FULL_PROD_STR_DESC",
    "GET_SHORT_SER_STR_DESC",
    "CHECK_SHORT_SER_STR_DESC",
    "GET_FULL_SER_STR_DESC",
    "CHECK_FULL_SER_STR_DESC",
    "SET_CONFIG",
    "CHECK_CONFIG",
    "COMPLETE",
    "CANCEL",
};

typedef struct {
    // Constant
    uint8_t new_dev_addr;           /**< Device address that should be assigned during enumeration */
    uint8_t bMaxPacketSize0;        /**< Max packet size of the device's EP0. Read from bMaxPacketSize0 field of device descriptor */
    uint16_t wTotalLength;          /**< Total length of device's configuration descriptor. Read from wTotalLength field of config descriptor */
    uint8_t iManufacturer;          /**< Index of the Manufacturer string descriptor */
    uint8_t iProduct;               /**< Index of the Product string descriptor */
    uint8_t iSerialNumber;          /**< Index of the Serial Number string descriptor */
    uint8_t str_desc_bLength;       /**< Saved bLength from getting a short string descriptor */
    uint8_t bConfigurationValue;    /**< Device's current configuration number */
} enum_device_params_t;

typedef struct {
    struct {
        // Device related objects, initialized at start of a particular enumeration
        unsigned int dev_uid;                       /**< Unique device ID being enumerated */
        usb_device_handle_t dev_hdl;                /**< Handle of device being enumerated */
        // Parent info for optimization and more clean debug output
        usb_device_handle_t parent_dev_hdl;         /**< Device's parent handle */
        uint8_t parent_dev_addr;                    /**< Device's parent address */
        uint8_t parent_port_num;                    /**< Device's parent port number */
        // Parameters, updated during enumeration
        enum_stage_t stage;                         /**< Current enumeration stage */
        enum_device_params_t dev_params;            /**< Parameters of device under enumeration */
        int expect_num_bytes;                       /**< Expected number of bytes for IN transfers stages. Set to 0 for OUT transfer */
        uint8_t next_dev_addr;                      /**< Device address for device under enumeration */
    } single_thread;                                /**< Single thread members don't require a critical section so long as they are never accessed from multiple threads */

    struct {
        // Internal objects
        urb_t *urb;                                 /**< URB used for enumeration control transfers. Max data length of ENUM_CTRL_TRANSFER_MAX_DATA_LEN */
        // Callbacks
        usb_proc_req_cb_t proc_req_cb;              /**< USB Host process request callback. Refer to proc_req_callback() in usb_host.c */
        void *proc_req_cb_arg;                      /**< USB Host process request callback argument */
        enum_event_cb_t enum_event_cb;              /**< Enumeration driver event callback */
        void *enum_event_cb_arg;                    /**< Enumeration driver event callback argument */
#if ENABLE_ENUM_FILTER_CALLBACK
        usb_host_enum_filter_cb_t enum_filter_cb;   /**< Set device configuration callback */
        void *enum_filter_cb_arg;                   /**< Set device configuration callback argument */
#endif // ENABLE_ENUM_FILTER_CALLBACK
    } constant;                                     /**< Constant members. Do not change after installation thus do not require a critical section or mutex */
} enum_driver_t;

static enum_driver_t *p_enum_driver = NULL;

const char *ENUM_TAG = "ENUM";

// -----------------------------------------------------------------------------
// ---------------------------- Helpers ----------------------------------------
// -----------------------------------------------------------------------------
#define ENUM_CHECK(cond, ret_val) ({                                        \
            if (!(cond)) {                                                  \
                return (ret_val);                                           \
            }                                                               \
})

// -----------------------------------------------------------------------------
// ------------------------ Private functions ----------------------------------
// -----------------------------------------------------------------------------
static inline uint8_t get_next_dev_addr(void)
{
    uint8_t ret = 0;

    p_enum_driver->single_thread.next_dev_addr++;
    if (p_enum_driver->single_thread.next_dev_addr > ENUM_MAX_ADDRESS) {
        p_enum_driver->single_thread.next_dev_addr = ENUM_INIT_VALUE_DEV_ADDR;
    }
    ret = p_enum_driver->single_thread.next_dev_addr;

    return ret;
}

static uint8_t get_next_free_dev_addr(void)
{
    usb_device_handle_t dev_hdl;
    uint8_t new_dev_addr = p_enum_driver->single_thread.next_dev_addr;

    while (1) {
        if (usbh_devs_open(new_dev_addr, &dev_hdl) == ESP_ERR_NOT_FOUND) {
            break;
        }
        // We have a device with the same address on a bus, close device and request new addr
        usbh_dev_close(dev_hdl);
        new_dev_addr = get_next_dev_addr();
    }
    // Sanity check
    assert(new_dev_addr != 0);
    return new_dev_addr;
}

/**
 * @brief Get Configuration descriptor index
 *
 * For Configuration descriptor bConfigurationValue and index are not the same and
 * should be different for SetConfiguration() and GetDescriptor() requests.
 * GetDescriptor(): index from 0 to one less than the bNumConfigurations (refer to section 9.4.3 Get Descriptor)
 * SetConfiguration(): bConfigurationValue field used as a parameter to the SetConfiguration() request. (refer to section 9.6.3 Configuration)
 *
 * @return uint8_t
 */
static inline uint8_t get_configuration_descriptor_index(uint8_t bConfigurationValue)
{
    return (bConfigurationValue == 0) ? bConfigurationValue : (bConfigurationValue - 1);
}

/**
 * @brief Select active configuration
 *
 * During enumeration process, device objects could have several configuration that can be activated
 * To be able to select configuration this call should be used
 * This will call the enumeration filter callback (if enabled) and set the bConfigurationValue for the upcoming SetConfiguration() command
 *
 * @return esp_err_t
 */
static esp_err_t select_active_configuration(void)
{
    // This configuration value must be zero or match a configuration value from a configuration descriptor.
    // If the configuration value is zero, the device is placed in its Address state.
    // But some devices STALLed get configuration descriptor with bConfigurationValue = 1, even they have one configuration with bValue = 1.
    uint8_t bConfigurationValue = ENUM_DEFAULT_CONFIGURATION_VALUE;

#if ENABLE_ENUM_FILTER_CALLBACK
    usb_device_handle_t dev_hdl = p_enum_driver->single_thread.dev_hdl;
    const usb_device_desc_t *dev_desc;
    ESP_ERROR_CHECK(usbh_dev_get_desc(dev_hdl, &dev_desc));

    bool enum_proceed = false;
    // Sanity check
    assert(dev_desc);

    if (p_enum_driver->constant.enum_filter_cb) {
        enum_proceed = p_enum_driver->constant.enum_filter_cb(dev_desc, &bConfigurationValue);
    }

    // User's request NOT to enumerate the USB device
    if (!enum_proceed) {
        ESP_LOGW(ENUM_TAG, "[%d:%d] Abort request of enumeration process (%#x:%#x)",
                 p_enum_driver->single_thread.parent_dev_addr,
                 p_enum_driver->single_thread.parent_port_num,
                 dev_desc->idProduct,
                 dev_desc->idVendor);
        enum_cancel(p_enum_driver->single_thread.dev_uid);
        return ESP_OK;
    }

    // Set configuration descriptor
    if ((bConfigurationValue == 0) || (bConfigurationValue > dev_desc->bNumConfigurations)) {
        ESP_LOGE(ENUM_TAG, "Invalid bConfigurationValue (%d) provided by user, using default", bConfigurationValue);
        bConfigurationValue = ENUM_DEFAULT_CONFIGURATION_VALUE;
    }
#endif // ENABLE_ENUM_FILTER_CALLBACK

    ESP_LOGD(ENUM_TAG, "Selected bConfigurationValue=%d", bConfigurationValue);
    p_enum_driver->single_thread.dev_params.bConfigurationValue = bConfigurationValue;
    return ESP_OK;
}

static esp_err_t second_reset_request(void)
{
    // Notify USB Host
    enum_event_data_t event_data = {
        .event = ENUM_EVENT_RESET_REQUIRED,
        .reset_req = {
            .parent_dev_hdl = p_enum_driver->single_thread.parent_dev_hdl,
            .parent_port_num = p_enum_driver->single_thread.parent_port_num,
        },
    };
    p_enum_driver->constant.enum_event_cb(&event_data, p_enum_driver->constant.enum_event_cb_arg);

    return ESP_OK;
}

/**
 * @brief Get index and langid
 *
 * Returns index and langid, based on enumerator stage.
 *
 * @param[in] stage     Stage
 * @param[out] index    String index
 * @param[out] langid   String langid
 */
static inline void get_index_langid_for_stage(enum_stage_t stage, uint8_t *index, uint16_t *langid)
{
    switch (stage) {
    case ENUM_STAGE_GET_SHORT_LANGID_TABLE:
    case ENUM_STAGE_GET_FULL_LANGID_TABLE:
        *index = 0;     // The LANGID table uses an index of 0
        *langid = 0;    // Getting the LANGID table itself should use a LANGID of 0
        break;
    case ENUM_STAGE_GET_SHORT_MANU_STR_DESC:
    case ENUM_STAGE_GET_FULL_MANU_STR_DESC:
        *index = p_enum_driver->single_thread.dev_params.iManufacturer;
        *langid = ENUM_LANGID;  // Use the default LANGID
        break;
    case ENUM_STAGE_GET_SHORT_PROD_STR_DESC:
    case ENUM_STAGE_GET_FULL_PROD_STR_DESC:
        *index = p_enum_driver->single_thread.dev_params.iProduct;
        *langid = ENUM_LANGID;  // Use the default LANGID
        break;
    case ENUM_STAGE_GET_SHORT_SER_STR_DESC:
    case ENUM_STAGE_GET_FULL_SER_STR_DESC:
        *index = p_enum_driver->single_thread.dev_params.iSerialNumber;
        *langid = ENUM_LANGID;  // Use the default LANGID
        break;
    default:
        // Should not occur
        abort();
        break;
    }
}

/**
 * @brief Control request: General
 *
 * Prepares the Control request byte-data transfer for current stage of the enumerator
 *
 * @param[in] stage  Enumeration stage
 */
static void control_request_general(enum_stage_t stage)
{
    usb_transfer_t *transfer = &p_enum_driver->constant.urb->transfer;
    uint8_t ctrl_ep_mps = p_enum_driver->single_thread.dev_params.bMaxPacketSize0;
    uint16_t wTotalLength = p_enum_driver->single_thread.dev_params.wTotalLength;
    uint8_t bConfigurationValue = p_enum_driver->single_thread.dev_params.bConfigurationValue;
    uint8_t desc_index = get_configuration_descriptor_index(bConfigurationValue);

    switch (stage) {
    case ENUM_STAGE_GET_SHORT_DEV_DESC: {
        // Initialize a short device descriptor request
        USB_SETUP_PACKET_INIT_GET_DEVICE_DESC((usb_setup_packet_t *)transfer->data_buffer);
        ((usb_setup_packet_t *)transfer->data_buffer)->wLength = ENUM_SHORT_DESC_REQ_LEN;
        transfer->num_bytes = sizeof(usb_setup_packet_t) + usb_round_up_to_mps(ENUM_SHORT_DESC_REQ_LEN, ctrl_ep_mps);
        // IN data stage should return exactly ENUM_SHORT_DESC_REQ_LEN bytes
        p_enum_driver->single_thread.expect_num_bytes = sizeof(usb_setup_packet_t) + ENUM_SHORT_DESC_REQ_LEN;
        break;
    }
    case ENUM_STAGE_SET_ADDR: {
        p_enum_driver->single_thread.dev_params.new_dev_addr = get_next_free_dev_addr();
        USB_SETUP_PACKET_INIT_SET_ADDR((usb_setup_packet_t *)transfer->data_buffer, p_enum_driver->single_thread.dev_params.new_dev_addr);
        transfer->num_bytes = sizeof(usb_setup_packet_t);   // No data stage
        p_enum_driver->single_thread.expect_num_bytes = 0;   // OUT transfer. No need to check number of bytes returned
        break;
    }
    case ENUM_STAGE_GET_FULL_DEV_DESC: {
        USB_SETUP_PACKET_INIT_GET_DEVICE_DESC((usb_setup_packet_t *)transfer->data_buffer);
        transfer->num_bytes = sizeof(usb_setup_packet_t) + usb_round_up_to_mps(sizeof(usb_device_desc_t), ctrl_ep_mps);
        // IN data stage should return exactly sizeof(usb_device_desc_t) bytes
        p_enum_driver->single_thread.expect_num_bytes = sizeof(usb_setup_packet_t) + sizeof(usb_device_desc_t);
        break;
    }
    case ENUM_STAGE_GET_SHORT_CONFIG_DESC: {
        // Get a short config descriptor at descriptor index
        USB_SETUP_PACKET_INIT_GET_CONFIG_DESC((usb_setup_packet_t *)transfer->data_buffer, desc_index, ENUM_SHORT_DESC_REQ_LEN);
        transfer->num_bytes = sizeof(usb_setup_packet_t) + usb_round_up_to_mps(ENUM_SHORT_DESC_REQ_LEN, ctrl_ep_mps);
        // IN data stage should return exactly ENUM_SHORT_DESC_REQ_LEN bytes
        p_enum_driver->single_thread.expect_num_bytes = sizeof(usb_setup_packet_t) + ENUM_SHORT_DESC_REQ_LEN;
        break;
    }
    case ENUM_STAGE_GET_FULL_CONFIG_DESC: {
        // Get the full configuration descriptor at descriptor index, requesting its exact length.
        USB_SETUP_PACKET_INIT_GET_CONFIG_DESC((usb_setup_packet_t *)transfer->data_buffer, desc_index, wTotalLength);
        transfer->num_bytes = sizeof(usb_setup_packet_t) + usb_round_up_to_mps(wTotalLength, ctrl_ep_mps);
        // IN data stage should return exactly wTotalLength bytes
        p_enum_driver->single_thread.expect_num_bytes = sizeof(usb_setup_packet_t) + wTotalLength;
        break;
    }
    case ENUM_STAGE_SET_CONFIG: {
        USB_SETUP_PACKET_INIT_SET_CONFIG((usb_setup_packet_t *)transfer->data_buffer, bConfigurationValue);
        transfer->num_bytes = sizeof(usb_setup_packet_t);   // No data stage
        p_enum_driver->single_thread.expect_num_bytes = 0;    // OUT transfer. No need to check number of bytes returned
        break;
    }
    default:
        // Should never occur
        p_enum_driver->single_thread.expect_num_bytes = 0;
        abort();
        break;
    }
}

/**
 * @brief Control request: String
 *
 * Prepares the Control request string-data transfer for current stage of the enumerator
 *
 * @param[in] stage  Enumeration stage
 */
static void control_request_string(enum_stage_t stage)
{
    usb_transfer_t *transfer = &p_enum_driver->constant.urb->transfer;
    uint8_t ctrl_ep_mps = p_enum_driver->single_thread.dev_params.bMaxPacketSize0;
    uint8_t bLength = p_enum_driver->single_thread.dev_params.str_desc_bLength;
    uint8_t index = 0;
    uint16_t langid = 0;

    get_index_langid_for_stage(stage, &index, &langid);

    switch (stage) {
    case ENUM_STAGE_GET_SHORT_LANGID_TABLE:
    case ENUM_STAGE_GET_SHORT_MANU_STR_DESC:
    case ENUM_STAGE_GET_SHORT_PROD_STR_DESC:
    case ENUM_STAGE_GET_SHORT_SER_STR_DESC: {
        // Get only the header of the string descriptor
        USB_SETUP_PACKET_INIT_GET_STR_DESC((usb_setup_packet_t *)transfer->data_buffer, index, langid, sizeof(usb_str_desc_t));
        transfer->num_bytes = sizeof(usb_setup_packet_t) + usb_round_up_to_mps(sizeof(usb_str_desc_t), ctrl_ep_mps);
        // IN data stage should return exactly sizeof(usb_str_desc_t) bytes
        p_enum_driver->single_thread.expect_num_bytes = sizeof(usb_setup_packet_t) + sizeof(usb_str_desc_t);
        break;
    }
    case ENUM_STAGE_GET_FULL_LANGID_TABLE:
    case ENUM_STAGE_GET_FULL_MANU_STR_DESC:
    case ENUM_STAGE_GET_FULL_PROD_STR_DESC:
    case ENUM_STAGE_GET_FULL_SER_STR_DESC: {
        // Get the full string descriptor at a particular index, requesting the descriptors exact length
        USB_SETUP_PACKET_INIT_GET_STR_DESC((usb_setup_packet_t *)transfer->data_buffer, index, langid, bLength);
        transfer->num_bytes = sizeof(usb_setup_packet_t) + usb_round_up_to_mps(bLength, ctrl_ep_mps);
        // IN data stage should return exactly str_desc_bLength bytes
        p_enum_driver->single_thread.expect_num_bytes = sizeof(usb_setup_packet_t) + bLength;
        break;
    }
    default:
        // Should never occur
        p_enum_driver->single_thread.expect_num_bytes = 0;
        abort();
        break;
    }
}

/**
 * @brief Parse short Device descriptor
 *
 * Parses short device descriptor response
 * Configures the EP0 MPS for device object under enumeration
 */
static esp_err_t parse_short_dev_desc(void)
{
    esp_err_t ret = ESP_OK;
    usb_device_handle_t dev_hdl = p_enum_driver->single_thread.dev_hdl;
    usb_transfer_t *ctrl_xfer = &p_enum_driver->constant.urb->transfer;
    const usb_device_desc_t *dev_desc = (usb_device_desc_t *)(ctrl_xfer->data_buffer + sizeof(usb_setup_packet_t));

    // Check if the returned descriptor has correct type
    if (dev_desc->bDescriptorType != USB_B_DESCRIPTOR_TYPE_DEVICE) {
        ESP_LOGE(ENUM_TAG, "Short dev desc has wrong bDescriptorType");
        ret = ESP_ERR_INVALID_RESPONSE;
        goto exit;
    }
    // Update and save actual MPS of the default pipe
    ret = usbh_dev_set_ep0_mps(dev_hdl, dev_desc->bMaxPacketSize0);
    if (ret != ESP_OK) {
        ESP_LOGE(ENUM_TAG, "Failed to update MPS");
        goto exit;
    }
    // Save the actual MPS of EP0 in enum driver context
    p_enum_driver->single_thread.dev_params.bMaxPacketSize0 = dev_desc->bMaxPacketSize0;

exit:
    return ret;
}

static esp_err_t check_addr(void)
{
    usb_device_handle_t dev_hdl = p_enum_driver->single_thread.dev_hdl;
    uint8_t assign_addr = p_enum_driver->single_thread.dev_params.new_dev_addr;

    ESP_LOGD(ENUM_TAG, "Assign address (dev_addr=%d)", assign_addr);

    esp_err_t ret = usbh_dev_set_addr(dev_hdl, assign_addr);
    if (ret != ESP_OK) {
        ESP_LOGE(ENUM_TAG, "Error during assign device address");
    }

    return ret;
}

/**
 * @brief Parse full Device descriptor response
 *
 * Parses full device descriptor response
 * Set device descriptor for device object under enumeration
 */
static esp_err_t parse_full_dev_desc(void)
{
    esp_err_t ret = ESP_OK;

    usb_device_handle_t dev_hdl = p_enum_driver->single_thread.dev_hdl;
    usb_transfer_t *ctrl_xfer = &p_enum_driver->constant.urb->transfer;
    const usb_device_desc_t *dev_desc = (usb_device_desc_t *)(ctrl_xfer->data_buffer + sizeof(usb_setup_packet_t));

    // Check if the returned descriptor has correct type
    if (dev_desc->bDescriptorType != USB_B_DESCRIPTOR_TYPE_DEVICE) {
        ESP_LOGE(ENUM_TAG, "Full dev desc has wrong bDescriptorType");
        ret = ESP_ERR_INVALID_RESPONSE;
        goto exit;
    }
    // Save string parameters
    p_enum_driver->single_thread.dev_params.iManufacturer = dev_desc->iManufacturer;
    p_enum_driver->single_thread.dev_params.iProduct = dev_desc->iProduct;
    p_enum_driver->single_thread.dev_params.iSerialNumber = dev_desc->iSerialNumber;

    // Device has more than one configuration
    if (dev_desc->bNumConfigurations > 1) {
        ESP_LOGW(ENUM_TAG, "Device has more than 1 configuration");
    }

    // Allocate Device descriptor and set it's value to device object
    ret = usbh_dev_set_desc(dev_hdl, dev_desc);

exit:
    return ret;
}

/**
 * @brief Parse short Configuration descriptor
 *
 * Parses short Configuration descriptor response
 * Set the length to request full Configuration descriptor
 */
static esp_err_t parse_short_config_desc(void)
{
    esp_err_t ret;
    usb_transfer_t *ctrl_xfer = &p_enum_driver->constant.urb->transfer;
    const usb_config_desc_t *config_desc = (usb_config_desc_t *)(ctrl_xfer->data_buffer + sizeof(usb_setup_packet_t));

    // Check if the returned descriptor is corrupted
    if (config_desc->bDescriptorType != USB_B_DESCRIPTOR_TYPE_CONFIGURATION) {
        ESP_LOGE(ENUM_TAG, "Short config desc has wrong bDescriptorType");
        ret = ESP_ERR_INVALID_RESPONSE;
        goto exit;
    }

#if (ENUM_CTRL_TRANSFER_MAX_DATA_LEN < UINT16_MAX)  // Suppress -Wtype-limits warning due to uint16_t wTotalLength
    // Check if the descriptor is too long to be supported
    if (config_desc->wTotalLength > ENUM_CTRL_TRANSFER_MAX_DATA_LEN) {
        ESP_LOGE(ENUM_TAG, "Configuration descriptor larger than control transfer max length");
        ret = ESP_ERR_INVALID_SIZE;
        goto exit;
    }
#endif
    // Set the configuration descriptor's full length
    p_enum_driver->single_thread.dev_params.wTotalLength = config_desc->wTotalLength;
    ret = ESP_OK;

exit:
    return ret;
}

/**
 * @brief Parse full Configuration descriptor
 *
 * Parses full Configuration descriptor response
 * Set the Configuration descriptor to device object under enumeration
 */
static esp_err_t parse_full_config_desc(void)
{
    esp_err_t ret;
    usb_device_handle_t dev_hdl = p_enum_driver->single_thread.dev_hdl;
    usb_transfer_t *ctrl_xfer = &p_enum_driver->constant.urb->transfer;
    const usb_config_desc_t *config_desc = (usb_config_desc_t *)(ctrl_xfer->data_buffer + sizeof(usb_setup_packet_t));

    // Check if the returned descriptor is corrupted
    if (config_desc->bDescriptorType != USB_B_DESCRIPTOR_TYPE_CONFIGURATION) {
        ESP_LOGE(ENUM_TAG, "Full config desc has wrong bDescriptorType");
        ret = ESP_ERR_INVALID_RESPONSE;
        goto exit;
    }
    // Allocate Configuration descriptor and set it's value to device object
    ret = usbh_dev_set_config_desc(dev_hdl, config_desc);

exit:
    return ret;
}

/**
 * @brief Parse short String descriptor
 *
 * Parses short String descriptor response
 * Set the length to request full String descriptor
 */
static esp_err_t parse_short_str_desc(void)
{
    esp_err_t ret;
    usb_transfer_t *transfer = &p_enum_driver->constant.urb->transfer;
    const usb_str_desc_t *str_desc = (usb_str_desc_t *)(transfer->data_buffer + sizeof(usb_setup_packet_t));

    //Check if the returned descriptor is supported or corrupted
    if (str_desc->bDescriptorType == 0) {
        ESP_LOGE(ENUM_TAG, "String desc not supported");
        ret = ESP_ERR_NOT_SUPPORTED;
        goto exit;
    } else if (str_desc->bDescriptorType != USB_B_DESCRIPTOR_TYPE_STRING) {
        ESP_LOGE(ENUM_TAG, "Short string desc corrupt");
        ret = ESP_ERR_INVALID_RESPONSE;
        goto exit;
    }
#if (ENUM_CTRL_TRANSFER_MAX_DATA_LEN < UINT8_MAX)   //Suppress -Wtype-limits warning due to uint8_t bLength
    //Check if the descriptor is too long to be supported
    if (str_desc->bLength > (uint32_t)ENUM_CTRL_TRANSFER_MAX_DATA_LEN) {
        ESP_LOGE(ENUM_TAG, "String descriptor larger than control transfer max length");
        ret = ESP_ERR_INVALID_SIZE;
        goto exit;
    }
#endif
    // Set the descriptor's full length
    p_enum_driver->single_thread.dev_params.str_desc_bLength = str_desc->bLength;
    ret = ESP_OK;

exit:
    return ret;
}

/**
 * @brief Parse Language ID table
 *
 * Parses Language ID table response
 * Searches Language ID table for LangID = 0x0409
 */
static esp_err_t parse_langid_table(void)
{
    esp_err_t ret;
    usb_transfer_t *transfer = &p_enum_driver->constant.urb->transfer;
    const usb_str_desc_t *str_desc = (usb_str_desc_t *)(transfer->data_buffer + sizeof(usb_setup_packet_t));

    //Scan the LANGID table for our target LANGID
    ret = ESP_ERR_NOT_FOUND;
    int langid_table_num_entries = (str_desc->bLength - sizeof(usb_str_desc_t)) / 2;  // Each LANGID is 2 bytes
    for (int i = 0; i < langid_table_num_entries; i++) {  // Each LANGID is 2 bytes
        if (str_desc->wData[i] == ENUM_LANGID) {
            ret = ESP_OK;
            break;
        }
    }
    if (ret != ESP_OK) {
        ESP_LOGE(ENUM_TAG, "LANGID %#x not found", ENUM_LANGID);
    }

    return ret;
}

/**
 * @brief Get String index number
 *
 * Returns string number index (0, 1 or 2) based on stage
 */
static inline int get_str_index(enum_stage_t stage)
{
    switch (stage) {
    case ENUM_STAGE_CHECK_FULL_MANU_STR_DESC:
        return 0;
    case ENUM_STAGE_CHECK_FULL_PROD_STR_DESC:
        return 1;
    case ENUM_STAGE_CHECK_FULL_SER_STR_DESC:
        return 2;
    default:
        break;
    }
    // Should never occurred
    abort();
    return -1;
}

/**
 * @brief Parse full String descriptor
 *
 * Set String descriptor to the device object under enumeration
 */
static esp_err_t parse_full_str_desc(void)
{
    usb_transfer_t *transfer = &p_enum_driver->constant.urb->transfer;
    usb_device_handle_t dev_hdl = p_enum_driver->single_thread.dev_hdl;
    const usb_str_desc_t *str_desc = (usb_str_desc_t *)(transfer->data_buffer + sizeof(usb_setup_packet_t));

    return usbh_dev_set_str_desc(dev_hdl, str_desc, get_str_index(p_enum_driver->single_thread.stage));
}

static esp_err_t check_config(void)
{
    // Nothing to parse after a SET_CONFIG request
    return ESP_OK;
}

// -----------------------------------------------------------------------------
// ---------------------- Stage handle functions -------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Control request stage
 *
 * Based on the stage, does prepare General or String Control request
 *
 * @param[in] stage  Enumeration stage
 */
static esp_err_t control_request(enum_stage_t stage)
{
    esp_err_t ret;

    switch (stage) {
    case ENUM_STAGE_GET_SHORT_DEV_DESC:
    case ENUM_STAGE_SET_ADDR:
    case ENUM_STAGE_GET_FULL_DEV_DESC:
    case ENUM_STAGE_GET_SHORT_CONFIG_DESC:
    case ENUM_STAGE_GET_FULL_CONFIG_DESC:
    case ENUM_STAGE_SET_CONFIG:
        control_request_general(stage);
        break;
    case ENUM_STAGE_GET_SHORT_LANGID_TABLE:
    case ENUM_STAGE_GET_FULL_LANGID_TABLE:
    case ENUM_STAGE_GET_SHORT_MANU_STR_DESC:
    case ENUM_STAGE_GET_FULL_MANU_STR_DESC:
    case ENUM_STAGE_GET_SHORT_PROD_STR_DESC:
    case ENUM_STAGE_GET_FULL_PROD_STR_DESC:
    case ENUM_STAGE_GET_SHORT_SER_STR_DESC:
    case ENUM_STAGE_GET_FULL_SER_STR_DESC:
        control_request_string(stage);
        break;
    default:    // Should never occur
        ret = ESP_ERR_INVALID_STATE;
        abort();
        break;
    }

    ret = usbh_dev_submit_ctrl_urb(p_enum_driver->single_thread.dev_hdl, p_enum_driver->constant.urb);
    if (ret != ESP_OK) {
        ESP_LOGE(ENUM_TAG, "[%d:%d] Control transfer submit error (%#x), stage '%s'",
                 p_enum_driver->single_thread.parent_dev_addr,
                 p_enum_driver->single_thread.parent_port_num,
                 ret,
                 enum_stage_strings[stage]);
    }

    return ret;
}

/**
 * @brief Control request response handling stage
 *
 * Based on the stage, does parse the response data
 *
 * @param[in] stage  Enumeration stage
 */
static esp_err_t control_response_handling(enum_stage_t stage)
{
    esp_err_t ret = ESP_FAIL;
    // Check transfer status
    int expected_num_bytes = p_enum_driver->single_thread.expect_num_bytes;
    usb_transfer_t *ctrl_xfer = &p_enum_driver->constant.urb->transfer;

    if (ctrl_xfer->status != USB_TRANSFER_STATUS_COMPLETED) {
        ESP_LOGE(ENUM_TAG, "Bad transfer status %d: %s",
                 ctrl_xfer->status,
                 enum_stage_strings[stage]);
        return ret;
    }

    // Transfer completed, verbose data in ESP_LOG_VERBOSE is set
    ESP_LOG_BUFFER_HEXDUMP(ENUM_TAG, ctrl_xfer->data_buffer, ctrl_xfer->actual_num_bytes, ESP_LOG_VERBOSE);

    // Check Control IN transfer returned the expected correct number of bytes
    if (expected_num_bytes != 0 && expected_num_bytes != ctrl_xfer->actual_num_bytes) {
        ESP_LOGW(ENUM_TAG, "[%d:%d] Unexpected (%d) device response length (expected %d)",
                 p_enum_driver->single_thread.parent_dev_addr,
                 p_enum_driver->single_thread.parent_port_num,
                 ctrl_xfer->actual_num_bytes,
                 expected_num_bytes);
        if (ctrl_xfer->actual_num_bytes < expected_num_bytes) {
            // The device returned less bytes than requested. We cannot continue.
            ESP_LOGE(ENUM_TAG, "Device returned less bytes than requested");
            ret = ESP_ERR_INVALID_SIZE;
            goto exit;
        }
        // The device returned more bytes than requested.
        // This violates the USB specs chapter 9.3.5, but we can continue
    }

    switch (stage) {
    case ENUM_STAGE_CHECK_SHORT_DEV_DESC:
        ret = parse_short_dev_desc();
        break;
    case ENUM_STAGE_CHECK_ADDR:
        ret = check_addr();
        break;
    case ENUM_STAGE_CHECK_FULL_DEV_DESC:
        ret = parse_full_dev_desc();
        break;
    case ENUM_STAGE_CHECK_SHORT_CONFIG_DESC:
        ret = parse_short_config_desc();
        break;
    case ENUM_STAGE_CHECK_FULL_CONFIG_DESC:
        ret = parse_full_config_desc();
        break;
    case ENUM_STAGE_CHECK_CONFIG:
        ret = check_config();
        break;
    case ENUM_STAGE_CHECK_SHORT_LANGID_TABLE:
    case ENUM_STAGE_CHECK_SHORT_MANU_STR_DESC:
    case ENUM_STAGE_CHECK_SHORT_PROD_STR_DESC:
    case ENUM_STAGE_CHECK_SHORT_SER_STR_DESC:
        ret = parse_short_str_desc();
        break;
    case ENUM_STAGE_CHECK_FULL_LANGID_TABLE:
        ret = parse_langid_table();
        break;
    case ENUM_STAGE_CHECK_FULL_MANU_STR_DESC:
    case ENUM_STAGE_CHECK_FULL_PROD_STR_DESC:
    case ENUM_STAGE_CHECK_FULL_SER_STR_DESC:
        ret = parse_full_str_desc();
        break;
    default:
        // Should never occurred
        ret = ESP_ERR_INVALID_STATE;
        abort();
        break;
    }

exit:
    return ret;
}

/**
 * @brief Cancel stage
 *
 * Force shutdown device object under enumeration
 */
static esp_err_t stage_cancel(void)
{
    // There should be device under enumeration
    usb_device_handle_t dev_hdl = p_enum_driver->single_thread.dev_hdl;
    usb_device_handle_t parent_dev_hdl = p_enum_driver->single_thread.parent_dev_hdl;
    uint8_t parent_port_num = p_enum_driver->single_thread.parent_port_num;

    if (dev_hdl) {
        ESP_ERROR_CHECK(usbh_dev_enum_unlock(dev_hdl));
        ESP_ERROR_CHECK(usbh_dev_close(dev_hdl));
    }

    // Clean up variables device from enumerator
    p_enum_driver->single_thread.dev_uid = 0;
    p_enum_driver->single_thread.dev_hdl = NULL;
    p_enum_driver->single_thread.parent_dev_hdl = NULL;
    p_enum_driver->single_thread.parent_dev_addr = 0;
    p_enum_driver->single_thread.parent_port_num = 0;

    p_enum_driver->constant.urb->transfer.context = NULL;

    // Propagate the event
    enum_event_data_t event_data = {
        .event = ENUM_EVENT_CANCELED,
        .canceled = {
            .parent_dev_hdl = parent_dev_hdl,
            .parent_port_num = parent_port_num,
        },
    };
    p_enum_driver->constant.enum_event_cb(&event_data, p_enum_driver->constant.enum_event_cb_arg);
    return ESP_OK;
}

/**
 * @brief Complete stage
 *
 * Closes device object under enumeration
 */
static esp_err_t stage_complete(void)
{
    usb_device_handle_t dev_hdl = p_enum_driver->single_thread.dev_hdl;
    usb_device_handle_t parent_dev_hdl = p_enum_driver->single_thread.parent_dev_hdl;
    uint8_t parent_dev_addr = p_enum_driver->single_thread.parent_dev_addr;
    uint8_t parent_port_num = p_enum_driver->single_thread.parent_port_num;
    uint8_t dev_addr = 0;
    ESP_ERROR_CHECK(usbh_dev_get_addr(dev_hdl, &dev_addr));

    // Close device
    ESP_ERROR_CHECK(usbh_dev_enum_unlock(dev_hdl));
    ESP_ERROR_CHECK(usbh_dev_close(dev_hdl));

    // Release device from enumerator
    p_enum_driver->single_thread.dev_uid = 0;
    p_enum_driver->single_thread.dev_hdl = NULL;
    p_enum_driver->single_thread.parent_dev_hdl = NULL;
    p_enum_driver->single_thread.parent_dev_addr = 0;
    p_enum_driver->single_thread.parent_port_num = 0;

    // Release device from enumerator
    p_enum_driver->constant.urb->transfer.context = NULL;

    // Flush device params
    memset(&p_enum_driver->single_thread.dev_params, 0, sizeof(enum_device_params_t));
    p_enum_driver->single_thread.expect_num_bytes = 0;

    // Increase device address to use new value during the next enumeration process
    get_next_dev_addr();

    ESP_LOGD(ENUM_TAG, "[%d:%d] Processing complete, new device address %d",
             parent_dev_addr,
             parent_port_num,
             dev_addr);

    enum_event_data_t event_data = {
        .event = ENUM_EVENT_COMPLETED,
        .complete = {
            .dev_hdl = dev_hdl,
            .dev_addr = dev_addr,
            .parent_dev_hdl = parent_dev_hdl,
            .parent_port_num = parent_port_num,
        },
    };
    p_enum_driver->constant.enum_event_cb(&event_data, p_enum_driver->constant.enum_event_cb_arg);
    return ESP_OK;
}

// -----------------------------------------------------------------------------
// -------------------------- State Machine ------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Returns the process requirement flag for the stage
 *
 * When stage doesn't have a control transfer callback which request processing, request process should be triggered immediately
 *
 * @param[in] stage  Enumeration process stage
 * @return Processing for the stage is:
 * @retval true Required
 * @retval false Not required
 */
static bool stage_need_process(enum_stage_t stage)
{
    bool need_process_cb = false;

    switch (stage) {
    // Transfer submission stages
    // Stages have transfer completion callback which will re-trigger the processing
    case ENUM_STAGE_GET_SHORT_DEV_DESC:
    case ENUM_STAGE_SET_ADDR:
    case ENUM_STAGE_GET_FULL_DEV_DESC:
    case ENUM_STAGE_GET_SHORT_CONFIG_DESC:
    case ENUM_STAGE_GET_FULL_CONFIG_DESC:
    case ENUM_STAGE_SET_CONFIG:
    case ENUM_STAGE_GET_SHORT_LANGID_TABLE:
    case ENUM_STAGE_GET_FULL_LANGID_TABLE:
    case ENUM_STAGE_GET_SHORT_MANU_STR_DESC:
    case ENUM_STAGE_GET_FULL_MANU_STR_DESC:
    case ENUM_STAGE_GET_SHORT_PROD_STR_DESC:
    case ENUM_STAGE_GET_FULL_PROD_STR_DESC:
    case ENUM_STAGE_GET_SHORT_SER_STR_DESC:
    case ENUM_STAGE_GET_FULL_SER_STR_DESC:
    // Other stages
    // Stages, require the re-triggering the processing
    case ENUM_STAGE_SECOND_RESET:
    case ENUM_STAGE_SET_ADDR_RECOVERY:
    case ENUM_STAGE_SELECT_CONFIG:
    case ENUM_STAGE_COMPLETE:
    case ENUM_STAGE_CANCEL:
        need_process_cb = true;
        break;
    default:
        break;
    }

    return need_process_cb;
}

/**
 * @brief Set next stage
 *
 * Does set next stage, based on the successful completion of last stage
 * Some stages (i.e., string descriptors) are skipped if the device doesn't support them
 * Some stages (i.e. string descriptors) are allowed to fail
 *
 * @param[in] last_stage_pass  Flag of successful completion last stage
 *
 * @return Processing for the next stage is:
 * @retval true     Required
 * @retval false    Not required
 */
static bool set_next_stage(bool last_stage_pass)
{
    enum_stage_t last_stage = p_enum_driver->single_thread.stage;
    enum_stage_t next_stage;

    while (1) {
        bool stage_skip = false;

        // Find the next stage
        if (last_stage_pass) {
            // Last stage was successful
            ESP_LOGD(ENUM_TAG, "[%d:%d] %s OK",
                     p_enum_driver->single_thread.parent_dev_addr,
                     p_enum_driver->single_thread.parent_port_num,
                     enum_stage_strings[last_stage]);
            // Get next stage
            if (last_stage == ENUM_STAGE_COMPLETE ||
                    last_stage == ENUM_STAGE_CANCEL) {
                // Stages are terminal, move state machine to IDLE
                next_stage = ENUM_STAGE_IDLE;
            } else {
                // Simply increment to get the next stage
                next_stage = last_stage + 1;
            }
        } else {
            ESP_LOGE(ENUM_TAG, "[%d:%d] %s FAILED",
                     p_enum_driver->single_thread.parent_dev_addr,
                     p_enum_driver->single_thread.parent_port_num,
                     enum_stage_strings[last_stage]);
            // These stages cannot fail
            assert(last_stage != ENUM_STAGE_SET_ADDR_RECOVERY &&
                   last_stage != ENUM_STAGE_SELECT_CONFIG &&
                   last_stage != ENUM_STAGE_SECOND_RESET &&
                   last_stage != ENUM_STAGE_SECOND_RESET_COMPLETE &&
                   last_stage != ENUM_STAGE_COMPLETE &&
                   last_stage != ENUM_STAGE_CANCEL);

            // Last stage failed
            switch (last_stage) {
            // Stages that are allowed to fail skip to the next appropriate stage
            case ENUM_STAGE_CHECK_SHORT_LANGID_TABLE:
            case ENUM_STAGE_CHECK_FULL_LANGID_TABLE:
            // Couldn't get LANGID, skip the rest of the string descriptors
            case ENUM_STAGE_CHECK_SHORT_SER_STR_DESC:
            case ENUM_STAGE_CHECK_FULL_SER_STR_DESC:
                // iSerialNumber string failed. Jump to Set Configuration and complete enumeration process.
                next_stage = ENUM_STAGE_SET_CONFIG;
                break;
            case ENUM_STAGE_CHECK_SHORT_MANU_STR_DESC:
            case ENUM_STAGE_CHECK_FULL_MANU_STR_DESC:
                // iManufacturer string failed. Get iProduct string next
                next_stage = ENUM_STAGE_GET_SHORT_PROD_STR_DESC;
                break;
            case ENUM_STAGE_CHECK_SHORT_PROD_STR_DESC:
            case ENUM_STAGE_CHECK_FULL_PROD_STR_DESC:
                // iProduct string failed. Get iSerialNumber string next
                next_stage = ENUM_STAGE_GET_SHORT_SER_STR_DESC;
                break;
            case ENUM_STAGE_COMPLETE:
            case ENUM_STAGE_CANCEL:
                // These stages should never fail
                abort();
                break;
            default:
                // Stage is not allowed to failed. Cancel enumeration.
                next_stage = ENUM_STAGE_CANCEL;
                break;
            }
        }

        // Check if the next stage should be skipped
        switch (next_stage) {
        case ENUM_STAGE_GET_SHORT_MANU_STR_DESC:
        case ENUM_STAGE_CHECK_SHORT_MANU_STR_DESC:
        case ENUM_STAGE_GET_FULL_MANU_STR_DESC:
        case ENUM_STAGE_CHECK_FULL_MANU_STR_DESC:
            // Device doesn't support iManufacturer string
            if (p_enum_driver->single_thread.dev_params.iManufacturer == 0) {
                ESP_LOGD(ENUM_TAG, "String iManufacturer not set, skip");
                stage_skip = true;
            }
            break;
        case ENUM_STAGE_GET_SHORT_PROD_STR_DESC:
        case ENUM_STAGE_CHECK_SHORT_PROD_STR_DESC:
        case ENUM_STAGE_GET_FULL_PROD_STR_DESC:
        case ENUM_STAGE_CHECK_FULL_PROD_STR_DESC:
            // Device doesn't support iProduct string
            if (p_enum_driver->single_thread.dev_params.iProduct == 0) {
                ESP_LOGD(ENUM_TAG, "String iProduct not set, skip");
                stage_skip = true;
            }
            break;
        case ENUM_STAGE_GET_SHORT_SER_STR_DESC:
        case ENUM_STAGE_CHECK_SHORT_SER_STR_DESC:
        case ENUM_STAGE_GET_FULL_SER_STR_DESC:
        case ENUM_STAGE_CHECK_FULL_SER_STR_DESC:
            // Device doesn't support iSerialNumber string
            if (p_enum_driver->single_thread.dev_params.iSerialNumber == 0) {
                ESP_LOGD(ENUM_TAG, "String iSerialNumber not set, skip");
                stage_skip = true;
            }
            break;
        default:
            break;
        }

        if (stage_skip) {
            // Loop back around to get the next stage again
            last_stage = next_stage;
        } else {
            break;
        }
    }
    p_enum_driver->single_thread.stage = next_stage;
    return stage_need_process(next_stage);
}

/**
 * @brief Control transfer completion callback
 *
 * Is called by lower logic when transfer is completed with or without error
 *
 * @param[in] ctrl_xfer Pointer to a transfer buffer
 */
static void enum_control_transfer_complete(usb_transfer_t *ctrl_xfer)
{
    // Sanity checks
    assert(ctrl_xfer);
    assert(ctrl_xfer->context);
    assert(p_enum_driver->single_thread.dev_hdl == ctrl_xfer->context);

    // Request processing
    p_enum_driver->constant.proc_req_cb(USB_PROC_REQ_SOURCE_ENUM, false, p_enum_driver->constant.proc_req_cb_arg);
}

// -----------------------------------------------------------------------------
// -------------------------- Public API ---------------------------------------
// -----------------------------------------------------------------------------

esp_err_t enum_install(enum_config_t *config, void **client_ret)
{
    ENUM_CHECK(p_enum_driver == NULL, ESP_ERR_INVALID_STATE);
    ENUM_CHECK(config != NULL, ESP_ERR_INVALID_ARG);

    esp_err_t ret;
    enum_driver_t *enum_drv = heap_caps_calloc(1, sizeof(enum_driver_t), MALLOC_CAP_DEFAULT);
    ENUM_CHECK(enum_drv, ESP_ERR_NO_MEM);

    // Initialize ENUM objects
    urb_t *urb = urb_alloc(sizeof(usb_setup_packet_t) + ENUM_CTRL_TRANSFER_MAX_DATA_LEN, 0);
    if (urb == NULL) {
        ret = ESP_ERR_NOT_FINISHED;
        goto alloc_err;
    }

    // Setup urb
    urb->usb_host_client = (void *) enum_drv;   // Client is an address of the enum driver object
    urb->transfer.callback = enum_control_transfer_complete;
    enum_drv->constant.urb = urb;
    // Save callbacks
    enum_drv->constant.proc_req_cb = config->proc_req_cb;
    enum_drv->constant.proc_req_cb_arg = config->proc_req_cb_arg;
    enum_drv->constant.enum_event_cb = config->enum_event_cb;
    enum_drv->constant.enum_event_cb_arg = config->enum_event_cb_arg;
#if ENABLE_ENUM_FILTER_CALLBACK
    enum_drv->constant.enum_filter_cb = config->enum_filter_cb;
    enum_drv->constant.enum_filter_cb_arg = config->enum_filter_cb_arg;
#endif // ENABLE_ENUM_FILTER_CALLBACK

    enum_drv->single_thread.next_dev_addr = ENUM_INIT_VALUE_DEV_ADDR;
    enum_drv->single_thread.stage = ENUM_STAGE_IDLE;

    // Enumeration driver is single_threaded
    if (p_enum_driver != NULL) {
        ret = ESP_ERR_NOT_FINISHED;
        goto err;
    }
    p_enum_driver = enum_drv;
    // Write-back client_ret pointer
    *client_ret = (void *)enum_drv;
    return ESP_OK;

err:
    urb_free(urb);
alloc_err:
    heap_caps_free(enum_drv);
    return ret;
}

esp_err_t enum_uninstall(void)
{
    ENUM_CHECK(p_enum_driver != NULL, ESP_ERR_INVALID_STATE);
    // Enumeration driver is single_threaded
    enum_driver_t *enum_drv = p_enum_driver;
    p_enum_driver = NULL;
    // Free resources
    urb_free(enum_drv->constant.urb);
    heap_caps_free(enum_drv);
    return ESP_OK;
}

esp_err_t enum_start(unsigned int uid)
{
    ENUM_CHECK(p_enum_driver != NULL, ESP_ERR_INVALID_STATE);

    esp_err_t ret = ESP_FAIL;

    // Open device and lock it for enumeration process
    usb_device_handle_t dev_hdl;
    ret = usbh_devs_open(0, &dev_hdl);
    if (ret != ESP_OK) {
        return ret;
    }
    ESP_ERROR_CHECK(usbh_dev_enum_lock(dev_hdl));

    // Get device info
    usb_device_info_t dev_info;
    uint8_t parent_dev_addr = 0;
    ESP_ERROR_CHECK(usbh_dev_get_info(dev_hdl, &dev_info));

    if (dev_info.parent.dev_hdl) {
        ESP_ERROR_CHECK(usbh_dev_get_addr(dev_info.parent.dev_hdl, &parent_dev_addr));
    }

    // Stage ENUM_STAGE_GET_SHORT_DEV_DESC
    ESP_LOGD(ENUM_TAG, "[%d:%d] Start processing, device address %d",
             parent_dev_addr,
             dev_info.parent.port_num,
             0);

    p_enum_driver->single_thread.stage = ENUM_STAGE_GET_SHORT_DEV_DESC;
    p_enum_driver->single_thread.dev_uid = uid;
    p_enum_driver->single_thread.dev_hdl = dev_hdl;
    p_enum_driver->single_thread.parent_dev_hdl = dev_info.parent.dev_hdl;
    p_enum_driver->single_thread.parent_dev_addr = parent_dev_addr;
    p_enum_driver->single_thread.parent_port_num = dev_info.parent.port_num;
    // Save device handle to the URB transfer context
    p_enum_driver->constant.urb->transfer.context = (void *) dev_hdl;
    // Device params
    memset(&p_enum_driver->single_thread.dev_params, 0, sizeof(enum_device_params_t));
    p_enum_driver->single_thread.dev_params.bMaxPacketSize0 = (dev_info.speed == USB_SPEED_LOW)
                                                              ? ENUM_WORST_CASE_MPS_LS
                                                              : ENUM_WORST_CASE_MPS_FS_HS;

    // Notify USB Host about starting enumeration process
    enum_event_data_t event_data = {
        .event = ENUM_EVENT_STARTED,
        .started = {
            .uid = uid,
            .parent_dev_hdl = dev_info.parent.dev_hdl,
            .parent_port_num = dev_info.parent.port_num,
        },
    };
    p_enum_driver->constant.enum_event_cb(&event_data, p_enum_driver->constant.enum_event_cb_arg);

    // Request processing
    p_enum_driver->constant.proc_req_cb(USB_PROC_REQ_SOURCE_ENUM, false, p_enum_driver->constant.proc_req_cb_arg);
    return ret;
}

esp_err_t enum_proceed(unsigned int uid)
{
    ENUM_CHECK(p_enum_driver != NULL, ESP_ERR_INVALID_STATE);
    // Request processing
    p_enum_driver->constant.proc_req_cb(USB_PROC_REQ_SOURCE_ENUM, false, p_enum_driver->constant.proc_req_cb_arg);
    return ESP_OK;
}

esp_err_t enum_cancel(unsigned int uid)
{
    ENUM_CHECK(p_enum_driver != NULL, ESP_ERR_INVALID_STATE);

    enum_stage_t old_stage = p_enum_driver->single_thread.stage;

    if (old_stage == ENUM_STAGE_IDLE ||
            old_stage == ENUM_STAGE_CANCEL) {
        // Nothing to cancel
        return ESP_OK;
    }

    p_enum_driver->single_thread.stage = ENUM_STAGE_CANCEL;

    ESP_LOGV(ENUM_TAG, "[%d:%d] Cancel at %s",
             p_enum_driver->single_thread.parent_dev_addr,
             p_enum_driver->single_thread.parent_port_num,
             enum_stage_strings[old_stage]);

    if (stage_need_process(old_stage)) {
        // These stages are required to trigger processing in the enum_process()
        // This means, that there is no ongoing transfer and we can release the
        // device from enumeration immediately
        usb_device_handle_t dev_hdl = p_enum_driver->single_thread.dev_hdl;
        if (dev_hdl) {
            // Close the device
            ESP_ERROR_CHECK(usbh_dev_enum_unlock(dev_hdl));
            ESP_ERROR_CHECK(usbh_dev_close(dev_hdl));
            p_enum_driver->single_thread.dev_hdl = NULL;
        }
    }

    // SECOND_RESET_COMPLETE is the exceptional stage, as it awaits the notification after port reset completion via the enum_proceed() call.
    // Meanwhile, the device could be detached during the reset, thus the device disconnect comes instead of reset completion.
    if (old_stage == ENUM_STAGE_SECOND_RESET_COMPLETE) {
        p_enum_driver->constant.proc_req_cb(USB_PROC_REQ_SOURCE_ENUM, false, p_enum_driver->constant.proc_req_cb_arg);
    }

    return ESP_OK;
}

esp_err_t enum_process(void)
{
    ENUM_CHECK(p_enum_driver != NULL, ESP_ERR_INVALID_STATE);

    esp_err_t res = ESP_FAIL;
    enum_stage_t stage = p_enum_driver->single_thread.stage;

    switch (stage) {
    // Transfer submission stages
    case ENUM_STAGE_GET_SHORT_DEV_DESC:
    case ENUM_STAGE_SET_ADDR:
    case ENUM_STAGE_GET_FULL_DEV_DESC:
    case ENUM_STAGE_GET_SHORT_CONFIG_DESC:
    case ENUM_STAGE_GET_FULL_CONFIG_DESC:
    case ENUM_STAGE_SET_CONFIG:
    case ENUM_STAGE_GET_SHORT_LANGID_TABLE:
    case ENUM_STAGE_GET_FULL_LANGID_TABLE:
    case ENUM_STAGE_GET_SHORT_MANU_STR_DESC:
    case ENUM_STAGE_GET_FULL_MANU_STR_DESC:
    case ENUM_STAGE_GET_SHORT_PROD_STR_DESC:
    case ENUM_STAGE_GET_FULL_PROD_STR_DESC:
    case ENUM_STAGE_GET_SHORT_SER_STR_DESC:
    case ENUM_STAGE_GET_FULL_SER_STR_DESC:
        res = control_request(stage);
        break;
    // Recovery interval
    case ENUM_STAGE_SET_ADDR_RECOVERY:
        // Need a short delay before device is ready. Todo: IDF-7007
        vTaskDelay(pdMS_TO_TICKS(SET_ADDR_RECOVERY_INTERVAL_MS));
        res = ESP_OK;
        break;
    // Transfer check stages
    case ENUM_STAGE_CHECK_SHORT_DEV_DESC:
    case ENUM_STAGE_CHECK_ADDR:
    case ENUM_STAGE_CHECK_FULL_DEV_DESC:
    case ENUM_STAGE_CHECK_SHORT_CONFIG_DESC:
    case ENUM_STAGE_CHECK_FULL_CONFIG_DESC:
    case ENUM_STAGE_CHECK_CONFIG:
    case ENUM_STAGE_CHECK_SHORT_LANGID_TABLE:
    case ENUM_STAGE_CHECK_FULL_LANGID_TABLE:
    case ENUM_STAGE_CHECK_SHORT_MANU_STR_DESC:
    case ENUM_STAGE_CHECK_FULL_MANU_STR_DESC:
    case ENUM_STAGE_CHECK_SHORT_PROD_STR_DESC:
    case ENUM_STAGE_CHECK_FULL_PROD_STR_DESC:
    case ENUM_STAGE_CHECK_SHORT_SER_STR_DESC:
    case ENUM_STAGE_CHECK_FULL_SER_STR_DESC:
        res = control_response_handling(stage);
        break;
    case ENUM_STAGE_SELECT_CONFIG:
        res = select_active_configuration();
        break;
    case ENUM_STAGE_SECOND_RESET:
        // We need to wait Hub driver to finish port reset
        res = second_reset_request();
        break;
    case ENUM_STAGE_SECOND_RESET_COMPLETE:
        // Second reset complete
        res = ESP_OK;
        break;
    case ENUM_STAGE_CANCEL:
        res = stage_cancel();
        break;
    case ENUM_STAGE_COMPLETE:
        res = stage_complete();
        break;
    default:
        // Should never occur
        abort();
        break;
    }

    // Set nest stage of enumeration process, based on the stage pass result
    if (set_next_stage(res == ESP_OK)) {
        p_enum_driver->constant.proc_req_cb(USB_PROC_REQ_SOURCE_ENUM, false, p_enum_driver->constant.proc_req_cb_arg);
    }

    return ESP_OK;
}
