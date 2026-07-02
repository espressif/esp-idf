/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * ESP-BLE-UART example. Backend (NimBLE / Bluedroid) is picked
 * by the host-stack Kconfig at compile time. Whatever the central
 * writes to the RX characteristic is echoed back over TX.
 */

#include <inttypes.h>
#include <stdio.h>

#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#include "ble_uart.h"

#if CONFIG_EXAMPLE_CUSTOM_ADV_DATA
/* Sample advertising payload demonstrating ble_uart_config_t::adv_data.
 * Replace these bytes with whatever your product needs (a different
 * Local Name, Manufacturer Specific Data, custom Service Data,
 * additional Service UUIDs, ...) — ble_uart broadcasts them verbatim.
 *
 * Format: a sequence of standard BT Core "AD structure" triplets,
 *         [length(1)] [AD type(1)] [value(length-1)].
 *
 * Length budget: total ≤ BLE_UART_ADV_DATA_MAX (28). The mandatory
 * 3-byte Flags AD is prepended by ble_uart and does NOT count against
 * this budget; oversize fails ble_uart_install() with EINVAL.
 *
 * The current contents (purely illustrative — edit freely):
 *
 *   Layout                                  bytes
 *   --------------------------------------  -----
 *   Complete Local Name AD   "BleUart"      1 + 1 + 7 = 9
 *   Complete 128-bit UUID AD                1 + 1 + 16 = 18
 *   --------------------------------------  -----
 *   total                                   27   (≤ 28)
 */
static const uint8_t example_adv_payload[] = {
    /* AD type 0x09: Complete Local Name */
    0x08, 0x09, 'B', 'l', 'e', 'U', 'a', 'r', 't',

    /* AD type 0x07: Complete List of 128-bit Service UUIDs.
     * UUID bytes are in over-the-air (little-endian) order, matching
     * ble_uart_service_uuid.bytes[]. */
    0x11, 0x07,
    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
    0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e,
};
#endif

static const char *TAG = "app";

static void ble_uart_on_rx(const uint8_t *data, size_t len)
{
    ESP_LOGI(TAG, "rx len: %u bytes", (unsigned)len);
    if (data == NULL || len == 0) {
        return;
    }
    ESP_LOG_BUFFER_HEX(TAG, data, len);
    ble_uart_tx(data, len);  /* echo back */
}

/* Lifecycle / link-state event sink. Runs on the BLE host task —
 * keep it short, never call ble_uart_close()/uninstall() from here.
 *
 * For production code: gate any sensitive TX on
 * BLE_UART_EVT_LINK_SECURE (encrypted+authenticated) instead of just
 * "connected"; ble_uart_is_connected() returns true while the link is
 * still plaintext during the pairing window. */
static void ble_uart_on_event(const ble_uart_evt_t *e)
{
    switch (e->id) {
    case BLE_UART_EVT_CONNECTED: {
        const uint8_t *b = e->connected.peer.bytes;
        ESP_LOGI(TAG,
                 "evt: connected peer=%02x:%02x:%02x:%02x:%02x:%02x type=%u",
                 b[0], b[1], b[2], b[3], b[4], b[5], e->connected.peer.type);
        break;
    }
    case BLE_UART_EVT_DISCONNECTED:
        ESP_LOGI(TAG, "evt: disconnected reason=0x%x",
                 e->disconnected.reason);
        break;
    case BLE_UART_EVT_SUBSCRIBED:
        ESP_LOGI(TAG, "evt: %ssubscribed",
                 e->subscribed.subscribed ? "" : "un");
        break;
    case BLE_UART_EVT_LINK_SECURE:
        ESP_LOGI(TAG, "evt: link_secure enc=%d auth=%d bond=%d ks=%u",
                 e->link_secure.encrypted, e->link_secure.authenticated,
                 e->link_secure.bonded, e->link_secure.key_size);
        break;
    case BLE_UART_EVT_PASSKEY_DISPLAY:
        ESP_LOGI(TAG, "evt: passkey=%06" PRIu32, e->passkey.passkey);
        break;
    case BLE_UART_EVT_PASSKEY_REQUEST:
        /* Fires only when cfg.security.io_cap is KEYBOARD_ONLY or
         * KEYBOARD_DISPLAY (this example leaves io_cap at AUTO →
         * DisplayOnly, so it should not fire). For a real keypad
         * product, prompt the user for the 6 digits the central
         * displayed and feed them in:
         *
         *     ble_uart_passkey_reply(digits);
         *
         * See PORTING.md §5.6.1 for the full pattern. */
        ESP_LOGW(TAG, "evt: passkey entry requested — no UI wired in this "
                      "example (see PORTING.md §5.6.1)");
        break;
    case BLE_UART_EVT_NUMERIC_COMPARE:
        /* Fires only when cfg.security.io_cap is DISPLAY_YES_NO or
         * KEYBOARD_DISPLAY (likewise dormant in this example). For a
         * product with a yes/no control, surface the digits to the
         * user and resolve the comparison:
         *
         *     ble_uart_compare_reply(user_says_match);
         *
         * See PORTING.md §5.6.1. */
        ESP_LOGW(TAG, "evt: numeric compare %06" PRIu32
                      " — no yes/no UI wired (see PORTING.md §5.6.1)",
                 e->numeric_compare.passkey);
        break;
    case BLE_UART_EVT_PAIRING_FAILED:
        ESP_LOGW(TAG, "evt: pairing failed reason=0x%x",
                 e->pairing_failed.reason);
        break;
    case BLE_UART_EVT_CLOSED:
        /* Only after ble_uart_close_async(). This example does not use
         * close_async; do not ble_uart_uninstall() here — defer to an
         * app task (PORTING.md §5.3.2). Kept for -Wswitch. */
        if (e->closed.status == BLE_UART_OK) {
            ESP_LOGI(TAG, "evt: closed (async-close succeeded)");
        } else {
            ESP_LOGW(TAG, "evt: closed async-close failed status=%d",
                     e->closed.status);
        }
        break;
    }
}

