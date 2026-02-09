/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "ble_log/ble_log_spi_out.h"

#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

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
#define SPI_OUT_FLUSH_TIMER_ENABLED             CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMER_ENABLED
#define SPI_OUT_FLUSH_TIMEOUT_US                (CONFIG_BT_BLE_LOG_SPI_OUT_FLUSH_TIMEOUT * 1000)
#define SPI_OUT_LE_AUDIO_ENABLED                CONFIG_BT_BLE_LOG_SPI_OUT_LE_AUDIO_ENABLED
#define SPI_OUT_LE_AUDIO_BUF_SIZE               CONFIG_BT_BLE_LOG_SPI_OUT_LE_AUDIO_BUF_SIZE
#define SPI_OUT_LE_AUDIO_TASK_CNT               CONFIG_BT_BLE_LOG_SPI_OUT_LE_AUDIO_TASK_CNT
#define SPI_OUT_HOST_ENABLED                    CONFIG_BT_BLE_LOG_SPI_OUT_HOST_ENABLED
#define SPI_OUT_HOST_BUF_SIZE                   CONFIG_BT_BLE_LOG_SPI_OUT_HOST_BUF_SIZE
#define SPI_OUT_HOST_TASK_CNT                   CONFIG_BT_BLE_LOG_SPI_OUT_HOST_TASK_CNT
#define SPI_OUT_HCI_ENABLED                     CONFIG_BT_BLE_LOG_SPI_OUT_HCI_ENABLED
#define SPI_OUT_HCI_BUF_SIZE                    CONFIG_BT_BLE_LOG_SPI_OUT_HCI_BUF_SIZE
#define SPI_OUT_HCI_TASK_CNT                    CONFIG_BT_BLE_LOG_SPI_OUT_HCI_TASK_CNT
#define SPI_OUT_MESH_ENABLED                    CONFIG_BT_BLE_LOG_SPI_OUT_MESH_ENABLED
#define SPI_OUT_MESH_BUF_SIZE                   CONFIG_BT_BLE_LOG_SPI_OUT_MESH_BUF_SIZE
#define SPI_OUT_MESH_TASK_CNT                   CONFIG_BT_BLE_LOG_SPI_OUT_MESH_TASK_CNT

// Private defines
#define BLE_LOG_TAG                             "BLE_LOG"
#define SPI_OUT_BUS                             SPI2_HOST
#define SPI_OUT_MAX_TRANSFER_SIZE               (10240)
#define SPI_OUT_TRANS_ITVL_MIN_US               (30)
#define SPI_OUT_LOG_STR_BUF_SIZE                (100)
#define SPI_OUT_MALLOC(size)                    heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#define SPI_OUT_TASK_PRIORITY                   (ESP_TASK_PRIO_MAX - 1)

#if CONFIG_IDF_TARGET_ARCH_RISCV
#define SPI_OUT_TASK_STACK_SIZE                 (1024)
#elif CONFIG_IDF_TARGET_ARCH_XTENSA
#define SPI_OUT_TASK_STACK_SIZE                 (2048)
#else
static_assert(false, "BLE Log SPI Out: Unsupported target architecture");
#endif /* CONFIG_IDF_TARGET_ARCH_RISCV */

#if SPI_OUT_TS_SYNC_ENABLED
#define SPI_OUT_TS_SYNC_TIMEOUT_MS              (1000)
#define SPI_OUT_TS_SYNC_TIMEOUT_US              (SPI_OUT_TS_SYNC_TIMEOUT_MS * 1000)
#endif // SPI_OUT_TS_SYNC_ENABLED

// Queue size defines
#define SPI_OUT_PING_PONG_BUF_CNT               (2)
#define SPI_OUT_UL_QUEUE_SIZE                   (SPI_OUT_PING_PONG_BUF_CNT)

#if SPI_OUT_LL_ENABLED
#define SPI_OUT_LL_QUEUE_SIZE                   (3 * SPI_OUT_PING_PONG_BUF_CNT)
#else
#define SPI_OUT_LL_QUEUE_SIZE                   (0)
#endif // SPI_OUT_LL_ENABLED

#if SPI_OUT_LE_AUDIO_ENABLED
#define SPI_OUT_LE_AUDIO_QUEUE_SIZE             (SPI_OUT_PING_PONG_BUF_CNT)
#else
#define SPI_OUT_LE_AUDIO_QUEUE_SIZE             (0)
#endif // SPI_OUT_LE_AUDIO_ENABLED

#if SPI_OUT_HOST_ENABLED
#define SPI_OUT_HOST_QUEUE_SIZE                 (SPI_OUT_PING_PONG_BUF_CNT)
#else
#define SPI_OUT_HOST_QUEUE_SIZE                 (0)
#endif // SPI_OUT_HOST_ENABLED

#if SPI_OUT_HCI_ENABLED
#define SPI_OUT_HCI_QUEUE_SIZE                  (SPI_OUT_PING_PONG_BUF_CNT)
#else
#define SPI_OUT_HCI_QUEUE_SIZE                  (0)
#endif // SPI_OUT_HCI_ENABLED

#if SPI_OUT_MESH_ENABLED
#define SPI_OUT_MESH_QUEUE_SIZE                 (SPI_OUT_PING_PONG_BUF_CNT)
#else
#define SPI_OUT_MESH_QUEUE_SIZE                 (0)
#endif // SPI_OUT_MESH_ENABLED

