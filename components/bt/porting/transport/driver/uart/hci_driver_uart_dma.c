/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "esp_hci_transport.h"
#include "esp_hci_internal.h"
#include "common/hci_driver_h4.h"
#include "common/hci_driver_util.h"
#include "common/hci_driver_mem.h"
#include "hci_driver_uart.h"

#include "esp_private/periph_ctrl.h"
#include "esp_private/gdma.h"
#include "hal/uhci_ll.h"

/*
 *  UART DMA Desc struct
 *
 * --------------------------------------------------------------
 * | own | EoF | sub_sof | 5'b0   | length [11:0] | size [11:0] |
 * --------------------------------------------------------------
 * |            buf_ptr [31:0]                                  |
 * --------------------------------------------------------------
 * |            next_desc_ptr [31:0]                            |
 * --------------------------------------------------------------
 */

/* this bitfield is start from the LSB!!! */
typedef struct uhci_lldesc_s {
    volatile uint32_t size  : 12,
             length: 12,
             offset: 5, /* h/w reserved 5bit, s/w use it as offset in buffer */
             sosf  : 1, /* start of sub-frame */
             eof   : 1, /* end of frame */
             owner : 1; /* hw or sw */
    volatile const uint8_t *buf;       /* point to buffer data */
    union {
        volatile uint32_t empty;
        STAILQ_ENTRY(uhci_lldesc_s) qe;  /* pointing to the next desc */
    };
} uhci_lldesc_t;

/**
 * @brief Enumeration of HCI transport transmission states.
 */
typedef enum {
    HCI_TRANS_TX_IDLE,    ///< HCI Transport TX is in idle state.
    HCI_TRANS_TX_START,   ///< HCI Transport TX is starting transmission.
    HCI_TRANS_TX_END,     ///< HCI Transport TX has completed transmission.
} hci_trans_tx_state_t;

typedef struct {
    TaskHandle_t task_handler;
    hci_driver_uart_params_config_t *hci_uart_params;
    SemaphoreHandle_t process_sem;
    struct hci_h4_sm *h4_sm;
    hci_driver_forward_fn *forward_cb;
    struct os_mempool *hci_rx_data_pool; /*!< Init a memory pool for rx_data cache */
    uint8_t *hci_rx_data_buffer;
    struct os_mempool *hci_rxinfo_pool; /*!< Init a memory pool for rxinfo cache */
    os_membuf_t *hci_rxinfo_buffer;
    volatile bool rxinfo_mem_exhausted; /*!< Indicate rxinfo memory does not exist */
    volatile bool is_continue_rx; /*!< Continue to rx */
    volatile hci_trans_tx_state_t hci_tx_state; /*!< HCI Tx State */
    struct os_mempool lldesc_mem_pool;/*!< Init a memory pool for uhci_lldesc_t */
    uhci_lldesc_t *lldesc_mem;
} hci_driver_uart_dma_env_t;

#define ESP_BT_HCI_TL_STATUS_OK            (0)   /*!< HCI_TL Tx/Rx operation status OK */
/* The number of lldescs pool */
#define HCI_LLDESCS_POOL_NUM                (CONFIG_BT_LE_HCI_LLDESCS_POOL_NUM)
/* Default block size for HCI RX data  */
#define HCI_RX_DATA_BLOCK_SIZE              (DEFAULT_BT_LE_ACL_BUF_SIZE + HCI_TRANSPORT_CMD_SZ)
#define HCI_RX_DATA_POOL_NUM                (CONFIG_BT_LE_HCI_TRANS_RX_MEM_NUM)
#define HCI_RX_INFO_POOL_NUM                (CONFIG_BT_LE_HCI_TRANS_RX_MEM_NUM + 1)

/**
 * @brief callback function for HCI Transport Layer send/receive operations
 */
typedef void (* esp_bt_hci_tl_callback_t) (void *arg, uint8_t status);

struct uart_txrxchannel {
    esp_bt_hci_tl_callback_t callback;
    void *arg;
    uhci_lldesc_t *link_head;
};

struct uart_env_tag {
    struct uart_txrxchannel tx;
    struct uart_txrxchannel rx;
};

