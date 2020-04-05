/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org),
 *               2020 Espressif Systems (Shanghai) Co. Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#include "tusb_option.h"

#if TUSB_OPT_DEVICE_ENABLED

#include "tusb.h"
#include "usbd.h"
#include "device/usbd_pvt.h"
#include "dcd.h"
#include "esp_log.h"

static const char *TAG = "TUSB:device";
#ifndef CFG_TUD_TASK_QUEUE_SZ
#define CFG_TUD_TASK_QUEUE_SZ 16
#endif

//--------------------------------------------------------------------+
// Device Data
//--------------------------------------------------------------------+
typedef struct {
    struct TU_ATTR_PACKED {
        volatile uint8_t connected : 1;
        volatile uint8_t configured : 1;
        volatile uint8_t suspended : 1;

        uint8_t remote_wakeup_en : 1;      // enable/disable by host
        uint8_t remote_wakeup_support : 1; // configuration descriptor's attribute
        uint8_t self_powered : 1;          // configuration descriptor's attribute
    };

    uint8_t ep_busy_map[2];  // bit mask for busy endpoint
    uint8_t ep_stall_map[2]; // bit map for stalled endpoint

    uint8_t itf2drv[16];  // map interface number to driver (0xff is invalid)
    uint8_t ep2drv[8][2]; // map endpoint to driver ( 0xff is invalid )
} usbd_device_t;

static usbd_device_t _usbd_dev = {0};

//--------------------------------------------------------------------+
// Class Driver
//--------------------------------------------------------------------+
typedef struct {
    uint8_t class_code;

    void (*init)(void);
    bool (*open)(uint8_t rhport, tusb_desc_interface_t const *desc_intf, uint16_t *p_length);
    bool (*control_request)(uint8_t rhport, tusb_control_request_t const *request);
    bool (*control_request_complete)(uint8_t rhport, tusb_control_request_t const *request);
    bool (*xfer_cb)(uint8_t rhport, uint8_t ep_addr, xfer_result_t, uint32_t);
    void (*sof)(uint8_t rhport);
    void (*reset)(uint8_t);
} usbd_class_driver_t;

static usbd_class_driver_t const usbd_class_drivers[] = {
#if CFG_TUD_CDC
    {
        .class_code = TUSB_CLASS_CDC,
        .init = cdcd_init,
        .open = cdcd_open,
        .control_request = cdcd_control_request,
        .control_request_complete = cdcd_control_request_complete,
        .xfer_cb = cdcd_xfer_cb,
        .sof = NULL,
        .reset = cdcd_reset
    },
#endif

#if CFG_TUD_MSC
    {
        .class_code = TUSB_CLASS_MSC,
        .init = mscd_init,
        .open = mscd_open,
        .control_request = mscd_control_request,
        .control_request_complete = mscd_control_request_complete,
        .xfer_cb = mscd_xfer_cb,
        .sof = NULL,
        .reset = mscd_reset
    },
#endif

#if CFG_TUD_HID
    {
        .class_code = TUSB_CLASS_HID,
        .init = hidd_init,
        .open = hidd_open,
        .control_request = hidd_control_request,
        .control_request_complete = hidd_control_request_complete,
        .xfer_cb = hidd_xfer_cb,
        .sof = NULL,
        .reset = hidd_reset
    },
#endif

#if CFG_TUD_MIDI
    {
        .class_code = TUSB_CLASS_AUDIO,
        .init = midid_init,
        .open = midid_open,
        .control_request = midid_control_request,
        .control_request_complete = midid_control_request_complete,
        .xfer_cb = midid_xfer_cb,
        .sof = NULL,
        .reset = midid_reset
    },
#endif

#if CFG_TUD_CUSTOM_CLASS
    {
        .class_code = TUSB_CLASS_VENDOR_SPECIFIC,
        .init = cusd_init,
        .open = cusd_open,
        .control_request = cusd_control_request,
        .control_request_complete = cusd_control_request_complete,
        .xfer_cb = cusd_xfer_cb,
        .sof = NULL,
        .reset = cusd_reset
    },
#endif
};

enum {
    USBD_CLASS_DRIVER_COUNT = TU_ARRAY_SZIE(usbd_class_drivers)
};

//--------------------------------------------------------------------+
// DCD Event
//--------------------------------------------------------------------+

