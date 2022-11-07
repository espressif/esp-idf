/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
Warning: The USB Host Library API is still a beta version and may be subject to change
*/

#include <stdlib.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "hub.h"
#include "usbh.h"
#include "esp_private/usb_phy.h"
#include "usb/usb_host.h"

static portMUX_TYPE host_lock = portMUX_INITIALIZER_UNLOCKED;

#define HOST_ENTER_CRITICAL_ISR()       portENTER_CRITICAL_ISR(&host_lock)
#define HOST_EXIT_CRITICAL_ISR()        portEXIT_CRITICAL_ISR(&host_lock)
#define HOST_ENTER_CRITICAL()           portENTER_CRITICAL(&host_lock)
#define HOST_EXIT_CRITICAL()            portEXIT_CRITICAL(&host_lock)
#define HOST_ENTER_CRITICAL_SAFE()      portENTER_CRITICAL_SAFE(&host_lock)
#define HOST_EXIT_CRITICAL_SAFE()       portEXIT_CRITICAL_SAFE(&host_lock)

#define HOST_CHECK(cond, ret_val) ({                                        \
            if (!(cond)) {                                                  \
                return (ret_val);                                           \
            }                                                               \
})
#define HOST_CHECK_FROM_CRIT(cond, ret_val) ({                              \
            if (!(cond)) {                                                  \
                HOST_EXIT_CRITICAL();                                       \
                return ret_val;                                             \
            }                                                               \
})

#define PROCESS_PENDING_FLAG_USBH       0x01
#define PROCESS_PENDING_FLAG_HUB        0x02
#define PROCESS_PENDING_FLAG_EVENT      0x04

typedef struct endpoint_s endpoint_t;
typedef struct interface_s interface_t;
typedef struct client_s client_t;

struct endpoint_s {
    //Dynamic members require a critical section
    struct {
        TAILQ_ENTRY(endpoint_s) tailq_entry;
        union {
            struct {
                uint32_t pending: 1;
                uint32_t reserved31:31;
            };
        } flags;
        uint32_t num_urb_inflight;
        hcd_pipe_event_t last_event;
    } dynamic;
    //Constant members do no change after claiming the interface thus do not require a critical section
    struct {
        hcd_pipe_handle_t pipe_hdl;
        const usb_ep_desc_t *ep_desc;
        interface_t *intf_obj;
    } constant;
};

struct interface_s {
    //Dynamic members require a critical section
    struct {
        TAILQ_ENTRY(interface_s) tailq_entry;
    } mux_protected;
    //Constant members do no change after claiming the interface thus do not require a critical section
    struct {
        const usb_intf_desc_t *intf_desc;
        usb_device_handle_t dev_hdl;
        client_t *client_obj;
        endpoint_t *endpoints[0];
    } constant;
};

struct client_s {
    //Dynamic members require a critical section
    struct {
        TAILQ_ENTRY(client_s) tailq_entry;
        TAILQ_HEAD(tailhead_pending_ep, endpoint_s) pending_ep_tailq;
        TAILQ_HEAD(tailhead_idle_ep, endpoint_s) idle_ep_tailq;
        TAILQ_HEAD(tailhead_done_ctrl_xfers, urb_s) done_ctrl_xfer_tailq;
        union {
            struct {
                uint32_t events_pending: 1;
                uint32_t handling_events: 1;
                uint32_t blocked: 1;
                uint32_t taking_mux: 1;
                uint32_t reserved4: 4;
                uint32_t num_intf_claimed: 8;
                uint32_t reserved16: 16;
            };
            uint32_t val;
        } flags;
        uint32_t num_done_ctrl_xfer;
        uint32_t opened_dev_addr_map;
    } dynamic;
    //Mux protected members must be protected by host library the mux_lock when accessed
    struct {
        TAILQ_HEAD(tailhead_interfaces, interface_s) interface_tailq;
    } mux_protected;
    //Constant members do no change after registration thus do not require a critical section
    struct {
        SemaphoreHandle_t event_sem;
        usb_host_client_event_cb_t event_callback;
        void *callback_arg;
        QueueHandle_t event_msg_queue;
    } constant;
};

typedef struct {
    //Dynamic members require a critical section
    struct {
        //Access to these should be done in a critical section
        uint32_t process_pending_flags;
        uint32_t lib_event_flags;
        union {
            struct {
                uint32_t process_pending: 1;
                uint32_t handling_events: 1;
                uint32_t blocked: 1;
                uint32_t reserved5: 5;
                uint32_t num_clients: 8;
                uint32_t reserved16: 16;
            };
            uint32_t val;
        } flags;
    } dynamic;
    //Mux protected members must be protected by host library the mux_lock when accessed
    struct {
        TAILQ_HEAD(tailhead_clients, client_s) client_tailq;  //List of all clients registered
    } mux_protected;
    //Constant members do no change after installation thus do not require a critical section
    struct {
        SemaphoreHandle_t event_sem;
        SemaphoreHandle_t mux_lock;
        usb_phy_handle_t phy_handle;    //Will be NULL if host library is installed with skip_phy_setup
    } constant;
} host_lib_t;

static host_lib_t *p_host_lib_obj = NULL;

const char *USB_HOST_TAG = "USB HOST";

// ----------------------------------------------------- Helpers -------------------------------------------------------

static inline void _record_client_opened_device(client_t *client_obj, uint8_t dev_addr)
{
    assert(dev_addr != 0);
    client_obj->dynamic.opened_dev_addr_map |= (1 << (dev_addr - 1));
}

static inline void _clear_client_opened_device(client_t *client_obj, uint8_t dev_addr)
{
    assert(dev_addr != 0);
    client_obj->dynamic.opened_dev_addr_map &= ~(1 << (dev_addr - 1));
}

static inline bool _check_client_opened_device(client_t *client_obj, uint8_t dev_addr)
{
    assert(dev_addr != 0);
    return (client_obj->dynamic.opened_dev_addr_map & (1 << (dev_addr - 1)));
}

static bool _unblock_client(client_t *client_obj, bool in_isr)
{
    bool send_sem;
    if (!client_obj->dynamic.flags.events_pending && !client_obj->dynamic.flags.handling_events) {
        client_obj->dynamic.flags.events_pending = 1;
        send_sem = true;
    } else {
        send_sem = false;
    }

    HOST_EXIT_CRITICAL_SAFE();
    bool yield = false;
    if (send_sem) {
        if (in_isr) {
            BaseType_t xTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(client_obj->constant.event_sem, &xTaskWoken);
            yield = (xTaskWoken == pdTRUE);
        } else {
            xSemaphoreGive(client_obj->constant.event_sem);
        }
    }
    HOST_ENTER_CRITICAL_SAFE();

    return yield;
}

static bool _unblock_lib(bool in_isr)
{
    bool send_sem;
    if (!p_host_lib_obj->dynamic.flags.process_pending && !p_host_lib_obj->dynamic.flags.handling_events) {
        p_host_lib_obj->dynamic.flags.process_pending = 1;
        send_sem = true;
    } else {
        send_sem = false;
    }

    HOST_EXIT_CRITICAL_SAFE();
    bool yield = false;
    if (send_sem) {
        if (in_isr) {
            BaseType_t xTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(p_host_lib_obj->constant.event_sem, &xTaskWoken);
            yield = (xTaskWoken == pdTRUE);
        } else {
            xSemaphoreGive(p_host_lib_obj->constant.event_sem);
        }
    }
    HOST_ENTER_CRITICAL_SAFE();

    return yield;
}