typedef struct hci_message {
    void *ptr;                   ///< Pointer to the message data.
    uint32_t length;             ///< Length of the message data.
    STAILQ_ENTRY(hci_message) next; ///< Next element in the linked list.
} hci_message_t;

static void hci_driver_uart_dma_recv_async(uint8_t *buf, uint32_t size, esp_bt_hci_tl_callback_t callback, void *arg);
int hci_driver_uart_dma_rx_start(uint8_t *rx_data, uint32_t length);
int hci_driver_uart_dma_tx_start(esp_bt_hci_tl_callback_t callback, void *arg);

static const char *TAG = "uart_dma";
static hci_driver_uart_dma_env_t s_hci_driver_uart_dma_env;
static struct hci_h4_sm s_hci_driver_uart_h4_sm;

/* The list for hci_rx_data */
STAILQ_HEAD(g_hci_rxinfo_list, hci_message);

DRAM_ATTR struct g_hci_rxinfo_list g_hci_rxinfo_head;
static DRAM_ATTR struct uart_env_tag uart_env;
static volatile uhci_dev_t *s_uhci_hw = &UHCI0;
static DRAM_ATTR gdma_channel_handle_t s_rx_channel;
static DRAM_ATTR gdma_channel_handle_t s_tx_channel;

static int hci_driver_uart_dma_memory_deinit(void)
{

    if (s_hci_driver_uart_dma_env.hci_rxinfo_buffer) {
        free(s_hci_driver_uart_dma_env.hci_rxinfo_buffer);
        s_hci_driver_uart_dma_env.hci_rxinfo_buffer = NULL;
    }

    if (s_hci_driver_uart_dma_env.hci_rxinfo_pool) {
        free(s_hci_driver_uart_dma_env.hci_rxinfo_pool);
        s_hci_driver_uart_dma_env.hci_rxinfo_pool = NULL;
    }

    if (s_hci_driver_uart_dma_env.hci_rx_data_buffer) {
        free(s_hci_driver_uart_dma_env.hci_rx_data_buffer);
        s_hci_driver_uart_dma_env.hci_rx_data_buffer = NULL;
    }

    if (s_hci_driver_uart_dma_env.hci_rx_data_pool) {
        free(s_hci_driver_uart_dma_env.hci_rx_data_pool);
        s_hci_driver_uart_dma_env.hci_rx_data_pool = NULL;
    }

    if (s_hci_driver_uart_dma_env.lldesc_mem) {
        free(s_hci_driver_uart_dma_env.lldesc_mem);
        s_hci_driver_uart_dma_env.lldesc_mem = NULL;
    }

    return 0;
}