// Event queue
// OPT_MODE_DEVICE is used by OS NONE for mutex (disable usb isr)
OSAL_QUEUE_DEF(OPT_MODE_DEVICE, _usbd_qdef, CFG_TUD_TASK_QUEUE_SZ, dcd_event_t);
static osal_queue_t _usbd_q;

//--------------------------------------------------------------------+
// Prototypes
//--------------------------------------------------------------------+
static void mark_interface_endpoint(uint8_t ep2drv[8][2], uint8_t const *p_desc, uint16_t desc_len, uint8_t driver_id);
static bool process_control_request(uint8_t rhport, tusb_control_request_t const *p_request);
static bool process_set_config(uint8_t rhport, uint8_t cfg_num);
static bool process_get_descriptor(uint8_t rhport, tusb_control_request_t const *p_request);

void usbd_control_reset(uint8_t rhport);
bool usbd_control_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t event, uint32_t xferred_bytes);
void usbd_control_set_complete_callback(bool (*fp)(uint8_t, tusb_control_request_t const *));

//--------------------------------------------------------------------+
// Application API
//--------------------------------------------------------------------+
bool tud_mounted(void)
{
    return _usbd_dev.configured;
}

bool tud_suspended(void)
{
    return _usbd_dev.suspended;
}

bool tud_remote_wakeup(void)
{
    // only wake up host if this feature is supported and enabled and we are suspended
    TU_VERIFY(_usbd_dev.suspended && _usbd_dev.remote_wakeup_support && _usbd_dev.remote_wakeup_en);
    dcd_remote_wakeup(TUD_OPT_RHPORT);
    return true;
}

//--------------------------------------------------------------------+
// USBD Task
//--------------------------------------------------------------------+
bool usbd_init(void)
{
    tu_varclr(&_usbd_dev);

    // Init device queue & task

    ESP_LOGV(TAG, "Init device queue & task...");
    _usbd_q = osal_queue_create(&_usbd_qdef);
    TU_ASSERT(_usbd_q != NULL);
    ESP_LOGV(TAG, "Init device queue & task: Done");

// Init class drivers
#   if USBD_CLASS_DRIVER_COUNT
    for (uint8_t i = 0; i < USBD_CLASS_DRIVER_COUNT; i++) {
        usbd_class_drivers[i].init();
    }
#   endif

    // Init device controller driver

    ESP_LOGV(TAG, "dcd_init...");
    dcd_init(TUD_OPT_RHPORT);
    ESP_LOGV(TAG, "dcd_init: Done");
    ESP_LOGV(TAG, "dcd_int_enable...");
    dcd_int_enable(TUD_OPT_RHPORT);
    ESP_LOGV(TAG, "dcd_int_enable: Done");

    return true;
}

static void usbd_reset(uint8_t rhport)
{
    tu_varclr(&_usbd_dev);

    memset(_usbd_dev.itf2drv, 0xff, sizeof(_usbd_dev.itf2drv)); // invalid mapping
    memset(_usbd_dev.ep2drv, 0xff, sizeof(_usbd_dev.ep2drv));   // invalid mapping

    usbd_control_reset(rhport);
#   if USBD_CLASS_DRIVER_COUNT
    for (uint8_t i = 0; i < USBD_CLASS_DRIVER_COUNT; i++) {
        if (usbd_class_drivers[i].reset) {
            usbd_class_drivers[i].reset(rhport);
        }
    }
#   endif
}

/* USB Device Driver task
 * This top level thread manages all device controller event and delegates events to class-specific drivers.
 * This should be called periodically within the mainloop or rtos thread.
 *
   @code
    int main(void)
    {
      application_init();
      tusb_init();

      while(1) // the mainloop
      {
        application_code();

        tud_task(); // tinyusb device task
      }
    }
    @endcode
 */
