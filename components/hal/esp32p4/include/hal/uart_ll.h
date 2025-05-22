/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for UART register operations.
// Note that most of the register operations in this layer are non-atomic operations.


#pragma once

#include "esp_attr.h"
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/uart_types.h"
#include "soc/uart_reg.h"
#include "soc/uart_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/lp_uart_reg.h"
#include "soc/lp_clkrst_struct.h"
#include "soc/lpperi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

// The default fifo depth
#define UART_LL_FIFO_DEF_LEN  (SOC_UART_FIFO_LEN)
#define LP_UART_LL_FIFO_DEF_LEN (SOC_LP_UART_FIFO_LEN)

// Get UART hardware instance with giving uart num
#define UART_LL_GET_HW(num) \
    ((num) == 0 ? (&UART0) : \
    (num) == 1 ? (&UART1) : \
    (num) == 2 ? (&UART2) : \
    (num) == 3 ? (&UART3) : \
    (num) == 4 ? (&UART4) : (&LP_UART))

#define UART_LL_REG_FIELD_BIT_SHIFT(hw)     (((hw) == &LP_UART) ? 3 : 0)

#define UART_LL_PULSE_TICK_CNT_MAX          UART_LOWPULSE_MIN_CNT_V

#define UART_LL_WAKEUP_EDGE_THRED_MIN       (3)
#define UART_LL_WAKEUP_EDGE_THRED_MAX(hw)   (((hw) == &LP_UART) ? LP_UART_ACTIVE_THRESHOLD_V : UART_ACTIVE_THRESHOLD_V )
#define UART_LL_WAKEUP_FIFO_THRED_MAX(hw)   (((hw) == &LP_UART) ? LP_UART_RX_WAKE_UP_THRHD_V : UART_RX_WAKE_UP_THRHD_V )

#define UART_LL_INTR_MASK         (0x7ffff) //All interrupt mask

#define UART_LL_FSM_IDLE                       (0x0)
#define UART_LL_FSM_TX_WAIT_SEND               (0xf)

// Define UART interrupts
typedef enum {
    UART_INTR_RXFIFO_FULL      = (0x1 << 0),
    UART_INTR_TXFIFO_EMPTY     = (0x1 << 1),
    UART_INTR_PARITY_ERR       = (0x1 << 2),
    UART_INTR_FRAM_ERR         = (0x1 << 3),
    UART_INTR_RXFIFO_OVF       = (0x1 << 4),
    UART_INTR_DSR_CHG          = (0x1 << 5),
    UART_INTR_CTS_CHG          = (0x1 << 6),
    UART_INTR_BRK_DET          = (0x1 << 7),
    UART_INTR_RXFIFO_TOUT      = (0x1 << 8),
    UART_INTR_SW_XON           = (0x1 << 9),
    UART_INTR_SW_XOFF          = (0x1 << 10),
    UART_INTR_GLITCH_DET       = (0x1 << 11),
    UART_INTR_TX_BRK_DONE      = (0x1 << 12),
    UART_INTR_TX_BRK_IDLE      = (0x1 << 13),
    UART_INTR_TX_DONE          = (0x1 << 14),
    UART_INTR_RS485_PARITY_ERR = (0x1 << 15),
    UART_INTR_RS485_FRM_ERR    = (0x1 << 16),
    UART_INTR_RS485_CLASH      = (0x1 << 17),
    UART_INTR_CMD_CHAR_DET     = (0x1 << 18),
    UART_INTR_WAKEUP           = (0x1 << 19),
} uart_intr_t;

/**
 * @brief Sync the update to UART core clock domain
 *
 * @param hw Beginning address of the peripheral registers.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_update(uart_dev_t *hw)
{
    hw->reg_update.reg_update = 1;
    while (hw->reg_update.reg_update);
}

/****************************************** LP_UART Specific ********************************************/
/**
 * @brief  Get the LP_UART source clock.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param source_clk Current LP_UART clock source, one in soc_periph_lp_uart_clk_src_t.
 */
FORCE_INLINE_ATTR void lp_uart_ll_get_sclk(uart_dev_t *hw, soc_module_clk_t *source_clk)
{
    (void)hw;
    switch (LPPERI.core_clk_sel.lp_uart_clk_sel) {
    default:
    case 0:
        *source_clk = (soc_module_clk_t)LP_UART_SCLK_LP_FAST;
        break;
    case 1:
        *source_clk = (soc_module_clk_t)LP_UART_SCLK_XTAL_D2;
        break;
    // case 2:
    //     *source_clk = (soc_module_clk_t)LP_UART_SCLK_LP_PLL;
    //     break;
    }
}

/**
 * @brief Set LP UART source clock
 *
 * @param  hw Address offset of the LP UART peripheral registers
 * @param  src_clk Source clock for the LP UART peripheral
 */
static inline void lp_uart_ll_set_source_clk(uart_dev_t *hw, soc_periph_lp_uart_clk_src_t src_clk)
{
    (void)hw;
    switch (src_clk) {
    case LP_UART_SCLK_LP_FAST:
        LPPERI.core_clk_sel.lp_uart_clk_sel = 0;
        break;
    case LP_UART_SCLK_XTAL_D2:
        LPPERI.core_clk_sel.lp_uart_clk_sel = 1;
        break;
    // case LP_UART_SCLK_LP_PLL: // TODO: LP_PLL clock requires extra support
    //     LPPERI.core_clk_sel.lp_uart_clk_sel = 2;
    //     break;
    default:
        // Invalid LP_UART clock source
        HAL_ASSERT(false);
    }
}

// LPPERI.core_clk_sel is a shared register, so this function must be used in an atomic way
#define lp_uart_ll_set_source_clk(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        lp_uart_ll_set_source_clk(__VA_ARGS__); \
    } while(0)

/**
 * @brief  Configure the lp uart baud-rate.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  baud The baud rate to be set.
 * @param  sclk_freq Frequency of the clock source of UART, in Hz.
 *
 * @return True if baud-rate set successfully; False if baud-rate requested cannot be achieved
 */
FORCE_INLINE_ATTR bool lp_uart_ll_set_baudrate(uart_dev_t *hw, uint32_t baud, uint32_t sclk_freq)
{
    if (baud == 0) {
        return false;
    }
    // No pre-divider for LP UART clock source on the target
    uint32_t clk_div = (sclk_freq << 4) / baud;
    // The baud rate configuration register is divided into an integer part and a fractional part.
    uint32_t clkdiv_int = clk_div >> 4;
    if (clkdiv_int > UART_CLKDIV_V) {
        return false; // unachievable baud-rate
    }
    uint32_t clkdiv_frag = clk_div & 0xf;
    hw->clkdiv_sync.clkdiv = clkdiv_int;
    hw->clkdiv_sync.clkdiv_frag = clkdiv_frag;
    uart_ll_update(hw);
    return true;
}

/**
 * @brief Enable bus clock for the LP UART module
 *
 * @param hw_id LP UART instance ID
 * @param enable True to enable, False to disable
 */
