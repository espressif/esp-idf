/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "usb/usb_host.h"

#include "hid_host.h"

// HID spinlock
static portMUX_TYPE hid_lock = portMUX_INITIALIZER_UNLOCKED;
#define HID_ENTER_CRITICAL()    portENTER_CRITICAL(&hid_lock)
#define HID_EXIT_CRITICAL()     portEXIT_CRITICAL(&hid_lock)

// HID verification macroses
#define HID_GOTO_ON_FALSE_CRITICAL(exp, err)    \
    do {                                        \
        if(!(exp)) {                            \
            HID_EXIT_CRITICAL();                \
            ret = err;                          \
            goto fail;                          \
        }                                       \
    } while(0)

#define HID_RETURN_ON_FALSE_CRITICAL(exp, err)  \
    do {                                        \
        if(!(exp)) {                            \
            HID_EXIT_CRITICAL();                \
            return err;                         \
        }                                       \
    } while(0)

#define HID_GOTO_ON_ERROR(exp) ESP_GOTO_ON_ERROR(exp, fail, TAG, "")

#define HID_GOTO_ON_FALSE(exp, err) ESP_GOTO_ON_FALSE( (exp), err, fail, TAG, "" )

#define HID_RETURN_ON_ERROR(exp) ESP_RETURN_ON_ERROR((exp), TAG, "")

#define HID_RETURN_ON_FALSE(exp, err) ESP_RETURN_ON_FALSE( (exp), (err), TAG, "")

#define HID_RETURN_ON_INVALID_ARG(exp) ESP_RETURN_ON_FALSE((exp) != NULL, ESP_ERR_INVALID_ARG, TAG, "")

static const char *TAG = "hid-host";

/**
 * @brief HID Interface structure in device to interact with. After HID device opening keeps the interface configuration
 *
 */
typedef struct hid_interface {
    STAILQ_ENTRY(hid_interface) tailq_entry;
    uint8_t dev_addr;                       /**< USB device address */
    hid_protocol_t proto;                   /**< HID Interface protocol */
    hid_host_device_handle_t dev;           /**< Parent USB HID device */
    uint8_t num;                            /**< Interface number */
    uint8_t ep;                             /**< Interrupt IN EP number */
    uint16_t ep_mps;                        /**< Interrupt IN max size */
    uint8_t country_code;                   /**< Country code */
    uint16_t report_size;                   /**< Size of Report */
    uint8_t *report;                        /**< Pointer to HID Report */
    usb_transfer_t *in_xfer;                /**< Pointer to IN transfer buffer */
    hid_input_report_cb_t report_cb;        /**< Input report callback */
    hid_host_interface_event_cb_t user_cb;  /**< Interface application callback */
    void *user_cb_arg;                      /**< Interface application callback arg */
    bool ready;                             /**< Interface ready for claiming */
} hid_iface_t;

/**
 * @brief HID Device structure.
 *
 */
typedef struct hid_host_device {
    STAILQ_ENTRY(hid_host_device) tailq_entry;  /**< HID device queue */
    SemaphoreHandle_t device_busy;              /**< HID device main mutex */
    SemaphoreHandle_t ctrl_xfer_done;           /**< Control transfer semaphore */
    usb_transfer_t *ctrl_xfer;                  /**< Pointer to control transfer buffer */
    usb_device_handle_t dev_hdl;                /**< USB device handle */
} hid_device_t;

/**
 * @brief HID driver default context
 *
 * This context is created during HID Host install.
 */
typedef struct {
    usb_host_client_handle_t client_handle;     /**< Client task handle */
    hid_host_event_cb_t user_cb;                /**< User application callback */
    void *user_arg;                             /**< User application callback args */
    SemaphoreHandle_t all_events_handled;       /**< Events handler semaphore */
    volatile bool end_client_event_handling;    /**< Client event handling flag */
} hid_driver_t;

static hid_driver_t *s_hid_driver;              /**< Internal pointer to HID driver */

STAILQ_HEAD(devices, hid_host_device) devices_tailq;
STAILQ_HEAD(interfaces, hid_interface) ifaces_tailq;

// ----------------------- Private -------------------------

/**
 * @brief Get Next Interface descriptor
 *
 * @param[in] desc   Pointer to Descriptor buffer (usually starts from Configuration descriptor)
 * @param[in] len    Total length
 * @param[in] offset Pointer to offset descriptor buffer
 * @return const usb_standard_desc_t* Pointer to Interface descriptor
 */
static inline const usb_standard_desc_t *next_interface_desc(const usb_standard_desc_t *desc, size_t len, size_t *offset)
{
    return usb_parse_next_descriptor_of_type(desc, len, USB_W_VALUE_DT_INTERFACE, (int *)offset);
}

/**
 * @brief Get Next HID descriptor
 *
 * @param[in] desc   Pointer to the Descriptor buffer
 * @param[in] len    Total length
 * @param[in] offset Pointer to offset in descriptor buffer
 * @return const usb_standard_desc_t* Pointer to HID descriptor
 */
static inline const usb_standard_desc_t *next_hid_descriptor(const usb_standard_desc_t *desc, size_t len, size_t *offset)
{
    return usb_parse_next_descriptor_of_type(desc, len, HID_CLASS_DESCRIPTOR_TYPE_HID, (int *)offset);
}