void tud_task(void)
{
    // Skip if stack is not initialized
    bool tusb_ready = tusb_inited();
    if (!tusb_ready) {

        ESP_LOGV(TAG, "is not ready");
        return;
    }

    ESP_LOGV(TAG, "started");
    // Loop until there is no more events in the queue
    while (1) {
        dcd_event_t event;

        volatile bool ev = osal_queue_receive(_usbd_q, &event);
        if (!ev) {
            ESP_LOGV(TAG, "USB EVENT ...empty...");
            return;
        }

        ESP_LOGV(TAG, "USB EVENT: %u", event.event_id);
        switch (event.event_id) {
        case DCD_EVENT_BUS_RESET:
            ESP_LOGV(TAG, "USB EVENT bus_reset");
            usbd_reset(event.rhport);
            break;

        case DCD_EVENT_UNPLUGGED:
            ESP_LOGV(TAG, "USB EVENT unplugged");
            usbd_reset(event.rhport);

            // invoke callback
            if (tud_umount_cb) {
                tud_umount_cb();
            }
            break;

        case DCD_EVENT_SETUP_RECEIVED:

            ESP_LOGV(TAG, "USB EVENT setup_received");
            // Mark as connected after receiving 1st setup packet.
            // But it is easier to set it every time instead of wasting time to check then set
            _usbd_dev.connected = 1;

            // Process control request
            if (!process_control_request(event.rhport, &event.setup_received)) {
                // Failed -> stall both control endpoint IN and OUT
                dcd_edpt_stall(event.rhport, 0);
                dcd_edpt_stall(event.rhport, 0 | TUSB_DIR_IN_MASK);
            }
            break;

        case DCD_EVENT_XFER_COMPLETE:
            // Only handle xfer callback in ready state
            // if (_usbd_dev.connected && !_usbd_dev.suspended)

            ESP_LOGV(TAG, "USB EVENT xfer_complete");
            {
                // Invoke the class callback associated with the endpoint address
                uint8_t const ep_addr = event.xfer_complete.ep_addr;
                uint8_t const epnum = tu_edpt_number(ep_addr);
                uint8_t const dir = tu_edpt_dir(ep_addr);

                _usbd_dev.ep_busy_map[dir] = (uint8_t)tu_bit_clear(_usbd_dev.ep_busy_map[dir], epnum);

                if (0 == tu_edpt_number(ep_addr)) {
                    // control transfer DATA stage callback
                    usbd_control_xfer_cb(event.rhport, ep_addr, event.xfer_complete.result, event.xfer_complete.len);
                } else {
                    uint8_t const drv_id = _usbd_dev.ep2drv[tu_edpt_number(ep_addr)][tu_edpt_dir(ep_addr)];
#               if USBD_CLASS_DRIVER_COUNT
                    TU_ASSERT(drv_id < USBD_CLASS_DRIVER_COUNT, );
#               endif
                    usbd_class_drivers[drv_id].xfer_cb(event.rhport, ep_addr, event.xfer_complete.result, event.xfer_complete.len);
                }
            }
            break;

        case DCD_EVENT_SUSPEND:
            ESP_LOGV(TAG, "USB EVENT suspend");
            if (tud_suspend_cb) {
                tud_suspend_cb(_usbd_dev.remote_wakeup_en);
            }
            break;

        case DCD_EVENT_RESUME:
            ESP_LOGV(TAG, "USB EVENT resume");
            if (tud_resume_cb) {
                tud_resume_cb();
            }
            break;

        case DCD_EVENT_SOF:
            ESP_LOGV(TAG, "USB EVENT sof");
#           if USBD_CLASS_DRIVER_COUNT
            for (uint8_t i = 0; i < USBD_CLASS_DRIVER_COUNT; i++) {
                if (usbd_class_drivers[i].sof) {
                    usbd_class_drivers[i].sof(event.rhport);
                }
            }
#           endif
            break;

        case USBD_EVENT_FUNC_CALL:
            ESP_LOGV(TAG, "USB EVENT func_call");
            if (event.func_call.func) {
                event.func_call.func(event.func_call.param);
            }
            break;

        default:
            ESP_LOGV(TAG, "USB EVENT unknown");
            TU_BREAKPOINT();
            break;
        }
    }
}

//--------------------------------------------------------------------+
// Control Request Parser & Handling
//--------------------------------------------------------------------+

