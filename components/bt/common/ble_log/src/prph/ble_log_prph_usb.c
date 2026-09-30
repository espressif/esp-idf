/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* -------------------------------------------------- */
/* BLE Log - Peripheral Interface - TinyUSB CDC-ACM   */
/* -------------------------------------------------- */

/* INCLUDE */
#include "ble_log_prph.h"
#include "ble_log_lbm_v2.h"
#include "ble_log_usb_tx.h"

#include "sdkconfig.h"
#include "tinyusb.h"
#include "tinyusb_cdc_acm.h"
#include "tinyusb_default_config.h"
#include "tusb.h"

/* MACRO */
_Static_assert(CONFIG_BLE_LOG_POOL_TRANS_SIZE <= CONFIG_BLE_LOG_USB_CDC_TX_BUFSIZE,
               "CDC TX FIFO must hold one BLE Log transport");
_Static_assert(CONFIG_BLE_LOG_USB_CDC_EP_BUFSIZE >= 64,
               "Bulk MPS is 64 (Full-Speed) or 512 (High-Speed)");

/* VARIABLE */
BLE_LOG_STATIC BLE_LOG_DRAM_ATTR bool s_inited = false;
BLE_LOG_STATIC uint32_t s_fifo_full_drops;
BLE_LOG_STATIC uint32_t s_fifo_full_dropped_bytes;

BLE_LOG_STATIC const tusb_desc_device_t s_dev_desc = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = BLE_LOG_USB_VID,
    .idProduct = BLE_LOG_USB_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01,
};

BLE_LOG_STATIC const char *s_str_desc[] = {
    (const char[]) { 0x09, 0x04 },
    BLE_LOG_USB_MANUFACTURER,
    BLE_LOG_USB_PRODUCT,
    CONFIG_TINYUSB_DESC_SERIAL_STRING,
    BLE_LOG_USB_PRODUCT,
};

/* PRIVATE FUNCTION */
BLE_LOG_STATIC void cdc_rx_discard(int itf, cdcacm_event_t *event)
{
    uint8_t sink[64];
    size_t n = 0;

    (void)event;
    while (tinyusb_cdcacm_read(itf, sink, sizeof(sink), &n) == ESP_OK && n > 0) {
    }
}

void ble_log_prph_take_cdc_fifo_drops(uint32_t *drops, uint32_t *bytes)
{
    /* The dispatch task owns the increments while the ESP Timer task (window
     * warning) and the deinit tail read them, so both fields are exchanged to
     * zero atomically: a load-then-clear pair would drop an increment landing
     * between the two, which is a real loss even under sequentially
     * consistent execution. The two fields are taken independently; the
     * warning contract does not require a transport/byte pair from the same
     * window. A byte-only tail still reports: the two exchanges do not
     * pair, so a producer can land a byte increment after the reporter
     * took the count; that increment would otherwise be silently cleared
     * by the next take. */
    *drops = __atomic_exchange_n(&s_fifo_full_drops, 0, __ATOMIC_ACQ_REL);
    *bytes = __atomic_exchange_n(&s_fifo_full_dropped_bytes, 0,
                                 __ATOMIC_ACQ_REL);
}

/* INTERFACE */
bool ble_log_prph_init(size_t trans_cnt)
{
    (void)trans_cnt;
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(s_inited)) {
        return true;
    }

    tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
    tusb_cfg.descriptor.device = &s_dev_desc;
    tusb_cfg.descriptor.string = s_str_desc;
    tusb_cfg.descriptor.string_count =
        (int)(sizeof(s_str_desc) / sizeof(s_str_desc[0]));
    if (tinyusb_driver_install(&tusb_cfg) != ESP_OK) {
        return false;
    }

    const tinyusb_config_cdcacm_t acm_cfg = {
        .cdc_port = TINYUSB_CDC_ACM_0,
        .callback_rx = &cdc_rx_discard,
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL,
    };
    if (tinyusb_cdcacm_init(&acm_cfg) != ESP_OK) {
        (void)tinyusb_driver_uninstall();
        return false;
    }

    BLE_LOG_ATOMIC_STORE_RELEASE(s_inited, true);
    return true;
}

