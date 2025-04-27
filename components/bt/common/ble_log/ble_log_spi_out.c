/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ble_log/ble_log_spi_out.h"

#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

// Private defines
#define BLE_LOG_TAG "BLE_LOG"
#define SPI_OUT_BUS SPI2_HOST
#define SPI_OUT_MAX_TRANSFER_SIZE 10240
#define SPI_OUT_FRAME_HEAD_LEN 4
#define SPI_OUT_FRAME_TAIL_LEN 4
#define SPI_OUT_FRAME_OVERHEAD (SPI_OUT_FRAME_HEAD_LEN + SPI_OUT_FRAME_TAIL_LEN)
#define SPI_OUT_RECYCLE_TIMEOUT_MS 10
#define SPI_OUT_TRANS_CB_FLAG_AVAILABLE 0
#define SPI_OUT_TRANS_CB_FLAG_NEED_QUEUE 1
#define SPI_OUT_TRANS_CB_FLAG_IN_QUEUE 2
#define SPI_OUT_FLUSHOUT_TIMEOUT (CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMEOUT * 1000)
#define SPI_OUT_PACKET_LOSS_UL 0
#define SPI_OUT_PACKET_LOSS_LL_TASK 1
#define SPI_OUT_PACKET_LOSS_LL_ISR 2
#define SPI_OUT_PACKET_LOSS_FRAME_SIZE 6
#define SPI_OUT_INTERFACE_FLAG_IN_ISR (1 << 3)
#define SPI_OUT_TRANS_ITVL_MIN_US 30
#define SPI_OUT_SPI_MASTER_QUEUE_SIZE 6

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
#define SPI_OUT_TS_SYNC_TIMEOUT (1000 * 1000)
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

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
    uint8_t frame_cnt;
    uint32_t bytes_loss_cnt;
    uint8_t trans_loss_cnt;
} spi_out_log_cb_t;

// Private variables
static bool spi_out_inited = false;
static spi_device_handle_t spi_handle = NULL;
static uint32_t last_tx_done_ts = 0;

static bool ul_log_inited = false;
static SemaphoreHandle_t ul_log_mutex = NULL;
static spi_out_log_cb_t *ul_log_cb = NULL;
#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
static esp_timer_handle_t ul_log_flushout_timer = NULL;
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED

#if CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED
static bool ll_log_inited = false;
static spi_out_log_cb_t *ll_task_log_cb = NULL;
static spi_out_log_cb_t *ll_isr_log_cb = NULL;
#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
static esp_timer_handle_t ll_log_flushout_timer = NULL;
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
static bool ts_sync_inited = false;
static bool sync_io_level = false;
static esp_timer_handle_t ts_sync_timer = NULL;
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

// Extern function declarations
extern void esp_panic_handler_feed_wdts(void);

// Private function declarations
static int spi_out_init_trans(spi_out_trans_cb_t **trans_cb, uint16_t buf_size);
static void spi_out_deinit_trans(spi_out_trans_cb_t **trans_cb);
static void spi_out_tx_done_cb(spi_transaction_t *ret_trans);
static inline int spi_out_append_trans(spi_out_trans_cb_t *trans_cb);

static int spi_out_log_cb_init(spi_out_log_cb_t **log_cb, uint16_t buf_size);
static void spi_out_log_cb_deinit(spi_out_log_cb_t **log_cb);
static inline int spi_out_log_cb_check_trans(spi_out_log_cb_t *log_cb, uint16_t len);
static inline void spi_out_log_cb_append_trans(spi_out_log_cb_t *log_cb);
static inline void spi_out_log_cb_flush_trans(spi_out_log_cb_t *log_cb);
static void spi_out_log_cb_write(spi_out_log_cb_t *log_cb, const uint8_t *addr, uint16_t len, \
                                 const uint8_t *addr_append, uint16_t len_append, uint8_t source);
static void spi_out_log_cb_write_packet_loss(spi_out_log_cb_t *log_cb, uint8_t flag);
static void spi_out_log_cb_dump(spi_out_log_cb_t *log_cb);

static int spi_out_ul_log_init(void);
static void spi_out_ul_log_deinit(void);
#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
static void esp_timer_cb_ul_log_flushout(void);
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED

#if CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED
static int spi_out_ll_log_init(void);
static void spi_out_ll_log_deinit(void);
static void spi_out_ll_log_ev_proc(void);
#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
static void esp_timer_cb_ll_log_flushout(void);
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED

