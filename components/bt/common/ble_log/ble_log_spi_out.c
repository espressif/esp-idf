/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ble_log/ble_log_spi_out.h"

#if CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED

// Private defines
#define SPI_OUT_BUS SPI2_HOST
#define SPI_OUT_TAIL 0xAA
#define SPI_OUT_FLUSHOUT_TIMEOUT (1000 * 1000)
#define SPI_OUT_TS_SYNC_TIMEOUT (1000 * 1000)

// Private typedefs
typedef struct spi_out_trans {
    spi_transaction_t trans;
    struct spi_out_trans *next;
} spi_out_trans_t;

// Private variables
static spi_device_handle_t spi_handle = NULL;
static spi_out_trans_t *trans_head = NULL;
static SemaphoreHandle_t mutex_handle = NULL;
static bool spi_out_inited = false;
static esp_timer_handle_t flushout_timer_handle = NULL;
static uint32_t loss_frame_cnt = 0;

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
static bool sync_io_level = false;
static esp_timer_handle_t ts_sync_timer_handle = NULL;
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

// Private function declarations
static void spi_out_init_trans(void);
static void spi_out_deinit_trans(void);
static void spi_out_recycle_trans(uint32_t ms_to_wait);
static void spi_out_append_trans(void);
static int spi_out_write(const uint8_t *addr, uint16_t len);
static void esp_timer_cb_flushout(void);
static void esp_timer_cb_ts_sync(void);

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
#if defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6)
extern uint32_t r_ble_lll_timer_current_tick_get(void);
#endif // CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32C6
#if defined(CONFIG_IDF_TARGET_ESP32C2)
extern uint32_t r_os_cputime_get32(void);
#endif // CONFIG_IDF_TARGET_ESP32C2
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

// Private functions
static void spi_out_init_trans(void)
{
    for (int i = 0; i < CONFIG_BT_BLE_LOG_SPI_OUT_QUEUE_SIZE; i++) {
        // Allocate memory for SPI transaction
        uint8_t *buf = (uint8_t *)spi_bus_dma_memory_alloc(SPI_OUT_BUS, CONFIG_BT_BLE_LOG_SPI_OUT_TRANS_BUF_SIZE, 0);
        assert(buf);

        // Initialize new trans
        spi_out_trans_t *new_trans = (spi_out_trans_t *)malloc(sizeof(spi_out_trans_t));
        assert(new_trans);
        memset(new_trans, 0, sizeof(spi_out_trans_t));
        new_trans->trans.tx_buffer = buf;
        new_trans->trans.length = 0;

        // Append new trans to free trans list
        new_trans->next = trans_head;
        trans_head = new_trans;
    }
    return;
}

static void spi_out_deinit_trans(void)
{
    // Wait up to QUEUE_SIZE * 100 ms for all transactions to complete and be recycled
    spi_out_recycle_trans(100);

    // Release memory
    spi_out_trans_t *next;
    while (trans_head != NULL) {
        next = trans_head->next;
        free((uint8_t *)trans_head->trans.tx_buffer);
        free(trans_head);
        trans_head = next;
    }
    trans_head = NULL;
    return;
}

// CRITICAL: Do not recycle trans when trans_head is not empty!
IRAM_ATTR static void spi_out_recycle_trans(uint32_t ms_to_wait)
{
    // Try to recycle transaction
    spi_transaction_t *ret_trans;
    spi_out_trans_t *recycled_trans;
    while (ESP_OK == spi_device_get_trans_result(spi_handle, &ret_trans, pdMS_TO_TICKS(ms_to_wait))) {
        recycled_trans = __containerof(ret_trans, spi_out_trans_t, trans);
        recycled_trans->next = trans_head;
        trans_head = recycled_trans;
        trans_head->trans.length = 0;
        trans_head->trans.rxlength = 0;
    }
}

IRAM_ATTR static void spi_out_append_trans(void)
{
    // Stop flushout timer
    esp_timer_stop(flushout_timer_handle);

    // Transaction head shall not be NULL for appending
    if (trans_head) {
        // Detach transaction head
        spi_out_trans_t *trans_to_append = trans_head;
        trans_head = trans_head->next;
        trans_to_append->next = NULL;

        // CRITICAL: Length unit conversion from bytes to bits
        trans_to_append->trans.length *= 8;
        ESP_ERROR_CHECK(spi_device_queue_trans(spi_handle, &trans_to_append->trans, 0));
    }

    // Try to recycle trans
    spi_out_recycle_trans(0);

    // Restart flushout timer
    esp_timer_start_once(flushout_timer_handle, SPI_OUT_FLUSHOUT_TIMEOUT);
}

