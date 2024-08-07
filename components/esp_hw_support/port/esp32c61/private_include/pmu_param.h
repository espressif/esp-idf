/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <esp_types.h>
#include "soc/pmu_struct.h"
#include "hal/pmu_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

//TODO: [ESP32C61] IDF-9250

#define HP_CALI_DBIAS_DEFAULT   26
#define LP_CALI_DBIAS_DEFAULT   25

// FOR  XTAL FORCE PU IN SLEEP
#define PMU_PD_CUR_SLEEP_ON    0
#define PMU_BIASSLP_SLEEP_ON   0

// FOR BOTH LIGHTSLEEP & DEEPSLEEP
#define PMU_PD_CUR_SLEEP_DEFAULT        1
#define PMU_BIASSLP_SLEEP_DEFAULT       1
#define PMU_LP_XPD_SLEEP_DEFAULT        1
#define PMU_LP_SLP_XPD_SLEEP_DEFAULT    0
#define PMU_LP_SLP_DBIAS_SLEEP_DEFAULT  0

// FOR LIGHTSLEEP
#define PMU_HP_DRVB_LIGHTSLEEP      0
#define PMU_LP_DRVB_LIGHTSLEEP      0
#define PMU_HP_XPD_LIGHTSLEEP       1

#define PMU_DBG_ATTEN_LIGHTSLEEP_DEFAULT    0
#define PMU_HP_DBIAS_LIGHTSLEEP_0V6_DEFAULT 1
#define PMU_LP_DBIAS_LIGHTSLEEP_0V7_DEFAULT 12

// FOR LIGHTSLEEP: XTAL FORCE PU
#define PMU_DBG_ATTEN_ACTIVE_DEFAULT    0

// FOR DEEPSLEEP
#define PMU_DBG_HP_DEEPSLEEP    0
#define PMU_HP_XPD_DEEPSLEEP    0
#define PMU_LP_DRVB_DEEPSLEEP   0

#define PMU_DBG_ATTEN_DEEPSLEEP_DEFAULT     12
#define PMU_LP_DBIAS_DEEPSLEEP_0V7_DEFAULT  23

uint32_t get_act_hp_dbias(void);
uint32_t get_act_lp_dbias(void);

typedef struct {
    pmu_hp_dig_power_reg_t  dig_power;
    pmu_hp_clk_power_reg_t  clk_power;
    pmu_hp_xtal_reg_t       xtal;
} pmu_hp_system_power_param_t;

const pmu_hp_system_power_param_t* pmu_hp_system_power_param_default(pmu_hp_mode_t mode);

typedef struct {
    uint32_t                icg_func;
    uint32_t                icg_apb;
    pmu_hp_icg_modem_reg_t  icg_modem;
    pmu_hp_sysclk_reg_t     sysclk;
} pmu_hp_system_clock_param_t;

const pmu_hp_system_clock_param_t* pmu_hp_system_clock_param_default(pmu_hp_mode_t mode);

typedef struct {
    pmu_hp_sys_cntl_reg_t   syscntl;
} pmu_hp_system_digital_param_t;

const pmu_hp_system_digital_param_t* pmu_hp_system_digital_param_default(pmu_hp_mode_t mode);

typedef struct {
    pmu_hp_bias_reg_t       bias;
    pmu_hp_regulator0_reg_t regulator0;
    pmu_hp_regulator1_reg_t regulator1;
} pmu_hp_system_analog_param_t;

const pmu_hp_system_analog_param_t* pmu_hp_system_analog_param_default(pmu_hp_mode_t mode);

typedef struct {
    pmu_hp_backup_reg_t     retention;
    uint32_t                backup_clk;
} pmu_hp_system_retention_param_t;

const pmu_hp_system_retention_param_t* pmu_hp_system_retention_param_default(pmu_hp_mode_t mode);

typedef struct {
    pmu_lp_dig_power_reg_t  dig_power;
    pmu_lp_clk_power_reg_t  clk_power;
    pmu_lp_xtal_reg_t       xtal;
} pmu_lp_system_power_param_t;

const pmu_lp_system_power_param_t* pmu_lp_system_power_param_default(pmu_lp_mode_t mode);



typedef struct {
    pmu_lp_bias_reg_t       bias;
    pmu_lp_regulator0_reg_t regulator0;
    pmu_lp_regulator1_reg_t regulator1;
} pmu_lp_system_analog_param_t;

