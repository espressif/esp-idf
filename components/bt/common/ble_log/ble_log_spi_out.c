/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ble_log/ble_log_spi_out.h"

#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

// Private includes
#include "esp_bt.h"

// sdkconfig defines
#define SPI_OUT_UL_TASK_BUF_SIZE                CONFIG_BT_BLE_LOG_SPI_OUT_UL_TASK_BUF_SIZE
#define SPI_OUT_LL_ENABLED                      CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED
#define SPI_OUT_LL_TASK_BUF_SIZE                CONFIG_BT_BLE_LOG_SPI_OUT_LL_TASK_BUF_SIZE
#define SPI_OUT_LL_ISR_BUF_SIZE                 CONFIG_BT_BLE_LOG_SPI_OUT_LL_ISR_BUF_SIZE
#define SPI_OUT_LL_HCI_BUF_SIZE                 CONFIG_BT_BLE_LOG_SPI_OUT_LL_HCI_BUF_SIZE
#define SPI_OUT_MOSI_IO_NUM                     CONFIG_BT_BLE_LOG_SPI_OUT_MOSI_IO_NUM
#define SPI_OUT_SCLK_IO_NUM                     CONFIG_BT_BLE_LOG_SPI_OUT_SCLK_IO_NUM
#define SPI_OUT_CS_IO_NUM                       CONFIG_BT_BLE_LOG_SPI_OUT_CS_IO_NUM
#define SPI_OUT_TS_SYNC_ENABLED                 CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
#define SPI_OUT_SYNC_IO_NUM                     CONFIG_BT_BLE_LOG_SPI_OUT_SYNC_IO_NUM
#define SPI_OUT_TS_SYNC_SLEEP_SUPPORT           CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_SLEEP_SUPPORT
#define SPI_OUT_FLUSH_TIMER_ENABLED             CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
#define SPI_OUT_FLUSH_TIMEOUT_US                (CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMEOUT * 1000)
#define SPI_OUT_LE_AUDIO_ENABLED                CONFIG_BT_BLE_LOG_SPI_OUT_LE_AUDIO_ENABLED
#define SPI_OUT_LE_AUDIO_BUF_SIZE               CONFIG_BT_BLE_LOG_SPI_OUT_LE_AUDIO_BUF_SIZE

// Private defines
#define BLE_LOG_TAG                             "BLE_LOG"
#define SPI_OUT_BUS                             SPI2_HOST
#define SPI_OUT_MAX_TRANSFER_SIZE               (10240)
#define SPI_OUT_FRAME_HEAD_LEN                  (4)
#define SPI_OUT_FRAME_TAIL_LEN                  (4)
#define SPI_OUT_FRAME_OVERHEAD                  (8)
#define SPI_OUT_PACKET_LOSS_FRAME_SIZE          (6)
#define SPI_OUT_TRANS_ITVL_MIN_US               (30)
#define SPI_OUT_UL_LOG_STR_BUF_SIZE             (100)

#if SPI_OUT_TS_SYNC_ENABLED
#define SPI_OUT_TS_SYNC_TIMEOUT                 (1000 * 1000)
#endif // SPI_OUT_TS_SYNC_ENABLED

// Queue size defines
#define SPI_OUT_PING_PONG_BUF_CNT               (2)
#define SPI_OUT_UL_QUEUE_SIZE                   (SPI_OUT_PING_PONG_BUF_CNT)

#if SPI_OUT_LL_ENABLED
#define SPI_OUT_LL_QUEUE_SIZE                   (3 * SPI_OUT_PING_PONG_BUF_CNT)
#else
#define SPI_OUT_LL_QUEUE_SIZE                   (0)
#endif // SPI_OUT_LL_ENABLED

#define SPI_OUT_SPI_MASTER_QUEUE_SIZE           (SPI_OUT_UL_QUEUE_SIZE +\
                                                 SPI_OUT_LL_QUEUE_SIZE)

// Private typedefs
typedef struct {
    // CRITICAL: 0 for available, 1 for need queue (ISR), 2 for in queue
    // This flag is for multithreading, must be a word, do not modify
    volatile uint32_t flag;
    uint16_t buf_size;
    uint16_t length;
    spi_transaction_t trans;
} spi_out_trans_cb_t;

typedef struct {
    spi_out_trans_cb_t *trans_cb[2];
    uint8_t trans_cb_idx;
    uint8_t frame_sn;
    uint16_t lost_frame_cnt;
    uint32_t lost_bytes_cnt;
    uint8_t type;
} spi_out_log_cb_t;

typedef struct {
    uint16_t length;
    uint8_t source;
    uint8_t frame_sn;
} __attribute__((packed)) frame_head_t;

typedef struct {
    uint8_t type;
    uint16_t lost_frame_cnt;
    uint32_t lost_bytes_cnt;
} __attribute__((packed)) loss_payload_t;

typedef struct {
    uint8_t io_level;
    uint32_t lc_ts;
    uint32_t esp_ts;
} __attribute__((packed)) ts_sync_data_t;

// Private enums
enum {
    TRANS_CB_FLAG_AVAILABLE = 0,
    TRANS_CB_FLAG_NEED_QUEUE,
    TRANS_CB_FLAG_IN_QUEUE,
};

enum {
    LOG_CB_TYPE_UL = 0,
    LOG_CB_TYPE_LL_TASK,
    LOG_CB_TYPE_LL_ISR,
    LOG_CB_TYPE_LL_HCI,
    LOG_CB_TYPE_LE_AUDIO,
};

enum {
    LL_LOG_FLAG_CONTINUE = 0,
    LL_LOG_FLAG_END,
    LL_LOG_FLAG_TASK,
    LL_LOG_FLAG_ISR,
    LL_LOG_FLAG_HCI,
    LL_LOG_FLAG_RAW,
};