// This handles the actual request and its response.
// return false will cause its caller to stall control endpoint
static bool process_control_request(uint8_t rhport, tusb_control_request_t const *p_request)
{
    usbd_control_set_complete_callback(NULL);

    switch (p_request->bmRequestType_bit.recipient) {
    //------------- Device Requests e.g in enumeration -------------//
    case TUSB_REQ_RCPT_DEVICE:
        if (TUSB_REQ_TYPE_STANDARD != p_request->bmRequestType_bit.type) {
            // Non standard request is not supported
            TU_BREAKPOINT();
            return false;
        }

        switch (p_request->bRequest) {
        case TUSB_REQ_SET_ADDRESS:
            ESP_LOGV(TAG, "TUSB_REQ_SET_ADDRESS");
            // Depending on mcu, status phase could be sent either before or after changing device address
            // Therefore DCD must include zero-length status response
            dcd_set_address(rhport, (uint8_t)p_request->wValue);
            return true; // skip status
            break;

        case TUSB_REQ_GET_CONFIGURATION: {
            ESP_LOGV(TAG, "TUSB_REQ_GET_CONFIGURATION");
            uint8_t cfgnum = _usbd_dev.configured ? 1 : 0;
            usbd_control_xfer(rhport, p_request, &cfgnum, 1);
        }
        break;

        case TUSB_REQ_SET_CONFIGURATION: {
            ESP_LOGV(TAG, "TUSB_REQ_SET_CONFIGURATION");
            uint8_t const cfg_num = (uint8_t)p_request->wValue;

            dcd_set_config(rhport, cfg_num);
            _usbd_dev.configured = cfg_num ? 1 : 0;

            if (cfg_num) {
                TU_ASSERT(process_set_config(rhport, cfg_num));
            }
            usbd_control_status(rhport, p_request);
        }
        break;

        case TUSB_REQ_GET_DESCRIPTOR:
            ESP_LOGV(TAG, "TUSB_REQ_GET_DESCRIPTOR");
            TU_VERIFY(process_get_descriptor(rhport, p_request));
            break;

        case TUSB_REQ_SET_FEATURE:
            ESP_LOGV(TAG, "TUSB_REQ_SET_FEATURE");
            // Only support remote wakeup for device feature
            TU_VERIFY(TUSB_REQ_FEATURE_REMOTE_WAKEUP == p_request->wValue);

            // Host may enable remote wake up before suspending especially HID device
            _usbd_dev.remote_wakeup_en = true;
            usbd_control_status(rhport, p_request);
            break;

        case TUSB_REQ_CLEAR_FEATURE:
            ESP_LOGV(TAG, "TUSB_REQ_CLEAR_FEATURE");
            // Only support remote wakeup for device feature
            TU_VERIFY(TUSB_REQ_FEATURE_REMOTE_WAKEUP == p_request->wValue);

            // Host may disable remote wake up after resuming
            _usbd_dev.remote_wakeup_en = false;
            usbd_control_status(rhport, p_request);
            break;

        case TUSB_REQ_GET_STATUS: {
            ESP_LOGV(TAG, "TUSB_REQ_GET_STATUS");
            // Device status bit mask
            // - Bit 0: Self Powered
            // - Bit 1: Remote Wakeup enabled
            uint16_t status = (_usbd_dev.self_powered ? 1 : 0) | (_usbd_dev.remote_wakeup_en ? 2 : 0);
            usbd_control_xfer(rhport, p_request, &status, 2);
        }
        break;

        // Unknown/Unsupported request
        default:
            TU_BREAKPOINT();
            return false;
        }
        break;

    //------------- Class/Interface Specific Request -------------//
    case TUSB_REQ_RCPT_INTERFACE: {
        uint8_t const itf = tu_u16_low(p_request->wIndex);
        uint8_t const drvid = _usbd_dev.itf2drv[itf];
#       if USBD_CLASS_DRIVER_COUNT
        TU_VERIFY(drvid < USBD_CLASS_DRIVER_COUNT);
#       endif
        usbd_control_set_complete_callback(usbd_class_drivers[drvid].control_request_complete);

        // stall control endpoint if driver return false
        return usbd_class_drivers[drvid].control_request(rhport, p_request);
    }
    break;

    //------------- Endpoint Request -------------//
    case TUSB_REQ_RCPT_ENDPOINT:
        // Non standard request is not supported
        TU_VERIFY(TUSB_REQ_TYPE_STANDARD == p_request->bmRequestType_bit.type);

        switch (p_request->bRequest) {
        case TUSB_REQ_GET_STATUS: {
            uint16_t status = usbd_edpt_stalled(rhport, tu_u16_low(p_request->wIndex)) ? 0x0001 : 0x0000;
            usbd_control_xfer(rhport, p_request, &status, 2);
        }
        break;

        case TUSB_REQ_CLEAR_FEATURE:
            if (TUSB_REQ_FEATURE_EDPT_HALT == p_request->wValue) {
                usbd_edpt_clear_stall(rhport, tu_u16_low(p_request->wIndex));
            }
            usbd_control_status(rhport, p_request);
            break;

        case TUSB_REQ_SET_FEATURE:
            if (TUSB_REQ_FEATURE_EDPT_HALT == p_request->wValue) {
                usbd_edpt_stall(rhport, tu_u16_low(p_request->wIndex));
            }
            usbd_control_status(rhport, p_request);
            break;

        // Unknown/Unsupported request
        default:
            TU_BREAKPOINT();
            return false;
        }
        break;

    // Unknown recipient
    default:
        TU_BREAKPOINT();
        return false;
    }

    return true;
}

