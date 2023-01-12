/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief PMU modes of HP system
 */
typedef enum {
    PMU_MODE_HP_ACTIVE = 0, /*!< PMU in HP_ACTIVE mode */
    PMU_MODE_HP_MODEM,      /*!< PMU in HP_MODEM mode */
    PMU_MODE_HP_SLEEP,      /*!< PMU in HP_SLEEP mode */
    PMU_MODE_HP_MAX,
} pmu_hp_mode_t;

/**
 * @brief PMU modes of LP system
 */
typedef enum {
    PMU_MODE_LP_ACTIVE = 0, /*!< PMU in LP_ACTIVE mode */
    PMU_MODE_LP_SLEEP,      /*!< PMU in LP_SLEEP mode */
    PMU_MODE_LP_MAX,
} pmu_lp_mode_t;

typedef enum {
    PMU_HP_PD_TOP = 0,      /* Power domain of digital top */
    PMU_HP_PD_AON,          /* Power domain of always-on */
    PMU_HP_PD_CPU,          /* Power domain of HP CPU */
    PMU_HP_PD_RESERVED,     /* Reserved power domain*/
    PMU_HP_PD_WIFI,         /* Power domain of WIFI */
    PMU_HP_PD_MAX
} pmu_hp_power_domain_t;


/* Software configuration instance type from pmu_struct.h */
typedef union {
    struct {
        uint32_t reserved0    : 21;
        uint32_t vdd_spi_pd_en: 1;
        uint32_t mem_dslp     : 1;
        uint32_t mem_pd_en    : 4;
        uint32_t wifi_pd_en   : 1;
        uint32_t reserved1    : 1;
        uint32_t cpu_pd_en    : 1;
        uint32_t aon_pd_en    : 1;
        uint32_t top_pd_en    : 1;
    };
    struct {
        uint32_t reserved2    : 26;
        uint32_t i2c_iso_en   : 1;
        uint32_t i2c_retention: 1;
        uint32_t xpd_bb_i2c   : 1;
        uint32_t xpd_bbpll_i2c: 1;
        uint32_t xpd_bbpll    : 1;
        uint32_t reserved3    : 1;
    };
    struct {
        uint32_t reserved4    : 31;
        uint32_t xpd_xtal     : 1;
    };
    uint32_t val;
} pmu_hp_power_t;

typedef union {
    struct {
        uint32_t reserved0 : 30;
        uint32_t mem_dslp  : 1;
        uint32_t peri_pd_en: 1;
    };
    struct {
        uint32_t reserved1  : 28;
        uint32_t xpd_xtal32k: 1;
        uint32_t xpd_rc32k  : 1;
        uint32_t xpd_fosc   : 1;
        uint32_t pd_osc     : 1;
    };
    struct {
        uint32_t reserved2  : 31;
        uint32_t xpd_xtal   : 1;
    };
    uint32_t val;
} pmu_lp_power_t;

typedef struct {
    struct {
        uint32_t reserved0 : 25;
        uint32_t xpd_bias  : 1;
        uint32_t dbg_atten : 4;
        uint32_t pd_cur    : 1;
        uint32_t bias_sleep: 1;
    };
    struct {
        uint32_t reserved1      : 16;
        uint32_t slp_mem_xpd    : 1;
        uint32_t slp_logic_xpd  : 1;
        uint32_t xpd            : 1;
        uint32_t slp_mem_dbias  : 4;
        uint32_t slp_logic_dbias: 4;
        uint32_t dbias          : 5;
    };
    struct {
        uint32_t reserved2: 8;
        uint32_t drv_b    : 24;
    };
} pmu_hp_analog_t;

typedef struct {
    struct {
        uint32_t reserved0 : 25;
        uint32_t xpd_bias  : 1;
        uint32_t dbg_atten : 4;
        uint32_t pd_cur    : 1;
        uint32_t bias_sleep: 1;
    };
    struct {
        uint32_t reserved1: 21;
        uint32_t slp_xpd  : 1;
        uint32_t xpd	  : 1;
        uint32_t slp_dbias: 4;
        uint32_t dbias    : 5;
    };
    struct {
        uint32_t reserved2: 28;
        uint32_t drv_b    : 4;
    };
} pmu_lp_analog_t;

typedef struct {
    uint32_t    modem_wakeup_wait_cycle;
    uint16_t    analog_wait_target_cycle;
    uint16_t    digital_power_down_wait_cycle;
    uint16_t    digital_power_supply_wait_cycle;
    uint16_t    digital_power_up_wait_cycle;
    uint16_t    pll_stable_wait_cycle;
    uint8_t     modify_icg_cntl_wait_cycle;
    uint8_t     switch_icg_cntl_wait_cycle;
    uint8_t     min_slp_slow_clk_cycle;
} pmu_hp_param_t;

typedef struct {
    uint16_t    digital_power_supply_wait_cycle;
    uint8_t     min_slp_slow_clk_cycle;
    uint8_t     analog_wait_target_cycle;
    uint8_t     digital_power_down_wait_cycle;
    uint8_t     digital_power_up_wait_cycle;
} pmu_lp_param_t;

typedef struct {
    union {
        uint16_t    xtal_stable_wait_slow_clk_cycle;
        uint16_t    xtal_stable_wait_cycle;
    };
} pmu_hp_lp_param_t;

#ifdef __cplusplus
}
#endif
