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
#include "freertos/event_groups.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "host/ble_esp_gap.h"

static const char *TAG = "l2cap_coc_cent";

#define L2CAP_COC_PSM       0x1002
#define L2CAP_COC_MTU       CONFIG_EXAMPLE_L2CAP_COC_MTU
#define COC_BUF_COUNT       (6 * MYNEWT_VAL(BLE_L2CAP_COC_MAX_NUM))
/* Block size must include mbuf headers so each SDU fits in one pool entry. */
#define SDU_BLOCK_SIZE      (L2CAP_COC_MTU + sizeof(struct os_mbuf_pkthdr) + sizeof(struct os_mbuf))
#define LL_PACKET_LENGTH    251
#define LL_PACKET_TIME      2120
#define L2CAP_COC_UUID      0x1812

/* EventGroup bits */
#define PHY_UPDATED_BIT     (1 << 0)
#define COC_CONNECTED_BIT   (1 << 1)
#define CONN_UPDATED_BIT    (1 << 2)
#define TX_UNSTALLED_BIT    (1 << 3)

/* Timeout / interval constants */
#define CONN_PARAM_UPDATE_TIMEOUT_MS  5000
#define PREDRAIN_TIMEOUT_MS          20000
#define POSTDRAIN_TIMEOUT_MS          5000
#define TX_YIELD_INTERVAL               50

static EventGroupHandle_t    coc_event_group;
static uint16_t              conn_handle  = BLE_HS_CONN_HANDLE_NONE;
static struct ble_l2cap_chan *coc_chan     = NULL;
static bool                  ci_is_slow          = false;
static bool                  l2cap_connecting    = false;  /* guards double L2CAP connect */
static volatile bool         chan_stalled         = false;
static uint32_t             *cent_seg_tx_done = NULL;  /* points to segment SDU counter for async TX_UNSTALLED */
static uint32_t             *cent_seg_tx_drop = NULL;  /* counts SDUs dropped (TX_UNSTALLED status != 0) */
static uint16_t              cent_tx_sdu_len    = L2CAP_COC_MTU; /* min(local, peer) after COC connect */

static const struct ble_gap_upd_params conn_params = {
    .itvl_min            = 6,
    .itvl_max            = 6,
    .latency             = 0,
    .supervision_timeout = 2000,
    .min_ce_len          = 12,
    .max_ce_len          = 24,
};

void ble_store_config_init(void);

static os_membuf_t        sdu_coc_mem[OS_MEMPOOL_SIZE(COC_BUF_COUNT, SDU_BLOCK_SIZE)];
static struct os_mempool  sdu_coc_mempool;
static struct os_mbuf_pool sdu_os_mbuf_pool;

typedef struct {
    uint8_t     tx_phys;
    uint8_t     rx_phys;
    uint8_t     phy_opts;   /* 0=none, 1=S2, 2=S8 */
    int         duration_s;
    const char *name;
    bool        is_coded_s8;
} phy_entry_t;

static const phy_entry_t phy_list[] = {
#if CONFIG_EXAMPLE_TEST_PHY_1M
    { BLE_HCI_LE_PHY_1M_PREF_MASK, BLE_HCI_LE_PHY_1M_PREF_MASK, 0,
      CONFIG_EXAMPLE_TEST_DURATION_1M, "1M", false },
#endif
#if CONFIG_EXAMPLE_TEST_PHY_2M
    { BLE_HCI_LE_PHY_2M_PREF_MASK, BLE_HCI_LE_PHY_2M_PREF_MASK, 0,
      CONFIG_EXAMPLE_TEST_DURATION_2M, "2M", false },
#endif
#if CONFIG_EXAMPLE_TEST_PHY_CODED_S2
    { BLE_HCI_LE_PHY_CODED_PREF_MASK, BLE_HCI_LE_PHY_CODED_PREF_MASK, 0x01,
      CONFIG_EXAMPLE_TEST_DURATION_CODED_S2, "Coded S2", false },
#endif
#if CONFIG_EXAMPLE_TEST_PHY_CODED_S8
    { BLE_HCI_LE_PHY_CODED_PREF_MASK, BLE_HCI_LE_PHY_CODED_PREF_MASK, 0x02,
      CONFIG_EXAMPLE_TEST_DURATION_CODED_S8, "Coded S8", true },
#endif
};
#define PHY_LIST_LEN  ((int)(sizeof(phy_list) / sizeof(phy_list[0])))