enum {
    LL_EV_FLAG_ISR_APPEND = 0,
    LL_EV_FLAG_FLUSH_LOG,
};

// Private variables
static bool spi_out_inited = false;
static bool spi_out_enabled = false;
static spi_device_handle_t spi_handle = NULL;
static uint32_t last_tx_done_ts = 0;

static bool ul_log_inited = false;
static SemaphoreHandle_t ul_log_mutex = NULL;
static spi_out_log_cb_t *ul_log_cb = NULL;
static uint8_t *ul_log_str_buf = NULL;

#if SPI_OUT_LL_ENABLED
static bool ll_log_inited = false;
static spi_out_log_cb_t *ll_task_log_cb = NULL;
static spi_out_log_cb_t *ll_isr_log_cb = NULL;
static spi_out_log_cb_t *ll_hci_log_cb = NULL;
static uint32_t ll_ev_flags = 0;
#endif // SPI_OUT_LL_ENABLED

#if SPI_OUT_TS_SYNC_ENABLED
static bool ts_sync_inited = false;
static bool ts_sync_enabled = false;
static ts_sync_data_t ts_sync_data = {0};

#if !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
static esp_timer_handle_t ts_sync_timer = NULL;
#endif // !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
#endif // SPI_OUT_TS_SYNC_ENABLED

#if SPI_OUT_FLUSH_TIMER_ENABLED
static esp_timer_handle_t flush_timer = NULL;
#endif // SPI_OUT_FLUSH_TIMER_ENABLED

#if SPI_OUT_LE_AUDIO_ENABLED
static bool le_audio_log_inited = false;
static spi_out_log_cb_t *le_audio_log_cb = NULL;
#endif // SPI_OUT_LE_AUDIO_ENABLED

// Extern function declarations
extern void esp_panic_handler_feed_wdts(void);

// Private function declarations
static int spi_out_init_trans(spi_out_trans_cb_t **trans_cb, uint16_t buf_size);
static void spi_out_deinit_trans(spi_out_trans_cb_t **trans_cb);
static void spi_out_tx_done_cb(spi_transaction_t *ret_trans);
static inline void spi_out_append_trans(spi_out_trans_cb_t *trans_cb);

static int spi_out_log_cb_init(spi_out_log_cb_t **log_cb, uint16_t buf_size, uint8_t type);
static void spi_out_log_cb_deinit(spi_out_log_cb_t **log_cb);
static inline bool spi_out_log_cb_check_trans(spi_out_log_cb_t *log_cb, uint16_t len, bool *need_append);
static inline void spi_out_log_cb_append_trans(spi_out_log_cb_t *log_cb);
static inline void spi_out_log_cb_flush_trans(spi_out_log_cb_t *log_cb);
static bool spi_out_log_cb_write(spi_out_log_cb_t *log_cb, const uint8_t *addr, uint16_t len,
                                 const uint8_t *addr_append, uint16_t len_append, uint8_t source,
                                 bool with_checksum);
static void spi_out_log_cb_write_loss(spi_out_log_cb_t *log_cb);
static void spi_out_log_cb_dump(spi_out_log_cb_t *log_cb);
static void spi_out_log_flush(void);

static int spi_out_ul_log_init(void);
static void spi_out_ul_log_deinit(void);
static void spi_out_ul_log_write(uint8_t source, const uint8_t *addr, uint16_t len, bool with_ts);
static bool spi_out_ul_log_printf(uint8_t source, const char *format, va_list args, int offset);

#if SPI_OUT_LL_ENABLED
static int spi_out_ll_log_init(void);
static void spi_out_ll_log_deinit(void);
static void spi_out_ll_log_flush(void);

#if defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) ||\
    defined(CONFIG_IDF_TARGET_ESP32C61) || defined(CONFIG_IDF_TARGET_ESP32H21)
extern void r_ble_log_simple_put_ev(void);
#define SPI_OUT_LL_PUT_EV r_ble_log_simple_put_ev()
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
extern void ble_log_simple_put_ev(void);
#define SPI_OUT_LL_PUT_EV ble_log_simple_put_ev()
#else
#define SPI_OUT_LL_PUT_EV
#endif
#endif // SPI_OUT_LL_ENABLED

#if SPI_OUT_TS_SYNC_ENABLED
static int spi_out_ts_sync_init(void);
static void spi_out_ts_sync_deinit(void);
static void spi_out_ts_sync_enable(bool enable);
static void spi_out_ts_sync_toggle(void);

#if defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) ||\
    defined(CONFIG_IDF_TARGET_ESP32C61) || defined(CONFIG_IDF_TARGET_ESP32H21)
extern uint32_t r_ble_lll_timer_current_tick_get(void);
#define SPI_OUT_GET_LC_TIME r_ble_lll_timer_current_tick_get()
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
extern uint32_t r_os_cputime_get32(void);
#define SPI_OUT_GET_LC_TIME r_os_cputime_get32()
#else
#define SPI_OUT_GET_LC_TIME 0
#endif

#if !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
static void esp_timer_cb_ts_sync(void);
#endif // !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
#endif // SPI_OUT_TS_SYNC_ENABLED

#if SPI_OUT_LE_AUDIO_ENABLED
static int spi_out_le_audio_log_init(void);
static void spi_out_le_audio_log_deinit(void);
#endif // SPI_OUT_LE_AUDIO_ENABLED

// Private functions
static int spi_out_init_trans(spi_out_trans_cb_t **trans_cb, uint16_t buf_size)
{
    // Memory allocations
    *trans_cb = (spi_out_trans_cb_t *)malloc(sizeof(spi_out_trans_cb_t));
    if (!(*trans_cb)) {
        return -1;
    }
    memset(*trans_cb, 0, sizeof(spi_out_trans_cb_t));

    uint8_t *buf = (uint8_t *)heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
    if (!buf) {
        free(*trans_cb);
        *trans_cb = NULL;
        return -1;
    }

    // Initialization
    (*trans_cb)->buf_size = buf_size;
    (*trans_cb)->trans.tx_buffer = buf;
    (*trans_cb)->trans.user = (void *)(*trans_cb);
    return 0;
}

