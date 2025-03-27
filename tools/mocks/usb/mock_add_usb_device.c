/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "usb_private.h"
#include "usb/usb_types_ch9.h"
#include "mock_add_usb_device.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


#define MOCK_CHECK(cond, ret_val) ({                                        \
            if (!(cond)) {                                                  \
                return (ret_val);                                           \
            }                                                               \
})

#define MOCK_CHECK_EMPTY_DEV_LIST   ({                                      \
            if (!mocked_devices_count) {                                    \
                ESP_LOGW(MOCK_TAG, "Mocked devices list empty");            \
                return ESP_ERR_INVALID_SIZE;                                \
            }                                                               \
})

#define MAX_DEV_COUNT 128
#define IS_VALID_ADDRESS(address) ((address) != 0xFF)
#define IS_EMPTY_ADDRESS(address) ((address) == 0xFF)


const char *MOCK_TAG_CB = "USB MOCK CB";    // Tag for callback functions
const char *MOCK_TAG = "USB MOCK";          // Tag for the rest of the functions

typedef struct client_s client_t;
struct client_s {
    struct {
        SemaphoreHandle_t event_sem;
    } constant;
};

/**
 * @brief Mocked devices structure
 */
typedef struct {
    int address;                            /*!< Device address */
    unsigned opened;                        /*!< Device opened status */
    const usb_device_desc_t *dev_desc;      /*!< Device descriptor */
    const usb_config_desc_t *config_desc;   /*!< Config descriptor */
} device_list_t;

static device_list_t device_list[MAX_DEV_COUNT];
static unsigned mocked_devices_count = 0;


void usb_host_mock_dev_list_init(void)
{
    for(int index = 0; index < MAX_DEV_COUNT; index++) {
        device_list[index].address = 0xFF;
        device_list[index].opened = 0;
        device_list[index].dev_desc = NULL;
        device_list[index].config_desc = NULL;
    }
    mocked_devices_count = 0;
}

int usb_host_mock_get_devs_count(void)
{
    return mocked_devices_count;
}

esp_err_t usb_host_mock_add_device(uint8_t dev_address, const usb_device_desc_t *dev_desc, const usb_config_desc_t *config_desc)
{
    MOCK_CHECK(dev_address < MAX_DEV_COUNT && dev_desc != NULL && config_desc != NULL, ESP_ERR_INVALID_ARG);

    // Increase mocked_devices_count, only for new device addresses
    if (IS_EMPTY_ADDRESS(device_list[dev_address].address)) {
        mocked_devices_count++;
    }

    // Fill device_list with new device parameters
    device_list[dev_address].address = dev_address;
    device_list[dev_address].opened = 0;
    device_list[dev_address].dev_desc = dev_desc;
    device_list[dev_address].config_desc = config_desc;

    return ESP_OK;
}

/**
 * @brief Print devices from device_list by index
 *
 * @param[in] index device index
 */
static void _print_mocked_device(int index)
{
    ESP_LOGI(MOCK_TAG, "Device handle = %p", (void*)(&device_list[index]));
    ESP_LOGI(MOCK_TAG, "Device address = %d", device_list[index].address);
    ESP_LOGI(MOCK_TAG, "Device opened by = %d clients", device_list[index].opened);

    usb_print_device_descriptor(device_list[index].dev_desc);
    usb_print_config_descriptor(device_list[index].config_desc, NULL);
    printf("\n");
}

esp_err_t usb_host_mock_print_mocked_devices(uint8_t dev_address)
{
    MOCK_CHECK_EMPTY_DEV_LIST;

    // dev_address is 0xFF, print all devices from device_list
    if (IS_EMPTY_ADDRESS(dev_address)) {
        for(int index = 0; index < MAX_DEV_COUNT; index++) {
            if(IS_VALID_ADDRESS(device_list[index].address)) {
                _print_mocked_device(index);
            }
        }
    // Print only device at dev_address
    } else {
        if (IS_VALID_ADDRESS(device_list[dev_address].address)) {
            _print_mocked_device(dev_address);
        } else {
            ESP_LOGE(MOCK_TAG, "Mocked device at address %d does not exist", dev_address);
            return ESP_ERR_INVALID_ARG;
        }
    }
    return ESP_OK;
}

