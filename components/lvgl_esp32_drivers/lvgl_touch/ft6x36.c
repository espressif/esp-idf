/*
* Copyright © 2020 Wolfgang Christl

* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the “Software”), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
* to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include <esp_log.h>
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include <lvgl.h>
#else
#include <lvgl/lvgl.h>
#endif
#include "ft6x36.h"

#include "lvgl_i2c/i2c_manager.h"

#define TAG "FT6X36"


ft6x36_status_t ft6x36_status;
uint8_t current_dev_addr;       // set during init

esp_err_t ft6x06_i2c_read8(uint8_t slave_addr, uint8_t register_addr, uint8_t *data_buf) {
    return lvgl_i2c_read(CONFIG_LV_I2C_TOUCH_PORT, slave_addr, register_addr, data_buf, 1);
}


/**
  * @brief  Read the FT6x36 gesture ID. Initialize first!
  * @param  dev_addr: I2C FT6x36 Slave address.
  * @retval The gesture ID or 0x00 in case of failure
  */
uint8_t ft6x36_get_gesture_id() {
    if (!ft6x36_status.inited) {
        ESP_LOGE(TAG, "Init first!");
        return 0x00;
    }
    uint8_t data_buf;
    esp_err_t ret;
    if ((ret = ft6x06_i2c_read8(current_dev_addr, FT6X36_GEST_ID_REG, &data_buf) != ESP_OK))
        ESP_LOGE(TAG, "Error reading from device: %s", esp_err_to_name(ret));
    return data_buf;
}

/**
  * @brief  Initialize for FT6x36 communication via I2C
  * @param  dev_addr: Device address on communication Bus (I2C slave address of FT6X36).
  * @retval None
  */
void ft6x06_init(uint16_t dev_addr) {

    ft6x36_status.inited = true;
    current_dev_addr = dev_addr;
    uint8_t data_buf;
    esp_err_t ret;
    ESP_LOGI(TAG, "Found touch panel controller");
    if ((ret = ft6x06_i2c_read8(dev_addr, FT6X36_PANEL_ID_REG, &data_buf) != ESP_OK))
        ESP_LOGE(TAG, "Error reading from device: %s",
                 esp_err_to_name(ret));    // Only show error the first time
    ESP_LOGI(TAG, "\tDevice ID: 0x%02x", data_buf);

    ft6x06_i2c_read8(dev_addr, FT6X36_CHIPSELECT_REG, &data_buf);
    ESP_LOGI(TAG, "\tChip ID: 0x%02x", data_buf);

    ft6x06_i2c_read8(dev_addr, FT6X36_DEV_MODE_REG, &data_buf);
    ESP_LOGI(TAG, "\tDevice mode: 0x%02x", data_buf);

    ft6x06_i2c_read8(dev_addr, FT6X36_FIRMWARE_ID_REG, &data_buf);
    ESP_LOGI(TAG, "\tFirmware ID: 0x%02x", data_buf);

    ft6x06_i2c_read8(dev_addr, FT6X36_RELEASECODE_REG, &data_buf);
    ESP_LOGI(TAG, "\tRelease code: 0x%02x", data_buf);

}

/**
  * @brief  Get the touch screen X and Y positions values. Ignores multi touch
  * @param  drv:
  * @param  data: Store data here
  * @retval Always false
  */
bool ft6x36_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    if (!ft6x36_status.inited) {
        ESP_LOGE(TAG, "Init first!");
        return 0x00;
    }
    uint8_t data_buf[5];        // 1 byte status, 2 bytes X, 2 bytes Y
    static int16_t last_x = 0;  // 12bit pixel value
    static int16_t last_y = 0;  // 12bit pixel value

    esp_err_t ret = lvgl_i2c_read(CONFIG_LV_I2C_TOUCH_PORT, current_dev_addr, FT6X36_TD_STAT_REG, &data_buf[0], 5);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error talking to touch IC: %s", esp_err_to_name(ret));
    }
    uint8_t touch_pnt_cnt = data_buf[0];  // Number of detected touch points

    if (ret != ESP_OK || touch_pnt_cnt != 1) {    // ignore no touch & multi touch
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_REL;
        return false;
    }

    last_x = ((data_buf[1] & FT6X36_MSB_MASK) << 8) | (data_buf[2] & FT6X36_LSB_MASK);
    last_y = ((data_buf[3] & FT6X36_MSB_MASK) << 8) | (data_buf[4] & FT6X36_LSB_MASK);

#if CONFIG_LV_FT6X36_INVERT_X
    last_x =  LV_HOR_RES - last_x;
#endif
#if CONFIG_LV_FT6X36_INVERT_Y
    last_y = LV_VER_RES - last_y;
#endif
#if CONFIG_LV_FT6X36_SWAPXY
    int16_t swap_buf = last_x;
    last_x = last_y;
    last_y = swap_buf;
#endif
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = LV_INDEV_STATE_PR;
    ESP_LOGD(TAG, "X=%u Y=%u", data->point.x, data->point.y);
    return false;
}