static int cent_gap_event(struct ble_gap_event *event, void *arg);
static int cent_l2cap_coc_event_cb(struct ble_l2cap_event *event, void *arg);

static void cent_l2cap_coc_mem_init(void)
{
    int rc;
    rc = os_mempool_init(&sdu_coc_mempool, COC_BUF_COUNT, SDU_BLOCK_SIZE,
                         sdu_coc_mem, "cent_coc_pool");
    assert(rc == 0);
    rc = os_mbuf_pool_init(&sdu_os_mbuf_pool, &sdu_coc_mempool,
                           SDU_BLOCK_SIZE, COC_BUF_COUNT);
    assert(rc == 0);
}

static void cent_l2cap_coc_connect(uint16_t conn_handle)
{
    struct ble_gap_conn_desc desc;
    if (ble_gap_conn_find(conn_handle, &desc) != 0) {
        ESP_LOGE(TAG, "L2CAP COC connect: connection %d not found", conn_handle);
        l2cap_connecting = false;
        return;
    }
    struct os_mbuf *sdu_rx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
    if (!sdu_rx) {
        ESP_LOGE(TAG, "Failed to alloc sdu_rx for L2CAP connect");
        l2cap_connecting = false;
        return;
    }
    int rc = ble_l2cap_connect(conn_handle, L2CAP_COC_PSM, L2CAP_COC_MTU,
                               sdu_rx, cent_l2cap_coc_event_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "L2CAP COC connect failed; rc=%d", rc);
        l2cap_connecting = false;
        /* EINVAL: NimBLE returns before chan alloc, sdu_rx not consumed — free it.
         * ENOTCONN: NimBLE frees sdu_rx on all ENOTCONN paths (early !conn check
         * and late TX failure via ble_l2cap_coc_cleanup_chan). Do not free here. */
        if (rc == BLE_HS_EINVAL) {
            os_mbuf_free_chain(sdu_rx);
        }
    }
}

static void cent_scan(void)
{
    struct ble_gap_disc_params disc_params = {
        .filter_duplicates = 1,
        .passive           = 1,
    };
    uint8_t own_addr_type;
    int rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error inferring addr type; rc=%d", rc);
        return;
    }
    rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                      cent_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error starting scan; rc=%d", rc);
    }
}

static int cent_should_connect(const struct ble_gap_disc_desc *disc)
{
    struct ble_hs_adv_fields fields;
    if (disc->event_type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND &&
            disc->event_type != BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {
        return 0;
    }
    int rc = ble_hs_adv_parse_fields(&fields, disc->data, disc->length_data);
    if (rc != 0) {
        return 0;
    }
    for (int i = 0; i < fields.num_uuids16; i++) {
        if (ble_uuid_u16(&fields.uuids16[i].u) == L2CAP_COC_UUID) {
            return 1;
        }
    }
    return 0;
}

static void cent_connect_if_interesting(const struct ble_gap_disc_desc *disc)
{
    if (!cent_should_connect(disc)) {
        return;
    }
    int rc = ble_gap_disc_cancel();
    if (rc != 0) {
        return;
    }
    uint8_t own_addr_type;
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error inferring addr type; rc=%d", rc);
        cent_scan();
        return;
    }
    ESP_LOGI(TAG, "Connecting to %02x:%02x:%02x:%02x:%02x:%02x (addr_type=%d)",
             disc->addr.val[5], disc->addr.val[4], disc->addr.val[3],
             disc->addr.val[2], disc->addr.val[1], disc->addr.val[0],
             disc->addr.type);
    rc = ble_gap_connect(own_addr_type, &disc->addr, 30000, NULL,
                         cent_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Connect failed; rc=%d", rc);
        cent_scan();
    }
}

