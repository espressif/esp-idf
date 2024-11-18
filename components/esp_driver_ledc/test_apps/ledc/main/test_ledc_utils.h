/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "soc/soc_caps.h"
#include "driver/ledc.h"

#define PULSE_IO      5

#define TEST_PWM_FREQ       2000
#define TEST_PWM_LOW_FREQ   200

#if SOC_LEDC_SUPPORT_HS_MODE
#define TEST_SPEED_MODE LEDC_HIGH_SPEED_MODE
#define SPEED_MODE_LIST {LEDC_HIGH_SPEED_MODE, LEDC_LOW_SPEED_MODE}
#else
#define TEST_SPEED_MODE LEDC_LOW_SPEED_MODE
#define SPEED_MODE_LIST {LEDC_LOW_SPEED_MODE}
#endif

#if SOC_LEDC_SUPPORT_APB_CLOCK
#define TEST_DEFAULT_CLK_CFG LEDC_USE_APB_CLK
#elif SOC_LEDC_SUPPORT_PLL_DIV_CLOCK
#if SOC_CLK_TREE_SUPPORTED
#define TEST_DEFAULT_CLK_CFG LEDC_USE_PLL_DIV_CLK
#else
#define TEST_DEFAULT_CLK_CFG LEDC_USE_XTAL_CLK
#endif
#endif

/**
 * Initialize a LEDC channel config structure
 */
ledc_channel_config_t initialize_channel_config(void);

/**
 * Initialize a LEDC timer config structure
 */
ledc_timer_config_t create_default_timer_config(void);

#if SOC_PCNT_SUPPORTED
/**
 * Setup PCNT test bench
 */
void setup_testbench(void);

/**
 * Tear down PCNT test bench
 */
void tear_testbench(void);

/**
 * Use PCNT to count pulse
 *
 * @param last_time Duration time in ms
 * @return Pulse count
 */
int wave_count(int last_time);
#endif