static void send_event_msg_to_clients(const usb_host_client_event_msg_t *event_msg, bool send_to_all, uint8_t opened_dev_addr)
{
    //Lock client list
    xSemaphoreTake(p_host_lib_obj->constant.mux_lock, portMAX_DELAY);
    //Send event message to relevant or all clients
    client_t *client_obj;
    TAILQ_FOREACH(client_obj, &p_host_lib_obj->mux_protected.client_tailq, dynamic.tailq_entry) {
        if (!send_to_all) {
            //Check if client opened the device
            HOST_ENTER_CRITICAL();
            bool send = _check_client_opened_device(client_obj, opened_dev_addr);
            HOST_EXIT_CRITICAL();
            if (!send) {
                continue;
            }
        }
        //Send the event message
        if (xQueueSend(client_obj->constant.event_msg_queue, event_msg, 0) == pdTRUE) {
            HOST_ENTER_CRITICAL();
            _unblock_client(client_obj, false);
            HOST_EXIT_CRITICAL();
        } else {
            ESP_LOGE(USB_HOST_TAG, "Client event message queue full");
        }
    }
    //Unlock client list
    xSemaphoreGive(p_host_lib_obj->constant.mux_lock);
}

// ---------------------------------------------------- Callbacks ------------------------------------------------------

// ------------------- Library Related ---------------------

static bool notif_callback(usb_notif_source_t source, bool in_isr, void *arg)
{
    HOST_ENTER_CRITICAL_SAFE();
    //Store notification source
    switch (source) {
        case USB_NOTIF_SOURCE_USBH:
            p_host_lib_obj->dynamic.process_pending_flags |= PROCESS_PENDING_FLAG_USBH;
            break;
        case USB_NOTIF_SOURCE_HUB:
            p_host_lib_obj->dynamic.process_pending_flags |= PROCESS_PENDING_FLAG_HUB;
            break;
    }
    bool yield = _unblock_lib(in_isr);
    HOST_EXIT_CRITICAL_SAFE();

    return yield;
}

static void ctrl_xfer_callback(usb_device_handle_t dev_hdl, urb_t *urb, void *arg)
{
    assert(urb->usb_host_client != NULL);
    //Redistribute done control transfer to the clients that submitted them
    client_t *client_obj = (client_t *)urb->usb_host_client;

    HOST_ENTER_CRITICAL();
    TAILQ_INSERT_TAIL(&client_obj->dynamic.done_ctrl_xfer_tailq, urb, tailq_entry);
    client_obj->dynamic.num_done_ctrl_xfer++;
    _unblock_client(client_obj, false);
    HOST_EXIT_CRITICAL();
}

static void dev_event_callback(usb_device_handle_t dev_hdl, usbh_event_t usbh_event, void *arg)
{
    //Check usbh_event. The data type of event_arg depends on the type of event
    switch (usbh_event) {
        case USBH_EVENT_DEV_NEW: {
            //Prepare a NEW_DEV client event message, the send it to all clients
            uint8_t dev_addr;
            ESP_ERROR_CHECK(usbh_dev_get_addr(dev_hdl, &dev_addr));
            usb_host_client_event_msg_t event_msg = {
                .event = USB_HOST_CLIENT_EVENT_NEW_DEV,
                .new_dev.address = dev_addr,
            };
            send_event_msg_to_clients(&event_msg, true, 0);
            break;
        }
        case USBH_EVENT_DEV_GONE: {
            //Prepare event msg, send only to clients that have opened the device
            uint8_t dev_addr;
            ESP_ERROR_CHECK(usbh_dev_get_addr(dev_hdl, &dev_addr));
            usb_host_client_event_msg_t event_msg = {
                .event = USB_HOST_CLIENT_EVENT_DEV_GONE,
                .dev_gone.dev_hdl = dev_hdl,
            };
            send_event_msg_to_clients(&event_msg, false, dev_addr);
            break;
        }
        case USBH_EVENT_DEV_ALL_FREE: {
            //Notify the lib handler that all devices are free
            HOST_ENTER_CRITICAL();
            p_host_lib_obj->dynamic.lib_event_flags |= USB_HOST_LIB_EVENT_FLAGS_ALL_FREE;
            _unblock_lib(false);
            HOST_EXIT_CRITICAL();
            break;
        }
        default:
            abort();    //Should never occur
            break;
    }
}

// ------------------- Client Related ----------------------

static bool pipe_callback(hcd_pipe_handle_t pipe_hdl, hcd_pipe_event_t pipe_event, void *user_arg, bool in_isr)
{
    endpoint_t *ep_obj = (endpoint_t *)user_arg;
    client_t *client_obj = (client_t *)ep_obj->constant.intf_obj->constant.client_obj;

    HOST_ENTER_CRITICAL_SAFE();
    //Store the event to be handled later. Note that we allow overwriting of events because more severe will halt the pipe prevent any further events.
    ep_obj->dynamic.last_event = pipe_event;
    //Add the EP to the client's pending list if it's not in the list already
    if (!ep_obj->dynamic.flags.pending) {
        ep_obj->dynamic.flags.pending = 1;
        TAILQ_REMOVE(&client_obj->dynamic.idle_ep_tailq, ep_obj, dynamic.tailq_entry);
        TAILQ_INSERT_TAIL(&client_obj->dynamic.pending_ep_tailq, ep_obj, dynamic.tailq_entry);
    }
    bool yield = _unblock_client(client_obj, in_isr);
    HOST_EXIT_CRITICAL_SAFE();

    return yield;
}

// ------------------------------------------------ Library Functions --------------------------------------------------

// ----------------------- Public --------------------------