/**
 * @brief Get Next Endpoint descriptor
 *
 * @param[in] desc   Pointer to Descriptor buffer
 * @param[in] len    Total length
 * @param[in] offset Pointer to offset in descriptor buffer
 * @return const usb_standard_desc_t* Pointer to Endpoint descriptor
 */
static inline const usb_standard_desc_t *next_endpoint_desc(const usb_standard_desc_t *desc, size_t len, size_t *offset)
{
    return usb_parse_next_descriptor_of_type(desc, len, USB_B_DESCRIPTOR_TYPE_ENDPOINT, (int *)offset);
}

/**
 * @brief USB Event handler
 *
 * Handle all USB related events such as USB host (usbh) events or hub events from USB hardware
 *
 * @param[in] arg   Argument, does not used
 */
static void event_handler_task(void *arg)
{
    while (1) {
        /* Here wee need a timeout 50 ms to handle end_client_event_handling flag
         * during situation when all devices were removed and it is time to remove
         * and destroy everything.
         */
        usb_host_client_handle_events(s_hid_driver->client_handle, pdMS_TO_TICKS(50));

        if (s_hid_driver->end_client_event_handling) {
            break;
        }
    }
    usb_host_client_unblock(s_hid_driver->client_handle);
    ESP_ERROR_CHECK( usb_host_client_deregister(s_hid_driver->client_handle) );
    xSemaphoreGive(s_hid_driver->all_events_handled);
    vTaskDelete(NULL);
}

/**
 * @brief Return HID device in devices list by USB device handle
 *
 * @param[in] usb_device_handle_t   USB device handle
 * @return hid_device_t Pointer to device, NULL if device not present
 */
static hid_device_t *get_hid_device_by_handle(usb_device_handle_t usb_handle)
{
    hid_host_device_handle_t device = NULL;

    HID_ENTER_CRITICAL();
    STAILQ_FOREACH(device, &devices_tailq, tailq_entry) {
        if (usb_handle == device->dev_hdl) {
            HID_EXIT_CRITICAL();
            return device;
        }
    }
    HID_EXIT_CRITICAL();
    return NULL;
}

/**
 * @brief Get HID Interface pointer by Endpoint address
 *
 * @param[in] hid_device   Pointer to HID device structure
 * @param[in] ep_addr      Endpoint address
 * @return hid_iface_t     Pointer to HID Interface configuration structure
 */
static hid_iface_t *get_interface_by_ep(hid_device_t *hid_device, uint8_t ep_addr)
{
    hid_iface_t *interface = NULL;

    HID_ENTER_CRITICAL();
    STAILQ_FOREACH(interface, &ifaces_tailq, tailq_entry) {
        if (ep_addr == interface->ep) {
            HID_EXIT_CRITICAL();
            return interface;
        }
    }

    HID_EXIT_CRITICAL();
    return NULL;
}

/**
 * @brief Get Interface structure from the RAM list
 *
 * @param[in] proto  HID Protocol
 * @return hid_iface_t  Pointer to an interface structure
 */
static hid_iface_t *get_interface_by_proto(hid_protocol_t proto)
{
    hid_iface_t *interface = NULL;

    HID_ENTER_CRITICAL();
    STAILQ_FOREACH(interface, &ifaces_tailq, tailq_entry) {
        if (proto == interface->proto) {
            HID_EXIT_CRITICAL();
            return interface;
        }
    }

    HID_EXIT_CRITICAL();
    return NULL;
}

/**
 * @brief Verify claim/release status of the particular interface
 *
 * @param[in] proto         HID Interface protocol
 * @return true             Interface is claimed by application logic
 * @return false            Interface is released
 */
static bool is_interface_claimed(hid_protocol_t proto)
{
    hid_iface_t *interface = get_interface_by_proto(proto);

    if (interface) {
        return (interface->report_cb != NULL);
    }

    return false;
}

/**
 * @brief Get Interface Descriptor
 *
 * @param[in] config_desc   Pointer to Configuration Descriptor
 * @param[in] offset        Offset
 * @param[in] proto         Interface Protocol field to search, or set to `HID_PROTOCOL_NONE` to ignore
 * @return esp_err_t
 */
static const usb_intf_desc_t *get_interface_desc_by_proto(const usb_config_desc_t *config_desc,
        size_t *offset,
        hid_protocol_t proto)
{
    size_t total_length = config_desc->wTotalLength;
    const usb_standard_desc_t *next_desc = (const usb_standard_desc_t *)config_desc;

    next_desc = next_interface_desc(next_desc, total_length, offset);

    while ( next_desc ) {

        const usb_intf_desc_t *ifc_desc = (const usb_intf_desc_t *)next_desc;

        if ( ifc_desc->bInterfaceClass == USB_CLASS_HID  &&
                ifc_desc->bInterfaceSubClass == HID_SUBCLASS_BOOT_INTERFACE) {

            if (proto != HID_PROTOCOL_NONE) {
                if (ifc_desc->bInterfaceProtocol == proto) {
                    return ifc_desc;
                }
            } else {
                return ifc_desc;
            }

        }

        next_desc = next_interface_desc(next_desc, total_length, offset);
    };
    return NULL;
}

/**
 * @brief Interface user callback function.
 *
 * @param[in] iface   Pointer to Interface structure
 * @param[in] event   HID Interface event
 */