static int hci_driver_uart_dma_memory_init(void)
{
    int rc = 0;

    s_hci_driver_uart_dma_env.lldesc_mem = malloc(OS_MEMPOOL_SIZE(HCI_LLDESCS_POOL_NUM,
                        sizeof (uhci_lldesc_t)) * sizeof(os_membuf_t));
    if (!s_hci_driver_uart_dma_env.lldesc_mem) {
        return -1;
    }

    rc = os_mempool_init(&s_hci_driver_uart_dma_env.lldesc_mem_pool, HCI_LLDESCS_POOL_NUM,
                         sizeof (uhci_lldesc_t), s_hci_driver_uart_dma_env.lldesc_mem, "hci_lldesc_pool");
    if (rc) {
        goto init_err;
    }

    s_hci_driver_uart_dma_env.hci_rx_data_pool = (struct os_mempool *)malloc(sizeof(struct os_mempool));
    if (!s_hci_driver_uart_dma_env.hci_rx_data_pool) {
        goto init_err;
    }

    memset(s_hci_driver_uart_dma_env.hci_rx_data_pool, 0, sizeof(struct os_mempool));
    s_hci_driver_uart_dma_env.hci_rx_data_buffer = malloc(OS_MEMPOOL_SIZE(HCI_RX_DATA_POOL_NUM,
                                                    HCI_RX_DATA_BLOCK_SIZE) * sizeof(os_membuf_t));
    if (!s_hci_driver_uart_dma_env.hci_rx_data_buffer) {
        goto init_err;
    }

    memset(s_hci_driver_uart_dma_env.hci_rx_data_buffer, 0, OS_MEMPOOL_SIZE(HCI_RX_DATA_POOL_NUM,
                                          HCI_RX_DATA_BLOCK_SIZE) * sizeof(os_membuf_t));
    rc = os_mempool_init(s_hci_driver_uart_dma_env.hci_rx_data_pool, HCI_RX_DATA_POOL_NUM,
                         HCI_RX_DATA_BLOCK_SIZE, s_hci_driver_uart_dma_env.hci_rx_data_buffer,
                         "hci_rx_data_pool");
    if (rc) {
        goto init_err;
    }


    /* Malloc hci rxinfo pool */
    s_hci_driver_uart_dma_env.hci_rxinfo_pool = (struct os_mempool *)malloc(sizeof(struct os_mempool));
    if (!s_hci_driver_uart_dma_env.hci_rxinfo_pool) {
        goto init_err;
    }

    memset(s_hci_driver_uart_dma_env.hci_rxinfo_pool, 0, sizeof(struct os_mempool));
    s_hci_driver_uart_dma_env.hci_rxinfo_buffer = malloc(OS_MEMPOOL_SIZE(HCI_RX_INFO_POOL_NUM,
                                         sizeof(hci_message_t)) * sizeof(os_membuf_t));
    if (!s_hci_driver_uart_dma_env.hci_rxinfo_buffer) {
        goto init_err;
    }

    memset(s_hci_driver_uart_dma_env.hci_rxinfo_buffer, 0, OS_MEMPOOL_SIZE(HCI_RX_INFO_POOL_NUM,
           sizeof(hci_message_t)) * sizeof(os_membuf_t));
    rc = os_mempool_init(s_hci_driver_uart_dma_env.hci_rxinfo_pool, HCI_RX_INFO_POOL_NUM,
                         sizeof(hci_message_t), s_hci_driver_uart_dma_env.hci_rxinfo_buffer,
                         "hci_rxinfo_pool");
    if (rc) {
        goto init_err;
    }

    return rc;
init_err:
    hci_driver_uart_dma_memory_deinit();
    return rc;
}

static IRAM_ATTR bool hci_uart_tl_rx_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    esp_bt_hci_tl_callback_t callback = uart_env.rx.callback;
    void *arg = uart_env.rx.arg;
    assert(dma_chan == s_rx_channel);
    assert(uart_env.rx.callback != NULL);
    // clear callback pointer
    uart_env.rx.callback = NULL;
    uart_env.rx.arg = NULL;
    // call handler
    callback(arg, ESP_BT_HCI_TL_STATUS_OK);
    return true;
}

static IRAM_ATTR bool hci_uart_tl_tx_eof_callback(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data)
{
    esp_bt_hci_tl_callback_t callback = uart_env.tx.callback;
    assert(dma_chan == s_tx_channel);
    assert(uart_env.tx.callback != NULL);
    // clear callback pointer
    uart_env.tx.callback = NULL;
    // call handler
    callback(uart_env.tx.arg, ESP_BT_HCI_TL_STATUS_OK);
    uart_env.tx.arg = NULL;
    return true;
}

uint8_t * IRAM_ATTR hci_driver_uart_dma_rxdata_memory_get(void)
{
    uint8_t *rx_data;
    rx_data = os_memblock_get(s_hci_driver_uart_dma_env.hci_rx_data_pool);
    return rx_data;
}

hci_message_t * IRAM_ATTR hci_driver_uart_dma_rxinfo_memory_get(void)
{
    hci_message_t *rx_info;
    rx_info = os_memblock_get(s_hci_driver_uart_dma_env.hci_rxinfo_pool);
    return rx_info;
}

void IRAM_ATTR hci_driver_uart_dma_cache_rxinfo(hci_message_t *hci_rxinfo)
{
    os_sr_t sr;

    OS_ENTER_CRITICAL(sr);
    STAILQ_INSERT_TAIL(&g_hci_rxinfo_head, hci_rxinfo, next);
    OS_EXIT_CRITICAL(sr);
}

void IRAM_ATTR hci_driver_uart_dma_continue_rx_enable(bool enable)
{
    os_sr_t sr;
    OS_ENTER_CRITICAL(sr);
    s_hci_driver_uart_dma_env.is_continue_rx = enable;
    OS_EXIT_CRITICAL(sr);
}

