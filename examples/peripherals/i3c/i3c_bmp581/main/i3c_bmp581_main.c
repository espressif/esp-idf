/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "driver/i3c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_err.h"
#include "bmp581_defs.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"

#define BMP581_BUF_SIZE (32)

#define I3C_SCL_FREQ_OD_HZ              400000  // Open-drain frequency (Hz)
#define I3C_SCL_FREQ_PP_HZ              1000000 // Push-pull frequency (Hz)
#define I3C_ENEC_IBI                    0x0B  // Enable IBI
#define PRESS_TEMP_TASK_PRIORITY        4       // Pressure task priority
#define IBI_TASK_PRIORITY               5       // IBI handler task priority
#define DEMO_DURATION_MS                30000   // Demo duration in milliseconds
#define QUEUE_RECEIVE_TIMEOUT_MS        500     // Queue receive timeout
#define TASK_DELAY_CHECK_MS             1000     // Task delay for stop flag check

#define BMP581_TASK_STACK_SIZE            4096
#define BMP581_IBI_QUEUE_SIZE             10
#define BMP581_COMMUNICATION_TIMEOUT_MS   1000
#define BMP581_TASK_STOP_TIMEOUT_MS       500

// Device Information
#define BMP581_STATIC_ADDR                          0x46
#define BMP581_DYNAMIC_ADDR                         0x09    // Dynamic address assigned to BMP581
#define BMP581_CHIP_ID                              0x50
#define BMP581_PID                                  0x70070000

#define I3C_DYNAMIC_ADDRESS_ASSIGN_BY_ENTDAA 1//CONFIG_I3C_DYNAMIC_ADDRESS_ASSIGN_BY_ENTDAA
// #define I3C_USE_IN_BAND_INTERRUPTS CONFIG_I3C_USE_IN_BAND_INTERRUPTS

static const char *TAG = "i3c-bmp581";

typedef struct {
    uint8_t ibi_id;
    uint8_t ibi_data_len;
    uint8_t ibi_payload;
} i3c_master_ibi_event_t;

typedef struct {
    uint8_t *tx_buf;
    uint8_t *rx_buf;
    QueueHandle_t event_queue;
    i3c_master_i3c_device_handle_t dev_handle;
    TaskHandle_t ibi_task_handle;
    TaskHandle_t press_temp_task_handle;
    TaskHandle_t main_task_handle;
    volatile bool stop_tasks;
} bmp581_context_t;