const pmu_lp_system_analog_param_t* pmu_lp_system_analog_param_default(pmu_lp_mode_t mode);



/* Following software configuration instance type from pmu_struct.h used for the PMU state machine in sleep flow*/
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
    uint8_t     isolate_wait_cycle;
    uint8_t     reset_wait_cycle;
} pmu_hp_param_t;

typedef struct {
    uint16_t    digital_power_supply_wait_cycle;
    uint8_t     min_slp_slow_clk_cycle;
    uint8_t     analog_wait_target_cycle;
    uint8_t     digital_power_down_wait_cycle;
    uint8_t     digital_power_up_wait_cycle;
    uint8_t     isolate_wait_cycle;
    uint8_t     reset_wait_cycle;
} pmu_lp_param_t;

typedef struct {
    union {
        uint16_t    xtal_stable_wait_slow_clk_cycle;
        uint16_t    xtal_stable_wait_cycle;
    };
} pmu_hp_lp_param_t;

#define PMU_HP_SLEEP_MIN_SLOW_CLK_CYCLES        (10)
#define PMU_LP_SLEEP_MIN_SLOW_CLK_CYCLES        (10)

#define PMU_HP_WAKEUP_DELAY_CYCLES              (0)
#define PMU_HP_XTAL_STABLE_WAIT_CYCLES          (3155)  /* Not used, Fast OSC as PMU work clock source is about 201 us, corresponding to PMU_LP_XTAL_STABLE_WAIT_SLOW_CLK_CYCLES */
#define PMU_HP_PLL_STABLE_WAIT_CYCLES           (2)
#define PMU_HP_ANALOG_WAIT_TARGET_CYCLES        (2419)  /* Fast OSC as PMU work clock source is about 154 us */
#define PMU_HP_DIGITAL_POWER_SUPPLY_WAIT_CYCLES (32)
#define PMU_HP_DIGITAL_POWER_UP_WAIT_CYCLES     (32)
#define PMU_HP_MODEM_WAKEUP_WAIT_CYCLES         (20700) /* Fast OSC as PMU work clock source is about 1318.6 us */

#define PMU_LP_WAKEUP_DELAY_CYCLES              (0)
#define PMU_LP_XTAL_STABLE_WAIT_SLOW_CLK_CYCLES (30)    /* Slow OSC as PMU slow clock source is about 201 us */
#define PMU_LP_ANALOG_WAIT_TARGET_CYCLES        (23)    /* Slow OSC as PMU slow clock source is about 154 us */
#define PMU_LP_DIGITAL_POWER_SUPPLY_WAIT_CYCLES (32)    /* Fast OSC as PMU work clock source is about 2 us */
#define PMU_LP_DIGITAL_POWER_UP_WAIT_CYCLES     (32)    /* Fast OSC as PMU work clock source is about 2 us */

#define PMU_LP_ANALOG_WAIT_TARGET_TIME_DSLP_US  (500)    /* Slow OSC as PMU slow clock source in deepsleep is about 500 us */

typedef struct {
    struct {
        pmu_hp_power_t  dig_power;
        pmu_hp_power_t  clk_power;
        pmu_hp_power_t  xtal;
    } hp_sys;
    struct {
        pmu_lp_power_t  dig_power;
        pmu_lp_power_t  clk_power;
        pmu_lp_power_t  xtal;
    } lp_sys[PMU_MODE_LP_MAX];
} pmu_sleep_power_config_t;