// Process Set Configure Request
// This function parse configuration descriptor & open drivers accordingly
static bool process_set_config(uint8_t rhport, uint8_t cfg_num)
{
    tusb_desc_configuration_t const *desc_cfg = (tusb_desc_configuration_t const *)tud_descriptor_configuration_cb(cfg_num - 1); // index is cfg_num-1
    TU_ASSERT(desc_cfg != NULL && desc_cfg->bDescriptorType == TUSB_DESC_CONFIGURATION);

    // Parse configuration descriptor
    _usbd_dev.remote_wakeup_support = (desc_cfg->bmAttributes & TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP) ? 1 : 0;
    _usbd_dev.self_powered = (desc_cfg->bmAttributes & TUSB_DESC_CONFIG_ATT_SELF_POWERED) ? 1 : 0;

    // Parse interface descriptor
    uint8_t const *p_desc = ((uint8_t const *)desc_cfg) + sizeof(tusb_desc_configuration_t);
    uint8_t const *desc_end = ((uint8_t const *)desc_cfg) + desc_cfg->wTotalLength;

    while (p_desc < desc_end) {
        // Each interface always starts with Interface or Association descriptor
        if (TUSB_DESC_INTERFACE_ASSOCIATION == tu_desc_type(p_desc)) {
            p_desc = tu_desc_next(p_desc); // ignore Interface Association
        } else {
            TU_ASSERT(TUSB_DESC_INTERFACE == tu_desc_type(p_desc));

            tusb_desc_interface_t *desc_itf = (tusb_desc_interface_t *)p_desc;

            // Check if class is supported
            uint8_t drv_id = 0;
#           if USBD_CLASS_DRIVER_COUNT
            for (; drv_id < USBD_CLASS_DRIVER_COUNT; drv_id++) {
                if (usbd_class_drivers[drv_id].class_code == desc_itf->bInterfaceClass) {
                    break;
                }
            }
#           endif
            // Interface number must not be used already TODO alternate interface
            TU_ASSERT(0xff == _usbd_dev.itf2drv[desc_itf->bInterfaceNumber]);
            _usbd_dev.itf2drv[desc_itf->bInterfaceNumber] = drv_id;

            uint16_t itf_len = 0;
            TU_ASSERT(usbd_class_drivers[drv_id].open(rhport, desc_itf, &itf_len));
            TU_ASSERT(itf_len >= sizeof(tusb_desc_interface_t));

            mark_interface_endpoint(_usbd_dev.ep2drv, p_desc, itf_len, drv_id);

            p_desc += itf_len; // next interface
        }
    }

    // invoke callback
    if (tud_mount_cb) {
        tud_mount_cb();
    }

    return true;
}

// Helper marking endpoint of interface belongs to class driver
static void mark_interface_endpoint(uint8_t ep2drv[8][2], uint8_t const *p_desc, uint16_t desc_len, uint8_t driver_id)
{
    uint16_t len = 0;

    while (len < desc_len) {
        if (TUSB_DESC_ENDPOINT == tu_desc_type(p_desc)) {
            uint8_t const ep_addr = ((tusb_desc_endpoint_t const *)p_desc)->bEndpointAddress;

            ep2drv[tu_edpt_number(ep_addr)][tu_edpt_dir(ep_addr)] = driver_id;
        }

        len += tu_desc_len(p_desc);
        p_desc = tu_desc_next(p_desc);
    }
}