IRAM_ATTR static int spi_out_write(const uint8_t *addr, uint16_t len)
{
    // Recycle trans if free buffer list is empty
    if (!trans_head) {
        spi_out_recycle_trans(0);
    }

    // Copy user data to buffer
    uint16_t copy_buf_len;
    uint16_t data_left_len = len;
    uint16_t empty_buf_len = CONFIG_BT_BLE_LOG_SPI_OUT_TRANS_BUF_SIZE - trans_head->trans.length;
    while (data_left_len) {
        // There shall always be available buffer in free buffer list during write operation
        if (!trans_head) {
            return -1;
        }

        // Copy data to buffer and update length
        copy_buf_len = (data_left_len > empty_buf_len) ? empty_buf_len : data_left_len;
        memcpy((uint8_t *)trans_head->trans.tx_buffer + trans_head->trans.length, addr + (len - data_left_len), copy_buf_len);
        trans_head->trans.length += copy_buf_len;
        data_left_len -= copy_buf_len;

        // If buffer is full, append transaction and reset buffer length
        if (trans_head->trans.length == CONFIG_BT_BLE_LOG_SPI_OUT_TRANS_BUF_SIZE) {
            spi_out_append_trans();
            empty_buf_len = CONFIG_BT_BLE_LOG_SPI_OUT_TRANS_BUF_SIZE;
        }
    }
    return 0;
}

// CRITICAL: This function is called in ESP Timer task
IRAM_ATTR static void esp_timer_cb_flushout(void)
{
    // Take semaphore
    assert(xSemaphoreTakeRecursive(mutex_handle, portMAX_DELAY) == pdTRUE);

    // Flushout
    if (trans_head) {
        // Make sure there's enough space for loss frame counter
        if (trans_head->next && loss_frame_cnt) {
            ble_log_spi_out_write(BLE_LOG_SPI_OUT_SOURCE_LOSS, (uint8_t *)&loss_frame_cnt, sizeof(loss_frame_cnt));
            loss_frame_cnt = 0;
        }
        if (trans_head->trans.length) {
            spi_out_append_trans();
        }
    }
    else {
        // Restart flushout timer
        esp_timer_start_once(flushout_timer_handle, SPI_OUT_FLUSHOUT_TIMEOUT);
    }

    // Release semaphore
    xSemaphoreGiveRecursive(mutex_handle);
}

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
// CRITICAL: This function is called in ESP Timer task
IRAM_ATTR static void esp_timer_cb_ts_sync(void)
{
    // Initialize variables
    uint32_t lc_ts = 0;
    uint32_t esp_ts = 0;

    // Enter critical
    portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_SAFE(&spinlock);

    // Get LC timestamp
#if defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6)
    lc_ts = r_ble_lll_timer_current_tick_get();
#endif // CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32C6
#if defined(CONFIG_IDF_TARGET_ESP32C2)
    lc_ts = r_os_cputime_get32();
#endif // CONFIG_IDF_TARGET_ESP32C2

    // Toggle Sync IO
    gpio_set_level(CONFIG_BT_BLE_LOG_SPI_OUT_SYNC_IO_NUM, (uint32_t)sync_io_level);

    // Get ESP timestamp
    esp_ts = esp_timer_get_time();
    portEXIT_CRITICAL_SAFE(&spinlock);
    // Exit critical

    // Write timestamp sync log
    uint8_t sync_frame[9];
    sync_frame[0] = ((uint8_t)sync_io_level & 0xFF);
    memcpy(sync_frame + 1, &lc_ts, sizeof(lc_ts));
    memcpy(sync_frame + 5, &esp_ts, sizeof(esp_ts));
    ble_log_spi_out_write(BLE_LOG_SPI_OUT_SOURCE_SYNC, sync_frame, 9);

    // Update IO level
    sync_io_level = !sync_io_level;
}
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

