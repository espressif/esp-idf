/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "usb_private.h"
#include "hcd.h"
#include "hub.h"

/*
Implementation of the HUB driver that only supports the root hub with a single port. Therefore, we currently don't
implement the bare minimum to control the root HCD port.
*/

#define HUB_ROOT_PORT_NUM                           1   //HCD only supports one port
#ifdef CONFIG_USB_HOST_HW_BUFFER_BIAS_IN
#define HUB_ROOT_HCD_PORT_FIFO_BIAS                 HCD_PORT_FIFO_BIAS_RX
#elif CONFIG_USB_HOST_HW_BUFFER_BIAS_PERIODIC_OUT
#define HUB_ROOT_HCD_PORT_FIFO_BIAS                 HCD_PORT_FIFO_BIAS_PTX
#else   //CONFIG_USB_HOST_HW_BUFFER_BIAS_BALANCED
#define HUB_ROOT_HCD_PORT_FIFO_BIAS                 HCD_PORT_FIFO_BIAS_BALANCED
#endif

#define ENUM_CTRL_TRANSFER_MAX_LEN                  CONFIG_USB_HOST_CONTROL_TRANSFER_MAX_SIZE
#define ENUM_DEV_ADDR                               1       //Device address used in enumeration
#define ENUM_CONFIG_INDEX                           0       //Index of the first configuration of the device
#define ENUM_DEV_DESC_REQ_SIZE                      64      //Worst case size for device descriptor request
#define ENUM_LOW_SPEED_MPS                          8       //Worst case MPS for the default endpoint of a low-speed device
#define ENUM_FULL_SPEED_MPS                         64      //Worst case MPS for the default endpoint of a full-speed device

#define HUB_DRIVER_FLAG_ACTION_ROOT_EVENT           0x01
#define HUB_DRIVER_FLAG_ACTION_ENUM_EVENT           0x02
#define HUB_DRIVER_FLAG_ACTION_PORT_RECOVER         0x04

typedef enum {
    HUB_DRIVER_STATE_INSTALLED,
    HUB_DRIVER_STATE_ROOT_POWERD,
    HUB_DRIVER_STATE_ROOT_ENUMERATING,
    HUB_DRIVER_STATE_ROOT_ACTIVE,
    HUB_DRIVER_STATE_ROOT_RECOVERY,
} hub_driver_state_t;

typedef enum {
    ENUM_STAGE_NONE,                                /**< There is no device awaiting enumeration */
    ENUM_STAGE_START,                               /**< Start of enumeration. Allocates device object */
    ENUM_STAGE_GET_SHORT_DEV_DESC,                  /**< Getting short device descriptor */
    ENUM_STAGE_GET_SHORT_DEV_DESC_CHECK,            /**< Check that short device descriptor was obtained */
    ENUM_STAGE_SECOND_RESET,                        /**< Doing second reset */
    ENUM_STAGE_SET_ADDR,                            /**< Setting address */
    ENUM_STAGE_SET_ADDR_CHECK,                      /**< Check that address was set successful */
    ENUM_STAGE_GET_FULL_DEV_DESC,                   /**< Getting full device descriptor */
    ENUM_STAGE_GET_FULL_DEV_DESC_CHECK,             /**< Check that full device descriptor was obtained */
    ENUM_STAGE_GET_CONFIG_DESC,                     /**< Getting full configuration descriptor */
    ENUM_STAGE_GET_CONFIG_DESC_CHECK,               /**< Check that full configuration descriptor was obtained */
    ENUM_STAGE_SET_CONFIG,                          /**< Set configuration number */
    ENUM_STAGE_SET_CONFIG_CHECK,                    /**< Check that configuration number was set */
    ENUM_STAGE_CLEANUP,                             /**< Clean up successful enumeration. Adds enumerated device to USBH */
    ENUM_STAGE_CLEANUP_FAILED,                      /**< Cleanup failed enumeration. Free device resources */
} enum_stage_t;