static esp_err_t bmp581_write_register(i3c_master_i3c_device_handle_t dev, uint8_t *tx_buf, uint8_t reg, uint8_t value)
{
    if (dev == NULL || tx_buf == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    tx_buf[0] = reg;
    tx_buf[1] = value;

    return i3c_master_i3c_device_transmit(dev, tx_buf, 2, BMP581_COMMUNICATION_TIMEOUT_MS);
}

static esp_err_t bmp581_read_register(i3c_master_i3c_device_handle_t dev, uint8_t *rx_buf, uint8_t reg)
{
    if (dev == NULL || rx_buf == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return i3c_master_i3c_device_transmit_receive(dev, &reg, 1, rx_buf, 1, BMP581_COMMUNICATION_TIMEOUT_MS);
}

static void get_sensor_data(bmp581_context_t *sens_ctx)
{
    uint32_t pressure_raw;
    int32_t temperature_raw;
    float pressure_pascals, temperature_celsius;

    uint8_t *rx_buf = sens_ctx->rx_buf;
    uint8_t *tx_buf = sens_ctx->tx_buf;

    tx_buf[0] = BMP581_REG_TEMP_DATA_XLSB;
    ESP_ERROR_CHECK(i3c_master_i3c_device_transmit_receive(sens_ctx->dev_handle, tx_buf, 1, rx_buf, 6, BMP581_COMMUNICATION_TIMEOUT_MS));

    temperature_raw = (int32_t)(((int32_t)rx_buf[2] << 16) | ((uint16_t)rx_buf[1] << 8) | rx_buf[0]);
    temperature_celsius = (float)(temperature_raw / 65536.0);

    pressure_raw = (uint32_t)(((uint32_t)rx_buf[5] << 16) | ((uint16_t)rx_buf[4] << 8) | rx_buf[3]);
    pressure_pascals = (float)(pressure_raw / 64.0);

    ESP_LOGI(TAG, "Temperature: %.2f°C, Pressure: %.2fPa", temperature_celsius, pressure_pascals);
}

#ifndef I3C_USE_IN_BAND_INTERRUPTS
// Pressure and temperature reading task
static void pressure_temperature_reading_task(void *param)
{
    bmp581_context_t *sens_ctx = (bmp581_context_t *)param;

    // Parameter validation
    if (sens_ctx == NULL) {
        ESP_LOGE(TAG, "Pressure task: Invalid context");
        vTaskDelete(NULL);
        return;
    }

    while (!sens_ctx->stop_tasks) {
        get_sensor_data(sens_ctx);

        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY_CHECK_MS));  // Check stop flag every 1000ms
    }

    ESP_LOGI(TAG, "Pressure reading task stopping...");
    // Notify main task that this task is about to finish
    if (sens_ctx->main_task_handle != NULL) {
        xTaskNotify(sens_ctx->main_task_handle, 0x01, eSetBits);
    }
    // Suspend this task and wait for main task to delete it
    vTaskSuspend(NULL);
}
#else
// Task to handle IBI events and read registers
static void ibi_handler_task(void *param)
{
    bmp581_context_t *sens_ctx = (bmp581_context_t *)param;
    i3c_master_ibi_event_t evt;

    // Parameter validation
    if (sens_ctx == NULL) {
        ESP_LOGE(TAG, "IBI task: Invalid context");
        vTaskDelete(NULL);
        return;
    }

    while (!sens_ctx->stop_tasks) {
        // Use timeout instead of blocking forever so we can check stop flag
        if (xQueueReceive(sens_ctx->event_queue, &evt, pdMS_TO_TICKS(QUEUE_RECEIVE_TIMEOUT_MS)) == pdTRUE) {
            // This is the IBI mandatory byte for BMP581, that is also the IBI payload that will be returned upon an GETSTATUS CCC command
            if (evt.ibi_payload & BMP581_INT_ASSERTED_DRDY) {
                ESP_LOGI(TAG, "Data ready event received!");
                // Clear status before reading
                ESP_ERROR_CHECK(bmp581_read_register(sens_ctx->dev_handle, &evt.ibi_payload, BMP581_REG_INT_STATUS));
                get_sensor_data(sens_ctx);
            }
        }
    }

    ESP_LOGI(TAG, "IBI handler task stopping...");
    // Notify main task that this task is about to finish
    if (sens_ctx->main_task_handle != NULL) {
        xTaskNotify(sens_ctx->main_task_handle, 0x02, eSetBits);
    }
    // Suspend this task and wait for main task to delete it
    vTaskSuspend(NULL);
}