esp_err_t usb_host_install(const usb_host_config_t *config)
{
    HOST_CHECK(config != NULL, ESP_ERR_INVALID_ARG);
    HOST_ENTER_CRITICAL();
    HOST_CHECK_FROM_CRIT(p_host_lib_obj == NULL, ESP_ERR_INVALID_STATE);
    HOST_EXIT_CRITICAL();

    esp_err_t ret;
    host_lib_t *host_lib_obj = heap_caps_calloc(1, sizeof(host_lib_t), MALLOC_CAP_DEFAULT);
    SemaphoreHandle_t event_sem = xSemaphoreCreateBinary();
    SemaphoreHandle_t mux_lock = xSemaphoreCreateMutex();
    if (host_lib_obj == NULL || event_sem == NULL || mux_lock == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto alloc_err;
    }
    //Initialize host library object
    TAILQ_INIT(&host_lib_obj->mux_protected.client_tailq);
    host_lib_obj->constant.event_sem = event_sem;
    host_lib_obj->constant.mux_lock = mux_lock;
    //Setup the USB PHY if necessary (USB PHY driver will also enable the underlying Host Controller)
    if (!config->skip_phy_setup) {
        //Host Library defaults to internal PHY
        usb_phy_config_t phy_config = {
            .controller = USB_PHY_CTRL_OTG,
            .target = USB_PHY_TARGET_INT,
            .otg_mode = USB_OTG_MODE_HOST,
            .otg_speed = USB_PHY_SPEED_UNDEFINED,   //In Host mode, the speed is determined by the connected device
            .ext_io_conf = NULL,
            .otg_io_conf = NULL,
        };
        ret = usb_new_phy(&phy_config, &host_lib_obj->constant.phy_handle);
         if (ret != ESP_OK) {
             goto phy_err;
         }
    }
    //Install USBH
    usbh_config_t usbh_config = {
        .notif_cb = notif_callback,
        .notif_cb_arg = NULL,
        .ctrl_xfer_cb = ctrl_xfer_callback,
        .ctrl_xfer_cb_arg = NULL,
        .event_cb = dev_event_callback,
        .event_cb_arg = NULL,
        .hcd_config = {
            .intr_flags = config->intr_flags,
        },
    };
    ret = usbh_install(&usbh_config);
    if (ret != ESP_OK) {
        goto usbh_err;
    }
    //Install Hub
    hub_config_t hub_config = {
        .notif_cb = notif_callback,
        .notif_cb_arg = NULL,
    };
    ret = hub_install(&hub_config);
    if (ret != ESP_OK) {
        goto hub_err;
    }

    //Assign host library object
    HOST_ENTER_CRITICAL();
    if (p_host_lib_obj != NULL) {
        HOST_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto assign_err;
    }
    p_host_lib_obj = host_lib_obj;
    HOST_EXIT_CRITICAL();

    //Start the root hub
    ESP_ERROR_CHECK(hub_root_start());
    ret = ESP_OK;
    return ret;

assign_err:
    ESP_ERROR_CHECK(hub_uninstall());
hub_err:
    ESP_ERROR_CHECK(usbh_uninstall());
usbh_err:
    if (p_host_lib_obj->constant.phy_handle) {
        ESP_ERROR_CHECK(usb_del_phy(p_host_lib_obj->constant.phy_handle));
    }
phy_err:
alloc_err:
    if (mux_lock) {
        vSemaphoreDelete(mux_lock);
    }
    if (event_sem) {
        vSemaphoreDelete(event_sem);
    }
    heap_caps_free(host_lib_obj);
    return ret;
}

esp_err_t usb_host_uninstall(void)
{
    //All devices must have been freed at this point
    HOST_ENTER_CRITICAL();
    HOST_CHECK_FROM_CRIT(p_host_lib_obj != NULL, ESP_ERR_INVALID_STATE);
    HOST_CHECK_FROM_CRIT(p_host_lib_obj->dynamic.process_pending_flags == 0 &&
                         p_host_lib_obj->dynamic.lib_event_flags == 0 &&
                         p_host_lib_obj->dynamic.flags.val == 0,
                         ESP_ERR_INVALID_STATE);
    HOST_EXIT_CRITICAL();

    //Stop the root hub
    ESP_ERROR_CHECK(hub_root_stop());
    //Uninstall Hub and USBH
    ESP_ERROR_CHECK(hub_uninstall());
    ESP_ERROR_CHECK(usbh_uninstall());

    HOST_ENTER_CRITICAL();
    host_lib_t *host_lib_obj = p_host_lib_obj;
    p_host_lib_obj = NULL;
    HOST_EXIT_CRITICAL();

    //If the USB PHY was setup, then delete it
    if (host_lib_obj->constant.phy_handle) {
        ESP_ERROR_CHECK(usb_del_phy(host_lib_obj->constant.phy_handle));
    }
    //Free memory objects
    vSemaphoreDelete(host_lib_obj->constant.mux_lock);
    vSemaphoreDelete(host_lib_obj->constant.event_sem);
    heap_caps_free(host_lib_obj);
    return ESP_OK;
}

esp_err_t usb_host_lib_handle_events(TickType_t timeout_ticks, uint32_t *event_flags_ret)
{
    esp_err_t ret;
    uint32_t event_flags = 0;

    HOST_ENTER_CRITICAL();
    if (!p_host_lib_obj->dynamic.flags.process_pending) {
        //There is currently processing that needs to be done. Wait for some processing
        HOST_EXIT_CRITICAL();
        BaseType_t sem_ret = xSemaphoreTake(p_host_lib_obj->constant.event_sem, timeout_ticks);
        if (sem_ret == pdFALSE) {
            ret = ESP_ERR_TIMEOUT;
            goto exit;
        }
        HOST_ENTER_CRITICAL();
    }
    //Read and clear process pending flags
    uint32_t process_pending_flags = p_host_lib_obj->dynamic.process_pending_flags;
    p_host_lib_obj->dynamic.process_pending_flags = 0;
    p_host_lib_obj->dynamic.flags.handling_events = 1;
    while (process_pending_flags) {
        HOST_EXIT_CRITICAL();
        if (process_pending_flags & PROCESS_PENDING_FLAG_USBH) {
            ESP_ERROR_CHECK(usbh_process());
        }
        if (process_pending_flags & PROCESS_PENDING_FLAG_HUB) {
            ESP_ERROR_CHECK(hub_process());
        }
        HOST_ENTER_CRITICAL();
        //Read and clear process pending flags again, and loop back if there is more to process
        process_pending_flags = p_host_lib_obj->dynamic.process_pending_flags;
        p_host_lib_obj->dynamic.process_pending_flags = 0;
    }
    p_host_lib_obj->dynamic.flags.process_pending = 0;
    p_host_lib_obj->dynamic.flags.handling_events = 0;
    event_flags = p_host_lib_obj->dynamic.lib_event_flags;
    p_host_lib_obj->dynamic.lib_event_flags = 0;
    HOST_EXIT_CRITICAL();

    ret = ESP_OK;
exit:
    if (event_flags_ret != NULL) {
        *event_flags_ret = event_flags;
    }
    return ret;
}

