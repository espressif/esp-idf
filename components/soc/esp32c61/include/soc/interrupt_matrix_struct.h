/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of wifi_mac_intr_map register
 *  WIFI_MAC_INTR mapping register
 */
typedef union {
    struct {
        /** wifi_mac_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t wifi_mac_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_wifi_mac_intr_map_reg_t;

/** Type of wifi_mac_nmi_map register
 *  WIFI_MAC_NMI mapping register
 */
typedef union {
    struct {
        /** wifi_mac_nmi_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t wifi_mac_nmi_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_wifi_mac_nmi_map_reg_t;

/** Type of wifi_pwr_intr_map register
 *  WIFI_PWR_INTR mapping register
 */
typedef union {
    struct {
        /** wifi_pwr_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t wifi_pwr_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_wifi_pwr_intr_map_reg_t;

/** Type of wifi_bb_intr_map register
 *  WIFI_BB_INTR mapping register
 */
typedef union {
    struct {
        /** wifi_bb_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t wifi_bb_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_wifi_bb_intr_map_reg_t;

/** Type of bt_mac_intr_map register
 *  BT_MAC_INTR mapping register
 */
typedef union {
    struct {
        /** bt_mac_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t bt_mac_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_bt_mac_intr_map_reg_t;

/** Type of bt_bb_intr_map register
 *  BT_BB_INTR mapping register
 */
typedef union {
    struct {
        /** bt_bb_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t bt_bb_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_bt_bb_intr_map_reg_t;

/** Type of bt_bb_nmi_map register
 *  BT_BB_NMI mapping register
 */
typedef union {
    struct {
        /** bt_bb_nmi_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t bt_bb_nmi_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_bt_bb_nmi_map_reg_t;

/** Type of lp_timer_intr_map register
 *  LP_TIMER_INTR mapping register
 */
typedef union {
    struct {
        /** lp_timer_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_timer_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_lp_timer_intr_map_reg_t;

/** Type of coex_intr_map register
 *  COEX_INTR mapping register
 */
typedef union {
    struct {
        /** coex_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t coex_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_coex_intr_map_reg_t;

/** Type of ble_timer_intr_map register
 *  BLE_TIMER_INTR mapping register
 */
typedef union {
    struct {
        /** ble_timer_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ble_timer_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_ble_timer_intr_map_reg_t;

/** Type of ble_sec_intr_map register
 *  BLE_SEC_INTR mapping register
 */
typedef union {
    struct {
        /** ble_sec_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ble_sec_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_ble_sec_intr_map_reg_t;

/** Type of i2c_mst_intr_map register
 *  I2C_MST_INTR mapping register
 */
typedef union {
    struct {
        /** i2c_mst_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t i2c_mst_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_i2c_mst_intr_map_reg_t;

/** Type of zb_mac_intr_map register
 *  ZB_MAC_INTR mapping register
 */
typedef union {
    struct {
        /** zb_mac_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t zb_mac_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_zb_mac_intr_map_reg_t;

/** Type of pmu_intr_map register
 *  PMU_INTR mapping register
 */
typedef union {
    struct {
        /** pmu_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t pmu_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_pmu_intr_map_reg_t;

/** Type of efuse_intr_map register
 *  EFUSE_INTR mapping register
 */
typedef union {
    struct {
        /** efuse_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t efuse_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_efuse_intr_map_reg_t;

/** Type of lp_rtc_timer_intr_map register
 *  LP_RTC_TIMER_INTR mapping register
 */
typedef union {
    struct {
        /** lp_rtc_timer_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_rtc_timer_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_lp_rtc_timer_intr_map_reg_t;

/** Type of lp_wdt_intr_map register
 *  LP_WDT_INTR mapping register
 */
typedef union {
    struct {
        /** lp_wdt_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_wdt_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_lp_wdt_intr_map_reg_t;

/** Type of lp_peri_timeout_intr_map register
 *  LP_PERI_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** lp_peri_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_peri_timeout_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_lp_peri_timeout_intr_map_reg_t;

/** Type of lp_apm_m0_intr_map register
 *  LP_APM_M0_INTR mapping register
 */
typedef union {
    struct {
        /** lp_apm_m0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t lp_apm_m0_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_lp_apm_m0_intr_map_reg_t;

/** Type of cpu_intr_from_cpu_0_map register
 *  CPU_INTR_FROM_CPU_0 mapping register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_0_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_intr_from_cpu_0_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_cpu_intr_from_cpu_0_map_reg_t;

/** Type of cpu_intr_from_cpu_1_map register
 *  CPU_INTR_FROM_CPU_1 mapping register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_1_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_intr_from_cpu_1_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_cpu_intr_from_cpu_1_map_reg_t;

/** Type of cpu_intr_from_cpu_2_map register
 *  CPU_INTR_FROM_CPU_2 mapping register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_2_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_intr_from_cpu_2_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_cpu_intr_from_cpu_2_map_reg_t;

/** Type of cpu_intr_from_cpu_3_map register
 *  CPU_INTR_FROM_CPU_3 mapping register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_3_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_intr_from_cpu_3_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_cpu_intr_from_cpu_3_map_reg_t;

/** Type of assist_debug_intr_map register
 *  ASSIST_DEBUG_INTR mapping register
 */
typedef union {
    struct {
        /** assist_debug_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t assist_debug_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_assist_debug_intr_map_reg_t;

/** Type of trace_intr_map register
 *  TRACE_INTR mapping register
 */
typedef union {
    struct {
        /** trace_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t trace_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_trace_intr_map_reg_t;

/** Type of cache_intr_map register
 *  CACHE_INTR mapping register
 */
typedef union {
    struct {
        /** cache_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cache_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_cache_intr_map_reg_t;

/** Type of cpu_peri_timeout_intr_map register
 *  CPU_PERI_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** cpu_peri_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t cpu_peri_timeout_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_cpu_peri_timeout_intr_map_reg_t;

/** Type of gpio_interrupt_pro_map register
 *  GPIO_INTERRUPT_PRO mapping register
 */
typedef union {
    struct {
        /** gpio_interrupt_pro_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t gpio_interrupt_pro_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_gpio_interrupt_pro_map_reg_t;

/** Type of gpio_interrupt_ext_map register
 *  GPIO_INTERRUPT_EXT mapping register
 */
typedef union {
    struct {
        /** gpio_interrupt_ext_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t gpio_interrupt_ext_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_gpio_interrupt_ext_map_reg_t;

/** Type of pau_intr_map register
 *  PAU_INTR mapping register
 */
typedef union {
    struct {
        /** pau_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t pau_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_pau_intr_map_reg_t;

/** Type of hp_peri_timeout_intr_map register
 *  HP_PERI_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** hp_peri_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_peri_timeout_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_hp_peri_timeout_intr_map_reg_t;

/** Type of modem_peri_timeout_intr_map register
 *  MODEM_PERI_TIMEOUT_INTR mapping register
 */
typedef union {
    struct {
        /** modem_peri_timeout_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t modem_peri_timeout_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_modem_peri_timeout_intr_map_reg_t;

/** Type of hp_apm_m0_intr_map register
 *  HP_APM_M0_INTR mapping register
 */
typedef union {
    struct {
        /** hp_apm_m0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_apm_m0_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_hp_apm_m0_intr_map_reg_t;

/** Type of hp_apm_m1_intr_map register
 *  HP_APM_M1_INTR mapping register
 */
typedef union {
    struct {
        /** hp_apm_m1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_apm_m1_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_hp_apm_m1_intr_map_reg_t;

/** Type of hp_apm_m2_intr_map register
 *  HP_APM_M2_INTR mapping register
 */
typedef union {
    struct {
        /** hp_apm_m2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_apm_m2_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_hp_apm_m2_intr_map_reg_t;

/** Type of hp_apm_m3_intr_map register
 *  HP_APM_M3_INTR mapping register
 */
typedef union {
    struct {
        /** hp_apm_m3_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t hp_apm_m3_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_hp_apm_m3_intr_map_reg_t;

/** Type of mspi_intr_map register
 *  MSPI_INTR mapping register
 */
typedef union {
    struct {
        /** mspi_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t mspi_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_mspi_intr_map_reg_t;

/** Type of i2s1_intr_map register
 *  I2S1_INTR mapping register
 */
typedef union {
    struct {
        /** i2s1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t i2s1_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_i2s1_intr_map_reg_t;

/** Type of uart0_intr_map register
 *  UART0_INTR mapping register
 */
typedef union {
    struct {
        /** uart0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t uart0_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_uart0_intr_map_reg_t;

/** Type of uart1_intr_map register
 *  UART1_INTR mapping register
 */
typedef union {
    struct {
        /** uart1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t uart1_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_uart1_intr_map_reg_t;

/** Type of uart2_intr_map register
 *  UART2_INTR mapping register
 */
typedef union {
    struct {
        /** uart2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t uart2_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_uart2_intr_map_reg_t;

/** Type of ledc_intr_map register
 *  LEDC_INTR mapping register
 */
typedef union {
    struct {
        /** ledc_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ledc_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_ledc_intr_map_reg_t;

/** Type of usb_intr_map register
 *  USB_INTR mapping register
 */
typedef union {
    struct {
        /** usb_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t usb_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_usb_intr_map_reg_t;

/** Type of i2c_ext0_intr_map register
 *  I2C_EXT0_INTR mapping register
 */
typedef union {
    struct {
        /** i2c_ext0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t i2c_ext0_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_i2c_ext0_intr_map_reg_t;

/** Type of tg0_t0_intr_map register
 *  TG0_T0_INTR mapping register
 */
typedef union {
    struct {
        /** tg0_t0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t tg0_t0_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_tg0_t0_intr_map_reg_t;

/** Type of tg0_t1_intr_map register
 *  TG0_T1_INTR mapping register
 */
typedef union {
    struct {
        /** tg0_t1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t tg0_t1_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_tg0_t1_intr_map_reg_t;

/** Type of tg0_wdt_intr_map register
 *  TG0_WDT_INTR mapping register
 */
typedef union {
    struct {
        /** tg0_wdt_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t tg0_wdt_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_tg0_wdt_intr_map_reg_t;

/** Type of tg1_t0_intr_map register
 *  TG1_T0_INTR mapping register
 */
typedef union {
    struct {
        /** tg1_t0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t tg1_t0_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_tg1_t0_intr_map_reg_t;

/** Type of tg1_t1_intr_map register
 *  TG1_T1_INTR mapping register
 */
typedef union {
    struct {
        /** tg1_t1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t tg1_t1_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_tg1_t1_intr_map_reg_t;

/** Type of tg1_wdt_intr_map register
 *  TG1_WDT_INTR mapping register
 */
typedef union {
    struct {
        /** tg1_wdt_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t tg1_wdt_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_tg1_wdt_intr_map_reg_t;

/** Type of systimer_target0_intr_map register
 *  SYSTIMER_TARGET0_INTR mapping register
 */
typedef union {
    struct {
        /** systimer_target0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t systimer_target0_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_systimer_target0_intr_map_reg_t;

/** Type of systimer_target1_intr_map register
 *  SYSTIMER_TARGET1_INTR mapping register
 */
typedef union {
    struct {
        /** systimer_target1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t systimer_target1_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_systimer_target1_intr_map_reg_t;

/** Type of systimer_target2_intr_map register
 *  SYSTIMER_TARGET2_INTR mapping register
 */
typedef union {
    struct {
        /** systimer_target2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t systimer_target2_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_systimer_target2_intr_map_reg_t;

/** Type of apb_adc_intr_map register
 *  APB_ADC_INTR mapping register
 */
typedef union {
    struct {
        /** apb_adc_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t apb_adc_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_apb_adc_intr_map_reg_t;

/** Type of dma_in_ch0_intr_map register
 *  DMA_IN_CH0_INTR mapping register
 */
typedef union {
    struct {
        /** dma_in_ch0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t dma_in_ch0_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_dma_in_ch0_intr_map_reg_t;

/** Type of dma_in_ch1_intr_map register
 *  DMA_IN_CH1_INTR mapping register
 */
typedef union {
    struct {
        /** dma_in_ch1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t dma_in_ch1_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_dma_in_ch1_intr_map_reg_t;

/** Type of dma_out_ch0_intr_map register
 *  DMA_OUT_CH0_INTR mapping register
 */
typedef union {
    struct {
        /** dma_out_ch0_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t dma_out_ch0_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_dma_out_ch0_intr_map_reg_t;

/** Type of dma_out_ch1_intr_map register
 *  DMA_OUT_CH1_INTR mapping register
 */
typedef union {
    struct {
        /** dma_out_ch1_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t dma_out_ch1_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_dma_out_ch1_intr_map_reg_t;

/** Type of gpspi2_intr_map register
 *  GPSPI2_INTR mapping register
 */
typedef union {
    struct {
        /** gpspi2_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t gpspi2_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_gpspi2_intr_map_reg_t;

/** Type of sha_intr_map register
 *  SHA_INTR mapping register
 */
typedef union {
    struct {
        /** sha_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t sha_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_sha_intr_map_reg_t;

/** Type of ecc_intr_map register
 *  ECC_INTR mapping register
 */
typedef union {
    struct {
        /** ecc_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ecc_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_ecc_intr_map_reg_t;

/** Type of ecdsa_intr_map register
 *  ECDSA_INTR mapping register
 */
typedef union {
    struct {
        /** ecdsa_intr_map : R/W; bitpos: [5:0]; default: 0;
         *  Configures the interrupt source into one CPU interrupt.
         */
        uint32_t ecdsa_intr_map:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} interrupt_matrix_ecdsa_intr_map_reg_t;

/** Type of int_status_0 register
 *  Status register for interrupt sources 0 ~ 31
 */
typedef union {
    struct {
        /** int_status_0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the status of the interrupt sources numbered from .Each bit corresponds
         *  to one interrupt source
         *  0:The corresponding interrupt source triggered an interrupt
         *  1:No interrupt triggered
         */
        uint32_t int_status_0:32;
    };
    uint32_t val;
} interrupt_matrix_int_status_0_reg_t;

/** Type of int_status_1 register
 *  Status register for interrupt sources 32 ~ 63
 */
typedef union {
    struct {
        /** int_status_1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the status of the interrupt sources numbered from .Each bit corresponds
         *  to one interrupt source
         *  0:The corresponding interrupt source triggered an interrupt
         *  1:No interrupt triggered
         */
        uint32_t int_status_1:32;
    };
    uint32_t val;
} interrupt_matrix_int_status_1_reg_t;

/** Type of clock_gate register
 *  Interrupt clock gating configure register
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 0;
         *  Interrupt clock gating configure register
         */
        uint32_t reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} interrupt_matrix_clock_gate_reg_t;


/** Group: Version Register */
/** Type of interrupt_date register
 *  Version control register
 */
typedef union {
    struct {
        /** interrupt_date : R/W; bitpos: [27:0]; default: 36774400;
         *  Version control register
         */
        uint32_t interrupt_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} interrupt_matrix_interrupt_date_reg_t;


typedef struct {
    volatile interrupt_matrix_wifi_mac_intr_map_reg_t wifi_mac_intr_map;
    volatile interrupt_matrix_wifi_mac_nmi_map_reg_t wifi_mac_nmi_map;
    volatile interrupt_matrix_wifi_pwr_intr_map_reg_t wifi_pwr_intr_map;
    volatile interrupt_matrix_wifi_bb_intr_map_reg_t wifi_bb_intr_map;
    volatile interrupt_matrix_bt_mac_intr_map_reg_t bt_mac_intr_map;
    volatile interrupt_matrix_bt_bb_intr_map_reg_t bt_bb_intr_map;
    volatile interrupt_matrix_bt_bb_nmi_map_reg_t bt_bb_nmi_map;
    volatile interrupt_matrix_lp_timer_intr_map_reg_t lp_timer_intr_map;
    volatile interrupt_matrix_coex_intr_map_reg_t coex_intr_map;
    volatile interrupt_matrix_ble_timer_intr_map_reg_t ble_timer_intr_map;
    volatile interrupt_matrix_ble_sec_intr_map_reg_t ble_sec_intr_map;
    volatile interrupt_matrix_i2c_mst_intr_map_reg_t i2c_mst_intr_map;
    volatile interrupt_matrix_zb_mac_intr_map_reg_t zb_mac_intr_map;
    volatile interrupt_matrix_pmu_intr_map_reg_t pmu_intr_map;
    volatile interrupt_matrix_efuse_intr_map_reg_t efuse_intr_map;
    volatile interrupt_matrix_lp_rtc_timer_intr_map_reg_t lp_rtc_timer_intr_map;
    volatile interrupt_matrix_lp_wdt_intr_map_reg_t lp_wdt_intr_map;
    volatile interrupt_matrix_lp_peri_timeout_intr_map_reg_t lp_peri_timeout_intr_map;
    volatile interrupt_matrix_lp_apm_m0_intr_map_reg_t lp_apm_m0_intr_map;
    volatile interrupt_matrix_cpu_intr_from_cpu_0_map_reg_t cpu_intr_from_cpu_0_map;
    volatile interrupt_matrix_cpu_intr_from_cpu_1_map_reg_t cpu_intr_from_cpu_1_map;
    volatile interrupt_matrix_cpu_intr_from_cpu_2_map_reg_t cpu_intr_from_cpu_2_map;
    volatile interrupt_matrix_cpu_intr_from_cpu_3_map_reg_t cpu_intr_from_cpu_3_map;
    volatile interrupt_matrix_assist_debug_intr_map_reg_t assist_debug_intr_map;
    volatile interrupt_matrix_trace_intr_map_reg_t trace_intr_map;
    volatile interrupt_matrix_cache_intr_map_reg_t cache_intr_map;
    volatile interrupt_matrix_cpu_peri_timeout_intr_map_reg_t cpu_peri_timeout_intr_map;
    volatile interrupt_matrix_gpio_interrupt_pro_map_reg_t gpio_interrupt_pro_map;
    volatile interrupt_matrix_gpio_interrupt_ext_map_reg_t gpio_interrupt_ext_map;
    volatile interrupt_matrix_pau_intr_map_reg_t pau_intr_map;
    volatile interrupt_matrix_hp_peri_timeout_intr_map_reg_t hp_peri_timeout_intr_map;
    volatile interrupt_matrix_modem_peri_timeout_intr_map_reg_t modem_peri_timeout_intr_map;
    volatile interrupt_matrix_hp_apm_m0_intr_map_reg_t hp_apm_m0_intr_map;
    volatile interrupt_matrix_hp_apm_m1_intr_map_reg_t hp_apm_m1_intr_map;
    volatile interrupt_matrix_hp_apm_m2_intr_map_reg_t hp_apm_m2_intr_map;
    volatile interrupt_matrix_hp_apm_m3_intr_map_reg_t hp_apm_m3_intr_map;
    volatile interrupt_matrix_mspi_intr_map_reg_t mspi_intr_map;
    volatile interrupt_matrix_i2s1_intr_map_reg_t i2s1_intr_map;
    volatile interrupt_matrix_uart0_intr_map_reg_t uart0_intr_map;
    volatile interrupt_matrix_uart1_intr_map_reg_t uart1_intr_map;
    volatile interrupt_matrix_uart2_intr_map_reg_t uart2_intr_map;
    volatile interrupt_matrix_ledc_intr_map_reg_t ledc_intr_map;
    volatile interrupt_matrix_usb_intr_map_reg_t usb_intr_map;
    volatile interrupt_matrix_i2c_ext0_intr_map_reg_t i2c_ext0_intr_map;
    volatile interrupt_matrix_tg0_t0_intr_map_reg_t tg0_t0_intr_map;
    volatile interrupt_matrix_tg0_t1_intr_map_reg_t tg0_t1_intr_map;
    volatile interrupt_matrix_tg0_wdt_intr_map_reg_t tg0_wdt_intr_map;
    volatile interrupt_matrix_tg1_t0_intr_map_reg_t tg1_t0_intr_map;
    volatile interrupt_matrix_tg1_t1_intr_map_reg_t tg1_t1_intr_map;
    volatile interrupt_matrix_tg1_wdt_intr_map_reg_t tg1_wdt_intr_map;
    volatile interrupt_matrix_systimer_target0_intr_map_reg_t systimer_target0_intr_map;
    volatile interrupt_matrix_systimer_target1_intr_map_reg_t systimer_target1_intr_map;
    volatile interrupt_matrix_systimer_target2_intr_map_reg_t systimer_target2_intr_map;
    volatile interrupt_matrix_apb_adc_intr_map_reg_t apb_adc_intr_map;
    volatile interrupt_matrix_dma_in_ch0_intr_map_reg_t dma_in_ch0_intr_map;
    volatile interrupt_matrix_dma_in_ch1_intr_map_reg_t dma_in_ch1_intr_map;
    volatile interrupt_matrix_dma_out_ch0_intr_map_reg_t dma_out_ch0_intr_map;
    volatile interrupt_matrix_dma_out_ch1_intr_map_reg_t dma_out_ch1_intr_map;
    volatile interrupt_matrix_gpspi2_intr_map_reg_t gpspi2_intr_map;
    volatile interrupt_matrix_sha_intr_map_reg_t sha_intr_map;
    volatile interrupt_matrix_ecc_intr_map_reg_t ecc_intr_map;
    volatile interrupt_matrix_ecdsa_intr_map_reg_t ecdsa_intr_map;
    volatile interrupt_matrix_int_status_0_reg_t int_status_0;
    volatile interrupt_matrix_int_status_1_reg_t int_status_1;
    volatile interrupt_matrix_clock_gate_reg_t clock_gate;
    uint32_t reserved_104[446];
    volatile interrupt_matrix_interrupt_date_reg_t interrupt_date;
} interrupt_matrix_dev_t;

extern interrupt_matrix_dev_t INTMTX;

#ifndef __cplusplus
_Static_assert(sizeof(interrupt_matrix_dev_t) == 0x800, "Invalid size of interrupt_matrix_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
