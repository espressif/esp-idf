/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "driver/i3c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_err.h"
#include "lsm6dox_defs.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"

#define LSM6DSOX_BUF_SIZE (32)

#define I3C_SCL_FREQ_OD_HZ              400000  // Open-drain frequency (Hz)
#define I3C_SCL_FREQ_PP_HZ              1000000 // Push-pull frequency (Hz)
#define TEMP_TASK_PRIORITY              4       // Temperature task priority
#define IBI_TASK_PRIORITY               5       // IBI handler task priority
#define DEMO_DURATION_MS                30000   // Demo duration in milliseconds
#define QUEUE_RECEIVE_TIMEOUT_MS        500     // Queue receive timeout
#define TASK_DELAY_CHECK_MS             1000     // Task delay for stop flag check
#define TEMP_CHECK_CYCLES               10      // Number of 100ms cycles for 1 second
#define LSM6DSOX_DEFAULT_TEMP_INTERVAL_MS   1000
#define LSM6DSOX_DEFAULT_TASK_STACK_SIZE    4096
#define LSM6DSOX_IBI_QUEUE_SIZE             10
#define LSM6DSOX_COMMUNICATION_TIMEOUT_MS   1000
#define LSM6DSOX_TASK_STOP_TIMEOUT_MS       500

#define I3C_DYNAMIC_ADDRESS_ASSIGN_BY_ENTDAA CONFIG_I3C_DYNAMIC_ADDRESS_ASSIGN_BY_ENTDAA

static const char *TAG = "i3c-lsm6dsox";

typedef struct {
    uint8_t ibi_id;
    uint8_t ibi_data_len;
} i3c_master_ibi_event_t;

typedef struct {
    uint8_t *tx_buf;
    uint8_t *rx_buf;
    QueueHandle_t event_queue;
    i3c_master_i3c_device_handle_t dev_handle;
    TaskHandle_t ibi_task_handle;
    TaskHandle_t temp_task_handle;
    TaskHandle_t main_task_handle;
    volatile bool stop_tasks;
} lsm6dsox_context_t;

static esp_err_t lsm6dsox_write_register(i3c_master_i3c_device_handle_t dev, uint8_t *tx_buf, uint8_t reg, uint8_t value)
{
    if (dev == NULL || tx_buf == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    tx_buf[0] = reg;
    tx_buf[1] = value;

    return i3c_master_i3c_device_transmit(dev, tx_buf, 2, LSM6DSOX_COMMUNICATION_TIMEOUT_MS);
}

// Temperature reading task
static void temperature_reading_task(void *param)
{
    lsm6dsox_context_t *sens_ctx = (lsm6dsox_context_t *)param;
    uint8_t temp_data[2];
    uint8_t temp_tx_buf[8];
    int16_t temp_raw;
    float temperature_celsius;

    // Parameter validation
    if (sens_ctx == NULL) {
        ESP_LOGE(TAG, "Temperature task: Invalid context");
        vTaskDelete(NULL);
        return;
    }

    while (!sens_ctx->stop_tasks) {

        // Read temperature low byte (LSM6DSOX_OUT_TEMP_L)
        temp_tx_buf[0] = LSM6DSOX_OUT_TEMP_L;
        ESP_ERROR_CHECK(i3c_master_i3c_device_transmit_receive(sens_ctx->dev_handle, temp_tx_buf, 1, temp_data, 2, LSM6DSOX_COMMUNICATION_TIMEOUT_MS));

        // Combine low and high bytes to form 16-bit temperature value
        temp_raw = (int16_t)((temp_data[1] << 8) | temp_data[0]);

        // Convert to Celsius according to LSM6DSOX datasheet
        // Temperature (°C) = (temp_raw / SCALE) + OFFSET
        // The raw value is a 16-bit signed integer
        temperature_celsius = ((float)temp_raw / LSM6DSOX_TEMP_CONVERSION_FACTOR) + LSM6DSOX_TEMP_OFFSET_CELSIUS;

        ESP_LOGI(TAG, "Temperature: %.2f°C", temperature_celsius);

        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY_CHECK_MS));  // Check stop flag every 1000ms
    }

    ESP_LOGI(TAG, "Temperature reading task stopping...");
    // Notify main task that this task is about to finish
    if (sens_ctx->main_task_handle != NULL) {
        xTaskNotify(sens_ctx->main_task_handle, 0x01, eSetBits);
    }
    // Suspend this task and wait for main task to delete it
    vTaskSuspend(NULL);
}

