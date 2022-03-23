/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <sys/queue.h>
#include "usb/usb_host.h"
#include "usb/cdc_acm_host.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_check.h"
#include "esp_system.h"

#define TAG "cdc_acm"

// CDC devices often implement Interface Association Descriptor (IAD). Parse IAD only when
// bDeviceClass = 0xEF (Miscellaneous Device Class), bDeviceSubClass = 0x02 (Common Class), bDeviceProtocol = 0x01 (Interface Association Descriptor)
// @see USB Interface Association Descriptor: Device Class Code and Use Model rev 1.0, Table 1-1
#define USB_SUBCLASS_COMMON        0x02
#define USB_DEVICE_PROTOCOL_IAD    0x01

// CDC-ACM spinlock
static portMUX_TYPE cdc_acm_lock = portMUX_INITIALIZER_UNLOCKED;
#define CDC_ACM_ENTER_CRITICAL()   portENTER_CRITICAL(&cdc_acm_lock)
#define CDC_ACM_EXIT_CRITICAL()    portEXIT_CRITICAL(&cdc_acm_lock)

// CDC-ACM events
#define CDC_ACM_TEARDOWN          BIT0
#define CDC_ACM_TEARDOWN_COMPLETE BIT1

// CDC-ACM check macros
#define CDC_ACM_CHECK(cond, ret_val) ({                                     \
            if (!(cond)) {                                                  \
                return (ret_val);                                           \
            }                                                               \
})

#define CDC_ACM_CHECK_FROM_CRIT(cond, ret_val) ({                           \
            if (!(cond)) {                                                  \
                CDC_ACM_EXIT_CRITICAL();                                    \
                return ret_val;                                             \
            }                                                               \
})

// CDC-ACM driver object
typedef struct {
    usb_host_client_handle_t cdc_acm_client_hdl;        /*!< USB Host handle reused for all CDC-ACM devices in the system */
    SemaphoreHandle_t open_close_mutex;
    EventGroupHandle_t event_group;
    SLIST_HEAD(list_dev, cdc_dev_s) cdc_devices_list;   /*!< List of open pseudo devices */
} cdc_acm_obj_t;

static cdc_acm_obj_t *p_cdc_acm_obj = NULL;

/**
 * @brief Default CDC-ACM driver configuration
 *
 * This configuration is used when user passes NULL to config pointer during device open.
 */
static const cdc_acm_host_driver_config_t cdc_acm_driver_config_default = {
    .driver_task_stack_size = 4096,
    .driver_task_priority = 10,
    .xCoreID = 0
};

/**
 * @brief USB CDC PSTN Call Descriptor
 *
 * @see Table 3, USB CDC-PSTN specification rev. 1.2
 */
typedef struct {
    uint8_t bFunctionLength;
    const uint8_t bDescriptorType;
    const cdc_desc_subtype_t bDescriptorSubtype;
    union {
        struct {
            uint8_t call_management:   1; // Device handles call management itself
            uint8_t call_over_data_if: 1; // Device sends/receives call management information over Data Class interface
            uint8_t reserved: 6;
        };
        uint8_t val;
    } bmCapabilities;
    uint8_t bDataInterface; // Interface number of Data Class interface optionally used for call management
} __attribute__((packed)) cdc_acm_call_desc_t;

/**
 * @brief USB CDC PSTN Abstract Control Model Descriptor
 *
 * @see Table 4, USB CDC-PSTN specification rev. 1.2
 */
typedef struct {
    uint8_t bFunctionLength;
    const uint8_t bDescriptorType;
    const cdc_desc_subtype_t bDescriptorSubtype;
    union {
        struct {
            uint8_t feature:    1; // Device supports Set/Clear/Get_Comm_Feature requests
            uint8_t serial:     1; // Device supports Set/Get_Line_Coding, Set_Control_Line_State and Serial_State request and notifications
            uint8_t send_break: 1; // Device supports Send_Break request
            uint8_t network:    1; // Device supports Network_Connection notification
            uint8_t reserved:   4;
        };
        uint8_t val;
    } bmCapabilities;
} __attribute__((packed)) cdc_acm_acm_desc_t;

typedef struct cdc_dev_s cdc_dev_t;
struct cdc_dev_s{
    usb_device_handle_t dev_hdl;          // USB device handle
    void *cb_arg;                         // Common argument for user's callbacks (data IN and Notification)
    struct {
        usb_transfer_t *out_xfer;         // OUT data transfer
        usb_transfer_t *in_xfer;          // IN data transfer
        cdc_acm_data_callback_t in_cb;    // User's callback for async (non-blocking) data IN
        const usb_intf_desc_t *intf_desc; // Pointer to data interface descriptor
        SemaphoreHandle_t out_mux;        // OUT mutex
    } data;

    struct {
        usb_transfer_t *xfer;             // IN notification transfer
        const usb_intf_desc_t *intf_desc; // Pointer to notification interface descriptor, can be NULL if there is no notification channel in the device
        cdc_acm_host_dev_callback_t cb;   // User's callback for device events
    } notif;                              // Structure with Notif pipe data

    usb_transfer_t *ctrl_transfer;        // CTRL (endpoint 0) transfer
    SemaphoreHandle_t ctrl_mux;           // CTRL mutex
    cdc_acm_uart_state_t serial_state;    // Serial State
    cdc_comm_protocol_t comm_protocol;
    cdc_data_protocol_t data_protocol;
    int             num_cdc_intf_desc;    // Number of CDC Interface descriptors in following array
    const usb_standard_desc_t **cdc_intf_desc;   // CDC Interface descriptors
    SLIST_ENTRY(cdc_dev_s) list_entry;
};

/**
 * @brief Notification received callback
 *
 * Notification (interrupt) IN transfer is submitted at the end of this function to ensure periodic poll of IN endpoint.
 *
 * @param[in] transfer Transfer that triggered the callback
 */
static void notif_xfer_cb(usb_transfer_t *transfer);

/**
 * @brief Data received callback
 *
 * Data (bulk) IN transfer is submitted at the end of this function to ensure continuous poll of IN endpoint.
 *
 * @param[in] transfer Transfer that triggered the callback
 */
static void in_xfer_cb(usb_transfer_t *transfer);

/**
 * @brief Data send callback
 *
 * Reused for bulk OUT and CTRL transfers
 *
 * @param[in] transfer Transfer that triggered the callback
 */
static void out_xfer_cb(usb_transfer_t *transfer);

/**
 * @brief USB Host Client event callback
 *
 * Handling of USB device connection/disconnection to/from root HUB.
 *
 * @param[in] event_msg Event message type
 * @param[in] arg Caller's argument (not used in this driver)
 */
static void usb_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg);

/**
 * @brief Send CDC specific request
 *
 * Helper function that will send CDC specific request to default endpoint.
 * Both IN and OUT requests are sent through this API, depending on the in_transfer parameter.
 *
 * @see  Chapter 6.2, USB CDC specification rev. 1.2
 * @note CDC specific requests are only supported by devices that have dedicated management element.
 *
 * @param[in] cdc_dev Pointer to CDC device
 * @param[in] in_transfer Direction of data phase. true: IN, false: OUT
 * @param[in] request CDC request code
 * @param[inout] data Pointer to data buffer. Input for OUT transfers, output for IN transfers.
 * @param[in] data_len Length of data buffer
 * @param[in] value Value to be set in bValue of Setup packet
 * @return esp_err_t
 */
static esp_err_t send_cdc_request(cdc_dev_t *cdc_dev, bool in_transfer, cdc_request_code_t request, uint8_t *data, uint16_t data_len, uint16_t value);