static void spi_out_deinit_trans(spi_out_trans_cb_t **trans_cb)
{
    if (!(*trans_cb)) {
        return;
    }
    if ((*trans_cb)->trans.tx_buffer) {
        free((uint8_t *)(*trans_cb)->trans.tx_buffer);
        (*trans_cb)->trans.tx_buffer = NULL;
    }

    free(*trans_cb);
    *trans_cb = NULL;
    return;
}

IRAM_ATTR static void spi_out_tx_done_cb(spi_transaction_t *ret_trans)
{
    last_tx_done_ts = esp_timer_get_time();
    spi_out_trans_cb_t *trans_cb = (spi_out_trans_cb_t *)ret_trans->user;
    trans_cb->length = 0;
    trans_cb->flag = TRANS_CB_FLAG_AVAILABLE;
}

IRAM_ATTR static void spi_out_pre_tx_cb(spi_transaction_t *ret_trans)
{
    // SPI slave continuous transaction workaround
    while (esp_timer_get_time() - last_tx_done_ts < SPI_OUT_TRANS_ITVL_MIN_US) {}
}

IRAM_ATTR static inline void spi_out_append_trans(spi_out_trans_cb_t *trans_cb)
{
    if (trans_cb->flag != TRANS_CB_FLAG_NEED_QUEUE || !trans_cb->length) {
        return;
    }

    // Note: To support dump log when disabled
    if (!spi_out_enabled) {
        goto recycle;
    }

    // CRITICAL: Length unit conversion from bytes to bits
    trans_cb->trans.length = trans_cb->length * 8;
    trans_cb->trans.rxlength = 0;
    trans_cb->flag = TRANS_CB_FLAG_IN_QUEUE;
    if (spi_device_queue_trans(spi_handle, &(trans_cb->trans), 0) != ESP_OK) {
        goto recycle;
    }
    return;

recycle:
    trans_cb->length = 0;
    trans_cb->flag = TRANS_CB_FLAG_AVAILABLE;
    return;
}

static int spi_out_log_cb_init(spi_out_log_cb_t **log_cb, uint16_t buf_size, uint8_t type)
{
    // Initialize log control block
    *log_cb = (spi_out_log_cb_t *)malloc(sizeof(spi_out_log_cb_t));
    if (!(*log_cb)) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize log control block!");
        return -1;
    }
    memset(*log_cb, 0, sizeof(spi_out_log_cb_t));

    // Initialize transactions
    int ret = 0;
    for (uint8_t i = 0; i < 2; i++) {
        ret |= spi_out_init_trans(&((*log_cb)->trans_cb[i]), buf_size);
    }
    if (ret != 0) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize SPI transactions!");
        spi_out_log_cb_deinit(log_cb);
        return -1;
    }

    (*log_cb)->type = type;
    return 0;
}

static void spi_out_log_cb_deinit(spi_out_log_cb_t **log_cb)
{
    if (!(*log_cb)) {
        return;
    }

    for (uint8_t i = 0; i < 2; i++) {
        if ((*log_cb)->trans_cb[i]) {
            spi_out_deinit_trans(&((*log_cb)->trans_cb[i]));
        }
    }
    free(*log_cb);
    *log_cb = NULL;
    return;
}

IRAM_ATTR static inline bool spi_out_log_cb_check_trans(spi_out_log_cb_t *log_cb, uint16_t len, bool *need_append)
{
    spi_out_trans_cb_t *trans_cb;
    uint16_t frame_len = len + SPI_OUT_FRAME_OVERHEAD;
    *need_append = false;
    for (uint8_t i = 0; i < 2; i++) {
        trans_cb = log_cb->trans_cb[log_cb->trans_cb_idx];
        if (frame_len > trans_cb->buf_size) {
            goto failed;
        }
        if (trans_cb->flag == TRANS_CB_FLAG_AVAILABLE) {
            if ((trans_cb->buf_size - trans_cb->length) >= frame_len) {
                return true;
            } else {
                trans_cb->flag = TRANS_CB_FLAG_NEED_QUEUE;
                *need_append = true;
            }
        }
        log_cb->trans_cb_idx = !(log_cb->trans_cb_idx);
    }
failed:
    log_cb->lost_bytes_cnt += frame_len;
    log_cb->lost_frame_cnt++;
    log_cb->frame_sn++;
    return false;
}

// CRITICAL: Shall not be called from ISR!
IRAM_ATTR static inline void spi_out_log_cb_append_trans(spi_out_log_cb_t *log_cb)
{
    spi_out_trans_cb_t *trans_cb;
    uint8_t idx = !log_cb->trans_cb_idx;
    for (uint8_t i = 0; i < 2; i++) {
        trans_cb = log_cb->trans_cb[idx];
        if (trans_cb->flag == TRANS_CB_FLAG_NEED_QUEUE) {
            spi_out_append_trans(trans_cb);
        }
        idx = !idx;
    }
}

IRAM_ATTR static inline void spi_out_log_cb_flush_trans(spi_out_log_cb_t *log_cb)
{
    spi_out_trans_cb_t *trans_cb;
    for (uint8_t i = 0; i < 2; i++) {
        trans_cb = log_cb->trans_cb[i];
        if (trans_cb->length && (trans_cb->flag == TRANS_CB_FLAG_AVAILABLE)) {
            trans_cb->flag = TRANS_CB_FLAG_NEED_QUEUE;
        }
    }
}