#if CONFIG_EXAMPLE_EXTENDED_ADV
static void cent_connect_if_interesting_ext(const struct ble_gap_ext_disc_desc *disc)
{
    if (!(disc->props & BLE_HCI_ADV_CONN_MASK)) {
        return;
    }
    struct ble_hs_adv_fields fields;
    if (ble_hs_adv_parse_fields(&fields, disc->data, disc->length_data) != 0) {
        return;
    }
    int found = 0;
    for (int i = 0; i < fields.num_uuids16; i++) {
        if (ble_uuid_u16(&fields.uuids16[i].u) == L2CAP_COC_UUID) {
            found = 1;
            break;
        }
    }
    if (!found) {
        return;
    }
    int rc = ble_gap_disc_cancel();
    if (rc != 0) {
        return;
    }
    uint8_t own_addr_type;
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error inferring addr type; rc=%d", rc);
        cent_scan();
        return;
    }
    ESP_LOGI(TAG, "Connecting to %02x:%02x:%02x:%02x:%02x:%02x (addr_type=%d)",
             disc->addr.val[5], disc->addr.val[4], disc->addr.val[3],
             disc->addr.val[2], disc->addr.val[1], disc->addr.val[0],
             disc->addr.type);
    rc = ble_gap_connect(own_addr_type, &disc->addr, 30000, NULL,
                         cent_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Connect failed; rc=%d", rc);
        cent_scan();
    }
}
#endif /* CONFIG_EXAMPLE_EXTENDED_ADV */

static int cent_l2cap_coc_event_cb(struct ble_l2cap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_L2CAP_EVENT_COC_CONNECTED: {
        struct ble_l2cap_chan_info info;
        uint16_t peer_mtu;

        if (event->connect.status != 0) {
            ESP_LOGE(TAG, "L2CAP COC connect status: %d,terminating GAP to restart", event->connect.status);
            l2cap_connecting = false;
            ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
            return 0;
        }
        ESP_LOGI(TAG, "L2CAP COC connected, chan=%p", event->connect.chan);
        coc_chan = event->connect.chan;

        peer_mtu = 0;
        cent_tx_sdu_len = L2CAP_COC_MTU;
        if (ble_l2cap_get_chan_info(coc_chan, &info) == 0) {
            peer_mtu = info.peer_coc_mtu;
            if (peer_mtu > 0 && peer_mtu < cent_tx_sdu_len) {
                cent_tx_sdu_len = peer_mtu;
            }
        }
        ESP_LOGI(TAG, "TX SDU size: %u bytes (peer CoC MTU %u)",
                 cent_tx_sdu_len, peer_mtu);
        l2cap_connecting = false;
        xEventGroupSetBits(coc_event_group, COC_CONNECTED_BIT);
        return 0;
    }

    case BLE_L2CAP_EVENT_COC_DISCONNECTED:
        ESP_LOGI(TAG, "L2CAP COC disconnected");
        coc_chan            = NULL;
        chan_stalled        = false;
        cent_seg_tx_done    = NULL;
        cent_seg_tx_drop    = NULL;
        l2cap_connecting    = false;
        xEventGroupClearBits(coc_event_group, COC_CONNECTED_BIT);
        xEventGroupSetBits(coc_event_group, TX_UNSTALLED_BIT | CONN_UPDATED_BIT | PHY_UPDATED_BIT);
        return 0;

    case BLE_L2CAP_EVENT_COC_TX_UNSTALLED:
        /* status==0: SDU delivered; status!=0: NimBLE dropped it (ENOMEM) — don't count. */
        chan_stalled = false;
        if (event->tx_unstalled.status == 0) {
            if (cent_seg_tx_done) {
                (*cent_seg_tx_done)++;
            }
        } else {
            if (cent_seg_tx_drop) {
                (*cent_seg_tx_drop)++;
            }
            ESP_LOGD(TAG, "TX_UNSTALLED status=%d: SDU dropped", event->tx_unstalled.status);
        }
        xEventGroupSetBits(coc_event_group, TX_UNSTALLED_BIT);
        return 0;

    case BLE_L2CAP_EVENT_COC_DATA_RECEIVED: {
        struct os_mbuf *sdu_rx;
        int rc;

        if (event->receive.sdu_rx) {
            os_mbuf_free_chain(event->receive.sdu_rx);
        }
        sdu_rx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
        if (sdu_rx) {
            rc = ble_l2cap_recv_ready(event->receive.chan, sdu_rx);
            if (rc != 0) {
                os_mbuf_free_chain(sdu_rx);
            }
        } else {
            ESP_LOGE(TAG, "DATA_RECEIVED: no mbuf for recv_ready; RX may stall");
        }
        return 0;
    }

    default:
        return 0;
    }
}