static inline void _lp_uart_ll_enable_bus_clock(int hw_id, bool enable)
{
    (void)hw_id;
    LPPERI.clk_en.ck_en_lp_uart = enable;
}

// LPPERI.clk_en is a shared register, so this function must be used in an atomic way
#define lp_uart_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _lp_uart_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief  Enable the UART clock.
 *
 * @param hw_id LP UART instance ID
 */
FORCE_INLINE_ATTR void lp_uart_ll_sclk_enable(int hw_id)
{
    (void)hw_id;
    LP_UART.clk_conf.tx_sclk_en = 1;
    LP_UART.clk_conf.rx_sclk_en = 1;
}

/**
 * @brief  Disable the UART clock.
 *
 * @param hw_id LP UART instance ID
 */
FORCE_INLINE_ATTR void lp_uart_ll_sclk_disable(int hw_id)
{
    (void)hw_id;
    LP_UART.clk_conf.tx_sclk_en = 0;
    LP_UART.clk_conf.rx_sclk_en = 0;
}

/**
 * @brief Reset LP UART module
 *
 * @param hw_id LP UART instance ID
 */
static inline void lp_uart_ll_reset_register(int hw_id)
{
    (void)hw_id;
    LPPERI.reset_en.rst_en_lp_uart = 1;
    LPPERI.reset_en.rst_en_lp_uart = 0;
}

// LPPERI.reset_en is a shared register, so this function must be used in an atomic way
#define lp_uart_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        lp_uart_ll_reset_register(__VA_ARGS__); \
    } while(0)

/*************************************** General LL functions ******************************************/

/**
 * @brief Check if UART is enabled or disabled.
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 *
 * @return true: enabled; false: disabled
 */
FORCE_INLINE_ATTR bool uart_ll_is_enabled(uint32_t uart_num)
{
    bool uart_rst_en = true;
    bool uart_apb_en = false;
    bool uart_sys_en = false;
    switch (uart_num) {
    case 0:
        uart_rst_en = HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart0_apb;
        uart_apb_en = HP_SYS_CLKRST.soc_clk_ctrl2.reg_uart0_apb_clk_en;
        uart_sys_en = HP_SYS_CLKRST.soc_clk_ctrl1.reg_uart0_sys_clk_en;
        break;
    case 1:
        uart_rst_en = HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart1_apb;
        uart_apb_en = HP_SYS_CLKRST.soc_clk_ctrl2.reg_uart1_apb_clk_en;
        uart_sys_en = HP_SYS_CLKRST.soc_clk_ctrl1.reg_uart1_sys_clk_en;
        break;
    case 2:
        uart_rst_en = HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart2_apb;
        uart_apb_en = HP_SYS_CLKRST.soc_clk_ctrl2.reg_uart2_apb_clk_en;
        uart_sys_en = HP_SYS_CLKRST.soc_clk_ctrl1.reg_uart2_sys_clk_en;
        break;
    case 3:
        uart_rst_en = HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart3_apb;
        uart_apb_en = HP_SYS_CLKRST.soc_clk_ctrl2.reg_uart3_apb_clk_en;
        uart_sys_en = HP_SYS_CLKRST.soc_clk_ctrl1.reg_uart3_sys_clk_en;
        break;
    case 4:
        uart_rst_en = HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart4_apb;
        uart_apb_en = HP_SYS_CLKRST.soc_clk_ctrl2.reg_uart4_apb_clk_en;
        uart_sys_en = HP_SYS_CLKRST.soc_clk_ctrl1.reg_uart4_sys_clk_en;
        break;
    case 5:
        uart_rst_en = LPPERI.reset_en.rst_en_lp_uart;
        uart_apb_en = LPPERI.clk_en.ck_en_lp_uart;
        uart_sys_en = true;
        break;
    default:
        // Unknown uart port number
        HAL_ASSERT(false);
        break;
    }
    return (!uart_rst_en && uart_apb_en && uart_sys_en);
}

/**
 * @brief Enable the bus clock for uart
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 * @param enable true to enable, false to disable
 */
static inline void _uart_ll_enable_bus_clock(uart_port_t uart_num, bool enable)
{
    switch (uart_num) {
    case 0:
        HP_SYS_CLKRST.soc_clk_ctrl2.reg_uart0_apb_clk_en = enable;
        HP_SYS_CLKRST.soc_clk_ctrl1.reg_uart0_sys_clk_en = enable;
        break;
    case 1:
        HP_SYS_CLKRST.soc_clk_ctrl2.reg_uart1_apb_clk_en = enable;
        HP_SYS_CLKRST.soc_clk_ctrl1.reg_uart1_sys_clk_en = enable;
        break;
    case 2:
        HP_SYS_CLKRST.soc_clk_ctrl2.reg_uart2_apb_clk_en = enable;
        HP_SYS_CLKRST.soc_clk_ctrl1.reg_uart2_sys_clk_en = enable;
        break;
    case 3:
        HP_SYS_CLKRST.soc_clk_ctrl2.reg_uart3_apb_clk_en = enable;
        HP_SYS_CLKRST.soc_clk_ctrl1.reg_uart3_sys_clk_en = enable;
        break;
    case 4:
        HP_SYS_CLKRST.soc_clk_ctrl2.reg_uart4_apb_clk_en = enable;
        HP_SYS_CLKRST.soc_clk_ctrl1.reg_uart4_sys_clk_en = enable;
        break;
    case 5:
        // LP_UART port having its own enable_bus_clock function: lp_uart_ll_enable_bus_clock
        break;;
    default:
        abort();
        break;
    }
}
// HP_SYS_CLKRST.soc_clk_ctrlx are shared registers, so this function must be used in an atomic way
#define uart_ll_enable_bus_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _uart_ll_enable_bus_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief Reset UART module
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 */
static inline void uart_ll_reset_register(uart_port_t uart_num)
{
    switch (uart_num) {
    case 0:
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart0_apb = 1;
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart0_apb = 0;
        break;
    case 1:
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart1_apb = 1;
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart1_apb = 0;
        break;
    case 2:
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart2_apb = 1;
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart2_apb = 0;
        break;
    case 3:
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart3_apb = 1;
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart3_apb = 0;
        break;
    case 4:
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart4_apb = 1;
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_uart4_apb = 0;
        break;
    case 5:
        // LP_UART port having its own enable_bus_clock function: lp_uart_ll_reset_register
        break;;
    default:
        abort();
        break;
    }
}
//  HP_SYS_CLKRST.hp_rst_en1 is a shared register, so this function must be used in an atomic way
#define uart_ll_reset_register(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        uart_ll_reset_register(__VA_ARGS__); \
    } while(0)

/**
 * @brief  Enable the UART clock.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return None.
 */

