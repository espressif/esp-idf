/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"

static const char *TAG = "l2cap_coc_prph";

#define L2CAP_COC_PSM       0x1002
#define L2CAP_COC_MTU       CONFIG_EXAMPLE_L2CAP_COC_MTU
#define COC_BUF_COUNT       (6 * MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM))
/* Block size must include mbuf headers so each SDU fits in one pool entry. */
#define SDU_BLOCK_SIZE      (L2CAP_COC_MTU + sizeof(struct os_mbuf_pkthdr) + sizeof(struct os_mbuf))
#define LL_PACKET_LENGTH    251
#define LL_PACKET_TIME      2120
#define L2CAP_COC_UUID      0x1812

static uint16_t              conn_handle = BLE_HS_CONN_HANDLE_NONE;
static struct ble_l2cap_chan *coc_chan    = NULL;
static uint8_t               own_addr_type;

static int64_t           phy_start_time = 0;
static volatile uint32_t rx_bytes       = 0;
static uint32_t          rx_packets     = 0;
static volatile bool     coc_active     = false;
static const char       *phy_name       = "1M";
static uint8_t           current_phy    = BLE_HCI_LE_PHY_1M;

void ble_store_config_init(void);

static os_membuf_t         sdu_coc_mem[OS_MEMPOOL_SIZE(COC_BUF_COUNT, SDU_BLOCK_SIZE)];
static struct os_mempool   sdu_coc_mempool;
static struct os_mbuf_pool sdu_os_mbuf_pool;

static int prph_gap_event(struct ble_gap_event *event, void *arg);

static const char *prph_phy_str(uint8_t phy)
{
    switch (phy) {
    case BLE_HCI_LE_PHY_2M:    return "2M";
    case BLE_HCI_LE_PHY_CODED: return "Coded";
    default:                   return "1M";
    }
}

static void prph_report_phy(int64_t end_time, int64_t start_time,
                             uint32_t bytes, uint32_t packets,
                             const char *phy_name)
{
    if (packets == 0 || start_time == 0) {
        return;
    }
    int64_t  elapsed_ms = (end_time - start_time) / 1000;
    if (elapsed_ms == 0) { elapsed_ms = 1; }
    uint32_t kbps       = (uint32_t)((uint64_t)bytes * 8ULL
                                     / (uint64_t)elapsed_ms);
    ESP_LOGI(TAG, "+-------------------------------------------------+");
    ESP_LOGI(TAG, "| PHY    : %-39s|", phy_name);
    ESP_LOGI(TAG, "| RX     : %-6" PRIu32 " kbps                            |", kbps);
    ESP_LOGI(TAG, "| Bytes  : %-10" PRIu32 "                               |", bytes);
    ESP_LOGI(TAG, "| Time   : %-5lld s                                |", elapsed_ms / 1000);
    ESP_LOGI(TAG, "+-------------------------------------------------+");
}

#if CONFIG_EXAMPLE_EXTENDED_ADV
static uint8_t ext_adv_pattern[] = {
    0x02, BLE_HS_ADV_TYPE_FLAGS, 0x06,
    0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16, 0x12, 0x18,
    0x11, BLE_HS_ADV_TYPE_COMP_NAME,
    'l','2','c','a','p','-','c','o','c','-','p','r','p','h','-','e',
};

static void prph_advertise(void)
{
    struct ble_gap_ext_adv_params params;
    struct os_mbuf *data;
    uint8_t instance = 0;
    int rc;

    memset(&params, 0, sizeof(params));
    params.connectable   = 1;
    params.own_addr_type = own_addr_type;
    params.primary_phy   = BLE_HCI_LE_PHY_1M;
    params.secondary_phy = BLE_HCI_LE_PHY_1M;
    params.tx_power      = 127;
    params.sid           = 1;
    params.itvl_min      = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    params.itvl_max      = BLE_GAP_ADV_FAST_INTERVAL1_MIN;

    rc = ble_gap_ext_adv_configure(instance, &params, NULL, prph_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "ext_adv_configure failed; rc=%d", rc);
        return;
    }

    data = os_msys_get_pkthdr(sizeof(ext_adv_pattern), 0);
    if (!data) {
        ESP_LOGE(TAG, "ext_adv: failed to alloc adv data mbuf");
        return;
    }
    rc = os_mbuf_append(data, ext_adv_pattern, sizeof(ext_adv_pattern));
    if (rc != 0) {
        ESP_LOGE(TAG, "ext_adv: mbuf_append failed; rc=%d", rc);
        os_mbuf_free_chain(data);
        return;
    }

    rc = ble_gap_ext_adv_set_data(instance, data);
    if (rc != 0) {
        ESP_LOGE(TAG, "ext_adv_set_data failed; rc=%d", rc);
        return;
    }

    rc = ble_gap_ext_adv_start(instance, 0, 0);
    if (rc != 0) {
        ESP_LOGE(TAG, "ext_adv_start failed; rc=%d", rc);
        return;
    }
    ESP_LOGI(TAG, "Extended advertising started");
}
#else
static void prph_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields  fields;
    int rc;

    memset(&fields, 0, sizeof(fields));
    fields.flags                 = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl            = BLE_HS_ADV_TX_PWR_LVL_AUTO;