void ble_log_prph_deinit(void)
{
    if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(s_inited)) {
        return;
    }
    BLE_LOG_ATOMIC_STORE_RELEASE(s_inited, false);
    BLE_LOG_ATOMIC_STORE_RELEASE(s_fifo_full_drops, 0);
    BLE_LOG_ATOMIC_STORE_RELEASE(s_fifo_full_dropped_bytes, 0);
    /* Stop the USB event/callback execution before the CDC object is freed.
     * tinyusb_cdcacm_deinit() frees the ACM object that the TinyUSB task's
     * tud_cdc_* callbacks dereference (get_acm() keeps returning the stale,
     * non-NULL pointer until the class deinit clears the slot), while the
     * dependency stops that task only in tinyusb_driver_uninstall(). This
     * ordering keeps the driver's own examples too. Residual window:
     * tinyusb_task_stop() uses vTaskDelete, which does not wait for a
     * callback already running on the other core. */
    (void)tinyusb_driver_uninstall();
    (void)tinyusb_cdcacm_deinit(TINYUSB_CDC_ACM_0);
}

bool ble_log_prph_trans_init(ble_log_prph_trans_t **trans, size_t trans_size)
{
    if (!trans || !trans_size) {
        return false;
    }

    *trans = (ble_log_prph_trans_t *)BLE_LOG_MALLOC(sizeof(ble_log_prph_trans_t));
    if (!(*trans)) {
        return false;
    }
    BLE_LOG_MEMSET(*trans, 0, sizeof(ble_log_prph_trans_t));
    (*trans)->size = trans_size;

    (*trans)->buf = (uint8_t *)BLE_LOG_MALLOC(trans_size);
    if (!(*trans)->buf) {
        BLE_LOG_FREE(*trans);
        *trans = NULL;
        return false;
    }
    BLE_LOG_MEMSET((*trans)->buf, 0, trans_size);
    return true;
}

void ble_log_prph_trans_deinit(ble_log_prph_trans_t **trans)
{
    if (!trans || !(*trans)) {
        return;
    }
    if ((*trans)->buf) {
        BLE_LOG_FREE((*trans)->buf);
    }
    BLE_LOG_FREE(*trans);
    *trans = NULL;
}

void ble_log_prph_send_trans(ble_log_prph_trans_t *trans)
{
    if (!trans) {
        return;
    }

    const size_t nbytes = trans->pos;
    const bool connected = tud_cdc_n_connected(TINYUSB_CDC_ACM_0);
    const size_t available = connected ?
                             tud_cdc_n_write_available(TINYUSB_CDC_ACM_0) : 0;
    const ble_log_usb_tx_action_t action =
        ble_log_usb_tx_action(connected, available, nbytes);

    if (ble_log_usb_tx_should_warn(action)) {
        BLE_LOG_ATOMIC_ADD_RELAXED(s_fifo_full_drops, 1);
        BLE_LOG_ATOMIC_ADD_RELAXED(s_fifo_full_dropped_bytes, (uint32_t)nbytes);
    } else if (action == BLE_LOG_USB_TX_WRITE && nbytes > 0) {
        /* Single-writer invariant: this path is the only producer of the
         * CDC TX FIFO (this build wires no CDC console or VFS writes into
         * it), and the TinyUSB device task only drains the FIFO, so
         * `available` cannot shrink between the check above and the write
         * below. A concurrent producer would break the whole-transport
         * contract here; the queued != nbytes backstop below still turns
         * any partial write into a counted drop instead of silently
         * tearing a frame on the wire. */
        const size_t queued = tinyusb_cdcacm_write_queue(
                                  TINYUSB_CDC_ACM_0, trans->buf, nbytes);
        if (queued != nbytes) {
            BLE_LOG_ATOMIC_ADD_RELAXED(s_fifo_full_drops, 1);
            BLE_LOG_ATOMIC_ADD_RELAXED(s_fifo_full_dropped_bytes,
                                       (uint32_t)(nbytes - queued));
        } else {
            (void)tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, 0);
        }
    }

    ble_log_lbm_recycle_trans(trans);
}