void IRAM_ATTR hci_driver_uart_dma_rxinfo_mem_exhausted_set(bool is_exhausted)
{
    os_sr_t sr;
    OS_ENTER_CRITICAL(sr);
    s_hci_driver_uart_dma_env.rxinfo_mem_exhausted = is_exhausted;
    OS_EXIT_CRITICAL(sr);
}

void IRAM_ATTR hci_driver_uart_dma_recv_callback(void *arg, uint8_t status)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    hci_message_t *hci_rxinfo;
    uint8_t *rx_data;

    if (s_hci_driver_uart_dma_env.rxinfo_mem_exhausted) {
        ESP_LOGE(TAG, "Will lost rx data, need adjust rxinfo memory count\n");
        assert(0);
    }

    hci_rxinfo = hci_driver_uart_dma_rxinfo_memory_get();
    if (!hci_rxinfo) {
        ESP_LOGW(TAG, "set rxinfo mem exhausted flag\n");
        hci_driver_uart_dma_rxinfo_mem_exhausted_set(true);
        xSemaphoreGiveFromISR(s_hci_driver_uart_dma_env.process_sem, &xHigherPriorityTaskWoken);
        return;
    }

    hci_rxinfo->ptr = (void *)uart_env.rx.link_head->buf;
    hci_rxinfo->length = uart_env.rx.link_head->length;
    hci_driver_uart_dma_cache_rxinfo(hci_rxinfo);
    xSemaphoreGiveFromISR(s_hci_driver_uart_dma_env.process_sem, &xHigherPriorityTaskWoken);
    rx_data = hci_driver_uart_dma_rxdata_memory_get();
    if (!rx_data) {
        hci_driver_uart_dma_continue_rx_enable(true);
    }else {
        hci_driver_uart_dma_rx_start(rx_data, HCI_RX_DATA_BLOCK_SIZE);
    }
}

void IRAM_ATTR hci_driver_uart_dma_txstate_set(hci_trans_tx_state_t tx_state)
{
    os_sr_t sr;
    OS_ENTER_CRITICAL(sr);
    s_hci_driver_uart_dma_env.hci_tx_state = tx_state;
    OS_EXIT_CRITICAL(sr);
}

void IRAM_ATTR hci_driver_uart_dma_send_callback(void *arg, uint8_t status)
{
    uhci_lldesc_t *lldesc_head;
    uhci_lldesc_t *lldesc_nxt;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    lldesc_head = uart_env.tx.link_head;
    while (lldesc_head) {
        lldesc_nxt = lldesc_head->qe.stqe_next;
        os_memblock_put(&s_hci_driver_uart_dma_env.lldesc_mem_pool, lldesc_head);
        lldesc_head = lldesc_nxt;
    }

    uart_env.tx.link_head = NULL;
    hci_driver_uart_dma_txstate_set(HCI_TRANS_TX_IDLE);
    xSemaphoreGiveFromISR(s_hci_driver_uart_dma_env.process_sem, &xHigherPriorityTaskWoken);
}

static IRAM_ATTR void hci_driver_uart_dma_recv_async(uint8_t *buf, uint32_t size, esp_bt_hci_tl_callback_t callback, void *arg)
{
    uhci_lldesc_t *lldesc_head;
    assert(buf != NULL);
    assert(size != 0);
    assert(callback != NULL);
    uart_env.rx.callback = callback;
    uart_env.rx.arg = arg;
    lldesc_head = uart_env.rx.link_head;

    while (lldesc_head) {
        os_memblock_put(&s_hci_driver_uart_dma_env.lldesc_mem_pool, lldesc_head),
        lldesc_head = lldesc_head->qe.stqe_next;
    }

    uart_env.rx.link_head = NULL;
    lldesc_head = os_memblock_get(&s_hci_driver_uart_dma_env.lldesc_mem_pool);
    assert(lldesc_head);
    memset(lldesc_head, 0, sizeof(uhci_lldesc_t));
    lldesc_head->buf = buf;
    lldesc_head->size = size;
    lldesc_head->eof = 0;
    s_uhci_hw->pkt_thres.pkt_thrs = size;
    uart_env.rx.link_head = lldesc_head;
    gdma_start(s_rx_channel, (intptr_t)(uart_env.rx.link_head));
}