void app_main(void)
{
    /* NVS is required by the BT controller (PHY calibration) and the
     * bond store, so it must be live before ble_uart_install(). */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* Device name = "<prefix>-XXXX" with XXXX = last two MAC bytes.
     * If esp_read_mac() fails, mac stays zero and the suffix degrades
     * to "0000" — log so the operator notices uniqueness was lost. */
    uint8_t mac[6] = {0};
    esp_err_t mac_err = esp_read_mac(mac, ESP_MAC_BT);
    if (mac_err != ESP_OK) {
        ESP_LOGW(TAG, "esp_read_mac(BT) failed (%s); device name suffix will be 0000",
                 esp_err_to_name(mac_err));
    }
    char name[BLE_UART_DEVICE_NAME_MAX + 1];
    snprintf(name, sizeof(name), "%s-%02X%02X",
             CONFIG_BLE_UART_DEVICE_NAME_PREFIX, mac[4], mac[5]);

    ESP_ERROR_CHECK(ble_uart_install(&(ble_uart_config_t){
        .encrypted      = true,
        .device_name    = name,
#if CONFIG_EXAMPLE_CUSTOM_ADV_DATA
        /* Hand the application-defined bytes to ble_uart. Whatever
         * the array contains is broadcast verbatim; what `device_name`
         * (above) holds is exposed via the GAP service for connected
         * centrals to read — independent paths. */
        .adv_data       = example_adv_payload,
        .adv_data_len   = sizeof(example_adv_payload),
        /* scan_rsp_data is left at its default (NULL) → ble_uart still
         * sends its built-in scan response. Override it the same way
         * if you want to control those bytes too. */
#endif
        .ble_uart_on_rx = ble_uart_on_rx,
        .on_event       = ble_uart_on_event,
    }));

    /* Demonstrate the bond-management API: list every bonded peer
     * already on flash. Replace the log with `ble_uart_clear_bonds()`
     * to wipe them at boot (e.g. when a "factory reset" GPIO is held);
     * use `ble_uart_remove_peer(&list[i])` to target one specifically. */
    size_t total = 0;
    ble_uart_addr_t list[8];
    int rc = ble_uart_get_bonded_peers(list, sizeof(list) / sizeof(list[0]),
                                       &total);
    if (rc == 0) {
        ESP_LOGI(TAG, "%u peer(s) currently bonded", (unsigned)total);
        size_t shown = total < sizeof(list) / sizeof(list[0])
                       ? total : sizeof(list) / sizeof(list[0]);
        for (size_t i = 0; i < shown; i++) {
            const uint8_t *b = list[i].bytes;
            ESP_LOGI(TAG, "  [%u] %02x:%02x:%02x:%02x:%02x:%02x type=%u",
                     (unsigned)i,
                     b[0], b[1], b[2], b[3], b[4], b[5], list[i].type);
        }
        if (total > shown) {
            ESP_LOGI(TAG, "  (%u more not shown)",
                     (unsigned)(total - shown));
        }
    }

    ESP_ERROR_CHECK(ble_uart_open());
}