FORCE_INLINE_ATTR void _uart_ll_sclk_enable(uart_dev_t *hw)
{
    if ((hw) == &UART0) {
        HP_SYS_CLKRST.peri_clk_ctrl110.reg_uart0_clk_en = 1;
    } else if ((hw) == &UART1) {
        HP_SYS_CLKRST.peri_clk_ctrl111.reg_uart1_clk_en = 1;
    } else if ((hw) == &UART2) {
        HP_SYS_CLKRST.peri_clk_ctrl112.reg_uart2_clk_en = 1;
    } else if ((hw) == &UART3) {
        HP_SYS_CLKRST.peri_clk_ctrl113.reg_uart3_clk_en = 1;
    } else if ((hw) == &UART4) {
        HP_SYS_CLKRST.peri_clk_ctrl114.reg_uart4_clk_en = 1;
    } else {
        // Not going to implement LP_UART reset in this function, it will have its own LL function
        abort();
    }
    hw->clk_conf.tx_sclk_en = 1;
    hw->clk_conf.rx_sclk_en = 1;
}
// HP_SYS_CLKRST.peri_clk_ctrlxxx are shared registers, so this function must be used in an atomic way
#define uart_ll_sclk_enable(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _uart_ll_sclk_enable(__VA_ARGS__); \
    } while(0)

/**
 * @brief  Disable the UART clock.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void _uart_ll_sclk_disable(uart_dev_t *hw)
{
    if ((hw) == &UART0) {
        HP_SYS_CLKRST.peri_clk_ctrl110.reg_uart0_clk_en = 0;
    } else if ((hw) == &UART1) {
        HP_SYS_CLKRST.peri_clk_ctrl111.reg_uart1_clk_en = 0;
    } else if ((hw) == &UART2) {
        HP_SYS_CLKRST.peri_clk_ctrl112.reg_uart2_clk_en = 0;
    } else if ((hw) == &UART3) {
        HP_SYS_CLKRST.peri_clk_ctrl113.reg_uart3_clk_en = 0;
    } else if ((hw) == &UART4) {
        HP_SYS_CLKRST.peri_clk_ctrl114.reg_uart4_clk_en = 0;
    } else {
        // Not going to implement LP_UART reset in this function, it will have its own LL function
        abort();
    }
    hw->clk_conf.tx_sclk_en = 0;
    hw->clk_conf.rx_sclk_en = 0;
}
// HP_SYS_CLKRST.peri_clk_ctrlxxx are shared registers, so this function must be used in an atomic way
#define uart_ll_sclk_disable(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _uart_ll_sclk_disable(__VA_ARGS__); \
    } while(0)

/**
 * @brief  Set the UART source clock.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  source_clk The UART source clock. The source clock can be PLL_F80M clock, RTC clock or XTAL clock.
 *                    All clock sources can remain at their original frequencies during DFS.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_sclk(uart_dev_t *hw, soc_module_clk_t source_clk)
{
    uint32_t sel_value = 0;
    switch (source_clk) {
    case UART_SCLK_XTAL:
        sel_value = 0;
        break;
    case UART_SCLK_RTC:
        sel_value = 1;
        break;
    case UART_SCLK_PLL_F80M:
        sel_value = 2;
        break;
    default:
        // Invalid HP_UART clock source
        abort();
    }
    if ((hw) == &UART0) {
        HP_SYS_CLKRST.peri_clk_ctrl110.reg_uart0_clk_src_sel = sel_value;
    } else if ((hw) == &UART1) {
        HP_SYS_CLKRST.peri_clk_ctrl111.reg_uart1_clk_src_sel = sel_value;
    } else if ((hw) == &UART2) {
        HP_SYS_CLKRST.peri_clk_ctrl112.reg_uart2_clk_src_sel = sel_value;
    } else if ((hw) == &UART3) {
        HP_SYS_CLKRST.peri_clk_ctrl113.reg_uart3_clk_src_sel = sel_value;
    } else if ((hw) == &UART4) {
        HP_SYS_CLKRST.peri_clk_ctrl114.reg_uart4_clk_src_sel = sel_value;
    } else {
        // LP_UART port having its own enable_bus_clock function: lp_uart_ll_set_source_clk
        abort();
    }
}
//HP_SYS_CLKRST.peri_clk_ctrlxxx are shared registers, so this function must be used in an atomic way
#define uart_ll_set_sclk(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        uart_ll_set_sclk(__VA_ARGS__); \
    } while(0)

/**
 * @brief  Get the UART source clock type.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  source_clk The pointer to accept the UART source clock type.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_get_sclk(uart_dev_t *hw, soc_module_clk_t *source_clk)
{
    uint32_t sel_value = 0;
    if ((hw) == &UART0) {
        sel_value = HP_SYS_CLKRST.peri_clk_ctrl110.reg_uart0_clk_src_sel;
    } else if ((hw) == &UART1) {
        sel_value = HP_SYS_CLKRST.peri_clk_ctrl111.reg_uart1_clk_src_sel;
    } else if ((hw) == &UART2) {
        sel_value = HP_SYS_CLKRST.peri_clk_ctrl112.reg_uart2_clk_src_sel;
    } else if ((hw) == &UART3) {
        sel_value = HP_SYS_CLKRST.peri_clk_ctrl113.reg_uart3_clk_src_sel;
    } else if ((hw) == &UART4) {
        sel_value = HP_SYS_CLKRST.peri_clk_ctrl114.reg_uart4_clk_src_sel;
    }
    if ((hw) != &LP_UART) {
        switch (sel_value) {
        default:
        case 1:
            *source_clk = (soc_module_clk_t)UART_SCLK_RTC;
            break;
        case 0:
            *source_clk = (soc_module_clk_t)UART_SCLK_XTAL;
            break;
        case 2:
            *source_clk = (soc_module_clk_t)UART_SCLK_PLL_F80M;
            break;
        }
    } else {
        lp_uart_ll_get_sclk(hw, source_clk);
    }
}

/**
 * @brief  Configure the baud-rate.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  baud The baud rate to be set.
 * @param  sclk_freq Frequency of the clock source of UART, in Hz.
 *
 * @return True if baud-rate set successfully; False if baud-rate requested cannot be achieved
 */