IRAM_ATTR static bool ibi_callback(i3c_master_i3c_device_handle_t dev_handle, const i3c_master_ibi_info_t *evt_data, void *user_data)
{
    bmp581_context_t *sens_ctx = (bmp581_context_t *)user_data;
    BaseType_t xTaskWoken = pdFALSE;
    i3c_master_ibi_event_t evt = {
        .ibi_id = evt_data->ibi_id,
        .ibi_data_len = evt_data->data_length,
        .ibi_payload = evt_data->ibi_data[0],
    };

    xQueueSendFromISR(sens_ctx->event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}
#endif

void app_main(void)
{
    // I3C bus initialization
    bmp581_context_t sens_ctx = {0};
    i3c_master_bus_config_t i3c_bus_config = {
        .scl_io_num = CONFIG_I3C_MASTER_SCL,
        .sda_io_num = CONFIG_I3C_MASTER_SDA,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .i3c_scl_freq_hz_od = I3C_SCL_FREQ_OD_HZ,
        .i3c_scl_freq_hz_pp = I3C_SCL_FREQ_PP_HZ,
        .i3c_scl_od_duty_cycle = 0.5,
        .i3c_scl_pp_duty_cycle = 0.5,
        .i3c_sda_od_hold_time_ns = 25,
        .i3c_sda_pp_hold_time_ns = 0,
        .entdaa_device_num = 2, // Only 1 device can be discovered by ENTDAA in this example. Set 2 because the example code can have 2 i3c device on bus but only use bmp581 as one of them.
        .internal_pullup_resistor_val = I3C_MASTER_INTERNAL_PULLUP_RESISTOR_0_3K,
    };

    i3c_master_bus_handle_t i3c_handle;

    ESP_ERROR_CHECK(i3c_new_master_bus(&i3c_bus_config, &i3c_handle));

    // Initialize event queue
    sens_ctx.event_queue = xQueueCreate(BMP581_IBI_QUEUE_SIZE, sizeof(i3c_master_ibi_event_t));
    if (sens_ctx.event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create event queue");
        return;
    }

    // Reset dynamic device address
    i3c_master_ccc_transfer_config_t trans_ccc = {
        .ccc_command = I3C_CCC_RSTDAA,
        .direction = I3C_MASTER_TRANSFER_DIRECTION_WRITE,
        .device_address = 0,
        .data = 0,
        .data_size = 0,
    };
    ESP_ERROR_CHECK(i3c_master_transfer_ccc(i3c_handle, &trans_ccc));

    uint8_t dynamic_addr = 0;

#if !I3C_DYNAMIC_ADDRESS_ASSIGN_BY_ENTDAA
    // Add device to I3C bus by SETDASA
    // You can also addsign a dynamic address manually by yourself.
    ESP_ERROR_CHECK(i3c_master_get_valid_address_slot(i3c_handle, BMP581_DYNAMIC_ADDR, &dynamic_addr));
    i3c_master_i3c_device_handle_t dev;
    i3c_device_i3c_config_t dev_cfg = {
        .dynamic_addr = dynamic_addr,
        .static_addr = BMP581_STATIC_ADDR,
    };
    ESP_ERROR_CHECK(i3c_master_bus_add_i3c_static_device(i3c_handle, &dev_cfg, &dev));
#else
    // Add device to I3C bus by ENTDAA
    i3c_master_i3c_device_table_handle_t dev_table;
    ESP_ERROR_CHECK(i3c_master_scan_devices_by_entdaa(i3c_handle, &dev_table));
    if (dev_table == NULL) {
        ESP_LOGE(TAG, "Failed to scan devices by ENTDAA");
        return;
    }
    size_t device_count = 0;
    ESP_ERROR_CHECK(i3c_master_get_device_count(dev_table, &device_count));

    i3c_master_i3c_device_handle_t dev = NULL;
    for (size_t i = 0; i < device_count; i++) {
        i3c_master_i3c_device_handle_t dev_handle;
        ESP_ERROR_CHECK(i3c_master_get_device_handle(dev_table, i, &dev_handle));
        i3c_device_information_t info;
        i3c_master_i3c_device_get_info(dev_handle, &info);
        if (info.pid == BMP581_PID) {
            dev = dev_handle;
            dynamic_addr = info.dynamic_addr;
            break;
        }
    }
    // Free the device handle table, which is no longer needed after searching for the device.
    ESP_ERROR_CHECK(i3c_master_free_device_handle_table(dev_table));

    if (dev == NULL) {
        ESP_LOGE(TAG, "Failed to find device");
        return;
    }
#endif
    
    // Allocate buffers for application context
    sens_ctx.tx_buf = heap_caps_calloc(1, BMP581_BUF_SIZE, MALLOC_CAP_DEFAULT);
    if (sens_ctx.tx_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate tx buffer");
        return;
    }

    sens_ctx.rx_buf = heap_caps_calloc(1, BMP581_BUF_SIZE, MALLOC_CAP_DEFAULT);
    if (sens_ctx.rx_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate rx buffer");
        return;
    }

    sens_ctx.dev_handle = dev;

    uint8_t *i3c_mst_tx_data = sens_ctx.tx_buf;

#ifdef I3C_USE_IN_BAND_INTERRUPTS
    // Configure IBI
    i3c_ibi_config_t ibi_config = {
        .enable_ibi = 1,
        .enable_ibi_payload = 1,
    };
    ESP_ERROR_CHECK(i3c_master_i3c_device_ibi_config(dev, &ibi_config));
#endif

    // Check chip routine
    uint8_t chip_id = 0x00;
    ESP_ERROR_CHECK(bmp581_read_register(dev, &chip_id, BMP581_REG_CHIP_ID));
    if (chip_id != BMP581_CHIP_ID) {
        ESP_LOGE(TAG, "Wrong chip ID");
        return;
    }
    uint8_t nvm_status = 0x00;
    ESP_ERROR_CHECK(bmp581_read_register(dev, &nvm_status, BMP581_REG_STATUS));
    if (!((nvm_status & BMP581_INT_NVM_RDY) && (!(nvm_status & BMP581_INT_NVM_ERR)))) {
        ESP_LOGE(TAG, "Invalid NVM status");
        return;
    }
    uint8_t int_status = 0x00;
    ESP_ERROR_CHECK(bmp581_read_register(dev, &int_status, BMP581_REG_INT_STATUS));
    if (!(int_status & BMP581_INT_ASSERTED_POR_SOFTRESET_COMPLETE)) {
        ESP_LOGE(TAG, "Invalid interrupt status");
        return;
    }

    // Sampling configuration
    uint8_t osr_data = 0x00;
    ESP_ERROR_CHECK(bmp581_read_register(dev, &osr_data, BMP581_REG_OSR_CONFIG));
    osr_data = BMP581_SET_BITS_POS_0(osr_data, BMP581_TEMP_OS, BMP581_DISABLE);
    osr_data = BMP581_SET_BITSLICE(osr_data, BMP581_PRESS_OS, BMP581_DISABLE);
    osr_data = BMP581_SET_BITSLICE(osr_data, BMP581_PRESS_EN, BMP581_ENABLE);           // Enable pressure measurements
    ESP_ERROR_CHECK(bmp581_write_register(dev, i3c_mst_tx_data, BMP581_REG_OSR_CONFIG, osr_data));
    uint8_t odr_data = 0x00;
    ESP_ERROR_CHECK(bmp581_read_register(dev, &odr_data, BMP581_REG_ODR_CONFIG));
    odr_data = BMP581_SET_BITSLICE(odr_data, BMP581_ODR, BMP581_ODR_01_HZ);
    odr_data = BMP581_SET_BITSLICE(odr_data, BMP581_DEEP_DISABLE, BMP581_DEEP_DISABLED);
    odr_data = BMP581_SET_BITS_POS_0(odr_data, BMP581_POWERMODE, BMP581_POWERMODE_STANDBY);
    ESP_ERROR_CHECK(bmp581_write_register(dev, i3c_mst_tx_data, BMP581_REG_ODR_CONFIG, odr_data));
    vTaskDelay(pdMS_TO_TICKS(2));
    odr_data = BMP581_SET_BITS_POS_0(odr_data, BMP581_POWERMODE, BMP581_POWERMODE_NORMAL);
    ESP_ERROR_CHECK(bmp581_write_register(dev, i3c_mst_tx_data, BMP581_REG_ODR_CONFIG, odr_data));

#ifdef I3C_USE_IN_BAND_INTERRUPTS
    uint8_t int_config = 0x00, int_source = 0x00;
    // Disable sources and clear status before applying the configuration
    ESP_ERROR_CHECK(bmp581_write_register(dev, i3c_mst_tx_data, BMP581_REG_INT_SOURCE, int_source));
    ESP_ERROR_CHECK(bmp581_read_register(dev, &int_status, BMP581_REG_INT_STATUS));
    ESP_ERROR_CHECK(bmp581_read_register(dev, &int_config, BMP581_REG_INT_CONFIG));
    int_config = BMP581_SET_BITS_POS_0(int_config, BMP581_INT_MODE, BMP581_INT_MODE_PULSED);
    int_config = BMP581_SET_BITSLICE(int_config, BMP581_INT_POL, BMP581_INT_POL_ACTIVE_HIGH);
    int_config = BMP581_SET_BITSLICE(int_config, BMP581_INT_OD, BMP581_INT_OD_PUSHPULL);
    int_config = BMP581_SET_BITSLICE(int_config, BMP581_INT_EN, BMP581_ENABLE);
    ESP_ERROR_CHECK(bmp581_write_register(dev, i3c_mst_tx_data, BMP581_REG_INT_CONFIG, int_config));
    // Enable in-band interrupt notifications when new measurement data is ready
    int_source = BMP581_SET_BITS_POS_0(int_source, BMP581_INT_DRDY_EN, BMP581_ENABLE);
    int_source = BMP581_SET_BITSLICE(int_source, BMP581_INT_FIFO_FULL_EN, BMP581_DISABLE);
    int_source = BMP581_SET_BITSLICE(int_source, BMP581_INT_FIFO_THRES_EN, BMP581_DISABLE);
    int_source = BMP581_SET_BITSLICE(int_source, BMP581_INT_OOR_PRESS_EN, BMP581_DISABLE);
    ESP_ERROR_CHECK(bmp581_write_register(dev, i3c_mst_tx_data, BMP581_REG_INT_SOURCE, int_source));

    uint8_t enec_data = I3C_ENEC_IBI;
    i3c_master_ccc_transfer_config_t trans_ccc3 = {
        .ccc_command = I3C_CCC_ENEC_DIRECT,
        .direction = I3C_MASTER_TRANSFER_DIRECTION_WRITE,
        .device_address = dynamic_addr,
        .data = &enec_data,
        .data_size = 1,
    };
    ESP_ERROR_CHECK(i3c_master_transfer_ccc(i3c_handle, &trans_ccc3));

    i3c_master_i3c_event_callbacks_t cbs = {
        .on_ibi = ibi_callback,
    };
    ESP_ERROR_CHECK(i3c_master_i3c_device_register_event_callbacks(dev, &cbs, &sens_ctx));
#endif

    // Store main task handle for task notification
    sens_ctx.main_task_handle = xTaskGetCurrentTaskHandle();

    ESP_LOGI(TAG, "Pressure and temperature reading task started");

#ifndef I3C_USE_IN_BAND_INTERRUPTS
    ESP_LOGI(TAG, "Measurements will be polled/read every second");
    // Create pressure reading task after sensor configuration is complete
    xTaskCreate(pressure_temperature_reading_task, "press_temp_reader", BMP581_TASK_STACK_SIZE, &sens_ctx, PRESS_TEMP_TASK_PRIORITY, &sens_ctx.press_temp_task_handle);
#else
    ESP_LOGI(TAG, "Waiting for IBI events...");
    // Create IBI handler task with app context
    xTaskCreate(ibi_handler_task, "ibi_handler", BMP581_TASK_STACK_SIZE, &sens_ctx, IBI_TASK_PRIORITY, &sens_ctx.ibi_task_handle);
#endif

    // Wait for some time to allow multiple IBI events
    vTaskDelay(pdMS_TO_TICKS(DEMO_DURATION_MS));  // Wait for demo

    // Signal tasks to stop
    ESP_LOGI(TAG, "Stopping tasks...");
    // Clear any pending notifications before waiting
    xTaskNotifyWait(ULONG_MAX, 0x00, NULL, 0);
    sens_ctx.stop_tasks = true;

    // Wait for tasks to finish gracefully using task notification
    uint32_t notification_value = 0;

#ifndef I3C_USE_IN_BAND_INTERRUPTS
    const uint32_t press_temp_task_bit = 0x01;  // Bit 0 for pressure task

    // Wait for pressure task to finish and delete it
    if (sens_ctx.press_temp_task_handle != NULL) {
        // Wait for press_temp_task_bit to be set
        xTaskNotifyWait(0x00, press_temp_task_bit, &notification_value, pdMS_TO_TICKS(BMP581_TASK_STOP_TIMEOUT_MS));
        // Delete the task after receiving notification
        vTaskDelete(sens_ctx.press_temp_task_handle);
        sens_ctx.press_temp_task_handle = NULL;
    }
#else
    const uint32_t ibi_task_bit = 0x02;   // Bit 1 for IBI task

    // Wait for IBI task to finish and delete it
    if (sens_ctx.ibi_task_handle != NULL) {
        // Wait for ibi_task_bit to be set
        xTaskNotifyWait(0x00, ibi_task_bit, &notification_value, pdMS_TO_TICKS(BMP581_TASK_STOP_TIMEOUT_MS));
        // Delete the task after receiving notification
        vTaskDelete(sens_ctx.ibi_task_handle);
        sens_ctx.ibi_task_handle = NULL;
    }
#endif

    ESP_LOGI(TAG, "bmp581 demo completed");
    vQueueDelete(sens_ctx.event_queue);
    free(sens_ctx.tx_buf);
    free(sens_ctx.rx_buf);

    ESP_ERROR_CHECK(i3c_master_bus_rm_i3c_device(dev));
    ESP_ERROR_CHECK(i3c_del_master_bus(i3c_handle));

    return;
}
