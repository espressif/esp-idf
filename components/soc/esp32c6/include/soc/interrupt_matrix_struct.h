/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
 *  register description
 */
typedef union {
    struct {
        /** wifi_mac_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t wifi_mac_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_wifi_mac_intr_map_reg_t;

/** Type of wifi_mac_nmi_map register
 *  register description
 */
typedef union {
    struct {
        /** wifi_mac_nmi_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t wifi_mac_nmi_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_wifi_mac_nmi_map_reg_t;

/** Type of wifi_pwr_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** wifi_pwr_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t wifi_pwr_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_wifi_pwr_intr_map_reg_t;

/** Type of wifi_bb_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** wifi_bb_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t wifi_bb_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_wifi_bb_intr_map_reg_t;

/** Type of bt_mac_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** bt_mac_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t bt_mac_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_bt_mac_intr_map_reg_t;

/** Type of bt_bb_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** bt_bb_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t bt_bb_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_bt_bb_intr_map_reg_t;

/** Type of bt_bb_nmi_map register
 *  register description
 */
typedef union {
    struct {
        /** bt_bb_nmi_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t bt_bb_nmi_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_bt_bb_nmi_map_reg_t;

/** Type of lp_timer_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** lp_timer_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t lp_timer_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_lp_timer_intr_map_reg_t;

/** Type of coex_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** coex_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t coex_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_coex_intr_map_reg_t;

/** Type of ble_timer_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** ble_timer_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t ble_timer_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_ble_timer_intr_map_reg_t;

/** Type of ble_sec_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** ble_sec_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t ble_sec_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_ble_sec_intr_map_reg_t;

/** Type of i2c_mst_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** i2c_mst_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t i2c_mst_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_i2c_mst_intr_map_reg_t;

/** Type of zb_mac_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** zb_mac_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t zb_mac_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_zb_mac_intr_map_reg_t;

/** Type of pmu_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** pmu_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t pmu_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_pmu_intr_map_reg_t;

/** Type of efuse_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** efuse_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t efuse_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_efuse_intr_map_reg_t;

/** Type of lp_rtc_timer_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** lp_rtc_timer_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t lp_rtc_timer_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_lp_rtc_timer_intr_map_reg_t;

/** Type of lp_uart_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** lp_uart_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t lp_uart_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_lp_uart_intr_map_reg_t;

/** Type of lp_i2c_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** lp_i2c_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t lp_i2c_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_lp_i2c_intr_map_reg_t;

/** Type of lp_wdt_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** lp_wdt_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t lp_wdt_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_lp_wdt_intr_map_reg_t;

/** Type of lp_peri_timeout_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** lp_peri_timeout_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t lp_peri_timeout_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_lp_peri_timeout_intr_map_reg_t;

/** Type of lp_apm_m0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** lp_apm_m0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t lp_apm_m0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_lp_apm_m0_intr_map_reg_t;

/** Type of lp_apm_m1_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** lp_apm_m1_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t lp_apm_m1_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_lp_apm_m1_intr_map_reg_t;

/** Type of cpu_intr_from_cpu_0_map register
 *  register description
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_0_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_intr_from_cpu_0_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_cpu_intr_from_cpu_0_map_reg_t;

/** Type of cpu_intr_from_cpu_1_map register
 *  register description
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_1_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_intr_from_cpu_1_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_cpu_intr_from_cpu_1_map_reg_t;

/** Type of cpu_intr_from_cpu_2_map register
 *  register description
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_2_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_intr_from_cpu_2_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_cpu_intr_from_cpu_2_map_reg_t;

/** Type of cpu_intr_from_cpu_3_map register
 *  register description
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_3_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_intr_from_cpu_3_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_cpu_intr_from_cpu_3_map_reg_t;

/** Type of assist_debug_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** assist_debug_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t assist_debug_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_assist_debug_intr_map_reg_t;

/** Type of trace_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** trace_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t trace_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_trace_intr_map_reg_t;

/** Type of cache_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** cache_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t cache_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_cache_intr_map_reg_t;

/** Type of cpu_peri_timeout_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** cpu_peri_timeout_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_peri_timeout_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_cpu_peri_timeout_intr_map_reg_t;

/** Type of gpio_interrupt_pro_map register
 *  register description
 */
typedef union {
    struct {
        /** gpio_interrupt_pro_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t gpio_interrupt_pro_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_gpio_interrupt_pro_map_reg_t;

/** Type of gpio_interrupt_pro_nmi_map register
 *  register description
 */
typedef union {
    struct {
        /** gpio_interrupt_pro_nmi_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t gpio_interrupt_pro_nmi_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_gpio_interrupt_pro_nmi_map_reg_t;

/** Type of pau_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** pau_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t pau_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_pau_intr_map_reg_t;

/** Type of hp_peri_timeout_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** hp_peri_timeout_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t hp_peri_timeout_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_hp_peri_timeout_intr_map_reg_t;

/** Type of modem_peri_timeout_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** modem_peri_timeout_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t modem_peri_timeout_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_modem_peri_timeout_intr_map_reg_t;

/** Type of hp_apm_m0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** hp_apm_m0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t hp_apm_m0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_hp_apm_m0_intr_map_reg_t;

/** Type of hp_apm_m1_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** hp_apm_m1_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t hp_apm_m1_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_hp_apm_m1_intr_map_reg_t;

/** Type of hp_apm_m2_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** hp_apm_m2_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t hp_apm_m2_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_hp_apm_m2_intr_map_reg_t;

/** Type of hp_apm_m3_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** hp_apm_m3_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t hp_apm_m3_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_hp_apm_m3_intr_map_reg_t;

/** Type of lp_apm0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** lp_apm0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t lp_apm0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_lp_apm0_intr_map_reg_t;

/** Type of mspi_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** mspi_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t mspi_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_mspi_intr_map_reg_t;

/** Type of i2s1_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** i2s1_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t i2s1_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_i2s1_intr_map_reg_t;

/** Type of uhci0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** uhci0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t uhci0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_uhci0_intr_map_reg_t;

/** Type of uart0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** uart0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t uart0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_uart0_intr_map_reg_t;

/** Type of uart1_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** uart1_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t uart1_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_uart1_intr_map_reg_t;

/** Type of ledc_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** ledc_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t ledc_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_ledc_intr_map_reg_t;

/** Type of can0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** can0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t can0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_can0_intr_map_reg_t;

/** Type of can1_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** can1_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t can1_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_can1_intr_map_reg_t;

/** Type of usb_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** usb_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t usb_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_usb_intr_map_reg_t;

/** Type of rmt_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** rmt_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t rmt_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_rmt_intr_map_reg_t;

/** Type of i2c_ext0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** i2c_ext0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t i2c_ext0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_i2c_ext0_intr_map_reg_t;

/** Type of tg0_t0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** tg0_t0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t tg0_t0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_tg0_t0_intr_map_reg_t;

/** Type of tg0_t1_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** tg0_t1_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t tg0_t1_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_tg0_t1_intr_map_reg_t;

/** Type of tg0_wdt_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** tg0_wdt_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t tg0_wdt_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_tg0_wdt_intr_map_reg_t;

/** Type of tg1_t0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** tg1_t0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t tg1_t0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_tg1_t0_intr_map_reg_t;

/** Type of tg1_t1_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** tg1_t1_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t tg1_t1_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_tg1_t1_intr_map_reg_t;

/** Type of tg1_wdt_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** tg1_wdt_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t tg1_wdt_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_tg1_wdt_intr_map_reg_t;

/** Type of systimer_target0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** systimer_target0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t systimer_target0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_systimer_target0_intr_map_reg_t;

/** Type of systimer_target1_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** systimer_target1_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t systimer_target1_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_systimer_target1_intr_map_reg_t;

/** Type of systimer_target2_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** systimer_target2_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t systimer_target2_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_systimer_target2_intr_map_reg_t;

/** Type of apb_adc_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** apb_adc_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t apb_adc_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_apb_adc_intr_map_reg_t;

/** Type of pwm_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** pwm_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t pwm_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_pwm_intr_map_reg_t;

/** Type of pcnt_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** pcnt_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t pcnt_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_pcnt_intr_map_reg_t;

/** Type of parl_io_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** parl_io_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t parl_io_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_parl_io_intr_map_reg_t;

/** Type of slc0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** slc0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t slc0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_slc0_intr_map_reg_t;

/** Type of slc1_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** slc1_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t slc1_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_slc1_intr_map_reg_t;

/** Type of dma_in_ch0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** dma_in_ch0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t dma_in_ch0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_dma_in_ch0_intr_map_reg_t;

/** Type of dma_in_ch1_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** dma_in_ch1_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t dma_in_ch1_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_dma_in_ch1_intr_map_reg_t;

/** Type of dma_in_ch2_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** dma_in_ch2_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t dma_in_ch2_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_dma_in_ch2_intr_map_reg_t;

/** Type of dma_out_ch0_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** dma_out_ch0_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t dma_out_ch0_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_dma_out_ch0_intr_map_reg_t;

/** Type of dma_out_ch1_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** dma_out_ch1_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t dma_out_ch1_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_dma_out_ch1_intr_map_reg_t;

/** Type of dma_out_ch2_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** dma_out_ch2_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t dma_out_ch2_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_dma_out_ch2_intr_map_reg_t;

/** Type of gpspi2_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** gpspi2_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t gpspi2_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_gpspi2_intr_map_reg_t;

/** Type of aes_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** aes_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t aes_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_aes_intr_map_reg_t;

/** Type of sha_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** sha_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t sha_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_sha_intr_map_reg_t;

/** Type of rsa_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** rsa_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t rsa_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_rsa_intr_map_reg_t;

/** Type of ecc_intr_map register
 *  register description
 */
typedef union {
    struct {
        /** ecc_intr_map : R/W; bitpos: [4:0]; default: 0;
         *  Need add description
         */
        uint32_t ecc_intr_map:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} interrupt_matrix_ecc_intr_map_reg_t;

/** Type of int_status_reg_0 register
 *  register description
 */
typedef union {
    struct {
        /** int_status_0 : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t int_status_0:32;
    };
    uint32_t val;
} interrupt_matrix_int_status_reg_0_reg_t;

/** Type of int_status_reg_1 register
 *  register description
 */
typedef union {
    struct {
        /** int_status_1 : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t int_status_1:32;
    };
    uint32_t val;
} interrupt_matrix_int_status_reg_1_reg_t;

/** Type of int_status_reg_2 register
 *  register description
 */
typedef union {
    struct {
        /** int_status_2 : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t int_status_2:32;
    };
    uint32_t val;
} interrupt_matrix_int_status_reg_2_reg_t;

/** Type of clock_gate register
 *  register description
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} interrupt_matrix_clock_gate_reg_t;

/** Type of interrupt_reg_date register
 *  register description
 */
typedef union {
    struct {
        /** interrupt_reg_date : R/W; bitpos: [27:0]; default: 35664144;
         *  Need add description
         */
        uint32_t interrupt_reg_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} interrupt_matrix_interrupt_reg_date_reg_t;


typedef struct interrupt_matrix_dev_t {
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
    volatile interrupt_matrix_lp_uart_intr_map_reg_t lp_uart_intr_map;
    volatile interrupt_matrix_lp_i2c_intr_map_reg_t lp_i2c_intr_map;
    volatile interrupt_matrix_lp_wdt_intr_map_reg_t lp_wdt_intr_map;
    volatile interrupt_matrix_lp_peri_timeout_intr_map_reg_t lp_peri_timeout_intr_map;
    volatile interrupt_matrix_lp_apm_m0_intr_map_reg_t lp_apm_m0_intr_map;
    volatile interrupt_matrix_lp_apm_m1_intr_map_reg_t lp_apm_m1_intr_map;
    volatile interrupt_matrix_cpu_intr_from_cpu_0_map_reg_t cpu_intr_from_cpu_0_map;
    volatile interrupt_matrix_cpu_intr_from_cpu_1_map_reg_t cpu_intr_from_cpu_1_map;
    volatile interrupt_matrix_cpu_intr_from_cpu_2_map_reg_t cpu_intr_from_cpu_2_map;
    volatile interrupt_matrix_cpu_intr_from_cpu_3_map_reg_t cpu_intr_from_cpu_3_map;
    volatile interrupt_matrix_assist_debug_intr_map_reg_t assist_debug_intr_map;
    volatile interrupt_matrix_trace_intr_map_reg_t trace_intr_map;
    volatile interrupt_matrix_cache_intr_map_reg_t cache_intr_map;
    volatile interrupt_matrix_cpu_peri_timeout_intr_map_reg_t cpu_peri_timeout_intr_map;
    volatile interrupt_matrix_gpio_interrupt_pro_map_reg_t gpio_interrupt_pro_map;
    volatile interrupt_matrix_gpio_interrupt_pro_nmi_map_reg_t gpio_interrupt_pro_nmi_map;
    volatile interrupt_matrix_pau_intr_map_reg_t pau_intr_map;
    volatile interrupt_matrix_hp_peri_timeout_intr_map_reg_t hp_peri_timeout_intr_map;
    volatile interrupt_matrix_modem_peri_timeout_intr_map_reg_t modem_peri_timeout_intr_map;
    volatile interrupt_matrix_hp_apm_m0_intr_map_reg_t hp_apm_m0_intr_map;
    volatile interrupt_matrix_hp_apm_m1_intr_map_reg_t hp_apm_m1_intr_map;
    volatile interrupt_matrix_hp_apm_m2_intr_map_reg_t hp_apm_m2_intr_map;
    volatile interrupt_matrix_hp_apm_m3_intr_map_reg_t hp_apm_m3_intr_map;
    volatile interrupt_matrix_lp_apm0_intr_map_reg_t lp_apm0_intr_map;
    volatile interrupt_matrix_mspi_intr_map_reg_t mspi_intr_map;
    volatile interrupt_matrix_i2s1_intr_map_reg_t i2s1_intr_map;
    volatile interrupt_matrix_uhci0_intr_map_reg_t uhci0_intr_map;
    volatile interrupt_matrix_uart0_intr_map_reg_t uart0_intr_map;
    volatile interrupt_matrix_uart1_intr_map_reg_t uart1_intr_map;
    volatile interrupt_matrix_ledc_intr_map_reg_t ledc_intr_map;
    volatile interrupt_matrix_can0_intr_map_reg_t can0_intr_map;
    volatile interrupt_matrix_can1_intr_map_reg_t can1_intr_map;
    volatile interrupt_matrix_usb_intr_map_reg_t usb_intr_map;
    volatile interrupt_matrix_rmt_intr_map_reg_t rmt_intr_map;
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
    volatile interrupt_matrix_pwm_intr_map_reg_t pwm_intr_map;
    volatile interrupt_matrix_pcnt_intr_map_reg_t pcnt_intr_map;
    volatile interrupt_matrix_parl_io_intr_map_reg_t parl_io_intr_map;
    volatile interrupt_matrix_slc0_intr_map_reg_t slc0_intr_map;
    volatile interrupt_matrix_slc1_intr_map_reg_t slc1_intr_map;
    volatile interrupt_matrix_dma_in_ch0_intr_map_reg_t dma_in_ch0_intr_map;
    volatile interrupt_matrix_dma_in_ch1_intr_map_reg_t dma_in_ch1_intr_map;
    volatile interrupt_matrix_dma_in_ch2_intr_map_reg_t dma_in_ch2_intr_map;
    volatile interrupt_matrix_dma_out_ch0_intr_map_reg_t dma_out_ch0_intr_map;
    volatile interrupt_matrix_dma_out_ch1_intr_map_reg_t dma_out_ch1_intr_map;
    volatile interrupt_matrix_dma_out_ch2_intr_map_reg_t dma_out_ch2_intr_map;
    volatile interrupt_matrix_gpspi2_intr_map_reg_t gpspi2_intr_map;
    volatile interrupt_matrix_aes_intr_map_reg_t aes_intr_map;
    volatile interrupt_matrix_sha_intr_map_reg_t sha_intr_map;
    volatile interrupt_matrix_rsa_intr_map_reg_t rsa_intr_map;
    volatile interrupt_matrix_ecc_intr_map_reg_t ecc_intr_map;
    volatile interrupt_matrix_int_status_reg_0_reg_t int_status_reg_0;
    volatile interrupt_matrix_int_status_reg_1_reg_t int_status_reg_1;
    volatile interrupt_matrix_int_status_reg_2_reg_t int_status_reg_2;
    volatile interrupt_matrix_clock_gate_reg_t clock_gate;
    uint32_t reserved_144[430];
    volatile interrupt_matrix_interrupt_reg_date_reg_t interrupt_reg_date;
} interrupt_matrix_dev_t;

extern interrupt_matrix_dev_t INTMTX;

#ifndef __cplusplus
_Static_assert(sizeof(interrupt_matrix_dev_t) == 0x800, "Invalid size of interrupt_matrix_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