FORCE_INLINE_ATTR bool uart_ll_set_baudrate(uart_dev_t *hw, uint32_t baud, uint32_t sclk_freq)
{
    if ((hw) == &LP_UART) {
        abort(); // need to call lp_uart_ll_set_baudrate()
    }

#define DIV_UP(a, b)    (((a) + (b) - 1) / (b))
    if (baud == 0) {
        return false;
    }
    const uint32_t max_div = UART_CLKDIV_V;   // UART divider integer part only has 12 bits
    uint32_t sclk_div = DIV_UP(sclk_freq, (uint64_t)max_div * baud);
#undef DIV_UP

    if (sclk_div == 0 || sclk_div > (HP_SYS_CLKRST_REG_UART0_SCLK_DIV_NUM_V + 1)) {
        return false; // unachievable baud-rate
    }

    uint32_t clk_div = ((sclk_freq) << 4) / (baud * sclk_div);
    // The baud rate configuration register is divided into an integer part and a fractional part.
    hw->clkdiv_sync.clkdiv = clk_div >> 4;
    hw->clkdiv_sync.clkdiv_frag = clk_div & 0xf;
    //needs force u32 write
    if ((hw) == &UART0) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl111, reg_uart0_sclk_div_num, sclk_div - 1);
    } else if ((hw) == &UART1) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl112, reg_uart1_sclk_div_num, sclk_div - 1);
    } else if ((hw) == &UART2) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl113, reg_uart2_sclk_div_num, sclk_div - 1);
    } else if ((hw) == &UART3) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl114, reg_uart3_sclk_div_num, sclk_div - 1);
    } else if ((hw) == &UART4) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl115, reg_uart4_sclk_div_num, sclk_div - 1);
    } else {
        abort();
    }
    uart_ll_update(hw);
    return true;
}
#if !BOOTLOADER_BUILD
//HP_SYS_CLKRST.peri_clk_ctrlxxx are shared registers, so this function must be used in an atomic way
#define uart_ll_set_baudrate(...) uart_ll_set_baudrate(__VA_ARGS__); (void)__DECLARE_RCC_ATOMIC_ENV
#endif

/**
 * @brief  Get the current baud-rate.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  sclk_freq Frequency of the clock source of UART, in Hz.
 *
 * @return The current baudrate
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_baudrate(uart_dev_t *hw, uint32_t sclk_freq)
{
    typeof(hw->clkdiv_sync) div_reg;
    div_reg.val = hw->clkdiv_sync.val;
    int sclk_div = 0;
    if ((hw) == &UART0) {
        sclk_div = HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl111, reg_uart0_sclk_div_num) + 1;
    } else if ((hw) == &UART1) {
        sclk_div = HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl112, reg_uart1_sclk_div_num) + 1;
    } else if ((hw) == &UART2) {
        sclk_div = HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl113, reg_uart2_sclk_div_num) + 1;
    } else if ((hw) == &UART3) {
        sclk_div = HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl114, reg_uart3_sclk_div_num) + 1;
    } else if ((hw) == &UART4) {
        sclk_div = HAL_FORCE_READ_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl115, reg_uart4_sclk_div_num) + 1;
    } else if ((hw) == &LP_UART) {
        sclk_div = 1; // no pre-divider for LP UART clock source on the target
    }
    return ((sclk_freq << 4)) / (((div_reg.clkdiv << 4) | div_reg.clkdiv_frag) * sclk_div);
}

/**
 * @brief  Enable the UART interrupt based on the given mask.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  mask The bitmap of the interrupts need to be enabled.
 *
 * @return None
 */
FORCE_INLINE_ATTR void uart_ll_ena_intr_mask(uart_dev_t *hw, uint32_t mask)
{
    hw->int_ena.val = hw->int_ena.val | mask;
}

/**
 * @brief  Disable the UART interrupt based on the given mask.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  mask The bitmap of the interrupts need to be disabled.
 *
 * @return None
 */
FORCE_INLINE_ATTR void uart_ll_disable_intr_mask(uart_dev_t *hw, uint32_t mask)
{
    hw->int_ena.val = hw->int_ena.val & (~mask);
}

/**
 * @brief  Get the UART raw interrupt status.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return The UART interrupt status.
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_intraw_mask(uart_dev_t *hw)
{
    return hw->int_raw.val;
}

/**
 * @brief  Get the UART interrupt status.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return The UART interrupt status.
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_intsts_mask(uart_dev_t *hw)
{
    return hw->int_st.val;
}

/**
 * @brief  Clear the UART interrupt status based on the given mask.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  mask The bitmap of the interrupts need to be cleared.
 *
 * @return None
 */
FORCE_INLINE_ATTR void uart_ll_clr_intsts_mask(uart_dev_t *hw, uint32_t mask)
{
    hw->int_clr.val = mask;
}

/**
 * @brief  Get status of enabled interrupt.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return interrupt enable value
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_intr_ena_status(uart_dev_t *hw)
{
    return hw->int_ena.val;
}

/**
 * @brief  Read the UART rxfifo.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  buf The data buffer. The buffer size should be large than 128 byts.
 * @param  rd_len The data length needs to be read.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_read_rxfifo(uart_dev_t *hw, uint8_t *buf, uint32_t rd_len)
{
    for (int i = 0; i < (int)rd_len; i++) {
        buf[i] = hw->fifo.val;
    }
}

/**
 * @brief  Write byte to the UART txfifo.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  buf The data buffer.
 * @param  wr_len The data length needs to be written.
 *
 * @return None
 */
FORCE_INLINE_ATTR void uart_ll_write_txfifo(uart_dev_t *hw, const uint8_t *buf, uint32_t wr_len)
{
    // Write to the FIFO should make sure only involve write operation, any read operation would cause data lost.
    // Non-32-bit access would lead to a read-modify-write operation to the register, which is undesired.
    // Therefore, use 32-bit access to avoid any potential problem.
    for (int i = 0; i < (int)wr_len; i++) {
        hw->fifo.val = (int)buf[i];
    }
}

/**
 * @brief  Reset the UART hw rxfifo.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return None
 */
FORCE_INLINE_ATTR void uart_ll_rxfifo_rst(uart_dev_t *hw)
{
    hw->conf0_sync.rxfifo_rst = 1;
    uart_ll_update(hw);
    hw->conf0_sync.rxfifo_rst = 0;
    uart_ll_update(hw);
}

/**
 * @brief  Reset the UART hw txfifo.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return None
 */
FORCE_INLINE_ATTR void uart_ll_txfifo_rst(uart_dev_t *hw)
{
    hw->conf0_sync.txfifo_rst = 1;
    uart_ll_update(hw);
    hw->conf0_sync.txfifo_rst = 0;
    uart_ll_update(hw);
}