// Public functions
void ble_log_spi_out_init(void)
{
    // Avoid double init
    if (spi_out_inited) {
        return;
    }

    // Initialize mutex
    mutex_handle = xSemaphoreCreateRecursiveMutex();

    // Initialize SPI
    spi_bus_config_t bus_config = {
        .miso_io_num = -1,
        .mosi_io_num = CONFIG_BT_BLE_LOG_SPI_OUT_MOSI_IO_NUM,
        .sclk_io_num = CONFIG_BT_BLE_LOG_SPI_OUT_SCLK_IO_NUM,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 10240
    };
    spi_device_interface_config_t dev_config = {
        .clock_speed_hz = SPI_MASTER_FREQ_20M,
        .mode = 0,
        .spics_io_num = CONFIG_BT_BLE_LOG_SPI_OUT_CS_IO_NUM,
        .queue_size = CONFIG_BT_BLE_LOG_SPI_OUT_QUEUE_SIZE
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI_OUT_BUS, &bus_config, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI_OUT_BUS, &dev_config, &spi_handle));

    // Initialize transaction link nodes
    spi_out_init_trans();

    // Initialize flushout timer
    esp_timer_create_args_t timer_args = {
        .callback = (esp_timer_cb_t)esp_timer_cb_flushout,
        .dispatch_method = ESP_TIMER_TASK
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &flushout_timer_handle));
    esp_timer_start_once(flushout_timer_handle, SPI_OUT_FLUSHOUT_TIMEOUT);
    loss_frame_cnt = 0;

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
    // Initialize timestamp synchronizer
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1UL << CONFIG_BT_BLE_LOG_SPI_OUT_SYNC_IO_NUM),
        .pull_down_en = 0,
        .pull_up_en = 0
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    sync_io_level = false;
    gpio_set_level(CONFIG_BT_BLE_LOG_SPI_OUT_SYNC_IO_NUM, sync_io_level);
    esp_timer_create_args_t ts_sync_timer_args = {
        .callback = (esp_timer_cb_t)esp_timer_cb_ts_sync,
        .dispatch_method = ESP_TIMER_TASK
    };
    ESP_ERROR_CHECK(esp_timer_create(&ts_sync_timer_args, &ts_sync_timer_handle));
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

    // Set init flag
    spi_out_inited = true;
}

void ble_log_spi_out_deinit(void)
{
    // Avoid double deinit
    if (!spi_out_inited) {
        return;
    }

#if CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED
    // Deinitialize timestamp synchronizer
    esp_timer_stop(ts_sync_timer_handle);
    esp_timer_delete(ts_sync_timer_handle);
    gpio_reset_pin(CONFIG_BT_BLE_LOG_SPI_OUT_SYNC_IO_NUM);
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

    // Deinitialize flushout timer
    esp_timer_stop(flushout_timer_handle);
    esp_timer_delete(flushout_timer_handle);

    // Deinitialize transaction link nodes
    spi_out_deinit_trans();

    // Deinitialize SPI
    ESP_ERROR_CHECK(spi_bus_remove_device(spi_handle));
    ESP_ERROR_CHECK(spi_bus_free(SPI_OUT_BUS));
    spi_handle = NULL;

    // Deinitialize mutex
    vSemaphoreDelete(mutex_handle);
    mutex_handle = NULL;

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
    if (ts_sync_timer_handle) {
        if (!esp_timer_is_active(ts_sync_timer_handle)) {
            esp_timer_start_periodic(ts_sync_timer_handle, SPI_OUT_TS_SYNC_TIMEOUT);
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
    if (ts_sync_timer_handle) {
        if (esp_timer_is_active(ts_sync_timer_handle)) {
            esp_timer_stop(ts_sync_timer_handle);
        }
    }
}
#endif // CONFIG_BT_BLE_LOG_SPI_OUT_TS_SYNC_ENABLED

IRAM_ATTR void ble_log_spi_out_write_esp(uint32_t len, const uint8_t *addr, bool end)
{
    return ble_log_spi_out_write(BLE_LOG_SPI_OUT_SOURCE_ESP, addr, len);
}

IRAM_ATTR void ble_log_spi_out_write(uint8_t source, const uint8_t *addr, uint16_t len)
{
    // Initialize frame sequence number
    static uint8_t frame_sn = 0;

    // Take semaphore
    assert(xSemaphoreTakeRecursive(mutex_handle, portMAX_DELAY) == pdTRUE);

    // Prepare frame head and frame tail
    const uint8_t head[4] = {len & 0xFF, (len >> 8) & 0xFF, (uint8_t)source, frame_sn};
    const uint8_t tail = SPI_OUT_TAIL;

    // Write frame head first, then payload, finally frame tail
    do
    {
        if (spi_out_write(head, 4) != 0) {
            loss_frame_cnt++;
            break;
        }
        if (spi_out_write(addr, len) != 0) {
            loss_frame_cnt++;
            break;
        }
        if (spi_out_write(&tail, 1) != 0) {
            loss_frame_cnt++;
            break;
        }
    } while (0);

    // Update frame sequence number
    frame_sn++;

    // Release semaphore
    xSemaphoreGiveRecursive(mutex_handle);
    return;
}

#endif // CONFIG_BT_BLE_LOG_SPI_OUT_ENABLED
