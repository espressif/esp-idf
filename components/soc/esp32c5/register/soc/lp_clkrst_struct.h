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

/** Group: configure_register */
/** Type of lp_clk_conf register
 *  Configures the root clk of LP system
 */
typedef union {
    struct {
        /** slow_clk_sel : R/W; bitpos: [1:0]; default: 0;
         *  Configures the source of LP_SLOW_CLK.
         *  0: RC_SLOW_CLK
         *  1: XTAL32K_CLK
         *  2: RC32K_CLK
         *  3:OSC_SLOW_CLK
         */
        uint32_t slow_clk_sel:2;
        /** fast_clk_sel : R/W; bitpos: [3:2]; default: 1;
         *  configures the source of LP_FAST_CLK.
         *  0: RC_FAST_CLK
         *  1: XTAL_D2_CLK
         *  2: XTAL_CLK
         */
        uint32_t fast_clk_sel:2;
        /** lp_peri_div_num : R/W; bitpos: [11:4]; default: 0;
         *  reserved
         */
        uint32_t lp_peri_div_num:8;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_clkrst_lp_clk_conf_reg_t;

/** Type of lp_clk_po_en register
 *  Configures the clk gate to pad
 */
typedef union {
    struct {
        /** aon_slow_oen : R/W; bitpos: [0]; default: 1;
         *  Configures the clock gate to pad of the LP_DYN_SLOW_CLK.
         *  0: Disable the clk pass clock gate
         *  1: Enable the clk pass clock gate
         */
        uint32_t aon_slow_oen:1;
        /** aon_fast_oen : R/W; bitpos: [1]; default: 1;
         *  Configures the clock gate to pad of the LP_DYN_FAST_CLK.
         *  0: Disable the clk pass clock gate
         *  1: Enable the clk pass clock gate
         */
        uint32_t aon_fast_oen:1;
        /** sosc_oen : R/W; bitpos: [2]; default: 1;
         *  Configures the clock gate to pad of the OSC_SLOW_CLK.
         *  0: Disable the clk pass clock gate
         *  1: Enable the clk pass clock gate
         */
        uint32_t sosc_oen:1;
        /** fosc_oen : R/W; bitpos: [3]; default: 1;
         *  Configures the clock gate to pad of the RC_FAST_CLK.
         *  0: Disable the clk pass clock gate
         *  1: Enable the clk pass clock gate
         */
        uint32_t fosc_oen:1;
        /** osc32k_oen : R/W; bitpos: [4]; default: 1;
         *  Configures the clock gate to pad of the RC32K_CLK.
         *  0: Disable the clk pass clock gate
         *  1: Enable the clk pass clock gate
         */
        uint32_t osc32k_oen:1;
        /** xtal32k_oen : R/W; bitpos: [5]; default: 1;
         *  Configures the clock gate to pad of the XTAL32K_CLK.
         *  0: Disable the clk pass clock gate
         *  1: Enable the clk pass clock gate
         */
        uint32_t xtal32k_oen:1;
        /** core_efuse_oen : R/W; bitpos: [6]; default: 1;
         *  Configures the clock gate to pad of the EFUSE_CTRL clock.
         *  0: Disable the clk pass clock gate
         *  1: Enable the clk pass clock gate
         */
        uint32_t core_efuse_oen:1;
        /** slow_oen : R/W; bitpos: [7]; default: 1;
         *  Configures the clock gate to pad of the LP_SLOW_CLK.
         *  0: Disable the clk pass clock gate
         *  1: Enable the clk pass clock gate
         */
        uint32_t slow_oen:1;
        /** fast_oen : R/W; bitpos: [8]; default: 1;
         *  Configures the clock gate to pad of the LP_FAST_CLK.
         *  0: Disable the clk pass clock gate
         *  1: Enable the clk pass clock gate
         */
        uint32_t fast_oen:1;
        /** rng_oen : R/W; bitpos: [9]; default: 1;
         *  Configures the clock gate to pad of the RNG clk.
         *  0: Disable the clk pass clock gate
         *  1: Enable the clk pass clock gate
         */
        uint32_t rng_oen:1;
        /** lpbus_oen : R/W; bitpos: [10]; default: 1;
         *  Configures the clock gate to pad of the LP bus clk.
         *  0: Disable the clk pass clock gate
         *  1: Enable the clk pass clock gate
         */
        uint32_t lpbus_oen:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} lp_clkrst_lp_clk_po_en_reg_t;

/** Type of lp_clk_en register
 *  Configure LP root clk source gate
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** fast_ori_gate : R/W; bitpos: [31]; default: 0;
         *  Configures the clock gate to LP_FAST_CLK
         *  0: Invalid. The clock gate controlled by hardware fsm
         *  1: Force the clk pass clock gate
         */
        uint32_t fast_ori_gate:1;
    };
    uint32_t val;
} lp_clkrst_lp_clk_en_reg_t;

/** Type of lp_rst_en register
 *  Configures the peri of LP system software reset
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** huk_reset_en : R/W; bitpos: [27]; default: 0;
         *  Configures whether or not to reset  HUK
         *  0: Invalid.No effect
         *  1: Reset
         */
        uint32_t huk_reset_en:1;
        /** aon_efuse_core_reset_en : R/W; bitpos: [28]; default: 0;
         *  Configures whether or not to reset  EFUSE_CTRL always-on part
         *  0: Invalid.No effect
         *  1: Reset
         */
        uint32_t aon_efuse_core_reset_en:1;
        /** lp_timer_reset_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to reset  LP_TIMER
         *  0: Invalid.No effect
         *  1: Reset
         */
        uint32_t lp_timer_reset_en:1;
        /** wdt_reset_en : R/W; bitpos: [30]; default: 0;
         *  Configures whether or not to reset LP_WDT and super watch dog
         *  0: Invalid.No effect
         *  1: Reset
         */
        uint32_t wdt_reset_en:1;
        /** ana_peri_reset_en : R/W; bitpos: [31]; default: 0;
         *  Configures whether or not to reset analog peri, include brownout controller
         *  0: Invalid.No effect
         *  1: Reset
         */
        uint32_t ana_peri_reset_en:1;
    };
    uint32_t val;
} lp_clkrst_lp_rst_en_reg_t;