#define SPI_OUT_SPI_MASTER_QUEUE_SIZE           (SPI_OUT_UL_QUEUE_SIZE +\
                                                 SPI_OUT_LL_QUEUE_SIZE +\
                                                 SPI_OUT_LE_AUDIO_QUEUE_SIZE +\
                                                 SPI_OUT_HOST_QUEUE_SIZE +\
                                                 SPI_OUT_HCI_QUEUE_SIZE +\
                                                 SPI_OUT_MESH_QUEUE_SIZE)

#if SPI_OUT_LL_ENABLED && CONFIG_SOC_ESP_NIMBLE_CONTROLLER
#include "os/os_mbuf.h"
#endif /* SPI_OUT_LL_ENABLED && CONFIG_SOC_ESP_NIMBLE_CONTROLLER */

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
    uint8_t type;
    uint16_t lost_frame_cnt;
    uint32_t lost_bytes_cnt;
    uint32_t frame_sn;
} spi_out_log_cb_t;

typedef struct {
    TaskHandle_t task_handle;
    spi_out_log_cb_t *log_cb;
    uint8_t *str_buf;
} task_map_t;

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

typedef struct {
    uint8_t io_level;
    uint32_t lc_ts;
    uint32_t os_ts;
} __attribute__((packed)) ts_sync_data_t;

// Private enums
enum {
    TRANS_CB_FLAG_AVAILABLE = 0,
    TRANS_CB_FLAG_NEED_QUEUE,
    TRANS_CB_FLAG_IN_QUEUE,
};