#if defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) ||\
    defined(CONFIG_IDF_TARGET_ESP32C61) || defined(CONFIG_IDF_TARGET_ESP32H21)
extern void r_ble_log_simple_put_ev(void);
#define BLE_LOG_LL_PUT_EV r_ble_log_simple_put_ev()
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
extern void ble_log_simple_put_ev(void);
#define BLE_LOG_LL_PUT_EV ble_log_simple_put_ev()
#else
#define BLE_LOG_LL_PUT_EV
#endif

#endif // CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
static int spi_out_ts_sync_init(void);
static void spi_out_ts_sync_deinit(void);
static void esp_timer_cb_ts_sync(void);

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

#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

// Private functions
static int spi_out_init_trans(spi_out_trans_cb_t **trans_cb, uint16_t buf_size)
{
    // Memory allocations
    *trans_cb = (spi_out_trans_cb_t *)malloc(sizeof(spi_out_trans_cb_t));
    if (!(*trans_cb)) {
        return -1;
    }
    memset(*trans_cb, 0, sizeof(spi_out_trans_cb_t));

    uint8_t *buf = (uint8_t *)spi_bus_dma_memory_alloc(SPI_OUT_BUS, (size_t)buf_size, 0);
    if (!buf) {
        free(*trans_cb);
        *trans_cb = NULL;
        return -1;
    }

    // Initialization
    (*trans_cb)->buf_size = buf_size;
    (*trans_cb)->trans.tx_buffer = buf;
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
    spi_out_trans_cb_t *trans_cb = __containerof(ret_trans, spi_out_trans_cb_t, trans);
    trans_cb->length = 0;
    trans_cb->flag = SPI_OUT_TRANS_CB_FLAG_AVAILABLE;
}

IRAM_ATTR static void spi_out_pre_tx_cb(spi_transaction_t *ret_trans)
{
    // SPI slave continuous transaction workaround
    while (esp_timer_get_time() - last_tx_done_ts < SPI_OUT_TRANS_ITVL_MIN_US) {}
}

IRAM_ATTR static inline int spi_out_append_trans(spi_out_trans_cb_t *trans_cb)
{
    if (trans_cb->flag != SPI_OUT_TRANS_CB_FLAG_NEED_QUEUE || !trans_cb->length) {
        return -1;
    }

    // CRITICAL: Length unit conversion from bytes to bits
    trans_cb->trans.length = trans_cb->length * 8;
    trans_cb->trans.rxlength = 0;
    if (spi_device_queue_trans(spi_handle, &(trans_cb->trans), 0) == ESP_OK) {
        trans_cb->flag = SPI_OUT_TRANS_CB_FLAG_IN_QUEUE;
        return 0;
    } else {
        trans_cb->length = 0;
        trans_cb->flag = SPI_OUT_TRANS_CB_FLAG_AVAILABLE;
        return -1;
    }
}

static int spi_out_log_cb_init(spi_out_log_cb_t **log_cb, uint16_t buf_size)
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

IRAM_ATTR static inline int spi_out_log_cb_check_trans(spi_out_log_cb_t *log_cb, uint16_t len)
{
    spi_out_trans_cb_t *trans_cb;
    uint16_t frame_len = len + SPI_OUT_FRAME_OVERHEAD;
    for (uint8_t i = 0; i < 2; i++) {
        trans_cb = log_cb->trans_cb[log_cb->trans_cb_idx];
        if (frame_len > trans_cb->buf_size) {
            goto failed;
        }
        if (trans_cb->flag == SPI_OUT_TRANS_CB_FLAG_AVAILABLE) {
            if ((trans_cb->buf_size - trans_cb->length) >= (len + SPI_OUT_FRAME_OVERHEAD)) {
                return 0;
            } else {
                trans_cb->flag = SPI_OUT_TRANS_CB_FLAG_NEED_QUEUE;
            }
        }
        log_cb->trans_cb_idx = !(log_cb->trans_cb_idx);
    }
failed:
    log_cb->bytes_loss_cnt += len + SPI_OUT_FRAME_OVERHEAD;
    log_cb->frame_cnt++;
    return -1;
}