static void interface_user_callback(hid_iface_t *iface, hid_interface_event_t event)
{
    if (!iface->user_cb) {
        return;
    }

    const hid_host_interface_event_t iface_event = {
        .event = event,
        .interface.dev_addr = iface->dev_addr,
        .interface.num = iface->num,
        .interface.proto = iface->proto
    };

    iface->user_cb(&iface_event, iface->user_cb_arg);
}

/**
 * @brief Add interface in a list
 *
 * @param[in] dev_config  Pointer to device config structure
 * @param[in] hid_device  Pointer to HID device structure
 * @param[in] iface_desc  Pointer to an Interface descriptor
 * @param[in] hid_desc    Pointer to an HID device descriptor
 * @param[in] ep_desc     Pointer to an EP descriptor
 * @return esp_err_t
 */
static esp_err_t add_interface_to_list(const hid_host_device_config_t *dev_config,
                                       hid_device_t *hid_device,
                                       const usb_intf_desc_t *iface_desc,
                                       const hid_descriptor_t *hid_desc,
                                       const usb_ep_desc_t *ep_desc)
{
    hid_iface_t *iface = calloc(1, sizeof(hid_iface_t));

    HID_RETURN_ON_FALSE(iface, ESP_ERR_NO_MEM);

    iface->dev_addr = dev_config->dev_addr;

    if (iface_desc) {
        iface->proto = iface_desc->bInterfaceProtocol;
        iface->num = iface_desc->bInterfaceNumber;
    }

    if (hid_desc) {
        iface->country_code = hid_desc->bCountryCode;
        iface->report_size = hid_desc->wReportDescriptorLength;
    }

    // EP IN && INT Type
    if (ep_desc) {
        if ( (ep_desc->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) &&
                (ep_desc->bmAttributes & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK) ) {
            iface->ep = ep_desc->bEndpointAddress;
            iface->ep_mps = USB_EP_DESC_GET_MPS(ep_desc);
        } else {
            ESP_LOGE(TAG, "HID device does not have INT EP");
        }
    }

    if (iface_desc && hid_desc && ep_desc) {
        iface->dev = hid_device;
        iface->ready = true;
    }

    iface->user_cb = dev_config->iface_event_cb;
    iface->user_cb_arg = dev_config->iface_event_arg;

    HID_ENTER_CRITICAL();
    STAILQ_INSERT_TAIL(&ifaces_tailq, iface, tailq_entry);
    HID_EXIT_CRITICAL();

    interface_user_callback(iface, HID_DEVICE_INTERFACE_INIT);

    return ESP_OK;
}

/**
 * @brief Search HID class descriptor in USB device
 *
 * @param[in] dev_addr   USB device physical address
 * @return true USB device contain HID Interface
 * @return false USB does not contain HID Interface
 */
static bool is_hid_device(uint8_t dev_addr)
{
    bool is_hid_device = false;
    usb_device_handle_t device;
    const usb_config_desc_t *config_desc;
    size_t offset = 0;

    if ( usb_host_device_open(s_hid_driver->client_handle, dev_addr, &device) == ESP_OK) {
        if ( usb_host_get_active_config_descriptor(device, &config_desc) == ESP_OK ) {
            if (get_interface_desc_by_proto(config_desc, &offset, HID_PROTOCOL_NONE)) {
                is_hid_device = true;
            }
        }
        usb_host_device_close(s_hid_driver->client_handle, device);
    }

    if (is_hid_device) {
        ESP_LOGD(TAG, "Found HID Interface, USB addr %d", dev_addr);
    } else {
        ESP_LOGW(TAG, "Device at addr %d has no HID Interface", dev_addr);
    }

    return is_hid_device;
}

/**
 * @brief Create a list of available interfaces in RAM
 *
 * @param[in] hid_device  Pointer to HID device structure
 * @param[in] dev_addr    USB device physical address
 * @param[in] sub_class   USB HID SubClass value
 * @return esp_err_t
 */
static esp_err_t create_interface_list(hid_device_t *hid_device,
                                       const hid_host_device_config_t *dev_config,
                                       uint8_t sub_class)
{
    const usb_config_desc_t *config_desc = NULL;
    const usb_intf_desc_t *iface_desc = NULL;
    const hid_descriptor_t *hid_desc = NULL;
    const usb_ep_desc_t *ep_desc = NULL;
    size_t offset = 0;

    HID_RETURN_ON_ERROR( usb_host_get_active_config_descriptor(hid_device->dev_hdl, &config_desc));

    const usb_standard_desc_t *next_desc = (const usb_standard_desc_t *)config_desc;
    size_t total_length = config_desc->wTotalLength;

    next_desc = next_interface_desc(next_desc, total_length, &offset);

    while ( next_desc ) {
        iface_desc = (const usb_intf_desc_t *)next_desc;
        hid_desc = NULL;
        ep_desc = NULL;

        if ( iface_desc->bInterfaceClass == USB_CLASS_HID &&
                iface_desc->bInterfaceSubClass == sub_class) {
            // HID descriptor (extract Report size)
            next_desc = next_hid_descriptor(next_desc, total_length, &offset);
            if (next_desc) {
                hid_desc = (const hid_descriptor_t *)next_desc;
            }
            // HID mouse and keyboard contain one INT descriptor
            next_desc = next_endpoint_desc(next_desc, total_length, &offset);
            if (next_desc) {
                ep_desc = (const usb_ep_desc_t *)next_desc;
            }

            HID_RETURN_ON_ERROR( add_interface_to_list(dev_config,
                                 hid_device,
                                 iface_desc,
                                 hid_desc,
                                 ep_desc) );
        }
        next_desc = next_interface_desc(next_desc, total_length, &offset);
    };

    return ESP_OK;
}

