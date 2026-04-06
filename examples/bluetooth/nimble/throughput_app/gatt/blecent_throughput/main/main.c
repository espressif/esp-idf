/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "nvs_flash.h"
/* BLE */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "gattc.h"
#include "esp_timer.h"
#include "linenoise/linenoise.h"
#include "esp_console.h"
#include "driver/uart.h"
#include "esp_vfs_dev.h"
#include "argtable3/argtable3.h"
#include "cmd_system.h"
#include "../src/ble_hs_hci_priv.h"

/* 0000xxxx-8c26-476f-89a7-a108033a69c7 */
#define THRPT_UUID_DECLARE(uuid16)                                \
    ((const ble_uuid_t *) (&(ble_uuid128_t) BLE_UUID128_INIT(   \
    0xc7, 0x69, 0x3a, 0x03, 0x08, 0xa1, 0xa7, 0x89,             \
    0x6f, 0x47, 0x26, 0x8c, uuid16, uuid16 >> 8, 0x00, 0x00     \
    )))

/* 0000xxxx-8c26-476f-89a7-a108033a69c6 */
#define THRPT_UUID_DECLARE_ALT(uuid16)                            \
    ((const ble_uuid_t *) (&(ble_uuid128_t) BLE_UUID128_INIT(   \
    0xc6, 0x69, 0x3a, 0x03, 0x08, 0xa1, 0xa7, 0x89,             \
    0x6f, 0x47, 0x26, 0x8c, uuid16, uuid16 >> 8, 0x00, 0x00     \
    )))

#define  THRPT_SVC                           0x0001
#define  THRPT_CHR_READ_WRITE                0x0006
#define  THRPT_CHR_NOTIFY                    0x000a
#define  THRPT_LONG_CHR_READ_WRITE           0x000b
#define  THRPT_LONG_CHR_READ_WRITE_ALT       0x001a
#define  THRPT_CHR_READ_WRITE_ALT            0x001f

/* Throughput cases */
#define READ_THROUGHPUT                    1
#define WRITE_THROUGHPUT                   2
#define NOTIFY_THROUGHPUT                  3

#define READ_THROUGHPUT_PAYLOAD            497 /* 502 bytes ACL -> 2x 251 LL packets exactly (497 + 1 Read Rsp + 4 L2CAP) */
#define WRITE_THROUGHPUT_PAYLOAD           495 /* 502 bytes ACL -> 2x 251 LL packets exactly (495 + 3 Write Cmd + 4 L2CAP) */
#define LL_PACKET_TIME                     2120
#define LL_PACKET_LENGTH                   251
static const char *tag = "blecent_throughput";
static int blecent_gap_event(struct ble_gap_event *event, void *arg);

static int mbuf_len_total;
static int failure_count;
static TaskHandle_t throughput_task_handle = NULL;
static int conn_params_def[] = {6, 6, 0, 500, 12, 24};
/* test_data accepts test_name and test_time from CLI */
static int test_data[6] = {1, 600, 0, 0, 0, 0};
static int mtu_def = 512;
static ble_addr_t conn_addr;
static uint16_t handle;
#define PHY_1M            0
#define PHY_2M            1
#define PHY_CODED_S2      2
#define PHY_CODED_S8      3

#if CONFIG_EXAMPLE_EXTENDED_ADV
static int current_phy_updated;
#endif

/* State for callback-chained read throughput test */
static volatile bool read_test_active = false;
static uint16_t read_val_handle_g;
static uint16_t read_conn_handle_g;
static int read_count_g;

/* ==============================================================================
 * Connection Parameter Tuning
 * ==============================================================================
 * itvl_min / itvl_max: Connection interval (time between connection events).
 *                      Units are in 1.25ms. (e.g., 24 * 1.25ms = 30ms).
 * latency:             Slave latency (number of events the peripheral can skip).
 * supervision_timeout: Time before link is considered lost. Units: 10ms.
 * min_ce_len / max_ce_len: Connection Event length. max_ce_len = 0xFFFF tells
 *                          the controller to use the ENTIRE connection interval
 *                          for transmitting data, instead of cutting it short.
 * ==============================================================================
 *
 * Write/Notify-optimized for 1M/2M: 30-50ms interval, full CE length. */
static struct ble_gap_upd_params conn_params = {
    .itvl_min = CONFIG_EXAMPLE_CONN_ITVL_MIN,
    .itvl_max = CONFIG_EXAMPLE_CONN_ITVL_MAX,
    .latency = CONFIG_EXAMPLE_CONN_LATENCY,
    .supervision_timeout = CONFIG_EXAMPLE_CONN_TIMEOUT,
    .min_ce_len = 0,
    .max_ce_len = 0xFFFF,
};

/* Write/Notify-optimized for Coded S2 */
static struct ble_gap_upd_params conn_params_coded_s2 = {
    .itvl_min = 24, /* 30 ms */
    .itvl_max = 32, /* 40 ms */
    .latency = 0,
    .supervision_timeout = CONFIG_EXAMPLE_CONN_TIMEOUT,
    .min_ce_len = 0,
    .max_ce_len = 0xFFFF,
};

/* Write/Notify-optimized for Coded S8 */
static struct ble_gap_upd_params conn_params_coded_s8 = {
    .itvl_min = 60, /* 75 ms */
    .itvl_max = 80, /* 100 ms */
    .latency = 0,
    .supervision_timeout = CONFIG_EXAMPLE_CONN_TIMEOUT,
    .min_ce_len = 0,
    .max_ce_len = 0xFFFF,
};

/* Read-optimized for 1M/2M: short interval for fast round-trips */
static struct ble_gap_upd_params conn_params_read = {
    .itvl_min = 6,          /* 7.5 ms  */
    .itvl_max = 8,          /* 10 ms   */
    .latency = 0,
    .supervision_timeout = CONFIG_EXAMPLE_CONN_TIMEOUT,
    .min_ce_len = 0,
    .max_ce_len = 0xFFFF,
};

