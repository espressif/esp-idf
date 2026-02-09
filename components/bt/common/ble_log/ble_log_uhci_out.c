/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ble_log/ble_log_uhci_out.h"


#if CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED

// Private includes
#include "esp_bt.h"

// sdkconfig defines
#define UHCI_OUT_LL_TASK_BUF_SIZE               CONFIG_BT_BLE_LOG_UHCI_OUT_LL_TASK_BUF_SIZE
#define UHCI_OUT_LL_ISR_BUF_SIZE                CONFIG_BT_BLE_LOG_UHCI_OUT_LL_ISR_BUF_SIZE
#define UHCI_OUT_LL_HCI_BUF_SIZE                CONFIG_BT_BLE_LOG_UHCI_OUT_LL_HCI_BUF_SIZE
#define UHCI_OUT_UART_PORT                      CONFIG_BT_BLE_LOG_UHCI_OUT_UART_PORT
#define UHCI_OUT_UART_NEED_INIT                 CONFIG_BT_BLE_LOG_UHCI_OUT_UART_NEED_INIT

#if UHCI_OUT_UART_NEED_INIT
#define UHCI_OUT_UART_BAUD_RATE                 CONFIG_BT_BLE_LOG_UHCI_OUT_UART_BAUD_RATE
#define UHCI_OUT_UART_IO_NUM_TX                 CONFIG_BT_BLE_LOG_UHCI_OUT_UART_IO_NUM_TX
#endif // UHCI_OUT_UART_NEED_INIT

// Private defines
#define UHCI_OUT_MAX_TRANSFER_SIZE              (10240)
#define UHCI_OUT_MALLOC(size)                   heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#define UHCI_OUT_FLUSH_TIMEOUT_MS               (100)
#define UHCI_OUT_FLUSH_TIMEOUT_US               (UHCI_OUT_FLUSH_TIMEOUT_MS * 1000)
#define UHCI_OUT_USER_BUF_SIZE                  (512)
#define UHCI_OUT_UART_PORT0                     (0)
#define UHCI_OUT_UART_PORT1                     (1)
#define UHCI_OUT_UART_DRIVER_RX_BUF_SIZE        (32)

// Queue size defines
#define UHCI_OUT_PING_PONG_BUF_CNT              (2)
#define UHCI_OUT_USER_QUEUE_SIZE                (UHCI_OUT_PING_PONG_BUF_CNT)
#define UHCI_OUT_LL_QUEUE_SIZE                  (3 * UHCI_OUT_PING_PONG_BUF_CNT)
#define UHCI_OUT_QUEUE_SIZE                     (UHCI_OUT_USER_QUEUE_SIZE + UHCI_OUT_LL_QUEUE_SIZE)

#if CONFIG_SOC_ESP_NIMBLE_CONTROLLER
#include "os/os_mbuf.h"
#endif /* CONFIG_SOC_ESP_NIMBLE_CONTROLLER */

// Private typedefs
typedef struct {
    // This flag is for multithreading, must be a word, do not modify
    volatile uint32_t flag;
    uint16_t buf_size;
    uint16_t length;
    uint8_t buffer[0];
} uhci_out_trans_cb_t;

typedef struct {
    uhci_out_trans_cb_t *trans_cb[2];
    uint8_t trans_cb_idx;
    uint8_t type;
    uint16_t lost_frame_cnt;
    uint32_t lost_bytes_cnt;
    uint32_t frame_sn;
} uhci_out_log_cb_t;

typedef struct {
    uint16_t length;
    uint8_t source;
    uint8_t type;
    uint16_t frame_sn;
} __attribute__((packed)) frame_head_t;

typedef struct {
    uint8_t type;
    uint16_t lost_frame_cnt;
    uint32_t lost_bytes_cnt;
} __attribute__((packed)) loss_payload_t;

// Private enums
enum {
    TRANS_CB_FLAG_AVAILABLE = 0,
    TRANS_CB_FLAG_NEED_QUEUE,
    TRANS_CB_FLAG_IN_QUEUE,
};