// Return value: Need append
IRAM_ATTR static bool spi_out_log_cb_write(spi_out_log_cb_t *log_cb, const uint8_t *addr, uint16_t len,
                                           const uint8_t *addr_append, uint16_t len_append, uint8_t source,
                                           bool with_checksum)
{
    spi_out_trans_cb_t *trans_cb = log_cb->trans_cb[log_cb->trans_cb_idx];

    uint8_t *buf = (uint8_t *)trans_cb->trans.tx_buffer + trans_cb->length;
    uint16_t total_length = len + len_append;
    frame_head_t head = {
        .length = total_length,
        .source = source,
        .frame_sn = log_cb->frame_sn,
    };
    uint32_t checksum = 0;
    if (with_checksum) {
        for (int i = 0; i < len; i++) {
            checksum += addr[i];
        }
        for (int i = 0; i < len_append; i++) {
            checksum += addr_append[i];
        }
    }

    memcpy(buf, (const uint8_t *)&head, SPI_OUT_FRAME_HEAD_LEN);
    memcpy(buf + SPI_OUT_FRAME_HEAD_LEN, addr, len);
    if (len_append) {
        memcpy(buf + SPI_OUT_FRAME_HEAD_LEN + len, addr_append, len_append);
    }
    memcpy(buf + SPI_OUT_FRAME_HEAD_LEN + total_length, &checksum, SPI_OUT_FRAME_TAIL_LEN);

    trans_cb->length += total_length + SPI_OUT_FRAME_OVERHEAD;
    log_cb->frame_sn++;
    if ((trans_cb->buf_size - trans_cb->length) <= SPI_OUT_FRAME_OVERHEAD) {
        trans_cb->flag = TRANS_CB_FLAG_NEED_QUEUE;
        return true;
    }
    return false;
}

IRAM_ATTR static void spi_out_log_cb_write_loss(spi_out_log_cb_t *log_cb)
{
    if (!log_cb->lost_bytes_cnt || !log_cb->lost_frame_cnt) {
        return;
    }
    bool need_append;
    if (spi_out_log_cb_check_trans(log_cb, sizeof(loss_payload_t), &need_append)) {
        loss_payload_t payload = {
            .type = log_cb->type,
            .lost_frame_cnt = log_cb->lost_frame_cnt,
            .lost_bytes_cnt = log_cb->lost_bytes_cnt,
        };
        spi_out_log_cb_write(log_cb, (const uint8_t *)&payload, sizeof(loss_payload_t),
                             NULL, 0, BLE_LOG_SPI_OUT_SOURCE_LOSS, true);

        log_cb->lost_frame_cnt = 0;
        log_cb->lost_bytes_cnt = 0;
    }
}

static void spi_out_log_cb_dump(spi_out_log_cb_t *log_cb)
{
    spi_out_trans_cb_t *trans_cb;
    uint8_t *buf;
    for (uint8_t i = 0; i < 2; i++) {
        // Dump the last transaction before dumping the current transaction
        log_cb->trans_cb_idx = !(log_cb->trans_cb_idx);
        trans_cb = log_cb->trans_cb[log_cb->trans_cb_idx];
        buf = (uint8_t *)trans_cb->trans.tx_buffer;
        for (uint16_t j = 0; j < trans_cb->buf_size; j++) {
            esp_rom_printf("%02x ", buf[j]);

            // Feed watchdogs periodically to avoid wdts timeout
            if ((j % 100) == 0) {
                esp_panic_handler_feed_wdts();
            }
        }
    }
}

static void spi_out_log_flush(void)
{
    // Flush ul log
    xSemaphoreTake(ul_log_mutex, portMAX_DELAY);
    spi_out_log_cb_flush_trans(ul_log_cb);
    spi_out_log_cb_append_trans(ul_log_cb);
    xSemaphoreGive(ul_log_mutex);

#if SPI_OUT_LL_ENABLED
    if (esp_bt_controller_get_status() >= ESP_BT_CONTROLLER_STATUS_INITED) {
        ll_ev_flags |= BIT(LL_EV_FLAG_FLUSH_LOG);
        SPI_OUT_LL_PUT_EV;
    }
#endif // SPI_OUT_LL_ENABLED
}

#if SPI_OUT_FLUSH_TIMER_ENABLED
// Context: ESP timer
static void esp_timer_cb_log_flush(void)
{
    spi_out_log_flush();
    esp_timer_start_once(flush_timer, SPI_OUT_FLUSH_TIMEOUT_US);
}
#endif // SPI_OUT_FLUSH_TIMER_ENABLED

static int spi_out_ul_log_init(void)
{
    if (ul_log_inited) {
        return 0;
    }

    // Initialize mutex
    ul_log_mutex = xSemaphoreCreateMutex();
    if (!ul_log_mutex) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to create mutex for upper layer task log!");
        goto mutex_init_failed;
    }

    // Initialize string buffer
    ul_log_str_buf = (uint8_t *)malloc(SPI_OUT_UL_LOG_STR_BUF_SIZE);
    if (!ul_log_str_buf) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize string buffer for upper layer task log!");
        goto str_buf_init_failed;
    }

    // Initialize log control block
    if (spi_out_log_cb_init(&ul_log_cb, SPI_OUT_UL_TASK_BUF_SIZE, LOG_CB_TYPE_UL) != 0) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize log control blocks for upper layer task log!");
        goto log_cb_init_failed;
    }

    // Initialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to initialize upper layer task log!");
    ul_log_inited = true;
    return 0;

log_cb_init_failed:
    if (ul_log_str_buf) {
        free(ul_log_str_buf);
        ul_log_str_buf = NULL;
    }
str_buf_init_failed:
    vSemaphoreDelete(ul_log_mutex);
mutex_init_failed:
    return -1;
}