/**
 * @brief  Get the length of readable data in UART rxfifo.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return The readable data length in rxfifo.
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_rxfifo_len(uart_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->status, rxfifo_cnt) >> UART_LL_REG_FIELD_BIT_SHIFT(hw);
}

/**
 * @brief  Get the writable data length of UART txfifo.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return The data length of txfifo can be written.
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_txfifo_len(uart_dev_t *hw)
{
    uint32_t total_fifo_len = ((hw) == &LP_UART) ? LP_UART_LL_FIFO_DEF_LEN : UART_LL_FIFO_DEF_LEN;
    uint32_t txfifo_len = HAL_FORCE_READ_U32_REG_FIELD(hw->status, txfifo_cnt) >> UART_LL_REG_FIELD_BIT_SHIFT(hw);
    return (total_fifo_len - txfifo_len);
}

/**
 * @brief  Configure the UART stop bit.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  stop_bit The stop bit number to be set.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_stop_bits(uart_dev_t *hw, uart_stop_bits_t stop_bit)
{
    hw->conf0_sync.stop_bit_num = stop_bit;
    uart_ll_update(hw);
}

/**
 * @brief  Get the configuration of the UART stop bit.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  stop_bit The pointer to accept the stop bit configuration
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_get_stop_bits(uart_dev_t *hw, uart_stop_bits_t *stop_bit)
{
    *stop_bit = (uart_stop_bits_t)hw->conf0_sync.stop_bit_num;
}

/**
 * @brief  Configure the UART parity check mode.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  parity_mode The parity check mode to be set.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_parity(uart_dev_t *hw, uart_parity_t parity_mode)
{
    if (parity_mode != UART_PARITY_DISABLE) {
        hw->conf0_sync.parity = parity_mode & 0x1;
    }
    hw->conf0_sync.parity_en = (parity_mode >> 1) & 0x1;
    uart_ll_update(hw);
}

/**
 * @brief  Get the UART parity check mode configuration.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  parity_mode The pointer to accept the parity check mode configuration.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_get_parity(uart_dev_t *hw, uart_parity_t *parity_mode)
{
    if (hw->conf0_sync.parity_en) {
        *parity_mode = (uart_parity_t)(0x2 | hw->conf0_sync.parity);
    } else {
        *parity_mode = UART_PARITY_DISABLE;
    }
}

/**
 * @brief  Set the UART rxfifo full threshold value. When the data in rxfifo is more than the threshold value,
 *         it will produce rxfifo_full_int_raw interrupt.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  full_thrhd The full threshold value of the rxfifo. `full_thrhd` should be less than `(LP_)UART_LL_FIFO_DEF_LEN`.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_rxfifo_full_thr(uart_dev_t *hw, uint16_t full_thrhd)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->conf1, rxfifo_full_thrhd, full_thrhd << UART_LL_REG_FIELD_BIT_SHIFT(hw));
}

/**
 * @brief  Set the txfifo empty threshold. when the data length in txfifo is less than threshold value,
 *         it will produce txfifo_empty_int_raw interrupt.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  empty_thrhd The empty threshold of txfifo.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_txfifo_empty_thr(uart_dev_t *hw, uint16_t empty_thrhd)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->conf1, txfifo_empty_thrhd, empty_thrhd << UART_LL_REG_FIELD_BIT_SHIFT(hw));
}

/**
 * @brief  Set the UART rx-idle threshold value. when receiver takes more time than rx_idle_thrhd to receive a byte data,
 *         it will produce frame end signal for uhci to stop receiving data.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  rx_idle_thr The rx-idle threshold to be set.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_rx_idle_thr(uart_dev_t *hw, uint32_t rx_idle_thr)
{
    hw->idle_conf_sync.rx_idle_thrhd = rx_idle_thr;
    uart_ll_update(hw);
}

/**
 * @brief  Configure the duration time between transfers.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  idle_num the duration time between transfers.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_tx_idle_num(uart_dev_t *hw, uint32_t idle_num)
{
    hw->idle_conf_sync.tx_idle_num = idle_num;
    uart_ll_update(hw);
}

/**
 * @brief  Configure the transmitter to send break chars.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  break_num The number of the break chars need to be send.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_tx_break(uart_dev_t *hw, uint32_t break_num)
{
    if (break_num > 0) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->txbrk_conf_sync, tx_brk_num, break_num);
        hw->conf0_sync.txd_brk = 1;
    } else {
        hw->conf0_sync.txd_brk = 0;
    }
    uart_ll_update(hw);
}

/**
 * @brief  Configure the UART hardware flow control.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  flow_ctrl The hw flow control configuration.
 * @param  rx_thrs The rx flow control signal will be active if the data length in rxfifo is more than this value.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_hw_flow_ctrl(uart_dev_t *hw, uart_hw_flowcontrol_t flow_ctrl, uint32_t rx_thrs)
{
    //only when UART_HW_FLOWCTRL_RTS is set , will the rx_thresh value be set.
    if (flow_ctrl & UART_HW_FLOWCTRL_RTS) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->hwfc_conf_sync, rx_flow_thrhd, rx_thrs << UART_LL_REG_FIELD_BIT_SHIFT(hw));
        hw->hwfc_conf_sync.rx_flow_en = 1;
    } else {
        hw->hwfc_conf_sync.rx_flow_en = 0;
    }
    if (flow_ctrl & UART_HW_FLOWCTRL_CTS) {
        hw->conf0_sync.tx_flow_en = 1;
    } else {
        hw->conf0_sync.tx_flow_en = 0;
    }
    uart_ll_update(hw);
}

/**
 * @brief  Configure the hardware flow control.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  flow_ctrl A pointer to accept the hw flow control configuration.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_get_hw_flow_ctrl(uart_dev_t *hw, uart_hw_flowcontrol_t *flow_ctrl)
{
    *flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    if (hw->hwfc_conf_sync.rx_flow_en) {
        *flow_ctrl = (uart_hw_flowcontrol_t)((unsigned int)(*flow_ctrl) | (unsigned int)UART_HW_FLOWCTRL_RTS);
    }
    if (hw->conf0_sync.tx_flow_en) {
        *flow_ctrl = (uart_hw_flowcontrol_t)((unsigned int)(*flow_ctrl) | (unsigned int)UART_HW_FLOWCTRL_CTS);
    }
}

/**
 * @brief  Configure the software flow control.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  flow_ctrl The UART software flow control settings.
 * @param  sw_flow_ctrl_en Set true to enable software flow control, otherwise set it false.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_sw_flow_ctrl(uart_dev_t *hw, uart_sw_flowctrl_t *flow_ctrl, bool sw_flow_ctrl_en)
{
    if (sw_flow_ctrl_en) {
        hw->swfc_conf0_sync.xonoff_del = 1;
        hw->swfc_conf0_sync.sw_flow_con_en = 1;
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->swfc_conf1, xon_threshold, (flow_ctrl->xon_thrd) << UART_LL_REG_FIELD_BIT_SHIFT(hw));
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->swfc_conf1, xoff_threshold, (flow_ctrl->xoff_thrd) << UART_LL_REG_FIELD_BIT_SHIFT(hw));
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->swfc_conf0_sync, xon_character, flow_ctrl->xon_char);
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->swfc_conf0_sync, xoff_character, flow_ctrl->xoff_char);
    } else {
        hw->swfc_conf0_sync.sw_flow_con_en = 0;
        hw->swfc_conf0_sync.xonoff_del = 0;
    }
    uart_ll_update(hw);
}

/**
 * @brief  Configure the AT cmd char. When the receiver receives a continuous AT cmd char, it will produce at_cmd_char_det interrupt.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  cmd_char The AT cmd char configuration.The configuration member is:
 *         - cmd_char The AT cmd character
 *         - char_num The number of received AT cmd char must be equal to or greater than this value
 *         - gap_tout The interval between each AT cmd char, when the duration is less than this value, it will not take this data as AT cmd char
 *         - pre_idle The idle time before the first AT cmd char, when the duration is less than this value, it will not take the previous data as the last AT cmd char
 *         - post_idle The idle time after the last AT cmd char, when the duration is less than this value, it will not take this data as the first AT cmd char
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_at_cmd_char(uart_dev_t *hw, uart_at_cmd_t *cmd_char)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->at_cmd_char_sync, at_cmd_char, cmd_char->cmd_char);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->at_cmd_char_sync, at_char_num, cmd_char->char_num);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->at_cmd_postcnt_sync, post_idle_num, cmd_char->post_idle);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->at_cmd_precnt_sync, pre_idle_num, cmd_char->pre_idle);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->at_cmd_gaptout_sync, rx_gap_tout, cmd_char->gap_tout);
    uart_ll_update(hw);
}

/**
 * @brief  Set the UART data bit mode.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  data_bit The data bit mode to be set.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_data_bit_num(uart_dev_t *hw, uart_word_length_t data_bit)
{
    hw->conf0_sync.bit_num = data_bit;
    uart_ll_update(hw);
}

/**
 * @brief  Set the rts active level.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  level The rts active level, 0 or 1.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_rts_active_level(uart_dev_t *hw, int level)
{
    hw->conf0_sync.sw_rts = level & 0x1;
    uart_ll_update(hw);
}

/**
 * @brief  Set the dtr active level.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  level The dtr active level, 0 or 1.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_dtr_active_level(uart_dev_t *hw, int level)
{
    hw->conf1.sw_dtr = level & 0x1;
}

/**
 * @brief  Set the UART wakeup threshold.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  wakeup_thrd The wakeup threshold value to be set. When the input rx edge changes more than this value,
 *         the UART will active from light sleeping mode.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_wakeup_edge_thrd(uart_dev_t *hw, uint32_t wakeup_thrd)
{
    // System would wakeup when the number of positive edges of RxD signal is larger than or equal to (UART_ACTIVE_THRESHOLD+3)
    hw->sleep_conf2.active_threshold = wakeup_thrd - UART_LL_WAKEUP_EDGE_THRED_MIN;
}


/**
 * @brief  Set the number of received data bytes for the RX FIFO threshold wake-up mode.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  wakeup_thrd The wakeup threshold value in bytes to be set.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_wakeup_fifo_thrd(uart_dev_t *hw, uint32_t wakeup_thrd)
{
    // System would wakeup when reach the number of the received data number threshold.
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sleep_conf2, rx_wake_up_thrhd, wakeup_thrd << UART_LL_REG_FIELD_BIT_SHIFT(hw));
}

/**
 * @brief  Set the UART wakeup mode.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  mode UART wakeup mode to be set.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_wakeup_mode(uart_dev_t *hw, uart_wakeup_mode_t mode)
{
    switch(mode){
        case UART_WK_MODE_ACTIVE_THRESH:
            hw->sleep_conf2.wk_mode_sel = 0;
            break;
        case UART_WK_MODE_FIFO_THRESH:
            hw->sleep_conf2.wk_mode_sel = 1;
            break;
        case UART_WK_MODE_START_BIT:
            hw->sleep_conf2.wk_mode_sel = 2;
            break;
        case UART_WK_MODE_CHAR_SEQ:
            hw->sleep_conf2.wk_mode_sel = 3;
            break;
        default:
            abort();
            break;
    }
}

/**
 * @brief  Set the UART specific character sequence wakeup mode mask.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  mask UART wakeup char seq mask to be set.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_wakeup_char_seq_mask(uart_dev_t *hw, uint32_t mask)
{
    hw->sleep_conf2.wk_char_mask = mask;
}

/**
 * @brief  Set the UART specific character sequence wakeup phrase size.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  char_num UART wakeup char seq phrase size to be set.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_wakeup_char_seq_char_num(uart_dev_t *hw, uint32_t char_num)
{
    hw->sleep_conf2.wk_char_num = char_num;
}

/**
 * @brief  Set the UART specific character sequence wakeup mode char.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  char_position UART wakeup char seq char position to be set.
 * @param  value UART wakeup char seq char value to be set.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_char_seq_wk_char(uart_dev_t *hw, uint32_t char_position, char value)
{
    switch (char_position) {
        case 0:
            HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sleep_conf1, wk_char0, value);
            break;
        case 1:
            HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sleep_conf0, wk_char1, value);
            break;
        case 2:
            HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sleep_conf0, wk_char2, value);
            break;
        case 3:
            HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sleep_conf0, wk_char3, value);
            break;
        case 4:
            HAL_FORCE_MODIFY_U32_REG_FIELD(hw->sleep_conf0, wk_char4, value);
            break;
        default:
            abort();
            break;
    }

}

/**
 * @brief   Enable/disable the UART pad clock in sleep_state
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param enable enable or disable
 */
