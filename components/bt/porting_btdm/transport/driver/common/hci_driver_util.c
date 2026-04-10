/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_hci_driver.h"
#include "esp_hci_internal.h"
#include "common/hci_driver_util.h"

#define TAG                                              "HCI_UTIL"
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
#define HCI_DRIVER_UTIL_BREDR_HCI_EVT_TX_POOL_NUM        4
#define HCI_DRIVER_UTIL_BREDR_TX_POOL_NUM                \
    (CONFIG_BT_CTRL_BR_EDR_ACLU_RX_BUF_NB_EFF + CONFIG_BT_CTRL_BR_EDR_SYNC_RX_BUF_NB_EFF + HCI_DRIVER_UTIL_BREDR_HCI_EVT_TX_POOL_NUM)
#else
#define HCI_DRIVER_UTIL_BREDR_TX_POOL_NUM                0
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

#if UC_BT_CTRL_BLE_IS_ENABLE
#define HCI_DRIVER_UTIL_BLE_TX_POOL_NUM                   \
    (CONFIG_BT_LE_ACL_BUF_COUNT + CONFIG_BT_LE_HCI_EVT_HI_BUF_COUNT + CONFIG_BT_LE_HCI_EVT_LO_BUF_COUNT)
#else
#define HCI_DRIVER_UTIL_BLE_TX_POOL_NUM                   0
#endif // UC_BT_CTRL_BLE_IS_ENABLE

#define HCI_DRIVER_UTIL_TX_POOL_NUM                       \
    (HCI_DRIVER_UTIL_BLE_TX_POOL_NUM + HCI_DRIVER_UTIL_BREDR_TX_POOL_NUM)

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

/**
 * @brief Structure representing HCI TX data.
 */
typedef struct hci_driver_util_tx_entry {
    hci_driver_data_type_t data_type; ///< Type of the HCI TX data.
    uint8_t data_source;              ///< Source of the HCI TX data.
    uint32_t length;                  ///< Length of the TX data.
    union {
        hci_driver_packet_t *pkt;
        uint8_t *data;               ///< Length of the TX ISO data.
    };
    STAILQ_ENTRY(hci_driver_util_tx_entry) next; ///< Next element in the linked list.
} hci_driver_util_tx_entry_t;

/* The list for hci_driver_util_tx_entry */
STAILQ_HEAD(hci_driver_util_tx_list, hci_driver_util_tx_entry);

typedef struct {
    struct hci_driver_util_tx_list tx_head;
    struct hci_driver_util_tx_entry *cur_tx_entry;
    uint32_t cur_tx_off;
    struct btdm_mempool *tx_entry_pool;
    uint8_t *tx_entry_mem;
} hci_driver_util_env_t;

static hci_driver_util_env_t s_hci_driver_util_env;

static void
hci_driver_util_memory_deinit(void)
{
    if (s_hci_driver_util_env.tx_entry_pool) {
        free(s_hci_driver_util_env.tx_entry_pool);
        s_hci_driver_util_env.tx_entry_pool = NULL;
    }
    if (s_hci_driver_util_env.tx_entry_mem) {
        free(s_hci_driver_util_env.tx_entry_mem);
        s_hci_driver_util_env.tx_entry_mem = NULL;
    }
}

static int
hci_driver_util_memory_init(void)
{
    int rc;

    s_hci_driver_util_env.tx_entry_pool = (struct btdm_mempool *)malloc(sizeof(struct btdm_mempool));
    if (!s_hci_driver_util_env.tx_entry_pool) {
        ESP_LOGE(TAG, "No memory for tx pool");
        goto init_err;
    }

    s_hci_driver_util_env.tx_entry_mem = malloc(BTDM_MEMPOOL_SIZE(HCI_DRIVER_UTIL_TX_POOL_NUM,
                                                sizeof(hci_driver_util_tx_entry_t)) * sizeof(btdm_membuf_t));
    if (!s_hci_driver_util_env.tx_entry_mem) {
        ESP_LOGE(TAG, "No memory for tx pool buffer");
        goto init_err;
    }

    rc = btdm_mempool_init(s_hci_driver_util_env.tx_entry_pool, HCI_DRIVER_UTIL_TX_POOL_NUM,
                         sizeof(hci_driver_util_tx_entry_t), s_hci_driver_util_env.tx_entry_mem,
                         "hci_tx_entry_pool");
    if (rc) {
        ESP_LOGE(TAG, "Failed to initialize tx pool");
        goto init_err;
    }

    return 0;

init_err:
    hci_driver_util_memory_deinit();
    return -1;
}