/**
 * @brief Destroy a list of available interfaces in RAM
 *
 * @return esp_err_t
 */
static esp_err_t destroy_interface_list(void)
{
    hid_iface_t *interface = NULL;

    for (int proto = HID_PROTOCOL_KEYBOARD; proto < HID_PROTOCOL_MAX; proto++) {

        interface = get_interface_by_proto(proto);

        if (interface) {
            HID_ENTER_CRITICAL();
            interface->ready = false;
            STAILQ_REMOVE(&ifaces_tailq, interface, hid_interface, tailq_entry);
            HID_EXIT_CRITICAL();
            free(interface);
        }
    }

    return ESP_OK;
}

/**
 * @brief USB Host Client's event callback
 *
 * @param[in] event    Client event message
 * @param[in] arg      Argument, does not used
 */
static void client_event_cb(const usb_host_client_event_msg_t *event, void *arg)
{
    if (event->event == USB_HOST_CLIENT_EVENT_NEW_DEV) {
        if (is_hid_device(event->new_dev.address)) {
            const hid_host_event_t hid_event = {
                .event = HID_DEVICE_CONNECTED,
                .device.address = event->new_dev.address,
            };
            s_hid_driver->user_cb(&hid_event, s_hid_driver->user_arg);
        }
    } else if (event->event == USB_HOST_CLIENT_EVENT_DEV_GONE) {
        hid_device_t *hid_device = get_hid_device_by_handle(event->dev_gone.dev_hdl);
        if (hid_device) {
            const hid_host_event_t hid_event = {
                .event = HID_DEVICE_DISCONNECTED,
                .device.handle = hid_device,
            };
            s_hid_driver->user_cb(&hid_event, s_hid_driver->user_arg);
        }
    }
}

/**
 * @brief HID Host prepare interface transfer
 *
 * @param[in] iface       Pointer to Interface structure,
 * @param[in] callback    Input report data callback
 * @return esp_err_t
 */
static esp_err_t hid_host_interface_prepare_transfer(hid_iface_t *iface)
{
    HID_RETURN_ON_ERROR( usb_host_interface_claim( s_hid_driver->client_handle,
                         iface->dev->dev_hdl,
                         iface->num, 0) );

    HID_RETURN_ON_ERROR( usb_host_transfer_alloc(iface->ep_mps, 0, &iface->in_xfer) );

    return ESP_OK;
}

/**
 * @brief Disable active interface
 *
 * @param[in] hid_device  Pointer to HID device structure
 * @param[in] iface       Pointer to Interface structure
 * @return esp_err_t
 */
static esp_err_t hid_host_disable_interface(hid_device_t *hid_device, hid_iface_t *iface)
{
    HID_RETURN_ON_INVALID_ARG(hid_device);
    HID_RETURN_ON_INVALID_ARG(iface);

    if (!iface->ready) {
        return ESP_ERR_INVALID_STATE;
    }

    HID_RETURN_ON_ERROR( usb_host_endpoint_halt(hid_device->dev_hdl, iface->ep) );
    HID_RETURN_ON_ERROR( usb_host_endpoint_flush(hid_device->dev_hdl, iface->ep) );
    usb_host_endpoint_clear(hid_device->dev_hdl, iface->ep);

    return ESP_OK;
}

/**
 * @brief HID transfer complete status verification
 *
 * @param[in] transfer   Pointer to USB transfer struct
 * @return true Transfer completed successfully
 * @return false Transfer completed with error
 */
static bool transfer_complete_status_verify(usb_transfer_t *transfer)
{
    bool completed = false;

    assert(transfer);

    switch (transfer->status) {
    case USB_TRANSFER_STATUS_COMPLETED:
        completed = true;
        break;
    case USB_TRANSFER_STATUS_NO_DEVICE: // User is notified about device disconnection from usb_event_cb
    case USB_TRANSFER_STATUS_CANCELED:
        break;
    case USB_TRANSFER_STATUS_ERROR:
    case USB_TRANSFER_STATUS_TIMED_OUT:
    case USB_TRANSFER_STATUS_STALL:
    case USB_TRANSFER_STATUS_OVERFLOW:
    case USB_TRANSFER_STATUS_SKIPPED:
    default:
        // Transfer was not completed or canceled by user.
        ESP_LOGE(TAG, "Transfer failed, status %d", transfer->status);
    }
    return completed;
}

/**
 * @brief HID IN Transfer complete callback
 *
 * @param[in] transfer  Pointer to transfer data structure
 */
static void in_xfer_done(usb_transfer_t *in_xfer)
{
    assert(in_xfer);

    hid_device_t *hid_device = (hid_device_t *)in_xfer->context;
    hid_iface_t *iface = get_interface_by_ep(hid_device, in_xfer->bEndpointAddress);

    assert(iface);

    if (!transfer_complete_status_verify(in_xfer)) {
        interface_user_callback(iface, HID_DEVICE_INTERFACE_TRANSFER_ERROR);
        return;
    }

    if (iface->report_cb) {
        iface->report_cb(in_xfer->data_buffer, in_xfer->actual_num_bytes);
    }

    usb_host_transfer_submit(in_xfer);
}