enum {
    LOG_CB_TYPE_USER = 0,
    LOG_CB_TYPE_LL,
};

enum {
    LOG_CB_LL_SUBTYPE_TASK = 0,
    LOG_CB_LL_SUBTYPE_ISR,
    LOG_CB_LL_SUBTYPE_HCI
};

enum {
    LL_LOG_FLAG_CONTINUE = 0,
    LL_LOG_FLAG_END,
    LL_LOG_FLAG_TASK,
    LL_LOG_FLAG_ISR,
    LL_LOG_FLAG_HCI,
    LL_LOG_FLAG_RAW,
    LL_LOG_FLAG_OMDATA,
    LL_LOG_FLAG_HCI_UPSTREAM,
};

enum {
    LL_EV_FLAG_ISR_APPEND = 0,
    LL_EV_FLAG_FLUSH_LOG,
};

// Private variables
static bool uhci_out_inited = false;
static uhci_controller_handle_t uhci_handle = NULL;

static bool user_log_inited = false;
static SemaphoreHandle_t user_log_mutex = NULL;
static uhci_out_log_cb_t *user_log_cb = NULL;
static uint32_t user_last_write_ts = 0;

static bool ll_log_inited = false;
static uhci_out_log_cb_t *ll_task_log_cb = NULL;
static uhci_out_log_cb_t *ll_isr_log_cb = NULL;
static uhci_out_log_cb_t *ll_hci_log_cb = NULL;
static uint32_t ll_ev_flags = 0;
static uint32_t ll_last_write_ts = 0;

static esp_timer_handle_t flush_timer = NULL;

// Private function declarations
extern void esp_panic_handler_feed_wdts(void);

static int uhci_out_init_trans(uhci_out_trans_cb_t **trans_cb, uint16_t buf_size);
static void uhci_out_deinit_trans(uhci_out_trans_cb_t **trans_cb);
static bool uhci_out_tx_done_cb(uhci_controller_handle_t uhci_ctrl,
                                const uhci_tx_done_event_data_t *edata, void *user_ctx);
static inline void uhci_out_append_trans(uhci_out_trans_cb_t *trans_cb);

static int uhci_out_log_cb_init(uhci_out_log_cb_t **log_cb, uint16_t buf_size, uint8_t type, uint8_t idx);
static void uhci_out_log_cb_deinit(uhci_out_log_cb_t **log_cb);
static inline bool uhci_out_log_cb_check_trans(uhci_out_log_cb_t *log_cb, uint16_t len, bool *need_append);
static inline void uhci_out_log_cb_append_trans(uhci_out_log_cb_t *log_cb);
static inline void uhci_out_log_cb_flush_trans(uhci_out_log_cb_t *log_cb);
static bool uhci_out_log_cb_write(uhci_out_log_cb_t *log_cb, const uint8_t *addr, uint16_t len,
                                 const uint8_t *addr_append, uint16_t len_append, uint8_t source, bool omdata);
static void uhci_out_log_cb_write_loss(uhci_out_log_cb_t *log_cb);
static void uhci_out_log_cb_dump(uhci_out_log_cb_t *log_cb);

static void esp_timer_cb_log_flush(void);
static void uhci_out_user_write_str(const uint8_t *src, uint16_t len);

#if UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0
static void uhci_out_user_write_char(char c);
#endif // UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0

static int uhci_out_user_log_init(void);
static void uhci_out_user_log_deinit(void);

static int uhci_out_ll_log_init(void);
static void uhci_out_ll_log_deinit(void);
static void uhci_out_ll_log_flush(void);

#if defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) ||\
    defined(CONFIG_IDF_TARGET_ESP32C61) || defined(CONFIG_IDF_TARGET_ESP32H21)
extern void r_ble_log_simple_put_ev(void);
#define UHCI_OUT_LL_PUT_EV r_ble_log_simple_put_ev()
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
extern void ble_log_simple_put_ev(void);
#define UHCI_OUT_LL_PUT_EV ble_log_simple_put_ev()
#else
#define UHCI_OUT_LL_PUT_EV
#endif