static void spi_out_ul_log_deinit(void)
{
    if (!ul_log_inited) {
        return;
    }
    ul_log_inited = false;

    xSemaphoreTake(ul_log_mutex, portMAX_DELAY);
    if (ul_log_str_buf) {
        free(ul_log_str_buf);
        ul_log_str_buf = NULL;
    }
    spi_out_log_cb_deinit(&ul_log_cb);
    xSemaphoreGive(ul_log_mutex);

    vSemaphoreDelete(ul_log_mutex);
    ul_log_mutex = NULL;

    ESP_LOGI(BLE_LOG_TAG, "Succeeded to deinitialize upper layer log!");
    return;
}

static void spi_out_ul_log_write(uint8_t source, const uint8_t *addr, uint16_t len, bool with_ts)
{
    uint16_t total_len = with_ts? (len + sizeof(uint32_t)): len;
    bool need_append;
    if (spi_out_log_cb_check_trans(ul_log_cb, total_len, &need_append)) {
        if (with_ts) {
            uint32_t esp_ts = esp_timer_get_time();
            need_append |= spi_out_log_cb_write(ul_log_cb, (const uint8_t *)&esp_ts,
                                                sizeof(uint32_t), addr, len, source, true);
        } else {
            need_append |= spi_out_log_cb_write(ul_log_cb, addr, len, NULL, 0, source, true);
        }
    }
    if (need_append) {
        spi_out_log_cb_append_trans(ul_log_cb);
    }
    spi_out_log_cb_write_loss(ul_log_cb);
}

static bool spi_out_ul_log_printf(uint8_t source, const char *format, va_list args, int offset)
{
    int len = vsnprintf((char *)(ul_log_str_buf + offset),
                        SPI_OUT_UL_LOG_STR_BUF_SIZE - offset, format, args);
    if (len < 0) {
        return false;
    }
    len += offset;

    // Truncate string if overflowed
    if (len >= SPI_OUT_UL_LOG_STR_BUF_SIZE) {
        len = SPI_OUT_UL_LOG_STR_BUF_SIZE - 1;
        ul_log_str_buf[len] = '\0';
    }

    spi_out_ul_log_write(source, ul_log_str_buf, len, true);
    return true;
}

#if SPI_OUT_LL_ENABLED
static int spi_out_ll_log_init(void)
{
    if (ll_log_inited) {
        return 0;
    }

    // Initialize log control blocks for controller task & ISR logs
    if (spi_out_log_cb_init(&ll_task_log_cb, SPI_OUT_LL_TASK_BUF_SIZE, LOG_CB_TYPE_LL_TASK) != 0) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize log control blocks for controller task!");
        goto task_log_cb_init_failed;
    }
    if (spi_out_log_cb_init(&ll_isr_log_cb, SPI_OUT_LL_ISR_BUF_SIZE, LOG_CB_TYPE_LL_ISR) != 0) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize log control blocks for controller ISR!");
        goto isr_log_cb_init_failed;
    }
    if (spi_out_log_cb_init(&ll_hci_log_cb, SPI_OUT_LL_HCI_BUF_SIZE, LOG_CB_TYPE_LL_HCI) != 0) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize log control blocks for controller ISR!");
        goto hci_log_cb_init_failed;
    }

    // Initialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to initialize log control blocks for controller task & ISR!");
    ll_log_inited = true;
    return 0;

hci_log_cb_init_failed:
    spi_out_log_cb_deinit(&ll_isr_log_cb);
isr_log_cb_init_failed:
    spi_out_log_cb_deinit(&ll_task_log_cb);
task_log_cb_init_failed:
    return -1;
}

static void spi_out_ll_log_deinit(void)
{
    if (!ll_log_inited) {
        return;
    }

    spi_out_log_cb_deinit(&ll_hci_log_cb);
    spi_out_log_cb_deinit(&ll_isr_log_cb);
    spi_out_log_cb_deinit(&ll_task_log_cb);

    // Deinitialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to deinitialize controller log!");
    ll_log_inited = false;
    return;
}

// Context: LL task
static void spi_out_ll_log_flush(void)
{
    // Flush task log and hci log buffer
    spi_out_log_cb_flush_trans(ll_task_log_cb);
    spi_out_log_cb_flush_trans(ll_hci_log_cb);

    // Race condition for isr log buffer
    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);
    spi_out_log_cb_flush_trans(ll_isr_log_cb);
    portEXIT_CRITICAL_SAFE(&spinlock);

    // Note: Save SPI transfer start time
    spi_out_log_cb_append_trans(ll_task_log_cb);
    spi_out_log_cb_append_trans(ll_hci_log_cb);
    spi_out_log_cb_append_trans(ll_isr_log_cb);
}
#endif // SPI_OUT_LL_ENABLED

#if SPI_OUT_TS_SYNC_ENABLED
static int spi_out_ts_sync_init(void)
{
    if (ts_sync_inited) {
        return 0;
    }

#if !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
    // Initialize sync timer
    esp_timer_create_args_t timer_args = {
        .callback = (esp_timer_cb_t)esp_timer_cb_ts_sync,
        .dispatch_method = ESP_TIMER_TASK
    };
    if (esp_timer_create(&timer_args, &ts_sync_timer) != ESP_OK) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize timestamp synchronizer timer!");
        goto timer_init_failed;
    }
#endif // !SPI_OUT_TS_SYNC_SLEEP_SUPPORT

    // Initialize sync IO
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT(SPI_OUT_SYNC_IO_NUM),
        .pull_down_en = 0,
        .pull_up_en = 0
    };
    if (gpio_config(&io_conf) != ESP_OK) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize timestamp synchronizer IO!");
        goto gpio_init_failed;
    }

    // Initialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to initialize timestamp synchronizer!");
    ts_sync_inited = true;
    return 0;