#if CONFIG_BT_NIMBLE_GAP_SERVICE
    const char *name             = ble_svc_gap_device_name();
    fields.name                  = (uint8_t *)name;
    fields.name_len              = strlen(name);
    fields.name_is_complete      = 1;
#endif
    fields.uuids16               = (ble_uuid16_t[]){ BLE_UUID16_INIT(L2CAP_COC_UUID) };
    fields.num_uuids16           = 1;
    fields.uuids16_is_complete   = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error setting adv data; rc=%d", rc);
        return;
    }

    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, prph_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error starting adv; rc=%d", rc);
    }
}
#endif /* CONFIG_EXAMPLE_EXTENDED_ADV */

static void prph_l2cap_coc_mem_init(void)
{
    int rc;
    rc = os_mempool_init(&sdu_coc_mempool, COC_BUF_COUNT, SDU_BLOCK_SIZE,
                         sdu_coc_mem, "prph_coc_pool");
    assert(rc == 0);
    rc = os_mbuf_pool_init(&sdu_os_mbuf_pool, &sdu_coc_mempool, SDU_BLOCK_SIZE,
                           COC_BUF_COUNT);
    assert(rc == 0);
}

static int prph_l2cap_coc_accept(struct ble_l2cap_chan *chan)
{
    struct os_mbuf *sdu_rx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    if (!sdu_rx) {
        return BLE_HS_ENOMEM;
    }
    int rc = ble_l2cap_recv_ready(chan, sdu_rx);
    if (rc != 0) {
        os_mbuf_free_chain(sdu_rx);
    }
    return rc;
}

static int prph_l2cap_coc_event_cb(struct ble_l2cap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_L2CAP_EVENT_COC_CONNECTED:
        if (event->connect.status != 0) {
            ESP_LOGE(TAG, "L2CAP COC connect error: %d", event->connect.status);
            return 0;
        }
        ESP_LOGI(TAG, "L2CAP COC connected, chan=%p", event->connect.chan);
        coc_chan        = event->connect.chan;
        phy_start_time  = 0;      /* anchored on first data SDU, not connect */
        rx_bytes        = 0;
        rx_packets      = 0;
        coc_active      = true;
        phy_name = prph_phy_str(current_phy);
        return 0;

    case BLE_L2CAP_EVENT_COC_DISCONNECTED:
        coc_active  = false;
        coc_chan     = NULL;
        current_phy  = BLE_HCI_LE_PHY_1M;
        {
            int64_t  end  = esp_timer_get_time();
            int64_t  st   = phy_start_time;
            uint32_t by   = rx_bytes;
            uint32_t pk   = rx_packets;
            prph_report_phy(end, st, by, pk, phy_name);
            rx_bytes = 0; rx_packets = 0; phy_start_time = 0;
        }
        ESP_LOGI(TAG, "L2CAP COC disconnected");
        return 0;

    case BLE_L2CAP_EVENT_COC_ACCEPT: {
        /* Pre-grant 2 receive buffers so the central can pipeline 2 SDUs. */
        int rc = prph_l2cap_coc_accept(event->accept.chan);
        if (rc != 0) {
            return rc;
        }
        /* Second buffer is best-effort; one buffer is enough for the channel to operate. */
        if (prph_l2cap_coc_accept(event->accept.chan) != 0) {
            ESP_LOGW(TAG, "L2CAP COC accept: second RX buffer unavailable, running with one");
        }
        return 0;
    }

    case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
        if (event->receive.sdu_rx) {
            if (rx_packets == 0) {
                phy_start_time = esp_timer_get_time();
            }
            rx_bytes   += OS_MBUF_PKTLEN(event->receive.sdu_rx);
            rx_packets += 1;
            os_mbuf_free_chain(event->receive.sdu_rx);
        }
        if (prph_l2cap_coc_accept(event->receive.chan) != 0) {
            ESP_LOGE(TAG, "DATA_RECEIVED: no mbuf for recv_ready; RX may stall");
        }
        return 0;

    default:
        return 0;
    }
}