static void hid_device_release(hid_device_t *hid_device)
{
    xSemaphoreGive(hid_device->device_busy);
}

static esp_err_t hid_device_claim(hid_device_t *hid_device, uint32_t timeout_ms)
{
    return ( xSemaphoreTake(hid_device->device_busy, pdMS_TO_TICKS(timeout_ms))
             ? ESP_OK
             : ESP_ERR_TIMEOUT );
}

static esp_err_t hid_device_wait_ready(hid_device_t *hid_device, uint32_t timeout_ms)
{
    HID_RETURN_ON_ERROR( hid_device_claim(hid_device, timeout_ms) );
    hid_device_release(hid_device);
    return ESP_OK;
}

/**
 * @brief HID Control transfer complete callback
 *
 * @param[in] ctrl_xfer  Pointer to transfer data structure
 */
static void ctrl_xfer_done(usb_transfer_t *ctrl_xfer)
{
    assert(ctrl_xfer);
    hid_device_t *hid_device = (hid_device_t *)ctrl_xfer->context;
    xSemaphoreGive(hid_device->ctrl_xfer_done);
}

/**
 * @brief HID control transfer synchronous.
 *
 * @note  Passes interface and endpoint descriptors to obtain:

 *        - interface number, IN endpoint, OUT endpoint, max. packet size
 *
 * @param[in] hid_device  Pointer to HID device structure
 * @param[in] ctrl_xfer   Pointer to the Transfer structure
 * @param[in] len         Number of bytes to transfer
 * @param[in] timeout_ms  Timeout in ms
 * @return esp_err_t
 */
esp_err_t hid_control_transfer(hid_device_t *hid_device,
                               usb_transfer_t *ctrl_xfer,
                               size_t len,
                               uint32_t timeout_ms)
{
    ctrl_xfer->device_handle = hid_device->dev_hdl;
    ctrl_xfer->callback = ctrl_xfer_done;
    ctrl_xfer->context = hid_device;
    ctrl_xfer->bEndpointAddress = 0;
    ctrl_xfer->timeout_ms = timeout_ms;
    ctrl_xfer->num_bytes = len;

    HID_RETURN_ON_ERROR( usb_host_transfer_submit_control(s_hid_driver->client_handle, ctrl_xfer));

    BaseType_t received = xSemaphoreTake(hid_device->ctrl_xfer_done, pdMS_TO_TICKS(ctrl_xfer->timeout_ms));

    if (received != pdTRUE) {
        // Transfer was not finished, error in USB LIB. Reset the endpoint
        ESP_LOGE(TAG, "Control Transfer Timeout");

        HID_RETURN_ON_ERROR( usb_host_endpoint_halt(hid_device->dev_hdl, ctrl_xfer->bEndpointAddress) );
        HID_RETURN_ON_ERROR( usb_host_endpoint_flush(hid_device->dev_hdl, ctrl_xfer->bEndpointAddress) );
        usb_host_endpoint_clear(hid_device->dev_hdl, ctrl_xfer->bEndpointAddress);
        return ESP_ERR_TIMEOUT;
    }

    ESP_LOG_BUFFER_HEXDUMP(TAG, ctrl_xfer->data_buffer, ctrl_xfer->actual_num_bytes, ESP_LOG_DEBUG);

    return ESP_OK;
}

/**
 * @brief HID class specific request Set
 *
 * @param[in] hid_device Pointer to HID device structure
 * @param[in] bRequest   Value to fill bRequest filed in request
 * @param[in] wValue     Value to fill wValue filed in request
 * @param[in] wIndex     Value to fill wIndex filed in request
 * @param[in] wLength    Value to fill wLength filed in request
 * @return esp_err_t
 */
static esp_err_t hid_class_request_set(hid_device_t *hid_device,
                                       uint8_t bRequest,
                                       uint16_t wValue,
                                       uint16_t wIndex,
                                       uint16_t wLength)
{
    esp_err_t ret;
    usb_transfer_t *ctrl_xfer = hid_device->ctrl_xfer;
    HID_RETURN_ON_INVALID_ARG(hid_device);
    HID_RETURN_ON_INVALID_ARG(hid_device->ctrl_xfer);

    HID_RETURN_ON_ERROR( hid_device_wait_ready(hid_device, 5000 /* timeout */) );
    HID_RETURN_ON_ERROR( hid_device_claim(hid_device, 5000 /* timeout */) );

    usb_setup_packet_t *setup = (usb_setup_packet_t *)ctrl_xfer->data_buffer;
    setup->bmRequestType = USB_BM_REQUEST_TYPE_DIR_OUT |
                           USB_BM_REQUEST_TYPE_TYPE_CLASS |
                           USB_BM_REQUEST_TYPE_RECIP_INTERFACE;
    setup->bRequest = bRequest;
    setup->wValue = wValue;
    setup->wIndex = wIndex;
    setup->wLength = wLength;

    ret = hid_control_transfer(hid_device, ctrl_xfer, USB_SETUP_PACKET_SIZE, 5000 /* timeout */);

    hid_device_release(hid_device);

    return ret;
}

