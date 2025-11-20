/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "example_af.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/isp_af.h"
#include "isp_af_scheme_sa.h"
#include "esp_sccb_intf.h"
#include "example_config.h"

static const char *TAG = "isp_af";

// DW9714 register structure
typedef union {
    struct {
        uint16_t s    : 4;
        uint16_t d    : 10;
        uint16_t flag : 1;
        uint16_t pd   : 1;
    };
    struct {
        uint16_t byte2 : 8;
        uint16_t byte1 : 8;
    };
    uint16_t val;
} dw9714_reg_t;

static example_isp_af_task_param_t s_af_task_param = {0};
static TaskHandle_t s_af_task_handle = NULL;

/**
 * @brief AF environment change callback (called from ISR)
 */
static bool IRAM_ATTR s_af_env_change_cb(isp_af_ctlr_t af_ctrlr, const esp_isp_af_env_detector_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    TaskHandle_t task_handle = (TaskHandle_t)user_data;
    vTaskNotifyGiveFromISR(task_handle, &mustYield);
    return (mustYield == pdTRUE);
}

/**
 * @brief Set focus value to DW9714 VCM
 */
static esp_err_t s_sensor_set_focus_val(int focus_val, void *arg)
{
    esp_sccb_io_handle_t dw9714_io_handle = arg;

    dw9714_reg_t reg = {0};
    reg.d = (uint16_t)((focus_val / 120.0) * 1023.0);

    uint8_t data[2] = {0};
    data[0] = reg.byte1;
    data[1] = reg.byte2;

    uint16_t reg_addr = (data[0] << 8) + (data[1]);
    uint8_t reg_val = 0;

    esp_err_t ret = esp_sccb_transmit_reg_a16v8(dw9714_io_handle, reg_addr, reg_val);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "dw9714 esp_sccb_transmit_reg_a16v8 failed");
        return ret;
    }

    return ESP_OK;
}

/**
 * @brief AF task - continuously monitors and adjusts focus
 */
static void example_af_task(void *arg)
{
    TaskHandle_t task_handle = xTaskGetCurrentTaskHandle();
    example_isp_af_task_param_t af_task_param = *(example_isp_af_task_param_t *)arg;

    // Default AF window configuration (centered window)
    int window_size = 100;
    int edge_thresh = 128;
    int env_interval = 10;
    int first_step_val = 12;
    int first_approx_cycles = 10;
    int second_step_val = 2;
    int second_approx_cycles = 10;
    int focus_val_max = 120;

    // Use default values from example_config if available
    int h_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES;
    int v_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES;

    /**
     * AF window configuration
     * Windows for ISP hardware to record the luminance and definition
     */
    esp_isp_af_config_t af_config = {
        .window = {
            [0] = {
                .top_left = {
                    .x = (h_res / 2) - window_size,
                    .y = (v_res / 2) - window_size,
                },
                .btm_right = {
                    .x = (h_res / 2) + window_size - 1,
                    .y = (v_res / 2) + window_size - 1,
                },
            },
            [1] = {
                .top_left = {
                    .x = (h_res / 2) - window_size,
                    .y = (v_res / 2) - window_size,
                },
                .btm_right = {
                    .x = (h_res / 2) + window_size - 1,
                    .y = (v_res / 2) + window_size - 1,
                },
            },
            [2] = {
                .top_left = {
                    .x = (h_res / 2) - window_size,
                    .y = (v_res / 2) - window_size,
                },
                .btm_right = {
                    .x = (h_res / 2) + window_size - 1,
                    .y = (v_res / 2) + window_size - 1,
                },
            },
        },
        .edge_thresh = edge_thresh,
    };

    isp_af_ctlr_t af_ctrlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_af_controller(af_task_param.isp_proc, &af_config, &af_ctrlr));

    // Configure environment detector
    esp_isp_af_env_config_t env_config = {
        .interval = env_interval,
    };
    ESP_ERROR_CHECK(esp_isp_af_controller_set_env_detector(af_ctrlr, &env_config));

    // Register environment change callback
    esp_isp_af_env_detector_evt_cbs_t cbs = {
        .on_env_change = s_af_env_change_cb,
    };
    ESP_ERROR_CHECK(esp_isp_af_env_detector_register_event_callbacks(af_ctrlr, &cbs, task_handle));

    // Create SA (Step-and-Approach) scheme
    isp_af_sa_scheme_config_t af_scheme_config = {
        .first_step_val = first_step_val,
        .first_approx_cycles = first_approx_cycles,
        .second_step_val = second_step_val,
        .second_approx_cycles = second_approx_cycles,
    };
    isp_af_scheme_handle_t af_scheme = NULL;
    ESP_ERROR_CHECK(isp_af_create_sa_scheme(af_ctrlr, &af_scheme_config, &af_scheme));

    // Register sensor driver
    isp_af_sa_scheme_sensor_drv_t sensor_driver = {
        .af_sensor_set_focus = s_sensor_set_focus_val,
    };
    isp_af_sa_scheme_sensor_info_t sensor_info = {
        .focus_val_max = focus_val_max,
    };
    ESP_ERROR_CHECK(isp_af_sa_scheme_register_sensor_driver(af_scheme, &sensor_driver, &sensor_info, af_task_param.dw9714_io_handle));

    // Enable AF controller
    int definition_thresh = 0;
    int luminance_thresh = 0;
    ESP_ERROR_CHECK(esp_isp_af_controller_enable(af_ctrlr));

    ESP_LOGI(TAG, "AF task started");

    // Main AF loop
    while (1) {
        // Wait for environment change notification
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // Process AF: adjust focus based on definition and luminance
        ESP_ERROR_CHECK(isp_af_process(af_scheme, &definition_thresh, &luminance_thresh));

        // Update environment detector thresholds
        ESP_ERROR_CHECK(esp_isp_af_controller_set_env_detector_threshold(af_ctrlr, definition_thresh, luminance_thresh));
    }
}

esp_err_t example_isp_af_init(isp_proc_handle_t isp_proc,
                              esp_sccb_io_handle_t dw9714_io_handle)
{
    if (isp_proc == NULL || dw9714_io_handle == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    s_af_task_param.isp_proc = isp_proc;
    s_af_task_param.dw9714_io_handle = dw9714_io_handle;

    ESP_LOGI(TAG, "AF module initialized");
    return ESP_OK;
}

esp_err_t example_isp_af_start(int task_priority, int core_id)
{
    if (s_af_task_param.isp_proc == NULL) {
        ESP_LOGE(TAG, "AF not initialized, call example_isp_af_init() first");
        return ESP_ERR_INVALID_STATE;
    }

    BaseType_t ret = xTaskCreatePinnedToCore(example_af_task,
                                             "af_task",
                                             8192,
                                             &s_af_task_param,
                                             task_priority,
                                             &s_af_task_handle,
                                             core_id);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create AF task");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "AF task created");
    return ESP_OK;
}