FORCE_INLINE_ATTR void _uart_ll_enable_pad_sleep_clock(uart_dev_t *hw, bool enable)
{
    if (hw == &UART0) {
        LP_AON_CLKRST.hp_clk_ctrl.hp_pad_uart0_slp_clk_en = 1;
    } else if (hw == &UART1) {
        LP_AON_CLKRST.hp_clk_ctrl.hp_pad_uart1_slp_clk_en = 1;
    } else if (hw == &UART2) {
        LP_AON_CLKRST.hp_clk_ctrl.hp_pad_uart2_slp_clk_en = 1;
    } else if (hw == &UART3) {
        LP_AON_CLKRST.hp_clk_ctrl.hp_pad_uart3_slp_clk_en = 1;
    } else if (hw == &UART4) {
        LP_AON_CLKRST.hp_clk_ctrl.hp_pad_uart4_slp_clk_en = 1;
    }
}

// LP_AON_CLKRST.hp_clk_ctrl is a shared register, so this function must be used in an atomic way
#define uart_ll_enable_pad_sleep_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _uart_ll_enable_pad_sleep_clock(__VA_ARGS__); \
    } while(0)

/**
 * @brief  Configure the UART work in normal mode.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_mode_normal(uart_dev_t *hw)
{
    // This function is only for HP_UART use
    // LP_UART can only work in normal mode
    // lp_uart_dev_t has no following fields (reserved), but no harm since we map the LP_UART instance to the uart_dev_t struct

    hw->rs485_conf_sync.rs485_en = 0;
    hw->rs485_conf_sync.rs485tx_rx_en = 0;
    hw->rs485_conf_sync.rs485rxby_tx_en = 0;
    hw->conf0_sync.irda_en = 0;
    uart_ll_update(hw);
}

/**
 * @brief  Configure the UART work in rs485_app_ctrl mode.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_mode_rs485_app_ctrl(uart_dev_t *hw)
{
    // This function is only for HP_UART use
    // LP_UART can only work in normal mode
    // lp_uart_dev_t has no following fields (reserved), but no harm since we map the LP_UART instance to the uart_dev_t struct

    // Application software control, remove echo
    hw->rs485_conf_sync.rs485rxby_tx_en = 1;
    hw->conf0_sync.irda_en = 0;
    hw->conf0_sync.sw_rts = 0;
    hw->conf0_sync.irda_en = 0;
    hw->rs485_conf_sync.dl0_en = 1;
    hw->rs485_conf_sync.dl1_en = 1;
    hw->rs485_conf_sync.rs485_en = 1;
    uart_ll_update(hw);
}

/**
 * @brief  Configure the UART work in rs485_half_duplex mode.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_mode_rs485_half_duplex(uart_dev_t *hw)
{
    // This function is only for HP_UART use
    // LP_UART can only work in normal mode
    // lp_uart_dev_t has no following fields (reserved), but no harm since we map the LP_UART instance to the uart_dev_t struct

    // Enable receiver, sw_rts = 1  generates low level on RTS pin
    hw->conf0_sync.sw_rts = 1;
    // Half duplex mode
    hw->rs485_conf_sync.rs485tx_rx_en = 0;
    // Setting this bit will allow data to be transmitted while receiving data(full-duplex mode).
    // But note that this full-duplex mode has no conflict detection function
    hw->rs485_conf_sync.rs485rxby_tx_en = 0;
    hw->conf0_sync.irda_en = 0;
    hw->rs485_conf_sync.dl0_en = 1;
    hw->rs485_conf_sync.dl1_en = 1;
    hw->rs485_conf_sync.rs485_en = 1;
    uart_ll_update(hw);
}

/**
 * @brief  Configure the UART work in collision_detect mode.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_mode_collision_detect(uart_dev_t *hw)
{
    // This function is only for HP_UART use
    // LP_UART can only work in normal mode
    // lp_uart_dev_t has no following fields (reserved), but no harm since we map the LP_UART instance to the uart_dev_t struct

    hw->conf0_sync.irda_en = 0;
    // Enable full-duplex mode
    hw->rs485_conf_sync.rs485tx_rx_en = 1;
    // Transmitter should send data when the receiver is busy,
    hw->rs485_conf_sync.rs485rxby_tx_en = 1;
    hw->rs485_conf_sync.dl0_en = 1;
    hw->rs485_conf_sync.dl1_en = 1;
    hw->conf0_sync.sw_rts = 0;
    hw->rs485_conf_sync.rs485_en = 1;
    uart_ll_update(hw);
}

/**
 * @brief  Configure the UART work in irda mode.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_mode_irda(uart_dev_t *hw)
{
    // This function is only for HP_UART use
    // LP_UART can only work in normal mode
    // lp_uart_dev_t has no following fields (reserved), but no harm since we map the LP_UART instance to the uart_dev_t struct

    hw->rs485_conf_sync.rs485_en = 0;
    hw->rs485_conf_sync.rs485tx_rx_en = 0;
    hw->rs485_conf_sync.rs485rxby_tx_en = 0;
    hw->conf0_sync.sw_rts = 0;
    hw->conf0_sync.irda_en = 1;
    uart_ll_update(hw);
}

/**
 * @brief  Set uart mode.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  mode The UART mode to be set.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_mode(uart_dev_t *hw, uart_mode_t mode)
{
    switch (mode) {
        default:
        case UART_MODE_UART:
            uart_ll_set_mode_normal(hw);
            break;
        case UART_MODE_RS485_COLLISION_DETECT:
            // Only HP_UART support this mode
            uart_ll_set_mode_collision_detect(hw);
            break;
        case UART_MODE_RS485_APP_CTRL:
            // Only HP_UART support this mode
            uart_ll_set_mode_rs485_app_ctrl(hw);
            break;
        case UART_MODE_RS485_HALF_DUPLEX:
            // Only HP_UART support this mode
            uart_ll_set_mode_rs485_half_duplex(hw);
            break;
        case UART_MODE_IRDA:
            // Only HP_UART support this mode
            uart_ll_set_mode_irda(hw);
            break;
    }
}

/**
 * @brief  Get the UART AT cmd char configuration.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  cmd_char The Pointer to accept value of UART AT cmd char.
 * @param  char_num Pointer to accept the repeat number of UART AT cmd char.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_get_at_cmd_char(uart_dev_t *hw, uint8_t *cmd_char, uint8_t *char_num)
{
    *cmd_char = HAL_FORCE_READ_U32_REG_FIELD(hw->at_cmd_char_sync, at_cmd_char);
    *char_num = HAL_FORCE_READ_U32_REG_FIELD(hw->at_cmd_char_sync, at_char_num);
}

/**
 * @brief  Get the UART wakeup threshold value.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return The UART wakeup threshold value.
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_wakeup_edge_thrd(uart_dev_t *hw)
{
    return hw->sleep_conf2.active_threshold + UART_LL_WAKEUP_EDGE_THRED_MIN;
}

/**
 * @brief  Get the UART data bit configuration.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  data_bit The pointer to accept the UART data bit configuration.
 *
 * @return The bit mode.
 */