/**
 * @brief HID class specific request Get
 *
 * @param[in] hid_device Pointer to HID device structure
 * @param[in] bRequest   Value to fill bRequest filed in request
 * @param[in] wValue     Value to fill wValue filed in request
 * @param[in] wIndex     Value to fill wIndex filed in request
 * @param[in] data       Pointer to date buffer to put received data, should be not less than length
 * @param[in] length     Length of data to receive
 * @return esp_err_t
 */
static esp_err_t hid_class_request_get(hid_device_t *hid_device,
                                       uint8_t bRequest,
                                       uint16_t wValue,
                                       uint16_t wIndex,
                                       uint8_t *data,
                                       uint32_t length)
{
    esp_err_t ret;
    HID_RETURN_ON_INVALID_ARG(hid_device);
    HID_RETURN_ON_INVALID_ARG(hid_device->ctrl_xfer);
    HID_RETURN_ON_INVALID_ARG(data);

    usb_transfer_t *ctrl_xfer = hid_device->ctrl_xfer;

    HID_RETURN_ON_ERROR( hid_device_wait_ready(hid_device, 5000 /* timeout */) );
    HID_RETURN_ON_ERROR( hid_device_claim(hid_device, 5000 /* timeout */) );

    usb_setup_packet_t *setup = (usb_setup_packet_t *)ctrl_xfer->data_buffer;

    setup->bmRequestType = USB_BM_REQUEST_TYPE_DIR_IN |
                           USB_BM_REQUEST_TYPE_TYPE_CLASS |
                           USB_BM_REQUEST_TYPE_RECIP_INTERFACE;
    setup->bRequest = bRequest;
    setup->wValue = wValue;
    setup->wIndex = wIndex;
    setup->wLength = length;

    ret = hid_control_transfer(hid_device, ctrl_xfer, USB_SETUP_PACKET_SIZE + length, 5000 /* timeout */);

    if (ESP_OK == ret) {
        ctrl_xfer->actual_num_bytes -= USB_SETUP_PACKET_SIZE;

        if (ctrl_xfer->actual_num_bytes <= length) {
            memcpy(data, ctrl_xfer->data_buffer + USB_SETUP_PACKET_SIZE, ctrl_xfer->actual_num_bytes);
        }
    }

    hid_device_release(hid_device);

    return ret;
}

/**
 * @brief HID class specific request SET IDLE
 *
 * @param[in] iface      Pointer to HID Interface configuration structure
 * @param[in] duration   0 (zero) for the indefinite duration, non-zero, then a fixed duration used.
 * @param[in] report_id  If 0 (zero) the idle rate applies to all input reports generated by the device, otherwise ReportID
 * @return esp_err_t
 */
static esp_err_t hid_class_request_set_idle(hid_iface_t *iface,
        uint8_t duration,
        uint8_t report_id)
{
    HID_RETURN_ON_INVALID_ARG(iface);

    return hid_class_request_set(iface->dev,
                                 HID_CLASS_SPECIFIC_REQ_SET_IDLE,
                                 (duration << 8) | report_id,
                                 iface->num, 0);
}

/**
 * @brief HID class specific request SET PROTOCOL
 *
 * @param[in] iface      Pointer to HID Interface configuration structure
 * @param[in] protocol   HID report protocol (boot or report)
 * @return esp_err_t
 */
static esp_err_t hid_class_request_set_protocol(hid_iface_t *iface,
        hid_report_protocol_t protocol)
{
    HID_RETURN_ON_INVALID_ARG(iface);

    return hid_class_request_set(iface->dev,
                                 HID_CLASS_SPECIFIC_REQ_SET_PROTOCOL,
                                 protocol,
                                 iface->num, 0);
}

/**
 * @brief HID class specific request GET PROTOCOL
 *
 * @param[in] iface      Pointer to HID Interface configuration structure
 * @param[in] protocol   Pointer to HID report protocol (boot or report) of device
 * @return esp_err_t
 */
static esp_err_t hid_class_request_get_protocol(hid_iface_t *iface,
        hid_report_protocol_t *protocol)
{
    uint8_t tmp[1] = { 0xff };

    HID_RETURN_ON_INVALID_ARG(iface);
    HID_RETURN_ON_INVALID_ARG(protocol);

    hid_class_request_get(iface->dev, HID_CLASS_SPECIFIC_REQ_GET_PROTOCOL, 0, iface->num, tmp, 1);
    *protocol = (hid_report_protocol_t) tmp[0];
    return ESP_OK;
}

/**
 * @brief HID Host Interface start transfer
 *
 * @param[in] iface      Pointer to HID Interface configuration structure
 * @param[in] timeout    Timeout in ms
 * @return esp_err_t
 */
static esp_err_t hid_host_iface_start_transfer(hid_iface_t *iface,
        const uint32_t timeout)
{
    HID_RETURN_ON_INVALID_ARG(iface);
    HID_RETURN_ON_INVALID_ARG(iface->in_xfer);
    HID_RETURN_ON_INVALID_ARG(iface->dev);

    // prepare transfer
    iface->in_xfer->device_handle = iface->dev->dev_hdl;
    iface->in_xfer->callback = in_xfer_done;
    iface->in_xfer->context = iface->dev;
    iface->in_xfer->timeout_ms = timeout;
    iface->in_xfer->bEndpointAddress = iface->ep;
    iface->in_xfer->num_bytes = iface->ep_mps;

    // start data transfer
    return usb_host_transfer_submit(iface->in_xfer);
}

// ----------------------- Public --------------------------

