/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
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

//Device action flags. LISTED IN THE ORDER THEY SHOULD BE HANDLED IN within usbh_process(). Some actions are mutually exclusive
#define DEV_FLAG_ACTION_PIPE_HALT_AND_FLUSH         0x0001  //Halt all non-default pipes then flush them (called after a device gone is gone)
#define DEV_FLAG_ACTION_DEFAULT_PIPE_FLUSH          0x0002  //Retire all URBS in the default pipe
#define DEV_FLAG_ACTION_DEFAULT_PIPE_DEQUEUE        0x0004  //Dequeue all URBs from default pipe
#define DEV_FLAG_ACTION_DEFAULT_PIPE_CLEAR          0x0008  //Move the default pipe to the active state
#define DEV_FLAG_ACTION_SEND_GONE_EVENT             0x0010  //Send a USB_HOST_CLIENT_EVENT_DEV_GONE event
#define DEV_FLAG_ACTION_FREE                        0x0020  //Free the device object
#define DEV_FLAG_ACTION_FREE_AND_RECOVER            0x0040  //Free the device object, but send a USBH_HUB_REQ_PORT_RECOVER request afterwards.
#define DEV_FLAG_ACTION_PORT_DISABLE                0x0080  //Request the hub driver to disable the port of the device
#define DEV_FLAG_ACTION_SEND_NEW                    0x0100  //Send a new device event

#define EP_NUM_MIN                                  1
#define EP_NUM_MAX                                  16