#define PMU_SLEEP_POWER_CONFIG_DEFAULT(pd_flags) {                          \
    .hp_sys = {                                                             \
        .dig_power = {                                                      \
            .vdd_spi_pd_en = ((pd_flags) & PMU_SLEEP_PD_VDDSDIO) ? 1 : 0,   \
            .wifi_pd_en    = ((pd_flags) & PMU_SLEEP_PD_MODEM)   ? 1 : 0,   \
            .cpu_pd_en     = ((pd_flags) & PMU_SLEEP_PD_CPU)     ? 1 : 0,   \
            .aon_pd_en     = ((pd_flags) & PMU_SLEEP_PD_HP_AON)  ? 1 : 0,   \
            .top_pd_en     = ((pd_flags) & PMU_SLEEP_PD_TOP)     ? 1 : 0,   \
            .mem_pd_en     = 0,                                             \
            .mem_dslp      = 0                                              \
        },                                                                  \
        .clk_power = {                                                      \
            .i2c_iso_en    = 1,                                             \
            .i2c_retention = 1,                                             \
            .xpd_bb_i2c    = 0,                                             \
            .xpd_bbpll_i2c = 0,                                             \
            .xpd_bbpll     = 0                                              \
        },                                                                  \
        .xtal = {                                                           \
            .xpd_xtal      = ((pd_flags) & PMU_SLEEP_PD_XTAL) ? 0 : 1,      \
        }                                                                   \
    },                                                                      \
    .lp_sys[PMU_MODE_LP_ACTIVE] = {                                         \
        .dig_power = {                                                      \
            .peri_pd_en    = 0,                                             \
            .mem_dslp      = 0                                              \
        },                                                                  \
        .clk_power = {                                                      \
            .xpd_xtal32k   = ((pd_flags) & PMU_SLEEP_PD_XTAL32K) ? 0 : 1,   \
            .xpd_rc32k     = ((pd_flags) & PMU_SLEEP_PD_RC32K) ? 0 : 1,     \
            .xpd_fosc      = 1                                              \
        }                                                                   \
    },                                                                      \
    .lp_sys[PMU_MODE_LP_SLEEP] = {                                          \
        .dig_power = {                                                      \
            .peri_pd_en    = ((pd_flags) & PMU_SLEEP_PD_LP_PERIPH) ? 1 : 0, \
            .mem_dslp      = 1                                              \
        },                                                                  \
        .clk_power = {                                                      \
            .xpd_xtal32k   = ((pd_flags) & PMU_SLEEP_PD_XTAL32K) ? 0 : 1,   \
            .xpd_rc32k     = ((pd_flags) & PMU_SLEEP_PD_RC32K) ? 0 : 1,     \
            .xpd_fosc      = ((pd_flags) & PMU_SLEEP_PD_RC_FAST) ? 0 : 1    \
        },                                                                  \
        .xtal = {                                                           \
            .xpd_xtal      = ((pd_flags) & PMU_SLEEP_PD_XTAL) ? 0 : 1,      \
        }                                                                   \
    }                                                                       \
}

typedef struct {
    pmu_hp_sys_cntl_reg_t   syscntl;
} pmu_sleep_digital_config_t;

#define PMU_SLEEP_DIGITAL_LSLP_CONFIG_DEFAULT(pd_flags) {               \
    .syscntl = {                                                        \
        .dig_pad_slp_sel = ((pd_flags) & PMU_SLEEP_PD_TOP) ? 0 : 1,     \
    }                                                                   \
}

typedef struct {
    struct {
        pmu_hp_analog_t analog;
    } hp_sys;
    struct {
        pmu_lp_analog_t analog;
    } lp_sys[PMU_MODE_LP_MAX];
} pmu_sleep_analog_config_t;

#define PMU_SLEEP_ANALOG_LSLP_CONFIG_DEFAULT(pd_flags) {            \
    .hp_sys = {                                                     \
        .analog = {                                                 \
            .drv_b           = PMU_HP_DRVB_LIGHTSLEEP,              \
            .pd_cur          = PMU_PD_CUR_SLEEP_DEFAULT,            \
            .bias_sleep      = PMU_BIASSLP_SLEEP_DEFAULT,           \
            .xpd             = PMU_HP_XPD_LIGHTSLEEP,               \
            .dbg_atten       = PMU_DBG_ATTEN_LIGHTSLEEP_DEFAULT,    \
            .dbias           = PMU_HP_DBIAS_LIGHTSLEEP_0V6_DEFAULT  \
        }                                                           \
    },                                                              \
    .lp_sys[PMU_MODE_LP_SLEEP] = {                                  \
        .analog = {                                                 \
            .drv_b         = PMU_LP_DRVB_DEEPSLEEP,                 \
            .pd_cur        = PMU_PD_CUR_SLEEP_DEFAULT,              \
            .bias_sleep    = PMU_BIASSLP_SLEEP_DEFAULT,             \
            .slp_xpd       = PMU_LP_SLP_XPD_SLEEP_DEFAULT,          \
            .slp_dbias     = PMU_LP_SLP_DBIAS_SLEEP_DEFAULT,        \
            .xpd           = PMU_LP_XPD_SLEEP_DEFAULT,              \
            .dbg_atten     = PMU_DBG_ATTEN_LIGHTSLEEP_DEFAULT,      \
            .dbias         = PMU_LP_DBIAS_LIGHTSLEEP_0V7_DEFAULT    \
        }                                                           \
    }                                                               \
}