esp_err_t hid_host_install(const hid_host_driver_config_t *config)
{
    esp_err_t ret;
    HID_RETURN_ON_INVALID_ARG(config);
    HID_RETURN_ON_INVALID_ARG(config->callback);
    if ( config->create_background_task ) {
        HID_RETURN_ON_FALSE(config->stack_size != 0, ESP_ERR_INVALID_ARG);
        HID_RETURN_ON_FALSE(config->task_priority != 0, ESP_ERR_INVALID_ARG);
    }
    HID_RETURN_ON_FALSE(!s_hid_driver, ESP_ERR_INVALID_STATE);

    hid_driver_t *driver = calloc(1, sizeof(hid_driver_t));
    HID_RETURN_ON_FALSE(driver, ESP_ERR_NO_MEM);
    driver->user_cb = config->callback;
    driver->user_arg = config->callback_arg;

    usb_host_client_config_t client_config = {
        .async.client_event_callback = client_event_cb,
        .async.callback_arg = NULL,
        .max_num_event_msg = 10,
    };

    driver->end_client_event_handling = false;
    driver->all_events_handled = xSemaphoreCreateBinary();
    HID_GOTO_ON_FALSE(driver->all_events_handled, ESP_ERR_NO_MEM);

    HID_GOTO_ON_ERROR( usb_host_client_register(&client_config, &driver->client_handle) );

    HID_ENTER_CRITICAL();
    HID_GOTO_ON_FALSE_CRITICAL(!s_hid_driver, ESP_ERR_INVALID_STATE);
    s_hid_driver = driver;
    STAILQ_INIT(&devices_tailq);
    STAILQ_INIT(&ifaces_tailq);
    HID_EXIT_CRITICAL();

    if (config->create_background_task) {
        BaseType_t task_created = xTaskCreatePinnedToCore(
                                      event_handler_task,
                                      "USB HID HOST",
                                      config->stack_size,
                                      NULL,
                                      config->task_priority,
                                      NULL,
                                      config->core_id);
        HID_GOTO_ON_FALSE(task_created, ESP_ERR_NO_MEM);
    }

    return ESP_OK;

fail:
    s_hid_driver = NULL;
    if (driver->client_handle) {
        usb_host_client_deregister(driver->client_handle);
    }
    if (driver->all_events_handled) {
        vSemaphoreDelete(driver->all_events_handled);
    }
    free(driver);
    return ret;
}

esp_err_t hid_host_uninstall(void)
{
    // Make sure hid driver is installed,
    // not being uninstalled from other task
    // and no hid device is registered
    HID_ENTER_CRITICAL();
    HID_RETURN_ON_FALSE_CRITICAL( s_hid_driver != NULL, ESP_ERR_INVALID_STATE );
    HID_RETURN_ON_FALSE_CRITICAL( !s_hid_driver->end_client_event_handling, ESP_ERR_INVALID_STATE );
    HID_RETURN_ON_FALSE_CRITICAL( STAILQ_EMPTY(&devices_tailq), ESP_ERR_INVALID_STATE );
    HID_RETURN_ON_FALSE_CRITICAL( STAILQ_EMPTY(&ifaces_tailq), ESP_ERR_INVALID_STATE );
    s_hid_driver->end_client_event_handling = true;
    HID_EXIT_CRITICAL();

    xSemaphoreTake(s_hid_driver->all_events_handled, portMAX_DELAY);
    vSemaphoreDelete(s_hid_driver->all_events_handled);
    free(s_hid_driver);
    s_hid_driver = NULL;
    return ESP_OK;
}

void hid_host_event_handler_task(void *arg)
{
    event_handler_task(arg);
}