enum {
    LOG_CB_TYPE_UL = 0,
    LOG_CB_TYPE_LL,
    LOG_CB_TYPE_LE_AUDIO,
    LOG_CB_TYPE_HOST,
    LOG_CB_TYPE_HCI,
    LOG_CB_TYPE_MESH,
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
static TaskHandle_t spi_out_task_handle = NULL;
static bool spi_out_inited = false;
static bool spi_out_enabled = false;
static spi_device_handle_t spi_handle = NULL;
static uint32_t last_tx_done_ts = 0;

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
#endif // SPI_OUT_TS_SYNC_ENABLED

#if SPI_OUT_FLUSH_TIMER_ENABLED
static esp_timer_handle_t flush_timer = NULL;
#endif // SPI_OUT_FLUSH_TIMER_ENABLED

// Extern function declarations
extern void esp_panic_handler_feed_wdts(void);

// Private function declarations
static int spi_out_init_trans(spi_out_trans_cb_t **trans_cb, uint16_t buf_size);
static void spi_out_deinit_trans(spi_out_trans_cb_t **trans_cb);
static void spi_out_tx_done_cb(spi_transaction_t *ret_trans);
static inline void spi_out_append_trans(spi_out_trans_cb_t *trans_cb);

static int spi_out_log_cb_init(spi_out_log_cb_t **log_cb, uint16_t buf_size, uint8_t type, uint8_t idx);
static void spi_out_log_cb_deinit(spi_out_log_cb_t **log_cb);
static inline bool spi_out_log_cb_check_trans(spi_out_log_cb_t *log_cb, uint16_t len, bool *need_append);
static inline void spi_out_log_cb_append_trans(spi_out_log_cb_t *log_cb);
static inline void spi_out_log_cb_flush_trans(spi_out_log_cb_t *log_cb);
static bool spi_out_log_cb_write(spi_out_log_cb_t *log_cb, const uint8_t *addr, uint16_t len,
                                 const uint8_t *addr_append, uint16_t len_append, uint8_t source,
                                 bool with_checksum, bool omdata);
static void spi_out_log_cb_write_loss(spi_out_log_cb_t *log_cb);
static void spi_out_log_cb_dump(spi_out_log_cb_t *log_cb);

#if SPI_OUT_HOST_ENABLED || SPI_OUT_MESH_ENABLED || SPI_OUT_HCI_ENABLED || SPI_OUT_LE_AUDIO_ENABLED
static bool spi_out_get_task_mapping(task_map_t *map, size_t num,
                                     spi_out_log_cb_t **log_cb, uint8_t **str_buf);
#endif // SPI_OUT_HOST_ENABLED || SPI_OUT_MESH_ENABLED || SPI_OUT_HCI_ENABLED || SPI_OUT_LE_AUDIO_ENABLED

static void spi_out_log_flush(void);

static void spi_out_write_hex(spi_out_log_cb_t *log_cb, uint8_t source,
                              const uint8_t *addr, uint16_t len, bool with_ts);

#if SPI_OUT_HOST_ENABLED || SPI_OUT_MESH_ENABLED
static int spi_out_write_str(uint8_t *str_buf, const char *format, va_list args, int offset);
#endif // SPI_OUT_HOST_ENABLED || SPI_OUT_MESH_ENABLED

#if SPI_OUT_LL_ENABLED
static int spi_out_ll_log_init(void);
static void spi_out_ll_log_deinit(void);
static void spi_out_ll_log_flush(void);
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
#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
extern uint32_t lld_read_clock_us(void);
#define SPI_OUT_GET_LC_TIME lld_read_clock_us()
#else
#define SPI_OUT_GET_LC_TIME esp_timer_get_time()
#endif

#endif // SPI_OUT_TS_SYNC_ENABLED

// Private templates
#define IF_1(...) __VA_ARGS__
#define IF_0(...)

#define LOG_MODULE_INIT_FLAG(ID)        (ID##_log_inited)
#define LOG_MODULE_CB_ARR(ID)           (ID##_log_cb)
#define LOG_MODULE_CB_CNT(ID)           (ID##_log_cb_cnt)
#define LOG_MODULE_CB(ID, IDX)          (ID##_log_cb[IDX])
#define LOG_MODULE_TASK_MAP(ID)         (ID##_task_map)
#define LOG_MODULE_MUTEX(ID)            (ID##_log_mutex)
#define LOG_MODULE_STR_BUF_ARR(ID)      (ID##_log_str_buf)
#define LOG_MODULE_STR_BUF(ID, IDX)     (ID##_log_str_buf[IDX])
#define LOG_MODULE_INIT(ID)             (spi_out_##ID##_log_init)
#define LOG_MODULE_DEINIT(ID)           (spi_out_##ID##_log_deinit)
#define LOG_MODULE_FLUSH(ID)            (spi_out_##ID##_log_flush)
#define LOG_MODULE_DUMP(ID)                                                         \
    do {                                                                            \
        if (LOG_MODULE_INIT_FLAG(ID)) {                                             \
            for (int i = 0; i < LOG_MODULE_CB_CNT(ID); i++) {                       \
                spi_out_log_cb_dump(LOG_MODULE_CB(ID, i));                          \
            }                                                                       \
        }                                                                           \
    } while (0)

#define DECLARE_LOG_MODULE(ID, TYPE, BUF_SIZE, NUM_CB, USE_MUTEX, USE_STR)          \
    static bool LOG_MODULE_INIT_FLAG(ID) = false;                                   \
    static spi_out_log_cb_t *LOG_MODULE_CB_ARR(ID)[NUM_CB] = { NULL };              \
    static const size_t LOG_MODULE_CB_CNT(ID) = (NUM_CB);                           \
    static task_map_t LOG_MODULE_TASK_MAP(ID)[NUM_CB] = {0};                        \
    IF_##USE_MUTEX(static SemaphoreHandle_t LOG_MODULE_MUTEX(ID) = NULL;)           \
    IF_##USE_STR(static uint8_t *LOG_MODULE_STR_BUF_ARR(ID)[NUM_CB] = {0};)         \
                                                                                    \
    static int LOG_MODULE_INIT(ID)(void);                                           \
    static void LOG_MODULE_DEINIT(ID)(void);                                        \
    static void LOG_MODULE_FLUSH(ID)(void);                                         \
                                                                                    \
    static int LOG_MODULE_INIT(ID)(void) {                                          \
        if (LOG_MODULE_INIT_FLAG(ID)) {                                             \
            return 0;                                                               \
        }                                                                           \
        IF_##USE_MUTEX(                                                             \
            LOG_MODULE_MUTEX(ID) = xSemaphoreCreateMutex();                         \
            if (!LOG_MODULE_MUTEX(ID)) {                                            \
                goto failed;                                                        \
            }                                                                       \
        )                                                                           \
        IF_##USE_STR(                                                               \
            for (size_t i = 0; i < LOG_MODULE_CB_CNT(ID); i++) {                    \
                LOG_MODULE_STR_BUF(ID, i) = (uint8_t *)SPI_OUT_MALLOC(              \
                    SPI_OUT_LOG_STR_BUF_SIZE                                        \
                );                                                                  \
                if (!LOG_MODULE_STR_BUF(ID, i)) {                                   \
                    goto failed;                                                    \
                }                                                                   \
                LOG_MODULE_TASK_MAP(ID)[i].str_buf = LOG_MODULE_STR_BUF(ID, i);     \
            }                                                                       \
        )                                                                           \
        for (size_t i = 0; i < LOG_MODULE_CB_CNT(ID); i++) {                        \
            if (spi_out_log_cb_init(&LOG_MODULE_CB(ID, i), BUF_SIZE, TYPE, i) != 0) \
            {                                                                       \
                goto failed;                                                        \
            }                                                                       \
            LOG_MODULE_TASK_MAP(ID)[i].log_cb = LOG_MODULE_CB(ID, i);               \
        }                                                                           \
        LOG_MODULE_INIT_FLAG(ID) = true;                                            \
        return 0;                                                                   \
    failed:                                                                         \
        LOG_MODULE_DEINIT(ID)();                                                    \
        return -1;                                                                  \
    }                                                                               \
    static void LOG_MODULE_DEINIT(ID)(void) {                                       \
        LOG_MODULE_INIT_FLAG(ID) = false;                                           \
        IF_##USE_MUTEX(                                                             \
            if (!LOG_MODULE_MUTEX(ID)) {                                            \
                return;                                                             \
            }                                                                       \
            xSemaphoreTake(LOG_MODULE_MUTEX(ID), portMAX_DELAY);                    \
        )                                                                           \
        IF_##USE_STR(                                                               \
            for (size_t i = 0; i < LOG_MODULE_CB_CNT(ID); i++) {                    \
                if (LOG_MODULE_STR_BUF(ID, i)) {                                    \
                    free(LOG_MODULE_STR_BUF(ID, i));                                \
                    LOG_MODULE_STR_BUF(ID, i) = NULL;                               \
                }                                                                   \
            }                                                                       \
        )                                                                           \
        for (size_t i = 0; i < LOG_MODULE_CB_CNT(ID); i++) {                        \
            spi_out_log_cb_deinit(&LOG_MODULE_CB(ID, i));                           \
            LOG_MODULE_CB(ID, i) = NULL;                                            \
        }                                                                           \
        for (size_t i = 0; i < LOG_MODULE_CB_CNT(ID); i++) {                        \
            memset(&LOG_MODULE_TASK_MAP(ID)[i], 0, sizeof(task_map_t));             \
        }                                                                           \
        IF_##USE_MUTEX(                                                             \
            xSemaphoreGive(LOG_MODULE_MUTEX(ID));                                   \
            vSemaphoreDelete(LOG_MODULE_MUTEX(ID));                                 \
            LOG_MODULE_MUTEX(ID) = NULL;                                            \
        )                                                                           \
    }                                                                               \
    static void LOG_MODULE_FLUSH(ID)(void) {                                        \
        if (!LOG_MODULE_INIT_FLAG(ID)) {                                            \
            return;                                                                 \
        }                                                                           \
        IF_##USE_MUTEX(                                                             \
            xSemaphoreTake(LOG_MODULE_MUTEX(ID), portMAX_DELAY);                    \
        )                                                                           \
        for (size_t i = 0; i < LOG_MODULE_CB_CNT(ID); ++i) {                        \
            spi_out_log_cb_write_loss(LOG_MODULE_CB(ID, i));                        \
            spi_out_log_cb_flush_trans(LOG_MODULE_CB(ID, i));                       \
            spi_out_log_cb_append_trans(LOG_MODULE_CB(ID, i));                      \
        }                                                                           \
        IF_##USE_MUTEX(                                                             \
            xSemaphoreGive(LOG_MODULE_MUTEX(ID));                                   \
        )                                                                           \
    }