/**
 * @brief CDC-ACM driver handling task
 *
 * USB host client registration and deregistration is handled here.
 *
 * @param[in] arg User's argument. Handle of a task that started this task.
 */
static void cdc_acm_client_task(void *arg)
{
    vTaskSuspend(NULL); // Task will be resumed from cdc_acm_host_install()
    cdc_acm_obj_t *cdc_acm_obj = p_cdc_acm_obj; // Make local copy of the driver's handle
    assert(cdc_acm_obj->cdc_acm_client_hdl);

    // Start handling client's events
    while (1) {
        usb_host_client_handle_events(cdc_acm_obj->cdc_acm_client_hdl, portMAX_DELAY);
        EventBits_t events = xEventGroupGetBits(cdc_acm_obj->event_group);
        if (events & CDC_ACM_TEARDOWN) {
            break;
        }
    }

    ESP_LOGD(TAG, "Deregistering client");
    ESP_ERROR_CHECK(usb_host_client_deregister(cdc_acm_obj->cdc_acm_client_hdl));
    xEventGroupSetBits(cdc_acm_obj->event_group, CDC_ACM_TEARDOWN_COMPLETE);
    vTaskDelete(NULL);
}

/**
 * @brief Cancel transfer and reset endpoint
 *
 * This function will cancel ongoing transfer a reset its endpoint to ready state.
 *
 * @param[in] dev_hdl USB device handle
 * @param[in] transfer Transfer to be cancelled
 * @return esp_err_t
 */
static esp_err_t cdc_acm_reset_transfer_endpoint(usb_device_handle_t dev_hdl, usb_transfer_t *transfer)
{
    assert(dev_hdl);
    assert(transfer);

    ESP_RETURN_ON_ERROR(usb_host_endpoint_halt(dev_hdl, transfer->bEndpointAddress), TAG,);
    ESP_RETURN_ON_ERROR(usb_host_endpoint_flush(dev_hdl, transfer->bEndpointAddress), TAG,);
    usb_host_endpoint_clear(dev_hdl, transfer->bEndpointAddress);
    return ESP_OK;
}

/**
 * @brief Start CDC device
 *
 * After this call, USB host peripheral will continuously poll IN endpoints.
 *
 * @param cdc_dev
 * @param[in] event_cb  Device event callback
 * @param[in] in_cb     Data received callback
 * @param[in] user_arg  Optional user's argument, that will be passed to the callbacks
 * @return esp_err_t
 */
static esp_err_t cdc_acm_start(cdc_dev_t *cdc_dev, cdc_acm_host_dev_callback_t event_cb, cdc_acm_data_callback_t in_cb, void *user_arg)
{
    esp_err_t ret = ESP_OK;
    assert(cdc_dev);

    CDC_ACM_ENTER_CRITICAL();
    cdc_dev->notif.cb = event_cb;
    cdc_dev->data.in_cb = in_cb;
    cdc_dev->cb_arg = user_arg;
    CDC_ACM_EXIT_CRITICAL();

    // Claim data interface and start polling its IN endpoint
    ESP_GOTO_ON_ERROR(usb_host_interface_claim(p_cdc_acm_obj->cdc_acm_client_hdl, cdc_dev->dev_hdl, cdc_dev->data.intf_desc->bInterfaceNumber, 0), err, TAG,);
    ESP_LOGD("CDC_ACM", "Submitting poll for BULK IN transfer");
    ESP_ERROR_CHECK(usb_host_transfer_submit(cdc_dev->data.in_xfer));

    // If notification are supported, claim its interface and start polling its IN endpoint
    if (cdc_dev->notif.intf_desc != NULL) {
        if (cdc_dev->notif.intf_desc != cdc_dev->data.intf_desc) {
            ESP_GOTO_ON_ERROR(usb_host_interface_claim(p_cdc_acm_obj->cdc_acm_client_hdl, cdc_dev->dev_hdl,
                                                     cdc_dev->notif.intf_desc->bInterfaceNumber, 0), err, TAG,);
        }
        ESP_LOGD("CDC_ACM", "Submitting poll for INTR IN transfer");
        ESP_ERROR_CHECK(usb_host_transfer_submit(cdc_dev->notif.xfer));
    }

    // Everything OK, add the device into list and return
    CDC_ACM_ENTER_CRITICAL();
    SLIST_INSERT_HEAD(&p_cdc_acm_obj->cdc_devices_list, cdc_dev, list_entry);
    CDC_ACM_EXIT_CRITICAL();
    return ret;

err:
    usb_host_interface_release(p_cdc_acm_obj->cdc_acm_client_hdl, cdc_dev->dev_hdl, cdc_dev->data.intf_desc->bInterfaceNumber);
    if (cdc_dev->notif.intf_desc != NULL) {
        usb_host_interface_release(p_cdc_acm_obj->cdc_acm_client_hdl, cdc_dev->dev_hdl, cdc_dev->notif.intf_desc->bInterfaceNumber);
    }
    return ret;
}

static void cdc_acm_transfers_free(cdc_dev_t *cdc_dev);
/**
 * @brief Helper function that releases resources claimed by CDC device
 *
 * Close underlying USB device, free device driver memory
 *
 * @note All interfaces claimed by this device must be release before calling this function
 * @param cdc_dev CDC device handle to be removed
 */
static void cdc_acm_device_remove(cdc_dev_t *cdc_dev)
{
    assert(cdc_dev);
    cdc_acm_transfers_free(cdc_dev);
    free(cdc_dev->cdc_intf_desc);
    // We don't check the error code of usb_host_device_close, as the close might fail, if someone else is still using the device (not all interfaces are released)
    usb_host_device_close(p_cdc_acm_obj->cdc_acm_client_hdl, cdc_dev->dev_hdl); // Gracefully continue on error
    free(cdc_dev);
}

/**
 * @brief Open USB device with requested VID/PID
 *
 * This function has two regular return paths:
 * 1. USB device with matching VID/PID is already opened by this driver: allocate new CDC device on top of the already opened USB device.
 * 2. USB device with matching VID/PID is NOT opened by this driver yet: poll USB connected devices until it is found.
 *
 * @note This function will block for timeout_ms, if the device is not enumerated at the moment of calling this function.
 * @param[in] vid Vendor ID
 * @param[in] pid Product ID
 * @param[in] timeout_ms Connection timeout [ms]
 * @param[out] dev CDC-ACM device
 * @return esp_err_t
 */