esp_err_t hid_host_claim_interface(const hid_host_interface_config_t *iface_config,
                                   hid_host_interface_handle_t *iface_handle)
{
    hid_iface_t *iface = NULL;
    hid_report_protocol_t report_protocol;

    HID_RETURN_ON_FALSE((iface_config->proto > HID_PROTOCOL_NONE)
                        && (iface_config->proto < HID_PROTOCOL_MAX),
                        ESP_ERR_INVALID_ARG);

    // Search Interface in list
    iface = get_interface_by_proto(iface_config->proto);

    // Interface not found in list
    if (iface == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    // Interface already claimed
    if (iface->report_cb != NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    HID_RETURN_ON_ERROR( hid_host_interface_prepare_transfer(iface) );

    /* Set infinity duration for output report = 0. Only reporting when a
     * change is detected in the report data.
     */
    HID_RETURN_ON_ERROR( hid_class_request_set_idle(iface, 0, 0) );

    // Set BOOT protocol for interface
    HID_RETURN_ON_ERROR (hid_class_request_get_protocol(iface, &report_protocol) );

    if (report_protocol != HID_REPORT_PROTOCOL_BOOT) {
        HID_RETURN_ON_ERROR( hid_class_request_set_protocol(iface, HID_REPORT_PROTOCOL_BOOT) );
        // verify that protocol has been successfully changed
        report_protocol = HID_REPORT_PROTOCOL_MAX;
        HID_RETURN_ON_ERROR (hid_class_request_get_protocol(iface, &report_protocol) );

        if (report_protocol != HID_REPORT_PROTOCOL_BOOT) {
            ESP_LOGE(TAG, "Interface %d Set BOOT Protocol Failure, protocol=%d",
                     iface->num,
                     report_protocol);
        }
    }

    // Claim interface and save report callback
    iface->report_cb = iface_config->callback;

    HID_RETURN_ON_ERROR( hid_host_iface_start_transfer(iface, 5000 /* timeout */) );

    interface_user_callback(iface, HID_DEVICE_INTERFACE_CLAIM);

    *iface_handle = iface;

    return ESP_OK;
}

esp_err_t hid_host_release_interface(hid_host_interface_handle_t iface_handle)
{
    hid_iface_t *iface = (hid_iface_t *)iface_handle;

    HID_RETURN_ON_FALSE(iface, ESP_ERR_NOT_FOUND);

    if (!is_interface_claimed(iface->proto)) {
        return ESP_ERR_INVALID_STATE;
    }

    if (iface->ready) {
        HID_RETURN_ON_ERROR( hid_host_disable_interface(iface->dev, iface));
        HID_RETURN_ON_ERROR( usb_host_interface_release(s_hid_driver->client_handle, iface->dev->dev_hdl, iface->num) );
    } else {
        usb_host_interface_release(s_hid_driver->client_handle, iface->dev->dev_hdl, iface->num);
    }

    ESP_ERROR_CHECK( usb_host_transfer_free(iface->in_xfer) );

    iface->report_cb = NULL;

    interface_user_callback(iface, HID_DEVICE_INTERFACE_RELEASE);

    return ESP_OK;
}

esp_err_t hid_host_install_device(const hid_host_device_config_t *hid_host_dev_config,
                                  hid_host_device_handle_t *hid_device_handle)
{
    esp_err_t ret;
    hid_device_t *hid_device;

    HID_GOTO_ON_FALSE( hid_device = calloc(1, sizeof(hid_device_t)), ESP_ERR_NO_MEM );

    HID_GOTO_ON_FALSE( hid_device->ctrl_xfer_done = xSemaphoreCreateBinary(), ESP_ERR_NO_MEM );
    HID_GOTO_ON_FALSE( hid_device->device_busy =  xSemaphoreCreateMutex(), ESP_ERR_NO_MEM );

    HID_GOTO_ON_ERROR( usb_host_device_open(s_hid_driver->client_handle,
                                            hid_host_dev_config->dev_addr,
                                            &hid_device->dev_hdl) );


    // dev_info.bMaxPacketSize0 + 1 because of the GET PROTOCOL specific class request
    usb_device_info_t dev_info;
    ESP_ERROR_CHECK(usb_host_device_info(hid_device->dev_hdl, &dev_info));
    HID_GOTO_ON_ERROR(usb_host_transfer_alloc(dev_info.bMaxPacketSize0 + 1, 0, &hid_device->ctrl_xfer));

    HID_GOTO_ON_ERROR( create_interface_list(hid_device,
                       hid_host_dev_config,
                       HID_SUBCLASS_BOOT_INTERFACE) );

    HID_ENTER_CRITICAL();
    HID_GOTO_ON_FALSE_CRITICAL( s_hid_driver, ESP_ERR_INVALID_STATE );
    HID_GOTO_ON_FALSE_CRITICAL( s_hid_driver->client_handle, ESP_ERR_INVALID_STATE );
    STAILQ_INSERT_TAIL(&devices_tailq, hid_device, tailq_entry);
    HID_EXIT_CRITICAL();

    *hid_device_handle = hid_device;

    return ESP_OK;

fail:
    hid_host_uninstall_device(hid_device);
    return ret;
}

esp_err_t hid_host_uninstall_device(hid_host_device_handle_t hid_device_handle)
{
    hid_device_t *hid_device = (hid_device_t *)hid_device_handle;

    HID_RETURN_ON_INVALID_ARG(hid_device);

    for (int proto = HID_PROTOCOL_KEYBOARD; proto < HID_PROTOCOL_MAX; proto++) {
        if (is_interface_claimed(proto)) {
            return ESP_ERR_INVALID_STATE;
        };
    }

    HID_RETURN_ON_ERROR( destroy_interface_list() );

    HID_RETURN_ON_ERROR( usb_host_transfer_free(hid_device->ctrl_xfer) );
    HID_RETURN_ON_ERROR( usb_host_device_close(s_hid_driver->client_handle, hid_device->dev_hdl) );

    if (hid_device->ctrl_xfer_done) {
        vSemaphoreDelete(hid_device->ctrl_xfer_done);
    }

    if (hid_device->device_busy) {
        vSemaphoreDelete(hid_device->device_busy);
    }

    HID_ENTER_CRITICAL();
    STAILQ_REMOVE(&devices_tailq, hid_device, hid_host_device, tailq_entry);
    HID_EXIT_CRITICAL();

    free(hid_device);
    return ESP_OK;

}

esp_err_t hid_host_print_descriptors(hid_host_device_handle_t hid_device_handle)
{
    hid_device_t *hid_devive = (hid_device_t *)hid_device_handle;
    const usb_device_desc_t *device_desc;
    const usb_config_desc_t *config_desc;
    HID_RETURN_ON_ERROR( usb_host_get_device_descriptor(hid_devive->dev_hdl, &device_desc) );
    HID_RETURN_ON_ERROR( usb_host_get_active_config_descriptor(hid_devive->dev_hdl, &config_desc) );
    usb_print_device_descriptor(device_desc);
    usb_print_config_descriptor(config_desc, NULL);
    return ESP_OK;
}