DECLARE_LOG_MODULE(ul, LOG_CB_TYPE_UL, SPI_OUT_UL_TASK_BUF_SIZE, 1, 1, 1)

#if SPI_OUT_LE_AUDIO_ENABLED
DECLARE_LOG_MODULE(le_audio, LOG_CB_TYPE_LE_AUDIO, SPI_OUT_LE_AUDIO_BUF_SIZE,
                   SPI_OUT_LE_AUDIO_TASK_CNT, 0, 0)
#endif // SPI_OUT_LE_AUDIO_ENABLED

#if SPI_OUT_HOST_ENABLED
DECLARE_LOG_MODULE(host, LOG_CB_TYPE_HOST, SPI_OUT_HOST_BUF_SIZE,
                   SPI_OUT_HOST_TASK_CNT, 0, 1)
#endif // SPI_OUT_HOST_ENABLED

#if SPI_OUT_HCI_ENABLED
DECLARE_LOG_MODULE(hci, LOG_CB_TYPE_HCI, SPI_OUT_HCI_BUF_SIZE,
                   SPI_OUT_HCI_TASK_CNT, 0, 0)
#endif // SPI_OUT_HCI_ENABLED

#if SPI_OUT_MESH_ENABLED
DECLARE_LOG_MODULE(mesh, LOG_CB_TYPE_MESH, SPI_OUT_MESH_BUF_SIZE,
                  SPI_OUT_MESH_TASK_CNT, 0, 1)
#endif // SPI_OUT_MESH_ENABLED

// Private macros
#define SPI_OUT_FRAME_HEAD_LEN      (sizeof(frame_head_t))
#define SPI_OUT_FRAME_TAIL_LEN      (sizeof(uint32_t))
#define SPI_OUT_FRAME_OVERHEAD      (SPI_OUT_FRAME_HEAD_LEN + SPI_OUT_FRAME_TAIL_LEN)
#define SPI_OUT_GET_FRAME_SN(VAR)   __atomic_fetch_add(VAR, 1, __ATOMIC_RELAXED)
#define SPI_OUT_TASK_NOTIFY_FROM_ISR                                                    \
    do {                                                                                \
        if (spi_out_task_handle) {                                                      \
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;                              \
            vTaskNotifyGiveFromISR(spi_out_task_handle, &xHigherPriorityTaskWoken);     \
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);                               \
        }                                                                               \
    } while (0)
#define SPI_OUT_TASK_NOTIFY_FROM_TASK                                                   \
    do {                                                                                \
        if (spi_out_task_handle) {                                                      \
            xTaskNotifyGive(spi_out_task_handle);                                       \
        }                                                                               \
    } while (0)

// Private functions
static int spi_out_init_trans(spi_out_trans_cb_t **trans_cb, uint16_t buf_size)
{
    // Memory allocations
    *trans_cb = (spi_out_trans_cb_t *)SPI_OUT_MALLOC(sizeof(spi_out_trans_cb_t));
    if (!(*trans_cb)) {
        return -1;
    }
    memset(*trans_cb, 0, sizeof(spi_out_trans_cb_t));

    uint8_t *buf = (uint8_t *)SPI_OUT_MALLOC((size_t)buf_size);
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
    SPI_OUT_TASK_NOTIFY_FROM_ISR;
    return;

recycle:
    trans_cb->length = 0;
    trans_cb->flag = TRANS_CB_FLAG_AVAILABLE;
    return;
}