gpio_init_failed:
#if !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
    esp_timer_delete(ts_sync_timer);
    ts_sync_timer = NULL;
timer_init_failed:
#endif // !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
    return -1;
}

static void spi_out_ts_sync_deinit(void)
{
    if (!ts_sync_inited) {
        return;
    }

#if !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
    // Deinitialize timestamp synchronizer
    esp_timer_stop(ts_sync_timer);
    esp_timer_delete(ts_sync_timer);
    ts_sync_timer = NULL;
#endif // !SPI_OUT_TS_SYNC_SLEEP_SUPPORT

    // Deinitialize sync IO
    spi_out_ts_sync_enable(false);
    gpio_reset_pin(SPI_OUT_SYNC_IO_NUM);

    // Deinitialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to deinitialize timestamp synchronizer!");
    ts_sync_inited = false;
    return;
}

static void spi_out_ts_sync_enable(bool enable)
{
    // Update ts sync status
    ts_sync_enabled = enable;
    if (enable) {
#if !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
        // Start timestamp sync timer
        if (ts_sync_timer) {
            if (!esp_timer_is_active(ts_sync_timer)) {
                esp_timer_start_periodic(ts_sync_timer, SPI_OUT_TS_SYNC_TIMEOUT);
            }
        }
#endif // !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
    } else {
#if !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
        // Stop timestamp sync timer
        if (ts_sync_timer) {
            if (esp_timer_is_active(ts_sync_timer)) {
                esp_timer_stop(ts_sync_timer);
            }
        }
#endif // !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
        if (!ts_sync_data.io_level) {
            gpio_set_level(SPI_OUT_SYNC_IO_NUM, 1);
        }
    }
    ts_sync_data.io_level = 0;
    gpio_set_level(SPI_OUT_SYNC_IO_NUM, (uint32_t)ts_sync_data.io_level);
}

static void spi_out_ts_sync_toggle(void)
{
    // Toggle sync IO
    ts_sync_data.io_level = !ts_sync_data.io_level;

    // Enter critical
    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&spinlock);

    // Get LC timestamp
    ts_sync_data.lc_ts = SPI_OUT_GET_LC_TIME;

    // Set sync IO level
    gpio_set_level(SPI_OUT_SYNC_IO_NUM, (uint32_t)ts_sync_data.io_level);

    // Get ESP timestamp
    ts_sync_data.esp_ts = esp_timer_get_time();
    portEXIT_CRITICAL(&spinlock);
    // Exit critical
}

#if !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
// CRITICAL: This function is called in ESP Timer task
static void esp_timer_cb_ts_sync(void)
{
    spi_out_ts_sync_toggle();
    ble_log_spi_out_write(BLE_LOG_SPI_OUT_SOURCE_SYNC, (const uint8_t *)&ts_sync_data,
                          sizeof(ts_sync_data_t));
}
#else

#endif // !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
#endif // SPI_OUT_TS_SYNC_ENABLED

#if SPI_OUT_LE_AUDIO_ENABLED
static int spi_out_le_audio_log_init(void)
{
    if (le_audio_log_inited) {
        return 0;
    }

    // Initialize log control blocks for controller task & ISR logs
    if (spi_out_log_cb_init(&le_audio_log_cb, SPI_OUT_LE_AUDIO_BUF_SIZE, LOG_CB_TYPE_LE_AUDIO) != 0) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize log control blocks for LE audio!");
        return -1;
    }

    // Initialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to initialize log control blocks for LE Audio!");
    le_audio_log_inited = true;
    return 0;
}

static void spi_out_le_audio_log_deinit(void)
{
    if (!le_audio_log_inited) {
        return;
    }

    spi_out_log_cb_deinit(&le_audio_log_cb);

    // Deinitialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to deinitialize LE audio log!");
    le_audio_log_inited = false;
    return;
}
#endif // SPI_OUT_LE_AUDIO_ENABLED

// Public functions
int ble_log_spi_out_init(void)
{
    // Avoid double init
    if (spi_out_inited) {
        return 0;
    }

    // Initialize SPI
    spi_bus_config_t bus_config = {
        .miso_io_num = -1,
        .mosi_io_num = SPI_OUT_MOSI_IO_NUM,
        .sclk_io_num = SPI_OUT_SCLK_IO_NUM,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = SPI_OUT_MAX_TRANSFER_SIZE,
#if CONFIG_SPI_MASTER_ISR_IN_IRAM
        .intr_flags = ESP_INTR_FLAG_IRAM
#endif // CONFIG_SPI_MASTER_ISR_IN_IRAM
    };
    spi_device_interface_config_t dev_config = {
        .clock_speed_hz = SPI_MASTER_FREQ_20M,
        .mode = 0,
        .spics_io_num = SPI_OUT_CS_IO_NUM,
        .queue_size = SPI_OUT_SPI_MASTER_QUEUE_SIZE,
        .post_cb = spi_out_tx_done_cb,
        .pre_cb = spi_out_pre_tx_cb,
        .flags = SPI_DEVICE_NO_RETURN_RESULT
    };
    if (spi_bus_initialize(SPI_OUT_BUS, &bus_config, SPI_DMA_CH_AUTO) != ESP_OK) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize SPI bus!");
        goto spi_bus_init_failed;
    }
    if (spi_bus_add_device(SPI_OUT_BUS, &dev_config, &spi_handle) != ESP_OK) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to add device to SPI bus!");
        goto spi_device_add_failed;
    }

    if (spi_out_ul_log_init() != 0) {
        goto ul_log_init_failed;
    }

#if SPI_OUT_LL_ENABLED
    if (spi_out_ll_log_init() != 0) {
        goto ll_log_init_failed;
    }
#endif // SPI_OUT_LL_ENABLED

#if SPI_OUT_TS_SYNC_ENABLED
    if (spi_out_ts_sync_init() != 0) {
        goto ts_sync_init_failed;
    }