/* Read-optimized for Coded S2 */
static struct ble_gap_upd_params conn_params_coded_s2_read = {
    .itvl_min = 12, /* 15 ms */
    .itvl_max = 16, /* 20 ms */
    .latency = 0,
    .supervision_timeout = CONFIG_EXAMPLE_CONN_TIMEOUT,
    .min_ce_len = 0,
    .max_ce_len = 0xFFFF,
};

/* Read-optimized for Coded S8 */
static struct ble_gap_upd_params conn_params_coded_s8_read = {
    .itvl_min = 36, /* 45 ms */
    .itvl_max = 40, /* 50 ms */
    .latency = 0,
    .supervision_timeout = CONFIG_EXAMPLE_CONN_TIMEOUT,
    .min_ce_len = 0,
    .max_ce_len = 0xFFFF,
};

static void switch_conn_params(uint16_t conn_handle, const struct ble_gap_upd_params *params) {
    int rc = ble_gap_update_params(conn_handle, params);
    if (rc != 0) {
        ESP_LOGE(tag, "Failed to update connection parameters: %d", rc);
    } else {
        ESP_LOGI(tag, "Requested connection parameter update");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void ble_store_config_init(void);

#if CONFIG_EXAMPLE_EXTENDED_ADV
void set_default_le_phy(uint8_t tx_phys_mask, uint8_t rx_phys_mask)
{
    int rc = ble_gap_set_prefered_default_le_phy(tx_phys_mask, rx_phys_mask);
    if (rc == 0) {
        ESP_LOGI(tag, "Default LE PHY set successfully; tx_phy = %d, rx_phy = %d",
                    tx_phys_mask, rx_phys_mask);
    } else {
        ESP_LOGE(tag, "Failed to set default LE PHY");
    }
}
#endif

static int
blecent_notify(uint16_t conn_handle, uint16_t val_handle,
               ble_gatt_attr_fn *cb, struct peer *peer, int test_time)
{
    uint8_t value[2] = {1, 0};/*To subscribe to notifications*/
    int rc;

    rc = ble_gattc_write_flat(conn_handle, val_handle,
                              value, sizeof value, NULL, &test_time);
    if (rc != 0) {
        ESP_LOGE(tag, "Error: Failed to subscribe to characteristic; "
                 "rc = %d", rc);
        goto err;
    }

    return 0;
err:
    /* Terminate the connection. */
    return ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

static int blecent_write(uint16_t conn_handle, uint16_t val_handle,
                         struct peer *peer, int test_time)
{
    int64_t start_time, end_time, write_time = 0;
    int write_count = 0;
    uint8_t value[WRITE_THROUGHPUT_PAYLOAD] = {0};
    int rc;

    value[0] = rand();
    failure_count = 0;
    start_time = esp_timer_get_time();

    while (write_time < test_time * 1000) {
        rc = ble_gattc_write_no_rsp_flat(conn_handle, val_handle, &value, sizeof value);

        if (rc == BLE_HS_ENOMEM) {
            vTaskDelay(2); /* Wait for buffers to free up and try again */
            end_time = esp_timer_get_time();
            write_time = (end_time - start_time) / 1000;
            continue;
        } else if (rc != 0) {
            ESP_LOGE(tag, "Error: Failed to write characteristic; rc=%d", rc);
            goto err;
        }

        end_time = esp_timer_get_time();
        write_time = (end_time - start_time) / 1000;
        write_count += 1;
    }

    /* Each successful write is of WRITE_THROUGHPUT_PAYLOAD Bytes of
     * application data. */
    printf("\n****************************************************************\n");
    ESP_LOGI(tag, "Application Write throughput = %d bps, write count = %d,"
             "failure count = %d",
             ((write_count - failure_count) * 8 * WRITE_THROUGHPUT_PAYLOAD) / test_time,
             write_count, failure_count);
    printf("\n****************************************************************\n");

    return 0;
err:
    /* Terminate the connection. */
    return ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

/**
 * Read callback that chains the next read immediately from within the
 * NimBLE host context. This eliminates the FreeRTOS task-switch delay
 * that previously caused each read to waste an extra connection event.
 */
static int
blecent_repeat_read(uint16_t conn_handle,
                    const struct ble_gatt_error *error,
                    struct ble_gatt_attr *attr,
                    void *arg)
{
    if (error->status == 0) {
        ESP_LOGD(tag, " attr_handle=%d value=", attr->handle);
        mbuf_len_total += OS_MBUF_PKTLEN(attr->om);
        read_count_g++;
    } else {
        ESP_LOGE(tag, " Read failed, callback error code = %d", error->status);
        /* On error, stop chaining and wake up main task */
        read_test_active = false;
        if (throughput_task_handle) {
            xTaskNotifyGive(throughput_task_handle);
        }
        return 0;
    }

    /* Chain next read immediately (zero task-switch overhead) */
    if (read_test_active) {
        int rc = ble_gattc_read(read_conn_handle_g, read_val_handle_g,
                                blecent_repeat_read, arg);
        if (rc != 0) {
            ESP_LOGE(tag, "Failed to chain read; rc=%d", rc);
            read_test_active = false;
            if (throughput_task_handle) {
                xTaskNotifyGive(throughput_task_handle);
            }
        }
    } else {
        /* Test time expired — wake up main task to print results */
        if (throughput_task_handle) {
            xTaskNotifyGive(throughput_task_handle);
        }
    }

    return 0;
}

static int blecent_read(uint16_t conn_handle, uint16_t val_handle,
                        struct peer *peer, int test_time)
{
    int rc;
    int64_t start_time;

    /* Reset counters */
    mbuf_len_total = 0;
    read_count_g = 0;
    read_conn_handle_g = conn_handle;
    read_val_handle_g = val_handle;
    read_test_active = true;

    /* Drain any stale notifications from a prior test that timed out */
    ulTaskNotifyTake(pdTRUE, 0);

    start_time = esp_timer_get_time();

    ESP_LOGD(tag, "  Throughput read started :val_handle=%d test_time=%d", val_handle,
             test_time);

    /* Kick off the first read — the callback will chain all subsequent reads */
    rc = ble_gattc_read(conn_handle, val_handle,
                        blecent_repeat_read, (void *) peer);
    if (rc != 0) {
        ESP_LOGE(tag, "Error: Failed to start read chain; rc=%d", rc);
        read_test_active = false;
        goto err;
    }

    /* Sleep for the test duration while the callback chain runs autonomously.
     * If the chain encounters an error (e.g. disconnect), it will notify us early. */
    uint32_t notified = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(test_time * 1000));

    if (notified == 0) {
        /* Normal timeout, signal the chain to stop after the current in-flight read completes */
        read_test_active = false;
        /* Wait for the last callback to notify us (up to 5s safety timeout) */
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5000));
    } else {
        /* Woken early by an error in the callback chain */
        read_test_active = false;
    }

    int64_t end_time = esp_timer_get_time();
    int actual_secs = (int)((end_time - start_time) / 1000000);
    if (actual_secs == 0) actual_secs = test_time;

    /* Application data throughput */
    printf("\n****************************************************************\n");
    ESP_LOGI(tag, "Application Read throughput = %d bps, Read op counter = %d",
             (mbuf_len_total * 8) / actual_secs, read_count_g);
    printf("\n****************************************************************\n");
    return 0;

err:
    /* Terminate the connection. */
    vTaskDelay(100 / portTICK_PERIOD_MS);
    return ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
}

#if CONFIG_EXAMPLE_EXTENDED_ADV
// Function to update PHY dynamically
int update_phy(uint16_t conn_handle, uint8_t phy_mode)
{
    int rc;
    switch (phy_mode) {
        case PHY_1M:
            current_phy_updated = 0;
            rc = ble_gap_set_prefered_le_phy(conn_handle, BLE_HCI_LE_PHY_1M_PREF_MASK, BLE_HCI_LE_PHY_1M_PREF_MASK, 0);
            if(rc != 0) {
                current_phy_updated = 1;
                ESP_LOGE(tag, "Requested PHY: 1M failed, rc=%d", rc);
            }
            else {
                ESP_LOGI(tag, "Requested PHY: 1M");
            }
            return rc;

        case PHY_2M:
            current_phy_updated = 0;
            rc = ble_gap_set_prefered_le_phy(conn_handle, BLE_HCI_LE_PHY_2M_PREF_MASK, BLE_HCI_LE_PHY_2M_PREF_MASK, 0);
            if(rc != 0) {
                current_phy_updated = 1;
                ESP_LOGE(tag, "Requested PHY: 2M failed, rc=%d", rc);
            }
            else {
                ESP_LOGI(tag, "Requested PHY: 2M");
            }
            return rc;

        case PHY_CODED_S2:
            current_phy_updated = 0;
            rc = ble_gap_set_prefered_le_phy(conn_handle, BLE_HCI_LE_PHY_CODED_PREF_MASK, BLE_HCI_LE_PHY_CODED_PREF_MASK, 0x01);
            if(rc != 0) {
                current_phy_updated = 1;
                ESP_LOGE(tag, "Requested PHY: Coded S2 failed, rc=%d", rc);
            }
            else {
                ESP_LOGI(tag, "Requested PHY: Coded S2");
            }
            return rc;

        case PHY_CODED_S8:
            current_phy_updated = 0;
            rc = ble_gap_set_prefered_le_phy(conn_handle, BLE_HCI_LE_PHY_CODED_PREF_MASK, BLE_HCI_LE_PHY_CODED_PREF_MASK, 0x02);
            if(rc != 0) {
                current_phy_updated = 1;
                ESP_LOGE(tag, "Requested PHY: Coded S8 failed, rc=%d", rc);
            }
            else {
                ESP_LOGI(tag, "Requested PHY: Coded S8");
            }
            return rc;

        default:
            ESP_LOGE(tag, "Invalid PHY mode selected.");
            return BLE_HS_EINVAL;
    }

    return rc;
}
#endif

static void throughput_task(void *arg)
{
    struct peer *peer = (struct peer *)arg;
    const struct peer_chr *chr;
    const struct peer_dsc *dsc;
    struct ble_gap_conn_desc desc;
    uint16_t conn_handle;
    int rc = 0;

    /* Store conn_handle immediately to avoid UAF if peer is freed */
    if (peer == NULL) {
        ESP_LOGE(tag, "Invalid peer, deleting task");
        throughput_task_handle = NULL;
        vTaskDelete(NULL);
        return;
    }
    conn_handle = peer->conn_handle;

    while (1) {
        /* Check if connection is still valid using stored conn_handle */
        rc = ble_gap_conn_find(conn_handle, &desc);
        if (rc != 0) {
            ESP_LOGI(tag, "Connection lost, deleting throughput task");
            throughput_task_handle = NULL;
            vTaskDelete(NULL);
            return;
        }

        vTaskDelay(4000 / portTICK_PERIOD_MS);
        ESP_LOGI(tag, "Format for throughput demo:: throughput read 100");
        printf(" ====================================================================================\n");
        printf(" |                 Steps to test nimble throughput                                   |\n");
        printf(" |                                                                                   |\n");
        printf(" |  1. Enter throughput [--Type] [--Test time] [--Phy]                               |\n");
        printf(" |  Type: read/write/notify.                                                         |\n");
        printf(" |  Test time: Enter value in seconds.                                               |\n");
        printf(" |  Phy mode: Enter value in 0 for 1M, 1 for 2M ,2 for Coded S2,                     |\n");
        printf(" |              3 for Coded S8.                                                      |\n");
        printf(" |                                                                                   |\n");
        printf(" |  e.g. throughput read 60 3                                                       |\n");
        printf(" |                                                                                   |\n");
        printf(" |  ** Enter 'throughput read 60 0' for reading char for 60 seconds on 1M Phy        |\n");
        printf(" |  OR 'throughput write 60 1' for writing to char for 60 seconds on 2M Phy          |\n");
        printf(" |  OR 'throughput notify 60 2' for notifications (for 60 seconds) on s=2 coded phy**|\n");
        printf(" |                                                                                   |\n");
        printf(" =====================================================================================\n\n");
        /* XXX Delay ? */
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        if (!cli_receive_key(test_data)) {
            /* No command supplied, start with reading */
            test_data[0] = READ_THROUGHPUT;
            test_data[1] = 60;
            test_data[2] = 0;
            ESP_LOGI(tag, "No command received from user, start with READ op"
                     " with 60 seconds test time");
        }
        scli_reset_queue();
#if CONFIG_EXAMPLE_EXTENDED_ADV
        if(test_data[2] >= 0) {
            rc = update_phy(conn_handle, test_data[2]);
            if(rc != 0) {
                ESP_LOGE(tag, "Failed to update phy, rc=%d. Skipping PHY update wait.", rc);
                /* Flag is already set to 1 on failure, but skip wait loop for clarity */
            } else {
                /* Wait for PHY update event to complete */
                while (!current_phy_updated) {
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                }
            }

            /* Send a 1-byte control message to the peripheral's read/write char
             * to tell it which PHY coding we are testing. This ensures the peripheral
             * perfectly mirrors our PHY preference (1M, 2M, S2, or S8). */
            if (test_data[2] >= 0 && test_data[2] <= 3) {
                const struct peer_chr *cmd_chr = peer_chr_find_uuid(peer,
                                                    THRPT_UUID_DECLARE(THRPT_SVC),
                                                    THRPT_UUID_DECLARE(THRPT_CHR_READ_WRITE));
                if (cmd_chr != NULL) {
                    uint8_t phy_cmd = test_data[2];
                    rc = ble_gattc_write_no_rsp_flat(conn_handle, cmd_chr->chr.val_handle, &phy_cmd, 1);
                    if (rc != 0) {
                        ESP_LOGW(tag, "Failed to send PHY cmd to peripheral; rc=%d", rc);
                    } else {
                        vTaskDelay(200 / portTICK_PERIOD_MS); /* Give peripheral time to apply PHY */
                    }
                }
            }
        }
#else
        /* Extended advertising is disabled, only 1M PHY is available */
        if (test_data[2] != PHY_1M) {
            ESP_LOGW(tag, "Extended advertising disabled; forcing PHY to 1M (ignoring user selection %d)", test_data[2]);
            test_data[2] = PHY_1M;
        }
#endif

        switch (test_data[0]) {

        case READ_THROUGHPUT:
            /* PHY-aware read connection params: short interval for fast
             * round-trips on 1M/2M, longer for coded PHY */
            if (test_data[2] == PHY_CODED_S2) {
                switch_conn_params(conn_handle, &conn_params_coded_s2_read);
            } else if (test_data[2] == PHY_CODED_S8) {
                switch_conn_params(conn_handle, &conn_params_coded_s8_read);
            } else {
                switch_conn_params(conn_handle, &conn_params_read);
            }

            /* Read the characteristic supporting long read support
             * `THRPT_LONG_CHR_READ_WRITE` (0x000b) */
            chr = peer_chr_find_uuid(peer,
                                     THRPT_UUID_DECLARE(THRPT_SVC),
                                     THRPT_UUID_DECLARE(THRPT_LONG_CHR_READ_WRITE));
            if (chr == NULL) {
                ESP_LOGE(tag, "Peer does not support "
                         "LONG_READ (0x000b) characteristic ");
                goto read_cleanup;
            }

            if (test_data[1] > 0) {
                rc = blecent_read(conn_handle, chr->chr.val_handle,
                                  (void *) peer, test_data[1]);
                if (rc != 0) {
                    ESP_LOGE(tag, "Error while reading from GATTS; rc = %d", rc);
                    /* Delete task on critical error (connection lost or fatal error) */
                    if (rc == BLE_HS_ENOTCONN || rc == BLE_HS_EDONE) {
                        ESP_LOGI(tag, "Connection error, deleting throughput task");
                        throughput_task_handle = NULL;
                        vTaskDelete(NULL);
                        return;
                    }
                }
            } else {
                ESP_LOGE(tag, "Please enter non-zero value for test time in seconds!!");
            }

read_cleanup:
            /* Restore write-optimized params after the read test */
            ESP_LOGI(tag, "Restoring WRITE-optimized conn params");
            if (test_data[2] == PHY_CODED_S2) {
                switch_conn_params(conn_handle, &conn_params_coded_s2);
            } else if (test_data[2] == PHY_CODED_S8) {
                switch_conn_params(conn_handle, &conn_params_coded_s8);
            } else {
                switch_conn_params(conn_handle, &conn_params);
            }
            break;

        case WRITE_THROUGHPUT:
            if (test_data[2] == PHY_CODED_S2) {
                switch_conn_params(conn_handle, &conn_params_coded_s2);
            } else if (test_data[2] == PHY_CODED_S8) {
                switch_conn_params(conn_handle, &conn_params_coded_s8);
            } else {
                switch_conn_params(conn_handle, &conn_params);
            }

            chr = peer_chr_find_uuid(peer,
                                     THRPT_UUID_DECLARE(THRPT_SVC),
                                     THRPT_UUID_DECLARE(THRPT_CHR_READ_WRITE));
            if (chr == NULL) {
                ESP_LOGE(tag, "Error: Peer doesn't support the READ "
                         "WRITE characteristic (0x0006) ");
                break;
            }

            if (test_data[1] > 0) {
                rc = blecent_write(conn_handle, chr->chr.val_handle, (void *) peer, test_data[1]);
                if (rc != 0) {
                    ESP_LOGE(tag, "Error while writing data; rc = %d", rc);
                    /* Delete task on critical error (connection lost or fatal error) */
                    if (rc == BLE_HS_ENOTCONN || rc == BLE_HS_EDONE) {
                        ESP_LOGI(tag, "Connection error, deleting throughput task");
                        throughput_task_handle = NULL;
                        vTaskDelete(NULL);
                        return;
                    }
                }
            } else {
                ESP_LOGE(tag, "Please enter non-zero value for test time in seconds!!");
            }
            break;

        case NOTIFY_THROUGHPUT:
            if (test_data[2] == PHY_CODED_S2) {
                switch_conn_params(conn_handle, &conn_params_coded_s2);
            } else if (test_data[2] == PHY_CODED_S8) {
                switch_conn_params(conn_handle, &conn_params_coded_s8);
            } else {
                switch_conn_params(conn_handle, &conn_params);
            }

            chr = peer_chr_find_uuid(peer,
                                     THRPT_UUID_DECLARE(THRPT_SVC),
                                     THRPT_UUID_DECLARE(THRPT_CHR_NOTIFY));
            if (chr == NULL) {
                ESP_LOGE(tag, "Error: Peer doesn't support the NOTIFY "
                         "characteristic (0x000a) ");
                break;
            }
            dsc = peer_dsc_find_uuid(peer,
                                     THRPT_UUID_DECLARE(THRPT_SVC),
                                     THRPT_UUID_DECLARE(THRPT_CHR_NOTIFY),
                                     BLE_UUID16_DECLARE(BLE_GATT_DSC_CLT_CFG_UUID16));
            if (dsc == NULL) {
                ESP_LOGE(tag, "Error: Peer lacks a CCCD for the Notify "
                         "Status characteristic\n");
                break;
            }

            rc = blecent_notify(conn_handle, dsc->dsc.handle,
                                NULL, (void *) peer, test_data[1]);
            if (rc != 0) {
                ESP_LOGE(tag, "Subscribing to notification failed; rc = %d ", rc);
                /* Delete task on critical error (connection lost or fatal error) */
                if (rc == BLE_HS_ENOTCONN || rc == BLE_HS_EDONE) {
                    ESP_LOGI(tag, "Connection error, deleting throughput task");
                    throughput_task_handle = NULL;
                    vTaskDelete(NULL);
                    return;
                }
            } else {
                ESP_LOGI(tag, "Subscribed to notifications. Throughput number"
                         " can be seen on peripheral terminal after %d seconds",
                         test_data[1]);
            }
            vTaskDelay(test_data[1]*1000 / portTICK_PERIOD_MS);

            /* Unsubscribe so the next notify test triggers a fresh
             * BLE_GAP_EVENT_SUBSCRIBE on the peripheral (cur_notify 0→1) */
            {
                uint8_t unsub_value[2] = {0, 0};
                rc = ble_gattc_write_flat(conn_handle, dsc->dsc.handle,
                                          unsub_value, sizeof unsub_value,
                                          NULL, NULL);
                if (rc != 0) {
                    ESP_LOGW(tag, "Unsubscribe failed; rc=%d (non-fatal)", rc);
                } else {
                    ESP_LOGI(tag, "Unsubscribed from notifications");
                }
            }
            break;

        default:
            break;
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

static void
blecent_read_write_subscribe(const struct peer *peer)
{
    /* Delete previous task if it exists */
    /* Capture handle and set global to NULL atomically to prevent race condition
     * where task deletes itself between NULL check and vTaskDelete call */
    TaskHandle_t task_to_delete = throughput_task_handle;
    throughput_task_handle = NULL;
    if (task_to_delete != NULL) {
        vTaskDelete(task_to_delete);
    }
    xTaskCreate(throughput_task, "throughput_task", 4096, (void *) peer, 10, &throughput_task_handle);
    return;
}

/**
 * Called when service discovery of the specified peer has completed.
 */
static void
blecent_on_disc_complete(const struct peer *peer, int status, void *arg)
{

    if (status != 0) {
        /* Service discovery failed.  Terminate the connection. */
        ESP_LOGE(tag, "Error: Service discovery failed; status=%d "
                 "conn_handle=%d\n", status, peer->conn_handle);
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    /* Service discovery has completed successfully.  Now we have a complete
     * list of services, characteristics, and descriptors that the peer
     * supports.
     */
    ESP_LOGI(tag, "Service discovery complete; status=%d "
             "conn_handle=%d\n", status, peer->conn_handle);

    /* Now perform three GATT procedures against the peer: read,
     * write, and subscribe to notifications depending upon user input.
     */
    blecent_read_write_subscribe(peer);
}

/**
 * Initiates the GAP general discovery procedure.
 */
static void
blecent_scan(void)
{
    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params = {0};
    int rc;

    /* Figure out address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(tag, "error determining address type; rc=%d", rc);
        return;
    }

    /* Tell the controller to filter duplicates; we don't want to process
     * repeated advertisements from the same device.
     */
    disc_params.filter_duplicates = 1;

    /**
     * Perform a passive scan.  I.e., don't send follow-up scan requests to
     * each advertiser.
     */
    disc_params.passive = 1;

    /* Use defaults for the rest of the parameters. */
    disc_params.itvl = 0;
    disc_params.window = 0;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;

    rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                      blecent_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(tag, "Error initiating GAP discovery procedure; rc=%d",
                 rc);
    }
}

#if CONFIG_EXAMPLE_EXTENDED_ADV
/**
 * Indicates whether we should try to connect to the sender of the specified
 * advertisement.  The function returns a positive result if the device
 * advertises connectability and support for the LE PHY service.
 */
static int
ext_blecent_should_connect(const struct ble_gap_ext_disc_desc *disc)
{
    int offset = 0;
    int ad_struct_len = 0;
    uint8_t test_addr[6];
    uint32_t peer_addr[6];
    uint8_t phy_uuid_found = 0;

    if (disc->legacy_event_type != BLE_HCI_ADV_RPT_EVTYPE_ADV_IND &&
            disc->legacy_event_type != BLE_HCI_ADV_RPT_EVTYPE_DIR_IND) {
        return 0;
    }
    if (strlen(CONFIG_EXAMPLE_PEER_ADDR) && (strncmp(CONFIG_EXAMPLE_PEER_ADDR, "ADDR_ANY", strlen("ADDR_ANY")) != 0)) {
       //  ESP_LOGI(tag, "Peer address from menuconfig: %s", CONFIG_EXAMPLE_PEER_ADDR);
        /* Convert string to address */
        sscanf(CONFIG_EXAMPLE_PEER_ADDR, "%lx:%lx:%lx:%lx:%lx:%lx",
               &peer_addr[5], &peer_addr[4], &peer_addr[3],
               &peer_addr[2], &peer_addr[1], &peer_addr[0]);

       /* Conversion */
        for (int i=0; i<6; i++) {
            test_addr[5 - i] = (uint8_t )peer_addr[i];
        }
        if (memcmp(test_addr, disc->addr.val, sizeof(disc->addr.val)) != 0) {
            return 0;
        }
    }

    /* The device has to advertise support LE PHY UUID (0xABF2).
    */
    do {
        if (offset + 1 >= (int)disc->length_data) {  /* At least read length and type */
            break;
        }
        ad_struct_len = disc->data[offset];

        if (!ad_struct_len || offset + ad_struct_len + 1 > (int)disc->length_data) {
            break;
        }

        /* Search for Complete Local Name (AD type 0x09) */
        if (disc->data[offset + 1] == 0x09 && phy_uuid_found) {
            int name_len = disc->data[offset] - 1;  /* Length minus type byte */
            char serv_name[] = "nimble_prph";
            if (name_len > 0) {
                ESP_LOGI(tag, "Device Name = %.*s", name_len, (char *)&disc->data[offset + 2]);
                if (name_len == strlen(serv_name) &&
                    memcmp(&disc->data[offset + 2], serv_name, name_len) == 0) {
                    ESP_LOGI(tag, "central connect to `nimble_prph` success");
                    return 1;
                }
                return 0;
            }
        }

        /* Search if LE PHY UUID is advertised */
        if (disc->data[offset] == 0x03 && disc->data[offset + 1] == 0x03 &&
            offset + 3 < (int)disc->length_data &&
            disc->data[offset + 2] == 0xAB && disc->data[offset + 3] == 0xF2) {
            phy_uuid_found = 1;
        }

        offset += ad_struct_len + 1;

    } while (offset < (int)disc->length_data);

    return phy_uuid_found;
}
 #else
/**
 * Indicates whether we should try to connect to the sender of the specified
 * advertisement.  The function returns a positive result if the device
 * advertises connectability and support for the THRPT service i.e. 0x0001.
 */
static int
blecent_should_connect(const struct ble_gap_disc_desc *disc)
{
    struct ble_hs_adv_fields fields;
    int rc;
    int i;
    uint8_t test_addr[6];
    uint32_t peer_addr[6];

    memset(peer_addr, 0x0, sizeof peer_addr);

    rc = ble_hs_adv_parse_fields(&fields, disc->data, disc->length_data);
    if (rc != 0) {
        return 0;
    }

    if (strlen(CONFIG_EXAMPLE_PEER_ADDR) && (strncmp(CONFIG_EXAMPLE_PEER_ADDR, "ADDR_ANY", strlen("ADDR_ANY")) != 0)) {
        ESP_LOGI(tag, "Peer address from menuconfig: %s", CONFIG_EXAMPLE_PEER_ADDR);
        /* Convert string to address */
        sscanf(CONFIG_EXAMPLE_PEER_ADDR, "%lx:%lx:%lx:%lx:%lx:%lx",
               &peer_addr[5], &peer_addr[4], &peer_addr[3],
               &peer_addr[2], &peer_addr[1], &peer_addr[0]);

        /* Conversion */
        for (int i=0; i<6; i++) {
            test_addr[i] = (uint8_t )peer_addr[i];
        }

        if (memcmp(test_addr, disc->addr.val, sizeof(disc->addr.val)) != 0) {
            return 0;
        }
    }

    ESP_LOGD(tag, "connect; fields.num_uuids128 =%d", fields.num_uuids128);
    for (i = 0; i < fields.num_uuids128; i++) {
        if ((memcmp(&fields.uuids128[i], THRPT_UUID_DECLARE(THRPT_SVC),
                    sizeof(ble_uuid128_t))) == 0 ) {
            ESP_LOGI(tag, "blecent_should_connect 'THRPT' success");
            return 1;
        }
    }

    char serv_name[] = "nimble_prph";
    if (fields.name != NULL) {
        ESP_LOGI(tag, "Device Name = %s", (char *)fields.name);

        if (memcmp(fields.name, serv_name, fields.name_len) == 0) {
            ESP_LOGI(tag, "central connect to `nimble_prph` success");
            return 1;
        }
    }
    return 0;
}
#endif

/**
 * Connects to the sender of the specified advertisement of it looks
 * interesting.  A device is "interesting" if it advertises connectability and
 * support for the Alert Notification service.
 */
static void
blecent_connect_if_interesting(void *disc)
{
    uint8_t own_addr_type;
    int rc;

    /* Don't do anything if we don't care about this advertiser. */
#if CONFIG_EXAMPLE_EXTENDED_ADV
    const struct ble_gap_ext_disc_desc *disc_ext = (const struct ble_gap_ext_disc_desc *)disc;
    if (!ext_blecent_should_connect(disc_ext)) {
        return;
    }
    /* Copy addr information for next connection */
    memcpy(&conn_addr, &disc_ext->addr, sizeof(conn_addr));
#else
    const struct ble_gap_disc_desc *disc_desc = (const struct ble_gap_disc_desc *)disc;
    if (!blecent_should_connect(disc_desc)) {
        return;
    }
    /* Copy addr information for next connection */
    memcpy(&conn_addr, &disc_desc->addr, sizeof(conn_addr));
#endif

#if !(MYNEWT_VAL(BLE_HOST_ALLOW_CONNECT_WITH_SCAN))
    /* Scanning must be stopped before a connection can be initiated. */
    rc = ble_gap_disc_cancel();
    if (rc != 0) {
        MODLOG_DFLT(DEBUG, "Failed to cancel scan; rc=%d\n", rc);
        return;
    }
#endif

    /* Figure out address to use for connect (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(tag, "error determining address type; rc=%d", rc);
        return;
    }

    /* Try to connect the the advertiser.  Allow 30 seconds (30000 ms) for
     * timeout.
     */
    rc = ble_gap_connect(own_addr_type, &conn_addr, 30000, NULL,
                         blecent_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(tag, "Error: Failed to connect to device; addr_type=%d "
                 "addr=%s; rc=%d\n",
                 conn_addr.type, addr_str(conn_addr.val), rc);
        return;
    }
}

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that is
 * established.  central uses the same callback for all connections.
 *
 * @param event                 The event being signalled.
 * @param arg                   Application-specified argument; unused by
 *                                  gattc.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular GAP event being signalled.
 */
static int
blecent_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    struct ble_hs_adv_fields fields;
    int rc;


    switch (event->type) {
    case BLE_GAP_EVENT_DISC:
        ESP_LOGD(tag, "Event DISC ");
        rc = ble_hs_adv_parse_fields(&fields, event->disc.data,
                                     event->disc.length_data);
        if (rc != 0) {
            return 0;
        }

        /* An advertisement report was received during GAP discovery. */
        print_adv_fields(&fields);

        /* Try to connect to the advertiser if it looks interesting. */
        blecent_connect_if_interesting(&event->disc);
        return 0;

    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            /* Connection successfully established. */
            /* XXX Set packet length in controller for better throughput */
            ESP_LOGI(tag, "Connection established ");
            rc = ble_hs_hci_util_set_data_len(event->connect.conn_handle,
                                              LL_PACKET_LENGTH, LL_PACKET_TIME);
            if (rc != 0) {
                ESP_LOGE(tag, "Set packet length failed; rc = %d", rc);
            }

            rc = ble_att_set_preferred_mtu(mtu_def);
            if (rc != 0) {
                ESP_LOGE(tag, "Failed to set preferred MTU; rc = %d", rc);
            }

            rc = ble_gattc_exchange_mtu(event->connect.conn_handle, NULL, NULL);
            if (rc != 0) {
                ESP_LOGE(tag, "Failed to negotiate MTU; rc = %d", rc);
            }

            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            print_conn_desc(&desc);

            rc = ble_gap_update_params(event->connect.conn_handle, &conn_params);
            if (rc != 0) {
                ESP_LOGE(tag, "Failed to update params; rc = %d", rc);
            }

            /* Remember peer. */
            rc = peer_add(event->connect.conn_handle);
            if (rc != 0) {
                ESP_LOGE(tag, "Failed to add peer; rc = %d", rc);
                return 0;
            }

            /* Perform service discovery. */
            rc = peer_disc_all(event->connect.conn_handle,
                               blecent_on_disc_complete, NULL);
            if (rc != 0) {
                ESP_LOGE(tag, "Failed to discover services; rc = %d", rc);
                return 0;
            }
        } else {
            /* Connection attempt failed; resume scanning. */
            ESP_LOGE(tag, "Error: Connection failed; status = %d",
                     event->connect.status);
            blecent_scan();
        }

        handle = event->connect.conn_handle;
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        /* Connection terminated. */
        ESP_LOGI(tag, "disconnect; reason=%d", event->disconnect.reason);
        print_conn_desc(&event->disconnect.conn);
        ESP_LOGI(tag, " ");

        /* Delete throughput task if it exists */
        /* Capture handle and set global to NULL atomically to prevent race condition
         * where task deletes itself between NULL check and vTaskDelete call */
        TaskHandle_t task_to_delete = throughput_task_handle;
        throughput_task_handle = NULL;
        if (task_to_delete != NULL) {
            vTaskDelete(task_to_delete);
        }

        /* Forget about peer. */
        peer_delete(event->disconnect.conn.conn_handle);
        vTaskDelay(200);

        /* Resume scanning. */
        blecent_scan();
        return 0;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        ESP_LOGI(tag, "discovery complete; reason = %d",
                 event->disc_complete.reason);
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        /* Encryption has been enabled or disabled for this connection. */
        ESP_LOGI(tag, "encryption change event; status = %d",
                 event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        print_conn_desc(&desc);
        return 0;

    case BLE_GAP_EVENT_NOTIFY_RX:
        /* Peer sent us a notification or indication. */
        mbuf_len_total = mbuf_len_total + OS_MBUF_PKTLEN(event->notify_rx.om);
        ESP_LOGD(tag, "received %s; conn_handle = %d attr_handle = %d "
                 "attr_len = %d ; Total length = %d",
                 event->notify_rx.indication ?
                 "indication" :
                 "notification",
                 event->notify_rx.conn_handle,
                 event->notify_rx.attr_handle,
                 OS_MBUF_PKTLEN(event->notify_rx.om),
                 mbuf_len_total);
        /* Attribute data is contained in event->notify_rx.attr_data. */
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        ESP_LOGI(tag, "connection updated; status=%d",
                 event->conn_update.status);
        return 0;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(tag, "mtu update event; conn_handle = %d cid = %d mtu = %d",
                 event->mtu.conn_handle,
                 event->mtu.channel_id,
                 event->mtu.value);
        return 0;

#if CONFIG_EXAMPLE_EXTENDED_ADV
    case BLE_GAP_EVENT_EXT_DISC:
        /* An advertisement report was received during GAP discovery. */
        blecent_connect_if_interesting(&event->ext_disc);
        return 0;

    case BLE_GAP_EVENT_PHY_UPDATE_COMPLETE:
        current_phy_updated = 1;
        ESP_LOGI(tag, "PHY Update Event: Status=%d, Conn_Handle=0x%04X, TX_PHY=%d, RX_PHY=%d",
            event->phy_updated.status, event->phy_updated.conn_handle,
            event->phy_updated.tx_phy, event->phy_updated.rx_phy);
        return 0;
#endif

    default:
        return 0;
    }
}

static void
blecent_on_reset(int reason)
{
    ESP_LOGE(tag, "Resetting state; reason=%d", reason);
}

static void
blecent_on_sync(void)
{
    int rc;
    bool yes = false;
#if CONFIG_EXAMPLE_EXTENDED_ADV
    uint8_t all_phy;
#endif

    /* Make sure we have proper identity address set (public preferred) */
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

#if CONFIG_EXAMPLE_EXTENDED_ADV
    all_phy = BLE_HCI_LE_PHY_1M_PREF_MASK | BLE_HCI_LE_PHY_2M_PREF_MASK | BLE_HCI_LE_PHY_CODED_PREF_MASK;

    set_default_le_phy(all_phy, all_phy);
#endif

    ESP_LOGI(tag, "Do you want to configure connection params ? ");
    ESP_LOGI(tag, "If yes then enter in this format: `Insert Yes` ");
    if (scli_receive_yesno(&yes)) {
        if (yes) {
            ESP_LOGI(tag, " Enter preferred MTU, format:: `MTU 512` ");
            int mtu_buf[6];
            if (scli_receive_key(mtu_buf)) {
                mtu_def = mtu_buf[0];
                ESP_LOGI(tag, "MTU provided by user= %d", mtu_def);
            } else {
                ESP_LOGD(tag, "No input for setting MTU; use default mtu = %d", mtu_def);
            }

            scli_reset_queue();
            ESP_LOGI(tag, "For Conn param: Enter `min_itvl` `max_itvl`"
                     "`latency` `timeout` `min_ce` `max_ce` in same order");
            ESP_LOGI(tag, "Enter conn_update in this format:: `conn 6 120 0 500 0 0`");

            if (scli_receive_key(conn_params_def)) {
                /** Minimum value for connection interval in 1.25ms units */
                conn_params.itvl_min = conn_params_def[0];
                /** Maximum value for connection interval in 1.25ms units */
                conn_params.itvl_max = conn_params_def[1];
                /** Connection latency */
                conn_params.latency = conn_params_def[2];
                /** Supervision timeout in 10ms units */
                conn_params.supervision_timeout = conn_params_def[3];
                /** Minimum length of connection event in 0.625ms units */
                conn_params.min_ce_len = conn_params_def[4];
                /** Maximum length of connection event in 0.625ms units */
                conn_params.max_ce_len = conn_params_def[5];

            } else {
                ESP_LOGD(tag, "no input by user for conn param; use default ");
            }
            scli_reset_queue();
        }
    }
    /* Begin scanning for a peripheral to connect to. */
    blecent_scan();
}

void blecent_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");

    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void
app_main(void)
{
    int rc;
    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if  (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to init nimble %d ", ret);
        return;
    }

    /* Configure the host. */
    ble_hs_cfg.reset_cb = blecent_on_reset;
    ble_hs_cfg.sync_cb = blecent_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Initialize data structures to track connected peers. */
#if MYNEWT_VAL(BLE_INCL_SVC_DISCOVERY) || MYNEWT_VAL(BLE_GATT_CACHING_INCLUDE_SERVICES)
    rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64, 64);
    assert(rc == 0);
#else
    rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64);
    assert(rc == 0);
#endif

    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set("gattc-throughput");
    assert(rc == 0);

    /* XXX Need to have template for store */
    ble_store_config_init();

    /* Before starting with blecent host task, let us get CLI task up and
     * running */
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    esp_console_repl_t *repl = NULL;
    repl_config.prompt = "Throughput : ";
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
    register_system();
    esp_console_register_help_command();
    /* Register commands */
    ble_register_cli();

    /* As BLE CLI has been registered, let us start nimble host task */
    nimble_port_freertos_init(blecent_host_task);

    printf("\n ===============================================================================================\n");
    printf(" |                         Steps to test nimble throughput                                       |\n");
    printf(" |                                                                                               |\n");
    printf(" |  1. Print 'help' to gain overview of commands                                                 |\n");
    printf(" |                                                                                               |\n");
    printf("\n ===============================================================================================\n");

    const char *prompt = LOG_COLOR_I "Throughput demo >> " LOG_RESET_COLOR;

    while (true) {
        /* Get a line using linenoise.
         * The line is returned when ENTER is pressed.
         */
        char *line = linenoise(prompt);
        if (line == NULL) { /* Ignore empty lines */
            continue;
        }
        /* Add the command to the history */
        linenoiseHistoryAdd(line);

        /* Try to run the command */
        int ret;
        esp_err_t err = esp_console_run(line, &ret);
        if (err == ESP_ERR_NOT_FOUND) {
            printf("Unrecognized command\n");
        } else if (err == ESP_ERR_INVALID_ARG) {
            // command was empty
        } else if (err == ESP_OK && ret != ESP_OK) {
            printf("Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(ret));
        } else if (err != ESP_OK) {
            printf("Internal error: %s\n", esp_err_to_name(err));
        }
        /* linenoise allocates line buffer on the heap, so need to free it */
        linenoiseFree(line);
    }
}
