/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// TODO: IDF-5645
#if CONFIG_IDF_TARGET_ESP32C6
#include "soc/lp_aon_reg.h"
#include "soc/lp_analog_peri_reg.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/lp_i2c_reg.h"
#include "soc/lp_i2c_struct.h"
#include "soc/lp_io_reg.h"
#include "soc/lp_io_struct.h"
#include "soc/lp_timer_reg.h"
#include "soc/lp_timer_struct.h"
#include "soc/lp_uart_reg.h"
#include "soc/lp_uart_struct.h"
#include "soc/lp_wdt_reg.h"
#include "soc/lp_wdt_struct.h"
#else
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"
#endif