// Private macros
#define UHCI_OUT_FRAME_HEAD_LEN     (sizeof(frame_head_t))
#define UHCI_OUT_FRAME_TAIL_LEN     (sizeof(uint32_t))
#define UHCI_OUT_FRAME_OVERHEAD     (UHCI_OUT_FRAME_HEAD_LEN + UHCI_OUT_FRAME_TAIL_LEN)
#define UHCI_OUT_GET_FRAME_SN(VAR)  __atomic_fetch_add(VAR, 1, __ATOMIC_RELAXED)

// Private functions
static int uhci_out_init_trans(uhci_out_trans_cb_t **trans_cb, uint16_t buf_size)
{
    // Memory allocations
    size_t cb_size = sizeof(uhci_out_trans_cb_t) + buf_size;
    *trans_cb = (uhci_out_trans_cb_t *)UHCI_OUT_MALLOC(cb_size);
    if (!(*trans_cb)) {
        return -1;
    }
    memset(*trans_cb, 0, sizeof(uhci_out_trans_cb_t));

    // Initialization
    (*trans_cb)->buf_size = buf_size;
    return 0;
}

static void uhci_out_deinit_trans(uhci_out_trans_cb_t **trans_cb)
{
    if (!(*trans_cb)) {
        return;
    }

    free(*trans_cb);
    *trans_cb = NULL;
    return;
}

IRAM_ATTR static bool uhci_out_tx_done_cb(uhci_controller_handle_t uhci_ctrl,
                                          const uhci_tx_done_event_data_t *edata, void *user_ctx)
{
    uhci_out_trans_cb_t *trans_cb = (uhci_out_trans_cb_t *)((uint8_t *)edata->buffer - sizeof(uhci_out_trans_cb_t));
    trans_cb->length = 0;
    trans_cb->flag = TRANS_CB_FLAG_AVAILABLE;
    return true;
}

IRAM_ATTR static inline void uhci_out_append_trans(uhci_out_trans_cb_t *trans_cb)
{
    if ((trans_cb->flag != TRANS_CB_FLAG_NEED_QUEUE) || !trans_cb->length) {
        return;
    }

    // Note: If task yield after transmission but before flag set
    //       flag might be reset in tx done ISR before flag set, leading to buffer access failure
    trans_cb->flag = TRANS_CB_FLAG_IN_QUEUE;
    if (uhci_transmit(uhci_handle, trans_cb->buffer, trans_cb->length) != ESP_OK) {
        goto recycle;
    }
    return;

recycle:
    trans_cb->length = 0;
    trans_cb->flag = TRANS_CB_FLAG_AVAILABLE;
    return;
}

static int uhci_out_log_cb_init(uhci_out_log_cb_t **log_cb, uint16_t buf_size, uint8_t type, uint8_t idx)
{
    // Initialize log control block
    *log_cb = (uhci_out_log_cb_t *)UHCI_OUT_MALLOC(sizeof(uhci_out_log_cb_t));
    if (!(*log_cb)) {
        return -1;
    }
    memset(*log_cb, 0, sizeof(uhci_out_log_cb_t));

    // Initialize transactions
    int ret = 0;
    for (uint8_t i = 0; i < 2; i++) {
        ret |= uhci_out_init_trans(&((*log_cb)->trans_cb[i]), buf_size);
    }
    if (ret != 0) {
        uhci_out_log_cb_deinit(log_cb);
        return -1;
    }

    (*log_cb)->type = (type << 4) | (idx);
    return 0;
}

static void uhci_out_log_cb_deinit(uhci_out_log_cb_t **log_cb)
{
    if (!(*log_cb)) {
        return;
    }

    for (uint8_t i = 0; i < 2; i++) {
        if ((*log_cb)->trans_cb[i]) {
            uhci_out_deinit_trans(&((*log_cb)->trans_cb[i]));
        }
    }
    free(*log_cb);
    *log_cb = NULL;
    return;
}