// Task to handle IBI events and read registers
static void ibi_handler_task(void *param)
{
    lsm6dsox_context_t *sens_ctx = (lsm6dsox_context_t *)param;
    i3c_master_ibi_event_t evt;
    uint8_t ibi_tx_buf[8];
    uint8_t ibi_rx_buf[8];

    // Parameter validation
    if (sens_ctx == NULL) {
        ESP_LOGE(TAG, "IBI task: Invalid context");
        vTaskDelete(NULL);
        return;
    }

    while (!sens_ctx->stop_tasks) {
        // Use timeout instead of blocking forever so we can check stop flag
        if (xQueueReceive(sens_ctx->event_queue, &evt, pdMS_TO_TICKS(QUEUE_RECEIVE_TIMEOUT_MS)) == pdTRUE) {
            // Read TAP_SRC register to check tap detection source
            ibi_tx_buf[0] = LSM6DSOX_TAP_SRC;
            esp_err_t ret = i3c_master_i3c_device_transmit_receive(sens_ctx->dev_handle, ibi_tx_buf, 1, ibi_rx_buf, 1, LSM6DSOX_COMMUNICATION_TIMEOUT_MS);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "Failed to read TAP_SRC register");
                continue;
            }

            // Parse TAP_SRC register bits and display event information
            uint8_t tap_src = ibi_rx_buf[0];
            if (tap_src & LSM6DSOX_TAP_SRC_SINGLE_TAP) {  // Single tap detected
                const char* axis = "";
                if (tap_src & LSM6DSOX_TAP_SRC_X_TAP) {
                    axis = "X-axis";    // X-axis tap
                } else if (tap_src & LSM6DSOX_TAP_SRC_Y_TAP) {
                    axis = "Y-axis";    // Y-axis tap
                } else if (tap_src & LSM6DSOX_TAP_SRC_Z_TAP) {
                    axis = "Z-axis";    // Z-axis tap
                }

                const char* direction = (tap_src & LSM6DSOX_TAP_SRC_TAP_SIGN) ? "negative direction" : "positive direction"; // Tap sign
                ESP_LOGI(TAG, "Single tap detected: %s %s", axis, direction);
            }
            if (tap_src & LSM6DSOX_TAP_SRC_DOUBLE_TAP) {  // Double tap detected
                ESP_LOGI(TAG, "Double tap detected");
            }
            if (tap_src & LSM6DSOX_TAP_SRC_TAP_IA) {  // Tap interrupt active
                ESP_LOGI(TAG, "Tap interrupt active");
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
    lsm6dsox_context_t *sens_ctx = (lsm6dsox_context_t *)user_data;
    BaseType_t xTaskWoken = pdFALSE;
    i3c_master_ibi_event_t evt = {
        .ibi_id = evt_data->ibi_id,
        .ibi_data_len = evt_data->data_length,
    };

    xQueueSendFromISR(sens_ctx->event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

void app_main(void)
{
    // I3C bus initialization
    lsm6dsox_context_t sens_ctx = {0};
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
        .entdaa_device_num = 2, // Only 1 device can be discovered by ENTDAA in this example. Set 2 because the example code can have 2 i3c device on bus but only use lsm6dsox as one of them.
        .internal_pullup_resistor_val = I3C_MASTER_INTERNAL_PULLUP_RESISTOR_0_3K,
    };

    i3c_master_bus_handle_t i3c_handle;

    ESP_ERROR_CHECK(i3c_new_master_bus(&i3c_bus_config, &i3c_handle));

    // Initialize event queue
    sens_ctx.event_queue = xQueueCreate(LSM6DSOX_IBI_QUEUE_SIZE, sizeof(i3c_master_ibi_event_t));
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
    ESP_ERROR_CHECK(i3c_master_get_valid_address_slot(i3c_handle, LSM6DSOX_DYNAMIC_ADDR, &dynamic_addr));
    i3c_master_i3c_device_handle_t dev;
    i3c_device_i3c_config_t dev_cfg = {
        .dynamic_addr = dynamic_addr,
        .static_addr = LSM6DSOX_STATIC_ADDRESS,
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
        if (info.pid == 0x08020B10) {
            dev = dev_handle;
            dynamic_addr = info.dynamic_addr;
            break;
        }
    }

    if (dev == NULL) {
        ESP_LOGE(TAG, "Failed to find device");
        return;
    }
    // Free the device handle table after finding the device, which is no longer needed.
    ESP_ERROR_CHECK(i3c_master_free_device_handle_table(dev_table));
#endif

    // Set maximum read length
    uint32_t setmrl_data = SETMRL_DATA;
    i3c_master_ccc_transfer_config_t trans_ccc2 = {
        .ccc_command = I3C_CCC_SETMRL_DIRECT,
        .direction = I3C_MASTER_TRANSFER_DIRECTION_WRITE,
        .device_address = dynamic_addr,
        .data = (uint8_t*) &setmrl_data,
        .data_size = 3,
    };
    ESP_ERROR_CHECK(i3c_master_transfer_ccc(i3c_handle, &trans_ccc2));

    // Allocate buffers for application context
    sens_ctx.tx_buf = heap_caps_calloc(1, LSM6DSOX_BUF_SIZE, MALLOC_CAP_DEFAULT);
    if (sens_ctx.tx_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate tx buffer");
        return;
    }

    sens_ctx.rx_buf = heap_caps_calloc(1, LSM6DSOX_BUF_SIZE, MALLOC_CAP_DEFAULT);
    if (sens_ctx.rx_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate rx buffer");
        return;
    }

    sens_ctx.dev_handle = dev;

    uint8_t *i3c_mst_tx_data = sens_ctx.tx_buf;

    // Configure IBI
    i3c_ibi_config_t ibi_config = {
        .enable_ibi = 1,
        .enable_ibi_payload = 1,
    };
    ESP_ERROR_CHECK(i3c_master_i3c_device_ibi_config(dev, &ibi_config));

    // Configure interrupt pins - disable all interrupts initially
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_INT1, LSM6DSOX_REG_DEFAULT_VALUE));
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_INT2, LSM6DSOX_REG_DEFAULT_VALUE));

    // Set the gyroscope control register to work at 104 Hz, 2000 dps and in bypass mode
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_CTRL2_G, LSM6DSOX_CTRL2_G_ODR_104HZ_2000DPS));

    // Set the Accelerometer control register to work at 104 Hz, 4 g,and in bypass mode and enable ODR/4
    // low pass filter (check figure9 of LSM6DSOX's datasheet)
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_CTRL1_XL, LSM6DSOX_CTRL1_XL_ODR_104HZ_4G));

    // Configure control registers
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_CTRL4_C, LSM6DSOX_REG_DEFAULT_VALUE));
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_CTRL3_C, LSM6DSOX_CTRL3_C_DEFAULT));
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_CTRL5_C, LSM6DSOX_REG_DEFAULT_VALUE));
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_CTRL6_C, LSM6DSOX_REG_DEFAULT_VALUE));

    // Set gyroscope power mode to high performance
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_CTRL7_G, LSM6DSOX_REG_DEFAULT_VALUE));

    // Set the ODR config register to ODR/4
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_CTRL8_XL, LSM6DSOX_REG_DEFAULT_VALUE));
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_CTRL9_XL, LSM6DSOX_CTRL9_XL_DEFAULT));
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_CTRL10_C, LSM6DSOX_REG_DEFAULT_VALUE));

    // Configure TAP detection registers
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_TAP_CFG0, LSM6DSOX_TAP_CFG0_XYZ_EN));
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_TAP_CFG1, LSM6DSOX_TAP_CFG1_XYZ_EN));
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_TAP_CFG2, LSM6DSOX_TAP_CFG2_XYZ_EN));
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_TAP_THS_6D, LSM6DSOX_TAP_THS_DEFAULT));
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_INT_DUR2, LSM6DSOX_INT_DUR2_DEFAULT));
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_MD1_CFG, LSM6DSOX_MD1_CFG_TAP_EN));
    ESP_ERROR_CHECK(lsm6dsox_write_register(dev, i3c_mst_tx_data, LSM6DSOX_I3C_BUS_AVB, LSM6DSOX_I3C_BUS_AVB_DEFAULT));

    uint8_t enec_data = ENEC_IBI;
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

    // Store main task handle for task notification
    sens_ctx.main_task_handle = xTaskGetCurrentTaskHandle();

    // Create temperature reading task after sensor configuration is complete
    xTaskCreate(temperature_reading_task, "temp_reader", LSM6DSOX_DEFAULT_TASK_STACK_SIZE, &sens_ctx, TEMP_TASK_PRIORITY, &sens_ctx.temp_task_handle);

    // Create IBI handler task with app context
    xTaskCreate(ibi_handler_task, "ibi_handler", LSM6DSOX_DEFAULT_TASK_STACK_SIZE, &sens_ctx, IBI_TASK_PRIORITY, &sens_ctx.ibi_task_handle);

    // Wait for IBI events (can handle multiple events)
    ESP_LOGI(TAG, "Waiting for IBI events... Tap the sensor to trigger interrupt");
    ESP_LOGI(TAG, "Temperature reading task started - will read temperature every second");

    // Wait for some time to allow multiple IBI events
    vTaskDelay(pdMS_TO_TICKS(DEMO_DURATION_MS));  // Wait for demo

    // Signal tasks to stop
    ESP_LOGI(TAG, "Stopping tasks...");
    // Clear any pending notifications before waiting
    xTaskNotifyWait(ULONG_MAX, 0x00, NULL, 0);
    sens_ctx.stop_tasks = true;

    // Wait for tasks to finish gracefully using task notification
    uint32_t notification_value = 0;
    const uint32_t temp_task_bit = 0x01;  // Bit 0 for temperature task
    const uint32_t ibi_task_bit = 0x02;   // Bit 1 for IBI task

    // Wait for temperature task to finish and delete it
    if (sens_ctx.temp_task_handle != NULL) {
        // Wait for temp_task_bit to be set
        xTaskNotifyWait(0x00, temp_task_bit, &notification_value, pdMS_TO_TICKS(LSM6DSOX_TASK_STOP_TIMEOUT_MS));
        // Delete the task after receiving notification
        vTaskDelete(sens_ctx.temp_task_handle);
        sens_ctx.temp_task_handle = NULL;
    }

    // Wait for IBI task to finish and delete it
    if (sens_ctx.ibi_task_handle != NULL) {
        // Wait for ibi_task_bit to be set
        xTaskNotifyWait(0x00, ibi_task_bit, &notification_value, pdMS_TO_TICKS(LSM6DSOX_TASK_STOP_TIMEOUT_MS));
        // Delete the task after receiving notification
        vTaskDelete(sens_ctx.ibi_task_handle);
        sens_ctx.ibi_task_handle = NULL;
    }

    ESP_LOGI(TAG, "lsm6dsox demo completed");
    vQueueDelete(sens_ctx.event_queue);
    free(sens_ctx.tx_buf);
    free(sens_ctx.rx_buf);

    ESP_ERROR_CHECK(i3c_master_bus_rm_i3c_device(dev));
    ESP_ERROR_CHECK(i3c_del_master_bus(i3c_handle));

    return;
}