#define PMU_SLEEP_ANALOG_DSLP_CONFIG_DEFAULT(pd_flags) {            \
    .hp_sys = {                                                     \
        .analog = {                                                 \
            .pd_cur        = PMU_PD_CUR_SLEEP_ON,                   \
            .bias_sleep    = PMU_BIASSLP_SLEEP_ON,                  \
            .xpd           = PMU_HP_XPD_DEEPSLEEP,                  \
            .dbg_atten     = PMU_DBG_HP_DEEPSLEEP                   \
        }                                                           \
    },                                                              \
    .lp_sys[PMU_MODE_LP_SLEEP] = {                                  \
        .analog = {                                                 \
            .drv_b         = PMU_LP_DRVB_DEEPSLEEP,                 \
            .pd_cur        = PMU_PD_CUR_SLEEP_DEFAULT,              \
            .bias_sleep    = PMU_BIASSLP_SLEEP_DEFAULT,             \
            .slp_xpd       = PMU_LP_SLP_XPD_SLEEP_DEFAULT,          \
            .slp_dbias     = PMU_LP_SLP_DBIAS_SLEEP_DEFAULT,        \
            .xpd           = PMU_LP_XPD_SLEEP_DEFAULT,              \
            .dbg_atten     = PMU_DBG_ATTEN_DEEPSLEEP_DEFAULT,       \
            .dbias         = PMU_LP_DBIAS_DEEPSLEEP_0V7_DEFAULT     \
        }                                                           \
    }                                                               \
}

typedef struct {
    pmu_hp_param_t      hp_sys;
    pmu_lp_param_t      lp_sys;
    pmu_hp_lp_param_t   hp_lp;
} pmu_sleep_param_config_t;

#define PMU_SLEEP_PARAM_CONFIG_DEFAULT(pd_flags) {                                  \
    .hp_sys = {                                                                     \
        .min_slp_slow_clk_cycle          = PMU_HP_SLEEP_MIN_SLOW_CLK_CYCLES,        \
        .analog_wait_target_cycle        = PMU_HP_ANALOG_WAIT_TARGET_CYCLES,        \
        .digital_power_supply_wait_cycle = PMU_HP_DIGITAL_POWER_SUPPLY_WAIT_CYCLES, \
        .digital_power_up_wait_cycle     = PMU_HP_DIGITAL_POWER_UP_WAIT_CYCLES,     \
        .modem_wakeup_wait_cycle         = PMU_HP_MODEM_WAKEUP_WAIT_CYCLES,         \
        .pll_stable_wait_cycle           = PMU_HP_PLL_STABLE_WAIT_CYCLES            \
    },                                                                              \
    .lp_sys = {                                                                     \
        .min_slp_slow_clk_cycle          = PMU_LP_SLEEP_MIN_SLOW_CLK_CYCLES,        \
        .analog_wait_target_cycle        = PMU_LP_ANALOG_WAIT_TARGET_CYCLES,        \
        .digital_power_supply_wait_cycle = PMU_LP_DIGITAL_POWER_SUPPLY_WAIT_CYCLES, \
        .digital_power_up_wait_cycle     = PMU_LP_DIGITAL_POWER_UP_WAIT_CYCLES      \
    },                                                                              \
    .hp_lp = {                                                                      \
        .xtal_stable_wait_slow_clk_cycle = PMU_LP_XTAL_STABLE_WAIT_SLOW_CLK_CYCLES  \
    }                                                                               \
}

typedef struct {
    pmu_sleep_power_config_t    power;
    pmu_sleep_digital_config_t  digital;
    pmu_sleep_analog_config_t   analog;
    pmu_sleep_param_config_t    param;
} pmu_sleep_config_t;