// return descriptor's buffer and update desc_len
static bool process_get_descriptor(uint8_t rhport, tusb_control_request_t const *p_request)
{
    tusb_desc_type_t const desc_type = (tusb_desc_type_t)tu_u16_high(p_request->wValue);
    uint8_t const desc_index = tu_u16_low(p_request->wValue);

    switch (desc_type) {
    case TUSB_DESC_DEVICE:
        return usbd_control_xfer(rhport, p_request, (void *)tud_descriptor_device_cb(), sizeof(tusb_desc_device_t));
        break;

    case TUSB_DESC_CONFIGURATION: {
        tusb_desc_configuration_t const *desc_config = (tusb_desc_configuration_t const *)tud_descriptor_configuration_cb(desc_index);
        return usbd_control_xfer(rhport, p_request, (void *)desc_config, desc_config->wTotalLength);
    }
    break;

    case TUSB_DESC_STRING:
        // String Descriptor always uses the desc set from user
        if (desc_index == 0xEE) {
            // The 0xEE index string is a Microsoft USB extension.
            // It can be used to tell Windows what driver it should use for the device !!!
            return false;
        } else {
            uint8_t const *desc_str = (uint8_t const *)tud_descriptor_string_cb(desc_index);
            TU_ASSERT(desc_str);

            // first byte of descriptor is its size
            return usbd_control_xfer(rhport, p_request, (void *)desc_str, desc_str[0]);
        }
        break;

    case TUSB_DESC_DEVICE_QUALIFIER:
        return false;
        break;

    default:
        return false;
    }

    return true;
}

//--------------------------------------------------------------------+
// DCD Event Handler
//--------------------------------------------------------------------+
void dcd_event_handler(dcd_event_t const *event, bool in_isr)
{
    switch (event->event_id) {
    case DCD_EVENT_BUS_RESET:
        osal_queue_send(_usbd_q, event, in_isr);
        break;

    case DCD_EVENT_UNPLUGGED:
        _usbd_dev.connected = 0;
        _usbd_dev.configured = 0;
        _usbd_dev.suspended = 0;
        osal_queue_send(_usbd_q, event, in_isr);
        break;

    case DCD_EVENT_SOF:
        // nothing to do now
        break;

    case DCD_EVENT_SUSPEND:
        // NOTE: When plugging/unplugging device, the D+/D- state are unstable and can accidentally meet the
        // SUSPEND condition ( Idle for 3ms ). Some MCUs such as SAMD doesn't distinguish suspend vs disconnect as well.
        // We will skip handling SUSPEND/RESUME event if not currently connected
        if (_usbd_dev.connected) {
            _usbd_dev.suspended = 1;
            osal_queue_send(_usbd_q, event, in_isr);
        }
        break;

    case DCD_EVENT_RESUME:
        if (_usbd_dev.connected) {
            _usbd_dev.suspended = 0;
            osal_queue_send(_usbd_q, event, in_isr);
        }
        break;

    case DCD_EVENT_SETUP_RECEIVED:
        osal_queue_send(_usbd_q, event, in_isr);
        break;

    case DCD_EVENT_XFER_COMPLETE:
        // skip zero-length control status complete event, should DCD notify us.
        if ((0 == tu_edpt_number(event->xfer_complete.ep_addr)) && (event->xfer_complete.len == 0)) {
            break;
        }

        osal_queue_send(_usbd_q, event, in_isr);
        TU_ASSERT(event->xfer_complete.result == XFER_RESULT_SUCCESS, );
        break;

    // Not an DCD event, just a convenient way to defer ISR function should we need to
    case USBD_EVENT_FUNC_CALL:
        osal_queue_send(_usbd_q, event, in_isr);
        break;

    default:
        break;
    }
}

// helper to send bus signal event
void dcd_event_bus_signal(uint8_t rhport, dcd_eventid_t eid, bool in_isr)
{
    dcd_event_t event = {
        .rhport = rhport,
        .event_id = eid,
    };
    dcd_event_handler(&event, in_isr);
}

// helper to send setup received
void dcd_event_setup_received(uint8_t rhport, uint8_t const *setup, bool in_isr)
{
    dcd_event_t event = {.rhport = rhport, .event_id = DCD_EVENT_SETUP_RECEIVED};
    memcpy(&event.setup_received, setup, 8);

    dcd_event_handler(&event, in_isr);
}