IRAM_ATTR static inline bool uhci_out_log_cb_check_trans(uhci_out_log_cb_t *log_cb, uint16_t len, bool *need_append)
{
    uhci_out_trans_cb_t *trans_cb;
    *need_append = false;
    for (uint8_t i = 0; i < 2; i++) {
        trans_cb = log_cb->trans_cb[log_cb->trans_cb_idx];
        if (len > trans_cb->buf_size) {
            goto failed;
        }
        if (trans_cb->flag == TRANS_CB_FLAG_AVAILABLE) {
            if ((trans_cb->buf_size - trans_cb->length) >= len) {
                return true;
            } else {
                trans_cb->flag = TRANS_CB_FLAG_NEED_QUEUE;
                *need_append = true;
            }
        }
        log_cb->trans_cb_idx = !(log_cb->trans_cb_idx);
    }
failed:
    log_cb->lost_bytes_cnt += len;
    log_cb->lost_frame_cnt++;
    return false;
}

// CRITICAL: Shall not be called from ISR!
IRAM_ATTR static inline void uhci_out_log_cb_append_trans(uhci_out_log_cb_t *log_cb)
{
    uhci_out_trans_cb_t *trans_cb;
    uint8_t idx = !log_cb->trans_cb_idx;
    for (uint8_t i = 0; i < 2; i++) {
        trans_cb = log_cb->trans_cb[idx];
        if (trans_cb->flag == TRANS_CB_FLAG_NEED_QUEUE) {
            uhci_out_append_trans(trans_cb);
        }
        idx = !idx;
    }
}

IRAM_ATTR static inline void uhci_out_log_cb_flush_trans(uhci_out_log_cb_t *log_cb)
{
    uhci_out_trans_cb_t *trans_cb;
    for (uint8_t i = 0; i < 2; i++) {
        trans_cb = log_cb->trans_cb[i];
        if (trans_cb->length && (trans_cb->flag == TRANS_CB_FLAG_AVAILABLE)) {
            trans_cb->flag = TRANS_CB_FLAG_NEED_QUEUE;
        }
    }
}

// Return value: Need append
IRAM_ATTR static bool uhci_out_log_cb_write(uhci_out_log_cb_t *log_cb, const uint8_t *addr, uint16_t len,
                                           const uint8_t *addr_append, uint16_t len_append, uint8_t source, bool omdata)
{
    uhci_out_trans_cb_t *trans_cb = log_cb->trans_cb[log_cb->trans_cb_idx];

    uint8_t *buf = trans_cb->buffer + trans_cb->length;
    uint16_t total_length = len + len_append;
    frame_head_t head = {
        .length = total_length,
        .source = source,
        .type = log_cb->type,
        .frame_sn = UHCI_OUT_GET_FRAME_SN(&(log_cb->frame_sn)) & 0xFFFF,
    };

    memcpy(buf, (const uint8_t *)&head, UHCI_OUT_FRAME_HEAD_LEN);
    memcpy(buf + UHCI_OUT_FRAME_HEAD_LEN, addr, len);
    if (len_append && addr_append) {
#if CONFIG_SOC_ESP_NIMBLE_CONTROLLER
        if (omdata) {
            os_mbuf_copydata((struct os_mbuf *)addr_append, 0,
                             len_append, buf + UHCI_OUT_FRAME_HEAD_LEN + len);
        }
        else
#endif /* CONFIG_SOC_ESP_NIMBLE_CONTROLLER */
        {
            memcpy(buf + UHCI_OUT_FRAME_HEAD_LEN + len, addr_append, len_append);
        }
    }

    uint32_t checksum = 0;
    for (int i = 0; i < UHCI_OUT_FRAME_HEAD_LEN + total_length; i++) {
        checksum += buf[i];
    }
    memcpy(buf + UHCI_OUT_FRAME_HEAD_LEN + total_length, &checksum, UHCI_OUT_FRAME_TAIL_LEN);

    trans_cb->length += total_length + UHCI_OUT_FRAME_OVERHEAD;
    if ((trans_cb->buf_size - trans_cb->length) <= UHCI_OUT_FRAME_OVERHEAD) {
        trans_cb->flag = TRANS_CB_FLAG_NEED_QUEUE;
        return true;
    }
    return false;
}

