/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_types.h"
#if CONFIG_I2C_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "freertos/FreeRTOS.h"
#include "hal/i2c_hal.h"
#include "hal/gpio_hal.h"
#include "esp_private/periph_ctrl.h"
#include "esp_rom_gpio.h"
#include "i2c_private.h"
#include "driver/gpio.h"
#include "soc/clk_tree_defs.h"
#include "soc/i2c_periph.h"
#include "esp_clk_tree.h"
#include "clk_ctrl_os.h"

static const char *TAG = "i2c.common";

typedef struct i2c_platform_t {
    _lock_t mutex;                        // platform level mutex lock.
    i2c_bus_handle_t buses[SOC_I2C_NUM];  // array of I2C bus instances.
    uint32_t count[SOC_I2C_NUM];          // reference count used to protect group install/uninstall.
} i2c_platform_t;

static i2c_platform_t s_i2c_platform = {}; // singleton platform

static esp_err_t s_i2c_bus_handle_aquire(i2c_port_num_t port_num, i2c_bus_handle_t *i2c_new_bus, i2c_bus_mode_t mode)
{
#if CONFIG_I2C_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    bool new_bus = false;
    i2c_bus_t *bus = NULL;
    esp_err_t ret = ESP_OK;

    if (!s_i2c_platform.buses[port_num]) {
        new_bus = true;
        bus = heap_caps_calloc(1, sizeof(i2c_bus_t), I2C_MEM_ALLOC_CAPS);
        if (bus) {
            s_i2c_platform.buses[port_num] = bus;
            bus->port_num = port_num;
            bus->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
            bus->bus_mode = mode;

            // Enable the I2C module
            periph_module_enable(i2c_periph_signal[port_num].module);
            periph_module_reset(i2c_periph_signal[port_num].module);
            i2c_hal_init(&bus->hal, port_num);
        }
    } else {
        ESP_LOGE(TAG, "I2C bus id(%d) has already been acquired", port_num);
        bus = s_i2c_platform.buses[port_num];
        ret = ESP_ERR_INVALID_STATE;
    }
    if (bus) {
        s_i2c_platform.count[port_num]++;
    }

    if (new_bus) {
        ESP_LOGD(TAG, "new bus(%d) at %p", port_num, bus);
    }
    *i2c_new_bus = bus;
    return ret;
}

static bool i2c_bus_occupied(i2c_port_num_t port_num)
{
    return s_i2c_platform.buses[port_num] != NULL;
}

esp_err_t i2c_acquire_bus_handle(i2c_port_num_t port_num, i2c_bus_handle_t *i2c_new_bus, i2c_bus_mode_t mode)
{
    bool bus_occupied = false;
    bool bus_found = false;
    esp_err_t ret = ESP_OK;
    _lock_acquire(&s_i2c_platform.mutex);
    if (port_num == -1) {
        for (int i = 0; i < SOC_I2C_NUM; i++) {
            bus_occupied = i2c_bus_occupied(i);
            if (bus_occupied == false) {
                ret = s_i2c_bus_handle_aquire(i, i2c_new_bus, mode);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "acquire bus failed");
                    _lock_release(&s_i2c_platform.mutex);
                    return ret;
                }
                bus_found = true;
                port_num = i;
                break;
            }
        }
        ESP_RETURN_ON_FALSE((bus_found == true), ESP_ERR_NOT_FOUND, TAG, "acquire bus failed, no free bus");
    } else {
        ret = s_i2c_bus_handle_aquire(port_num, i2c_new_bus, mode);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "acquire bus failed");
        }
    }
    _lock_release(&s_i2c_platform.mutex);
    return ret;
}

esp_err_t i2c_release_bus_handle(i2c_bus_handle_t i2c_bus)
{
    int port_num = i2c_bus->port_num;
    i2c_clock_source_t clk_src = i2c_bus->clk_src;
    bool do_deinitialize = false;
    _lock_acquire(&s_i2c_platform.mutex);
    if (s_i2c_platform.buses[port_num]) {
        s_i2c_platform.count[port_num]--;
        if (s_i2c_platform.count[port_num] == 0) {
            do_deinitialize = true;
            s_i2c_platform.buses[port_num] = NULL;
            if (i2c_bus->intr_handle) {
                ESP_RETURN_ON_ERROR(esp_intr_free(i2c_bus->intr_handle), TAG, "delete interrupt service failed");
            }
            if (i2c_bus->pm_lock) {
                ESP_RETURN_ON_ERROR(esp_pm_lock_delete(i2c_bus->pm_lock), TAG, "delete pm_lock failed");
            }
            // Disable I2C module
            periph_module_disable(i2c_periph_signal[port_num].module);
            free(i2c_bus);
        }
    }
    _lock_release(&s_i2c_platform.mutex);

    switch (clk_src) {
#if SOC_I2C_SUPPORT_RTC
    case I2C_CLK_SRC_RC_FAST:
        periph_rtc_dig_clk8m_disable();
        break;
#endif // SOC_I2C_SUPPORT_RTC
    default:
        break;
    }

    if (do_deinitialize) {
        ESP_LOGD(TAG, "delete bus %d", port_num);
    }

    ESP_RETURN_ON_FALSE(s_i2c_platform.count[port_num] == 0, ESP_ERR_INVALID_STATE, TAG, "Bus not freed entirely");
    return ESP_OK;
}