#endif // SPI_OUT_TS_SYNC_ENABLED

#if SPI_OUT_LE_AUDIO_ENABLED
    if (spi_out_le_audio_log_init() != 0) {
        goto le_audio_init_failed;
    }
#endif // SPI_OUT_LE_AUDIO_ENABLED

#if SPI_OUT_FLUSH_TIMER_ENABLED
    esp_timer_create_args_t timer_args = {
        .callback = (esp_timer_cb_t)esp_timer_cb_log_flush,
        .dispatch_method = ESP_TIMER_TASK
    };
    if (esp_timer_create(&timer_args, &flush_timer) != ESP_OK) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize flush timer!");
        goto timer_init_failed;
    }
#endif // SPI_OUT_FLUSH_TIMER_ENABLED

    // Initialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to initialize BLE log SPI output interface!");
    spi_out_inited = true;
    spi_out_enabled = true;

#if SPI_OUT_FLUSH_TIMER_ENABLED
    esp_timer_start_once(flush_timer, SPI_OUT_FLUSH_TIMEOUT_US);
#endif // SPI_OUT_FLUSH_TIMER_ENABLED
    return 0;

#if SPI_OUT_FLUSH_TIMER_ENABLED
timer_init_failed:
#endif // SPI_OUT_FLUSH_TIMER_ENABLED
#if SPI_OUT_LE_AUDIO_ENABLED
    spi_out_le_audio_log_deinit();
le_audio_init_failed:
#endif // SPI_OUT_LE_AUDIO_ENABLED
#if SPI_OUT_TS_SYNC_ENABLED
    spi_out_ts_sync_deinit();
ts_sync_init_failed:
#endif // SPI_OUT_TS_SYNC_ENABLED
#if SPI_OUT_LL_ENABLED
    spi_out_ll_log_deinit();
ll_log_init_failed:
#endif // SPI_OUT_LL_ENABLED
    spi_out_ul_log_deinit();
ul_log_init_failed:
    spi_bus_remove_device(spi_handle);
    spi_handle = NULL;
spi_device_add_failed:
    spi_bus_free(SPI_OUT_BUS);
spi_bus_init_failed:
    return -1;
}

void ble_log_spi_out_deinit(void)
{
    // Avoid double deinit
    if (!spi_out_inited) {
        return;
    }

#if SPI_OUT_FLUSH_TIMER_ENABLED
    esp_timer_stop(flush_timer);
    esp_timer_delete(flush_timer);
    flush_timer = NULL;
#endif // SPI_OUT_FLUSH_TIMER_ENABLED

    // Drain all queued transactions
    assert(spi_device_acquire_bus(spi_handle, portMAX_DELAY) == ESP_OK);
    spi_device_release_bus(spi_handle);

    // Remove SPI master
    spi_bus_remove_device(spi_handle);
    spi_handle = NULL;
    spi_bus_free(SPI_OUT_BUS);

#if SPI_OUT_TS_SYNC_ENABLED
    spi_out_ts_sync_deinit();
#endif // SPI_OUT_TS_SYNC_ENABLED

#if SPI_OUT_LL_ENABLED
    spi_out_ll_log_deinit();
#endif // SPI_OUT_LL_ENABLED

    spi_out_ul_log_deinit();

    // Reset init flag
    spi_out_inited = false;
    spi_out_enabled = false;
}

#if SPI_OUT_TS_SYNC_ENABLED
void ble_log_spi_out_ts_sync_start(void)
{
    // Check if SPI out is initialized
    if (!spi_out_inited) {
        return;
    }
    spi_out_ts_sync_enable(true);
}

void ble_log_spi_out_ts_sync_stop(void)
{
    // Check if SPI out is initialized
    if (!spi_out_inited) {
        return;
    }
    spi_out_ts_sync_enable(false);
}
#endif // SPI_OUT_TS_SYNC_ENABLED

#if SPI_OUT_LL_ENABLED
// Only LL task has access to this API
IRAM_ATTR void ble_log_spi_out_ll_write(uint32_t len, const uint8_t *addr, uint32_t len_append,
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
    spi_out_log_cb_t *log_cb;
    if (flag & BIT(LL_LOG_FLAG_ISR)) {
        log_cb = ll_isr_log_cb;
        source = BLE_LOG_SPI_OUT_SOURCE_ESP_ISR;
        in_isr = true;
    } else if (flag & BIT(LL_LOG_FLAG_HCI)) {
        log_cb = ll_hci_log_cb;
        source = BLE_LOG_SPI_OUT_SOURCE_LL_HCI;
    } else {
        log_cb = ll_task_log_cb;
        source = BLE_LOG_SPI_OUT_SOURCE_ESP;
    }

    bool need_append;
    if (spi_out_log_cb_check_trans(log_cb, (uint16_t)(len + len_append), &need_append)) {
        need_append |= spi_out_log_cb_write(log_cb, addr, (uint16_t)len, addr_append,
                                            (uint16_t)len_append, source, true);
    }
    if (need_append) {
        if (in_isr) {
            ll_ev_flags |= BIT(LL_EV_FLAG_ISR_APPEND);
            SPI_OUT_LL_PUT_EV;
        } else {
            spi_out_log_cb_append_trans(log_cb);

#if SPI_OUT_TS_SYNC_SLEEP_SUPPORT
            if (ts_sync_inited && ts_sync_enabled) {
                if (last_tx_done_ts >= (SPI_OUT_TS_SYNC_TIMEOUT + ts_sync_data.esp_ts)) {
                    if (spi_out_log_cb_check_trans(ll_task_log_cb, sizeof(ts_sync_data_t), &need_append)) {
                        spi_out_ts_sync_toggle();
                        spi_out_log_cb_write(ll_task_log_cb, (const uint8_t *)&ts_sync_data,
                                             sizeof(ts_sync_data_t), NULL, 0, BLE_LOG_SPI_OUT_SOURCE_SYNC, true);
                    }
                }
            }
#endif // !SPI_OUT_TS_SYNC_SLEEP_SUPPORT
        }
    }
    spi_out_log_cb_write_loss(log_cb);
}

