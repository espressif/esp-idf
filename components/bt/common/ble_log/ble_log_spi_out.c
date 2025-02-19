/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ble_log/ble_log_spi_out.h"

// Private defines
#define SPI_OUT_BUS SPI2_HOST

// Private typedefs
typedef struct spi_out_trans
{
    spi_transaction_t trans;
    struct spi_out_trans *next;
} spi_out_trans_t;

// Private variables
static spi_device_handle_t spi_handle = NULL;
static spi_out_trans_t *trans_head = NULL;
static SemaphoreHandle_t mutex_handle = NULL;
static bool spi_out_inited = false;

// Private function declarations
static void spi_out_init_trans(void);
static void spi_out_deinit_trans(void);
static void spi_out_recycle_trans(uint32_t ms_to_wait);
static void spi_out_append_trans(void);

// Private functions
static void spi_out_init_trans(void)
{
    for (int i = 0; i < CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_QUEUE_SIZE; i++)
    {
        // Allocate memory for SPI transaction
        uint8_t *buf = (uint8_t *)heap_caps_malloc(CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_TRANS_BUF_SIZE, MALLOC_CAP_DMA);
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
    while (trans_head != NULL)
    {
        next = trans_head->next;
        free((uint8_t *)trans_head->trans.tx_buffer);
        free(trans_head);
        trans_head = next;
    }
    trans_head = NULL;
    return;
}

IRAM_ATTR static void spi_out_recycle_trans(uint32_t ms_to_wait)
{
    // Try to recycle transaction
    spi_transaction_t *ret_trans;
    spi_out_trans_t *recycled_trans;
    while (ESP_OK == spi_device_get_trans_result(spi_handle, &ret_trans, pdMS_TO_TICKS(ms_to_wait)))
    {
        recycled_trans = __containerof(ret_trans, spi_out_trans_t, trans);
        recycled_trans->next = trans_head;
        trans_head = recycled_trans;
        trans_head->trans.length = 0;
    }
}

IRAM_ATTR static void spi_out_append_trans(void)
{
    // Transaction head shall not be NULL for appending
    assert(trans_head);

    // Detach transaction head
    spi_out_trans_t *trans_to_append = trans_head;
    trans_head = trans_head->next;
    trans_to_append->next = NULL;

    // CRITICAL: Length unit conversion from bytes to bits
    trans_to_append->trans.length *= 8;
    assert(ESP_OK == spi_device_queue_trans(spi_handle, &trans_to_append->trans, 0));

    // Try to recycle trans
    spi_out_recycle_trans(0);
}

// Public functions
void ble_log_spi_out_init(void)
{
    // Avoid double init
    if (spi_out_inited)
    {
        return;
    }

    // Initialize SPI
    spi_bus_config_t bus_config = {
        .miso_io_num = -1,
        .mosi_io_num = CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_MOSI_IO_NUM,
        .sclk_io_num = CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_SCLK_IO_NUM,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 10240
    };
    spi_device_interface_config_t dev_config = {
        .clock_speed_hz = SPI_MASTER_FREQ_20M,
        .mode = 0,
        .spics_io_num = CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_CS_IO_NUM,
        .queue_size = CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_QUEUE_SIZE
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI_OUT_BUS, &bus_config, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI_OUT_BUS, &dev_config, &spi_handle));

    // Initialize transaction link nodes
    spi_out_init_trans();

    // Initialize mutex
    mutex_handle = xSemaphoreCreateMutex();

    // Set init flag
    spi_out_inited = true;
}

void ble_log_spi_out_deinit(void)
{
    // Avoid double deinit
    if (!spi_out_inited)
    {
        return;
    }

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

IRAM_ATTR void ble_log_spi_out_write(uint32_t len, const uint8_t *addr, spi_out_source_t source)
{
    // Take semaphore
    assert(xSemaphoreTake(mutex_handle, portMAX_DELAY) == pdTRUE);

    // Recycle trans if free buffer list is empty
    if (!trans_head)
    {
        spi_out_recycle_trans(0);
    }

    // Length of 0 means flush out
    if (!len)
    {
        assert(trans_head);
        if (trans_head->trans.length)
        {
            spi_out_append_trans();
        }
        goto release;
    }

    // Copy user data to buffer
    uint32_t copy_buf_len;
    uint32_t data_left_len = len;
    uint32_t empty_buf_len = CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_TRANS_BUF_SIZE - trans_head->trans.length;
    while (data_left_len)
    {
        // There shall always be available buffer in free buffer list during write operation
        assert(trans_head);

        // Copy data to buffer and update length
        copy_buf_len = (data_left_len > empty_buf_len) ? empty_buf_len : data_left_len;
        memcpy((uint8_t *)trans_head->trans.tx_buffer + trans_head->trans.length, addr + (len - data_left_len), copy_buf_len);
        trans_head->trans.length += copy_buf_len;
        data_left_len -= copy_buf_len;

        // Transaction buffer length shall never exceed buffer size
        assert(trans_head->trans.length <= CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_TRANS_BUF_SIZE);

        // If buffer is full, append transaction and reset buffer length
        if (trans_head->trans.length == CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_TRANS_BUF_SIZE)
        {
            spi_out_append_trans();
            empty_buf_len = CONFIG_BT_LE_CONTROLLER_LOG_SPI_OUT_TRANS_BUF_SIZE;
        }
    }

release:
    xSemaphoreGive(mutex_handle);
    return;
}

IRAM_ATTR void ble_log_spi_out_write_esp(uint32_t len, const uint8_t *addr, bool end)
{
    ble_log_spi_out_write(len, addr, esp_controller);
}