// CRITICAL: Shall not be called from ISR!
IRAM_ATTR static inline void spi_out_log_cb_append_trans(spi_out_log_cb_t *log_cb)
{
    spi_out_trans_cb_t *trans_cb;
    uint8_t idx = !log_cb->trans_cb_idx;
    for (uint8_t i = 0; i < 2; i++) {
        trans_cb = log_cb->trans_cb[idx];
        if (trans_cb->flag == SPI_OUT_TRANS_CB_FLAG_NEED_QUEUE) {
            if (spi_out_append_trans(trans_cb) != 0) {
                log_cb->trans_loss_cnt++;
            }
        }
        idx = !idx;
    }
}

IRAM_ATTR static inline void spi_out_log_cb_flush_trans(spi_out_log_cb_t *log_cb)
{
    spi_out_trans_cb_t *trans_cb = log_cb->trans_cb[log_cb->trans_cb_idx];
    if (trans_cb->length && (trans_cb->flag == SPI_OUT_TRANS_CB_FLAG_AVAILABLE)) {
        trans_cb->flag = SPI_OUT_TRANS_CB_FLAG_NEED_QUEUE;
    }
}

IRAM_ATTR static void spi_out_log_cb_write(spi_out_log_cb_t *log_cb, const uint8_t *addr, uint16_t len, \
                                           const uint8_t *addr_append, uint16_t len_append, uint8_t source)
{
    spi_out_trans_cb_t *trans_cb = log_cb->trans_cb[log_cb->trans_cb_idx];

    uint8_t *buf = (uint8_t *)trans_cb->trans.tx_buffer + trans_cb->length;
    uint16_t total_length = len + len_append;
    const uint8_t head[4] = {total_length & 0xFF, (total_length >> 8) & 0xFF, source, log_cb->frame_cnt};
    uint32_t checksum = 0;
    for (int i = 0; i < len; i++) {
        checksum += addr[i];
    }
    for (int i = 0; i < len_append; i++) {
        checksum += addr_append[i];
    }

    memcpy(buf, head, SPI_OUT_FRAME_HEAD_LEN);
    memcpy(buf + SPI_OUT_FRAME_HEAD_LEN, addr, len);
    if (len_append) {
        memcpy(buf + SPI_OUT_FRAME_HEAD_LEN + len, addr_append, len_append);
    }
    memcpy(buf + SPI_OUT_FRAME_HEAD_LEN + total_length, &checksum, SPI_OUT_FRAME_TAIL_LEN);

    trans_cb->length += total_length + SPI_OUT_FRAME_OVERHEAD;
    log_cb->frame_cnt++;
    if ((trans_cb->buf_size - trans_cb->length) <= SPI_OUT_FRAME_OVERHEAD) {
        trans_cb->flag = SPI_OUT_TRANS_CB_FLAG_NEED_QUEUE;
    }
    return;
}

IRAM_ATTR static void spi_out_log_cb_write_packet_loss(spi_out_log_cb_t *log_cb, uint8_t flag)
{
    if (!log_cb->bytes_loss_cnt && !log_cb->trans_loss_cnt) {
        return;
    }
    if (spi_out_log_cb_check_trans(log_cb, SPI_OUT_PACKET_LOSS_FRAME_SIZE) == 0) {
        uint8_t packet_loss_frame[SPI_OUT_PACKET_LOSS_FRAME_SIZE];
        packet_loss_frame[0] = flag;
        memcpy(packet_loss_frame + 1, (uint8_t *)&log_cb->bytes_loss_cnt, 4);
        packet_loss_frame[5] = log_cb->trans_loss_cnt;
        spi_out_log_cb_write(log_cb, packet_loss_frame, SPI_OUT_PACKET_LOSS_FRAME_SIZE, NULL, 0, BLE_LOG_SPI_OUT_SOURCE_LOSS);

        log_cb->bytes_loss_cnt = 0;
        log_cb->trans_loss_cnt = 0;
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

#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
    // Initialize flushout timer
    esp_timer_create_args_t timer_args = {
        .callback = (esp_timer_cb_t)esp_timer_cb_ul_log_flushout,
        .dispatch_method = ESP_TIMER_TASK
    };
    if (esp_timer_create(&timer_args, &ul_log_flushout_timer) != ESP_OK) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize flushout timer upper layer task log!");
        goto timer_init_failed;
    }
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED

    // Initialize log control block
    if (spi_out_log_cb_init(&ul_log_cb, CONFIG_BT_BLE_LOG_SPI_OUT_UL_TASK_BUF_SIZE) != 0) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize log control blocks for upper layer task log!");
        goto log_cb_init_failed;
    }

    // Initialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to initialize upper layer task log!");
    ul_log_inited = true;
    return 0;