int IRAM_ATTR hci_driver_uart_dma_rx_start(uint8_t *rx_data, uint32_t length)
{
    hci_driver_uart_dma_recv_async(rx_data, length, hci_driver_uart_dma_recv_callback, NULL);
    return 0;
}

int hci_driver_uart_dma_tx_start(esp_bt_hci_tl_callback_t callback, void *arg)
{
    void *data;
    bool last_frame;
    bool head_is_setted;
    uint32_t tx_len;
    uhci_lldesc_t *lldesc_data;
    uhci_lldesc_t *lldesc_head;
    uhci_lldesc_t *lldesc_tail;

    lldesc_head = NULL;
    lldesc_tail = NULL;
    head_is_setted = false;
    last_frame = false;
    while (true) {
        tx_len = hci_driver_util_tx_list_dequeue(0xffffff, &data, &last_frame);
        if (!tx_len) {
            break;
        }

        lldesc_data = os_memblock_get(&s_hci_driver_uart_dma_env.lldesc_mem_pool);
        /* According to the current processing logic， It should not be empty */
        assert(lldesc_data);
        memset(lldesc_data, 0, sizeof(uhci_lldesc_t));
        lldesc_data->length = tx_len;
        lldesc_data->buf = data;
        lldesc_data->eof = 0;
        if (!head_is_setted) {
            lldesc_head = lldesc_data;
            head_is_setted = true;
        } else {
            lldesc_tail->qe.stqe_next = lldesc_data;
        }

        lldesc_tail = lldesc_data;
        if (last_frame) {
            break;
        }
    }

    if (lldesc_head) {
        lldesc_tail->eof = 1;
        uart_env.tx.link_head = lldesc_head;
        uart_env.tx.callback = callback;
        uart_env.tx.arg = arg;
        /* The DMA interrupt may have been triggered before setting the tx_state,
         * So we set it first.
         */
        hci_driver_uart_dma_txstate_set(HCI_TRANS_TX_START);
        gdma_start(s_tx_channel, (intptr_t)(uart_env.tx.link_head));
        return 0;
    } else {
        return -1;
    }
}

static void hci_driver_uart_dma_install(void)
{
    periph_module_enable(PERIPH_UHCI0_MODULE);
    periph_module_reset(PERIPH_UHCI0_MODULE);
    // install DMA driver
    gdma_channel_alloc_config_t tx_channel_config = {
        .flags.reserve_sibling = 1,
        .direction = GDMA_CHANNEL_DIRECTION_TX,
    };

    ESP_ERROR_CHECK(gdma_new_channel(&tx_channel_config, &s_tx_channel));
    gdma_channel_alloc_config_t rx_channel_config = {
        .direction = GDMA_CHANNEL_DIRECTION_RX,
        .sibling_chan = s_tx_channel,
    };

    ESP_ERROR_CHECK(gdma_new_channel(&rx_channel_config, &s_rx_channel));
    gdma_connect(s_tx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_UHCI, 0));
    gdma_connect(s_rx_channel, GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_UHCI, 0));
    gdma_strategy_config_t strategy_config = {
        .auto_update_desc = false,
        .owner_check = false
    };

    gdma_apply_strategy(s_tx_channel, &strategy_config);
    gdma_apply_strategy(s_rx_channel, &strategy_config);
    gdma_rx_event_callbacks_t rx_cbs = {
        .on_recv_eof = hci_uart_tl_rx_eof_callback
    };

    gdma_register_rx_event_callbacks(s_rx_channel, &rx_cbs, NULL);
    gdma_tx_event_callbacks_t tx_cbs = {
        .on_trans_eof = hci_uart_tl_tx_eof_callback
    };

    gdma_register_tx_event_callbacks(s_tx_channel, &tx_cbs, NULL);
    // configure UHCI
    uhci_ll_init((uhci_dev_t *)s_uhci_hw);
    // uhci_ll_set_eof_mode((uhci_dev_t *)s_uhci_hw, UHCI_RX_LEN_EOF);
    uhci_ll_set_eof_mode((uhci_dev_t *)s_uhci_hw, UHCI_RX_IDLE_EOF);
    // disable software flow control
    s_uhci_hw->escape_conf.val = 0;
    uhci_ll_attach_uart_port((uhci_dev_t *)s_uhci_hw, s_hci_driver_uart_dma_env.hci_uart_params->hci_uart_port);
}