esp_err_t usb_host_mock_get_device_descriptor_by_address(uint8_t dev_address, const usb_device_desc_t **device_desc)
{
    MOCK_CHECK(device_desc != NULL && dev_address < MAX_DEV_COUNT, ESP_ERR_INVALID_ARG);
    ESP_LOGD(MOCK_TAG_CB, "Get device descriptor by device address");
    MOCK_CHECK_EMPTY_DEV_LIST;

    if (IS_VALID_ADDRESS(device_list[dev_address].address)) {
        *device_desc = device_list[dev_address].dev_desc;
    } else {
        ESP_LOGE(MOCK_TAG, "Mocked device at address %d does not exist", dev_address);
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t usb_host_mock_get_config_descriptor_by_address(uint8_t dev_address, const usb_config_desc_t **config_desc)
{
    MOCK_CHECK(config_desc != NULL && dev_address < MAX_DEV_COUNT, ESP_ERR_INVALID_ARG);
    ESP_LOGD(MOCK_TAG_CB, "Get configuration descriptor by device address");
    MOCK_CHECK_EMPTY_DEV_LIST;

    if (IS_VALID_ADDRESS(device_list[dev_address].address)) {
        *config_desc = device_list[dev_address].config_desc;
    } else {
        ESP_LOGE(MOCK_TAG, "Mocked device at address %d does not exist", dev_address);
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

// ------------------------------------------------- CMock callback functions ------------------------------------------

esp_err_t usb_host_transfer_alloc_mock_callback(size_t data_buffer_size, int num_isoc_packets, usb_transfer_t **transfer, int call_count)
{
    urb_t *urb = urb_alloc(data_buffer_size, num_isoc_packets);
    if (urb == NULL) {
        return ESP_ERR_NO_MEM;
    }
    *transfer = &urb->transfer;

    ESP_LOGD(MOCK_TAG_CB, "New transfer allocated: %p", *transfer);
    return ESP_OK;
}

esp_err_t usb_host_transfer_free_mock_callback(usb_transfer_t *transfer, int call_count)
{
    if (transfer == NULL) {
        return ESP_OK;
    }

    ESP_LOGD(MOCK_TAG_CB, "Transfer freed: %p", transfer);
    urb_t *urb = __containerof(transfer, urb_t, transfer);
    urb_free(urb);
    return ESP_OK;
}

esp_err_t usb_host_client_register_mock_callback(const usb_host_client_config_t *client_config, usb_host_client_handle_t *client_hdl_ret, int call_count)
{
    MOCK_CHECK(client_config != NULL && client_hdl_ret != NULL, ESP_ERR_INVALID_ARG);

    esp_err_t ret;
    // Create client object
    client_t *client_obj = (client_t*)calloc(1, sizeof(client_t));
    SemaphoreHandle_t event_sem = xSemaphoreCreateBinary();
    if (client_obj == NULL || event_sem == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto alloc_err;
    }

    client_obj->constant.event_sem = event_sem;

    ESP_LOGD(MOCK_TAG_CB, "USB Host client registered");

    // Write back client handle
    *client_hdl_ret = (usb_host_client_handle_t)client_obj;
    ret = ESP_OK;
    return ret;

alloc_err:
    if (event_sem) {
        vSemaphoreDelete(event_sem);
    }
    free(client_obj);
    return ret;
}

esp_err_t usb_host_client_handle_events_mock_callback(usb_host_client_handle_t client_hdl, TickType_t timeout_ticks, int call_count)
{
    MOCK_CHECK(client_hdl != NULL, ESP_ERR_INVALID_ARG);
    esp_err_t ret = (timeout_ticks == 0) ? ESP_OK : ESP_ERR_TIMEOUT;    // We don't want to return ESP_ERR_TIMEOUT if we aren't blocking
    client_t *client_obj = (client_t *)client_hdl;

    while (1) {
        // Loop until there are no more events
        if (xSemaphoreTake(client_obj->constant.event_sem, timeout_ticks) == pdFALSE) {
            // Timed out waiting for semaphore or currently no events
            break;
        }

        ret = ESP_OK;
        // Set timeout_ticks to 0 so that we can check for events again without blocking
        timeout_ticks = 0;
    }
    return ret;
}

esp_err_t usb_host_client_unblock_mock_callback(usb_host_client_handle_t client_hdl, int call_count)
{
    MOCK_CHECK(client_hdl != NULL, ESP_ERR_INVALID_ARG);
    client_t *client_obj = (client_t *)client_hdl;
    xSemaphoreGive(client_obj->constant.event_sem);
    return ESP_OK;
}

esp_err_t usb_host_client_deregister_mock_callback(usb_host_client_handle_t client_hdl, int call_count)
{
    MOCK_CHECK(client_hdl != NULL, ESP_ERR_INVALID_ARG);
    client_t *client_obj = (client_t *)client_hdl;

    vSemaphoreDelete(client_obj->constant.event_sem);
    free(client_obj);
    return ESP_OK;
}

esp_err_t usb_host_device_open_mock_callback(usb_host_client_handle_t client_hdl, uint8_t dev_address, usb_device_handle_t *dev_hdl_ret, int call_count)
{
    MOCK_CHECK(dev_address < MAX_DEV_COUNT && client_hdl != NULL && dev_hdl_ret != NULL, ESP_ERR_INVALID_ARG);
    // Find a device in dev_list by dev_address
    for (int index = 0; index < MAX_DEV_COUNT; index++) {
        if(device_list[index].address == dev_address) {

            // We should check, if the device has not been opened by the same client
            // But we are keeping this mock implementation simple

            // Device found in dev_list
            *dev_hdl_ret = (usb_device_handle_t)(&device_list[index]);
            device_list[index].opened++;

            return ESP_OK;
        }
    }

    // Device not found
    ESP_LOGW(MOCK_TAG_CB, "Device not found: dev_address = %d", dev_address);
    return ESP_ERR_NOT_FOUND;
}

esp_err_t usb_host_device_close_mock_callback(usb_host_client_handle_t client_hdl, usb_device_handle_t dev_hdl, int call_count)
{
    MOCK_CHECK(dev_hdl != NULL && client_hdl != NULL, ESP_ERR_INVALID_ARG);
    device_list_t* current_device = (device_list_t *) dev_hdl;

    if (current_device->opened == 0) {
        // Device was never opened
        ESP_LOGW(MOCK_TAG_CB, "Device was never opened: dev_address = %d", current_device->address);
        return ESP_ERR_INVALID_STATE;
    }

    // We should also check, if the device has been opened by this client
    // But we are keeping this mock implementation simple

    // Closing the device
    current_device->opened--;
    ESP_LOGD(MOCK_TAG_CB, "Device closed: dev_address = %d, dev_hdl = %p", current_device->address, current_device);
    return ESP_OK;
}

esp_err_t usb_host_device_addr_list_fill_mock_callback(int list_len, uint8_t *dev_addr_list, int *num_dev_ret, int call_count)
{
    MOCK_CHECK(dev_addr_list != NULL && num_dev_ret != NULL, ESP_ERR_INVALID_ARG);

    int found_devices_count = 0;
    for (int index = 0; index < MAX_DEV_COUNT; index++) {
        if(IS_VALID_ADDRESS(device_list[index].address) && (found_devices_count < list_len)) {
            dev_addr_list[found_devices_count++] = device_list[index].address;
        }
    }

    // Just print found devices
    ESP_LOGD(MOCK_TAG_CB, "%d USB Devices found", found_devices_count);
    for (int i = 0; i < found_devices_count; i++) {
        ESP_LOGD(MOCK_TAG_CB, "dev_addr_list[%d] = %d", i, dev_addr_list[i]);
    }

    *num_dev_ret = found_devices_count;
    return ESP_OK;
}

esp_err_t usb_host_get_device_descriptor_mock_callback(usb_device_handle_t dev_hdl, const usb_device_desc_t **device_desc, int call_count)
{
    MOCK_CHECK(dev_hdl != NULL && device_desc != NULL, ESP_ERR_INVALID_ARG);
    ESP_LOGD(MOCK_TAG_CB, "Get device descriptor");

    const device_list_t* current_device = (const device_list_t *) dev_hdl;
    *device_desc = current_device->dev_desc;
    return ESP_OK;
}

esp_err_t usb_host_get_active_config_descriptor_mock_callback(usb_device_handle_t dev_hdl, const usb_config_desc_t **config_desc, int call_count)
{
    MOCK_CHECK(dev_hdl != NULL && config_desc != NULL, ESP_ERR_INVALID_ARG);
    ESP_LOGD(MOCK_TAG_CB, "Get active config descriptor");

    const device_list_t* current_device = (const device_list_t *) dev_hdl;
    *config_desc = current_device->config_desc;
    return ESP_OK;
}

esp_err_t usb_host_transfer_submit_success_mock_callback(usb_transfer_t *transfer, int call_count)
{
    MOCK_CHECK(transfer != NULL, ESP_ERR_INVALID_ARG);
    // Check that transfer and target endpoint are valid
    MOCK_CHECK(transfer->device_handle != NULL, ESP_ERR_INVALID_ARG);   // Target device must be set
    MOCK_CHECK((transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK) != 0, ESP_ERR_INVALID_ARG);
    transfer->callback(transfer);
    transfer->status = USB_TRANSFER_STATUS_COMPLETED;
    transfer->actual_num_bytes = transfer->num_bytes;
    ESP_LOGD(MOCK_TAG_CB, "Mocked transfer submitted, buff len: %d, buff: %s", transfer->num_bytes, transfer->data_buffer);
    return ESP_OK;
}

esp_err_t usb_host_transfer_submit_invalid_response_mock_callback(usb_transfer_t *transfer, int call_count)
{
    MOCK_CHECK(transfer != NULL, ESP_ERR_INVALID_ARG);
    // Check that transfer and target endpoint are valid
    MOCK_CHECK(transfer->device_handle != NULL, ESP_ERR_INVALID_ARG);   // Target device must be set
    MOCK_CHECK((transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK) != 0, ESP_ERR_INVALID_ARG);
    transfer->callback(transfer);
    transfer->status = USB_TRANSFER_STATUS_ERROR;
    transfer->actual_num_bytes = 0;
    ESP_LOGD(MOCK_TAG_CB, "Mocked transfer submitted, buff len: %d, buff: %s", transfer->num_bytes, transfer->data_buffer);
    ESP_LOGW(MOCK_TAG_CB, "Mocked transfer error");
    return ESP_OK;
}

esp_err_t usb_host_transfer_submit_timeout_mock_callback(usb_transfer_t *transfer, int call_count)
{
    MOCK_CHECK(transfer != NULL, ESP_ERR_INVALID_ARG);
    // Check that transfer and target endpoint are valid
    MOCK_CHECK(transfer->device_handle != NULL, ESP_ERR_INVALID_ARG);   // Target device must be set
    MOCK_CHECK((transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK) != 0, ESP_ERR_INVALID_ARG);
    return ESP_OK;
}

esp_err_t usb_host_transfer_submit_control_success_mock_callback(usb_host_client_handle_t client_hdl, usb_transfer_t *transfer, int call_count)
{
    MOCK_CHECK(client_hdl != NULL && transfer != NULL, ESP_ERR_INVALID_ARG);
    // Check that control transfer is valid
    MOCK_CHECK(transfer->device_handle != NULL, ESP_ERR_INVALID_ARG);   // Target device must be set
    // Control transfers must be targeted at EP 0
    MOCK_CHECK((transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK) == 0, ESP_ERR_INVALID_ARG);

    transfer->status = USB_TRANSFER_STATUS_COMPLETED;
    transfer->actual_num_bytes = transfer->num_bytes;
    transfer->callback(transfer);
    ESP_LOGD(MOCK_TAG_CB, "CTRL mocked transfer submitted, buff len: %d, buff: %s", transfer->num_bytes, transfer->data_buffer);
    return ESP_OK;
}

esp_err_t usb_host_transfer_submit_control_invalid_response_mock_callback(usb_host_client_handle_t client_hdl, usb_transfer_t *transfer, int call_count)
{
    MOCK_CHECK(client_hdl != NULL && transfer != NULL, ESP_ERR_INVALID_ARG);
    // Check that control transfer is valid
    MOCK_CHECK(transfer->device_handle != NULL, ESP_ERR_INVALID_ARG);   // Target device must be set
    // Control transfers must be targeted at EP 0
    MOCK_CHECK((transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK) == 0, ESP_ERR_INVALID_ARG);

    transfer->status = USB_TRANSFER_STATUS_ERROR;
    transfer->actual_num_bytes = 0;
    transfer->callback(transfer);
    ESP_LOGD(MOCK_TAG_CB, "CTRL mocked transfer submitted, buff len: %d, buff: %s", transfer->num_bytes, transfer->data_buffer);
    ESP_LOGW(MOCK_TAG_CB, "CTRL mocked transfer error");
    return ESP_OK;
}

esp_err_t usb_host_transfer_submit_control_timeout_mock_callback(usb_host_client_handle_t client_hdl, usb_transfer_t *transfer, int call_count)
{
    MOCK_CHECK(client_hdl != NULL && transfer != NULL, ESP_ERR_INVALID_ARG);
    // Check that control transfer is valid
    MOCK_CHECK(transfer->device_handle != NULL, ESP_ERR_INVALID_ARG);   // Target device must be set
    // Control transfers must be targeted at EP 0
    MOCK_CHECK((transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK) == 0, ESP_ERR_INVALID_ARG);
    return ESP_OK;
}

esp_err_t usb_host_device_info_mock_callback(usb_device_handle_t dev_hdl, usb_device_info_t *dev_info, int call_count)
{
    MOCK_CHECK(dev_hdl != NULL && dev_info != NULL, ESP_ERR_INVALID_ARG);

    const device_list_t *current_device = (const device_list_t *) dev_hdl;
    memset(dev_info, 0, sizeof(usb_device_info_t));
    dev_info->dev_addr = current_device->address;
    dev_info->bMaxPacketSize0 = current_device->dev_desc->bMaxPacketSize0;
    dev_info->bConfigurationValue = 1;
    return ESP_OK;
}