static void prph_stats_task(void *arg)
{
    uint32_t prev_bytes = 0;
    int64_t  prev_time  = 0;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        if (!coc_active) {
            prev_bytes = 0;
            prev_time  = 0;
            continue;
        }

        int64_t  now      = esp_timer_get_time();
        uint32_t bytes    = rx_bytes;

        if (prev_time > 0) {
            if (bytes < prev_bytes) {
                prev_bytes = bytes;
                prev_time  = now;
                continue;
            }
            int64_t  dt_us    = now - prev_time;
            uint32_t dt_bytes = bytes - prev_bytes;
            uint32_t kbps     = (uint32_t)((uint64_t)dt_bytes * 8ULL * 1000000ULL
                                           / (uint64_t)dt_us / 1000ULL);
            ESP_LOGI(TAG, "| RX : %-6" PRIu32 " kbps |", kbps);
        }

        prev_bytes = bytes;
        prev_time  = now;
    }
}

static int prph_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status != 0) {
            ESP_LOGE(TAG, "Connection failed; status=%d", event->connect.status);
            prph_advertise();
            return 0;
        }
        ESP_LOGI(TAG, "Connected; handle=%d", event->connect.conn_handle);
        conn_handle = event->connect.conn_handle;
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "Disconnected; reason=%d", event->disconnect.reason);
        conn_handle = BLE_HS_CONN_HANDLE_NONE;
        coc_chan     = NULL;
        coc_active   = false;
        current_phy  = BLE_HCI_LE_PHY_1M;
        phy_name     = "1M";
#if CONFIG_EXAMPLE_EXTENDED_ADV
        ble_gap_ext_adv_stop(0);
#endif
        prph_advertise();
        return 0;

    case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
        ESP_LOGI(TAG, "PHY updated: tx=%d rx=%d status=%d",
                 event->phy_updated.tx_phy,
                 event->phy_updated.rx_phy,
                 event->phy_updated.status);
        if (event->phy_updated.status == 0) {
            if (coc_active) {
                int64_t  end  = esp_timer_get_time();
                int64_t  st   = phy_start_time;
                uint32_t by   = rx_bytes;
                uint32_t pk   = rx_packets;
                prph_report_phy(end, st, by, pk, phy_name);
                rx_bytes = 0; rx_packets = 0; phy_start_time = 0;
            }
            current_phy = event->phy_updated.rx_phy;
            phy_name = prph_phy_str(event->phy_updated.rx_phy);
        }
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        ESP_LOGI(TAG, "Conn params updated; status=%d", event->conn_update.status);
        if (event->conn_update.status == 0 && coc_active &&
                current_phy == BLE_HCI_LE_PHY_CODED) {
            struct ble_gap_conn_desc desc;
            if (ble_gap_conn_find(conn_handle, &desc) == 0) {
                if (desc.conn_itvl != 16 && desc.conn_itvl != 32) {
                    return 0;
                }
                if (strcmp(phy_name, "Coded") != 0) {
                    int64_t  end = esp_timer_get_time();
                    uint32_t by  = rx_bytes;
                    uint32_t pk  = rx_packets;
                    prph_report_phy(end, phy_start_time, by, pk, phy_name);
                    rx_bytes = 0; rx_packets = 0; phy_start_time = 0;
                }
                phy_name = (desc.conn_itvl >= 32) ? "Coded S8" : "Coded S2";
                ESP_LOGI(TAG, "Coding scheme updated to %s (CI=%u × 1.25ms)",
                         phy_name, desc.conn_itvl);
            }
        }
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
#if !CONFIG_EXAMPLE_EXTENDED_ADV
        prph_advertise();
#endif
        return 0;

    default:
        return 0;
    }
}

static void prph_on_reset(int reason)
{
    ESP_LOGE(TAG, "Host reset; reason=%d", reason);
}

static void prph_on_sync(void)
{
    int rc;

    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    assert(rc == 0);

    rc = ble_l2cap_create_server(L2CAP_COC_PSM, L2CAP_COC_MTU,
                                  prph_l2cap_coc_event_cb, NULL);
    if (rc != 0 && rc != BLE_HS_EALREADY) {
        ESP_LOGE(TAG, "Failed to create L2CAP COC server; rc=%d", rc);
        return;
    }

    uint8_t addr[6] = {0};
    ble_hs_id_copy_addr(own_addr_type, addr, NULL);
    ESP_LOGI(TAG, "Device Address: %02x:%02x:%02x:%02x:%02x:%02x",
             addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);

    prph_advertise();
}

static void prph_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nimble_port_init failed; rc=%d", ret);
        return;
    }

    prph_l2cap_coc_mem_init();

    ble_hs_cfg.reset_cb        = prph_on_reset;
    ble_hs_cfg.sync_cb         = prph_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

#if CONFIG_BT_NIMBLE_GAP_SERVICE
    int rc = ble_svc_gap_device_name_set("l2cap-coc-prph");
    assert(rc == 0);
#endif

    ble_store_config_init();

    if (xTaskCreate(prph_stats_task, "prph_stats", 4096, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create stats task");
    }

    nimble_port_freertos_init(prph_host_task);
}