typedef struct {
    //Dynamic members require a critical section
    struct {
        union {
            struct {
                uint32_t actions: 8;
                uint32_t reserved24: 24;
            };
            uint32_t val;
        } flags;
        hub_driver_state_t driver_state;
        usb_device_handle_t root_dev_hdl;   //Indicates if an enumerated device is connected to the root port
    } dynamic;
    //Single thread members don't require a critical section so long as they are never accessed from multiple threads
    struct {
        enum_stage_t enum_stage;
        urb_t *enum_urb;
        usb_device_handle_t enum_dev_hdl;   //Handle of the device being enumerated. Moves to root_dev_hdl on enumeration completion
        hcd_pipe_handle_t enum_dflt_pipe_hdl;
    } single_thread;
    //Constant members do no change after installation thus do not require a critical section
    struct {
        hcd_port_handle_t root_port_hdl;
        usb_notif_cb_t notif_cb;
        void *notif_cb_arg;
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

// ------------------------------------------------- Event Handling ----------------------------------------------------

// ---------------------- Callbacks ------------------------

static bool root_port_callback(hcd_port_handle_t port_hdl, hcd_port_event_t port_event, void *user_arg, bool in_isr)
{
    HUB_DRIVER_ENTER_CRITICAL_SAFE();
    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_ROOT_EVENT;
    HUB_DRIVER_EXIT_CRITICAL_SAFE();
    bool yield;
    if (in_isr) {
        p_hub_driver_obj->constant.notif_cb(USB_NOTIF_SOURCE_HUB, in_isr, p_hub_driver_obj->constant.notif_cb_arg);
    } else {
        yield = false;
    }
    return yield;
}

static bool enum_dflt_pipe_callback(hcd_pipe_handle_t pipe_hdl, hcd_pipe_event_t pipe_event, void *user_arg, bool in_isr)
{
    //Note: This callback may have triggered when a failed enumeration is already cleaned up (e.g., due to a failed port reset)
    HUB_DRIVER_ENTER_CRITICAL_SAFE();
    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_ENUM_EVENT;
    HUB_DRIVER_EXIT_CRITICAL_SAFE();
    return p_hub_driver_obj->constant.notif_cb(USB_NOTIF_SOURCE_HUB, in_isr, p_hub_driver_obj->constant.notif_cb_arg);
}

static void usbh_hub_callback(hcd_port_handle_t port_hdl, usbh_hub_event_t hub_event, void *arg)
{
    //We currently only support the root port
    assert(port_hdl == p_hub_driver_obj->constant.root_port_hdl);
    HUB_DRIVER_ENTER_CRITICAL();
    //Any hub_event results in whatever device connected to the root port to no longer be valid. We clear root_dev_hdl here.
    usb_device_handle_t dev_hdl = p_hub_driver_obj->dynamic.root_dev_hdl;
    p_hub_driver_obj->dynamic.root_dev_hdl = NULL;
    assert(dev_hdl);
    bool call_port_disable = false;
    switch (hub_event) {
        case USBH_HUB_EVENT_CLEANUP_PORT:
            //After USBH has cleaned up gone device. The port can be recovered.
            p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_PORT_RECOVER;
            break;
        case USBH_HUB_EVENT_DISABLE_PORT:
            p_hub_driver_obj->dynamic.driver_state = HUB_DRIVER_STATE_ROOT_POWERD;
            call_port_disable = true;
            break;
        default:
            abort();    //Should never occur
            break;
    }
    HUB_DRIVER_EXIT_CRITICAL();
    if (call_port_disable) {
        ESP_LOGD(HUB_DRIVER_TAG, "Disabling root port");
        hcd_port_command(port_hdl, HCD_PORT_CMD_DISABLE);
        ESP_ERROR_CHECK(usbh_hub_dev_port_disabled(dev_hdl));
    }
}

// ---------------------- Handlers -------------------------

static void root_port_handle_events(hcd_port_handle_t root_port_hdl)
{
    hcd_port_event_t port_event = hcd_port_handle_event(root_port_hdl);
    switch (port_event) {
        case HCD_PORT_EVENT_NONE:
            //Nothing to do
            break;
        case HCD_PORT_EVENT_CONNECTION: {
            if (hcd_port_command(root_port_hdl, HCD_PORT_CMD_RESET) == ESP_OK) {
                ESP_LOGD(HUB_DRIVER_TAG, "Root port reset");
                //Start enumeration
                HUB_DRIVER_ENTER_CRITICAL();
                p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_ENUM_EVENT;
                p_hub_driver_obj->dynamic.driver_state = HUB_DRIVER_STATE_ROOT_ENUMERATING;
                HUB_DRIVER_EXIT_CRITICAL();
                p_hub_driver_obj->single_thread.enum_stage = ENUM_STAGE_START;
            } else {
                ESP_LOGE(HUB_DRIVER_TAG, "Root port reset failed");
            }
            break;
        }
        case HCD_PORT_EVENT_DISCONNECTION:
        case HCD_PORT_EVENT_ERROR:
        case HCD_PORT_EVENT_OVERCURRENT: {
            usb_device_handle_t dev_hdl = NULL;
            HUB_DRIVER_ENTER_CRITICAL();
            switch (p_hub_driver_obj->dynamic.driver_state) {
                case HUB_DRIVER_STATE_ROOT_POWERD:
                    //This occurred before enumeration. Therefore, there's no device and we can go straight to port recovery
                    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_ENUM_EVENT;
                    break;
                case HUB_DRIVER_STATE_ROOT_ENUMERATING:
                    //This occurred during enumeration. Therefore, we need to recover the failed enumeration
                    p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_ENUM_EVENT;
                    p_hub_driver_obj->single_thread.enum_stage = ENUM_STAGE_CLEANUP_FAILED;
                    break;
                case HUB_DRIVER_STATE_ROOT_ACTIVE:
                    //There was an enumerated device. We need to indicate to USBH that the device is gone
                    dev_hdl = p_hub_driver_obj->dynamic.root_dev_hdl;
                    break;
                default:
                    abort();    //Should never occur
                    break;
            }
            p_hub_driver_obj->dynamic.driver_state = HUB_DRIVER_STATE_ROOT_RECOVERY;
            HUB_DRIVER_EXIT_CRITICAL();
            if (dev_hdl) {
                ESP_ERROR_CHECK(usbh_hub_mark_dev_gone(dev_hdl));
            }
            break;
        }
        default:
            abort();    //Should never occur
            break;
    }
}

// ------------------------------------------------- Enum Functions ----------------------------------------------------

static enum_stage_t enum_stage_start(void)
{
    usb_speed_t speed;
    if (hcd_port_get_speed(p_hub_driver_obj->constant.root_port_hdl, &speed) != ESP_OK) {
        return ENUM_STAGE_NONE;
    }
    usb_device_handle_t enum_dev_hdl;
    hcd_pipe_handle_t enum_dflt_pipe_hdl;
    //We use NULL as the parent device to indicate the root hub port 1. We currently only support a single device
    if (usbh_hub_add_dev(p_hub_driver_obj->constant.root_port_hdl, speed, &enum_dev_hdl, &enum_dflt_pipe_hdl) != ESP_OK) {
        return ENUM_STAGE_NONE;
    }
    //Set our own default pipe callback
    ESP_ERROR_CHECK(hcd_pipe_update_callback(enum_dflt_pipe_hdl, enum_dflt_pipe_callback, NULL));
    HUB_DRIVER_ENTER_CRITICAL();
    p_hub_driver_obj->single_thread.enum_dev_hdl = enum_dev_hdl;
    p_hub_driver_obj->single_thread.enum_dflt_pipe_hdl = enum_dflt_pipe_hdl;
    HUB_DRIVER_EXIT_CRITICAL();
    ESP_LOGD(HUB_DRIVER_TAG, "Enumeration starting");
    return ENUM_STAGE_GET_SHORT_DEV_DESC;
}

static enum_stage_t enum_stage_get_short_dev_desc(hcd_pipe_handle_t pipe_hdl, urb_t *enum_urb)
{
    USB_SETUP_PACKET_INIT_GET_DEVICE_DESC((usb_setup_packet_t *)enum_urb->transfer.data_buffer);
    enum_urb->transfer.num_bytes = sizeof(usb_setup_packet_t) + ENUM_DEV_DESC_REQ_SIZE;
    if (hcd_urb_enqueue(pipe_hdl, enum_urb) != ESP_OK) {
        ESP_LOGE(HUB_DRIVER_TAG, "Failed to get short device descriptor");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    ESP_LOGD(HUB_DRIVER_TAG, "Getting short device descriptor");
    return ENUM_STAGE_GET_SHORT_DEV_DESC_CHECK;
}

static enum_stage_t enum_stage_check_short_dev_desc(hcd_pipe_handle_t pipe_hdl, urb_t *enum_urb)
{
    //Dequeue the URB
    urb_t *dequeued_enum_urb = hcd_urb_dequeue(pipe_hdl);
    assert(dequeued_enum_urb == enum_urb);
    if (enum_urb->transfer.status != USB_TRANSFER_STATUS_COMPLETED) {
        ESP_LOGE(HUB_DRIVER_TAG, "Short device descriptor transfer failed");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    usb_device_desc_t *device_desc = (usb_device_desc_t *)(enum_urb->transfer.data_buffer + sizeof(usb_setup_packet_t));
    if (enum_urb->transfer.actual_num_bytes < sizeof(usb_setup_packet_t) + 8) {  //Device must return at least 8 bytes in its data stage
        ESP_LOGE(HUB_DRIVER_TAG, "Short device descriptor too short");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    //Update the MPS of the default pipe
    if (hcd_pipe_update_mps(pipe_hdl, device_desc->bMaxPacketSize0) != ESP_OK) {
        ESP_LOGE(HUB_DRIVER_TAG, "Failed to update default pipe MPS");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    ESP_LOGD(HUB_DRIVER_TAG, "Short device descriptor obtained");
    return ENUM_STAGE_SECOND_RESET;
}

static enum_stage_t enum_stage_second_reset(hcd_pipe_handle_t pipe_hdl, urb_t *enum_urb)
{
    ESP_ERROR_CHECK(hcd_pipe_set_persist_reset(pipe_hdl));  //Persist the default pipe through the reset
    if (hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_RESET) != ESP_OK) {
        ESP_LOGE(HUB_DRIVER_TAG, "Failed to issue second reset");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    ESP_LOGD(HUB_DRIVER_TAG, "Second reset done");
    return ENUM_STAGE_SET_ADDR;
}

static enum_stage_t enum_stage_set_addr(hcd_pipe_handle_t pipe_hdl, urb_t *enum_urb)
{
    USB_SETUP_PACKET_INIT_SET_ADDR((usb_setup_packet_t *)enum_urb->transfer.data_buffer, ENUM_DEV_ADDR);
    enum_urb->transfer.num_bytes = sizeof(usb_setup_packet_t);   //No data stage
    if (hcd_urb_enqueue(pipe_hdl, enum_urb) != ESP_OK) {
        ESP_LOGE(HUB_DRIVER_TAG, "Failed to set address");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    ESP_LOGD(HUB_DRIVER_TAG, "Setting address %d", ENUM_DEV_ADDR);
    return ENUM_STAGE_SET_ADDR_CHECK;
}

static enum_stage_t enum_stage_check_set_addr(hcd_pipe_handle_t pipe_hdl, urb_t *enum_urb, usb_device_handle_t dev_hdl)
{
    //Dequeue the URB
    urb_t *dequeued_enum_urb = hcd_urb_dequeue(pipe_hdl);
    assert(dequeued_enum_urb == enum_urb);
    if (enum_urb->transfer.status != USB_TRANSFER_STATUS_COMPLETED) {
        ESP_LOGE(HUB_DRIVER_TAG, "Set address transfer failed");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    //Update the pipe and device's address, and fill the address into the device object
    ESP_ERROR_CHECK(hcd_pipe_update_dev_addr(pipe_hdl, ENUM_DEV_ADDR));
    ESP_ERROR_CHECK(usbh_hub_enum_fill_dev_addr(dev_hdl, ENUM_DEV_ADDR));
    ESP_LOGD(HUB_DRIVER_TAG, "Address set successfully");
    return ENUM_STAGE_GET_FULL_DEV_DESC;
}

static enum_stage_t enum_stage_get_full_dev_desc(hcd_pipe_handle_t pipe_hdl, urb_t *enum_urb)
{
    USB_SETUP_PACKET_INIT_GET_DEVICE_DESC((usb_setup_packet_t *)enum_urb->transfer.data_buffer);
    enum_urb->transfer.num_bytes = sizeof(usb_setup_packet_t) + ENUM_DEV_DESC_REQ_SIZE;
    if (hcd_urb_enqueue(pipe_hdl, enum_urb) != ESP_OK) {
        ESP_LOGE(HUB_DRIVER_TAG, "Failed to get full device descriptor");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    ESP_LOGD(HUB_DRIVER_TAG, "Getting full device descriptor");
    return ENUM_STAGE_GET_FULL_DEV_DESC_CHECK;
}

static enum_stage_t enum_stage_check_full_dev_desc(hcd_pipe_handle_t pipe_hdl, urb_t *enum_urb, usb_device_handle_t dev_hdl)
{
    //Dequeue the URB
    urb_t *dequeued_enum_urb = hcd_urb_dequeue(pipe_hdl);
    assert(dequeued_enum_urb == enum_urb);
    if (enum_urb->transfer.status != USB_TRANSFER_STATUS_COMPLETED) {
        ESP_LOGE(HUB_DRIVER_TAG, "Full device descriptor transfer failed");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    if (enum_urb->transfer.actual_num_bytes < sizeof(usb_setup_packet_t) + sizeof(usb_device_desc_t)) {
        ESP_LOGE(HUB_DRIVER_TAG, "Full device descriptor too short");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    //Fill device descriptor into device
    const usb_device_desc_t *device_desc = (const usb_device_desc_t *)(enum_urb->transfer.data_buffer + sizeof(usb_setup_packet_t));
    ESP_ERROR_CHECK(usbh_hub_enum_fill_dev_desc(dev_hdl, device_desc));
    ESP_LOGD(HUB_DRIVER_TAG, "Full device descriptor obtained");
    return ENUM_STAGE_GET_CONFIG_DESC;
}

static enum_stage_t enum_stage_get_config_desc(hcd_pipe_handle_t pipe_hdl, urb_t *enum_urb)
{
    //Get the configuration descriptor at index 0
    USB_SETUP_PACKET_INIT_GET_CONFIG_DESC((usb_setup_packet_t *)enum_urb->transfer.data_buffer, ENUM_CONFIG_INDEX, ENUM_CTRL_TRANSFER_MAX_LEN);
    enum_urb->transfer.num_bytes = sizeof(usb_setup_packet_t) + ENUM_CTRL_TRANSFER_MAX_LEN;
    if (hcd_urb_enqueue(pipe_hdl, enum_urb) != ESP_OK) {
        ESP_LOGE(HUB_DRIVER_TAG, "Failed to get configuration descriptor");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    ESP_LOGD(HUB_DRIVER_TAG, "Getting configuration descriptor");
    return ENUM_STAGE_GET_CONFIG_DESC_CHECK;
}

static enum_stage_t enum_stage_check_config_desc(hcd_pipe_handle_t pipe_hdl, urb_t *enum_urb, usb_device_handle_t dev_hdl)
{
    //Dequeue the URB
    urb_t *dequeued_enum_urb = hcd_urb_dequeue(pipe_hdl);
    assert(dequeued_enum_urb == enum_urb);
    if (enum_urb->transfer.status != USB_TRANSFER_STATUS_COMPLETED) {
        ESP_LOGE(HUB_DRIVER_TAG, "Configuration descriptor transfer failed");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    usb_config_desc_t *config_desc = (usb_config_desc_t *)(enum_urb->transfer.data_buffer + sizeof(usb_setup_packet_t));
    if (enum_urb->transfer.actual_num_bytes < sizeof(usb_setup_packet_t) + sizeof(usb_config_desc_t)) {
        ESP_LOGE(HUB_DRIVER_TAG, "Configuration descriptor too small");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    if (config_desc->wTotalLength > ENUM_CTRL_TRANSFER_MAX_LEN) {
        ESP_LOGE(HUB_DRIVER_TAG, "Configuration descriptor larger than control transfer max length");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    //Fill configuration descriptor into device
    ESP_ERROR_CHECK(usbh_hub_enum_fill_config_desc(dev_hdl, config_desc));
    return ENUM_STAGE_SET_CONFIG;
}

static enum_stage_t enum_stage_set_config(hcd_pipe_handle_t pipe_hdl, urb_t *enum_urb)
{
    USB_SETUP_PACKET_INIT_SET_CONFIG((usb_setup_packet_t *)enum_urb->transfer.data_buffer, ENUM_CONFIG_INDEX + 1);
    enum_urb->transfer.num_bytes = sizeof(usb_setup_packet_t);   //No data stage
    if (hcd_urb_enqueue(pipe_hdl, enum_urb) != ESP_OK) {
        ESP_LOGE(HUB_DRIVER_TAG, "Failed to set configuration");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    ESP_LOGD(HUB_DRIVER_TAG, "Setting configuration");
    return ENUM_STAGE_SET_CONFIG_CHECK;
}

static enum_stage_t enum_stage_check_config(hcd_pipe_handle_t pipe_hdl, urb_t *enum_urb)
{
    //Dequeue the URB
    urb_t *dequeued_enum_urb = hcd_urb_dequeue(pipe_hdl);
    assert(dequeued_enum_urb == enum_urb);
    if (enum_urb->transfer.status != USB_TRANSFER_STATUS_COMPLETED) {
        ESP_LOGE(HUB_DRIVER_TAG, "Set configuration transfer failed");
        return ENUM_STAGE_CLEANUP_FAILED;
    }
    ESP_LOGD(HUB_DRIVER_TAG, "Configuration set successfully");
    return ENUM_STAGE_CLEANUP;
}

static enum_stage_t enum_stage_cleanup(void)
{
    //We currently only support a single device connected to the root port. Move the device handle from enum to root
    HUB_DRIVER_ENTER_CRITICAL();
    p_hub_driver_obj->dynamic.root_dev_hdl = p_hub_driver_obj->single_thread.enum_dev_hdl;
    p_hub_driver_obj->dynamic.driver_state = HUB_DRIVER_STATE_ROOT_ACTIVE;
    HUB_DRIVER_EXIT_CRITICAL();
    usb_device_handle_t dev_hdl = p_hub_driver_obj->single_thread.enum_dev_hdl;
    p_hub_driver_obj->single_thread.enum_dev_hdl = NULL;
    p_hub_driver_obj->single_thread.enum_dflt_pipe_hdl = NULL;
    //Update device object after enumeration is done
    ESP_ERROR_CHECK(usbh_hub_enum_done(dev_hdl));
    return ENUM_STAGE_NONE;
}

static enum_stage_t enum_stage_cleanup_failed(void)
{
    //Enumeration failed. Clear the enum device handle and pipe
    usb_device_handle_t dev_hdl = p_hub_driver_obj->single_thread.enum_dev_hdl;
    if (p_hub_driver_obj->single_thread.enum_dev_hdl) {
        //Halt, flush, and dequeue enum default pipe
        ESP_ERROR_CHECK(hcd_pipe_command(p_hub_driver_obj->single_thread.enum_dflt_pipe_hdl, HCD_PIPE_CMD_HALT));
        ESP_ERROR_CHECK(hcd_pipe_command(p_hub_driver_obj->single_thread.enum_dflt_pipe_hdl, HCD_PIPE_CMD_FLUSH));
        urb_t *dequeued_enum_urb = hcd_urb_dequeue(p_hub_driver_obj->single_thread.enum_dflt_pipe_hdl);
        assert(dequeued_enum_urb == p_hub_driver_obj->single_thread.enum_urb);
        ESP_ERROR_CHECK(usbh_hub_enum_failed(dev_hdl)); //Free the underlying device object first before recovering the port
    }
    p_hub_driver_obj->single_thread.enum_dev_hdl = NULL;
    p_hub_driver_obj->single_thread.enum_dflt_pipe_hdl = NULL;
    HUB_DRIVER_ENTER_CRITICAL();
    //Enum could have failed due to a port error. If so, we need to trigger a port recovery
    if (p_hub_driver_obj->dynamic.driver_state == HUB_DRIVER_STATE_ROOT_RECOVERY) {
        p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_PORT_RECOVER;
    }
    HUB_DRIVER_EXIT_CRITICAL();
    return ENUM_STAGE_NONE;
}

static void enum_handle_events(void)
{
    enum_stage_t cur_stage = p_hub_driver_obj->single_thread.enum_stage;
    hcd_pipe_handle_t enum_dflt_pipe_hdl = p_hub_driver_obj->single_thread.enum_dflt_pipe_hdl;
    urb_t *enum_urb = p_hub_driver_obj->single_thread.enum_urb;
    usb_device_handle_t dev_hdl = p_hub_driver_obj->single_thread.enum_dev_hdl;
    enum_stage_t next_stage;
    switch (cur_stage) {
        case ENUM_STAGE_START:
            next_stage = enum_stage_start();
            break;
        case ENUM_STAGE_GET_SHORT_DEV_DESC:
            next_stage = enum_stage_get_short_dev_desc(enum_dflt_pipe_hdl, enum_urb);
            break;
        case ENUM_STAGE_GET_SHORT_DEV_DESC_CHECK:
            next_stage = enum_stage_check_short_dev_desc(enum_dflt_pipe_hdl, enum_urb);
            break;
        case ENUM_STAGE_SECOND_RESET:
            next_stage = enum_stage_second_reset(enum_dflt_pipe_hdl, enum_urb);
            break;
        case ENUM_STAGE_SET_ADDR:
            next_stage = enum_stage_set_addr(enum_dflt_pipe_hdl, enum_urb);
            break;
        case ENUM_STAGE_SET_ADDR_CHECK:
            next_stage = enum_stage_check_set_addr(enum_dflt_pipe_hdl, enum_urb, dev_hdl);
            break;
        case ENUM_STAGE_GET_FULL_DEV_DESC:
            next_stage = enum_stage_get_full_dev_desc(enum_dflt_pipe_hdl, enum_urb);
            break;
        case ENUM_STAGE_GET_FULL_DEV_DESC_CHECK:
            next_stage = enum_stage_check_full_dev_desc(enum_dflt_pipe_hdl, enum_urb, dev_hdl);
            break;
        case ENUM_STAGE_GET_CONFIG_DESC:
            next_stage = enum_stage_get_config_desc(enum_dflt_pipe_hdl, enum_urb);
            break;
        case ENUM_STAGE_GET_CONFIG_DESC_CHECK:
            next_stage = enum_stage_check_config_desc(enum_dflt_pipe_hdl, enum_urb, dev_hdl);
            break;
        case ENUM_STAGE_SET_CONFIG:
            next_stage = enum_stage_set_config(enum_dflt_pipe_hdl, enum_urb);
            break;
        case ENUM_STAGE_SET_CONFIG_CHECK:
            next_stage = enum_stage_check_config(enum_dflt_pipe_hdl, enum_urb);
            break;
        case ENUM_STAGE_CLEANUP:
            next_stage = enum_stage_cleanup();
            break;
        case ENUM_STAGE_CLEANUP_FAILED:
            next_stage = enum_stage_cleanup_failed();
            break;
        default:
            //Note: Don't abort here. The enum_dflt_pipe_callback() can trigger a HUB_DRIVER_FLAG_ACTION_ENUM_EVENT after a cleanup
            next_stage = ENUM_STAGE_NONE;
            break;
    }
    p_hub_driver_obj->single_thread.enum_stage = next_stage;
    HUB_DRIVER_ENTER_CRITICAL();
    if (next_stage == ENUM_STAGE_GET_SHORT_DEV_DESC ||
        next_stage == ENUM_STAGE_SECOND_RESET ||
        next_stage == ENUM_STAGE_SET_ADDR ||
        next_stage == ENUM_STAGE_GET_FULL_DEV_DESC ||
        next_stage == ENUM_STAGE_GET_CONFIG_DESC ||
        next_stage == ENUM_STAGE_SET_CONFIG ||
        next_stage == ENUM_STAGE_CLEANUP ||
        next_stage == ENUM_STAGE_CLEANUP_FAILED) {
        p_hub_driver_obj->dynamic.flags.actions |= HUB_DRIVER_FLAG_ACTION_ENUM_EVENT;
    }
    HUB_DRIVER_EXIT_CRITICAL();
}

// ---------------------------------------------- Hub Driver Functions -------------------------------------------------

esp_err_t hub_install(hub_config_t *hub_config)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj == NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_EXIT_CRITICAL();
    //Allocate Hub driver object
    hub_driver_t *hub_driver_obj = heap_caps_calloc(1, sizeof(hub_driver_t), MALLOC_CAP_DEFAULT);
    urb_t *enum_urb = urb_alloc(sizeof(usb_setup_packet_t) + ENUM_CTRL_TRANSFER_MAX_LEN, 0, 0);
    if (hub_driver_obj == NULL || enum_urb == NULL) {
        return ESP_ERR_NO_MEM;
    }
    esp_err_t ret;
    //Install HCD port
    hcd_port_config_t port_config = {
        .fifo_bias = HUB_ROOT_HCD_PORT_FIFO_BIAS,
        .callback = root_port_callback,
        .callback_arg = NULL,
        .context = NULL,
    };
    hcd_port_handle_t port_hdl;
    ret = hcd_port_init(HUB_ROOT_PORT_NUM, &port_config, &port_hdl);
    if (ret != ESP_OK) {
        goto err;
    }
    //Initialize hub driver object
    hub_driver_obj->dynamic.driver_state = HUB_DRIVER_STATE_INSTALLED;
    hub_driver_obj->single_thread.enum_stage = ENUM_STAGE_NONE;
    hub_driver_obj->single_thread.enum_urb = enum_urb;
    hub_driver_obj->constant.root_port_hdl = port_hdl;
    hub_driver_obj->constant.notif_cb = hub_config->notif_cb;
    hub_driver_obj->constant.notif_cb_arg = hub_config->notif_cb_arg;
    HUB_DRIVER_ENTER_CRITICAL();
    if (p_hub_driver_obj != NULL) {
        HUB_DRIVER_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto assign_err;
    }
    p_hub_driver_obj = hub_driver_obj;
    HUB_DRIVER_EXIT_CRITICAL();
    //Indicate to USBH that the hub is installed
    ESP_ERROR_CHECK(usbh_hub_is_installed(usbh_hub_callback, NULL));
    ret = ESP_OK;
    return ret;

assign_err:
    ESP_ERROR_CHECK(hcd_port_deinit(port_hdl));
err:
    urb_free(enum_urb);
    heap_caps_free(hub_driver_obj);
    return ret;
}

esp_err_t hub_uninstall(void)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj != NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj->dynamic.driver_state == HUB_DRIVER_STATE_INSTALLED, ESP_ERR_INVALID_STATE);
    hub_driver_t *hub_driver_obj = p_hub_driver_obj;
    p_hub_driver_obj = NULL;
    HUB_DRIVER_EXIT_CRITICAL();

    ESP_ERROR_CHECK(hcd_port_deinit(hub_driver_obj->constant.root_port_hdl));
    //Free Hub driver resources
    urb_free(hub_driver_obj->single_thread.enum_urb);
    heap_caps_free(hub_driver_obj);
    return ESP_OK;
}

esp_err_t hub_root_start(void)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj != NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj->dynamic.driver_state == HUB_DRIVER_STATE_INSTALLED, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_EXIT_CRITICAL();
    //Power ON the root port
    esp_err_t ret;
    ret = hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_POWER_ON);
    if (ret == ESP_OK) {
        HUB_DRIVER_ENTER_CRITICAL();
        p_hub_driver_obj->dynamic.driver_state = HUB_DRIVER_STATE_ROOT_POWERD;
        HUB_DRIVER_EXIT_CRITICAL();
    }
    return ret;
}

esp_err_t hub_root_stop(void)
{
    HUB_DRIVER_ENTER_CRITICAL();
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj != NULL, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_CHECK_FROM_CRIT(p_hub_driver_obj->dynamic.driver_state != HUB_DRIVER_STATE_INSTALLED, ESP_ERR_INVALID_STATE);
    HUB_DRIVER_EXIT_CRITICAL();
    esp_err_t ret;
    ret = hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_POWER_OFF);
    if (ret == ESP_OK) {
        HUB_DRIVER_ENTER_CRITICAL();
        p_hub_driver_obj->dynamic.driver_state = HUB_DRIVER_STATE_INSTALLED;
        HUB_DRIVER_EXIT_CRITICAL();
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
        if (action_flags & HUB_DRIVER_FLAG_ACTION_ENUM_EVENT) {
            enum_handle_events();
        }
        if (action_flags & HUB_DRIVER_FLAG_ACTION_PORT_RECOVER) {
            ESP_LOGD(HUB_DRIVER_TAG, "Recovering root port");
            ESP_ERROR_CHECK(hcd_port_recover(p_hub_driver_obj->constant.root_port_hdl));
            ESP_ERROR_CHECK(hcd_port_command(p_hub_driver_obj->constant.root_port_hdl, HCD_PORT_CMD_POWER_ON));
            HUB_DRIVER_ENTER_CRITICAL();
            p_hub_driver_obj->dynamic.driver_state = HUB_DRIVER_STATE_ROOT_POWERD;
            HUB_DRIVER_EXIT_CRITICAL();
        }
        HUB_DRIVER_ENTER_CRITICAL();
        action_flags = p_hub_driver_obj->dynamic.flags.actions;
        p_hub_driver_obj->dynamic.flags.actions = 0;
        HUB_DRIVER_EXIT_CRITICAL();
    }
    return ESP_OK;
}