/** Type of reset_cause register
 *  Represents the reset cause
 */
typedef union {
    struct {
        /** reset_cause : RO; bitpos: [4:0]; default: 0;
         *  Represents the reset cause
         */
        uint32_t reset_cause:5;
        /** core0_reset_flag : RO; bitpos: [5]; default: 1;
         *  Represents the reset flag
         */
        uint32_t core0_reset_flag:1;
        uint32_t reserved_6:23;
        /** core0_reset_cause_clr : WT; bitpos: [29]; default: 0;
         *  0: no operation
         */
        uint32_t core0_reset_cause_clr:1;
        /** core0_reset_flag_set : WT; bitpos: [30]; default: 0;
         *  configure set reset flag
         */
        uint32_t core0_reset_flag_set:1;
        /** core0_reset_flag_clr : WT; bitpos: [31]; default: 0;
         *  configure clear reset flag
         *  0: no operation
         *  1: clear flag to 0
         */
        uint32_t core0_reset_flag_clr:1;
    };
    uint32_t val;
} lp_clkrst_reset_cause_reg_t;

/** Type of cpu_reset register
 *  Configures CPU reset
 */
typedef union {
    struct {
        uint32_t reserved_0:21;
        /** hpcore0_lockup_reset_en : R/W; bitpos: [21]; default: 1;
         *  configure the hpcore0 luckup reset enable
         *  0: disable
         *  1:enable
         */
        uint32_t hpcore0_lockup_reset_en:1;
        /** rtc_wdt_cpu_reset_length : R/W; bitpos: [24:22]; default: 1;
         *  configures the reset length of LP_WDT reset CPU
         *  Measurement unit: LP_DYN_FAST_CLK
         */
        uint32_t rtc_wdt_cpu_reset_length:3;
        /** rtc_wdt_cpu_reset_en : R/W; bitpos: [25]; default: 0;
         *  Configures whether or not LP_WDT can reset CPU
         *  0: LP_WDT could not reset CPU when LP_WDT timeout
         *  1: LP_WDT could reset CPU when LP_WDT timeout
         */
        uint32_t rtc_wdt_cpu_reset_en:1;
        /** cpu_stall_wait : R/W; bitpos: [30:26]; default: 1;
         *  configure the time between CPU stall and reset
         *  Measurement unit: LP_DYN_FAST_CLK
         */
        uint32_t cpu_stall_wait:5;
        /** cpu_stall_en : R/W; bitpos: [31]; default: 0;
         *  Configures whether or not CPU entry stall state before LP_WDT and software reset CPU
         *  0: CPU will not entry stall state before LP_WDT and software reset CPU
         *  1: CPU will entry stall state before LP_WDT and software reset CPU
         */
        uint32_t cpu_stall_en:1;
    };
    uint32_t val;
} lp_clkrst_cpu_reset_reg_t;

/** Type of fosc_cntl register
 *  Configures the RC_FAST_CLK frequency
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** fosc_dfreq : R/W; bitpos: [31:22]; default: 172;
         *  Configures the RC_FAST_CLK frequency,the clock frequency will increase with this
         *  field
         */
        uint32_t fosc_dfreq:10;
    };
    uint32_t val;
} lp_clkrst_fosc_cntl_reg_t;

/** Type of rc32k_cntl register
 *  Configures the RC32K_CLK frequency
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** rc32k_dfreq : R/W; bitpos: [31:22]; default: 172;
         *  Configures the RC32K_CLK frequency, the clock frequency will increase with this
         *  field
         */
        uint32_t rc32k_dfreq:10;
    };
    uint32_t val;
} lp_clkrst_rc32k_cntl_reg_t;