typedef struct pmu_sleep_machine_constant {
    struct {
        uint16_t    min_slp_time_us;            /* Minimum sleep protection time (unit: microsecond) */
        uint8_t     wakeup_wait_cycle;          /* Modem wakeup signal (WiFi MAC and BEACON wakeup) waits for the slow & fast clock domain synchronization and the wakeup signal triggers the PMU FSM switching wait cycle (unit: slow clock cycle) */
        uint8_t     reserved0;
        uint16_t    analog_wait_time_us;        /* LP LDO power up wait time (unit: microsecond) */
        uint16_t    xtal_wait_stable_time_us;   /* Main XTAL stabilization wait time (unit: microsecond) */
        uint8_t     clk_switch_cycle;           /* Clock switch to FOSC (unit: slow clock cycle) */
        uint8_t     clk_power_on_wait_cycle;    /* Clock power on wait cycle (unit: slow clock cycle) */
        uint8_t     isolate_wait_time_us;       /* Waiting for all isolate signals to be ready (unit: microsecond) */
        uint8_t     reset_wait_time_us;         /* Waiting for all reset signals to be ready (unit: microsecond) */
        uint16_t    power_supply_wait_time_us;  /* (unit: microsecond) */
        uint16_t    power_up_wait_time_us;      /* (unit: microsecond) */
    } lp;
    struct {
        uint16_t    min_slp_time_us;            /* Minimum sleep protection time (unit: microsecond) */
        uint16_t    clock_domain_sync_time_us;  /* The Slow OSC clock domain synchronizes time with the Fast OSC domain, at least 4 slow clock cycles (unit: microsecond) */
        uint16_t    system_dfs_up_work_time_us; /* System DFS up scaling work time (unit: microsecond) */
        uint16_t    analog_wait_time_us;        /* HP LDO power up wait time (unit: microsecond) */
        uint8_t     isolate_wait_time_us;       /* Waiting for all isolate signals to be ready (unit: microsecond) */
        uint8_t     reset_wait_time_us;         /* Waiting for all reset signals to be ready (unit: microsecond) */
        uint16_t    power_supply_wait_time_us;  /* (unit: microsecond) */
        uint16_t    power_up_wait_time_us;      /* (unit: microsecond) */
        uint16_t    regdma_s2m_work_time_us;    /* Modem Subsystem (S2M switch) REGDMA restore time (unit: microsecond) */
        uint16_t    regdma_s2a_work_time_us;    /* SOC System (Digital Peripheral + Modem Subsystem) REGDMA (S2A switch) restore time (unit: microsecond) */
        uint16_t    regdma_m2a_work_time_us;    /* Digital Peripheral (M2A switch) REGDMA restore time (unit: microsecond) */
        uint16_t    regdma_a2s_work_time_us;    /* SOC System (Digital Peripheral + Modem Subsystem) REGDMA (A2S switch) backup time (unit: microsecond) */
        uint16_t    regdma_rf_on_work_time_us;  /* The REGDMA work time of RF enable (unit: microsecond) */
        uint16_t    regdma_rf_off_work_time_us; /* The REGDMA work time of RF disable (unit: microsecond) */
        uint16_t    xtal_wait_stable_time_us;   /* Main XTAL stabilization wait time (unit: microsecond) */
        uint16_t    pll_wait_stable_time_us;    /* PLL stabilization wait time (unit: microsecond) */
    } hp;
} pmu_sleep_machine_constant_t;

#define PMU_SLEEP_MC_DEFAULT()      {           \
    .lp = {                                     \
        .min_slp_time_us                = 450,  \
        .wakeup_wait_cycle              = 4,    \
        .analog_wait_time_us            = 154,  \
        .xtal_wait_stable_time_us       = 250,  \
        .clk_switch_cycle               = 1,    \
        .clk_power_on_wait_cycle        = 1,    \
        .isolate_wait_time_us           = 1,    \
        .reset_wait_time_us             = 1,    \
        .power_supply_wait_time_us      = 2,    \
        .power_up_wait_time_us          = 2     \
    },                                          \
    .hp = {                                     \
        .min_slp_time_us                = 450,  \
        .clock_domain_sync_time_us      = 150,  \
        .system_dfs_up_work_time_us     = 124,  \
        .analog_wait_time_us            = 154,  \
        .isolate_wait_time_us           = 1,    \
        .reset_wait_time_us             = 1,    \
        .power_supply_wait_time_us      = 2,    \
        .power_up_wait_time_us          = 2,    \
        .regdma_s2m_work_time_us        = 172,  \
        .regdma_s2a_work_time_us        = 480,  \
        .regdma_m2a_work_time_us        = 278,  \
        .regdma_a2s_work_time_us        = 382,  \
        .regdma_rf_on_work_time_us      = 70,   \
        .regdma_rf_off_work_time_us     = 23,   \
        .xtal_wait_stable_time_us       = 250,  \
        .pll_wait_stable_time_us        = 1     \
    }                                           \
}

#ifdef __cplusplus
}
#endif
