/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "soc/soc_caps.h"

// TODO: IDF-5645
#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
#include "soc/lp_io_reg.h"
#include "soc/lp_io_struct.h"
#include "soc/lp_aon_reg.h"
#elif CONFIG_IDF_TARGET_ESP32C61
#include "soc/lp_gpio_reg.h"
#include "soc/lp_gpio_struct.h"
#include "soc/lp_aon_reg.h"

// ESP32H2-TODO: IDF-6327
#elif CONFIG_IDF_TARGET_ESP32H2
#include "soc/lp_aon_reg.h"
#elif CONFIG_IDF_TARGET_ESP32P4 || CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
#include "soc/lp_gpio_reg.h"
#include "soc/lp_gpio_struct.h"
#include "soc/lp_iomux_reg.h"
#include "soc/lp_iomux_struct.h"
#endif

// TODO: IDF-5645
#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32P4 || CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C61
#include "soc/lp_analog_peri_reg.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/lp_clkrst_struct.h"
#if SOC_LP_I2C_SUPPORTED
#include "soc/lp_i2c_reg.h"
#include "soc/lp_i2c_struct.h"
#endif
#if SOC_LP_TIMER_SUPPORTED
#include "soc/lp_timer_reg.h"
#include "soc/lp_timer_struct.h"
#endif
#if SOC_ULP_LP_UART_SUPPORTED
#include "soc/lp_uart_reg.h"
#include "soc/lp_uart_struct.h"
#endif
#include "soc/lp_wdt_reg.h"
#include "soc/lp_wdt_struct.h"
#elif CONFIG_IDF_TARGET_ESP32H2
// ESP32H2-TODO: IDF-6327
#include "soc/lp_analog_peri_reg.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/lp_timer_reg.h"
#include "soc/lp_timer_struct.h"
#include "soc/lp_wdt_reg.h"
#include "soc/lp_wdt_struct.h"
#else
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"
#endif