static int spi_out_log_cb_init(spi_out_log_cb_t **log_cb, uint16_t buf_size, uint8_t type, uint8_t idx)
{
    // Initialize log control block
    *log_cb = (spi_out_log_cb_t *)SPI_OUT_MALLOC(sizeof(spi_out_log_cb_t));
    if (!(*log_cb)) {
        return -1;
    }
    memset(*log_cb, 0, sizeof(spi_out_log_cb_t));

    // Initialize transactions
    int ret = 0;
    for (uint8_t i = 0; i < 2; i++) {
        ret |= spi_out_init_trans(&((*log_cb)->trans_cb[i]), buf_size);
    }
    if (ret != 0) {
        spi_out_log_cb_deinit(log_cb);
        return -1;
    }

    (*log_cb)->type = (type << 4) | (idx);
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
                                           bool with_checksum, bool omdata)
{
    spi_out_trans_cb_t *trans_cb = log_cb->trans_cb[log_cb->trans_cb_idx];

    uint8_t *buf = (uint8_t *)trans_cb->trans.tx_buffer + trans_cb->length;
    uint16_t total_length = len + len_append;
    frame_head_t head = {
        .length = total_length,
        .source = source,
        .type = log_cb->type,
        .frame_sn = SPI_OUT_GET_FRAME_SN(&(log_cb->frame_sn)) & 0xFFFF,
    };

    memcpy(buf, (const uint8_t *)&head, SPI_OUT_FRAME_HEAD_LEN);
    memcpy(buf + SPI_OUT_FRAME_HEAD_LEN, addr, len);
    if (len_append && addr_append) {
#if SPI_OUT_LL_ENABLED && CONFIG_SOC_ESP_NIMBLE_CONTROLLER
        if (omdata) {
            os_mbuf_copydata((struct os_mbuf *)addr_append, 0,
                             len_append, buf + SPI_OUT_FRAME_HEAD_LEN + len);
        }
        else
#endif /* SPI_OUT_LL_ENABLED && CONFIG_SOC_ESP_NIMBLE_CONTROLLER */
        {
            memcpy(buf + SPI_OUT_FRAME_HEAD_LEN + len, addr_append, len_append);
        }
    }

    uint32_t checksum = 0;
    for (int i = 0; i < SPI_OUT_FRAME_HEAD_LEN + total_length; i++) {
        checksum += buf[i];
    }
    memcpy(buf + SPI_OUT_FRAME_HEAD_LEN + total_length, &checksum, SPI_OUT_FRAME_TAIL_LEN);

    trans_cb->length += total_length + SPI_OUT_FRAME_OVERHEAD;
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
                             NULL, 0, BLE_LOG_SPI_OUT_SOURCE_LOSS, true, false);

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

static void spi_out_update_task_mapping(int idx, void *ptr)
{
    // It is a must to clear task handle after task deletion
    task_map_t *entry = (task_map_t *)ptr;
    entry->task_handle = NULL;
}

#if SPI_OUT_HOST_ENABLED || SPI_OUT_MESH_ENABLED || SPI_OUT_HCI_ENABLED || SPI_OUT_LE_AUDIO_ENABLED
static bool spi_out_get_task_mapping(task_map_t *map, size_t num,
                                     spi_out_log_cb_t **log_cb, uint8_t **str_buf)
{
    if (!map || !log_cb) {
        return false;
    }

    // Shall not be called in ISR
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();
    if (!handle) {
        return false;
    }

    // Check if the given task handle is already in map
    for (size_t i = 0; i < num; i++) {
        task_map_t *entry = &map[i];
        if (entry->task_handle == handle) {
            *log_cb = entry->log_cb;
            if (str_buf) {
                *str_buf = entry->str_buf;
            }
            return true;
        }
    }

    // Task handle not in map, try to allocate free slot
    bool ret = false;
    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);
    for (size_t i = 0; i < num; i++) {
        task_map_t *entry = &map[i];
        if (entry->task_handle == NULL) {
            vTaskSetThreadLocalStoragePointerAndDelCallback(
                NULL, 0, (void *)entry, spi_out_update_task_mapping
            );
            entry->task_handle = handle;
            *log_cb = entry->log_cb;
            if (str_buf) {
                *str_buf = entry->str_buf;
            }
            ret = true;
            break;
        }
    }
    portEXIT_CRITICAL_SAFE(&spinlock);
    return ret;
}
#endif // SPI_OUT_HOST_ENABLED || SPI_OUT_MESH_ENABLED || SPI_OUT_HCI_ENABLED || SPI_OUT_LE_AUDIO_ENABLED

static void spi_out_log_flush(void)
{
    LOG_MODULE_FLUSH(ul)();

#if SPI_OUT_LL_ENABLED
    ll_ev_flags |= BIT(LL_EV_FLAG_FLUSH_LOG);
    SPI_OUT_TASK_NOTIFY_FROM_TASK;
#endif // SPI_OUT_LL_ENABLED

#if SPI_OUT_LE_AUDIO_ENABLED
    LOG_MODULE_FLUSH(le_audio)();
#endif // SPI_OUT_LE_AUDIO_ENABLED

#if SPI_OUT_HOST_ENABLED
    LOG_MODULE_FLUSH(host)();
#endif // SPI_OUT_HOST_ENABLED

#if SPI_OUT_HCI_ENABLED
    LOG_MODULE_FLUSH(hci)();
#endif // SPI_OUT_HCI_ENABLED

#if SPI_OUT_MESH_ENABLED
    LOG_MODULE_FLUSH(mesh)();
#endif // SPI_OUT_MESH_ENABLED
}

#if SPI_OUT_FLUSH_TIMER_ENABLED
// Context: ESP timer
static void esp_timer_cb_log_flush(void)
{
    spi_out_log_flush();
    esp_timer_start_once(flush_timer, SPI_OUT_FLUSH_TIMEOUT_US);
}
#endif // SPI_OUT_FLUSH_TIMER_ENABLED

static void spi_out_write_hex(spi_out_log_cb_t *log_cb, uint8_t source,
                              const uint8_t *addr, uint16_t len, bool with_ts)
{
    uint16_t total_len = with_ts? (len + sizeof(uint32_t)): len;
    bool need_append;
    if (spi_out_log_cb_check_trans(log_cb, total_len, &need_append)) {
        if (with_ts) {
            uint32_t os_ts = pdTICKS_TO_MS(xTaskGetTickCount());
            need_append |= spi_out_log_cb_write(log_cb, (const uint8_t *)&os_ts,
                                                sizeof(uint32_t), addr, len, source, true, false);
        } else {
            need_append |= spi_out_log_cb_write(log_cb, addr, len, NULL, 0, source, true, false);
        }
    }
    if (need_append) {
        spi_out_log_cb_append_trans(log_cb);
    }
    spi_out_log_cb_write_loss(log_cb);
}