static esp_err_t cdc_acm_find_and_open_usb_device(uint16_t vid, uint16_t pid, int timeout_ms, cdc_dev_t **dev)
{
    assert(p_cdc_acm_obj);
    assert(dev);

    *dev = calloc(1, sizeof(cdc_dev_t));
    if (*dev == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // First, check list of already opened CDC devices
    ESP_LOGD(TAG, "Checking list of opened USB devices");
    cdc_dev_t *cdc_dev;
    SLIST_FOREACH(cdc_dev, &p_cdc_acm_obj->cdc_devices_list, list_entry) {
        const usb_device_desc_t *device_desc;
        ESP_ERROR_CHECK(usb_host_get_device_descriptor(cdc_dev->dev_hdl, &device_desc));
        if (device_desc->idVendor == vid && device_desc->idProduct == pid) {
            // Return path 1:
            (*dev)->dev_hdl = cdc_dev->dev_hdl;
            return ESP_OK;
        }
    }

    // Second, poll connected devices until new device is connected or timeout
    TickType_t timeout_ticks = (timeout_ms == 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    TimeOut_t connection_timeout;
    vTaskSetTimeOutState(&connection_timeout);

    while (true) {
        ESP_LOGD(TAG, "Checking list of connected USB devices");
        uint8_t dev_addr_list[10];
        int num_of_devices;
        ESP_ERROR_CHECK(usb_host_device_addr_list_fill(sizeof(dev_addr_list), dev_addr_list, &num_of_devices));

        // Go through device address list and find the one we are looking for
        for (int i = 0; i < num_of_devices; i++) {
            usb_device_handle_t current_device;
            // Open USB device
            if (usb_host_device_open(p_cdc_acm_obj->cdc_acm_client_hdl, dev_addr_list[i], &current_device) != ESP_OK) {
                continue; // In case we failed to open this device, continue with next one in the list
            }
            assert(current_device);
            const usb_device_desc_t *device_desc;
            ESP_ERROR_CHECK(usb_host_get_device_descriptor(current_device, &device_desc));
            if (device_desc->idVendor == vid && device_desc->idProduct == pid) {
                // Return path 2:
                (*dev)->dev_hdl = current_device;
                return ESP_OK;
            }
            usb_host_device_close(p_cdc_acm_obj->cdc_acm_client_hdl, current_device);
        }

        if (xTaskCheckForTimeOut(&connection_timeout, &timeout_ticks) != pdFALSE) {
            break; // Timeout elapsed and the device is not connected
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Timeout was reached, clean-up
    free(*dev);
    *dev = NULL;
    return ESP_ERR_NOT_FOUND;
}

esp_err_t cdc_acm_host_install(const cdc_acm_host_driver_config_t *driver_config)
{
    CDC_ACM_CHECK(!p_cdc_acm_obj, ESP_ERR_INVALID_STATE);

    // Check driver configuration, use default if NULL is passed
    if (driver_config == NULL) {
        driver_config = &cdc_acm_driver_config_default;
    }

    // Allocate all we need for this driver
    esp_err_t ret;
    cdc_acm_obj_t *cdc_acm_obj = heap_caps_calloc(1, sizeof(cdc_acm_obj_t), MALLOC_CAP_DEFAULT);
    EventGroupHandle_t event_group = xEventGroupCreate();
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    TaskHandle_t driver_task_h = NULL;
    xTaskCreatePinnedToCore(
            cdc_acm_client_task, "USB-CDC", driver_config->driver_task_stack_size, NULL,
            driver_config->driver_task_priority, &driver_task_h, driver_config->xCoreID);

    if (cdc_acm_obj == NULL || driver_task_h == NULL || event_group == NULL || mutex == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }

    // Register USB Host client
    usb_host_client_handle_t usb_client = NULL;
    const usb_host_client_config_t client_config = {
        .is_synchronous = false,
        .max_num_event_msg = 3,
        .async.client_event_callback = usb_event_cb,
        .async.callback_arg = NULL
    };
    ESP_GOTO_ON_ERROR(usb_host_client_register(&client_config, &usb_client), err, TAG, "Failed to register USB host client");

    // Initialize CDC-ACM driver structure
    SLIST_INIT(&(cdc_acm_obj->cdc_devices_list));
    cdc_acm_obj->event_group = event_group;
    cdc_acm_obj->open_close_mutex = mutex;
    cdc_acm_obj->cdc_acm_client_hdl = usb_client;

    // Between 1st call of this function and following section, another task might try to install this driver:
    // Make sure that there is only one instance of this driver in the system
    CDC_ACM_ENTER_CRITICAL();
    if (p_cdc_acm_obj) {
        // Already created
        ret = ESP_ERR_INVALID_STATE;
        CDC_ACM_EXIT_CRITICAL();
        goto client_err;
    } else {
        p_cdc_acm_obj = cdc_acm_obj;
    }
    CDC_ACM_EXIT_CRITICAL();

    // Everything OK: Start CDC-Driver task and return
    vTaskResume(driver_task_h);
    return ESP_OK;

client_err:
    usb_host_client_deregister(usb_client);
err: // Clean-up
    free(cdc_acm_obj);
    if (event_group) {
        vEventGroupDelete(event_group);
    }
    if (driver_task_h) {
        vTaskDelete(driver_task_h);
    }
    if (mutex) {
        vSemaphoreDelete(mutex);
    }
    return ret;
}

esp_err_t cdc_acm_host_uninstall()
{
    esp_err_t ret;

    CDC_ACM_ENTER_CRITICAL();
    CDC_ACM_CHECK_FROM_CRIT(p_cdc_acm_obj, ESP_ERR_INVALID_STATE);
    cdc_acm_obj_t *cdc_acm_obj = p_cdc_acm_obj; // Save Driver's handle to temporary handle
    CDC_ACM_EXIT_CRITICAL();

    xSemaphoreTake(p_cdc_acm_obj->open_close_mutex, portMAX_DELAY); // Wait for all open/close calls to finish

    CDC_ACM_ENTER_CRITICAL();
    if (SLIST_EMPTY(&p_cdc_acm_obj->cdc_devices_list)) { // Check that device list is empty (all devices closed)
        p_cdc_acm_obj = NULL; // NULL static driver pointer: No open/close calls form this point
    } else {
        ret = ESP_ERR_INVALID_STATE;
        CDC_ACM_EXIT_CRITICAL();
        goto unblock;
    }
    CDC_ACM_EXIT_CRITICAL();

    // Signal to CDC task to stop, unblock it and wait for its deletion
    xEventGroupSetBits(cdc_acm_obj->event_group, CDC_ACM_TEARDOWN);
    usb_host_client_unblock(cdc_acm_obj->cdc_acm_client_hdl);
    ESP_GOTO_ON_FALSE(
        xEventGroupWaitBits(cdc_acm_obj->event_group, CDC_ACM_TEARDOWN_COMPLETE, pdFALSE, pdFALSE, pdMS_TO_TICKS(100)),
        ESP_ERR_NOT_FINISHED, unblock, TAG,);

    // Free remaining resources and return
    vEventGroupDelete(cdc_acm_obj->event_group);
    xSemaphoreGive(cdc_acm_obj->open_close_mutex);
    vSemaphoreDelete(cdc_acm_obj->open_close_mutex);
    free(cdc_acm_obj);
    return ESP_OK;

unblock:
    xSemaphoreGive(cdc_acm_obj->open_close_mutex);
    return ret;
}

/**
 * @brief Free USB transfers used by this device
 *
 * @note There can be no transfers in flight, at the moment of calling this function.
 * @param[in] cdc_dev Pointer to CDC device
 */
static void cdc_acm_transfers_free(cdc_dev_t *cdc_dev)
{
    assert(cdc_dev);
    usb_host_transfer_free(cdc_dev->notif.xfer);
    usb_host_transfer_free(cdc_dev->data.in_xfer);
    if (cdc_dev->data.out_xfer != NULL) {
        if (cdc_dev->data.out_xfer->context != NULL) {
            vSemaphoreDelete((SemaphoreHandle_t)cdc_dev->data.out_xfer->context);
        }
        if (cdc_dev->data.out_mux != NULL) {
            vSemaphoreDelete(cdc_dev->data.out_mux);
        }
        usb_host_transfer_free(cdc_dev->data.out_xfer);
    }
    if (cdc_dev->ctrl_transfer != NULL) {
        if (cdc_dev->ctrl_transfer->context != NULL) {
            vSemaphoreDelete((SemaphoreHandle_t)cdc_dev->ctrl_transfer->context);
        }
        if (cdc_dev->ctrl_mux != NULL) {
            vSemaphoreDelete(cdc_dev->ctrl_mux);
        }
        usb_host_transfer_free(cdc_dev->ctrl_transfer);
    }
}

/**
 * @brief Allocate CDC transfers
 *
 * @param[in] cdc_dev       Pointer to CDC device
 * @param[in] notif_ep_desc Pointer to notification EP descriptor
 * @param[in] in_ep_desc-   Pointer to data IN EP descriptor
 * @param[in] out_ep_desc   Pointer to data OUT EP descriptor
 * @param[in] out_buf_len   Length of data OUT buffer
 * @return esp_err_t
 */
static esp_err_t cdc_acm_transfers_allocate(cdc_dev_t *cdc_dev, const usb_ep_desc_t *notif_ep_desc, const usb_ep_desc_t *in_ep_desc, const usb_ep_desc_t *out_ep_desc, size_t out_buf_len)
{
    esp_err_t ret;

    // 1. Setup notification transfer if it is supported
    if (notif_ep_desc) {
        ESP_GOTO_ON_ERROR(
            usb_host_transfer_alloc(USB_EP_DESC_GET_MPS(notif_ep_desc), 0, &cdc_dev->notif.xfer),
            err, TAG,);
        cdc_dev->notif.xfer->device_handle = cdc_dev->dev_hdl;
        cdc_dev->notif.xfer->bEndpointAddress = notif_ep_desc->bEndpointAddress;
        cdc_dev->notif.xfer->callback = notif_xfer_cb;
        cdc_dev->notif.xfer->context = cdc_dev;
        cdc_dev->notif.xfer->num_bytes = USB_EP_DESC_GET_MPS(notif_ep_desc);
    }

    // 2. Setup control transfer
    usb_device_info_t dev_info;
    ESP_ERROR_CHECK(usb_host_device_info(cdc_dev->dev_hdl, &dev_info));
    ESP_GOTO_ON_ERROR(
        usb_host_transfer_alloc(dev_info.bMaxPacketSize0, 0, &cdc_dev->ctrl_transfer),
        err, TAG,);
    cdc_dev->ctrl_transfer->timeout_ms = 1000;
    cdc_dev->ctrl_transfer->bEndpointAddress = 0;
    cdc_dev->ctrl_transfer->device_handle = cdc_dev->dev_hdl;
    cdc_dev->ctrl_transfer->context = cdc_dev;
    cdc_dev->ctrl_transfer->callback = out_xfer_cb;
    cdc_dev->ctrl_transfer->context = xSemaphoreCreateBinary();
    ESP_GOTO_ON_FALSE(cdc_dev->ctrl_transfer->context, ESP_ERR_NO_MEM, err, TAG,);
    cdc_dev->ctrl_mux = xSemaphoreCreateMutex();
    ESP_GOTO_ON_FALSE(cdc_dev->ctrl_mux, ESP_ERR_NO_MEM, err, TAG,);

    // 3. Setup IN data transfer
    ESP_GOTO_ON_ERROR(
        usb_host_transfer_alloc(USB_EP_DESC_GET_MPS(in_ep_desc), 0, &cdc_dev->data.in_xfer),
        err, TAG,
    );
    assert(cdc_dev->data.in_xfer);
    cdc_dev->data.in_xfer->callback = in_xfer_cb;
    cdc_dev->data.in_xfer->num_bytes = USB_EP_DESC_GET_MPS(in_ep_desc);
    cdc_dev->data.in_xfer->bEndpointAddress = in_ep_desc->bEndpointAddress;
    cdc_dev->data.in_xfer->device_handle = cdc_dev->dev_hdl;
    cdc_dev->data.in_xfer->context = cdc_dev;

    // 4. Setup OUT bulk transfer (if it is required (out_buf_len > 0))
    if (out_buf_len != 0) {
        ESP_GOTO_ON_ERROR(
            usb_host_transfer_alloc(out_buf_len, 0, &cdc_dev->data.out_xfer),
            err, TAG,
        );
        assert(cdc_dev->data.out_xfer);
        cdc_dev->data.out_xfer->device_handle = cdc_dev->dev_hdl;
        cdc_dev->data.out_xfer->context = xSemaphoreCreateBinary();
        ESP_GOTO_ON_FALSE(cdc_dev->data.out_xfer->context, ESP_ERR_NO_MEM, err, TAG,);
        cdc_dev->data.out_mux = xSemaphoreCreateMutex();
        ESP_GOTO_ON_FALSE(cdc_dev->data.out_mux, ESP_ERR_NO_MEM, err, TAG,);
        cdc_dev->data.out_xfer->bEndpointAddress = out_ep_desc->bEndpointAddress;
        cdc_dev->data.out_xfer->callback = out_xfer_cb;
    }
    return ESP_OK;

err:
    cdc_acm_transfers_free(cdc_dev);
    return ret;
}

/**
 * @brief Find CDC interface descriptor and its endpoint descriptors
 *
 * @note This function is called in open procedure of CDC compliant devices only.
 * @param[in]  cdc_dev  Pointer to CDC device
 * @param[in]  intf_idx Index of CDC interface that should be used for this device
 * @param[out] notif_ep Pointer to notification EP descriptor
 * @param[out] in_ep    Pointer to data IN EP descriptor
 * @param[out] out_ep   Pointer to data OUT EP descriptor
 * @return esp_err_t
 */
static esp_err_t cdc_acm_find_intf_and_ep_desc(cdc_dev_t *cdc_dev, uint8_t intf_idx, const usb_ep_desc_t **notif_ep, const usb_ep_desc_t **in_ep, const usb_ep_desc_t **out_ep)
{
    bool interface_found = false;
    const usb_config_desc_t *config_desc;
    const usb_device_desc_t *device_desc;
    int data_intf_idx, notif_intf_idx;
    int desc_offset = 0;

    // Get required descriptors
    ESP_ERROR_CHECK(usb_host_get_device_descriptor(cdc_dev->dev_hdl, &device_desc));
    ESP_ERROR_CHECK(usb_host_get_active_config_descriptor(cdc_dev->dev_hdl, &config_desc));

    if ((device_desc->bDeviceClass == USB_CLASS_MISC) && (device_desc->bDeviceSubClass == USB_SUBCLASS_COMMON) &&
        (device_desc->bDeviceProtocol == USB_DEVICE_PROTOCOL_IAD)) {
        // This is a composite device, that uses Interface Association Descriptor
        const usb_standard_desc_t *this_desc = (const usb_standard_desc_t *)config_desc;
        do {
            this_desc = usb_parse_next_descriptor_of_type(
                this_desc, config_desc->wTotalLength, USB_B_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION, &desc_offset);

            if (this_desc == NULL)
                break; // Reached end of configuration descriptor

            const usb_iad_desc_t *iad_desc = (const usb_iad_desc_t *)this_desc;
            if (iad_desc->bFirstInterface == intf_idx) {
                // IAD with correct interface number was found: Check Class/Subclass codes, save Interface indexes
                assert(iad_desc->bInterfaceCount == 2);
                assert(iad_desc->bFunctionClass == USB_CLASS_COMM);
                assert(iad_desc->bFunctionSubClass == USB_CDC_SUBCLASS_ACM);
                notif_intf_idx = iad_desc->bFirstInterface;
                data_intf_idx = iad_desc->bFirstInterface + 1;
                interface_found = true;
            }
        } while (!interface_found);
    } else if ((device_desc->bDeviceClass == USB_CLASS_COMM) && (intf_idx == 0)) {
        // This is a Communication Device Class
        notif_intf_idx = 0;
        data_intf_idx = 1;
        interface_found = true;
    }

    // Save found interfaces descriptors:
    if (interface_found) {
        // Notification IF and EP
        cdc_dev->notif.intf_desc = usb_parse_interface_descriptor(config_desc, notif_intf_idx, 0, &desc_offset);
        assert(cdc_dev->notif.intf_desc);

        // CDC specific descriptors should be right after CDC-Communication interface descriptor
        // Note: That's why we use usb_parse_next_descriptor instead of usb_parse_next_descriptor_of_type.
        // The latter could return CDC specific descriptors that don't belong to this interface
        const usb_standard_desc_t *cdc_desc = (usb_standard_desc_t *)cdc_dev->notif.intf_desc;
        do {
            cdc_desc = usb_parse_next_descriptor(cdc_desc, config_desc->wTotalLength, &desc_offset);
            if ((cdc_desc == NULL) || (cdc_desc->bDescriptorType != ((USB_CLASS_COMM << 4) | USB_B_DESCRIPTOR_TYPE_INTERFACE )))
                break; // We found all CDC specific descriptors
            cdc_dev->num_cdc_intf_desc++;
            cdc_dev->cdc_intf_desc =
                realloc(cdc_dev->cdc_intf_desc, cdc_dev->num_cdc_intf_desc * (sizeof(usb_standard_desc_t *)));
            assert(cdc_dev->cdc_intf_desc);
            cdc_dev->cdc_intf_desc[cdc_dev->num_cdc_intf_desc - 1] = cdc_desc;
        } while (1);
        *notif_ep = usb_parse_endpoint_descriptor_by_index(cdc_dev->notif.intf_desc, 0, config_desc->wTotalLength, &desc_offset);
        assert(notif_ep);

        // Data IF and EP
        cdc_dev->data.intf_desc = usb_parse_interface_descriptor(config_desc, data_intf_idx, 0, &desc_offset);
        assert(cdc_dev->data.intf_desc);
        int temp_offset = desc_offset;
        for (int i = 0; i < 2; i++) {
            const usb_ep_desc_t *this_ep = usb_parse_endpoint_descriptor_by_index(cdc_dev->data.intf_desc, i, config_desc->wTotalLength, &desc_offset);
            assert(this_ep);
            if (USB_EP_DESC_GET_EP_DIR(this_ep)) {
                *in_ep = this_ep;
            } else {
                *out_ep = this_ep;
            }
            desc_offset = temp_offset;
        }
        return ESP_OK;
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t cdc_acm_host_open(uint16_t vid, uint16_t pid, uint8_t interface_idx, const cdc_acm_host_device_config_t *dev_config, cdc_acm_dev_hdl_t *cdc_hdl_ret)
{
    esp_err_t ret;
    CDC_ACM_CHECK(p_cdc_acm_obj, ESP_ERR_INVALID_STATE);
    CDC_ACM_CHECK(dev_config, ESP_ERR_INVALID_ARG);
    CDC_ACM_CHECK(cdc_hdl_ret, ESP_ERR_INVALID_ARG);

    xSemaphoreTake(p_cdc_acm_obj->open_close_mutex, portMAX_DELAY);
    // Find underlying USB device
    cdc_dev_t *cdc_dev;
    ESP_GOTO_ON_ERROR(
        cdc_acm_find_and_open_usb_device(vid, pid, dev_config->connection_timeout_ms, &cdc_dev),
        exit, TAG, "USB device with VID: 0x%04X, PID: 0x%04X not found", vid, pid);

    // Find and save relevant interface and endpoint descriptors
    const usb_ep_desc_t *notif_ep = NULL;
    const usb_ep_desc_t *in_ep = NULL;
    const usb_ep_desc_t *out_ep = NULL;
    ESP_GOTO_ON_ERROR(
        cdc_acm_find_intf_and_ep_desc(cdc_dev, interface_idx, &notif_ep, &in_ep, &out_ep),
        err, TAG, "Could not find required interface");

    // Check whether found Interfaces are really CDC-ACM
    assert(cdc_dev->notif.intf_desc->bInterfaceClass == USB_CLASS_COMM);
    assert(cdc_dev->notif.intf_desc->bInterfaceSubClass == USB_CDC_SUBCLASS_ACM);
    assert(cdc_dev->notif.intf_desc->bNumEndpoints == 1);
    assert(cdc_dev->data.intf_desc->bInterfaceClass == USB_CLASS_CDC_DATA);
    assert(cdc_dev->data.intf_desc->bNumEndpoints == 2);

    // Save Communication and Data protocols
    cdc_dev->comm_protocol = (cdc_comm_protocol_t)cdc_dev->notif.intf_desc->bInterfaceProtocol;
    cdc_dev->data_protocol = (cdc_data_protocol_t)cdc_dev->data.intf_desc->bInterfaceProtocol;

    // Allocate USB transfers, claim CDC interfaces and return CDC-ACM handle
    ESP_GOTO_ON_ERROR(cdc_acm_transfers_allocate(cdc_dev, notif_ep, in_ep, out_ep, dev_config->out_buffer_size), err, TAG,);
    ESP_GOTO_ON_ERROR(cdc_acm_start(cdc_dev, dev_config->event_cb, dev_config->data_cb, dev_config->user_arg), err, TAG,);
    *cdc_hdl_ret = (cdc_acm_dev_hdl_t)cdc_dev;
    xSemaphoreGive(p_cdc_acm_obj->open_close_mutex);
    return ESP_OK;

err:
    cdc_acm_device_remove(cdc_dev);
exit:
    xSemaphoreGive(p_cdc_acm_obj->open_close_mutex);
    *cdc_hdl_ret = NULL;
    return ret;
}

esp_err_t cdc_acm_host_open_vendor_specific(uint16_t vid, uint16_t pid, uint8_t interface_num, const cdc_acm_host_device_config_t *dev_config, cdc_acm_dev_hdl_t *cdc_hdl_ret)
{
    esp_err_t ret;
    CDC_ACM_CHECK(p_cdc_acm_obj, ESP_ERR_INVALID_STATE);
    CDC_ACM_CHECK(dev_config, ESP_ERR_INVALID_ARG);
    CDC_ACM_CHECK(cdc_hdl_ret, ESP_ERR_INVALID_ARG);

    xSemaphoreTake(p_cdc_acm_obj->open_close_mutex, portMAX_DELAY);

    // Find underlying USB device
    cdc_dev_t *cdc_dev;
    ESP_GOTO_ON_ERROR(
        cdc_acm_find_and_open_usb_device(vid, pid, dev_config->connection_timeout_ms, &cdc_dev),
        exit, TAG, "USB device with VID: 0x%04X, PID: 0x%04X not found", vid, pid);

    // Open procedure for CDC-ACM non-compliant devices:
    const usb_config_desc_t *config_desc;
    int desc_offset;
    ESP_ERROR_CHECK(usb_host_get_active_config_descriptor(cdc_dev->dev_hdl, &config_desc));
    cdc_dev->data.intf_desc = usb_parse_interface_descriptor(config_desc, interface_num, 0, &desc_offset);
    ESP_GOTO_ON_FALSE(
        cdc_dev->data.intf_desc,
        ESP_ERR_NOT_FOUND, err, TAG, "Required interfece no %d was not found.", interface_num);
    const int temp_offset = desc_offset; // Save this offset for later

    // The interface can have 2-3 endpoints. 2 for data and 1 optional for notifications
    const usb_ep_desc_t *in_ep = NULL;
    const usb_ep_desc_t *out_ep = NULL;
    const usb_ep_desc_t *notif_ep = NULL;
    int ep_idx = 0;
    if (cdc_dev->data.intf_desc->bNumEndpoints == 3) {
        // Notification channel does not have its dedicated interface (data and notif interface is the same)
        // First endpoint of this interface is used as notification channel
        cdc_dev->notif.intf_desc = cdc_dev->data.intf_desc;
        notif_ep = usb_parse_endpoint_descriptor_by_index(cdc_dev->data.intf_desc, 0, config_desc->wTotalLength, &desc_offset);
        desc_offset = temp_offset;
        ep_idx++;
    }

    for (int i = ep_idx; i < ep_idx + 2; i++) {
        const usb_ep_desc_t *this_ep = usb_parse_endpoint_descriptor_by_index(cdc_dev->data.intf_desc, i, config_desc->wTotalLength, &desc_offset);
        assert(this_ep);
        if (USB_EP_DESC_GET_EP_DIR(this_ep)) {
            in_ep = this_ep;
        } else {
            out_ep = this_ep;
        }
        desc_offset = temp_offset;
    }

    // Allocate USB transfers, claim CDC interfaces and return CDC-ACM handle
    ESP_GOTO_ON_ERROR(cdc_acm_transfers_allocate(cdc_dev, notif_ep, in_ep, out_ep, dev_config->out_buffer_size), err, TAG, );
    ESP_GOTO_ON_ERROR(cdc_acm_start(cdc_dev, dev_config->event_cb, dev_config->data_cb, dev_config->user_arg), err, TAG,);
    *cdc_hdl_ret = (cdc_acm_dev_hdl_t)cdc_dev;
    xSemaphoreGive(p_cdc_acm_obj->open_close_mutex);
    return ESP_OK;
err:
    cdc_acm_device_remove(cdc_dev);
exit:
    xSemaphoreGive(p_cdc_acm_obj->open_close_mutex);
    return ret;
}

esp_err_t cdc_acm_host_close(cdc_acm_dev_hdl_t cdc_hdl)
{
    CDC_ACM_CHECK(p_cdc_acm_obj, ESP_ERR_INVALID_STATE);
    CDC_ACM_CHECK(cdc_hdl, ESP_ERR_INVALID_ARG);

    xSemaphoreTake(p_cdc_acm_obj->open_close_mutex, portMAX_DELAY);

    cdc_dev_t *cdc_dev = (cdc_dev_t *)cdc_hdl;

    // Cancel polling of BULK IN and INTERRUPT IN endpoints
    cdc_dev->notif.cb = NULL;
    cdc_dev->data.in_cb = NULL;
    ESP_ERROR_CHECK(cdc_acm_reset_transfer_endpoint(cdc_dev->dev_hdl, cdc_dev->data.in_xfer));
    if (cdc_dev->notif.intf_desc != NULL) {
        ESP_ERROR_CHECK(cdc_acm_reset_transfer_endpoint(cdc_dev->dev_hdl, cdc_dev->notif.xfer));
    }

    // Release all interfaces
    ESP_ERROR_CHECK(usb_host_interface_release(p_cdc_acm_obj->cdc_acm_client_hdl, cdc_dev->dev_hdl, cdc_dev->data.intf_desc->bInterfaceNumber));
    if ((cdc_dev->notif.intf_desc != NULL) && (cdc_dev->notif.intf_desc != cdc_dev->data.intf_desc)) {
        ESP_ERROR_CHECK(usb_host_interface_release(p_cdc_acm_obj->cdc_acm_client_hdl, cdc_dev->dev_hdl, cdc_dev->notif.intf_desc->bInterfaceNumber));
    }

    CDC_ACM_ENTER_CRITICAL();
    SLIST_REMOVE(&p_cdc_acm_obj->cdc_devices_list, cdc_dev, cdc_dev_s, list_entry);
    CDC_ACM_EXIT_CRITICAL();

    cdc_acm_device_remove(cdc_dev);
    xSemaphoreGive(p_cdc_acm_obj->open_close_mutex);
    return ESP_OK;
}

/**
 * @brief Print CDC specific descriptor in human readable form
 *
 * This is a callback function that is called from USB Host library,
 * when it wants to print full configuration descriptor to stdout.
 *
 * @param[in] _desc CDC specific descriptor
 */
static void cdc_acm_print_desc(const usb_standard_desc_t *_desc)
{
    if (_desc->bDescriptorType != ((USB_CLASS_COMM << 4) | USB_B_DESCRIPTOR_TYPE_INTERFACE ))
    {
        // Quietly return in case that this descriptor is not CDC interface descriptor
        return;
    }

    switch (((cdc_header_desc_t *)_desc)->bDescriptorSubtype) {
    case USB_CDC_DESC_SUBTYPE_HEADER: {
        cdc_header_desc_t *desc = (cdc_header_desc_t *)_desc;
        printf("\t*** CDC Header Descriptor ***\n");
        printf("\tbcdCDC: %d.%d0\n", ((desc->bcdCDC >> 8) & 0xF), ((desc->bcdCDC >> 4) & 0xF));
        break;
    }
    case USB_CDC_DESC_SUBTYPE_CALL: {
        cdc_acm_call_desc_t *desc = (cdc_acm_call_desc_t *)_desc;
        printf("\t*** CDC Call Descriptor ***\n");
        printf("\tbmCapabilities: 0x%02X\n", desc->bmCapabilities.val);
        printf("\tbDataInterface: %d\n", desc->bDataInterface);
        break;
    }
    case USB_CDC_DESC_SUBTYPE_ACM: {
        cdc_acm_acm_desc_t *desc = (cdc_acm_acm_desc_t *)_desc;
        printf("\t*** CDC ACM Descriptor ***\n");
        printf("\tbmCapabilities: 0x%02X\n", desc->bmCapabilities.val);
        break;
    }
    case USB_CDC_DESC_SUBTYPE_UNION: {
        cdc_union_desc_t *desc = (cdc_union_desc_t *)_desc;
        printf("\t*** CDC Union Descriptor ***\n");
        printf("\tbControlInterface: %d\n", desc->bControlInterface);
        printf("\tbSubordinateInterface[0]: %d\n", desc->bSubordinateInterface[0]);
        break;
    }
    default:
        ESP_LOGW(TAG, "Unsupported CDC specific descriptor");
        break;
    }
}

void cdc_acm_host_desc_print(cdc_acm_dev_hdl_t cdc_hdl)
{
    assert(cdc_hdl);
    cdc_dev_t *cdc_dev = (cdc_dev_t *)cdc_hdl;

    const usb_device_desc_t *device_desc;
    const usb_config_desc_t *config_desc;
    ESP_ERROR_CHECK_WITHOUT_ABORT(usb_host_get_device_descriptor(cdc_dev->dev_hdl, &device_desc));
    ESP_ERROR_CHECK_WITHOUT_ABORT(usb_host_get_active_config_descriptor(cdc_dev->dev_hdl, &config_desc));
    usb_print_device_descriptor(device_desc);
    usb_print_config_descriptor(config_desc, cdc_acm_print_desc);
}

/**
 * @brief Check finished transfer status
 *
 * Return to on transfer completed OK.
 * Cancel the transfer and issue user's callback in case of an error.
 *
 * @param[in] transfer Transfer to be checked
 * @return true Transfer completed
 * @return false Transfer NOT completed
 */
static bool cdc_acm_is_transfer_completed(usb_transfer_t *transfer)
{
    cdc_dev_t *cdc_dev = (cdc_dev_t *)transfer->context;
    bool completed = false;

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
        // Transfer was not completed or cancelled by user. Inform user about this
        if (cdc_dev->notif.cb) {
            const cdc_acm_host_dev_event_data_t error_event = {
                .type = CDC_ACM_HOST_ERROR,
                .data.error = (int) transfer->status
            };
            cdc_dev->notif.cb((cdc_acm_dev_hdl_t) cdc_dev, &error_event, cdc_dev->cb_arg);
        }
    }
    return completed;
}

static void in_xfer_cb(usb_transfer_t *transfer)
{
    ESP_LOGD("CDC_ACM", "in xfer cb");
    cdc_dev_t *cdc_dev = (cdc_dev_t *)transfer->context;

    if (cdc_acm_is_transfer_completed(transfer)) {
        if (cdc_dev->data.in_cb) {
            cdc_dev->data.in_cb(transfer->data_buffer, transfer->actual_num_bytes, cdc_dev->cb_arg);
        }

        ESP_LOGD("CDC_ACM", "Submitting poll for BULK IN transfer");
        usb_host_transfer_submit(cdc_dev->data.in_xfer);
    }
}

static void notif_xfer_cb(usb_transfer_t *transfer)
{
    ESP_LOGD("CDC_ACM", "notif xfer cb");
    cdc_dev_t *cdc_dev = (cdc_dev_t *)transfer->context;

    if (cdc_acm_is_transfer_completed(transfer)) {
        cdc_notification_t *notif = (cdc_notification_t *)transfer->data_buffer;
        switch (notif->bNotificationCode) {
        case USB_CDC_NOTIF_NETWORK_CONNECTION: {
            if (cdc_dev->notif.cb) {
                const cdc_acm_host_dev_event_data_t net_conn_event = {
                    .type = CDC_ACM_HOST_NETWORK_CONNECTION,
                    .data.network_connected = (bool) notif->wValue
                };
                cdc_dev->notif.cb((cdc_acm_dev_hdl_t) cdc_dev, &net_conn_event, cdc_dev->cb_arg);
            }
            break;
        }
        case USB_CDC_NOTIF_SERIAL_STATE: {
            cdc_dev->serial_state.val = *((uint16_t *)notif->Data);
            if (cdc_dev->notif.cb) {
                const cdc_acm_host_dev_event_data_t serial_state_event = {
                    .type = CDC_ACM_HOST_SERIAL_STATE,
                    .data.serial_state = cdc_dev->serial_state
                };
                cdc_dev->notif.cb((cdc_acm_dev_hdl_t) cdc_dev, &serial_state_event, cdc_dev->cb_arg);
            }
            break;
        }
        case USB_CDC_NOTIF_RESPONSE_AVAILABLE: // Encapsulated commands not implemented - fallthrough
        default:
            ESP_LOGW("CDC_ACM", "Unsupported notification type 0x%02X", notif->bNotificationCode);
            ESP_LOG_BUFFER_HEX("CDC_ACM", transfer->data_buffer, transfer->actual_num_bytes);
            break;
        }

        // Start polling for new data again
        ESP_LOGD("CDC_ACM", "Submitting poll for INTR IN transfer");
        usb_host_transfer_submit(cdc_dev->notif.xfer);
    }
}

static void out_xfer_cb(usb_transfer_t *transfer)
{
    ESP_LOGD("CDC_ACM", "out/ctrl xfer cb");
    assert(transfer->context);
    xSemaphoreGive((SemaphoreHandle_t)transfer->context);
}

static void usb_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg)
{
    switch (event_msg->event) {
    case USB_HOST_CLIENT_EVENT_NEW_DEV:
        ESP_LOGD(TAG, "New device connected");
        break;
    case USB_HOST_CLIENT_EVENT_DEV_GONE: {
        ESP_LOGD(TAG, "Device suddenly disconnected");
        // Find CDC pseudo-devices associated with this USB device and close them
        cdc_dev_t *cdc_dev;
        cdc_dev_t *tcdc_dev;
        // We are using 'SAFE' version of 'SLIST_FOREACH' which enables user to close the disconnected device in the callback
        SLIST_FOREACH_SAFE(cdc_dev, &p_cdc_acm_obj->cdc_devices_list, list_entry, tcdc_dev) {
            if (cdc_dev->dev_hdl == event_msg->dev_gone.dev_hdl && cdc_dev->notif.cb) {
                // The suddenly disconnected device was opened by this driver: inform user about this
                const cdc_acm_host_dev_event_data_t disconn_event = {
                    .type = CDC_ACM_HOST_DEVICE_DISCONNECTED,
                };
                cdc_dev->notif.cb((cdc_acm_dev_hdl_t) cdc_dev, &disconn_event, cdc_dev->cb_arg);
            }
        }
        break;
    }
    default:
        assert(false);
        break;
    }
}

esp_err_t cdc_acm_host_data_tx_blocking(cdc_acm_dev_hdl_t cdc_hdl, const uint8_t *data, size_t data_len, uint32_t timeout_ms)
{
    esp_err_t ret;
    CDC_ACM_CHECK(cdc_hdl, ESP_ERR_INVALID_ARG);
    cdc_dev_t *cdc_dev = (cdc_dev_t *)cdc_hdl;
    CDC_ACM_CHECK(data && (data_len > 0), ESP_ERR_INVALID_ARG);
    CDC_ACM_CHECK(cdc_dev->data.out_xfer, ESP_ERR_NOT_SUPPORTED); // Device was opened as read-only.
    CDC_ACM_CHECK(data_len <= cdc_dev->data.out_xfer->data_buffer_size, ESP_ERR_INVALID_SIZE);

    // Take OUT mutex and fill the OUT transfer
    BaseType_t taken = xSemaphoreTake(cdc_dev->data.out_mux, pdMS_TO_TICKS(timeout_ms));
    if (taken != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    ESP_LOGD("CDC_ACM", "Submitting BULK OUT transfer");
    memcpy(cdc_dev->data.out_xfer->data_buffer, data, data_len);
    cdc_dev->data.out_xfer->num_bytes = data_len;
    cdc_dev->data.out_xfer->timeout_ms = timeout_ms;
    ESP_GOTO_ON_ERROR(usb_host_transfer_submit(cdc_dev->data.out_xfer), unblock, TAG,);

    // Wait for OUT transfer completion
    taken = xSemaphoreTake((SemaphoreHandle_t)cdc_dev->data.out_xfer->context, pdMS_TO_TICKS(timeout_ms));
    if (!taken) {
        // Reset the endpoint
        cdc_acm_reset_transfer_endpoint(cdc_dev->dev_hdl, cdc_dev->data.out_xfer);
        ret = ESP_ERR_TIMEOUT;
        goto unblock;
    }

    ESP_GOTO_ON_FALSE(cdc_dev->data.out_xfer->status == USB_TRANSFER_STATUS_COMPLETED, ESP_ERR_INVALID_RESPONSE, unblock, TAG, "Bulk OUT transfer error");
    ESP_GOTO_ON_FALSE(cdc_dev->data.out_xfer->actual_num_bytes == data_len, ESP_ERR_INVALID_RESPONSE, unblock, TAG, "Incorrect number of bytes transferred");
    ret = ESP_OK;

unblock:
    xSemaphoreGive(cdc_dev->data.out_mux);
    return ret;
}

esp_err_t cdc_acm_host_line_coding_get(cdc_acm_dev_hdl_t cdc_hdl, cdc_acm_line_coding_t *line_coding)
{
    CDC_ACM_CHECK(line_coding, ESP_ERR_INVALID_ARG);

    ESP_RETURN_ON_ERROR(
        send_cdc_request((cdc_dev_t *)cdc_hdl, true, USB_CDC_REQ_GET_LINE_CODING, (uint8_t *)line_coding, sizeof(cdc_acm_line_coding_t), 0),
        TAG,);
    ESP_LOGD(TAG, "Line Get: Rate: %d, Stop bits: %d, Parity: %d, Databits: %d", line_coding->dwDTERate,
             line_coding->bCharFormat, line_coding->bParityType, line_coding->bDataBits);
    return ESP_OK;
}

esp_err_t cdc_acm_host_line_coding_set(cdc_acm_dev_hdl_t cdc_hdl, const cdc_acm_line_coding_t *line_coding)
{
    CDC_ACM_CHECK(line_coding, ESP_ERR_INVALID_ARG);

    ESP_RETURN_ON_ERROR(
        send_cdc_request((cdc_dev_t *)cdc_hdl, false, USB_CDC_REQ_SET_LINE_CODING, (uint8_t *)line_coding, sizeof(cdc_acm_line_coding_t), 0),
        TAG,);
    ESP_LOGD(TAG, "Line Set: Rate: %d, Stop bits: %d, Parity: %d, Databits: %d", line_coding->dwDTERate,
             line_coding->bCharFormat, line_coding->bParityType, line_coding->bDataBits);
    return ESP_OK;
}

esp_err_t cdc_acm_host_set_control_line_state(cdc_acm_dev_hdl_t cdc_hdl, bool dtr, bool rts)
{
    const uint16_t ctrl_bitmap = (uint16_t)dtr | ((uint16_t)rts << 1);

    ESP_RETURN_ON_ERROR(
        send_cdc_request((cdc_dev_t *)cdc_hdl, false, USB_CDC_REQ_SET_CONTROL_LINE_STATE, NULL, 0, ctrl_bitmap),
        TAG,);
    ESP_LOGD(TAG, "Control Line Set: DTR: %d, RTS: %d", dtr, rts);
    return ESP_OK;
}

esp_err_t cdc_acm_host_send_break(cdc_acm_dev_hdl_t cdc_hdl, uint16_t duration_ms)
{
    ESP_RETURN_ON_ERROR(
        send_cdc_request((cdc_dev_t *)cdc_hdl, false, USB_CDC_REQ_SEND_BREAK, NULL, 0, duration_ms),
        TAG,);

    // Block until break is deasserted
    vTaskDelay(pdMS_TO_TICKS(duration_ms + 1));
    return ESP_OK;
}

esp_err_t cdc_acm_host_send_custom_request(cdc_acm_dev_hdl_t cdc_hdl, uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint8_t *data)
{
    CDC_ACM_CHECK(cdc_hdl, ESP_ERR_INVALID_ARG);
    cdc_dev_t *cdc_dev = (cdc_dev_t *)cdc_hdl;
    if (wLength > 0) {
        CDC_ACM_CHECK(data, ESP_ERR_INVALID_ARG);
    }
    CDC_ACM_CHECK(cdc_dev->ctrl_transfer->data_buffer_size >= wLength, ESP_ERR_INVALID_SIZE);

    esp_err_t ret;

    // Take Mutex and fill the CTRL request
    BaseType_t taken = xSemaphoreTake(cdc_dev->ctrl_mux, pdMS_TO_TICKS(5000));
    if (!taken) {
        return ESP_ERR_TIMEOUT;
    }
    usb_setup_packet_t *req = (usb_setup_packet_t *)(cdc_dev->ctrl_transfer->data_buffer);
    uint8_t *start_of_data = (uint8_t *)req + sizeof(usb_setup_packet_t);
    req->bmRequestType = bmRequestType;
    req->bRequest = bRequest;
    req->wValue = wValue;
    req->wIndex = wIndex;
    req->wLength = wLength;

    // For IN transfers we must transfer data ownership to CDC driver
    const bool in_transfer = bmRequestType & USB_BM_REQUEST_TYPE_DIR_IN;
    if (!in_transfer) {
        memcpy(start_of_data, data, wLength);
    }

    cdc_dev->ctrl_transfer->num_bytes = wLength + sizeof(usb_setup_packet_t);
    ESP_GOTO_ON_ERROR(
        usb_host_transfer_submit_control(p_cdc_acm_obj->cdc_acm_client_hdl, cdc_dev->ctrl_transfer),
        unblock, TAG, "CTRL transfer failed");

    taken = xSemaphoreTake((SemaphoreHandle_t)cdc_dev->ctrl_transfer->context, pdMS_TO_TICKS(5000)); // This is a fixed timeout. Every CDC device should be able to respond to CTRL transfer in 5 seconds
    if (!taken) {
        // Transfer was not finished, error in USB LIB. Reset the endpoint
        cdc_acm_reset_transfer_endpoint(cdc_dev->dev_hdl, cdc_dev->ctrl_transfer);
        ret = ESP_ERR_TIMEOUT;
        goto unblock;
    }

    ESP_GOTO_ON_FALSE(cdc_dev->ctrl_transfer->status == USB_TRANSFER_STATUS_COMPLETED, ESP_ERR_INVALID_RESPONSE, unblock, TAG, "Control transfer error");
    ESP_GOTO_ON_FALSE(cdc_dev->ctrl_transfer->actual_num_bytes == cdc_dev->ctrl_transfer->num_bytes, ESP_ERR_INVALID_RESPONSE, unblock, TAG, "Incorrect number of bytes transferred");

    // For OUT transfers, we must transfer data ownership to user
    if (in_transfer) {
        memcpy(data, start_of_data, wLength);
    }
    ret = ESP_OK;

unblock:
    xSemaphoreGive(cdc_dev->ctrl_mux);
    return ret;
}

static esp_err_t send_cdc_request(cdc_dev_t *cdc_dev, bool in_transfer, cdc_request_code_t request, uint8_t *data, uint16_t data_len, uint16_t value)
{
    CDC_ACM_CHECK(cdc_dev, ESP_ERR_INVALID_ARG);
    CDC_ACM_CHECK(cdc_dev->notif.intf_desc, ESP_ERR_NOT_SUPPORTED);

    uint8_t req_type = USB_BM_REQUEST_TYPE_TYPE_CLASS | USB_BM_REQUEST_TYPE_RECIP_INTERFACE;
    if (in_transfer) {
        req_type |= USB_BM_REQUEST_TYPE_DIR_IN;
    } else {
        req_type |= USB_BM_REQUEST_TYPE_DIR_OUT;
    }
    return cdc_acm_host_send_custom_request((cdc_acm_dev_hdl_t) cdc_dev, req_type, request, value, cdc_dev->notif.intf_desc->bInterfaceNumber, data_len, data);
}

esp_err_t cdc_acm_host_protocols_get(cdc_acm_dev_hdl_t cdc_hdl, cdc_comm_protocol_t *comm, cdc_data_protocol_t *data)
{
    CDC_ACM_CHECK(cdc_hdl, ESP_ERR_INVALID_ARG);
    cdc_dev_t *cdc_dev = (cdc_dev_t *)cdc_hdl;

    if (comm != NULL) *comm = cdc_dev->comm_protocol;
    if (data != NULL) *data = cdc_dev->data_protocol;
    return ESP_OK;
}