esp_err_t i2c_select_periph_clock(i2c_bus_handle_t handle, i2c_clock_source_t clk_src)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "I2C empty controller handle");
    uint32_t periph_src_clk_hz = 0;
    bool clock_selection_conflict = 0;

    portENTER_CRITICAL(&handle->spinlock);
    if (handle->clk_src == 0) {
        handle->clk_src = clk_src;
    } else {
        clock_selection_conflict = (handle->clk_src != clk_src);
    }
    portEXIT_CRITICAL(&handle->spinlock);
    ESP_RETURN_ON_FALSE(!clock_selection_conflict, ESP_ERR_INVALID_STATE, TAG,
                        "group clock conflict, already is %d but attempt to %d", handle->clk_src, clk_src);

    // TODO: [clk_tree] to use a generic clock enable/disable or acquire/release function for all clock source
#if SOC_I2C_SUPPORT_RTC
    if (clk_src == I2C_CLK_SRC_RC_FAST) {
        // RC_FAST clock is not enabled automatically on start up, we enable it here manually.
        // Note there's a ref count in the enable/disable function, we must call them in pair in the driver.
        periph_rtc_dig_clk8m_enable();
    }
#endif // SOC_I2C_SUPPORT_RTC

    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz(clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &periph_src_clk_hz), TAG, "i2c get clock frequency error");

    handle->clk_src_freq_hz = periph_src_clk_hz;

#if CONFIG_PM_ENABLE
    bool need_pm_lock = true;
    // to make the I2C work reliable, the source clock must stay alive and unchanged
    // driver will create different pm lock for that purpose, according to different clock source
    esp_pm_lock_type_t pm_lock_type = ESP_PM_NO_LIGHT_SLEEP;

#if SOC_I2C_SUPPORT_RTC
    if (clk_src == I2C_CLK_SRC_RC_FAST) {
        // I2C use fifo, which connected to APB, so we cannot use I2C either when in light sleep.
        need_pm_lock = ESP_PM_NO_LIGHT_SLEEP;
    }
#endif // SOC_I2C_SUPPORT_RTC

#if SOC_I2C_SUPPORT_APB
    if (clk_src == I2C_CLK_SRC_APB) {
        // APB clock frequency can be changed during DFS
        pm_lock_type = ESP_PM_APB_FREQ_MAX;
    }
#endif // SOC_I2C_SUPPORT_APB

    if (need_pm_lock) {
        sprintf(handle->pm_lock_name, "I2C_%d", handle->port_num); // e.g. PORT_0
        ret  = esp_pm_lock_create(pm_lock_type, 0, handle->pm_lock_name, &handle->pm_lock);
        ESP_RETURN_ON_ERROR(ret, TAG, "create pm lock failed");
    }
#endif // CONFIG_PM_ENABLE


    ESP_LOGD(TAG, "bus clock source frequency: %"PRIu32"hz", periph_src_clk_hz);
    return ret;
}

esp_err_t i2c_common_set_pins(i2c_bus_handle_t handle)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_INVALID_ARG, TAG, "I2C empty controller handle");
    int port_id = handle->port_num;

    // SDA pin configurations
    gpio_config_t sda_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT_OD,
        .pull_down_en = false,
        .pull_up_en = handle->pull_up_enable ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pin_bit_mask = 1ULL << handle->sda_num,
    };
    ESP_RETURN_ON_ERROR(gpio_set_level(handle->sda_num, 1), TAG, "i2c sda pin set level failed");
    ESP_RETURN_ON_ERROR(gpio_config(&sda_conf), TAG, "config GPIO failed");
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[handle->sda_num], PIN_FUNC_GPIO);
    esp_rom_gpio_connect_out_signal(handle->sda_num, i2c_periph_signal[port_id].sda_out_sig, 0, 0);
    esp_rom_gpio_connect_in_signal(handle->sda_num, i2c_periph_signal[port_id].sda_in_sig, 0);


    // SCL pin configurations
    gpio_config_t scl_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT_OD,
        .pull_down_en = false,
        .pull_up_en = handle->pull_up_enable ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
        .pin_bit_mask = 1ULL << handle->scl_num,
    };
    ESP_RETURN_ON_ERROR(gpio_set_level(handle->scl_num, 1), TAG, "i2c scl pin set level failed");
    ESP_RETURN_ON_ERROR(gpio_config(&scl_conf), TAG, "config GPIO failed");
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[handle->scl_num], PIN_FUNC_GPIO);
    esp_rom_gpio_connect_out_signal(handle->scl_num, i2c_periph_signal[port_id].scl_out_sig, 0, 0);
    esp_rom_gpio_connect_in_signal(handle->scl_num, i2c_periph_signal[port_id].scl_in_sig, 0);
    return ret;
}