// helper to send transfer complete event
void dcd_event_xfer_complete(uint8_t rhport, uint8_t ep_addr, uint32_t xferred_bytes, uint8_t result, bool in_isr)
{
    dcd_event_t event = {.rhport = rhport, .event_id = DCD_EVENT_XFER_COMPLETE};

    event.xfer_complete.ep_addr = ep_addr;
    event.xfer_complete.len = xferred_bytes;
    event.xfer_complete.result = result;

    dcd_event_handler(&event, in_isr);
}

//--------------------------------------------------------------------+
// Helper
//--------------------------------------------------------------------+

// Parse consecutive endpoint descriptors (IN & OUT)
bool usbd_open_edpt_pair(uint8_t rhport, uint8_t const *p_desc, uint8_t ep_count, uint8_t xfer_type, uint8_t *ep_out, uint8_t *ep_in)
{
    for (int i = 0; i < ep_count; i++) {
        tusb_desc_endpoint_t const *desc_ep = (tusb_desc_endpoint_t const *)p_desc;

        TU_VERIFY(TUSB_DESC_ENDPOINT == desc_ep->bDescriptorType && xfer_type == desc_ep->bmAttributes.xfer);
        TU_ASSERT(dcd_edpt_open(rhport, desc_ep));

        if (tu_edpt_dir(desc_ep->bEndpointAddress) == TUSB_DIR_IN) {
            (*ep_in) = desc_ep->bEndpointAddress;
        } else {
            (*ep_out) = desc_ep->bEndpointAddress;
        }

        p_desc = tu_desc_next(p_desc);
    }

    return true;
}

// Helper to defer an isr function
void usbd_defer_func(osal_task_func_t func, void *param, bool in_isr)
{
    dcd_event_t event = {
        .rhport = 0,
        .event_id = USBD_EVENT_FUNC_CALL,
    };

    event.func_call.func = func;
    event.func_call.param = param;

    dcd_event_handler(&event, in_isr);
}

//--------------------------------------------------------------------+
// USBD Endpoint API
//--------------------------------------------------------------------+

bool usbd_edpt_xfer(uint8_t rhport, uint8_t ep_addr, uint8_t *buffer, uint16_t total_bytes)
{
    uint8_t const epnum = tu_edpt_number(ep_addr);
    uint8_t const dir = tu_edpt_dir(ep_addr);

    TU_VERIFY(dcd_edpt_xfer(rhport, ep_addr, buffer, total_bytes));

    _usbd_dev.ep_busy_map[dir] = (uint8_t)tu_bit_set(_usbd_dev.ep_busy_map[dir], epnum);

    return true;
}

bool usbd_edpt_busy(uint8_t rhport, uint8_t ep_addr)
{
    (void)rhport;

    uint8_t const epnum = tu_edpt_number(ep_addr);
    uint8_t const dir = tu_edpt_dir(ep_addr);

    return tu_bit_test(_usbd_dev.ep_busy_map[dir], epnum);
}

void usbd_edpt_stall(uint8_t rhport, uint8_t ep_addr)
{
    uint8_t const epnum = tu_edpt_number(ep_addr);
    uint8_t const dir = tu_edpt_dir(ep_addr);

    dcd_edpt_stall(rhport, ep_addr);
    _usbd_dev.ep_stall_map[dir] = (uint8_t)tu_bit_set(_usbd_dev.ep_stall_map[dir], epnum);
}

void usbd_edpt_clear_stall(uint8_t rhport, uint8_t ep_addr)
{
    uint8_t const epnum = tu_edpt_number(ep_addr);
    uint8_t const dir = tu_edpt_dir(ep_addr);

    dcd_edpt_clear_stall(rhport, ep_addr);
    _usbd_dev.ep_stall_map[dir] = (uint8_t)tu_bit_clear(_usbd_dev.ep_stall_map[dir], epnum);
}

bool usbd_edpt_stalled(uint8_t rhport, uint8_t ep_addr)
{
    (void)rhport;

    uint8_t const epnum = tu_edpt_number(ep_addr);
    uint8_t const dir = tu_edpt_dir(ep_addr);

    return tu_bit_test(_usbd_dev.ep_stall_map[dir], epnum);
}

#endif