int
hci_driver_util_tx_list_enqueue(hci_driver_data_type_t type, uint8_t *data, uint32_t len, hci_driver_data_source_t data_source)
{
    hci_driver_util_tx_entry_t *tx_entry;
    hci_driver_packet_t *pkt;
    uint8_t event_code = 0;

    tx_entry = btdm_memblock_get(s_hci_driver_util_env.tx_entry_pool);
    if (tx_entry == NULL) {
        return -1;
    }

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    pkt = (hci_driver_packet_t *)data;
    switch (data_source) {
        case HCI_DRIVER_BREDR_ACL:
            bredr_hci_trans_acl_tx_done(pkt);
            break;
        case HCI_DRIVER_BREDR_SYNC:
            bredr_hci_trans_sync_tx_done(pkt);
            break;
        case HCI_DRIVER_BREDR_EVT:
            bredr_hci_trans_evt_tx_done(pkt);
            break;
        default:
            break;
    }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE


    tx_entry->data_type = type;
    tx_entry->data = data;
    tx_entry->length = len;
    tx_entry->data_source = data_source;
    if (data_source == HCI_DRIVER_BTDM_EVT) {
        pkt = (hci_driver_packet_t *)data;
        event_code = pkt->data[0];
    } else if (data_source == HCI_DRIVER_LE_EVT){
        event_code = data[0];
    }
    /* If the txbuf is command status event or command complete event, we should send firstly.
     * The tx list maybe used in the controller task and hci task. Therefore, enter critical area.
     */
    if ((data_source == HCI_DRIVER_LE_EVT || data_source  == HCI_DRIVER_BTDM_EVT) &&
            ((event_code == 0x0E) || (event_code == 0x0F))) {
        btdm_osal_hw_enter_critical();
        STAILQ_INSERT_HEAD(&s_hci_driver_util_env.tx_head, tx_entry, next);
        btdm_osal_hw_exit_critical(0);
    } else {
        btdm_osal_hw_enter_critical();
        STAILQ_INSERT_TAIL(&s_hci_driver_util_env.tx_head, tx_entry, next);
        btdm_osal_hw_exit_critical(0);
    }

    return 0;
}

uint32_t
hci_driver_util_tx_list_dequeue(uint32_t max_tx_len, void **tx_data, bool *last_frame)
{
    uint32_t tx_len;
    uint32_t data_len;
    hci_driver_util_tx_entry_t *tx_entry;
    hci_driver_packet_t *pkt = NULL;
    struct ble_mbuf *om = NULL;
    uint16_t out_off;
    /* Check if there is any remaining data that hasn't been sent completely. If it has been completed,
     * free the corresponding memory. Therefore, the HCI TX entry needs to be sent one by one; multiple
     * entries cannot be sent together.
     */
    tx_len = 0;
    tx_entry = s_hci_driver_util_env.cur_tx_entry;
    if (tx_entry) {
        pkt = tx_entry->pkt;
        data_len = tx_entry->length;
        if (tx_entry->data_type == HCI_DRIVER_TYPE_ACL) {
            if (s_hci_driver_util_env.cur_tx_off >= data_len) {
                if (tx_entry->data_source == HCI_DRIVER_LE_ACL) {
                    om = (struct ble_mbuf *)tx_entry->data;
                    ble_mbuf_free_chain(om);
                }
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
                else {
                    bredr_hci_trans_acl_free(pkt);
                }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
            } else {
                if (tx_entry->data_source == HCI_DRIVER_LE_ACL) {
                    om = (struct ble_mbuf *)tx_entry->data;
                    om = ble_mbuf_off(om, s_hci_driver_util_env.cur_tx_off, &out_off);
                    tx_len = min(max_tx_len, om->om_len - out_off);
                    *tx_data = (void *)&om->om_data[out_off];
                } else {
                    tx_len = min(max_tx_len, data_len - s_hci_driver_util_env.cur_tx_off);
                    *tx_data = &pkt->data[s_hci_driver_util_env.cur_tx_off];
                }
            }
        } else if (tx_entry->data_type == HCI_DRIVER_TYPE_EVT) {
            if (s_hci_driver_util_env.cur_tx_off >= data_len) {
                if (tx_entry->data_source == HCI_DRIVER_LE_EVT) {
                    r_ble_hci_trans_buf_free(tx_entry->data);
                }
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
                else if (tx_entry->data_source == HCI_DRIVER_BREDR_EVT) {
                    bredr_hci_trans_evt_free(pkt);
                }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
                else {
                    btdm_hci_trans_buf_free(pkt);
                }
            } else {
                tx_len = min(max_tx_len, data_len - s_hci_driver_util_env.cur_tx_off);
                if (tx_entry->data_source == HCI_DRIVER_LE_EVT) {
                    *tx_data = &tx_entry->data[s_hci_driver_util_env.cur_tx_off];
                } else {
                    *tx_data = &pkt->data[s_hci_driver_util_env.cur_tx_off];
                }
            }
        } else if (tx_entry->data_type == HCI_DRIVER_TYPE_ISO) {
            if (s_hci_driver_util_env.cur_tx_off >= data_len) {
                free(tx_entry->data);
            } else {
                tx_len = min(max_tx_len, data_len - s_hci_driver_util_env.cur_tx_off);
                *tx_data = &tx_entry->data[s_hci_driver_util_env.cur_tx_off];
            }
        }
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
        else if (tx_entry->data_type == HCI_DRIVER_TYPE_SYNC) {
            if (s_hci_driver_util_env.cur_tx_off >= data_len) {
                bredr_hci_trans_sync_free(pkt);
            } else {
                tx_len = min(max_tx_len, data_len - s_hci_driver_util_env.cur_tx_off);
                *tx_data = &pkt->data[s_hci_driver_util_env.cur_tx_off];
            }
        }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
        else {
            HCI_TRANS_ASSERT(0, tx_entry->data_type, data_len);
        }
        /* If this is the last frame, inform the invoker not to call this API until the current data
         * has been completely sent.
         */
        if (tx_len) {
            s_hci_driver_util_env.cur_tx_off += tx_len;

            if (s_hci_driver_util_env.cur_tx_off >= data_len) {
                *last_frame = true;
            } else {
                *last_frame = false;
            }
        } else {
            btdm_memblock_put(s_hci_driver_util_env.tx_entry_pool, (void *)tx_entry);
            s_hci_driver_util_env.cur_tx_entry = NULL;
        }
    }

    /* Find a new entry. */
    if (!tx_len && !STAILQ_EMPTY(&s_hci_driver_util_env.tx_head)) {
        btdm_osal_hw_enter_critical();
        tx_entry = STAILQ_FIRST(&s_hci_driver_util_env.tx_head);
        STAILQ_REMOVE_HEAD(&s_hci_driver_util_env.tx_head, next);
        btdm_osal_hw_exit_critical(0);

        *tx_data = &tx_entry->data_type;
        s_hci_driver_util_env.cur_tx_entry = tx_entry;
        s_hci_driver_util_env.cur_tx_off = 0;
        tx_len = 1;
        *last_frame = false;
    }

    return tx_len;
}

int
hci_driver_util_init(void)
{
    memset(&s_hci_driver_util_env, 0, sizeof(hci_driver_util_env_t));

    if (hci_driver_util_memory_init()) {
        return -1;
    }

    STAILQ_INIT(&s_hci_driver_util_env.tx_head);

    return 0;
}

void
hci_driver_util_deinit(void)
{
    hci_driver_util_tx_entry_t *tx_entry;
    hci_driver_util_tx_entry_t *next_entry;

    /* Free all of controller buffers which haven't been sent yet.  The whole mempool will be freed.
     * Therefore, it's unnecessary to put the tx_entry into mempool.
     */
    tx_entry = STAILQ_FIRST(&s_hci_driver_util_env.tx_head);
    while (tx_entry) {
        next_entry = STAILQ_NEXT(tx_entry, next);
        if (tx_entry->data_type == HCI_DRIVER_TYPE_EVT) {
            if (tx_entry->data_source == HCI_DRIVER_LE_EVT) {
                r_ble_hci_trans_buf_free(tx_entry->data);
            }
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
            else if (tx_entry->data_source == HCI_DRIVER_BREDR_EVT) {
                bredr_hci_trans_evt_tx_done((hci_driver_packet_t *)tx_entry->data);
                bredr_hci_trans_evt_free((hci_driver_packet_t *)tx_entry->data);
            }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
            else {
                btdm_hci_trans_buf_free((hci_driver_packet_t *)tx_entry->data);
            }
        } else if (tx_entry->data_type == HCI_DRIVER_TYPE_ACL) {
            if (tx_entry->data_source == HCI_DRIVER_LE_ACL) {
                ble_mbuf_free_chain((struct ble_mbuf *)tx_entry->data);
            }
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
            else {
                bredr_hci_trans_acl_tx_done((hci_driver_packet_t *)tx_entry->data);
                bredr_hci_trans_acl_free((hci_driver_packet_t *)tx_entry->data);
            }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
        } else if (tx_entry->data_type == HCI_DRIVER_TYPE_ISO) {
            free(tx_entry->data);
        }
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
        else {
            // SCO data
            bredr_hci_trans_sync_tx_done((hci_driver_packet_t *)tx_entry->pkt);
            bredr_hci_trans_sync_free((hci_driver_packet_t *)tx_entry->pkt);
        }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

        tx_entry = next_entry;
    }

    hci_driver_util_memory_deinit();

    memset(&s_hci_driver_util_env, 0, sizeof(hci_driver_util_env_t));
}


void
hci_driver_util_assert_check(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2)
{
    ESP_LOGE(TAG, "hci driver assert: line %d in function %s, param: 0x%x, 0x%x", ln, fn, param1, param2);
    assert(0);
}