log_cb_init_failed:
#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
    esp_timer_delete(ul_log_flushout_timer);
timer_init_failed:
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
    vSemaphoreDelete(ul_log_mutex);
mutex_init_failed:
    return -1;
}

static void spi_out_ul_log_deinit(void)
{
    if (!ul_log_inited) {
        return;
    }

#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
    esp_timer_stop(ul_log_flushout_timer);
    esp_timer_delete(ul_log_flushout_timer);
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED

    xSemaphoreTake(ul_log_mutex, portMAX_DELAY);
    spi_out_log_cb_deinit(&ul_log_cb);
    xSemaphoreGive(ul_log_mutex);

    vSemaphoreDelete(ul_log_mutex);
    ul_log_mutex = NULL;

    ESP_LOGI(BLE_LOG_TAG, "Succeeded to deinitialize upper layer log!");
    ul_log_inited = false;
    return;
}

#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
IRAM_ATTR static void esp_timer_cb_ul_log_flushout(void)
{
    xSemaphoreTake(ul_log_mutex, portMAX_DELAY);
    spi_out_log_cb_flush_trans(ul_log_cb);
    spi_out_log_cb_append_trans(ul_log_cb);
    xSemaphoreGive(ul_log_mutex);

    esp_timer_start_once(ul_log_flushout_timer, SPI_OUT_FLUSHOUT_TIMEOUT);
}
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED

#if CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED
static int spi_out_ll_log_init(void)
{
    if (ll_log_inited) {
        return 0;
    }

#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
    // Initialize flushout timer
    esp_timer_create_args_t timer_args = {
        .callback = (esp_timer_cb_t)esp_timer_cb_ll_log_flushout,
        .dispatch_method = ESP_TIMER_TASK
    };
    if (esp_timer_create(&timer_args, &ll_log_flushout_timer) != ESP_OK) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize flushout timer for controller log!");
        goto timer_init_failed;
    }
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED

    // Initialize log control blocks for controller task & ISR logs
    if (spi_out_log_cb_init(&ll_task_log_cb, CONFIG_BT_BLE_LOG_SPI_OUT_LL_TASK_BUF_SIZE) != 0) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize log control blocks for controller task!");
        goto task_log_cb_init_failed;
    }
    if (spi_out_log_cb_init(&ll_isr_log_cb, CONFIG_BT_BLE_LOG_SPI_OUT_LL_ISR_BUF_SIZE) != 0) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize log control blocks for controller ISR!");
        goto isr_log_cb_init_failed;
    }

    // Initialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to initialize log control blocks for controller task & ISR!");
    ll_log_inited = true;
    return 0;

isr_log_cb_init_failed:
    spi_out_log_cb_deinit(&ll_task_log_cb);
task_log_cb_init_failed:
#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
    esp_timer_delete(ll_log_flushout_timer);
timer_init_failed:
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
    return -1;
}

static void spi_out_ll_log_deinit(void)
{
    if (!ll_log_inited) {
        return;
    }

#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
    esp_timer_stop(ll_log_flushout_timer);
    esp_timer_delete(ll_log_flushout_timer);
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED

    spi_out_log_cb_deinit(&ll_isr_log_cb);
    spi_out_log_cb_deinit(&ll_task_log_cb);

    // Deinitialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to deinitialize controller log!");
    ll_log_inited = false;
    return;
}

IRAM_ATTR static void spi_out_ll_log_ev_proc(void)
{
#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
    // Request from flushout timer
    spi_out_log_cb_flush_trans(ll_isr_log_cb);
    spi_out_log_cb_append_trans(ll_isr_log_cb);

    spi_out_log_cb_flush_trans(ll_task_log_cb);
    spi_out_log_cb_append_trans(ll_task_log_cb);

    esp_timer_start_once(ll_log_flushout_timer, SPI_OUT_FLUSHOUT_TIMEOUT);
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
    return;
}

