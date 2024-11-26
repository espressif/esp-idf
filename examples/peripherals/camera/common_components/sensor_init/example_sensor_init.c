/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c_master.h"
#include "esp_sccb_intf.h"
#include "esp_sccb_i2c.h"
#include "esp_cam_sensor_detect.h"
#include "example_sensor_init.h"
#include "example_sensor_init_config.h"

static const char *TAG = "sensor_init";

void example_sensor_init(example_sensor_config_t *sensor_config, i2c_master_bus_handle_t *out_i2c_bus_handle)
{
    esp_err_t ret = ESP_FAIL;

    //---------------I2C Init------------------//
    i2c_master_bus_config_t i2c_bus_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = sensor_config->i2c_sda_io_num,
        .scl_io_num = sensor_config->i2c_scl_io_num,
        .i2c_port = sensor_config->i2c_port_num,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t i2c_bus_handle = NULL;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_conf, &i2c_bus_handle));

    //---------------SCCB Init------------------//
    esp_cam_sensor_config_t cam_config = {
        .reset_pin = -1,
        .pwdn_pin = -1,
        .xclk_pin = -1,
    };

    esp_cam_sensor_device_t *cam = NULL;
    for (esp_cam_sensor_detect_fn_t *p = &__esp_cam_sensor_detect_fn_array_start; p < &__esp_cam_sensor_detect_fn_array_end; ++p) {
        sccb_i2c_config_t i2c_config = {
            .scl_speed_hz = EXAMPLE_CAM_SCCB_FREQ,
            .device_address = p->sccb_addr,
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        };
        ESP_ERROR_CHECK(sccb_new_i2c_io(i2c_bus_handle, &i2c_config, &cam_config.sccb_handle));

        cam_config.sensor_port = p->port;

        cam = (*(p->detect))(&cam_config);
        if (cam) {
            if (p->port != sensor_config->port) {
                ESP_LOGE(TAG, "detect a camera sensor with mismatched interface");
                return;
            }
            break;
        }
        ESP_ERROR_CHECK(esp_sccb_del_i2c_io(cam_config.sccb_handle));
    }

    if (!cam) {
        ESP_LOGE(TAG, "failed to detect camera sensor");
        return;
    }

    esp_cam_sensor_format_array_t cam_fmt_array = {0};
    esp_cam_sensor_query_format(cam, &cam_fmt_array);
    const esp_cam_sensor_format_t *parray = cam_fmt_array.format_array;
    for (int i = 0; i < cam_fmt_array.count; i++) {
        ESP_LOGI(TAG, "fmt[%d].name:%s", i, parray[i].name);
    }

    esp_cam_sensor_format_t *cam_cur_fmt = NULL;
    for (int i = 0; i < cam_fmt_array.count; i++) {
        if (!strcmp(parray[i].name, sensor_config->format_name)) {
            cam_cur_fmt = (esp_cam_sensor_format_t *) & (parray[i]);
        }
    }

    ret = esp_cam_sensor_set_format(cam, (const esp_cam_sensor_format_t *) cam_cur_fmt);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Format set fail");
    } else {
        ESP_LOGI(TAG, "Format in use:%s", cam_cur_fmt->name);
    }
    int enable_flag = 1;
    // Set sensor output stream
    ret = esp_cam_sensor_ioctl(cam, ESP_CAM_SENSOR_IOC_S_STREAM, &enable_flag);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Start stream fail");
    }
    ESP_ERROR_CHECK(ret);

    *out_i2c_bus_handle = i2c_bus_handle;
}