static void wait_unstall(uint32_t timeout_ms)
{
    xEventGroupWaitBits(coc_event_group, TX_UNSTALLED_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(timeout_ms));
}

static void cent_send_task(void *arg)
{
    static uint8_t value[L2CAP_COC_MTU];
    int rc;

    for (int i = 0; i < L2CAP_COC_MTU; i++) {
        value[i] = i & 0xFF;
    }

    xEventGroupWaitBits(coc_event_group, COC_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    ESP_LOGI(TAG, "L2CAP COC Throughput — TX side (central sends to peripheral)");
    ESP_LOGI(TAG, "Number of enabled PHYs: %d", (int)PHY_LIST_LEN);

    if (PHY_LIST_LEN == 0) {
        ESP_LOGE(TAG, "No test PHY enabled; enable at least one EXAMPLE_TEST_PHY_* in menuconfig");
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        bool lost_connection = false;

        for (int i = 0; i < PHY_LIST_LEN && !lost_connection; i++) {
            const phy_entry_t *phy = &phy_list[i];

            xEventGroupClearBits(coc_event_group, PHY_UPDATED_BIT);
#if CONFIG_SOC_BLE_50_SUPPORTED
            rc = ble_gap_set_prefered_le_phy(conn_handle,
                                              phy->tx_phys,
                                              phy->rx_phys,
                                              phy->phy_opts);
            if (rc != 0) {
                ESP_LOGE(TAG, "PHY switch to %s failed; rc=%d — continuing anyway",
                         phy->name, rc);
                xEventGroupSetBits(coc_event_group, PHY_UPDATED_BIT);
            }
#else
            xEventGroupSetBits(coc_event_group, PHY_UPDATED_BIT);
#endif

            EventBits_t bits = xEventGroupWaitBits(coc_event_group, PHY_UPDATED_BIT,
                                                    pdTRUE, pdTRUE,
                                                    pdMS_TO_TICKS(5000));
            if (!(bits & PHY_UPDATED_BIT)) {
                ESP_LOGW(TAG, "PHY update timeout for %s; continuing anyway", phy->name);
            }

            if (phy->is_coded_s8) {
                /* CI=40ms, CE=32.5-40ms: fits 2 Coded S8 K-frames per CI and prevents credit starvation */
                struct ble_gap_upd_params s8_params = {
                    .itvl_min            = 32,
                    .itvl_max            = 32,
                    .latency             = 0,
                    .supervision_timeout = 2000,
                    .min_ce_len          = 52,
                    .max_ce_len          = 64,
                };
                xEventGroupClearBits(coc_event_group, CONN_UPDATED_BIT);
                rc = ble_gap_update_params(conn_handle, &s8_params);
                if (rc == 0) {
                    ESP_LOGI(TAG, "Coded S8: updating CI");
                    ci_is_slow = true;
                    xEventGroupWaitBits(coc_event_group, CONN_UPDATED_BIT, pdTRUE, pdTRUE,
                                        pdMS_TO_TICKS(CONN_PARAM_UPDATE_TIMEOUT_MS));
                } else {
                    ESP_LOGW(TAG, "S8 CI update failed (rc=%d)", rc);
                }
            } else if (phy->tx_phys == BLE_HCI_LE_PHY_CODED_PREF_MASK) {
                /* CI=20ms, CE=10-20ms: fits 2 Coded S2 K-frames per CI */
                struct ble_gap_upd_params s2_params = {
                    .itvl_min            = 16,
                    .itvl_max            = 16,
                    .latency             = 0,
                    .supervision_timeout = 2000,
                    .min_ce_len          = 16,
                    .max_ce_len          = 32,
                };
                xEventGroupClearBits(coc_event_group, CONN_UPDATED_BIT);
                rc = ble_gap_update_params(conn_handle, &s2_params);
                if (rc == 0) {
                    ESP_LOGI(TAG, "Coded S2: updating CI");
                    ci_is_slow = true;
                    xEventGroupWaitBits(coc_event_group, CONN_UPDATED_BIT, pdTRUE, pdTRUE,
                                        pdMS_TO_TICKS(CONN_PARAM_UPDATE_TIMEOUT_MS));
                } else {
                    ESP_LOGW(TAG, "S2 CI update failed (rc=%d)", rc);
                }
            } else if (ci_is_slow) {
                ci_is_slow = false;
                xEventGroupClearBits(coc_event_group, CONN_UPDATED_BIT);
                rc = ble_gap_update_params(conn_handle, &conn_params);
                if (rc == 0) {
                    ESP_LOGI(TAG, "%s: restoring CI to 6 ms", phy->name);
                    xEventGroupWaitBits(coc_event_group, CONN_UPDATED_BIT, pdTRUE, pdTRUE,
                                        pdMS_TO_TICKS(CONN_PARAM_UPDATE_TIMEOUT_MS));
                } else {
                    ESP_LOGW(TAG, "CI restore failed (rc=%d)", rc);
                }
            }

            if (chan_stalled) {
                ESP_LOGI(TAG, "Pre-drain: waiting for unstall");
                wait_unstall(PREDRAIN_TIMEOUT_MS);
                if (chan_stalled) {
                    ESP_LOGW(TAG, "Pre-drain timed out; forcing clear");
                    chan_stalled = false;
                }
            }

            int64_t start_us   = esp_timer_get_time();
            int64_t end_us     = start_us + (int64_t)phy->duration_s * 1000000LL;
            /* Both the send task and TX_UNSTALLED callback update these counters; a
             * lost update is possible on dual-core but harmless for throughput stats. */

            uint32_t segment_sdus  = 0;
            uint32_t segment_drops = 0;

            cent_seg_tx_done = &segment_sdus;
            cent_seg_tx_drop = &segment_drops;

            xEventGroupClearBits(coc_event_group, TX_UNSTALLED_BIT);  /* clear stale signal from previous segment */

            ESP_LOGI(TAG, "[%s PHY] Sending for %d s", phy->name, phy->duration_s);

            while (!lost_connection && esp_timer_get_time() < end_us) {
                /* Snapshot coc_chan — NimBLE host task can NULL it between check and send. */
                struct ble_l2cap_chan *chan = coc_chan;
                if (!chan) {
                    ESP_LOGW(TAG, "COC channel lost during test");
                    cent_seg_tx_done     = NULL;
                    cent_seg_tx_drop     = NULL;
                    lost_connection = true;
                    break;
                }

                struct os_mbuf *sdu_tx = os_mbuf_get_pkthdr(&sdu_os_mbuf_pool, 0);
                if (!sdu_tx) {
                    vTaskDelay(1);
                    continue;
                }

                rc = os_mbuf_append(sdu_tx, value, cent_tx_sdu_len);
                if (rc != 0) {
                    os_mbuf_free_chain(sdu_tx);
                    continue;
                }

                rc = ble_l2cap_send(chan, sdu_tx);

                if (rc == 0) {
                    segment_sdus++;
                } else if (rc == BLE_HS_ESTALLED) {
                    chan_stalled = true;
                    xEventGroupWaitBits(coc_event_group, TX_UNSTALLED_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(100));
                    continue;
                } else if (rc == BLE_HS_EBUSY) {
                    os_mbuf_free_chain(sdu_tx);
                    if (chan_stalled) {
                        xEventGroupWaitBits(coc_event_group, TX_UNSTALLED_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(100));
                    } else {
                        taskYIELD();
                    }
                    continue;
                } else if (rc == BLE_HS_ENOMEM) {
                    vTaskDelay(1);
                    continue;
                } else {
                    ESP_LOGE(TAG, "Send failed; rc=%d", rc);
                    if (rc == BLE_HS_EBADDATA) {
                        os_mbuf_free_chain(sdu_tx);
                    }
                    break;
                }

                if (segment_sdus % TX_YIELD_INTERVAL == 0 && segment_sdus > 0) {
                    vTaskDelay(1);
                }
            }

            if (!coc_chan && !lost_connection) {
                cent_seg_tx_done = NULL;
                cent_seg_tx_drop = NULL;
                lost_connection = true;
            }

            if (lost_connection) {
                break;
            }

            if (chan_stalled) {
                ESP_LOGI(TAG, "Post-drain: waiting for unstall");
                wait_unstall(POSTDRAIN_TIMEOUT_MS);
                if (chan_stalled) {
                    ESP_LOGW(TAG, "Post-drain timed out; pre-drain will retry");
                }
            }

            int64_t elapsed_us = esp_timer_get_time() - start_us;
            if (elapsed_us < 1) { elapsed_us = 1; }

            uint64_t bytes_sent  = (uint64_t)segment_sdus * cent_tx_sdu_len;
            uint32_t elapsed_ms  = (uint32_t)(elapsed_us / 1000);
            if (elapsed_ms == 0) { elapsed_ms = 1; }
            uint32_t tp_kbps     = (uint32_t)((bytes_sent * 8ULL) / elapsed_ms);
            uint32_t dropped     = segment_drops;

            cent_seg_tx_done     = NULL;
            cent_seg_tx_drop     = NULL;

            ESP_LOGI(TAG, "+-------------------------------------------------+");
            ESP_LOGI(TAG, "| PHY    : %-39s|", phy->name);
            ESP_LOGI(TAG, "| TX     : %-6" PRIu32 " kbps                            |", tp_kbps);
            ESP_LOGI(TAG, "| Bytes  : %-10" PRIu64 "                               |", bytes_sent);
            ESP_LOGI(TAG, "| Time   : %-5" PRIu32 " s                                |", elapsed_ms / 1000);
            ESP_LOGI(TAG, "+-------------------------------------------------+");
            if (dropped > 0) {
                ESP_LOGW(TAG, "%" PRIu32 " SDUs dropped (ENOMEM); raise BT_NIMBLE_MSYS_1_BLOCK_COUNT",
                         dropped);
            }
        }

        if (lost_connection) {
            ESP_LOGI(TAG, "Waiting for L2CAP COC reconnection...");
            /* disconnect handler already cleared COC_CONNECTED_BIT; clearing it
             * again here could cancel a bit set by a reconnect that raced ahead. */
            xEventGroupWaitBits(coc_event_group, COC_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
        } else {
            ESP_LOGI(TAG, "Cycle complete. Looping back to first PHY...");
        }
    }
}

static int cent_gap_event(struct ble_gap_event *event, void *arg)
{
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_DISC:
        cent_connect_if_interesting(&event->disc);
        return 0;

#if CONFIG_EXAMPLE_EXTENDED_ADV
    case BLE_GAP_EVENT_EXT_DISC:
        cent_connect_if_interesting_ext(&event->ext_disc);
        return 0;
#endif

    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            struct ble_gap_conn_desc desc;
            if (ble_gap_conn_find(event->connect.conn_handle, &desc) == 0) {
                ESP_LOGI(TAG, "Connected; handle=%d peer=%02x:%02x:%02x:%02x:%02x:%02x",
                         event->connect.conn_handle,
                         desc.peer_id_addr.val[5], desc.peer_id_addr.val[4],
                         desc.peer_id_addr.val[3], desc.peer_id_addr.val[2],
                         desc.peer_id_addr.val[1], desc.peer_id_addr.val[0]);
            }

            conn_handle      = event->connect.conn_handle;
            l2cap_connecting = false;

            rc = ble_hs_hci_util_set_data_len(conn_handle,
                                              LL_PACKET_LENGTH, LL_PACKET_TIME);
            if (rc != 0) {
                /* DATA_LEN_CHG won't fire — connect L2CAP directly as fallback */
                ESP_LOGE(TAG, "Set packet length failed; rc=%d, connecting L2CAP directly", rc);
                l2cap_connecting = true;
                cent_l2cap_coc_connect(conn_handle);
            } else {
                /* DLE accepted; connect L2CAP now — DATA_LEN_CHG may not fire if
                 * the peer's data length is already at the requested value */
                l2cap_connecting = true;
                cent_l2cap_coc_connect(conn_handle);
            }
        } else {
              ESP_LOGE(TAG, "Connection failed; status=%d", event->connect.status);
              cent_scan();
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "Disconnected; reason=%d", event->disconnect.reason);
        conn_handle      = BLE_HS_CONN_HANDLE_NONE;
        coc_chan         = NULL;
        chan_stalled     = false;
        ci_is_slow       = false;
        l2cap_connecting = false;
        xEventGroupClearBits(coc_event_group, COC_CONNECTED_BIT);
        xEventGroupSetBits(coc_event_group, TX_UNSTALLED_BIT | CONN_UPDATED_BIT | PHY_UPDATED_BIT);
        cent_scan();
        return 0;

    case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
        ESP_LOGI(TAG, "PHY updated: tx=%d rx=%d status=%d",
                 event->phy_updated.tx_phy,
                 event->phy_updated.rx_phy,
                 event->phy_updated.status);
        if (event->phy_updated.status != 0) {
            ESP_LOGW(TAG, "PHY update failed; status=%d", event->phy_updated.status);
        }
        xEventGroupSetBits(coc_event_group, PHY_UPDATED_BIT);
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        ESP_LOGI(TAG, "Conn params updated; status=%d", event->conn_update.status);
        if (event->conn_update.status != 0) {
            ESP_LOGW(TAG, "Connection parameter update failed (status=%d)",
                     event->conn_update.status);
        }
        xEventGroupSetBits(coc_event_group, CONN_UPDATED_BIT);
        return 0;

    case BLE_GAP_EVENT_DATA_LEN_CHG:
        /* fires for TX and RX; guard ensures connect called only once */
        if (!l2cap_connecting && coc_chan == NULL) {
            l2cap_connecting = true;
            cent_l2cap_coc_connect(conn_handle);
        }
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        ESP_LOGI(TAG, "Discovery complete; reason=%d", event->disc_complete.reason);
        return 0;

    default:
        return 0;
    }
}

