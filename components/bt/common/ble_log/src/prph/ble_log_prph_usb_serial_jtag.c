/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ble_log_prph.h"
#include "ble_log_lbm_v2.h"
#include "ble_log_usb_tx.h"

#include "sdkconfig.h"
#include "esp_rom_caps.h"
#include "esp_rom_sys.h"
#include "driver/usb_serial_jtag.h"
#if !CONFIG_FREERTOS_UNICORE && defined(CONFIG_ESP_IPC_ENABLE)
#include "esp_ipc.h"
#endif

#define BLE_LOG_USJ_RX_BUFSIZE   128
#define BLE_LOG_USJ_WRITE_TICKS  0

_Static_assert(CONFIG_BLE_LOG_POOL_TRANS_SIZE <= CONFIG_BLE_LOG_USJ_TX_BUFSIZE,
               "USJ TX ring must hold one BLE Log transport");
_Static_assert(BLE_LOG_USJ_RX_BUFSIZE > 64,
               "usb_serial_jtag_driver_install rejects rx_buffer_size <= 64");

BLE_LOG_STATIC BLE_LOG_DRAM_ATTR bool s_inited = false;
BLE_LOG_STATIC uint32_t s_tx_ring_drops;
BLE_LOG_STATIC uint32_t s_tx_ring_dropped_bytes;
#if !CONFIG_FREERTOS_UNICORE && defined(CONFIG_ESP_IPC_ENABLE)
BLE_LOG_STATIC BaseType_t s_install_core;
#endif

void ble_log_prph_take_tx_ring_drops(uint32_t *drops, uint32_t *bytes)
{
    *drops = __atomic_exchange_n(&s_tx_ring_drops, 0, __ATOMIC_ACQ_REL);
    *bytes = __atomic_exchange_n(&s_tx_ring_dropped_bytes, 0,
                                 __ATOMIC_ACQ_REL);
}

#if !CONFIG_FREERTOS_UNICORE && defined(CONFIG_ESP_IPC_ENABLE)
BLE_LOG_STATIC void ble_log_usj_uninstall_cb(void *arg)
{
    (void)arg;
    (void)usb_serial_jtag_driver_uninstall();
}
#endif

bool ble_log_prph_init(size_t trans_cnt)
{
    (void)trans_cnt;
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(s_inited)) {
        return true;
    }

    usb_serial_jtag_driver_config_t cfg = {
        .tx_buffer_size = CONFIG_BLE_LOG_USJ_TX_BUFSIZE,
        .rx_buffer_size = BLE_LOG_USJ_RX_BUFSIZE,
        .intr_priority = 0,
    };
    if (usb_serial_jtag_driver_install(&cfg) != ESP_OK) {
        return false;
    }
    /* C3/S3 ROM console still dual-writes to USJ after SECONDARY_NONE
     * because cpu_start forces g_usb_print. Channel 2 is the ROM
     * secondary USB putc. Mute both once this driver owns the FIFO.
     * Not restored on deinit: SECONDARY_NONE should stay silent. */
#if ESP_ROM_HAS_ETS_PRINTF_BUG
    extern bool g_usb_print;
    g_usb_print = false;
#endif
    esp_rom_install_channel_putc(2, NULL);
#if !CONFIG_FREERTOS_UNICORE && defined(CONFIG_ESP_IPC_ENABLE)
    s_install_core = xPortGetCoreID();
#endif
    BLE_LOG_ATOMIC_STORE_RELEASE(s_inited, true);
    return true;
}

void ble_log_prph_deinit(void)
{
    if (!BLE_LOG_ATOMIC_LOAD_ACQUIRE(s_inited)) {
        return;
    }
    BLE_LOG_ATOMIC_STORE_RELEASE(s_inited, false);
    BLE_LOG_ATOMIC_STORE_RELEASE(s_tx_ring_drops, 0);
    BLE_LOG_ATOMIC_STORE_RELEASE(s_tx_ring_dropped_bytes, 0);
    /* Residual TX ring bytes are dropped. COPY_FIFO deinit does not call
     * wait_tx_done. SOF with no host reader would hang. */
#if !CONFIG_FREERTOS_UNICORE && defined(CONFIG_ESP_IPC_ENABLE)
    if (xPortGetCoreID() != s_install_core) {
        (void)esp_ipc_call_blocking((uint32_t)s_install_core,
                                    ble_log_usj_uninstall_cb, NULL);
        return;
    }
#endif
    (void)usb_serial_jtag_driver_uninstall();
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
    const bool connected = usb_serial_jtag_is_connected();
    size_t available = 0;
    int queued = 0;

    if (connected && nbytes > 0) {
        queued = usb_serial_jtag_write_bytes(trans->buf, nbytes,
                                             BLE_LOG_USJ_WRITE_TICKS);
        if (queued == (int)nbytes) {
            available = nbytes;
        }
    }

    const ble_log_usb_tx_action_t action =
        ble_log_usb_tx_action(connected, available, nbytes);

    if (ble_log_usb_tx_should_warn(action)) {
        BLE_LOG_ATOMIC_ADD_RELAXED(s_tx_ring_drops, 1);
        BLE_LOG_ATOMIC_ADD_RELAXED(s_tx_ring_dropped_bytes, (uint32_t)nbytes);
    }

    ble_log_lbm_recycle_trans(trans);
}