typedef struct device_s device_t;
struct device_s {
    //Dynamic members require a critical section
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
    //Mux protected members must be protected by the USBH mux_lock when accessed
    struct {
        hcd_pipe_handle_t ep_in[EP_NUM_MAX - 1];    //IN EP owner contexts. -1 to exclude the default endpoint
        hcd_pipe_handle_t ep_out[EP_NUM_MAX - 1];   //OUT EP owner contexts. -1 to exclude the default endpoint
    } mux_protected;
    //Constant members do no change after device allocation and enumeration thus do not require a critical section
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
    //Dynamic members require a critical section
    struct {
        TAILQ_HEAD(tailhead_devs, device_s) devs_idle_tailq;        //Tailq of all enum and configured devices
        TAILQ_HEAD(tailhead_devs_cb, device_s) devs_pending_tailq;  //Tailq of devices that need to have their cb called
    } dynamic;
    //Mux protected members must be protected by the USBH mux_lock when accessed
    struct {
        uint8_t num_device;     //Number of enumerated devices
    } mux_protected;
    //Constant members do no change after installation thus do not require a critical section
    struct {
        usb_notif_cb_t notif_cb;
        void *notif_cb_arg;
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

// --------------------------------------------------- Allocation ------------------------------------------------------

static esp_err_t device_alloc(hcd_port_handle_t port_hdl, usb_speed_t speed, device_t **dev_obj_ret)
{
    esp_err_t ret;
    device_t *dev_obj = heap_caps_calloc(1, sizeof(device_t), MALLOC_CAP_DEFAULT);
    usb_device_desc_t *dev_desc = heap_caps_calloc(1, sizeof(usb_device_desc_t), MALLOC_CAP_DEFAULT);
    if (dev_obj == NULL || dev_desc == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
    //Allocate default pipe. We set the pipe callback to NULL for now
    hcd_pipe_config_t pipe_config = {
        .callback = NULL,
        .callback_arg = NULL,
        .context = (void *)dev_obj,
        .ep_desc = NULL,    //No endpoint descriptor means we're allocating a default pipe
        .dev_speed = speed,
        .dev_addr = 0,
    };
    hcd_pipe_handle_t default_pipe_hdl;
    ret = hcd_pipe_alloc(port_hdl, &pipe_config, &default_pipe_hdl);
    if (ret != ESP_OK) {
        goto err;
    }
    //Initialize device object
    dev_obj->dynamic.state = USB_DEVICE_STATE_DEFAULT;
    dev_obj->constant.default_pipe = default_pipe_hdl;
    dev_obj->constant.port_hdl = port_hdl;
    //Note: dev_obj->constant.address is assigned later during enumeration
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
    //Configuration might not have been allocated (in case of early enumeration failure)
    if (dev_obj->constant.config_desc) {
        heap_caps_free((usb_config_desc_t *)dev_obj->constant.config_desc);
    }
    //String descriptors might not have been allocated (in case of early enumeration failure)
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

// -------------------------------------------------- Event Related ----------------------------------------------------

static bool _dev_set_actions(device_t *dev_obj, uint32_t action_flags)
{
    if (action_flags == 0) {
        return false;
    }
    bool call_notif_cb;
    //Check if device is already on the callback list
    if (!dev_obj->dynamic.flags.in_pending_list) {
        //Move device form idle device list to callback device list
        TAILQ_REMOVE(&p_usbh_obj->dynamic.devs_idle_tailq, dev_obj, dynamic.tailq_entry);
        TAILQ_INSERT_TAIL(&p_usbh_obj->dynamic.devs_pending_tailq, dev_obj, dynamic.tailq_entry);
        dev_obj->dynamic.action_flags |= action_flags;
        dev_obj->dynamic.flags.in_pending_list = 1;
        call_notif_cb = true;
    } else {
        call_notif_cb = false;
    }
    return call_notif_cb;
}

static bool default_pipe_callback(hcd_pipe_handle_t pipe_hdl, hcd_pipe_event_t pipe_event, void *user_arg, bool in_isr)
{
    uint32_t action_flags;
    device_t *dev_obj = (device_t *)user_arg;
    switch (pipe_event) {
        case HCD_PIPE_EVENT_URB_DONE:
            //A control transfer completed on the default pipe. We need to dequeue it
            action_flags = DEV_FLAG_ACTION_DEFAULT_PIPE_DEQUEUE;
            break;
        case HCD_PIPE_EVENT_ERROR_XFER:
        case HCD_PIPE_EVENT_ERROR_URB_NOT_AVAIL:
        case HCD_PIPE_EVENT_ERROR_OVERFLOW:
            //The default pipe has encountered an error. We need to retire all URBs, dequeue them, then make the pipe active again
            action_flags = DEV_FLAG_ACTION_DEFAULT_PIPE_FLUSH |
                           DEV_FLAG_ACTION_DEFAULT_PIPE_DEQUEUE |
                           DEV_FLAG_ACTION_DEFAULT_PIPE_CLEAR;
            if (in_isr) {
                ESP_EARLY_LOGE(USBH_TAG, "Dev %d EP 0 Error", dev_obj->constant.address);
            } else {
                ESP_LOGE(USBH_TAG, "Dev %d EP 0 Error", dev_obj->constant.address);
            }
            break;
        case HCD_PIPE_EVENT_ERROR_STALL:
            //The default pipe encountered a "protocol stall". We just need to dequeue URBs then make the pipe active again
            action_flags = DEV_FLAG_ACTION_DEFAULT_PIPE_DEQUEUE | DEV_FLAG_ACTION_DEFAULT_PIPE_CLEAR;
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
    bool call_notif_cb = _dev_set_actions(dev_obj, action_flags);
    USBH_EXIT_CRITICAL_SAFE();

    bool yield = false;
    if (call_notif_cb) {
        yield = p_usbh_obj->constant.notif_cb(USB_NOTIF_SOURCE_USBH, in_isr, p_usbh_obj->constant.notif_cb_arg);
    }
    return yield;
}

static void handle_pipe_halt_and_flush(device_t *dev_obj)
{
    //We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    //Halt then flush all non-default IN pipes
    for (int i = 0; i < (EP_NUM_MAX - 1); i++) {
        if (dev_obj->mux_protected.ep_in[i] != NULL) {
            ESP_ERROR_CHECK(hcd_pipe_command(dev_obj->mux_protected.ep_in[i], HCD_PIPE_CMD_HALT));
            ESP_ERROR_CHECK(hcd_pipe_command(dev_obj->mux_protected.ep_in[i], HCD_PIPE_CMD_FLUSH));
        }
        if (dev_obj->mux_protected.ep_out[i] != NULL) {
            ESP_ERROR_CHECK(hcd_pipe_command(dev_obj->mux_protected.ep_out[i], HCD_PIPE_CMD_HALT));
            ESP_ERROR_CHECK(hcd_pipe_command(dev_obj->mux_protected.ep_out[i], HCD_PIPE_CMD_FLUSH));
        }
    }
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);
}

static bool handle_dev_free(device_t *dev_obj)
{
    //We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    USBH_ENTER_CRITICAL();
    //Remove the device object for it's containing list
    if (dev_obj->dynamic.flags.in_pending_list) {
        dev_obj->dynamic.flags.in_pending_list = 0;
        TAILQ_REMOVE(&p_usbh_obj->dynamic.devs_pending_tailq, dev_obj, dynamic.tailq_entry);
    } else {
        TAILQ_REMOVE(&p_usbh_obj->dynamic.devs_idle_tailq, dev_obj, dynamic.tailq_entry);
    }
    USBH_EXIT_CRITICAL();
    p_usbh_obj->mux_protected.num_device--;
    bool all_free = (p_usbh_obj->mux_protected.num_device == 0);
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);
    device_free(dev_obj);
    return all_free;
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
        goto alloc_err;
    }
    //Install HCD
    ret = hcd_install(&usbh_config->hcd_config);
    if (ret != ESP_OK) {
        goto hcd_install_err;
    }
    //Initialize usbh object
    TAILQ_INIT(&usbh_obj->dynamic.devs_idle_tailq);
    TAILQ_INIT(&usbh_obj->dynamic.devs_pending_tailq);
    usbh_obj->constant.notif_cb = usbh_config->notif_cb;
    usbh_obj->constant.notif_cb_arg = usbh_config->notif_cb_arg;
    usbh_obj->constant.event_cb = usbh_config->event_cb;
    usbh_obj->constant.event_cb_arg = usbh_config->event_cb_arg;
    usbh_obj->constant.ctrl_xfer_cb = usbh_config->ctrl_xfer_cb;
    usbh_obj->constant.ctrl_xfer_cb_arg = usbh_config->ctrl_xfer_cb_arg;
    usbh_obj->constant.mux_lock = mux_lock;

    //Assign usbh object pointer
    USBH_ENTER_CRITICAL();
    if (p_usbh_obj != NULL) {
        USBH_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto assign_err;
    }
    p_usbh_obj = usbh_obj;
    USBH_EXIT_CRITICAL();

    ret = ESP_OK;
    return ret;

assign_err:
    ESP_ERROR_CHECK(hcd_uninstall());
hcd_install_err:
alloc_err:
    if (mux_lock != NULL) {
        vSemaphoreDelete(mux_lock);
    }
    heap_caps_free(usbh_obj);
    return ret;
}

esp_err_t usbh_uninstall(void)
{
    //Check that USBH is in a state to be uninstalled
    USBH_ENTER_CRITICAL();
    USBH_CHECK_FROM_CRIT(p_usbh_obj != NULL, ESP_ERR_INVALID_STATE);
    usbh_t *usbh_obj = p_usbh_obj;
    USBH_EXIT_CRITICAL();

    esp_err_t ret;
    //We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(usbh_obj->constant.mux_lock, portMAX_DELAY);
    if (p_usbh_obj->mux_protected.num_device > 0) {
        //There are still devices allocated. Can't uninstall right now.
        ret = ESP_ERR_INVALID_STATE;
        goto exit;
    }
    //Check again if we can uninstall
    USBH_ENTER_CRITICAL();
    assert(p_usbh_obj == usbh_obj);
    p_usbh_obj = NULL;
    USBH_EXIT_CRITICAL();
    xSemaphoreGive(usbh_obj->constant.mux_lock);

    //Uninstall HCD, free resources
    ESP_ERROR_CHECK(hcd_uninstall());
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
    //Keep clearing devices with events
    while (!TAILQ_EMPTY(&p_usbh_obj->dynamic.devs_pending_tailq)){
        //Move the device back into the idle device list,
        device_t *dev_obj = TAILQ_FIRST(&p_usbh_obj->dynamic.devs_pending_tailq);
        TAILQ_REMOVE(&p_usbh_obj->dynamic.devs_pending_tailq, dev_obj, dynamic.tailq_entry);
        TAILQ_INSERT_TAIL(&p_usbh_obj->dynamic.devs_idle_tailq, dev_obj, dynamic.tailq_entry);
        //Clear the device's flags
        uint32_t action_flags = dev_obj->dynamic.action_flags;
        dev_obj->dynamic.action_flags = 0;
        dev_obj->dynamic.flags.in_pending_list = 0;

        /* ---------------------------------------------------------------------
        Exit critical section to handle device action flags in their listed order
        --------------------------------------------------------------------- */
        USBH_EXIT_CRITICAL();
        ESP_LOGD(USBH_TAG, "Processing actions 0x%"PRIx32"", action_flags);
        //Sanity check. If the device is being freed, there must not be any other action flags set
        assert(!(action_flags & DEV_FLAG_ACTION_FREE) || action_flags == DEV_FLAG_ACTION_FREE);

        if (action_flags & DEV_FLAG_ACTION_PIPE_HALT_AND_FLUSH) {
            handle_pipe_halt_and_flush(dev_obj);
        }
        if (action_flags & DEV_FLAG_ACTION_DEFAULT_PIPE_FLUSH) {
            ESP_ERROR_CHECK(hcd_pipe_command(dev_obj->constant.default_pipe, HCD_PIPE_CMD_HALT));
            ESP_ERROR_CHECK(hcd_pipe_command(dev_obj->constant.default_pipe, HCD_PIPE_CMD_FLUSH));
        }
        if (action_flags & DEV_FLAG_ACTION_DEFAULT_PIPE_DEQUEUE) {
            //Empty URBs from default pipe and trigger a control transfer callback
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
        if (action_flags & DEV_FLAG_ACTION_DEFAULT_PIPE_CLEAR) {
            //We allow the pipe command to fail just in case the pipe becomes invalid mid command
            hcd_pipe_command(dev_obj->constant.default_pipe, HCD_PIPE_CMD_CLEAR);
        }
        if (action_flags & DEV_FLAG_ACTION_SEND_GONE_EVENT) {
            //Flush the default pipe. Then do an event gone
            ESP_LOGE(USBH_TAG, "Device %d gone", dev_obj->constant.address);
            p_usbh_obj->constant.event_cb((usb_device_handle_t)dev_obj, USBH_EVENT_DEV_GONE, p_usbh_obj->constant.event_cb_arg);
        }
        /*
        Note: We make these action flags mutually exclusive in case they happen in rapid succession. They are handled
        in the order of precedence
        For example
        - New device event is requested followed immediately by a disconnection
        - Port disable requested followed immediately by a disconnection
        */
        if (action_flags & (DEV_FLAG_ACTION_FREE | DEV_FLAG_ACTION_FREE_AND_RECOVER)) {
            //Cache a copy of the port handle as we are about to free the device object
            hcd_port_handle_t port_hdl = dev_obj->constant.port_hdl;
            ESP_LOGD(USBH_TAG, "Freeing device %d", dev_obj->constant.address);
            if (handle_dev_free(dev_obj)) {
                ESP_LOGD(USBH_TAG, "Device all free");
                p_usbh_obj->constant.event_cb((usb_device_handle_t)NULL, USBH_EVENT_DEV_ALL_FREE, p_usbh_obj->constant.event_cb_arg);
            }
            //Check if we need to recover the device's port
            if (action_flags & DEV_FLAG_ACTION_FREE_AND_RECOVER) {
                p_usbh_obj->constant.hub_req_cb(port_hdl, USBH_HUB_REQ_PORT_RECOVER, p_usbh_obj->constant.hub_req_cb_arg);
            }
        } else if (action_flags & DEV_FLAG_ACTION_PORT_DISABLE) {
            //Request that the HUB disables this device's port
            ESP_LOGD(USBH_TAG, "Disable device port %d", dev_obj->constant.address);
            p_usbh_obj->constant.hub_req_cb(dev_obj->constant.port_hdl, USBH_HUB_REQ_PORT_DISABLE, p_usbh_obj->constant.hub_req_cb_arg);
        } else if (action_flags & DEV_FLAG_ACTION_SEND_NEW) {
            ESP_LOGD(USBH_TAG, "New device %d", dev_obj->constant.address);
            p_usbh_obj->constant.event_cb((usb_device_handle_t)dev_obj, USBH_EVENT_DEV_NEW, p_usbh_obj->constant.event_cb_arg);
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
    //Fill list with devices from idle tailq
    TAILQ_FOREACH(dev_obj, &p_usbh_obj->dynamic.devs_idle_tailq, dynamic.tailq_entry) {
        if (num_filled < list_len) {
            dev_addr_list[num_filled] = dev_obj->constant.address;
            num_filled++;
        } else {
            break;
        }
    }
    //Fill list with devices from pending tailq
    TAILQ_FOREACH(dev_obj, &p_usbh_obj->dynamic.devs_pending_tailq, dynamic.tailq_entry) {
        if (num_filled < list_len) {
            dev_addr_list[num_filled] = dev_obj->constant.address;
            num_filled++;
        } else {
            break;
        }
    }
    USBH_EXIT_CRITICAL();
    //Write back number of devices filled
    *num_dev_ret = num_filled;
    return ESP_OK;
}

esp_err_t usbh_dev_open(uint8_t dev_addr, usb_device_handle_t *dev_hdl)
{
    USBH_CHECK(dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    esp_err_t ret;

    USBH_ENTER_CRITICAL();
    //Go through the device lists to find the device with the specified address
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
        //The device is not in a state to be referenced
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
    bool call_notif_cb = false;
    if (dev_obj->dynamic.ref_count == 0) {
        //Sanity check.
        assert(dev_obj->dynamic.num_ctrl_xfers_inflight == 0);  //There cannot be any control transfer inflight
        assert(!dev_obj->dynamic.flags.waiting_free);   //This can only be set when ref count reaches 0
        if (dev_obj->dynamic.flags.is_gone) {
            //Device is already gone so it's port is already disabled. Trigger the USBH process to free the device
            dev_obj->dynamic.flags.waiting_free = 1;
            call_notif_cb = _dev_set_actions(dev_obj, DEV_FLAG_ACTION_FREE_AND_RECOVER); //Port error occurred so we need to recover it
        } else if (dev_obj->dynamic.flags.waiting_close) {
            //Device is still connected but is no longer needed. Trigger the USBH process to request device's port be disabled
            dev_obj->dynamic.flags.waiting_port_disable = 1;
            call_notif_cb = _dev_set_actions(dev_obj, DEV_FLAG_ACTION_PORT_DISABLE);
        }
        //Else, there's nothing to do. Leave the device allocated
    }
    USBH_EXIT_CRITICAL();

    if (call_notif_cb) {
        p_usbh_obj->constant.notif_cb(USB_NOTIF_SOURCE_USBH, false, p_usbh_obj->constant.notif_cb_arg);
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
    bool call_notif_cb = false;
    bool wait_for_free = false;
    for (int i = 0; i < 2; i++) {
        device_t *dev_obj_cur;
        device_t *dev_obj_next;
        //Go through pending list first as it's more efficient
        if (i == 0) {
            dev_obj_cur = TAILQ_FIRST(&p_usbh_obj->dynamic.devs_pending_tailq);
        } else {
            dev_obj_cur = TAILQ_FIRST(&p_usbh_obj->dynamic.devs_idle_tailq);
        }
        while (dev_obj_cur != NULL) {
            assert(!dev_obj_cur->dynamic.flags.waiting_close);  //Sanity check
            //Keep a copy of the next item first in case we remove the current item
            dev_obj_next = TAILQ_NEXT(dev_obj_cur, dynamic.tailq_entry);
            if (dev_obj_cur->dynamic.ref_count == 0 && !dev_obj_cur->dynamic.flags.is_gone) {
                //Device is not opened as is not gone, so we can disable it now
                dev_obj_cur->dynamic.flags.waiting_port_disable = 1;
                call_notif_cb |= _dev_set_actions(dev_obj_cur, DEV_FLAG_ACTION_PORT_DISABLE);
            } else {
                //Device is still opened. Just mark it as waiting to be closed
                dev_obj_cur->dynamic.flags.waiting_close = 1;
            }
            wait_for_free = true;   //As long as there is still a device, we need to wait for an event indicating it is freed
            dev_obj_cur = dev_obj_next;
        }
    }
    USBH_EXIT_CRITICAL();

    if (call_notif_cb) {
        p_usbh_obj->constant.notif_cb(USB_NOTIF_SOURCE_USBH, false, p_usbh_obj->constant.notif_cb_arg);
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
    //Device must be configured, or not attached (if it suddenly disconnected)
    USBH_ENTER_CRITICAL();
    if (!(dev_obj->dynamic.state == USB_DEVICE_STATE_CONFIGURED || dev_obj->dynamic.state == USB_DEVICE_STATE_NOT_ATTACHED)) {
        USBH_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto exit;
    }
    //Critical section for the dynamic members
    dev_info->speed = dev_obj->constant.speed;
    dev_info->dev_addr = dev_obj->constant.address;
    dev_info->bMaxPacketSize0 = dev_obj->constant.desc->bMaxPacketSize0;
    USBH_EXIT_CRITICAL();
    assert(dev_obj->constant.config_desc);
    dev_info->bConfigurationValue = dev_obj->constant.config_desc->bConfigurationValue;
    //String descriptors are allowed to be NULL as not all devices support them
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
    //Device must be in the configured state
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

    USBH_ENTER_CRITICAL();
    USBH_CHECK_FROM_CRIT(dev_obj->dynamic.state == USB_DEVICE_STATE_CONFIGURED, ESP_ERR_INVALID_STATE);
    //Increment the control transfer count first
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

esp_err_t usbh_ep_alloc(usb_device_handle_t dev_hdl, usbh_ep_config_t *ep_config, hcd_pipe_handle_t *pipe_hdl_ret)
{
    USBH_CHECK(dev_hdl != NULL && ep_config != NULL && pipe_hdl_ret != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;
    esp_err_t ret;

    //Allocate HCD pipe
    hcd_pipe_config_t pipe_config = {
        .callback = ep_config->pipe_cb,
        .callback_arg = ep_config->pipe_cb_arg,
        .context = ep_config->context,
        .ep_desc = ep_config->ep_desc,
        .dev_speed = dev_obj->constant.speed,
        .dev_addr = dev_obj->constant.address,
    };
    hcd_pipe_handle_t pipe_hdl;
    ret = hcd_pipe_alloc(dev_obj->constant.port_hdl, &pipe_config, &pipe_hdl);
    if (ret != ESP_OK) {
        goto pipe_alloc_err;
    }

    bool is_in = ep_config->ep_desc->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK;
    uint8_t addr = ep_config->ep_desc->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK;
    bool assigned = false;

    //We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    USBH_ENTER_CRITICAL();
    //Check the device's state before we assign the pipes to the endpoint
    if (dev_obj->dynamic.state != USB_DEVICE_STATE_CONFIGURED) {
        USBH_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto assign_err;
    }
    USBH_EXIT_CRITICAL();
    //Assign the allocated pipe to the correct endpoint
    if (is_in && dev_obj->mux_protected.ep_in[addr - 1] == NULL) {  //Is an IN EP
        dev_obj->mux_protected.ep_in[addr - 1] = pipe_hdl;
        assigned = true;
    } else if (!is_in && dev_obj->mux_protected.ep_out[addr - 1] == NULL) {   //Is an OUT EP
        dev_obj->mux_protected.ep_out[addr - 1] = pipe_hdl;
        assigned = true;
    }
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);

    if (!assigned) {
        ret = ESP_ERR_INVALID_STATE;
        goto assign_err;
    }
    //Write back output
    *pipe_hdl_ret = pipe_hdl;
    ret = ESP_OK;
    return ret;

assign_err:
    ESP_ERROR_CHECK(hcd_pipe_free(pipe_hdl));
pipe_alloc_err:
    return ret;
}

esp_err_t usbh_ep_free(usb_device_handle_t dev_hdl, uint8_t bEndpointAddress)
{
    USBH_CHECK(dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;

    esp_err_t ret;
    bool is_in = bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK;
    uint8_t addr = bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK;
    hcd_pipe_handle_t pipe_hdl;

    //We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    //Check if the EP was previously allocated. If so, set it to NULL
    if (is_in) {
        if (dev_obj->mux_protected.ep_in[addr - 1] != NULL) {
            pipe_hdl = dev_obj->mux_protected.ep_in[addr - 1];
            dev_obj->mux_protected.ep_in[addr - 1] = NULL;
            ret = ESP_OK;
        } else {
            ret = ESP_ERR_INVALID_STATE;
        }
    } else {
        //EP must have been previously allocated
        if (dev_obj->mux_protected.ep_out[addr - 1] != NULL) {
            pipe_hdl = dev_obj->mux_protected.ep_out[addr - 1];
            dev_obj->mux_protected.ep_out[addr - 1] = NULL;
            ret = ESP_OK;
        } else {
            ret = ESP_ERR_INVALID_STATE;
        }
    }
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);

    if (ret == ESP_OK) {
        ESP_ERROR_CHECK(hcd_pipe_free(pipe_hdl));
    }
    return ret;
}

esp_err_t usbh_ep_get_context(usb_device_handle_t dev_hdl, uint8_t bEndpointAddress, void **context_ret)
{
    bool is_in = bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK;
    uint8_t addr = bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK;
    USBH_CHECK(dev_hdl != NULL &&
               addr >= EP_NUM_MIN &&    //Control endpoints are owned by the USBH
               addr <= EP_NUM_MAX &&
               context_ret != NULL,
               ESP_ERR_INVALID_ARG);

    esp_err_t ret;
    device_t *dev_obj = (device_t *)dev_hdl;
    hcd_pipe_handle_t pipe_hdl;

    //We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    //Get the endpoint's corresponding pipe
    if (is_in) {
        pipe_hdl = dev_obj->mux_protected.ep_in[addr - 1];
    } else {
        pipe_hdl = dev_obj->mux_protected.ep_out[addr - 1];
    }
    //Check if the EP was allocated to begin with
    if (pipe_hdl == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto exit;
    }
    //Return the context of the pipe
    void *context = hcd_pipe_get_context(pipe_hdl);
    *context_ret = context;
    ret = ESP_OK;
exit:
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);
    return ret;
}

// -------------------------------------------------- Hub Functions ----------------------------------------------------

// ------------------- Device Related ----------------------

esp_err_t usbh_hub_is_installed(usbh_hub_req_cb_t hub_req_callback, void *callback_arg)
{
    USBH_CHECK(hub_req_callback != NULL, ESP_ERR_INVALID_ARG);

    USBH_ENTER_CRITICAL();
    //Check that USBH is already installed
    USBH_CHECK_FROM_CRIT(p_usbh_obj != NULL, ESP_ERR_INVALID_STATE);
    //Check that Hub has not be installed yet
    USBH_CHECK_FROM_CRIT(p_usbh_obj->constant.hub_req_cb == NULL, ESP_ERR_INVALID_STATE);
    p_usbh_obj->constant.hub_req_cb = hub_req_callback;
    p_usbh_obj->constant.hub_req_cb_arg = callback_arg;
    USBH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t usbh_hub_add_dev(hcd_port_handle_t port_hdl, usb_speed_t dev_speed, usb_device_handle_t *new_dev_hdl, hcd_pipe_handle_t *default_pipe_hdl)
{
    //Note: Parent device handle can be NULL if it's connected to the root hub
    USBH_CHECK(new_dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    esp_err_t ret;
    device_t *dev_obj;
    ret = device_alloc(port_hdl, dev_speed, &dev_obj);
    if (ret != ESP_OK) {
        return ret;
    }
    //Write-back device handle
    *new_dev_hdl = (usb_device_handle_t)dev_obj;
    *default_pipe_hdl = dev_obj->constant.default_pipe;
    ret = ESP_OK;
    return ret;
}

esp_err_t usbh_hub_pass_event(usb_device_handle_t dev_hdl, usbh_hub_event_t hub_event)
{
    USBH_CHECK(dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;

    bool call_notif_cb;
    switch (hub_event) {
        case USBH_HUB_EVENT_PORT_ERROR: {
            USBH_ENTER_CRITICAL();
            dev_obj->dynamic.flags.is_gone = 1;
            //Check if the device can be freed now
            if (dev_obj->dynamic.ref_count == 0) {
                dev_obj->dynamic.flags.waiting_free = 1;
                //Device is already waiting free so none of it's pipes will be in use. Can free immediately.
                call_notif_cb = _dev_set_actions(dev_obj, DEV_FLAG_ACTION_FREE_AND_RECOVER); //Port error occurred so we need to recover it
            } else {
                call_notif_cb = _dev_set_actions(dev_obj, DEV_FLAG_ACTION_PIPE_HALT_AND_FLUSH |
                                                          DEV_FLAG_ACTION_DEFAULT_PIPE_FLUSH |
                                                          DEV_FLAG_ACTION_DEFAULT_PIPE_DEQUEUE |
                                                          DEV_FLAG_ACTION_SEND_GONE_EVENT);
            }
            USBH_EXIT_CRITICAL();
            break;
        }
        case USBH_HUB_EVENT_PORT_DISABLED: {
            USBH_ENTER_CRITICAL();
            assert(dev_obj->dynamic.ref_count == 0);    //At this stage, the device should have been closed by all users
            dev_obj->dynamic.flags.waiting_free = 1;
            call_notif_cb = _dev_set_actions(dev_obj, DEV_FLAG_ACTION_FREE);
            USBH_EXIT_CRITICAL();
            break;
        }
        default:
            return ESP_ERR_INVALID_ARG;
    }

    if (call_notif_cb) {
        p_usbh_obj->constant.notif_cb(USB_NOTIF_SOURCE_USBH, false, p_usbh_obj->constant.notif_cb_arg);
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

    //We can modify the info members outside the critical section
    dev_obj->constant.address = dev_addr;
    return ESP_OK;
}

esp_err_t usbh_hub_enum_fill_dev_desc(usb_device_handle_t dev_hdl, const usb_device_desc_t *device_desc)
{
    USBH_CHECK(dev_hdl != NULL && device_desc != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;
    //We can modify the info members outside the critical section
    memcpy((usb_device_desc_t *)dev_obj->constant.desc, device_desc, sizeof(usb_device_desc_t));
    return ESP_OK;
}

esp_err_t usbh_hub_enum_fill_config_desc(usb_device_handle_t dev_hdl, const usb_config_desc_t *config_desc_full)
{
    USBH_CHECK(dev_hdl != NULL && config_desc_full != NULL, ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;
    //Allocate memory to store the configuration descriptor
    usb_config_desc_t *config_desc = heap_caps_malloc(config_desc_full->wTotalLength, MALLOC_CAP_DEFAULT);  //Buffer to copy over full configuration descriptor (wTotalLength)
    if (config_desc == NULL) {
        return ESP_ERR_NO_MEM;
    }
    //Copy the configuration descriptor
    memcpy(config_desc, config_desc_full, config_desc_full->wTotalLength);
    //Assign the config desc to the device object
    assert(dev_obj->constant.config_desc == NULL);
    dev_obj->constant.config_desc = config_desc;
    return ESP_OK;
}

esp_err_t usbh_hub_enum_fill_str_desc(usb_device_handle_t dev_hdl, const usb_str_desc_t *str_desc, int select)
{
    USBH_CHECK(dev_hdl != NULL && str_desc != NULL && (select >= 0 && select < 3), ESP_ERR_INVALID_ARG);
    device_t *dev_obj = (device_t *)dev_hdl;
    //Allocate memory to store the manufacturer string descriptor
    usb_str_desc_t *str_desc_fill = heap_caps_malloc(str_desc->bLength, MALLOC_CAP_DEFAULT);
    if (str_desc_fill == NULL) {
        return ESP_ERR_NO_MEM;
    }
    //Copy the string descriptor
    memcpy(str_desc_fill, str_desc, str_desc->bLength);
    //Assign filled string descriptor to the device object
    switch (select) {
        case 0:
            assert(dev_obj->constant.str_desc_manu == NULL);
            dev_obj->constant.str_desc_manu = str_desc_fill;
            break;
        case 1:
            assert(dev_obj->constant.str_desc_product == NULL);
            dev_obj->constant.str_desc_product = str_desc_fill;
            break;
        default:    //2
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

    //We need to take the mux_lock to access mux_protected members
    xSemaphoreTake(p_usbh_obj->constant.mux_lock, portMAX_DELAY);
    USBH_ENTER_CRITICAL();
    dev_obj->dynamic.state = USB_DEVICE_STATE_CONFIGURED;
    //Add the device to list of devices, then trigger a device event
    TAILQ_INSERT_TAIL(&p_usbh_obj->dynamic.devs_idle_tailq, dev_obj, dynamic.tailq_entry);   //Add it to the idle device list first
    bool call_notif_cb = _dev_set_actions(dev_obj, DEV_FLAG_ACTION_SEND_NEW);
    USBH_EXIT_CRITICAL();
    p_usbh_obj->mux_protected.num_device++;
    xSemaphoreGive(p_usbh_obj->constant.mux_lock);

    //Update the default pipe callback
    ESP_ERROR_CHECK(hcd_pipe_update_callback(dev_obj->constant.default_pipe, default_pipe_callback, (void *)dev_obj));
    //Call the notification callback
    if (call_notif_cb) {
        p_usbh_obj->constant.notif_cb(USB_NOTIF_SOURCE_USBH, false, p_usbh_obj->constant.notif_cb_arg);
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