static int
hci_driver_uart_dma_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length,
                   hci_driver_direction_t dir)
{
    /* By now, this layer is only used by controller. */
    assert(dir == HCI_DRIVER_DIR_C2H);
    ESP_LOGD(TAG, "dma tx:");
    ESP_LOG_BUFFER_HEXDUMP(TAG, data, length, ESP_LOG_DEBUG);

    hci_driver_util_tx_list_enqueue(data_type, data, length);
    xSemaphoreGive(s_hci_driver_uart_dma_env.process_sem);
    return 0;
}

static int
hci_driver_uart_dma_h4_frame_cb(uint8_t pkt_type, void *data)
{
    hci_driver_forward_fn *forward_cb;
    forward_cb = s_hci_driver_uart_dma_env.forward_cb;
    if (!forward_cb) {
        return -1;
    }
    ESP_LOGD(TAG, "h4 frame\n");
    return forward_cb(pkt_type, data, 0, HCI_DRIVER_DIR_H2C);
}

static void
hci_driver_uart_dma_process_task(void *p)
{
    hci_message_t *rxinfo_container;
    os_sr_t sr;
    int ret;
    uint8_t* rx_data;
    uint32_t rx_len;

    while (true) {
        xSemaphoreTake(s_hci_driver_uart_dma_env.process_sem, portMAX_DELAY);
        ESP_LOGD(TAG, "task run:%d\n",s_hci_driver_uart_dma_env.hci_tx_state);
        /* Process Tx data */
        if (s_hci_driver_uart_dma_env.hci_tx_state == HCI_TRANS_TX_IDLE) {
            hci_driver_uart_dma_tx_start(hci_driver_uart_dma_send_callback, (void*)&uart_env);
        }

        if (s_hci_driver_uart_dma_env.rxinfo_mem_exhausted) {
            rx_data = (void *)uart_env.rx.link_head->buf;
            rx_len = uart_env.rx.link_head->length;
            ESP_LOGD(TAG, "rxinfo exhausted:");
            ESP_LOG_BUFFER_HEXDUMP(TAG, rx_data, rx_len, ESP_LOG_DEBUG);
            ret  = hci_h4_sm_rx(s_hci_driver_uart_dma_env.h4_sm, rx_data, rx_len);
            hci_driver_uart_dma_rx_start(rx_data, HCI_RX_DATA_BLOCK_SIZE);
            hci_driver_uart_dma_rxinfo_mem_exhausted_set(false);
            if (ret < 0) {
               ESP_LOGW(TAG, "parse rx data error!\n");
               r_ble_ll_hci_ev_hw_err(ESP_HCI_SYNC_LOSS_ERR);
            }
        }

        while (!STAILQ_EMPTY(&g_hci_rxinfo_head)) {
            OS_ENTER_CRITICAL(sr);
            rxinfo_container = STAILQ_FIRST(&g_hci_rxinfo_head);
            STAILQ_REMOVE_HEAD(&g_hci_rxinfo_head, next);
            OS_EXIT_CRITICAL(sr);

            rx_data = rxinfo_container->ptr;
            rx_len = rxinfo_container->length;
            ESP_LOGD(TAG, "uart rx");
            ESP_LOG_BUFFER_HEXDUMP(TAG, rx_data, rx_len, ESP_LOG_DEBUG);
            ret  = hci_h4_sm_rx(s_hci_driver_uart_dma_env.h4_sm, rx_data, rx_len);
            if (ret < 0) {
                ESP_LOGW(TAG, "parse rx data error!\n");
                r_ble_ll_hci_ev_hw_err(ESP_HCI_SYNC_LOSS_ERR);
            }

            os_memblock_put(s_hci_driver_uart_dma_env.hci_rxinfo_pool, rxinfo_container);
            /* No need to enter CRITICAL */
            if (s_hci_driver_uart_dma_env.is_continue_rx) {
                /* We should set continux rx flag first, RX interrupted may happened when rx start soon */
                hci_driver_uart_dma_continue_rx_enable(false);
                hci_driver_uart_dma_rx_start(rx_data, HCI_RX_DATA_BLOCK_SIZE);
            } else {
                os_memblock_put(s_hci_driver_uart_dma_env.hci_rx_data_pool, rx_data);
            }
        }
    }
}