/** Type of clk_to_hp register
 *  Configures the clk gate of LP clk to HP system
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** icg_hp_xtal32k : R/W; bitpos: [28]; default: 1;
         *  Configures the clk gate of XTAL32K_CLK to HP system
         *  0: The clk could not pass to HP system
         *  1: The clk could pass to HP system
         */
        uint32_t icg_hp_xtal32k:1;
        /** icg_hp_sosc : R/W; bitpos: [29]; default: 1;
         *  Configures the clk gate of RC_SLOW_CLK to HP system
         *  0: The clk could not pass to HP system
         *  1: The clk could pass to HP system
         */
        uint32_t icg_hp_sosc:1;
        /** icg_hp_osc32k : R/W; bitpos: [30]; default: 1;
         *  Configures the clk gate of RC32K_CLK to HP system
         *  0: The clk could not pass to HP system
         *  1: The clk could pass to HP system
         */
        uint32_t icg_hp_osc32k:1;
        /** icg_hp_fosc : R/W; bitpos: [31]; default: 1;
         *  Configures the clk gate of RC_FAST_CLK to HP system
         *  0: The clk could not pass to HP system
         *  1: The clk could pass to HP system
         */
        uint32_t icg_hp_fosc:1;
    };
    uint32_t val;
} lp_clkrst_clk_to_hp_reg_t;

/** Type of lpmem_force register
 *  Configures the LP_MEM clk gate force parameter
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lpmem_clk_force_on : R/W; bitpos: [31]; default: 0;
         *  Configures whether ot not force open the clock gate of LP MEM
         *  0: Invalid. The clock gate controlled by hardware FSM
         *  1: Force open clock gate of LP MEM
         */
        uint32_t lpmem_clk_force_on:1;
    };
    uint32_t val;
} lp_clkrst_lpmem_force_reg_t;

/** Type of lpperi register
 *  Configures the LP peri clk
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** huk_clk_sel : R/W; bitpos: [29]; default: 1;
         *  Configures the source clk of HUK
         *  0: 0: RC_FAST_CLK
         *  1: XTAL_D2_CLK
         */
        uint32_t huk_clk_sel:1;
        /** lp_i2c_clk_sel : R/W; bitpos: [30]; default: 0;
         *  Configures the source clk of LP I2C.
         *  0: LP_FAST_CLK
         *  1: XTAL_D2_CLK
         */
        uint32_t lp_i2c_clk_sel:1;
        /** lp_uart_clk_sel : R/W; bitpos: [31]; default: 0;
         *  Configures the source clk of LP UART.
         *  0: LP_FAST_CLK
         *  1: XTAL_D2_CLK
         */
        uint32_t lp_uart_clk_sel:1;
    };
    uint32_t val;
} lp_clkrst_lpperi_reg_t;

/** Type of xtal32k register
 *  Configures the XTAL32K parameter
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** dres_xtal32k : R/W; bitpos: [24:22]; default: 3;
         *  Configures DRES
         */
        uint32_t dres_xtal32k:3;
        /** dgm_xtal32k : R/W; bitpos: [27:25]; default: 3;
         *  Configures DGM
         */
        uint32_t dgm_xtal32k:3;
        /** dbuf_xtal32k : R/W; bitpos: [28]; default: 0;
         *  Configures DBUF
         */
        uint32_t dbuf_xtal32k:1;
        /** dac_xtal32k : R/W; bitpos: [31:29]; default: 3;
         *  Configures DAC
         */
        uint32_t dac_xtal32k:3;
    };
    uint32_t val;
} lp_clkrst_xtal32k_reg_t;

/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** clkrst_date : R/W; bitpos: [30:0]; default: 36720768;
         *  Version control register
         */
        uint32_t clkrst_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  configure register clk bypass clk gate
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_clkrst_date_reg_t;


typedef struct {
    volatile lp_clkrst_lp_clk_conf_reg_t lp_clk_conf;
    volatile lp_clkrst_lp_clk_po_en_reg_t lp_clk_po_en;
    volatile lp_clkrst_lp_clk_en_reg_t lp_clk_en;
    volatile lp_clkrst_lp_rst_en_reg_t lp_rst_en;
    volatile lp_clkrst_reset_cause_reg_t reset_cause;
    volatile lp_clkrst_cpu_reset_reg_t cpu_reset;
    volatile lp_clkrst_fosc_cntl_reg_t fosc_cntl;
    volatile lp_clkrst_rc32k_cntl_reg_t rc32k_cntl;
    volatile lp_clkrst_clk_to_hp_reg_t clk_to_hp;
    volatile lp_clkrst_lpmem_force_reg_t lpmem_force;
    volatile lp_clkrst_lpperi_reg_t lpperi;
    volatile lp_clkrst_xtal32k_reg_t xtal32k;
    uint32_t reserved_030[243];
    volatile lp_clkrst_date_reg_t date;
} lp_clkrst_dev_t;

extern lp_clkrst_dev_t LP_CLKRST;

#ifndef __cplusplus
_Static_assert(sizeof(lp_clkrst_dev_t) == 0x400, "Invalid size of lp_clkrst_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