static void cent_on_reset(int reason)
{
    ESP_LOGE(TAG, "Host reset; reason=%d", reason);
}

static void cent_on_sync(void)
{
    int rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    uint8_t own_addr_type;
    uint8_t addr[6] = {0};

    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error inferring addr type; rc=%d", rc);
        return;
    }
    ble_hs_id_copy_addr(own_addr_type, addr, NULL);
    ESP_LOGI(TAG, "Device Address: %02x:%02x:%02x:%02x:%02x:%02x",
             addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);

    cent_scan();
}

static void cent_host_task(void *param)
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

    coc_event_group = xEventGroupCreate();
    assert(coc_event_group);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "nimble_port_init failed; rc=%d", ret);
        return;
    }

    cent_l2cap_coc_mem_init();

    ble_hs_cfg.reset_cb        = cent_on_reset;
    ble_hs_cfg.sync_cb         = cent_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

#if CONFIG_BT_NIMBLE_GAP_SERVICE
    int rc = ble_svc_gap_device_name_set("l2cap-coc-cent");
    assert(rc == 0);
#endif

    ble_store_config_init();

    if (xTaskCreate(cent_send_task, "cent_send_task", 4096, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create cent_send_task");
        return;
    }

    nimble_port_freertos_init(cent_host_task);
}