#if SPI_OUT_HOST_ENABLED || SPI_OUT_MESH_ENABLED
static int spi_out_write_str(uint8_t *str_buf, const char *format, va_list args, int offset)
{
    int len = vsnprintf((char *)(str_buf + offset), SPI_OUT_LOG_STR_BUF_SIZE - offset, format, args);
    if (len < 0) {
        return 0;
    }
    len += offset;

    if (len >= SPI_OUT_LOG_STR_BUF_SIZE) {
        len = SPI_OUT_LOG_STR_BUF_SIZE - 1;
        str_buf[len] = '\0';
    }
    return len;
}
#endif // SPI_OUT_HOST_ENABLED || SPI_OUT_MESH_ENABLED

#if SPI_OUT_LL_ENABLED
static int spi_out_ll_log_init(void)
{
    if (ll_log_inited) {
        return 0;
    }

    // Initialize log control blocks for controller task & ISR logs
    if (spi_out_log_cb_init(&ll_task_log_cb, SPI_OUT_LL_TASK_BUF_SIZE, LOG_CB_TYPE_LL, 0) != 0) {
        goto task_log_cb_init_failed;
    }
    if (spi_out_log_cb_init(&ll_isr_log_cb, SPI_OUT_LL_ISR_BUF_SIZE, LOG_CB_TYPE_LL, 1) != 0) {
        goto isr_log_cb_init_failed;
    }
    if (spi_out_log_cb_init(&ll_hci_log_cb, SPI_OUT_LL_HCI_BUF_SIZE, LOG_CB_TYPE_LL, 2) != 0) {
        goto hci_log_cb_init_failed;
    }

    // Initialization done
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
    ll_log_inited = false;
    return;
}

static void spi_out_ll_log_flush(void)
{
    if (!ll_log_inited) {
        return;
    }

    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);
    spi_out_log_cb_flush_trans(ll_task_log_cb);
    spi_out_log_cb_flush_trans(ll_hci_log_cb);
    spi_out_log_cb_flush_trans(ll_isr_log_cb);
    portEXIT_CRITICAL_SAFE(&spinlock);

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

    // Initialize sync IO
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT(SPI_OUT_SYNC_IO_NUM),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE
    };
    if (gpio_config(&io_conf) != ESP_OK) {
        goto failed;
    }

    // Initialization done
    ts_sync_inited = true;
    return 0;

failed:
    return -1;
}

static void spi_out_ts_sync_deinit(void)
{
    ts_sync_inited = false;

    spi_out_ts_sync_enable(false);
    gpio_reset_pin(SPI_OUT_SYNC_IO_NUM);
}

static void spi_out_ts_sync_enable(bool enable)
{
    // Update ts sync status
    ts_sync_enabled = enable;
    if (!enable && !ts_sync_data.io_level) {
        gpio_set_level(SPI_OUT_SYNC_IO_NUM, 1);
    }
    ts_sync_data.io_level = 0;
    gpio_set_level(SPI_OUT_SYNC_IO_NUM, 0);
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

    // Get OS timestamp
    ts_sync_data.os_ts = pdTICKS_TO_MS(xTaskGetTickCountFromISR());
    portEXIT_CRITICAL(&spinlock);
    // Exit critical
}
#endif // SPI_OUT_TS_SYNC_ENABLED

static void spi_out_task(void *pvParameters)
{
    while (1) {
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) {
#if SPI_OUT_LL_ENABLED
            ble_log_spi_out_ll_log_ev_proc();
#endif // SPI_OUT_LL_ENABLED
#if SPI_OUT_TS_SYNC_ENABLED
        if (ts_sync_inited && ts_sync_enabled) {
            uint32_t curr_os_ts = pdTICKS_TO_MS(xTaskGetTickCount());
            if ((curr_os_ts - ts_sync_data.os_ts) > SPI_OUT_TS_SYNC_TIMEOUT_MS) {
                spi_out_ts_sync_toggle();
                ble_log_spi_out_write(BLE_LOG_SPI_OUT_SOURCE_SYNC,
                                      (const uint8_t *)&ts_sync_data, sizeof(ts_sync_data_t));
            }
        }
#endif // SPI_OUT_TS_SYNC_ENABLED
        }
    }
}

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
        goto failed;
    }
    if (spi_bus_add_device(SPI_OUT_BUS, &dev_config, &spi_handle) != ESP_OK) {
        goto failed;
    }

    if (LOG_MODULE_INIT(ul)() != 0) {
        goto failed;
    }

#if SPI_OUT_LL_ENABLED
    if (spi_out_ll_log_init() != 0) {
        goto failed;
    }
#endif // SPI_OUT_LL_ENABLED

#if SPI_OUT_TS_SYNC_ENABLED
    if (spi_out_ts_sync_init() != 0) {
        goto failed;
    }
#endif // SPI_OUT_TS_SYNC_ENABLED

#if SPI_OUT_LE_AUDIO_ENABLED
    if (LOG_MODULE_INIT(le_audio)() != 0) {
        goto failed;
    }
#endif // SPI_OUT_LE_AUDIO_ENABLED

#if SPI_OUT_HOST_ENABLED
    if (LOG_MODULE_INIT(host)() != 0) {
        goto failed;
    }
#endif // SPI_OUT_HOST_ENABLED

#if SPI_OUT_HCI_ENABLED
    if (LOG_MODULE_INIT(hci)() != 0) {
        goto failed;
    }
#endif // SPI_OUT_HCI_ENABLED

#if SPI_OUT_MESH_ENABLED
    if (LOG_MODULE_INIT(mesh)() != 0) {
        goto failed;
    }
#endif // SPI_OUT_MESH_ENABLED