IRAM_ATTR static void uhci_out_log_cb_write_loss(uhci_out_log_cb_t *log_cb)
{
    if (!log_cb->lost_bytes_cnt || !log_cb->lost_frame_cnt) {
        return;
    }
    bool need_append;
    uint16_t frame_len = sizeof(loss_payload_t) + UHCI_OUT_FRAME_OVERHEAD;
    if (uhci_out_log_cb_check_trans(log_cb, frame_len, &need_append)) {
        loss_payload_t payload = {
            .type = log_cb->type,
            .lost_frame_cnt = log_cb->lost_frame_cnt,
            .lost_bytes_cnt = log_cb->lost_bytes_cnt,
        };
        uhci_out_log_cb_write(log_cb, (const uint8_t *)&payload, sizeof(loss_payload_t),
                             NULL, 0, BLE_LOG_UHCI_OUT_SOURCE_LOSS, false);

        log_cb->lost_frame_cnt = 0;
        log_cb->lost_bytes_cnt = 0;
    }
}

static void uhci_out_log_cb_dump(uhci_out_log_cb_t *log_cb)
{
    uhci_out_trans_cb_t *trans_cb;
    uint8_t *buf;
    for (uint8_t i = 0; i < 2; i++) {
        // Dump the last transaction before dumping the current transaction
        log_cb->trans_cb_idx = !(log_cb->trans_cb_idx);
        trans_cb = log_cb->trans_cb[log_cb->trans_cb_idx];
        buf = (uint8_t *)trans_cb->buffer;
        for (uint16_t j = 0; j < trans_cb->buf_size; j++) {
            esp_rom_printf("%02x ", buf[j]);

            // Feed watchdogs periodically to avoid wdts timeout
            if ((j % 100) == 0) {
                esp_panic_handler_feed_wdts();
            }
        }
    }
}

static void esp_timer_cb_log_flush(void)
{
    uint32_t os_ts = pdTICKS_TO_MS(xTaskGetTickCount());

    if ((os_ts - user_last_write_ts) > UHCI_OUT_FLUSH_TIMEOUT_MS) {
        xSemaphoreTake(user_log_mutex, portMAX_DELAY);
        uhci_out_log_cb_flush_trans(user_log_cb);
        uhci_out_log_cb_append_trans(user_log_cb);
        xSemaphoreGive(user_log_mutex);
    }

    if ((esp_bt_controller_get_status() >= ESP_BT_CONTROLLER_STATUS_INITED) &&
        ((os_ts - ll_last_write_ts) > UHCI_OUT_FLUSH_TIMEOUT_MS)) {
        ll_ev_flags |= BIT(LL_EV_FLAG_FLUSH_LOG);
        UHCI_OUT_LL_PUT_EV;
    }

    esp_timer_start_once(flush_timer, UHCI_OUT_FLUSH_TIMEOUT_US);
}

static void uhci_out_user_write_str(const uint8_t *src, uint16_t len)
{
    if (!user_log_inited || !src || !len) {
        return;
    }

    xSemaphoreTake(user_log_mutex, portMAX_DELAY);

    bool need_append;
    if (uhci_out_log_cb_check_trans(user_log_cb, len, &need_append)) {
        uhci_out_trans_cb_t *trans_cb = user_log_cb->trans_cb[user_log_cb->trans_cb_idx];
        uint8_t *buf = trans_cb->buffer + trans_cb->length;

        memcpy(buf, (const uint8_t *)src, len);
        trans_cb->length += len;
    }

    if (need_append) {
        uhci_out_log_cb_append_trans(user_log_cb);
    }

    user_last_write_ts = pdTICKS_TO_MS(xTaskGetTickCount());

    xSemaphoreGive(user_log_mutex);
}

