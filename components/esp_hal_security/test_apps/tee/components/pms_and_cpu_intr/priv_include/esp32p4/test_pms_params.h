/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/apm_types.h"
#include "soc/apm_defs.h"

#define ALIGN_TO_NEXT_4KB(addr)         (((addr) + 0x1000) & ~0xFFF)

/********* For DMA_PMS *********/

#define GDMA_HPMEM_TEST_REGN_NUM        APM_DMA_PMS_REGION_NUM
#define GDMA_HPMEM_TEST_REGN_SIZE       APM_DMA_PMS_REGION_ADDR_ALIGN
/* Reserved regions for HPMEM test: [0] pre-buf, [1] post-buf */
#define GDMA_HPMEM_TEST_REGN_RESV_NUM   2

#define GDMA_EXTMEM_TEST_REGN_NUM       APM_DMA_PMS_REGION_NUM
#define GDMA_EXTMEM_TEST_REGN_SIZE      APM_DMA_PMS_REGION_ADDR_ALIGN
/* Reserved regions for EXTMEM test: [0] pre-buf, [1] post-buf, [2] full HP-mem
 * TODO: Investigate why [2] is required for EXT_MEM case
 */
#define GDMA_EXTMEM_TEST_REGN_RESV_NUM  3

/********* For HP_PERI_PMS *********/

#include "soc/timer_group_reg.h"
#include "soc/i2c_reg.h"
#include "soc/i2s_reg.h"
#include "soc/pcnt_reg.h"
#include "soc/uart_reg.h"
#include "soc/spi_reg.h"
#include "soc/ledc_reg.h"
#include "soc/adc_reg.h"
#include "soc/parl_io_reg.h"
#include "soc/rmt_reg.h"
#include "soc/gpio_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/cache_reg.h"
#include "soc/hp_system_reg.h"
#include "soc/bitscrambler_reg.h"
#include "soc/trace_reg.h"
#include "soc/sha_reg.h"
#include "soc/assist_debug_reg.h"
#include "soc/parl_io_reg.h"
#include "soc/io_mux_reg.h"

/********* For HP2LP_PERI_PMS *********/

#include "soc/lp_timer_reg.h"
#include "soc/lp_analog_peri_reg.h"
#include "soc/pmu_reg.h"
#include "soc/lp_wdt_reg.h"
#include "soc/lp_uart_reg.h"
#include "soc/lp_i2c_reg.h"
#include "soc/lp_i2c_ana_mst_reg.h"
#include "soc/huk_reg.h"
#include "soc/efuse_reg.h"
#include "soc/lp_iomux_reg.h"
#include "soc/trng_reg.h"