static int
hci_driver_uart_dma_task_create(void)
{
    /* !TODO: Set the core id by menuconfig */
    xTaskCreatePinnedToCore(hci_driver_uart_dma_process_task, "hci_driver_uart_dma_process_task",
                            CONFIG_BT_LE_HCI_TRANS_TASK_STACK_SIZE, NULL,
                            ESP_TASK_BT_CONTROLLER_PRIO, &s_hci_driver_uart_dma_env.task_handler,
                            0);
    assert(s_hci_driver_uart_dma_env.task_handler);

    ESP_LOGI(TAG, "hci transport task create successfully, prio:%d, stack size: %ld",
             ESP_TASK_BT_CONTROLLER_PRIO, CONFIG_BT_LE_HCI_TRANS_TASK_STACK_SIZE);

    return 0;
}


static void
hci_driver_uart_dma_deinit(void)
{
    if (s_hci_driver_uart_dma_env.task_handler) {
        vTaskDelete(s_hci_driver_uart_dma_env.task_handler);
        s_hci_driver_uart_dma_env.task_handler = NULL;
    }

    ESP_ERROR_CHECK(uart_driver_delete(s_hci_driver_uart_dma_env.hci_uart_params->hci_uart_port));
    hci_driver_uart_dma_memory_deinit();
    if (!s_hci_driver_uart_dma_env.process_sem) {
        vSemaphoreDelete(s_hci_driver_uart_dma_env.process_sem);
    }

    hci_driver_util_deinit();
    memset(&s_hci_driver_uart_dma_env, 0, sizeof(hci_driver_uart_dma_env_t));
}


static int
hci_driver_uart_dma_init(hci_driver_forward_fn *cb)
{
    int rc;

    memset(&s_hci_driver_uart_dma_env, 0, sizeof(hci_driver_uart_dma_env_t));

    s_hci_driver_uart_dma_env.h4_sm = &s_hci_driver_uart_h4_sm;
    hci_h4_sm_init(s_hci_driver_uart_dma_env.h4_sm, &s_hci_driver_mem_alloc, &s_hci_driver_mem_free, hci_driver_uart_dma_h4_frame_cb);

    rc = hci_driver_util_init();
    if (rc) {
        goto error;
    }

    s_hci_driver_uart_dma_env.process_sem = xSemaphoreCreateBinary();
    if (!s_hci_driver_uart_dma_env.process_sem) {
        goto error;
    }

    rc = hci_driver_uart_dma_memory_init();
    if (rc) {
        goto error;
    }

    s_hci_driver_uart_dma_env.forward_cb = cb;
    s_hci_driver_uart_dma_env.hci_uart_params = hci_driver_uart_config_param_get();
    hci_driver_uart_config(s_hci_driver_uart_dma_env.hci_uart_params);

    ESP_LOGI(TAG, "uart attach uhci!");
    hci_driver_uart_dma_install();

    STAILQ_INIT(&g_hci_rxinfo_head);

    rc = hci_driver_uart_dma_task_create();
    if (rc) {
        goto error;
    }

    s_hci_driver_uart_dma_env.hci_tx_state = HCI_TRANS_TX_IDLE;
    s_hci_driver_uart_dma_env.rxinfo_mem_exhausted = false;
    s_hci_driver_uart_dma_env.is_continue_rx = false;
    hci_driver_uart_dma_rx_start(os_memblock_get(s_hci_driver_uart_dma_env.hci_rx_data_pool),
                                HCI_RX_DATA_BLOCK_SIZE);
    return 0;

error:
    hci_driver_uart_dma_deinit();
    return rc;
}

int
hci_driver_uart_dma_reconfig_pin(int tx_pin, int rx_pin, int cts_pin, int rts_pin)
{
    return hci_driver_uart_pin_update(tx_pin, rx_pin, cts_pin, rts_pin);
}


hci_driver_ops_t hci_driver_uart_dma_ops = {
    .hci_driver_tx = hci_driver_uart_dma_tx,
    .hci_driver_init = hci_driver_uart_dma_init,
    .hci_driver_deinit = hci_driver_uart_dma_deinit,
};