esp_err_t usb_host_lib_unblock(void)
{
    //All devices must have been freed at this point
    HOST_ENTER_CRITICAL();
    HOST_CHECK_FROM_CRIT(p_host_lib_obj != NULL, ESP_ERR_INVALID_STATE);
    _unblock_lib(false);
    HOST_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t usb_host_lib_info(usb_host_lib_info_t *info_ret)
{
    HOST_CHECK(info_ret != NULL, ESP_ERR_INVALID_ARG);
    int num_devs_temp;
    int num_clients_temp;
    HOST_ENTER_CRITICAL();
    HOST_CHECK_FROM_CRIT(p_host_lib_obj != NULL, ESP_ERR_INVALID_STATE);
    num_clients_temp = p_host_lib_obj->dynamic.flags.num_clients;
    HOST_EXIT_CRITICAL();
    usbh_num_devs(&num_devs_temp);

    //Write back return values
    info_ret->num_devices = num_devs_temp;
    info_ret->num_clients = num_clients_temp;
    return ESP_OK;
}

// ------------------------------------------------ Client Functions ---------------------------------------------------

// ----------------------- Private -------------------------

static void _handle_pending_ep(client_t *client_obj)
{
    //Handle each EP on the pending list
    while (!TAILQ_EMPTY(&client_obj->dynamic.pending_ep_tailq)) {
        //Get the next pending EP.
        endpoint_t *ep_obj = TAILQ_FIRST(&client_obj->dynamic.pending_ep_tailq);
        TAILQ_REMOVE(&client_obj->dynamic.pending_ep_tailq, ep_obj, dynamic.tailq_entry);
        TAILQ_INSERT_TAIL(&client_obj->dynamic.idle_ep_tailq, ep_obj, dynamic.tailq_entry);
        ep_obj->dynamic.flags.pending = 0;
        hcd_pipe_event_t last_event = ep_obj->dynamic.last_event;
        uint32_t num_urb_dequeued = 0;

        HOST_EXIT_CRITICAL();
        //Handle pipe event
        switch (last_event) {
            case HCD_PIPE_EVENT_ERROR_XFER:
            case HCD_PIPE_EVENT_ERROR_URB_NOT_AVAIL:
            case HCD_PIPE_EVENT_ERROR_OVERFLOW:
            case HCD_PIPE_EVENT_ERROR_STALL:
                //The pipe is now stalled. Flush all pending URBs
                ESP_ERROR_CHECK(hcd_pipe_command(ep_obj->constant.pipe_hdl, HCD_PIPE_CMD_FLUSH));
                //All URBs in this pipe are now retired waiting to be dequeued. Fall through to dequeue them
                __attribute__((fallthrough));
            case HCD_PIPE_EVENT_URB_DONE: {
                //Dequeue all URBs and run their transfer callback
                urb_t *urb = hcd_urb_dequeue(ep_obj->constant.pipe_hdl);
                while (urb != NULL) {
                    //Clear the transfer's inflight flag to indicate the transfer is no longer inflight
                    urb->usb_host_inflight = false;
                    urb->transfer.callback(&urb->transfer);
                    num_urb_dequeued++;
                    urb = hcd_urb_dequeue(ep_obj->constant.pipe_hdl);
                }
                break;
            }
            default:
                abort();    //Should never occur
                break;
        }
        HOST_ENTER_CRITICAL();

        //Update the endpoint's number of URB's inflight
        assert(num_urb_dequeued <= ep_obj->dynamic.num_urb_inflight);
        ep_obj->dynamic.num_urb_inflight -= num_urb_dequeued;
    }
}

// ----------------------- Public --------------------------

esp_err_t usb_host_client_register(const usb_host_client_config_t *client_config, usb_host_client_handle_t *client_hdl_ret)
{
    HOST_CHECK(p_host_lib_obj, ESP_ERR_INVALID_STATE);
    HOST_CHECK(client_config != NULL && client_hdl_ret != NULL, ESP_ERR_INVALID_ARG);
    HOST_CHECK(client_config->max_num_event_msg > 0, ESP_ERR_INVALID_ARG);
    if (!client_config->is_synchronous) {
        //Asynchronous clients must provide a
        HOST_CHECK(client_config->async.client_event_callback != NULL, ESP_ERR_INVALID_ARG);
    }

    esp_err_t ret;
    //Create client object
    client_t *client_obj = heap_caps_calloc(1, sizeof(client_t), MALLOC_CAP_DEFAULT);
    SemaphoreHandle_t event_sem = xSemaphoreCreateBinary();
    QueueHandle_t event_msg_queue = xQueueCreate(client_config->max_num_event_msg, sizeof(usb_host_client_event_msg_t));
    if (client_obj == NULL || event_sem == NULL || event_msg_queue == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto alloc_err;
    }
    //Initialize client object
    TAILQ_INIT(&client_obj->dynamic.pending_ep_tailq);
    TAILQ_INIT(&client_obj->dynamic.idle_ep_tailq);
    TAILQ_INIT(&client_obj->mux_protected.interface_tailq);
    TAILQ_INIT(&client_obj->dynamic.done_ctrl_xfer_tailq);
    client_obj->constant.event_sem = event_sem;
    client_obj->constant.event_callback = client_config->async.client_event_callback;
    client_obj->constant.callback_arg = client_config->async.callback_arg;
    client_obj->constant.event_msg_queue = event_msg_queue;

    //Add client to the host library's list of clients
    xSemaphoreTake(p_host_lib_obj->constant.mux_lock, portMAX_DELAY);
    HOST_ENTER_CRITICAL();
    p_host_lib_obj->dynamic.flags.num_clients++;
    HOST_EXIT_CRITICAL();
    TAILQ_INSERT_TAIL(&p_host_lib_obj->mux_protected.client_tailq, client_obj, dynamic.tailq_entry);
    xSemaphoreGive(p_host_lib_obj->constant.mux_lock);

    //Write back client handle
    *client_hdl_ret = (usb_host_client_handle_t)client_obj;
    ret = ESP_OK;
    return ret;

alloc_err:
    if (event_msg_queue) {
        vQueueDelete(event_msg_queue);
    }
    if (event_sem) {
        vSemaphoreDelete(event_sem);
    }
    heap_caps_free(client_obj);
    return ESP_OK;
}

esp_err_t usb_host_client_deregister(usb_host_client_handle_t client_hdl)
{
    HOST_CHECK(client_hdl != NULL, ESP_ERR_INVALID_ARG);
    client_t *client_obj = (client_t *)client_hdl;
    esp_err_t ret;

    //We take the mux_lock because we need to access the host library's client_tailq
    xSemaphoreTake(p_host_lib_obj->constant.mux_lock, portMAX_DELAY);
    HOST_ENTER_CRITICAL();
    //Check that client can currently deregistered
    bool can_deregister;
    if (!TAILQ_EMPTY(&client_obj->dynamic.pending_ep_tailq) ||
        !TAILQ_EMPTY(&client_obj->dynamic.idle_ep_tailq) ||
        !TAILQ_EMPTY(&client_obj->dynamic.done_ctrl_xfer_tailq) ||
        client_obj->dynamic.flags.handling_events ||
        client_obj->dynamic.flags.blocked ||
        client_obj->dynamic.flags.taking_mux ||
        client_obj->dynamic.flags.num_intf_claimed != 0 ||
        client_obj->dynamic.num_done_ctrl_xfer != 0 ||
        client_obj->dynamic.opened_dev_addr_map != 0) {
            can_deregister = false;
        } else {
            can_deregister = true;
        }
    HOST_EXIT_CRITICAL();
    if (!can_deregister) {
        ret = ESP_ERR_INVALID_STATE;
        goto exit;
    }

    //Remove client object from the library's list of clients
    TAILQ_REMOVE(&p_host_lib_obj->mux_protected.client_tailq, client_obj, dynamic.tailq_entry);
    HOST_ENTER_CRITICAL();
    p_host_lib_obj->dynamic.flags.num_clients--;
    if (p_host_lib_obj->dynamic.flags.num_clients == 0) {
        //This is the last client being deregistered. Notify the lib handler
        p_host_lib_obj->dynamic.lib_event_flags |= USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS;
        _unblock_lib(false);
    }
    HOST_EXIT_CRITICAL();
    //Free client object
    vQueueDelete(client_obj->constant.event_msg_queue);
    vSemaphoreDelete(client_obj->constant.event_sem);
    heap_caps_free(client_obj);
    ret = ESP_OK;
exit:
    xSemaphoreGive(p_host_lib_obj->constant.mux_lock);
    return ret;
}

esp_err_t usb_host_client_handle_events(usb_host_client_handle_t client_hdl, TickType_t timeout_ticks)
{
    HOST_CHECK(client_hdl != NULL, ESP_ERR_INVALID_ARG);
    esp_err_t ret;
    client_t *client_obj = (client_t *)client_hdl;

    HOST_ENTER_CRITICAL();
    if (!client_obj->dynamic.flags.events_pending) {
        //There are currently no events, wait for one to occur
        client_obj->dynamic.flags.blocked = 1;
        HOST_EXIT_CRITICAL();
        BaseType_t sem_ret = xSemaphoreTake(client_obj->constant.event_sem, timeout_ticks);
        HOST_ENTER_CRITICAL();
        client_obj->dynamic.flags.blocked = 0;
        if (sem_ret == pdFALSE) {
            HOST_EXIT_CRITICAL();
            //Timed out waiting for semaphore
            ret = ESP_ERR_TIMEOUT;
            goto exit;
        }
    }
    //Mark that we're processing events
    client_obj->dynamic.flags.handling_events = 1;
    while (client_obj->dynamic.flags.handling_events) {
        //Handle pending endpoints
        if (!TAILQ_EMPTY(&client_obj->dynamic.pending_ep_tailq)) {
            _handle_pending_ep(client_obj);
        }
        //Handle any done control transfers
        while (client_obj->dynamic.num_done_ctrl_xfer > 0) {
            urb_t *urb = TAILQ_FIRST(&client_obj->dynamic.done_ctrl_xfer_tailq);
            TAILQ_REMOVE(&client_obj->dynamic.done_ctrl_xfer_tailq, urb, tailq_entry);
            client_obj->dynamic.num_done_ctrl_xfer--;
            HOST_EXIT_CRITICAL();
            //Clear the transfer's inflight flag to indicate the transfer is no longer inflight
            urb->usb_host_inflight = false;
            //Call the transfer's callback
            urb->transfer.callback(&urb->transfer);
            HOST_ENTER_CRITICAL();
        }
        //Handle event messages
        while (uxQueueMessagesWaiting(client_obj->constant.event_msg_queue) > 0) {
            HOST_EXIT_CRITICAL();
            //Dequeue the event message and call the client event callback
            usb_host_client_event_msg_t event_msg;
            BaseType_t queue_ret = xQueueReceive(client_obj->constant.event_msg_queue, &event_msg, 0);
            assert(queue_ret == pdTRUE);
            client_obj->constant.event_callback(&event_msg, client_obj->constant.callback_arg);
            HOST_ENTER_CRITICAL();
        }
        //Check each event again to see any new events occurred
        if (TAILQ_EMPTY(&client_obj->dynamic.pending_ep_tailq) &&
            client_obj->dynamic.num_done_ctrl_xfer == 0 &&
            uxQueueMessagesWaiting(client_obj->constant.event_msg_queue) == 0) {
            //All pending endpoints and event messages handled
            client_obj->dynamic.flags.events_pending = 0;
            client_obj->dynamic.flags.handling_events = 0;
        }
    }
    HOST_EXIT_CRITICAL();

    ret = ESP_OK;
exit:
    return ret;
}

esp_err_t usb_host_client_unblock(usb_host_client_handle_t client_hdl)
{
    HOST_CHECK(client_hdl != NULL, ESP_ERR_INVALID_ARG);
    client_t *client_obj = (client_t *)client_hdl;

    HOST_ENTER_CRITICAL();
    _unblock_client(client_obj, false);
    HOST_EXIT_CRITICAL();

    return ESP_OK;
}

// ------------------------------------------------- Device Handling ---------------------------------------------------

esp_err_t usb_host_device_open(usb_host_client_handle_t client_hdl, uint8_t dev_addr, usb_device_handle_t *dev_hdl_ret)
{
    HOST_CHECK(dev_addr > 0 && client_hdl != NULL && dev_hdl_ret != NULL, ESP_ERR_INVALID_ARG);
    client_t *client_obj = (client_t *)client_hdl;

    esp_err_t ret;
    usb_device_handle_t dev_hdl;
    ret = usbh_dev_open(dev_addr, &dev_hdl);
    if (ret != ESP_OK) {
        goto exit;
    }

    HOST_ENTER_CRITICAL();
    if (_check_client_opened_device(client_obj, dev_addr)) {
        //Client has already opened the device. Close it and return an error
        ret = ESP_ERR_INVALID_STATE;
        HOST_EXIT_CRITICAL();
        goto already_opened;
    }
    //Record in client object that we have opened the device of this address
    _record_client_opened_device(client_obj, dev_addr);
    HOST_EXIT_CRITICAL();

    *dev_hdl_ret = dev_hdl;
    ret = ESP_OK;
    return ret;

already_opened:
    ESP_ERROR_CHECK(usbh_dev_close(dev_hdl));
exit:
    return ret;
}

esp_err_t usb_host_device_close(usb_host_client_handle_t client_hdl, usb_device_handle_t dev_hdl)
{
    HOST_CHECK(dev_hdl != NULL && client_hdl != NULL, ESP_ERR_INVALID_ARG);
    client_t *client_obj = (client_t *)client_hdl;

    //We take the lock because we need to walk the interface list
    xSemaphoreTake(p_host_lib_obj->constant.mux_lock, portMAX_DELAY);
    esp_err_t ret;
    //Check that all interfaces claimed by this client do not belong to this device
    bool all_released = true;
    interface_t *intf_obj;
    TAILQ_FOREACH(intf_obj, &client_obj->mux_protected.interface_tailq, mux_protected.tailq_entry) {
        if (intf_obj->constant.dev_hdl == dev_hdl) {
            all_released = false;
            break;
        }
    }
    if (!all_released) {
        ret = ESP_ERR_INVALID_STATE;
        goto exit;
    }

    //Check that client actually opened the device in the first place
    HOST_ENTER_CRITICAL();
    uint8_t dev_addr;
    ESP_ERROR_CHECK(usbh_dev_get_addr(dev_hdl, &dev_addr));
    HOST_CHECK_FROM_CRIT(_check_client_opened_device(client_obj, dev_addr), ESP_ERR_NOT_FOUND);
    if (!_check_client_opened_device(client_obj, dev_addr)) {
        //Client never opened this device
        ret = ESP_ERR_INVALID_STATE;
        HOST_EXIT_CRITICAL();
        goto exit;
    }
    //Proceed to clear the record of the device form the client
    _clear_client_opened_device(client_obj, dev_addr);
    HOST_EXIT_CRITICAL();

    ESP_ERROR_CHECK(usbh_dev_close(dev_hdl));
    ret = ESP_OK;
exit:
    xSemaphoreGive(p_host_lib_obj->constant.mux_lock);
    return ret;
}

esp_err_t usb_host_device_free_all(void)
{
    HOST_ENTER_CRITICAL();
    HOST_CHECK_FROM_CRIT(p_host_lib_obj->dynamic.flags.num_clients == 0, ESP_ERR_INVALID_STATE);    //All clients must have been deregistered
    HOST_EXIT_CRITICAL();
    esp_err_t ret;
    ret = usbh_dev_mark_all_free();
    //If ESP_ERR_NOT_FINISHED is returned, caller must wait for USB_HOST_LIB_EVENT_FLAGS_ALL_FREE to confirm all devices are free
    return ret;
}

esp_err_t usb_host_device_addr_list_fill(int list_len, uint8_t *dev_addr_list, int *num_dev_ret)
{
    HOST_CHECK(dev_addr_list != NULL && num_dev_ret != NULL, ESP_ERR_INVALID_ARG);
    return usbh_dev_addr_list_fill(list_len, dev_addr_list, num_dev_ret);
}

// ------------------------------------------------- Device Requests ---------------------------------------------------

// ------------------- Cached Requests ---------------------

esp_err_t usb_host_device_info(usb_device_handle_t dev_hdl, usb_device_info_t *dev_info)
{
    HOST_CHECK(dev_hdl != NULL && dev_info != NULL, ESP_ERR_INVALID_ARG);
    return usbh_dev_get_info(dev_hdl, dev_info);
}

// ----------------------------------------------- Descriptor Requests -------------------------------------------------

// ----------------- Cached Descriptors --------------------

esp_err_t usb_host_get_device_descriptor(usb_device_handle_t dev_hdl, const usb_device_desc_t **device_desc)
{
    HOST_CHECK(dev_hdl != NULL && device_desc != NULL, ESP_ERR_INVALID_ARG);
    return usbh_dev_get_desc(dev_hdl, device_desc);
}

esp_err_t usb_host_get_active_config_descriptor(usb_device_handle_t dev_hdl, const usb_config_desc_t **config_desc)
{
    HOST_CHECK(dev_hdl != NULL && config_desc != NULL, ESP_ERR_INVALID_ARG);
    return usbh_dev_get_config_desc(dev_hdl, config_desc);
}

// ----------------------------------------------- Interface Functions -------------------------------------------------

// ----------------------- Private -------------------------

static esp_err_t endpoint_alloc(usb_device_handle_t dev_hdl, const usb_ep_desc_t *ep_desc, interface_t *intf_obj, endpoint_t **ep_obj_ret)
{
    endpoint_t *ep_obj = heap_caps_calloc(1, sizeof(endpoint_t), MALLOC_CAP_DEFAULT);
    if (ep_obj == NULL) {
        return ESP_ERR_NO_MEM;
    }
    esp_err_t ret;
    usbh_ep_config_t ep_config = {
        .ep_desc = ep_desc,
        .pipe_cb = pipe_callback,
        .pipe_cb_arg = (void *)ep_obj,
        .context = (void *)ep_obj,
    };
    hcd_pipe_handle_t pipe_hdl;
    ret = usbh_ep_alloc(dev_hdl, &ep_config, &pipe_hdl);
    if (ret != ESP_OK) {
        goto ep_alloc_err;
    }
    //Initialize endpoint object
    ep_obj->constant.pipe_hdl = pipe_hdl;
    ep_obj->constant.ep_desc = ep_desc;
    ep_obj->constant.intf_obj = intf_obj;
    //Write back result
    *ep_obj_ret = ep_obj;
    ret = ESP_OK;
    return ret;

ep_alloc_err:
    heap_caps_free(ep_obj);
    return ret;
}

static void endpoint_free(usb_device_handle_t dev_hdl, endpoint_t *ep_obj)
{
    if (ep_obj == NULL) {
        return;
    }
    //Free the underlying endpoint
    ESP_ERROR_CHECK(usbh_ep_free(dev_hdl, ep_obj->constant.ep_desc->bEndpointAddress));
    //Free the endpoint object
    heap_caps_free(ep_obj);
}

static interface_t *interface_alloc(client_t *client_obj, usb_device_handle_t dev_hdl, const usb_intf_desc_t *intf_desc)
{
    interface_t *intf_obj = heap_caps_calloc(1, sizeof(interface_t) + (sizeof(endpoint_t *) * intf_desc->bNumEndpoints), MALLOC_CAP_DEFAULT);
    if (intf_obj == NULL) {
        return NULL;
    }
    intf_obj->constant.intf_desc = intf_desc;
    intf_obj->constant.client_obj = client_obj;
    intf_obj->constant.dev_hdl = dev_hdl;
    return intf_obj;
}

static void interface_free(interface_t *intf_obj)
{
    if (intf_obj == NULL) {
        return;
    }
    for (int i = 0; i < intf_obj->constant.intf_desc->bNumEndpoints; i++) {
        assert(intf_obj->constant.endpoints[i] == NULL);
    }
    heap_caps_free(intf_obj);
}

static esp_err_t interface_claim(client_t *client_obj, usb_device_handle_t dev_hdl, const usb_config_desc_t *config_desc, uint8_t bInterfaceNumber, uint8_t bAlternateSetting, interface_t **intf_obj_ret)
{
    esp_err_t ret;
    //We need to walk to configuration descriptor to find the correct interface descriptor, and each of its constituent endpoint descriptors
    //Find the interface descriptor and allocate the interface object
    int offset_intf;
    const usb_intf_desc_t *intf_desc = usb_parse_interface_descriptor(config_desc, bInterfaceNumber, bAlternateSetting, &offset_intf);
    if (intf_desc == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto exit;
    }
    //Allocate interface object
    interface_t *intf_obj = interface_alloc(client_obj, dev_hdl, intf_desc);
    if (intf_obj == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto exit;
    }
    //Find each endpoint descriptor in the interface by index, and allocate those endpoints
    for (int i = 0; i < intf_desc->bNumEndpoints; i++) {
        int offset_ep = offset_intf;
        const usb_ep_desc_t *ep_desc = usb_parse_endpoint_descriptor_by_index(intf_desc, i, config_desc->wTotalLength, &offset_ep);
        if (ep_desc == NULL) {
            ret = ESP_ERR_NOT_FOUND;
            goto ep_alloc_err;
        }
        //Allocate the endpoint
        endpoint_t *ep_obj;
        ret = endpoint_alloc(dev_hdl, ep_desc, intf_obj, &ep_obj);
        if (ret != ESP_OK) {
            goto ep_alloc_err;
        }
        //Fill the interface object with the allocated endpoints
        intf_obj->constant.endpoints[i] = ep_obj;
    }
    //Add interface object to client (safe because we have already taken the mutex)
    TAILQ_INSERT_TAIL(&client_obj->mux_protected.interface_tailq, intf_obj, mux_protected.tailq_entry);
    //Add each endpoint to the client's endpoint list
    HOST_ENTER_CRITICAL();
    for (int i = 0; i < intf_desc->bNumEndpoints; i++) {
        TAILQ_INSERT_TAIL(&client_obj->dynamic.idle_ep_tailq, intf_obj->constant.endpoints[i], dynamic.tailq_entry);
    }
    HOST_EXIT_CRITICAL();
    //Write back result
    *intf_obj_ret = intf_obj;
    ret = ESP_OK;
    return ret;

ep_alloc_err:
    for (int i = 0; i < intf_desc->bNumEndpoints; i++) {
        endpoint_free(dev_hdl, intf_obj->constant.endpoints[i]);
        intf_obj->constant.endpoints[i] = NULL;
    }
    interface_free(intf_obj);
exit:
    return ret;
}

static esp_err_t interface_release(client_t *client_obj, usb_device_handle_t dev_hdl, uint8_t bInterfaceNumber)
{
    esp_err_t ret;
    //Find the interface object
    interface_t *intf_obj_iter;
    interface_t *intf_obj = NULL;
    TAILQ_FOREACH(intf_obj_iter, &client_obj->mux_protected.interface_tailq, mux_protected.tailq_entry) {
        if (intf_obj_iter->constant.dev_hdl == dev_hdl && intf_obj_iter->constant.intf_desc->bInterfaceNumber == bInterfaceNumber) {
            intf_obj = intf_obj_iter;
            break;
        }
    }
    if (intf_obj == NULL) {
        ret = ESP_ERR_NOT_FOUND;
        goto exit;
    }

    //Check that all endpoints in the interface are in a state to be freed
    HOST_ENTER_CRITICAL();
    bool can_free = true;
    for (int i = 0; i < intf_obj->constant.intf_desc->bNumEndpoints; i++) {
        endpoint_t *ep_obj = intf_obj->constant.endpoints[i];
        //Endpoint must not be on the pending list and must not have inflight URBs
        if (ep_obj->dynamic.num_urb_inflight != 0 || ep_obj->dynamic.flags.pending) {
            can_free = false;
            break;
        }
    }
    if (!can_free) {
        HOST_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto exit;
    }
    //Proceed to remove all endpoint objects from list
    for (int i = 0; i < intf_obj->constant.intf_desc->bNumEndpoints; i++) {
        TAILQ_REMOVE(&client_obj->dynamic.idle_ep_tailq, intf_obj->constant.endpoints[i], dynamic.tailq_entry);
    }
    HOST_EXIT_CRITICAL();

    //Remove the interface object from the list (safe because we have already taken the mutex)
    TAILQ_REMOVE(&client_obj->mux_protected.interface_tailq, intf_obj, mux_protected.tailq_entry);

    //Free each endpoint in the interface
        for (int i = 0; i < intf_obj->constant.intf_desc->bNumEndpoints; i++) {
        endpoint_free(dev_hdl, intf_obj->constant.endpoints[i]);
        intf_obj->constant.endpoints[i] = NULL;
    }
    //Free the interface object itself
    interface_free(intf_obj);
    ret = ESP_OK;
exit:
    return ret;
}

// ----------------------- Public --------------------------

esp_err_t usb_host_interface_claim(usb_host_client_handle_t client_hdl, usb_device_handle_t dev_hdl, uint8_t bInterfaceNumber, uint8_t bAlternateSetting)
{
    HOST_CHECK(client_hdl != NULL && dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    client_t *client_obj = (client_t *)client_hdl;

    HOST_ENTER_CRITICAL();
    uint8_t dev_addr;
    ESP_ERROR_CHECK(usbh_dev_get_addr(dev_hdl, &dev_addr));
    //Check if client actually opened device
    HOST_CHECK_FROM_CRIT(_check_client_opened_device(client_obj, dev_addr), ESP_ERR_INVALID_STATE);
    client_obj->dynamic.flags.taking_mux = 1;
    HOST_EXIT_CRITICAL();

    //Take mux lock. This protects the client being released or other clients from claiming interfaces
    xSemaphoreTake(p_host_lib_obj->constant.mux_lock, portMAX_DELAY);
    esp_err_t ret;
    const usb_config_desc_t *config_desc;
    ESP_ERROR_CHECK(usbh_dev_get_config_desc(dev_hdl, &config_desc));
    interface_t *intf_obj;
    //Claim interface
    ret = interface_claim(client_obj, dev_hdl, config_desc, bInterfaceNumber, bAlternateSetting, &intf_obj);
    if (ret != ESP_OK) {
        goto exit;
    }
    ret = ESP_OK;
exit:
    xSemaphoreGive(p_host_lib_obj->constant.mux_lock);

    HOST_ENTER_CRITICAL();
    if (ret == ESP_OK) {
        client_obj->dynamic.flags.num_intf_claimed++;
    }
    client_obj->dynamic.flags.taking_mux = 0;
    HOST_EXIT_CRITICAL();
    return ret;
}

esp_err_t usb_host_interface_release(usb_host_client_handle_t client_hdl, usb_device_handle_t dev_hdl, uint8_t bInterfaceNumber)
{
    HOST_CHECK(client_hdl != NULL && dev_hdl != NULL, ESP_ERR_INVALID_ARG);
    client_t *client_obj = (client_t *)client_hdl;

    HOST_ENTER_CRITICAL();
    uint8_t dev_addr;
    ESP_ERROR_CHECK(usbh_dev_get_addr(dev_hdl, &dev_addr));
    //Check if client actually opened device
    HOST_CHECK_FROM_CRIT(_check_client_opened_device(client_obj, dev_addr), ESP_ERR_INVALID_STATE);
    client_obj->dynamic.flags.taking_mux = 1;
    HOST_EXIT_CRITICAL();

    //Take mux lock. This protects the client being released or other clients from claiming interfaces
    xSemaphoreTake(p_host_lib_obj->constant.mux_lock, portMAX_DELAY);
    esp_err_t ret = interface_release(client_obj, dev_hdl, bInterfaceNumber);
    xSemaphoreGive(p_host_lib_obj->constant.mux_lock);

    HOST_ENTER_CRITICAL();
    if (ret == ESP_OK) {
        client_obj->dynamic.flags.num_intf_claimed--;
    }
    client_obj->dynamic.flags.taking_mux = 0;
    HOST_EXIT_CRITICAL();

    return ret;
}

esp_err_t usb_host_endpoint_halt(usb_device_handle_t dev_hdl, uint8_t bEndpointAddress)
{
    esp_err_t ret;
    endpoint_t *ep_obj = NULL;
    ret = usbh_ep_get_context(dev_hdl, bEndpointAddress, (void **)&ep_obj);
    if (ret != ESP_OK) {
        goto exit;
    }
    assert(ep_obj != NULL);
    ret = hcd_pipe_command(ep_obj->constant.pipe_hdl, HCD_PIPE_CMD_HALT);
exit:
    return ret;
}

esp_err_t usb_host_endpoint_flush(usb_device_handle_t dev_hdl, uint8_t bEndpointAddress)
{
    esp_err_t ret;
    endpoint_t *ep_obj = NULL;
    ret = usbh_ep_get_context(dev_hdl, bEndpointAddress, (void **)&ep_obj);
    if (ret != ESP_OK) {
        goto exit;
    }
    assert(ep_obj != NULL);
    ret = hcd_pipe_command(ep_obj->constant.pipe_hdl, HCD_PIPE_CMD_FLUSH);
exit:
    return ret;
}

esp_err_t usb_host_endpoint_clear(usb_device_handle_t dev_hdl, uint8_t bEndpointAddress)
{
    esp_err_t ret;
    endpoint_t *ep_obj = NULL;
    ret = usbh_ep_get_context(dev_hdl, bEndpointAddress, (void **)&ep_obj);
    if (ret != ESP_OK) {
        goto exit;
    }
    assert(ep_obj != NULL);
    ret = hcd_pipe_command(ep_obj->constant.pipe_hdl, HCD_PIPE_CMD_CLEAR);
exit:
    return ret;
}

// ------------------------------------------------ Asynchronous I/O ---------------------------------------------------

// ----------------------- Private -------------------------

static bool transfer_check(usb_transfer_t *transfer, usb_transfer_type_t type, int mps, bool is_in)
{
    if (transfer->callback == NULL) {
        ESP_LOGE(USB_HOST_TAG, "Transfer callback is NULL");
        return false;
    }
    //Check that the total transfer length does not exceed data buffer size
    if (transfer->num_bytes > transfer->data_buffer_size) {
        ESP_LOGE(USB_HOST_TAG, "Transfer num_bytes > data_buffer_size");
        return false;
    }
    if (type == USB_TRANSFER_TYPE_CTRL) {
        //Check that num_bytes and wLength are set correctly
        usb_setup_packet_t *setup_pkt = (usb_setup_packet_t *)transfer->data_buffer;
        if (transfer->num_bytes != sizeof(usb_setup_packet_t) + setup_pkt->wLength) {
            ESP_LOGE(USB_HOST_TAG, "Control transfer num_bytes wLength mismatch");
            return false;
        }
    } else if (type == USB_TRANSFER_TYPE_ISOCHRONOUS) {
        //Check that there is at least one isochronous packet descriptor
        if (transfer->num_isoc_packets <= 0) {
            ESP_LOGE(USB_HOST_TAG, "ISOC transfer has 0 packet descriptors");
            return false;
        }
        //Check that sum of all packet lengths add up to transfer length
        //If IN, check that each packet length is integer multiple of MPS
        int total_num_bytes = 0;
        bool mod_mps_all_zero = true;
        for (int i = 0; i < transfer->num_isoc_packets; i++) {
            total_num_bytes += transfer->isoc_packet_desc[i].num_bytes;
            if (transfer->isoc_packet_desc[i].num_bytes % mps != 0) {
                mod_mps_all_zero = false;
            }
        }
        if (transfer->num_bytes != total_num_bytes) {
            ESP_LOGE(USB_HOST_TAG, "ISOC transfer num_bytes not equal to total num_bytes of all packets");
            return false;
        }
        if (is_in && !mod_mps_all_zero) {
            ESP_LOGE(USB_HOST_TAG, "ISOC IN transfer all packets num_bytes must be integer multiple of MPS");
            return false;
        }
    } else {
        //Check that IN transfers are integer multiple of MPS
        if (is_in && (transfer->num_bytes % mps != 0)) {
            ESP_LOGE(USB_HOST_TAG, "IN transfer num_bytes must be integer multiple of MPS");
            return false;
        }
    }
    return true;
}

// ----------------------- Public --------------------------

esp_err_t usb_host_transfer_alloc(size_t data_buffer_size, int num_isoc_packets, usb_transfer_t **transfer)
{
    urb_t *urb = urb_alloc(data_buffer_size, 0, num_isoc_packets);
    if (urb == NULL) {
        return ESP_ERR_NO_MEM;
    }
    *transfer = &urb->transfer;
    return ESP_OK;
}

esp_err_t usb_host_transfer_free(usb_transfer_t *transfer)
{
    if (transfer == NULL) {
        return ESP_OK;
    }
    urb_t *urb = __containerof(transfer, urb_t, transfer);
    urb_free(urb);
    return ESP_OK;
}

esp_err_t usb_host_transfer_submit(usb_transfer_t *transfer)
{
    HOST_CHECK(transfer != NULL, ESP_ERR_INVALID_ARG);
    //Check that transfer and target endpoint are valid
    HOST_CHECK(transfer->device_handle != NULL, ESP_ERR_INVALID_ARG);   //Target device must be set
    HOST_CHECK((transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK) != 0, ESP_ERR_INVALID_ARG);
    endpoint_t *ep_obj = NULL;
    urb_t *urb_obj = __containerof(transfer, urb_t, transfer);
    esp_err_t ret;
    ret = usbh_ep_get_context(transfer->device_handle, transfer->bEndpointAddress, (void **)&ep_obj);
    if (ret != ESP_OK) {
        goto err;
    }
    assert(ep_obj != NULL);
    HOST_CHECK(transfer_check(transfer,
                              USB_EP_DESC_GET_XFERTYPE(ep_obj->constant.ep_desc),
                              USB_EP_DESC_GET_MPS(ep_obj->constant.ep_desc),
                              transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK), ESP_ERR_INVALID_ARG);
    //Check that we are not submitting a transfer already inflight
    HOST_CHECK(!urb_obj->usb_host_inflight, ESP_ERR_NOT_FINISHED);
    urb_obj->usb_host_inflight = true;
    HOST_ENTER_CRITICAL();
    ep_obj->dynamic.num_urb_inflight++;
    HOST_EXIT_CRITICAL();
    //Check if pipe is in a state to enqueue URBs
    if (hcd_pipe_get_state(ep_obj->constant.pipe_hdl) != HCD_PIPE_STATE_ACTIVE) {
        ret = ESP_ERR_INVALID_STATE;
        goto hcd_err;
    }
    ret = hcd_urb_enqueue(ep_obj->constant.pipe_hdl, urb_obj);
    if (ret != ESP_OK) {
        goto hcd_err;
    }
    ret = ESP_OK;
    return ret;

hcd_err:
    HOST_ENTER_CRITICAL();
    ep_obj->dynamic.num_urb_inflight--;
    HOST_EXIT_CRITICAL();
    urb_obj->usb_host_inflight = false;
err:
    return ret;
}

esp_err_t usb_host_transfer_submit_control(usb_host_client_handle_t client_hdl, usb_transfer_t *transfer)
{
    HOST_CHECK(client_hdl != NULL && transfer != NULL, ESP_ERR_INVALID_ARG);

    //Check that control transfer is valid
    HOST_CHECK(transfer->device_handle != NULL, ESP_ERR_INVALID_ARG);   //Target device must be set
    usb_device_handle_t dev_hdl = transfer->device_handle;
    bool xfer_is_in = ((usb_setup_packet_t *)transfer->data_buffer)->bmRequestType & USB_BM_REQUEST_TYPE_DIR_OUT;
    usb_device_info_t dev_info;
    ESP_ERROR_CHECK(usbh_dev_get_info(dev_hdl, &dev_info));
    HOST_CHECK(transfer_check(transfer, USB_TRANSFER_TYPE_CTRL, dev_info.bMaxPacketSize0, xfer_is_in), ESP_ERR_INVALID_ARG);
    //Control transfers must be targeted at EP 0
    HOST_CHECK((transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK) == 0, ESP_ERR_INVALID_ARG);

    urb_t *urb_obj = __containerof(transfer, urb_t, transfer);
    //Check that we are not submitting a transfer already inflight
    HOST_CHECK(!urb_obj->usb_host_inflight, ESP_ERR_NOT_FINISHED);
    urb_obj->usb_host_inflight = true;
    //Save client handle into URB
    urb_obj->usb_host_client = (void *)client_hdl;

    esp_err_t ret;
    ret = usbh_dev_submit_ctrl_urb(dev_hdl, urb_obj);
    if (ret != ESP_OK) {
        urb_obj->usb_host_inflight = false;
    }
    return ret;
}