#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
IRAM_ATTR static void esp_timer_cb_ll_log_flushout(void)
{
    BLE_LOG_LL_PUT_EV;
}
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
static int spi_out_ts_sync_init(void)
{
    if (ts_sync_inited) {
        return 0;
    }

    // Initialize sync timer
    esp_timer_create_args_t timer_args = {
        .callback = (esp_timer_cb_t)esp_timer_cb_ts_sync,
        .dispatch_method = ESP_TIMER_TASK
    };
    if (esp_timer_create(&timer_args, &ts_sync_timer) != ESP_OK) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize timestamp synchronizer timer!");
        goto timer_init_failed;
    }

    // Initialize sync IO
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1UL << CONFIG_BT_BLE_LOG_SPI_OUT_SYNC_IO_NUM),
        .pull_down_en = 0,
        .pull_up_en = 0
    };
    if (gpio_config(&io_conf) != ESP_OK) {
        ESP_LOGE(BLE_LOG_TAG, "Failed to initialize timestamp synchronizer IO!");
        goto gpio_init_failed;
    }

    // Initialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to initialize timestamp synchronizer!");
    sync_io_level = false;
    gpio_set_level(CONFIG_BT_BLE_LOG_SPI_OUT_SYNC_IO_NUM, sync_io_level);
    ts_sync_inited = true;
    return 0;

gpio_init_failed:
    esp_timer_delete(ts_sync_timer);
timer_init_failed:
    return -1;
}

static void spi_out_ts_sync_deinit(void)
{
    if (!ts_sync_inited) {
        return;
    }

    // Deinitialize timestamp synchronizer
    esp_timer_stop(ts_sync_timer);
    esp_timer_delete(ts_sync_timer);

    // Deinitialize sync IO
    sync_io_level = false;
    gpio_set_level(CONFIG_BT_BLE_LOG_SPI_OUT_SYNC_IO_NUM, sync_io_level);
    gpio_reset_pin(CONFIG_BT_BLE_LOG_SPI_OUT_SYNC_IO_NUM);

    // Deinitialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to deinitialize timestamp synchronizer!");
    ts_sync_inited = false;
    return;
}

// CRITICAL: This function is called in ESP Timer task
static void esp_timer_cb_ts_sync(void)
{
    // Initialize variables
    uint32_t lc_ts = 0;
    uint32_t esp_ts = 0;

    // Toggle sync IO
    sync_io_level = !sync_io_level;

    // Enter critical
    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&spinlock);

    // Get LC timestamp
    lc_ts = SPI_OUT_GET_LC_TIME;

    // Set sync IO level
    gpio_set_level(CONFIG_BT_BLE_LOG_SPI_OUT_SYNC_IO_NUM, (uint32_t)sync_io_level);

    // Get ESP timestamp
    esp_ts = esp_timer_get_time();
    portEXIT_CRITICAL(&spinlock);
    // Exit critical

    // Write timestamp sync log
    uint8_t sync_frame[9];
    sync_frame[0] = ((uint8_t)sync_io_level & 0xFF);
    memcpy(sync_frame + 1, &lc_ts, sizeof(lc_ts));
    memcpy(sync_frame + 5, &esp_ts, sizeof(esp_ts));
    ble_log_spi_out_write(BLE_LOG_SPI_OUT_SOURCE_SYNC, sync_frame, 9);
}
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

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
        .mosi_io_num = CONFIG_BT_BLE_LOG_SPI_OUT_MOSI_IO_NUM,
        .sclk_io_num = CONFIG_BT_BLE_LOG_SPI_OUT_SCLK_IO_NUM,
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
        .spics_io_num = CONFIG_BT_BLE_LOG_SPI_OUT_CS_IO_NUM,
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

#if CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED
    if (spi_out_ll_log_init() != 0) {
        goto ll_log_init_failed;
    }
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
    if (spi_out_ts_sync_init() != 0) {
        goto ts_sync_init_failed;
    }
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

    // Initialization done
    ESP_LOGI(BLE_LOG_TAG, "Succeeded to initialize BLE log SPI output interface!");
    spi_out_inited = true;

#if CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
    // Start flushout timer
    esp_timer_start_once(ul_log_flushout_timer, SPI_OUT_FLUSHOUT_TIMEOUT);

#if CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED
    esp_timer_start_once(ll_log_flushout_timer, SPI_OUT_FLUSHOUT_TIMEOUT);
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
    return 0;

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
ts_sync_init_failed:
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
#if CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED
    spi_out_ll_log_deinit();
ll_log_init_failed:
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED
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

    // Drain all queued transactions
    assert(spi_device_acquire_bus(spi_handle, portMAX_DELAY) == ESP_OK);
    spi_device_release_bus(spi_handle);

    // Remove SPI master
    spi_bus_remove_device(spi_handle);
    spi_handle = NULL;
    spi_bus_free(SPI_OUT_BUS);

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
    spi_out_ts_sync_deinit();
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