FORCE_INLINE_ATTR void uart_ll_get_data_bit_num(uart_dev_t *hw, uart_word_length_t *data_bit)
{
    *data_bit = (uart_word_length_t)hw->conf0_sync.bit_num;
}

/**
 * @brief  Check if the UART sending state machine is in the IDLE state.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return True if the state machine is in the IDLE state, otherwise false is returned.
 */
FORCE_INLINE_ATTR bool uart_ll_is_tx_idle(uart_dev_t *hw)
{
    return (((HAL_FORCE_READ_U32_REG_FIELD(hw->status, txfifo_cnt) >> UART_LL_REG_FIELD_BIT_SHIFT(hw)) == 0) && (hw->fsm_status.st_utx_out == 0));
}

/**
 * @brief  Check if the UART rts flow control is enabled.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return True if hw rts flow control is enabled, otherwise false is returned.
 */
FORCE_INLINE_ATTR bool uart_ll_is_hw_rts_en(uart_dev_t *hw)
{
    return hw->hwfc_conf_sync.rx_flow_en;
}

/**
 * @brief  Check if the UART cts flow control is enabled.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return True if hw cts flow control is enabled, otherwise false is returned.
 */
FORCE_INLINE_ATTR bool uart_ll_is_hw_cts_en(uart_dev_t *hw)
{
    return hw->conf0_sync.tx_flow_en;
}

/**
 * @brief Configure TX signal loop back to RX module, just for the testing purposes
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  loop_back_en Set true to enable the loop back function, else set it false.
 *
 * @return None
 */
FORCE_INLINE_ATTR void uart_ll_set_loop_back(uart_dev_t *hw, bool loop_back_en)
{
    hw->conf0_sync.loopback = loop_back_en;
    uart_ll_update(hw);
}

FORCE_INLINE_ATTR void uart_ll_xon_force_on(uart_dev_t *hw, bool always_on)
{
    hw->swfc_conf0_sync.force_xon = 1;
    uart_ll_update(hw);
    if(!always_on) {
        hw->swfc_conf0_sync.force_xon = 0;
        uart_ll_update(hw);
    }
}