#if UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0
static void uhci_out_user_write_char(char c)
{
    uhci_out_user_write_str((const uint8_t *)&c, 1);
}
#endif // UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0

static int uhci_out_user_log_init(void)
{
    if (user_log_inited) {
        return 0;
    }

    // Initialize mutex
    user_log_mutex = xSemaphoreCreateMutex();
    if (!user_log_mutex) {
        goto failed;
    }

    // Initialize log control block
    if (uhci_out_log_cb_init(&user_log_cb, UHCI_OUT_USER_BUF_SIZE, LOG_CB_TYPE_USER, 0) != 0) {
        goto failed;
    }

    // Initialization done
    user_log_inited = true;
    return 0;

failed:
    uhci_out_user_log_deinit();
    return -1;
}

static void uhci_out_user_log_deinit(void)
{
    user_log_inited = false;

    if (!user_log_mutex) {
        return;
    }
    xSemaphoreTake(user_log_mutex, portMAX_DELAY);

    uhci_out_log_cb_deinit(&user_log_cb);

    xSemaphoreGive(user_log_mutex);
    vSemaphoreDelete(user_log_mutex);
    user_log_mutex = NULL;
}

static int uhci_out_ll_log_init(void)
{
    if (ll_log_inited) {
        return 0;
    }

    if (uhci_out_log_cb_init(&ll_task_log_cb, UHCI_OUT_LL_TASK_BUF_SIZE,
                             LOG_CB_TYPE_LL, LOG_CB_LL_SUBTYPE_TASK) != 0) {
        goto failed;
    }
    if (uhci_out_log_cb_init(&ll_isr_log_cb, UHCI_OUT_LL_ISR_BUF_SIZE,
                             LOG_CB_TYPE_LL, LOG_CB_LL_SUBTYPE_ISR) != 0) {
        goto failed;
    }
    if (uhci_out_log_cb_init(&ll_hci_log_cb, UHCI_OUT_LL_HCI_BUF_SIZE,
                             LOG_CB_TYPE_LL, LOG_CB_LL_SUBTYPE_HCI) != 0) {
        goto failed;
    }

    ll_log_inited = true;
    return 0;

failed:
    uhci_out_ll_log_deinit();
    return -1;
}

static void uhci_out_ll_log_deinit(void)
{
    ll_log_inited = false;

    uhci_out_log_cb_deinit(&ll_hci_log_cb);
    uhci_out_log_cb_deinit(&ll_isr_log_cb);
    uhci_out_log_cb_deinit(&ll_task_log_cb);
}

static void uhci_out_ll_log_flush(void)
{
    if (!ll_log_inited) {
        return;
    }

    uhci_out_log_cb_write_loss(ll_task_log_cb);
    uhci_out_log_cb_write_loss(ll_hci_log_cb);

    uhci_out_log_cb_flush_trans(ll_task_log_cb);
    uhci_out_log_cb_flush_trans(ll_hci_log_cb);

    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);
    uhci_out_log_cb_write_loss(ll_isr_log_cb);
    uhci_out_log_cb_flush_trans(ll_isr_log_cb);
    portEXIT_CRITICAL_SAFE(&spinlock);

    uhci_out_log_cb_append_trans(ll_task_log_cb);
    uhci_out_log_cb_append_trans(ll_hci_log_cb);
    uhci_out_log_cb_append_trans(ll_isr_log_cb);
}