#if CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED
    spi_out_ll_log_deinit();
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED

    spi_out_ul_log_deinit();

    // Reset init flag
    spi_out_inited = false;
}

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
void ble_log_spi_out_ts_sync_start(void)
{
    // Check if SPI out is initialized
    if (!spi_out_inited) {
        return;
    }

    // Start timestamp sync timer
    if (ts_sync_timer) {
        if (!esp_timer_is_active(ts_sync_timer)) {
            esp_timer_start_periodic(ts_sync_timer, SPI_OUT_TS_SYNC_TIMEOUT);
        }
    }
}

void ble_log_spi_out_ts_sync_stop(void)
{
    // Check if SPI out is initialized
    if (!spi_out_inited) {
        return;
    }

    // Stop timestamp sync timer
    if (ts_sync_timer) {
        if (esp_timer_is_active(ts_sync_timer)) {
            esp_timer_stop(ts_sync_timer);
        }

        // Set sync IO to low level
        sync_io_level = 0;
        gpio_set_level(CONFIG_BT_BLE_LOG_SPI_OUT_SYNC_IO_NUM, (uint32_t)sync_io_level);
    }
}
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

#if CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED
// Only LL task has access to this API
IRAM_ATTR void ble_log_spi_out_ll_write(uint32_t len, const uint8_t *addr, uint32_t len_append,\
                                        const uint8_t *addr_append, uint32_t flag)
{
    if (!ll_log_inited) {
        return;
    }

    bool in_isr = (bool)(flag & SPI_OUT_INTERFACE_FLAG_IN_ISR);
    uint8_t source = in_isr ? BLE_LOG_SPI_OUT_SOURCE_ESP_ISR : BLE_LOG_SPI_OUT_SOURCE_ESP;
    spi_out_log_cb_t *log_cb = in_isr ? ll_isr_log_cb : ll_task_log_cb;
    uint16_t total_length = (uint16_t)(len + len_append);
    if (spi_out_log_cb_check_trans(log_cb, total_length) == 0) {
        spi_out_log_cb_write(log_cb, addr, (uint16_t)len, addr_append, (uint16_t)len_append, source);
        if (in_isr) {
            spi_out_log_cb_write_packet_loss(log_cb, SPI_OUT_PACKET_LOSS_LL_ISR);
        } else {
            spi_out_log_cb_append_trans(ll_isr_log_cb);
            spi_out_log_cb_append_trans(ll_task_log_cb);
            spi_out_log_cb_write_packet_loss(log_cb, SPI_OUT_PACKET_LOSS_LL_TASK);
        }
    }
    return;
}

IRAM_ATTR void ble_log_spi_out_ll_log_ev_proc(void)
{
    if (!ll_log_inited) {
        return;
    }
    return spi_out_ll_log_ev_proc();
}
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED

int ble_log_spi_out_write(uint8_t source, const uint8_t *addr, uint16_t len)
{
    if (!ul_log_inited) {
        return -1;
    }

    xSemaphoreTake(ul_log_mutex, portMAX_DELAY);
    if (spi_out_log_cb_check_trans(ul_log_cb, len) == 0) {
        spi_out_log_cb_write(ul_log_cb, addr, len, NULL, 0, source);
        spi_out_log_cb_append_trans(ul_log_cb);

        spi_out_log_cb_write_packet_loss(ul_log_cb, SPI_OUT_PACKET_LOSS_UL);
    }
    xSemaphoreGive(ul_log_mutex);
    return 0;
}

int ble_log_spi_out_printf(uint8_t source, const char *format, ...)
{
    if (!ul_log_inited) {
        return -1;
    }

    // Get arguments
    va_list args;
    va_start(args, format);

    // Get len as ref to allocate heap memory
    va_list args_copy;
    va_copy(args_copy, args);
    int len = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    // Length validation
    if ((len < 0) || (len > 0xFFFF)) {
        va_end(args);
        return -1;
    }

    // Allocate memory
    uint8_t *buffer = malloc(len + 1);
    if (!buffer) {
        va_end(args);
        return -1;
    }

    // Generate string
    vsnprintf((char *)buffer, len + 1, format, args);
    va_end(args);

    uint32_t esp_ts = esp_timer_get_time();
    xSemaphoreTake(ul_log_mutex, portMAX_DELAY);
    if (spi_out_log_cb_check_trans(ul_log_cb, sizeof(uint32_t) + len) == 0) {
        spi_out_log_cb_write(ul_log_cb, (const uint8_t *)&esp_ts, sizeof(uint32_t), (const uint8_t *)buffer, len, source);
        spi_out_log_cb_append_trans(ul_log_cb);

        spi_out_log_cb_write_packet_loss(ul_log_cb, SPI_OUT_PACKET_LOSS_UL);
    }
    xSemaphoreGive(ul_log_mutex);

    // Release
    free(buffer);
    return 0;
}