#if SPI_OUT_FLUSH_TIMER_ENABLED
    esp_timer_create_args_t timer_args = {
        .callback = (esp_timer_cb_t)esp_timer_cb_log_flush,
        .dispatch_method = ESP_TIMER_TASK
    };
    if (esp_timer_create(&timer_args, &flush_timer) != ESP_OK) {
        goto failed;
    }
#endif // SPI_OUT_FLUSH_TIMER_ENABLED

    BaseType_t ret = xTaskCreate(spi_out_task, "BLELogSPIOut",
                                 SPI_OUT_TASK_STACK_SIZE, NULL,
                                 SPI_OUT_TASK_PRIORITY, &spi_out_task_handle);
    if (ret == pdFALSE) {
        goto failed;
    }

    // Initialization done
    spi_out_inited = true;
    spi_out_enabled = true;

#if SPI_OUT_FLUSH_TIMER_ENABLED
    esp_timer_start_once(flush_timer, SPI_OUT_FLUSH_TIMEOUT_US);
#endif // SPI_OUT_FLUSH_TIMER_ENABLED
    return 0;

failed:
    ble_log_spi_out_deinit();
    return -1;
}

void ble_log_spi_out_deinit(void)
{
    spi_out_inited = false;
    spi_out_enabled = false;

    if (spi_out_task_handle) {
        vTaskDelete(spi_out_task_handle);
        spi_out_task_handle = NULL;
    }

#if SPI_OUT_FLUSH_TIMER_ENABLED
    if (flush_timer) {
        esp_timer_stop(flush_timer);
        esp_timer_delete(flush_timer);
        flush_timer = NULL;
    }
#endif // SPI_OUT_FLUSH_TIMER_ENABLED

    if (spi_handle) {
        // Drain all queued transactions
        assert(spi_device_acquire_bus(spi_handle, portMAX_DELAY) == ESP_OK);
        spi_device_release_bus(spi_handle);

        // Remove SPI master
        spi_bus_remove_device(spi_handle);
        spi_handle = NULL;
        spi_bus_free(SPI_OUT_BUS);
    }

#if SPI_OUT_TS_SYNC_ENABLED
    spi_out_ts_sync_deinit();
#endif // SPI_OUT_TS_SYNC_ENABLED

#if SPI_OUT_MESH_ENABLED
    LOG_MODULE_DEINIT(mesh)();
#endif // SPI_OUT_MESH_ENABLED

#if SPI_OUT_HCI_ENABLED
    LOG_MODULE_DEINIT(hci)();
#endif // SPI_OUT_HCI_ENABLED

#if SPI_OUT_HOST_ENABLED
    LOG_MODULE_DEINIT(host)();
#endif // SPI_OUT_HOST_ENABLED

#if SPI_OUT_LE_AUDIO_ENABLED
    LOG_MODULE_DEINIT(le_audio)();
#endif // SPI_OUT_LE_AUDIO_ENABLED

#if SPI_OUT_LL_ENABLED
    spi_out_ll_log_deinit();
#endif // SPI_OUT_LL_ENABLED

    LOG_MODULE_DEINIT(ul)();
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
    } else if (flag & BIT(LL_LOG_FLAG_HCI_UPSTREAM)) {
        log_cb = ll_hci_log_cb;
        source = BLE_LOG_SPI_OUT_SOURCE_HCI_UPSTREAM;
    } else {
        log_cb = ll_task_log_cb;
        source = BLE_LOG_SPI_OUT_SOURCE_ESP;
    }
    bool omdata = flag & BIT(LL_LOG_FLAG_OMDATA);

    bool need_append;
    if (spi_out_log_cb_check_trans(log_cb, (uint16_t)(len + len_append), &need_append)) {
        need_append |= spi_out_log_cb_write(log_cb, addr, (uint16_t)len, addr_append,
                                            (uint16_t)len_append, source, true, omdata);
    }
    if (need_append) {
        if (in_isr) {
            ll_ev_flags |= BIT(LL_EV_FLAG_ISR_APPEND);
            SPI_OUT_TASK_NOTIFY_FROM_ISR;
        } else {
            spi_out_log_cb_append_trans(log_cb);
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
    if (!LOG_MODULE_INIT_FLAG(ul)) {
        return -1;
    }

    xSemaphoreTake(LOG_MODULE_MUTEX(ul), portMAX_DELAY);
    spi_out_write_hex(LOG_MODULE_CB(ul, 0), source, addr, len, false);
    xSemaphoreGive(LOG_MODULE_MUTEX(ul));
    return 0;
}

void ble_log_spi_out_dump_all(void)
{
    if (!spi_out_inited) {
        return;
    }

    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);

    esp_rom_printf("[SPI_LOG_DUMP_START:\n");

#if SPI_OUT_LL_ENABLED
    if (ll_log_inited) {
        spi_out_log_cb_dump(ll_isr_log_cb);
        spi_out_log_cb_dump(ll_task_log_cb);
        spi_out_log_cb_dump(ll_hci_log_cb);
    }
#endif // SPI_OUT_LL_ENABLED

    LOG_MODULE_DUMP(ul);

#if SPI_OUT_LE_AUDIO_ENABLED
    LOG_MODULE_DUMP(le_audio);
#endif // SPI_OUT_LE_AUDIO_ENABLED

#if SPI_OUT_HOST_ENABLED
    LOG_MODULE_DUMP(host);
#endif // SPI_OUT_HOST_ENABLED

#if SPI_OUT_HCI_ENABLED
    LOG_MODULE_DUMP(hci);
#endif // SPI_OUT_HCI_ENABLED

#if SPI_OUT_MESH_ENABLED
    LOG_MODULE_DUMP(mesh);
#endif // SPI_OUT_MESH_ENABLED

    esp_rom_printf("\n:SPI_LOG_DUMP_END]\n\n");

    portEXIT_CRITICAL_SAFE(&spinlock);
}