// Public functions
int ble_log_uhci_out_init(void)
{
    // Avoid double init
    if (uhci_out_inited) {
        return 0;
    }

#if UHCI_OUT_UART_NEED_INIT
    uart_config_t uart_config = {
        .baud_rate = UHCI_OUT_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    uart_param_config(UHCI_OUT_UART_PORT, &uart_config);
    uart_set_pin(UHCI_OUT_UART_PORT, UHCI_OUT_UART_IO_NUM_TX, -1, -1, -1);
#endif // UHCI_OUT_UART_NEED_INIT

    uhci_controller_config_t uhci_config = {
        .uart_port = UHCI_OUT_UART_PORT,
        .tx_trans_queue_depth = UHCI_OUT_QUEUE_SIZE,
        .max_receive_internal_mem = 1024,
        .max_transmit_size = UHCI_OUT_MAX_TRANSFER_SIZE,
        .dma_burst_size = 32,
        .rx_eof_flags.idle_eof = 1,
    };
    if (uhci_new_controller(&uhci_config, &uhci_handle) != ESP_OK) {
        goto failed;
    }

    uhci_event_callbacks_t uhci_cbs = {
        .on_tx_trans_done = uhci_out_tx_done_cb,
    };
    uhci_register_event_callbacks(uhci_handle, &uhci_cbs, NULL);

    if (uhci_out_user_log_init() != 0) {
        goto failed;
    }

    if (uhci_out_ll_log_init() != 0) {
        goto failed;
    }

    esp_timer_create_args_t timer_args = {
        .callback = (esp_timer_cb_t)esp_timer_cb_log_flush,
        .dispatch_method = ESP_TIMER_TASK
    };
    if (esp_timer_create(&timer_args, &flush_timer) != ESP_OK) {
        goto failed;
    }

#if UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0
    // Install UART Driver if not installed
    if (!uart_is_driver_installed(UHCI_OUT_UART_PORT0)) {
        uart_driver_install(UHCI_OUT_UART_PORT0, UHCI_OUT_UART_DRIVER_RX_BUF_SIZE, 0, 0, NULL, 0);
    }

    // Redirect UART VFS Driver to UART Driver
    uart_vfs_dev_use_driver(UHCI_OUT_UART_PORT0);

    // Redirect esp_rom_printf to BLE Log UHCI Out
    esp_rom_install_channel_putc(1, uhci_out_user_write_char);
    esp_rom_install_channel_putc(2, NULL);
#endif // UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0

    uhci_out_inited = true;
    esp_timer_start_once(flush_timer, UHCI_OUT_FLUSH_TIMEOUT_US);
    return 0;

failed:
    ble_log_uhci_out_deinit();
    return -1;
}

void ble_log_uhci_out_deinit(void)
{
    uhci_out_inited = false;

    if (flush_timer) {
        esp_timer_stop(flush_timer);
        esp_timer_delete(flush_timer);
        flush_timer = NULL;
    }

    if (uhci_handle) {
        uhci_wait_all_tx_transaction_done(uhci_handle, portMAX_DELAY);
        uhci_del_controller(uhci_handle);
        uhci_handle = NULL;
    }

    uhci_out_ll_log_deinit();
    uhci_out_user_log_deinit();
}

IRAM_ATTR void ble_log_uhci_out_ll_write(uint32_t len, const uint8_t *addr, uint32_t len_append,
                                        const uint8_t *addr_append, uint32_t flag)
{
    // Raw logs will come in case of assert, shall be printed to console directly
    if (flag & BIT(LL_LOG_FLAG_RAW)) {
        if (len && addr) {
            for (uint32_t i = 0; i < len; i++) { esp_rom_printf("%02x ", addr[i]); }
        }
        if (len_append && addr_append) {
            for (uint32_t i = 0; i < len_append; i++) { esp_rom_printf("%02x ", addr_append[i]); }
        }
        if (flag & BIT(LL_LOG_FLAG_END)) { esp_rom_printf("\n"); }
    }

    if (!ll_log_inited) {
        return;
    }

    bool in_isr = false;
    uint8_t source;
    uhci_out_log_cb_t *log_cb;
    if (flag & BIT(LL_LOG_FLAG_ISR)) {
        log_cb = ll_isr_log_cb;
        source = BLE_LOG_UHCI_OUT_SOURCE_ESP_ISR;
        in_isr = true;
    } else if (flag & BIT(LL_LOG_FLAG_HCI)) {
        log_cb = ll_hci_log_cb;
        source = BLE_LOG_UHCI_OUT_SOURCE_LL_HCI;
    } else {
        log_cb = ll_task_log_cb;
        source = BLE_LOG_UHCI_OUT_SOURCE_ESP;
    }
    bool omdata = flag & BIT(LL_LOG_FLAG_OMDATA);

    bool need_append;
    uint16_t frame_len = len + len_append + UHCI_OUT_FRAME_OVERHEAD;
    if (uhci_out_log_cb_check_trans(log_cb, frame_len, &need_append)) {
        need_append |= uhci_out_log_cb_write(log_cb, addr, len, addr_append,
                                            len_append, source, omdata);
    }

    ll_last_write_ts = in_isr?\
                       pdTICKS_TO_MS(xTaskGetTickCountFromISR()):\
                       pdTICKS_TO_MS(xTaskGetTickCount());

    if (need_append) {
        if (in_isr) {
            ll_ev_flags |= BIT(LL_EV_FLAG_ISR_APPEND);
            UHCI_OUT_LL_PUT_EV;
        } else {
            uhci_out_log_cb_append_trans(log_cb);
        }
    }
}

IRAM_ATTR void ble_log_uhci_out_ll_log_ev_proc(void)
{
    if (!ll_log_inited) {
        return;
    }

    if (ll_ev_flags & BIT(LL_EV_FLAG_ISR_APPEND)) {
        uhci_out_log_cb_append_trans(ll_isr_log_cb);
        ll_ev_flags &= ~BIT(LL_EV_FLAG_ISR_APPEND);
    }

    if (ll_ev_flags & BIT(LL_EV_FLAG_FLUSH_LOG)) {
        uhci_out_ll_log_flush();
        ll_ev_flags &= ~BIT(LL_EV_FLAG_FLUSH_LOG);
    }

    ll_ev_flags = 0;
}

// Redirect UART Driver to BLE Log UHCI Out
int __real_uart_tx_chars(uart_port_t uart_num, const char *buffer, uint32_t len);
int __wrap_uart_tx_chars(uart_port_t uart_num, const char *buffer, uint32_t len)
{
#if UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0
    uhci_out_user_write_str((const uint8_t *)buffer, len);
    return 0;
#else
    return __real_uart_tx_chars(uart_num, buffer, len);
#endif // UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0
}

int __real_uart_write_bytes(uart_port_t uart_num, const void *src, size_t size);
int __wrap_uart_write_bytes(uart_port_t uart_num, const void *src, size_t size)
{
#if UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0
    uhci_out_user_write_str((const uint8_t *)src, size);
    return 0;
#else
    return __real_uart_write_bytes(uart_num, src, size);
#endif // UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0
}

int __real_uart_write_bytes_with_break(uart_port_t uart_num, const void *src, size_t size, int brk_len);
int __wrap_uart_write_bytes_with_break(uart_port_t uart_num, const void *src, size_t size, int brk_len)
{
#if UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0
    return __wrap_uart_write_bytes(uart_num, src, size);
#else
    return __real_uart_write_bytes_with_break(uart_num, src, size, brk_len);
#endif // UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0
}

void ble_log_uhci_out_dump_all(void)
{
    if (!uhci_out_inited) {
        return;
    }

#if UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0
    esp_rom_output_tx_wait_idle(UHCI_OUT_UART_PORT0);
    esp_rom_install_uart_printf();
#endif // UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0

    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);

    if (ll_log_inited) {
        esp_rom_printf("[DUMP_START:\n");
        uhci_out_log_cb_dump(ll_isr_log_cb);
        uhci_out_log_cb_dump(ll_task_log_cb);
        uhci_out_log_cb_dump(ll_hci_log_cb);
        esp_rom_printf("\n:DUMP_END]\n\n");
    }
    portEXIT_CRITICAL_SAFE(&spinlock);

#if UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0
    esp_rom_install_channel_putc(1, uhci_out_user_write_char);
#endif // UHCI_OUT_UART_PORT == UHCI_OUT_UART_PORT0
}
#endif // CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED
