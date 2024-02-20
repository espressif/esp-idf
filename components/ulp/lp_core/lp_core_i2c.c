/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lp_core_i2c.h"
#include "esp_check.h"
#include "hal/i2c_hal.h"
#include "soc/lp_io_struct.h"
#include "driver/rtc_io.h"
#include "soc/rtc_io_channel.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/periph_ctrl.h"

static const char *LPI2C_TAG = "lp_core_i2c";

#define LP_I2C_FILTER_CYC_NUM_DEF       (7)

/* I2C LL context */
i2c_hal_context_t i2c_hal;

/* Use the register structure to access LP_IO module registers */
lp_io_dev_t *lp_io_dev = &LP_IO;

static esp_err_t lp_i2c_gpio_is_cfg_valid(gpio_num_t sda_io_num, gpio_num_t scl_io_num)
{
    /* Verify that the SDA and SCL GPIOs are valid LP IO (RTCIO) pins */
    ESP_RETURN_ON_ERROR(!rtc_gpio_is_valid_gpio(sda_io_num), LPI2C_TAG, "LP I2C SDA GPIO invalid");
    ESP_RETURN_ON_ERROR(!rtc_gpio_is_valid_gpio(scl_io_num), LPI2C_TAG, "LP I2C SCL GPIO invalid");

    /* Verify that the SDA and SCL line belong to the LP IO Mux I2C function group */
    if (sda_io_num != RTCIO_GPIO6_CHANNEL) {
        ESP_LOGE(LPI2C_TAG, "SDA pin can only be configured as GPIO#6");
        return ESP_ERR_INVALID_ARG;
    }

    if (scl_io_num != RTCIO_GPIO7_CHANNEL) {
        ESP_LOGE(LPI2C_TAG, "SCL pin can only be configured as GPIO#7");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

static esp_err_t lp_i2c_configure_io(gpio_num_t io_num, bool pullup_en)
{
    /* Initialize IO Pin */
    ESP_RETURN_ON_ERROR(rtc_gpio_init(io_num), LPI2C_TAG, "LP GPIO Init failed for GPIO %d", io_num);

    /* Set direction to input+output open-drain mode */
    ESP_RETURN_ON_ERROR(rtc_gpio_set_direction(io_num, RTC_GPIO_MODE_INPUT_OUTPUT_OD), LPI2C_TAG, "LP GPIO Set direction failed for %d", io_num);
    /* Disable pulldown on the io pin */
    ESP_RETURN_ON_ERROR(rtc_gpio_pulldown_dis(io_num), LPI2C_TAG, "LP GPIO pulldown disable failed for %d", io_num);
    /* Enable pullup based on pullup_en flag */
    if (pullup_en) {
        ESP_RETURN_ON_ERROR(rtc_gpio_pullup_en(io_num), LPI2C_TAG, "LP GPIO pullup enable failed for %d", io_num);
    } else {
        ESP_RETURN_ON_ERROR(rtc_gpio_pullup_dis(io_num), LPI2C_TAG, "LP GPIO pullup disable failed for %d", io_num);
    }

    return ESP_OK;
}

static esp_err_t lp_i2c_set_pin(const lp_core_i2c_cfg_t *cfg)
{
    gpio_num_t sda_io_num = cfg->i2c_pin_cfg.sda_io_num;
    gpio_num_t scl_io_num = cfg->i2c_pin_cfg.scl_io_num;
    bool sda_pullup_en = cfg->i2c_pin_cfg.sda_pullup_en;
    bool scl_pullup_en = cfg->i2c_pin_cfg.scl_pullup_en;

    /* Verify that the LP I2C GPIOs are valid */
    ESP_RETURN_ON_ERROR(lp_i2c_gpio_is_cfg_valid(sda_io_num, scl_io_num), LPI2C_TAG, "LP I2C GPIO config invalid");

    /* Initialize SDA Pin */
    ESP_RETURN_ON_ERROR(lp_i2c_configure_io(sda_io_num, sda_pullup_en), LPI2C_TAG, "LP I2C SDA pin config failed");

    /* Initialize SCL Pin */
    ESP_RETURN_ON_ERROR(lp_i2c_configure_io(scl_io_num, scl_pullup_en), LPI2C_TAG, "LP I2C SCL pin config failed");

    /* Select LP I2C function for the SDA Pin */
    lp_io_dev->gpio[sda_io_num].mcu_sel = 1;

    /* Select LP I2C function for the SCL Pin */
    lp_io_dev->gpio[scl_io_num].mcu_sel = 1;

    return ESP_OK;
}

static esp_err_t lp_i2c_config_clk(const lp_core_i2c_cfg_t *cfg)
{
    esp_err_t ret = ESP_OK;
    uint32_t source_freq = 0;
    soc_periph_lp_i2c_clk_src_t source_clk = LP_I2C_SCLK_DEFAULT;

    /* Check if we have a valid user configured source clock */
    soc_periph_lp_i2c_clk_src_t clk_srcs[] = SOC_LP_I2C_CLKS;
    for (int i = 0; i < sizeof(clk_srcs) / sizeof(clk_srcs[0]); i++) {
        if (clk_srcs[i] == cfg->i2c_src_clk) {
            /* Clock source matches. Override the source clock type with the user configured value */
            source_clk = cfg->i2c_src_clk;
            break;
        }
    }

    /* Fetch the clock source fequency */
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz(source_clk, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &source_freq), LPI2C_TAG, "Invalid LP I2C source clock selected");

    /* Verify that the I2C_SCLK operates at a frequency 20 times larger than the requested SCL bus frequency */
    ESP_RETURN_ON_FALSE(cfg->i2c_timing_cfg.clk_speed_hz * 20 <= source_freq, ESP_ERR_INVALID_ARG, LPI2C_TAG, "I2C_SCLK frequency (%"PRId32") should operate at a frequency at least 20 times larger than the I2C SCL bus frequency (%"PRId32")", source_freq, cfg->i2c_timing_cfg.clk_speed_hz);

    /* LP I2C clock source is mixed with other peripherals in the same register */
    PERIPH_RCC_ATOMIC() {
        lp_i2c_ll_set_source_clk(i2c_hal.dev, source_clk);
    }

    /* Configure LP I2C timing paramters. source_clk is ignored for LP_I2C in this call */
    i2c_hal_set_bus_timing(&i2c_hal, cfg->i2c_timing_cfg.clk_speed_hz, (i2c_clock_source_t)source_clk, source_freq);

    return ret;
}

esp_err_t lp_core_i2c_master_init(i2c_port_t lp_i2c_num, const lp_core_i2c_cfg_t *cfg)
{
    /* Verify LP_I2C port number */
    ESP_RETURN_ON_FALSE((lp_i2c_num == LP_I2C_NUM_0), ESP_ERR_INVALID_ARG, LPI2C_TAG, "LP I2C port number incorrect");

    /* Verify that the input cfg param is valid */
    ESP_RETURN_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, LPI2C_TAG, "LP I2C configuration is NULL");

    /* Configure LP I2C GPIOs */
    ESP_RETURN_ON_ERROR(lp_i2c_set_pin(cfg), LPI2C_TAG, "Failed to configure LP I2C GPIOs");

    PERIPH_RCC_ATOMIC() {
        /* Enable LP I2C bus clock */
        lp_i2c_ll_enable_bus_clock(lp_i2c_num - LP_I2C_NUM_0, true);
        /* Reset LP I2C register */
        lp_i2c_ll_reset_register(lp_i2c_num - LP_I2C_NUM_0);
    }

    /* Initialize LP I2C HAL */
    i2c_hal_init(&i2c_hal, lp_i2c_num);

    /* Initialize LP I2C Master mode */
    i2c_hal_master_init(&i2c_hal);

    /* Enable internal open-drain mode for I2C IO lines */
    i2c_hal.dev->ctr.sda_force_out = 0;
    i2c_hal.dev->ctr.scl_force_out = 0;

    /* Configure LP I2C clock and timing paramters */
    ESP_RETURN_ON_ERROR(lp_i2c_config_clk(cfg), LPI2C_TAG, "Failed to configure LP I2C source clock");

    /* Enable SDA and SCL filtering. This configuration matches the HP I2C filter config */
    i2c_ll_master_set_filter(i2c_hal.dev, LP_I2C_FILTER_CYC_NUM_DEF);

    /* Configure the I2C master to send a NACK when the Rx FIFO count is full */
    i2c_ll_master_rx_full_ack_level(i2c_hal.dev, 1);

    /* Synchronize the config register values to the LP I2C peripheral clock */
    i2c_ll_update(i2c_hal.dev);

    return ESP_OK;
}