IRAM_ATTR void ble_log_spi_out_ll_log_ev_proc(void)
{
    if (!ll_log_inited) {
        return;
    }

    if (ll_ev_flags & BIT(LL_EV_FLAG_ISR_APPEND)) {
        spi_out_log_cb_append_trans(ll_isr_log_cb);
        ll_ev_flags &= ~BIT(LL_EV_FLAG_ISR_APPEND);
    }

    if (ll_ev_flags & BIT(LL_EV_FLAG_FLUSH_LOG)) {
        spi_out_ll_log_flush();
        ll_ev_flags &= ~BIT(LL_EV_FLAG_FLUSH_LOG);
    }

    ll_ev_flags = 0;
}
#endif // SPI_OUT_LL_ENABLED

int ble_log_spi_out_write(uint8_t source, const uint8_t *addr, uint16_t len)
{
    if (!ul_log_inited) {
        return -1;
    }

    xSemaphoreTake(ul_log_mutex, portMAX_DELAY);
    spi_out_ul_log_write(source, addr, len, false);
    xSemaphoreGive(ul_log_mutex);
    return 0;
}

int ble_log_spi_out_printf(uint8_t source, const char *format, ...)
{
    if (!ul_log_inited) {
        return -1;
    }

    if (!format) {
        return -1;
    }

    // Get arguments
    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);

    xSemaphoreTake(ul_log_mutex, portMAX_DELAY);
    bool ret = spi_out_ul_log_printf(source, format, args_copy, 0);
    xSemaphoreGive(ul_log_mutex);

    va_end(args_copy);
    va_end(args);
    return ret? 0: -1;
}

int ble_log_spi_out_printf_enh(uint8_t source, uint8_t level, const char *tag, const char *format, ...)
{
    if (!ul_log_inited) {
        return -1;
    }

    if (!tag || !format) {
        return -1;
    }

    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);

    // Create log prefix in the format: "[level][tag] "
    bool ret = false;
    xSemaphoreTake(ul_log_mutex, portMAX_DELAY);
    int prefix_len = snprintf((char *)ul_log_str_buf, SPI_OUT_UL_LOG_STR_BUF_SIZE,
                              "[%d][%s]", level, tag? tag: "NULL");
    if ((prefix_len < 0) || (prefix_len >= SPI_OUT_UL_LOG_STR_BUF_SIZE)) {
        goto exit;
    }
    ret = spi_out_ul_log_printf(source, format, args_copy, prefix_len);
exit:
    xSemaphoreGive(ul_log_mutex);
    va_end(args_copy);
    va_end(args);
    return ret? 0: -1;
}

int ble_log_spi_out_write_with_ts(uint8_t source, const uint8_t *addr, uint16_t len)
{
    if (!ul_log_inited) {
        return -1;
    }

    xSemaphoreTake(ul_log_mutex, portMAX_DELAY);
    spi_out_ul_log_write(source, addr, len, true);
    xSemaphoreGive(ul_log_mutex);
    return 0;
}

void ble_log_spi_out_dump_all(void)
{
    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);

#if SPI_OUT_LL_ENABLED
    if (ll_log_inited) {
        // Dump lower layer log buffer
        esp_rom_printf("[LL_ISR_LOG_DUMP_START:\n");
        spi_out_log_cb_dump(ll_isr_log_cb);
        esp_rom_printf("\n:LL_ISR_LOG_DUMP_END]\n\n");

        esp_rom_printf("[LL_TASK_LOG_DUMP_START:\n");
        spi_out_log_cb_dump(ll_task_log_cb);
        esp_rom_printf("\n:LL_TASK_LOG_DUMP_END]\n\n");

        esp_rom_printf("[LL_HCI_LOG_DUMP_START:\n");
        spi_out_log_cb_dump(ll_hci_log_cb);
        esp_rom_printf("\n:LL_HCI_LOG_DUMP_END]\n\n");
    }
#endif // SPI_OUT_LL_ENABLED

    if (ul_log_inited) {
        // Dump upper layer log buffer
        esp_rom_printf("[UL_LOG_DUMP_START:\n");
        spi_out_log_cb_dump(ul_log_cb);
        esp_rom_printf("\n:UL_LOG_DUMP_END]\n\n");
    }
    portEXIT_CRITICAL_SAFE(&spinlock);
}

void ble_log_spi_out_enable(bool enable)
{
    spi_out_enabled = enable;

    if (!enable) {
#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
        ble_log_spi_out_ts_sync_stop();
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
    }
}

void ble_log_spi_out_flush(void)
{
    if (!spi_out_enabled) {
        return;
    }

    spi_out_log_flush();
}

#if CONFIG_BT_BLE_LOG_SPI_OUT_LE_AUDIO_ENABLED
IRAM_ATTR void ble_log_spi_out_le_audio_write(const uint8_t *addr, uint16_t len)
{
    if (!le_audio_log_inited) {
        return;
    }

    bool need_append;
    if (spi_out_log_cb_check_trans(le_audio_log_cb, len, &need_append)) {
        need_append |= spi_out_log_cb_write(le_audio_log_cb, addr, len, NULL, 0,
                                            BLE_LOG_SPI_OUT_SOURCE_LE_AUDIO, false);
    }
    if (need_append) {
        spi_out_log_cb_append_trans(le_audio_log_cb);
    }
    spi_out_log_cb_write_loss(le_audio_log_cb);
    return;
}
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_LE_AUDIO_ENABLED
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