void ble_log_spi_out_enable(bool enable)
{
    if (!spi_out_inited) {
        return;
    }

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

#if SPI_OUT_LE_AUDIO_ENABLED
IRAM_ATTR void ble_log_spi_out_le_audio_write(const uint8_t *addr, uint16_t len)
{
    if (!LOG_MODULE_INIT_FLAG(le_audio)) {
        return;
    }

    spi_out_log_cb_t *log_cb;
    if (!spi_out_get_task_mapping(LOG_MODULE_TASK_MAP(le_audio),
                                  LOG_MODULE_CB_CNT(le_audio), &log_cb, NULL)) {
        return;
    }

    bool need_append;
    if (spi_out_log_cb_check_trans(log_cb, len, &need_append)) {
        need_append |= spi_out_log_cb_write(log_cb, addr, len, NULL, 0,
                                            BLE_LOG_SPI_OUT_SOURCE_LE_AUDIO, false, false);
    }
    if (need_append) {
        spi_out_log_cb_append_trans(log_cb);
    }
    spi_out_log_cb_write_loss(log_cb);
    return;
}
#endif // SPI_OUT_LE_AUDIO_ENABLED

#if SPI_OUT_HOST_ENABLED
int ble_log_spi_out_host_write(uint8_t source, const char *prefix, const char *format, ...)
{
    if (!LOG_MODULE_INIT_FLAG(host) || !prefix || !format) {
        return -1;
    }

    spi_out_log_cb_t *log_cb;
    uint8_t *str_buf;
    bool fallback = false;
    if (!spi_out_get_task_mapping(LOG_MODULE_TASK_MAP(host),
                                  LOG_MODULE_CB_CNT(host), &log_cb, &str_buf)) {
        // NimBLE workaround
        fallback = true;
        xSemaphoreTake(LOG_MODULE_MUTEX(ul), portMAX_DELAY);
        log_cb = LOG_MODULE_CB(ul, 0);
        str_buf = LOG_MODULE_STR_BUF(ul, 0);
    }

    // Copy prefix to string buffer
    int prefix_len = strlen(prefix);
    if (prefix_len >= SPI_OUT_LOG_STR_BUF_SIZE) {
        return -1;
    }
    memcpy(str_buf, prefix, prefix_len);

    // Write string buffer
    va_list args;
    va_start(args, format);
    int total_len = spi_out_write_str(str_buf, format, args, prefix_len);
    va_end(args);
    if (total_len == 0) {
        return -1;
    }

    // Write log control block buffer
    spi_out_write_hex(log_cb, source, str_buf, (uint16_t)total_len, true);

    if (fallback) {
        xSemaphoreGive(LOG_MODULE_MUTEX(ul));
    }
    return 0;
}
#endif // SPI_OUT_HOST_ENABLED

#if SPI_OUT_HCI_ENABLED
int ble_log_spi_out_hci_write(uint8_t source, const uint8_t *addr, uint16_t len)
{
    if (!LOG_MODULE_INIT_FLAG(hci)) {
        return -1;
    }

#if SPI_OUT_LL_ENABLED
    if (source == BLE_LOG_SPI_OUT_SOURCE_HCI_UPSTREAM) {
        ble_log_spi_out_ll_write(len, addr, 0, NULL, BIT(LL_LOG_FLAG_HCI_UPSTREAM));
    }
    if (source == BLE_LOG_SPI_OUT_SOURCE_HCI_DOWNSTREAM)
#endif /* SPI_OUT_LL_ENABLED */
    {
        spi_out_log_cb_t *log_cb;
        bool fallback = false;
        if (!spi_out_get_task_mapping(LOG_MODULE_TASK_MAP(hci),
                                      LOG_MODULE_CB_CNT(hci), &log_cb, NULL)) {
            // NimBLE workaround
            fallback = true;
            xSemaphoreTake(LOG_MODULE_MUTEX(ul), portMAX_DELAY);
            log_cb = LOG_MODULE_CB(ul, 0);
        }
        spi_out_write_hex(log_cb, source, addr, len, true);
        if (fallback) {
            xSemaphoreGive(LOG_MODULE_MUTEX(ul));
        }
    }
    return 0;
}
#endif // SPI_OUT_HCI_ENABLED

#if SPI_OUT_MESH_ENABLED
int ble_log_spi_out_mesh_write(const char *prefix, const char *format, ...)
{
    if (!LOG_MODULE_INIT_FLAG(mesh) || !prefix || !format) {
        return -1;
    }

    spi_out_log_cb_t *log_cb;
    uint8_t *str_buf;
    if (!spi_out_get_task_mapping(LOG_MODULE_TASK_MAP(mesh),
                                  LOG_MODULE_CB_CNT(mesh), &log_cb, &str_buf)) {
        return -1;
    }

    // Copy prefix to string buffer
    int prefix_len = strlen(prefix);
    if (prefix_len >= SPI_OUT_LOG_STR_BUF_SIZE) {
        return -1;
    }
    memcpy(str_buf, prefix, prefix_len);

    // Write string buffer
    va_list args;
    va_start(args, format);
    int total_len = spi_out_write_str(str_buf, format, args, prefix_len);
    va_end(args);
    if (total_len == 0) {
        return -1;
    }

    // Write log control block buffer
    spi_out_write_hex(log_cb, BLE_LOG_SPI_OUT_SOURCE_MESH, str_buf, (uint16_t)total_len, true);
    return 0;
}
#endif // SPI_OUT_MESH_ENABLED
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
