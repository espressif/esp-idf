/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "os/os.h"
#include "os/os_mempool.h"
#include "esp_hci_driver.h"
#include "esp_hci_internal.h"
#include "common/hci_driver_util.h"

#define TAG                 "HCI_UTIL"
#define HCI_DRIVER_UTIL_TX_POOL_NUM                                     \
    (CONFIG_BT_LE_ACL_BUF_COUNT + CONFIG_BT_LE_HCI_EVT_HI_BUF_COUNT + CONFIG_BT_LE_HCI_EVT_LO_BUF_COUNT)

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

/**
 * @brief Structure representing HCI TX data.
 */
typedef struct hci_driver_util_tx_entry {
    hci_driver_data_type_t data_type;   ///< Type of the HCI TX data.
    uint8_t *data;                         ///< Pointer to the TX data.
    uint32_t length;                    ///< Length of the TX data.
    STAILQ_ENTRY(hci_driver_util_tx_entry) next; ///< Next element in the linked list.
} hci_driver_util_tx_entry_t;

/* The list for hci_driver_util_tx_entry */
STAILQ_HEAD(hci_driver_util_tx_list, hci_driver_util_tx_entry);

typedef struct {
    struct hci_driver_util_tx_list tx_head;
    struct hci_driver_util_tx_entry *cur_tx_entry;
    uint32_t cur_tx_off;
    struct os_mempool *tx_entry_pool;
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

    s_hci_driver_util_env.tx_entry_pool = (struct os_mempool *)malloc(sizeof(struct os_mempool));
    if (!s_hci_driver_util_env.tx_entry_pool) {
        ESP_LOGE(TAG, "No memory for tx pool");
        goto init_err;
    }

    s_hci_driver_util_env.tx_entry_mem = malloc(OS_MEMPOOL_SIZE(HCI_DRIVER_UTIL_TX_POOL_NUM,
                                                sizeof(hci_driver_util_tx_entry_t)) * sizeof(os_membuf_t));
    if (!s_hci_driver_util_env.tx_entry_mem) {
        ESP_LOGE(TAG, "No memory for tx pool buffer");
        goto init_err;
    }

    rc = os_mempool_init(s_hci_driver_util_env.tx_entry_pool, HCI_DRIVER_UTIL_TX_POOL_NUM,
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

void
hci_driver_util_tx_list_enqueue(hci_driver_data_type_t type, uint8_t *data, uint32_t len)
{
    os_sr_t sr;
    hci_driver_util_tx_entry_t *tx_entry;

    tx_entry = os_memblock_get(s_hci_driver_util_env.tx_entry_pool);
    HCI_TRANS_ASSERT((tx_entry != NULL), 0, 0);
    tx_entry->data_type = type;
    tx_entry->data = data;
    tx_entry->length = len;
    /* If the txbuf is command status event or command complete event, we should send firstly.
     * The tx list maybe used in the controller task and hci task. Therefore, enter critical area.
     */
    if ((type == HCI_DRIVER_TYPE_EVT) && ((data[0] == 0x0E) || (data[0] == 0x0F))) {
        OS_ENTER_CRITICAL(sr);
        STAILQ_INSERT_HEAD(&s_hci_driver_util_env.tx_head, tx_entry, next);
        OS_EXIT_CRITICAL(sr);
    } else {
        OS_ENTER_CRITICAL(sr);
        STAILQ_INSERT_TAIL(&s_hci_driver_util_env.tx_head, tx_entry, next);
        OS_EXIT_CRITICAL(sr);
    }
}

uint32_t
hci_driver_util_tx_list_dequeue(uint32_t max_tx_len, void **tx_data, bool *last_frame)
{
    os_sr_t sr;
    uint32_t tx_len;
    uint32_t data_len;
    uint16_t out_off;
    struct os_mbuf *om;
    hci_driver_util_tx_entry_t *tx_entry;

    /* Check if there is any remaining data that hasn't been sent completely. If it has been completed,
     * free the corresponding memory. Therefore, the HCI TX entry needs to be sent one by one; multiple
     * entries cannot be sent together.
     */
    tx_len = 0;
    tx_entry = s_hci_driver_util_env.cur_tx_entry;
    if (tx_entry) {
        data_len = tx_entry->length;
        if (tx_entry->data_type == HCI_DRIVER_TYPE_ACL) {
            om = (struct os_mbuf *)tx_entry->data;
            if (s_hci_driver_util_env.cur_tx_off >= data_len) {
                os_mbuf_free_chain(om);
            } else {
                om = os_mbuf_off(om, s_hci_driver_util_env.cur_tx_off, &out_off);
                tx_len = min(max_tx_len, om->om_len - out_off);
                *tx_data = (void *)&om->om_data[out_off];
            }
        } else if (tx_entry->data_type == HCI_DRIVER_TYPE_EVT) {
            if (s_hci_driver_util_env.cur_tx_off >= data_len) {
                r_ble_hci_trans_buf_free(tx_entry->data);
            } else {
                tx_len = min(max_tx_len, data_len - s_hci_driver_util_env.cur_tx_off);
                *tx_data = &tx_entry->data[s_hci_driver_util_env.cur_tx_off];
            }
        } else {
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
            os_memblock_put(s_hci_driver_util_env.tx_entry_pool, (void *)tx_entry);
            s_hci_driver_util_env.cur_tx_entry = NULL;
        }
    }

    /* Find a new entry. */
    if (!tx_len && !STAILQ_EMPTY(&s_hci_driver_util_env.tx_head)) {
        OS_ENTER_CRITICAL(sr);
        tx_entry = STAILQ_FIRST(&s_hci_driver_util_env.tx_head);
        STAILQ_REMOVE_HEAD(&s_hci_driver_util_env.tx_head, next);
        OS_EXIT_CRITICAL(sr);

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
        if (tx_entry->data_type == HCI_DRIVER_TYPE_ACL) {
            os_mbuf_free_chain((struct os_mbuf *)tx_entry->data);
        } else if (tx_entry->data_type == HCI_DRIVER_TYPE_EVT) {
            r_ble_hci_trans_buf_free(tx_entry->data);
        }
        tx_entry = next_entry;
    }

    hci_driver_util_memory_deinit();

    memset(&s_hci_driver_util_env, 0, sizeof(hci_driver_util_env_t));
}


void
hci_driver_util_assert_check(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2)
{
    ESP_LOGE(TAG, "hci driver assert: line %d in function %s, param: 0x%x, 0x%x", ln, fn, param1, param2);
}