int ble_log_spi_out_printf_enh(uint8_t source, uint8_t level, const char *tag, const char *format, ...)
{
    if (!ul_log_inited) {
        return -1;
    }

    // Create log prefix in the format: "[level][tag] "
    char prefix[32];
    int prefix_len = snprintf(prefix, sizeof(prefix), "[%d][%s] ", level, tag ? tag : "NULL");

    // Compute the length of the formatted log message
    va_list args;
    va_start(args, format);
    va_list args_copy;
    va_copy(args_copy, args);
    int log_len = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    // Validate length
    if (log_len < 0 || log_len > 0xFFFF) {
        va_end(args);
        return -1;
    }

    // Compute total log length (prefix + formatted message)
    int total_len = prefix_len + log_len;

    // Allocate memory for the complete log message
    uint8_t *buffer = malloc(total_len + 1);
    if (!buffer) {
        va_end(args);
        return -1;
    }

    // Construct the final log message
    memcpy(buffer, prefix, prefix_len);  // Copy the prefix
    vsnprintf((char *)(buffer + prefix_len), log_len + 1, format, args);
    va_end(args);

    uint32_t esp_ts = esp_timer_get_time();
    xSemaphoreTake(ul_log_mutex, portMAX_DELAY);
    if (spi_out_log_cb_check_trans(ul_log_cb, sizeof(uint32_t) + total_len) == 0) {
        spi_out_log_cb_write(ul_log_cb, (const uint8_t *)&esp_ts, sizeof(uint32_t), (const uint8_t *)buffer, total_len, source);
        spi_out_log_cb_append_trans(ul_log_cb);

        spi_out_log_cb_write_packet_loss(ul_log_cb, SPI_OUT_PACKET_LOSS_UL);
    }
    xSemaphoreGive(ul_log_mutex);

    free(buffer);
    return 0;
}

int ble_log_spi_out_write_with_ts(uint8_t source, const uint8_t *addr, uint16_t len)
{
    if (!ul_log_inited) {
        return -1;
    }

    uint32_t esp_ts = esp_timer_get_time();
    xSemaphoreTake(ul_log_mutex, portMAX_DELAY);
    if (spi_out_log_cb_check_trans(ul_log_cb, sizeof(uint32_t) + len) == 0) {
        spi_out_log_cb_write(ul_log_cb, (const uint8_t *)&esp_ts, sizeof(uint32_t), addr, len, source);
        spi_out_log_cb_append_trans(ul_log_cb);

        spi_out_log_cb_write_packet_loss(ul_log_cb, SPI_OUT_PACKET_LOSS_UL);
    }
    xSemaphoreGive(ul_log_mutex);
    return 0;
}

void ble_log_spi_out_dump_all(void)
{
    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);

#if CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED
    if (ll_log_inited) {
        // Dump lower layer log buffer
        esp_rom_printf("[LL_ISR_LOG_DUMP_START:\n");
        spi_out_log_cb_dump(ll_isr_log_cb);
        esp_rom_printf("\n:LL_ISR_LOG_DUMP_END]\n\n");

        esp_rom_printf("[LL_TASK_LOG_DUMP_START:\n");
        spi_out_log_cb_dump(ll_task_log_cb);
        esp_rom_printf("\n:LL_TASK_LOG_DUMP_END]\n\n");
    }
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_LL_ENABLED

    if (ul_log_inited) {
        // Dump upper layer log buffer
        esp_rom_printf("[UL_LOG_DUMP_START:\n");
        spi_out_log_cb_dump(ul_log_cb);
        esp_rom_printf("\n:UL_LOG_DUMP_END]\n\n");
    }
    portEXIT_CRITICAL_SAFE(&spinlock);
}

#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