/**
 * @brief  Inverse the UART signal with the given mask.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  inv_mask The UART signal bitmap needs to be inversed.
 *         Use the ORred mask of `uart_signal_inv_t`;
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_inverse_signal(uart_dev_t *hw, uint32_t inv_mask)
{
    // LP_UART does not support UART_SIGNAL_IRDA_TX_INV and UART_SIGNAL_IRDA_RX_INV
    // lp_uart_dev_t has no these fields (reserved), but no harm since we map the LP_UART instance to the uart_dev_t struct

    typeof(hw->conf0_sync) conf0_reg;
    conf0_reg.val = hw->conf0_sync.val;
    conf0_reg.irda_tx_inv = (inv_mask & UART_SIGNAL_IRDA_TX_INV) ? 1 : 0;
    conf0_reg.irda_rx_inv = (inv_mask & UART_SIGNAL_IRDA_RX_INV) ? 1 : 0;
    conf0_reg.rxd_inv = (inv_mask & UART_SIGNAL_RXD_INV) ? 1 : 0;
    conf0_reg.txd_inv = (inv_mask & UART_SIGNAL_TXD_INV) ? 1 : 0;
    hw->conf0_sync.val = conf0_reg.val;

    typeof(hw->conf1) conf1_reg;
    conf1_reg.val = hw->conf1.val;
    conf1_reg.rts_inv = (inv_mask & UART_SIGNAL_RTS_INV) ? 1 : 0;
    conf1_reg.dtr_inv = (inv_mask & UART_SIGNAL_DTR_INV) ? 1 : 0;
    conf1_reg.cts_inv = (inv_mask & UART_SIGNAL_CTS_INV) ? 1 : 0;
    conf1_reg.dsr_inv = (inv_mask & UART_SIGNAL_DSR_INV) ? 1 : 0;
    hw->conf1.val = conf1_reg.val;
    uart_ll_update(hw);
}

/**
 * @brief  Configure the timeout value for receiver receiving a byte, and enable rx timeout function.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  tout_thrd The timeout value as UART bit time. The rx timeout function will be disabled if `tout_thrd == 0`.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_rx_tout(uart_dev_t *hw, uint16_t tout_thrd)
{
    uint16_t tout_val = tout_thrd;
    if(tout_thrd > 0) {
        hw->tout_conf_sync.rx_tout_thrhd = tout_val;
        hw->tout_conf_sync.rx_tout_en = 1;
    } else {
        hw->tout_conf_sync.rx_tout_en = 0;
    }
    uart_ll_update(hw);
}

/**
 * @brief  Get the timeout value for receiver receiving a byte.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return tout_thr The timeout threshold value. If timeout feature is disabled returns 0.
 */
FORCE_INLINE_ATTR uint16_t uart_ll_get_rx_tout_thr(uart_dev_t *hw)
{
    uint16_t tout_thrd = 0;
    if(hw->tout_conf_sync.rx_tout_en > 0) {
        tout_thrd = hw->tout_conf_sync.rx_tout_thrhd;
    }
    return tout_thrd;
}

/**
 * @brief  Get UART maximum timeout threshold.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return maximum timeout threshold.
 */
FORCE_INLINE_ATTR uint16_t uart_ll_max_tout_thrd(uart_dev_t *hw)
{
    return ((hw) == &LP_UART) ? LP_UART_RX_TOUT_THRHD_V : UART_RX_TOUT_THRHD_V;
}

/**
 * @brief  Configure the auto baudrate.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  enable Boolean marking whether the auto baudrate should be enabled or not.
 */
FORCE_INLINE_ATTR void uart_ll_set_autobaud_en(uart_dev_t *hw, bool enable)
{
    // LP_UART does not support autobaud
    // lp_uart_dev_t has no following fields (reserved), but no harm since we map the LP_UART instance to the uart_dev_t struct

    hw->conf0_sync.autobaud_en = enable ? 1 : 0;
    uart_ll_update(hw);
}

/**
 * @brief  Get the RXD edge count.
 *
 * @param  hw Beginning address of the peripheral registers.
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_rxd_edge_cnt(uart_dev_t *hw)
{
    return hw->rxd_cnt.rxd_edge_cnt;
}

/**
 * @brief  Get the positive pulse minimum count.
 *
 * @param  hw Beginning address of the peripheral registers.
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_pos_pulse_cnt(uart_dev_t *hw)
{
    return hw->pospulse.posedge_min_cnt;
}

/**
 * @brief  Get the negative pulse minimum count.
 *
 * @param  hw Beginning address of the peripheral registers.
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_neg_pulse_cnt(uart_dev_t *hw)
{
    return hw->negpulse.negedge_min_cnt;
}

/**
 * @brief  Get the high pulse minimum count.
 *
 * @param  hw Beginning address of the peripheral registers.
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_high_pulse_cnt(uart_dev_t *hw)
{
    return hw->highpulse.highpulse_min_cnt;
}

/**
 * @brief  Get the low pulse minimum count.
 *
 * @param  hw Beginning address of the peripheral registers.
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_low_pulse_cnt(uart_dev_t *hw)
{
    return hw->lowpulse.lowpulse_min_cnt;
}

/**
 * @brief  Force UART xoff.
 *
 * @param  uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_force_xoff(uart_port_t uart_num)
{
    uart_dev_t *hw = UART_LL_GET_HW(uart_num);
    hw->swfc_conf0_sync.force_xon = 0;
    hw->swfc_conf0_sync.sw_flow_con_en = 1;
    hw->swfc_conf0_sync.force_xoff = 1;
    uart_ll_update(hw);
}

/**
 * @brief  Force UART xon.
 *
 * @param  uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_force_xon(uart_port_t uart_num)
{
    uart_dev_t *hw = UART_LL_GET_HW(uart_num);
    hw->swfc_conf0_sync.force_xoff = 0;
    hw->swfc_conf0_sync.force_xon = 1;
    hw->swfc_conf0_sync.sw_flow_con_en = 0;
    hw->swfc_conf0_sync.force_xon = 0;
    uart_ll_update(hw);
}

/**
 * @brief  Get UART transmitter finite-state machine status.
 *
 * @param  uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 *
 * @return UART module FSM status.
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_tx_fsm_status(uart_port_t uart_num)
{
    return REG_GET_FIELD(UART_FSM_STATUS_REG(uart_num), UART_ST_UTX_OUT);
}

/**
 * @brief  Configure UART whether to discard when receiving wrong data
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  discard true: Receiver stops storing data into FIFO when data is wrong
 *                false: Receiver continue storing data into FIFO when data is wrong
 */
FORCE_INLINE_ATTR void uart_ll_discard_error_data(uart_dev_t *hw, bool discard)
{
    hw->conf0_sync.err_wr_mask = discard ? 1 : 0;
    uart_ll_update(hw);
}

#ifdef __cplusplus
}
#endif
