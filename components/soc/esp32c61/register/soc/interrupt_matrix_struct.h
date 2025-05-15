/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of core0_wifi_mac_intr_map register
 *  WIFI_MAC_INTR mapping register
 */
typedef union {
    struct {
        /** core0_wifi_mac_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_wifi_mac_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_wifi_mac_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_wifi_mac_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_wifi_mac_intr_map_reg_t;

/** Type of core0_wifi_mac_nmi_map register
 *  WIFI_MAC_NMI mapping register
 */
typedef union {
    struct {
        /** core0_wifi_mac_nmi_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_wifi_mac_nmi_map:6;
        uint32_t reserved_6:2;
        /** core0_wifi_mac_nmi_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_wifi_mac_nmi_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_wifi_mac_nmi_map_reg_t;

/** Type of core0_wifi_pwr_intr_map register
 *  WIFI_PWR_INTR mapping register
 */
typedef union {
    struct {
        /** core0_wifi_pwr_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_wifi_pwr_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_wifi_pwr_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_wifi_pwr_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_wifi_pwr_intr_map_reg_t;

/** Type of core0_wifi_bb_intr_map register
 *  WIFI_BB_INTR mapping register
 */
typedef union {
    struct {
        /** core0_wifi_bb_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_wifi_bb_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_wifi_bb_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_wifi_bb_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_wifi_bb_intr_map_reg_t;

/** Type of core0_bt_mac_intr_map register
 *  BT_MAC_INTR mapping register
 */
typedef union {
    struct {
        /** core0_bt_mac_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_bt_mac_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_bt_mac_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_bt_mac_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_bt_mac_intr_map_reg_t;

/** Type of core0_bt_bb_intr_map register
 *  BT_BB_INTR mapping register
 */
typedef union {
    struct {
        /** core0_bt_bb_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_bt_bb_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_bt_bb_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_bt_bb_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_bt_bb_intr_map_reg_t;

/** Type of core0_bt_bb_nmi_map register
 *  BT_BB_NMI mapping register
 */
typedef union {
    struct {
        /** core0_bt_bb_nmi_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_bt_bb_nmi_map:6;
        uint32_t reserved_6:2;
        /** core0_bt_bb_nmi_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_bt_bb_nmi_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_bt_bb_nmi_map_reg_t;

/** Type of core0_lp_timer_intr_map register
 *  LP_TIMER_INTR mapping register
 */
typedef union {
    struct {
        /** core0_lp_timer_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_lp_timer_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_lp_timer_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_lp_timer_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_lp_timer_intr_map_reg_t;

/** Type of core0_coex_intr_map register
 *  COEX_INTR mapping register
 */
typedef union {
    struct {
        /** core0_coex_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_coex_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_coex_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_coex_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_coex_intr_map_reg_t;

/** Type of core0_ble_timer_intr_map register
 *  BLE_TIMER_INTR mapping register
 */
typedef union {
    struct {
        /** core0_ble_timer_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_ble_timer_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_ble_timer_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_ble_timer_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_ble_timer_intr_map_reg_t;

/** Type of core0_ble_sec_intr_map register
 *  BLE_SEC_INTR mapping register
 */
typedef union {
    struct {
        /** core0_ble_sec_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_ble_sec_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_ble_sec_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_ble_sec_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_ble_sec_intr_map_reg_t;

/** Type of core0_i2c_mst_intr_map register
 *  I2C_MST_INTR mapping register
 */
typedef union {
    struct {
        /** core0_i2c_mst_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_i2c_mst_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_i2c_mst_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_i2c_mst_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_i2c_mst_intr_map_reg_t;

/** Type of core0_zb_mac_intr_map register
 *  ZB_MAC_INTR mapping register
 */
typedef union {
    struct {
        /** core0_zb_mac_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_zb_mac_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_zb_mac_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_zb_mac_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_zb_mac_intr_map_reg_t;

/** Type of core0_pmu_intr_map register
 *  PMU_INTR mapping register
 */
typedef union {
    struct {
        /** core0_pmu_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_pmu_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_pmu_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_pmu_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_pmu_intr_map_reg_t;

/** Type of core0_efuse_intr_map register
 *  EFUSE_INTR mapping register
 */
typedef union {
    struct {
        /** core0_efuse_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_efuse_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_efuse_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_efuse_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_efuse_intr_map_reg_t;

/** Type of core0_lp_rtc_timer_intr_map register
 *  LP_RTC_TIMER_INTR mapping register
 */
typedef union {
    struct {
        /** core0_lp_rtc_timer_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_lp_rtc_timer_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_lp_rtc_timer_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_lp_rtc_timer_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_lp_rtc_timer_intr_map_reg_t;

/** Type of core0_lp_wdt_intr_map register
 *  LP_WDT_INTR mapping register
 */
typedef union {
    struct {
        /** core0_lp_wdt_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_lp_wdt_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_lp_wdt_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_lp_wdt_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_lp_wdt_intr_map_reg_t;

/** Type of core0_lp_peri_timeout_intr_map register
 *  LP_PERI_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** core0_lp_peri_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_lp_peri_timeout_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_lp_peri_timeout_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_lp_peri_timeout_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_lp_peri_timeout_intr_map_reg_t;

/** Type of core0_lp_apm_m0_intr_map register
 *  LP_APM_M0_INTR mapping register
 */
typedef union {
    struct {
        /** core0_lp_apm_m0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_lp_apm_m0_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_lp_apm_m0_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_lp_apm_m0_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_lp_apm_m0_intr_map_reg_t;

/** Type of core0_cpu_intr_from_cpu_0_map register
 *  CPU_INTR_FROM_CPU_0 mapping register
 */
typedef union {
    struct {
        /** core0_cpu_intr_from_cpu_0_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_cpu_intr_from_cpu_0_map:6;
        uint32_t reserved_6:2;
        /** core0_cpu_intr_from_cpu_0_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_cpu_intr_from_cpu_0_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_cpu_intr_from_cpu_0_map_reg_t;

/** Type of core0_cpu_intr_from_cpu_1_map register
 *  CPU_INTR_FROM_CPU_1 mapping register
 */
typedef union {
    struct {
        /** core0_cpu_intr_from_cpu_1_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_cpu_intr_from_cpu_1_map:6;
        uint32_t reserved_6:2;
        /** core0_cpu_intr_from_cpu_1_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_cpu_intr_from_cpu_1_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_cpu_intr_from_cpu_1_map_reg_t;

/** Type of core0_cpu_intr_from_cpu_2_map register
 *  CPU_INTR_FROM_CPU_2 mapping register
 */
typedef union {
    struct {
        /** core0_cpu_intr_from_cpu_2_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_cpu_intr_from_cpu_2_map:6;
        uint32_t reserved_6:2;
        /** core0_cpu_intr_from_cpu_2_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_cpu_intr_from_cpu_2_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_cpu_intr_from_cpu_2_map_reg_t;

/** Type of core0_cpu_intr_from_cpu_3_map register
 *  CPU_INTR_FROM_CPU_3 mapping register
 */
typedef union {
    struct {
        /** core0_cpu_intr_from_cpu_3_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_cpu_intr_from_cpu_3_map:6;
        uint32_t reserved_6:2;
        /** core0_cpu_intr_from_cpu_3_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_cpu_intr_from_cpu_3_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_cpu_intr_from_cpu_3_map_reg_t;

/** Type of core0_assist_debug_intr_map register
 *  ASSIST_DEBUG_INTR mapping register
 */
typedef union {
    struct {
        /** core0_assist_debug_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_assist_debug_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_assist_debug_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_assist_debug_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_assist_debug_intr_map_reg_t;

/** Type of core0_trace_intr_map register
 *  TRACE_INTR mapping register
 */
typedef union {
    struct {
        /** core0_trace_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_trace_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_trace_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_trace_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_trace_intr_map_reg_t;

/** Type of core0_cache_intr_map register
 *  CACHE_INTR mapping register
 */
typedef union {
    struct {
        /** core0_cache_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_cache_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_cache_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_cache_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_cache_intr_map_reg_t;

/** Type of core0_cpu_peri_timeout_intr_map register
 *  CPU_PERI_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** core0_cpu_peri_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_cpu_peri_timeout_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_cpu_peri_timeout_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_cpu_peri_timeout_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_cpu_peri_timeout_intr_map_reg_t;

/** Type of core0_gpio_interrupt_pro_map register
 *  GPIO_INTERRUPT_PRO mapping register
 */
typedef union {
    struct {
        /** core0_gpio_interrupt_pro_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_gpio_interrupt_pro_map:6;
        uint32_t reserved_6:2;
        /** core0_gpio_interrupt_pro_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_gpio_interrupt_pro_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_gpio_interrupt_pro_map_reg_t;

/** Type of core0_gpio_interrupt_ext_map register
 *  GPIO_INTERRUPT_EXT mapping register
 */
typedef union {
    struct {
        /** core0_gpio_interrupt_ext_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_gpio_interrupt_ext_map:6;
        uint32_t reserved_6:2;
        /** core0_gpio_interrupt_ext_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_gpio_interrupt_ext_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_gpio_interrupt_ext_map_reg_t;

/** Type of core0_pau_intr_map register
 *  PAU_INTR mapping register
 */
typedef union {
    struct {
        /** core0_pau_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_pau_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_pau_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_pau_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_pau_intr_map_reg_t;

/** Type of core0_hp_peri_timeout_intr_map register
 *  HP_PERI_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** core0_hp_peri_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_hp_peri_timeout_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_hp_peri_timeout_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_hp_peri_timeout_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_hp_peri_timeout_intr_map_reg_t;

/** Type of core0_modem_peri_timeout_intr_map register
 *  MODEM_PERI_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** core0_modem_peri_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_modem_peri_timeout_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_modem_peri_timeout_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_modem_peri_timeout_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_modem_peri_timeout_intr_map_reg_t;

/** Type of core0_hp_apm_m0_intr_map register
 *  HP_APM_M0_INTR mapping register
 */
typedef union {
    struct {
        /** core0_hp_apm_m0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_hp_apm_m0_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_hp_apm_m0_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_hp_apm_m0_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_hp_apm_m0_intr_map_reg_t;

/** Type of core0_hp_apm_m1_intr_map register
 *  HP_APM_M1_INTR mapping register
 */
typedef union {
    struct {
        /** core0_hp_apm_m1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_hp_apm_m1_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_hp_apm_m1_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_hp_apm_m1_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_hp_apm_m1_intr_map_reg_t;

/** Type of core0_hp_apm_m2_intr_map register
 *  HP_APM_M2_INTR mapping register
 */
typedef union {
    struct {
        /** core0_hp_apm_m2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_hp_apm_m2_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_hp_apm_m2_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_hp_apm_m2_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_hp_apm_m2_intr_map_reg_t;

/** Type of core0_hp_apm_m3_intr_map register
 *  HP_APM_M3_INTR mapping register
 */
typedef union {
    struct {
        /** core0_hp_apm_m3_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_hp_apm_m3_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_hp_apm_m3_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_hp_apm_m3_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_hp_apm_m3_intr_map_reg_t;

/** Type of core0_cpu_apm_m0_intr_map register
 *  CPU_APM_M0_INTR mapping register
 */
typedef union {
    struct {
        /** core0_cpu_apm_m0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_cpu_apm_m0_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_cpu_apm_m0_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_cpu_apm_m0_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_cpu_apm_m0_intr_map_reg_t;

/** Type of core0_cpu_apm_m1_intr_map register
 *  CPU_APM_M1_INTR mapping register
 */
typedef union {
    struct {
        /** core0_cpu_apm_m1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_cpu_apm_m1_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_cpu_apm_m1_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_cpu_apm_m1_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_cpu_apm_m1_intr_map_reg_t;

/** Type of core0_mspi_intr_map register
 *  MSPI_INTR mapping register
 */
typedef union {
    struct {
        /** core0_mspi_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_mspi_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_mspi_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_mspi_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_mspi_intr_map_reg_t;

/** Type of core0_i2s1_intr_map register
 *  I2S1_INTR mapping register
 */
typedef union {
    struct {
        /** core0_i2s1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_i2s1_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_i2s1_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_i2s1_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_i2s1_intr_map_reg_t;

/** Type of core0_uart0_intr_map register
 *  UART0_INTR mapping register
 */
typedef union {
    struct {
        /** core0_uart0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_uart0_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_uart0_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_uart0_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_uart0_intr_map_reg_t;

/** Type of core0_uart1_intr_map register
 *  UART1_INTR mapping register
 */
typedef union {
    struct {
        /** core0_uart1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_uart1_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_uart1_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_uart1_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_uart1_intr_map_reg_t;

/** Type of core0_uart2_intr_map register
 *  UART2_INTR mapping register
 */
typedef union {
    struct {
        /** core0_uart2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_uart2_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_uart2_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_uart2_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_uart2_intr_map_reg_t;

/** Type of core0_ledc_intr_map register
 *  LEDC_INTR mapping register
 */
typedef union {
    struct {
        /** core0_ledc_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_ledc_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_ledc_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_ledc_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_ledc_intr_map_reg_t;

/** Type of core0_usb_intr_map register
 *  USB_INTR mapping register
 */
typedef union {
    struct {
        /** core0_usb_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_usb_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_usb_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_usb_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_usb_intr_map_reg_t;

/** Type of core0_i2c_ext0_intr_map register
 *  I2C_EXT0_INTR mapping register
 */
typedef union {
    struct {
        /** core0_i2c_ext0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_i2c_ext0_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_i2c_ext0_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_i2c_ext0_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_i2c_ext0_intr_map_reg_t;

/** Type of core0_tg0_t0_intr_map register
 *  TG0_T0_INTR mapping register
 */
typedef union {
    struct {
        /** core0_tg0_t0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_tg0_t0_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_tg0_t0_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_tg0_t0_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_tg0_t0_intr_map_reg_t;

/** Type of core0_tg0_t1_intr_map register
 *  TG0_T1_INTR mapping register
 */
typedef union {
    struct {
        /** core0_tg0_t1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_tg0_t1_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_tg0_t1_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_tg0_t1_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_tg0_t1_intr_map_reg_t;

/** Type of core0_tg0_wdt_intr_map register
 *  TG0_WDT_INTR mapping register
 */
typedef union {
    struct {
        /** core0_tg0_wdt_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_tg0_wdt_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_tg0_wdt_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_tg0_wdt_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_tg0_wdt_intr_map_reg_t;

/** Type of core0_tg1_t0_intr_map register
 *  TG1_T0_INTR mapping register
 */
typedef union {
    struct {
        /** core0_tg1_t0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_tg1_t0_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_tg1_t0_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_tg1_t0_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_tg1_t0_intr_map_reg_t;

/** Type of core0_tg1_t1_intr_map register
 *  TG1_T1_INTR mapping register
 */
typedef union {
    struct {
        /** core0_tg1_t1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_tg1_t1_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_tg1_t1_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_tg1_t1_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_tg1_t1_intr_map_reg_t;

/** Type of core0_tg1_wdt_intr_map register
 *  TG1_WDT_INTR mapping register
 */
typedef union {
    struct {
        /** core0_tg1_wdt_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_tg1_wdt_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_tg1_wdt_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_tg1_wdt_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_tg1_wdt_intr_map_reg_t;

/** Type of core0_systimer_target0_intr_map register
 *  SYSTIMER_TARGET0_INTR mapping register
 */
typedef union {
    struct {
        /** core0_systimer_target0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_systimer_target0_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_systimer_target0_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_systimer_target0_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_systimer_target0_intr_map_reg_t;

/** Type of core0_systimer_target1_intr_map register
 *  SYSTIMER_TARGET1_INTR mapping register
 */
typedef union {
    struct {
        /** core0_systimer_target1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_systimer_target1_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_systimer_target1_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_systimer_target1_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_systimer_target1_intr_map_reg_t;

/** Type of core0_systimer_target2_intr_map register
 *  SYSTIMER_TARGET2_INTR mapping register
 */
typedef union {
    struct {
        /** core0_systimer_target2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_systimer_target2_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_systimer_target2_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_systimer_target2_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_systimer_target2_intr_map_reg_t;

/** Type of core0_apb_adc_intr_map register
 *  APB_ADC_INTR mapping register
 */
typedef union {
    struct {
        /** core0_apb_adc_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_apb_adc_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_apb_adc_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_apb_adc_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_apb_adc_intr_map_reg_t;

/** Type of core0_slc0_intr_map register
 *  SLC0_INTR mapping register
 */
typedef union {
    struct {
        /** core0_slc0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_slc0_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_slc0_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_slc0_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_slc0_intr_map_reg_t;

/** Type of core0_slc1_intr_map register
 *  SLC1_INTR mapping register
 */
typedef union {
    struct {
        /** core0_slc1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_slc1_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_slc1_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_slc1_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_slc1_intr_map_reg_t;

/** Type of core0_dma_in_ch0_intr_map register
 *  DMA_IN_CH0_INTR mapping register
 */
typedef union {
    struct {
        /** core0_dma_in_ch0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_dma_in_ch0_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_dma_in_ch0_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_dma_in_ch0_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_dma_in_ch0_intr_map_reg_t;

/** Type of core0_dma_in_ch1_intr_map register
 *  DMA_IN_CH1_INTR mapping register
 */
typedef union {
    struct {
        /** core0_dma_in_ch1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_dma_in_ch1_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_dma_in_ch1_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_dma_in_ch1_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_dma_in_ch1_intr_map_reg_t;

/** Type of core0_dma_out_ch0_intr_map register
 *  DMA_OUT_CH0_INTR mapping register
 */
typedef union {
    struct {
        /** core0_dma_out_ch0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_dma_out_ch0_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_dma_out_ch0_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_dma_out_ch0_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_dma_out_ch0_intr_map_reg_t;

/** Type of core0_dma_out_ch1_intr_map register
 *  DMA_OUT_CH1_INTR mapping register
 */
typedef union {
    struct {
        /** core0_dma_out_ch1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_dma_out_ch1_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_dma_out_ch1_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_dma_out_ch1_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_dma_out_ch1_intr_map_reg_t;

/** Type of core0_gpspi2_intr_map register
 *  GPSPI2_INTR mapping register
 */
typedef union {
    struct {
        /** core0_gpspi2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_gpspi2_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_gpspi2_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_gpspi2_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_gpspi2_intr_map_reg_t;

/** Type of core0_sha_intr_map register
 *  SHA_INTR mapping register
 */
typedef union {
    struct {
        /** core0_sha_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_sha_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_sha_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_sha_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_sha_intr_map_reg_t;

/** Type of core0_ecc_intr_map register
 *  ECC_INTR mapping register
 */
typedef union {
    struct {
        /** core0_ecc_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_ecc_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_ecc_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_ecc_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_ecc_intr_map_reg_t;

/** Type of core0_ecdsa_intr_map register
 *  ECDSA_INTR mapping register
 */
typedef union {
    struct {
        /** core0_ecdsa_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t core0_ecdsa_intr_map:6;
        uint32_t reserved_6:2;
        /** core0_ecdsa_intr_pass_in_sec : R/W; bitpos: [8]; default: 0;
         *  Configures the PASS_IN_SEC flag of the interrupt source.
         */
        uint32_t core0_ecdsa_intr_pass_in_sec:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} interrupt_core0_ecdsa_intr_map_reg_t;

/** Type of core0_sig_idx_assert_in_sec register
 *  reserved
 */
typedef union {
    struct {
        /** core0_int_sig_idx_assert_in_sec : R/W; bitpos: [5:0]; default: 0;
         *  reserved
         */
        uint32_t core0_int_sig_idx_assert_in_sec:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_core0_sig_idx_assert_in_sec_reg_t;

/** Type of core0_clock_gate register
 *  Interrupt clock gating configure register
 */
typedef union {
    struct {
        /** core0_reg_clk_en : R/W; bitpos: [0]; default: 0;
         *  Interrupt clock gating configure register
         */
        uint32_t core0_reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} interrupt_core0_clock_gate_reg_t;


/** Group: Status Registers */
/** Type of core0_int_status_reg_0 register
 *  Status register for interrupt sources 0 ~ 31
 */
typedef union {
    struct {
        /** core0_int_status_0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the status of the interrupt sources within interrupt-index-range 0 ~ 31.
         *  Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source triggered an interrupt
         *  1:No interrupt triggered
         */
        uint32_t core0_int_status_0:32;
    };
    uint32_t val;
} interrupt_core0_int_status_reg_0_reg_t;

/** Type of core0_int_status_reg_1 register
 *  Status register for interrupt sources 32 ~ 63
 */
typedef union {
    struct {
        /** core0_int_status_1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the status of the interrupt sources within interrupt-index-range 32 ~
         *  63. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source triggered an interrupt
         *  1:No interrupt triggered
         */
        uint32_t core0_int_status_1:32;
    };
    uint32_t val;
} interrupt_core0_int_status_reg_1_reg_t;

/** Type of core0_int_status_reg_2 register
 *  Status register for interrupt sources 64 ~ 65
 */
typedef union {
    struct {
        /** core0_int_status_2 : RO; bitpos: [1:0]; default: 0;
         *  Represents the status of the interrupt sources within interrupt-index-range 64 ~
         *  65. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source triggered an interrupt
         *  1:No interrupt triggered
         */
        uint32_t core0_int_status_2:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} interrupt_core0_int_status_reg_2_reg_t;

/** Type of core0_src_pass_in_sec_status_0 register
 *  PASS_IN_SEC status register for interrupt sources 0 ~ 31
 */
typedef union {
    struct {
        /** core0_int_src_pass_in_sec_status_0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the PASS_IN_SEC status of the interrupt sources within
         *  interrupt-index-range 0 ~ 31. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS_IN_SEC.
         *  1:The corresponding interrupt source is PASS_IN_SEC.
         */
        uint32_t core0_int_src_pass_in_sec_status_0:32;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_sec_status_0_reg_t;

/** Type of core0_src_pass_in_sec_status_1 register
 *  PASS_IN_SEC status register for interrupt sources 32 ~ 63
 */
typedef union {
    struct {
        /** core0_int_src_pass_in_sec_status_1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the PASS_IN_SEC status of the interrupt sources within
         *  interrupt-index-range 32 ~ 63. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS_IN_SEC.
         *  1:The corresponding interrupt source is PASS_IN_SEC.
         */
        uint32_t core0_int_src_pass_in_sec_status_1:32;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_sec_status_1_reg_t;

/** Type of core0_src_pass_in_sec_status_2 register
 *  PASS_IN_SEC status register for interrupt sources 64 ~ 65
 */
typedef union {
    struct {
        /** core0_int_src_pass_in_sec_status_2 : RO; bitpos: [1:0]; default: 0;
         *  Represents the PASS_IN_SEC status of the interrupt sources with
         *  interrupt-index-range 64 ~ 65. Each bit corresponds to one interrupt source
         *  0:The corresponding interrupt source is not PASS_IN_SEC.
         *  1:The corresponding interrupt source is PASS_IN_SEC.
         */
        uint32_t core0_int_src_pass_in_sec_status_2:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} interrupt_core0_src_pass_in_sec_status_2_reg_t;

/** Type of core0_secure_status register
 *  reserved
 */
typedef union {
    struct {
        /** core0_int_secure_status : RO; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t core0_int_secure_status:32;
    };
    uint32_t val;
} interrupt_core0_secure_status_reg_t;


/** Group: Version Register */
/** Type of core0_interrupt_date register
 *  Version control register
 */
typedef union {
    struct {
        /** core0_interrupt_date : R/W; bitpos: [27:0]; default: 38801441;
         *  Version control register
         */
        uint32_t core0_interrupt_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} interrupt_core0_interrupt_date_reg_t;


typedef struct {
    volatile interrupt_core0_wifi_mac_intr_map_reg_t core0_wifi_mac_intr_map;
    volatile interrupt_core0_wifi_mac_nmi_map_reg_t core0_wifi_mac_nmi_map;
    volatile interrupt_core0_wifi_pwr_intr_map_reg_t core0_wifi_pwr_intr_map;
    volatile interrupt_core0_wifi_bb_intr_map_reg_t core0_wifi_bb_intr_map;
    volatile interrupt_core0_bt_mac_intr_map_reg_t core0_bt_mac_intr_map;
    volatile interrupt_core0_bt_bb_intr_map_reg_t core0_bt_bb_intr_map;
    volatile interrupt_core0_bt_bb_nmi_map_reg_t core0_bt_bb_nmi_map;
    volatile interrupt_core0_lp_timer_intr_map_reg_t core0_lp_timer_intr_map;
    volatile interrupt_core0_coex_intr_map_reg_t core0_coex_intr_map;
    volatile interrupt_core0_ble_timer_intr_map_reg_t core0_ble_timer_intr_map;
    volatile interrupt_core0_ble_sec_intr_map_reg_t core0_ble_sec_intr_map;
    volatile interrupt_core0_i2c_mst_intr_map_reg_t core0_i2c_mst_intr_map;
    volatile interrupt_core0_zb_mac_intr_map_reg_t core0_zb_mac_intr_map;
    volatile interrupt_core0_pmu_intr_map_reg_t core0_pmu_intr_map;
    volatile interrupt_core0_efuse_intr_map_reg_t core0_efuse_intr_map;
    volatile interrupt_core0_lp_rtc_timer_intr_map_reg_t core0_lp_rtc_timer_intr_map;
    volatile interrupt_core0_lp_wdt_intr_map_reg_t core0_lp_wdt_intr_map;
    volatile interrupt_core0_lp_peri_timeout_intr_map_reg_t core0_lp_peri_timeout_intr_map;
    volatile interrupt_core0_lp_apm_m0_intr_map_reg_t core0_lp_apm_m0_intr_map;
    volatile interrupt_core0_cpu_intr_from_cpu_0_map_reg_t core0_cpu_intr_from_cpu_0_map;
    volatile interrupt_core0_cpu_intr_from_cpu_1_map_reg_t core0_cpu_intr_from_cpu_1_map;
    volatile interrupt_core0_cpu_intr_from_cpu_2_map_reg_t core0_cpu_intr_from_cpu_2_map;
    volatile interrupt_core0_cpu_intr_from_cpu_3_map_reg_t core0_cpu_intr_from_cpu_3_map;
    volatile interrupt_core0_assist_debug_intr_map_reg_t core0_assist_debug_intr_map;
    volatile interrupt_core0_trace_intr_map_reg_t core0_trace_intr_map;
    volatile interrupt_core0_cache_intr_map_reg_t core0_cache_intr_map;
    volatile interrupt_core0_cpu_peri_timeout_intr_map_reg_t core0_cpu_peri_timeout_intr_map;
    volatile interrupt_core0_gpio_interrupt_pro_map_reg_t core0_gpio_interrupt_pro_map;
    volatile interrupt_core0_gpio_interrupt_ext_map_reg_t core0_gpio_interrupt_ext_map;
    volatile interrupt_core0_pau_intr_map_reg_t core0_pau_intr_map;
    volatile interrupt_core0_hp_peri_timeout_intr_map_reg_t core0_hp_peri_timeout_intr_map;
    volatile interrupt_core0_modem_peri_timeout_intr_map_reg_t core0_modem_peri_timeout_intr_map;
    volatile interrupt_core0_hp_apm_m0_intr_map_reg_t core0_hp_apm_m0_intr_map;
    volatile interrupt_core0_hp_apm_m1_intr_map_reg_t core0_hp_apm_m1_intr_map;
    volatile interrupt_core0_hp_apm_m2_intr_map_reg_t core0_hp_apm_m2_intr_map;
    volatile interrupt_core0_hp_apm_m3_intr_map_reg_t core0_hp_apm_m3_intr_map;
    volatile interrupt_core0_cpu_apm_m0_intr_map_reg_t core0_cpu_apm_m0_intr_map;
    volatile interrupt_core0_cpu_apm_m1_intr_map_reg_t core0_cpu_apm_m1_intr_map;
    volatile interrupt_core0_mspi_intr_map_reg_t core0_mspi_intr_map;
    volatile interrupt_core0_i2s1_intr_map_reg_t core0_i2s1_intr_map;
    volatile interrupt_core0_uart0_intr_map_reg_t core0_uart0_intr_map;
    volatile interrupt_core0_uart1_intr_map_reg_t core0_uart1_intr_map;
    volatile interrupt_core0_uart2_intr_map_reg_t core0_uart2_intr_map;
    volatile interrupt_core0_ledc_intr_map_reg_t core0_ledc_intr_map;
    volatile interrupt_core0_usb_intr_map_reg_t core0_usb_intr_map;
    volatile interrupt_core0_i2c_ext0_intr_map_reg_t core0_i2c_ext0_intr_map;
    volatile interrupt_core0_tg0_t0_intr_map_reg_t core0_tg0_t0_intr_map;
    volatile interrupt_core0_tg0_t1_intr_map_reg_t core0_tg0_t1_intr_map;
    volatile interrupt_core0_tg0_wdt_intr_map_reg_t core0_tg0_wdt_intr_map;
    volatile interrupt_core0_tg1_t0_intr_map_reg_t core0_tg1_t0_intr_map;
    volatile interrupt_core0_tg1_t1_intr_map_reg_t core0_tg1_t1_intr_map;
    volatile interrupt_core0_tg1_wdt_intr_map_reg_t core0_tg1_wdt_intr_map;
    volatile interrupt_core0_systimer_target0_intr_map_reg_t core0_systimer_target0_intr_map;
    volatile interrupt_core0_systimer_target1_intr_map_reg_t core0_systimer_target1_intr_map;
    volatile interrupt_core0_systimer_target2_intr_map_reg_t core0_systimer_target2_intr_map;
    volatile interrupt_core0_apb_adc_intr_map_reg_t core0_apb_adc_intr_map;
    volatile interrupt_core0_slc0_intr_map_reg_t core0_slc0_intr_map;
    volatile interrupt_core0_slc1_intr_map_reg_t core0_slc1_intr_map;
    volatile interrupt_core0_dma_in_ch0_intr_map_reg_t core0_dma_in_ch0_intr_map;
    volatile interrupt_core0_dma_in_ch1_intr_map_reg_t core0_dma_in_ch1_intr_map;
    volatile interrupt_core0_dma_out_ch0_intr_map_reg_t core0_dma_out_ch0_intr_map;
    volatile interrupt_core0_dma_out_ch1_intr_map_reg_t core0_dma_out_ch1_intr_map;
    volatile interrupt_core0_gpspi2_intr_map_reg_t core0_gpspi2_intr_map;
    volatile interrupt_core0_sha_intr_map_reg_t core0_sha_intr_map;
    volatile interrupt_core0_ecc_intr_map_reg_t core0_ecc_intr_map;
    volatile interrupt_core0_ecdsa_intr_map_reg_t core0_ecdsa_intr_map;
    volatile interrupt_core0_int_status_reg_0_reg_t core0_int_status_reg_0;
    volatile interrupt_core0_int_status_reg_1_reg_t core0_int_status_reg_1;
    volatile interrupt_core0_int_status_reg_2_reg_t core0_int_status_reg_2;
    volatile interrupt_core0_src_pass_in_sec_status_0_reg_t core0_src_pass_in_sec_status_0;
    volatile interrupt_core0_src_pass_in_sec_status_1_reg_t core0_src_pass_in_sec_status_1;
    volatile interrupt_core0_src_pass_in_sec_status_2_reg_t core0_src_pass_in_sec_status_2;
    volatile interrupt_core0_sig_idx_assert_in_sec_reg_t core0_sig_idx_assert_in_sec;
    volatile interrupt_core0_secure_status_reg_t core0_secure_status;
    volatile interrupt_core0_clock_gate_reg_t core0_clock_gate;
    uint32_t reserved_12c[436];
    volatile interrupt_core0_interrupt_date_reg_t core0_interrupt_date;
} interrupt_matrix_dev_t;

extern interrupt_matrix_dev_t INTMTX;

#ifndef __cplusplus
_Static_assert(sizeof(interrupt_matrix_dev_t) == 0x800, "Invalid size of interrupt_matrix_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
