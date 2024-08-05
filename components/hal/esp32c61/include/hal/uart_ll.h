/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for UART register operations.
// Note that most of the register operations in this layer are non-atomic operations.


#pragma once

#include <stdlib.h>
#include "esp_attr.h"
#include "hal/misc.h"
#include "hal/uart_types.h"
#include "soc/uart_reg.h"
#include "soc/uart_struct.h"
#include "soc/pcr_struct.h"
#include "soc/pcr_reg.h"
#include "esp_attr.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

// The default fifo depth
#define UART_LL_FIFO_DEF_LEN  (SOC_UART_FIFO_LEN)
// Get UART hardware instance with giving uart num
#define UART_LL_GET_HW(num) (((num) == UART_NUM_0) ? (&UART0) : (((num) == UART_NUM_1) ? (&UART1) : (&UART2)))

#define UART_LL_MIN_WAKEUP_THRESH (2)
#define UART_LL_INTR_MASK         (0x7ffff) //All interrupt mask

#define UART_LL_FSM_IDLE                       (0x0)
#define UART_LL_FSM_TX_WAIT_SEND               (0xf)

#define UART_LL_PCR_REG_U32_SET(hw, reg_suffix, field_suffix, val)  \
    if ((hw) == &UART0) { \
        HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.uart0_##reg_suffix, uart0_##field_suffix, (val))  \
    } else if ((hw) == &UART1) {  \
        HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.uart1_##reg_suffix, uart1_##field_suffix, (val))  \
    } else { \
        HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.uart2_##reg_suffix, uart2_##field_suffix, (val))  \
    }

#define UART_LL_PCR_REG_U32_GET(hw, reg_suffix, field_suffix)  \
    (((hw) == &UART0) ? HAL_FORCE_READ_U32_REG_FIELD(PCR.uart0_##reg_suffix, uart0_##field_suffix) : \
    ((hw) == &UART1) ? HAL_FORCE_READ_U32_REG_FIELD(PCR.uart1_##reg_suffix, uart1_##field_suffix) : \
    HAL_FORCE_READ_U32_REG_FIELD(PCR.uart2_##reg_suffix, uart2_##field_suffix))

#define UART_LL_PCR_REG_SET(hw, reg_suffix, field_suffix, val)    \
    if ((hw) == &UART0) { \
        PCR.uart0_##reg_suffix.uart0_##field_suffix = (val);  \
    } else if ((hw) == &UART1) {  \
        PCR.uart1_##reg_suffix.uart1_##field_suffix = (val);  \
    } else { \
        PCR.uart2_##reg_suffix.uart2_##field_suffix = (val);  \
    }

#define UART_LL_PCR_REG_GET(hw, reg_suffix, field_suffix)  \
    (((hw) == &UART0) ? PCR.uart0_##reg_suffix.uart0_##field_suffix : \
    ((hw) == &UART1) ? PCR.uart1_##reg_suffix.uart1_##field_suffix : \
    PCR.uart2_##reg_suffix.uart2_##field_suffix)

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
 * @brief Check if UART is enabled or disabled.
 *
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 *
 * @return true: enabled; false: disabled
 */
FORCE_INLINE_ATTR bool uart_ll_is_enabled(uint32_t uart_num)
{
    switch (uart_num) {
    case 0:
        return PCR.uart0_conf.uart0_clk_en && !PCR.uart0_conf.uart0_rst_en;
    case 1: // UART_1
        return PCR.uart1_conf.uart1_clk_en && !PCR.uart1_conf.uart1_rst_en;
    case 2: // UART_2
        return PCR.uart2_conf.uart2_clk_en && !PCR.uart2_conf.uart2_rst_en;
    default:
        HAL_ASSERT(false);
        return false;
    }

}

/**
 * @brief Enable the bus clock for uart
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 * @param enable true to enable, false to disable
 */
static inline void uart_ll_enable_bus_clock(uart_port_t uart_num, bool enable)
{
    switch (uart_num) {
    case 0:
        PCR.uart0_conf.uart0_clk_en = enable;
        break;
    case 1:
        PCR.uart1_conf.uart1_clk_en = enable;
        break;
    case 2:
        PCR.uart2_conf.uart2_clk_en = enable;
        break;
    default:
        abort();
        break;
    }
}

/**
 * @brief Reset UART module
 * @param uart_num UART port number, the max port number is (UART_NUM_MAX -1).
 */
static inline void uart_ll_reset_register(uart_port_t uart_num)
{
    switch (uart_num) {
    case 0:
        PCR.uart0_conf.uart0_rst_en = 1;
        PCR.uart0_conf.uart0_rst_en = 0;
        break;
    case 1:
        PCR.uart1_conf.uart1_rst_en = 1;
        PCR.uart1_conf.uart1_rst_en = 0;
        break;
    case 2:
        PCR.uart2_conf.uart2_rst_en = 1;
        PCR.uart2_conf.uart2_rst_en = 0;
        break;
    default:
        abort();
        break;
    }
}

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

/**
 * @brief  Enable the UART clock.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_sclk_enable(uart_dev_t *hw)
{
    UART_LL_PCR_REG_SET(hw, sclk_conf, sclk_en, 1);
}

/**
 * @brief  Disable the UART clock.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_sclk_disable(uart_dev_t *hw)
{
    UART_LL_PCR_REG_SET(hw, sclk_conf, sclk_en, 0);
}

/**
 * @brief  Set the UART source clock.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  source_clk The UART source clock. The source clock can be PLL_F48M clock, RTC clock or XTAL clock.
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
            // Invalid UART clock source
            abort();
    }
    UART_LL_PCR_REG_SET(hw, sclk_conf, sclk_sel, sel_value);
}

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
    switch (UART_LL_PCR_REG_GET(hw, sclk_conf, sclk_sel)) {
        default:
        case 0:
            *source_clk = (soc_module_clk_t)UART_SCLK_XTAL;
            break;
        case 1:
            *source_clk = (soc_module_clk_t)UART_SCLK_RTC;
            break;
        case 2:
            *source_clk = (soc_module_clk_t)UART_SCLK_PLL_F80M;
            break;
    }
}

/**
 * @brief  Configure the baud-rate.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  baud The baud rate to be set.
 * @param  sclk_freq Frequency of the clock source of UART, in Hz.
 *
 * @return None
 */
FORCE_INLINE_ATTR void uart_ll_set_baudrate(uart_dev_t *hw, uint32_t baud, uint32_t sclk_freq)
{
#define DIV_UP(a, b)    (((a) + (b) - 1) / (b))
    const uint32_t max_div = BIT(12) - 1;   // UART divider integer part only has 12 bits
    uint32_t sclk_div = DIV_UP(sclk_freq, (uint64_t)max_div * baud);

    if (sclk_div == 0) abort();

    uint32_t clk_div = ((sclk_freq) << 4) / (baud * sclk_div);
    // The baud rate configuration register is divided into
    // an integer part and a fractional part.
    hw->clkdiv_sync.clkdiv_int = clk_div >> 4;
    hw->clkdiv_sync.clkdiv_frag = clk_div & 0xf;
    UART_LL_PCR_REG_U32_SET(hw, sclk_conf, sclk_div_num, sclk_div - 1);
#undef DIV_UP
    uart_ll_update(hw);
}

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
    int sclk_div = UART_LL_PCR_REG_U32_GET(hw, sclk_conf, sclk_div_num) + 1;
    return ((sclk_freq << 4)) / (((div_reg.clkdiv_int << 4) | div_reg.clkdiv_frag) * sclk_div);
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
        buf[i] = hw->fifo.rxfifo_rd_byte;
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
    return hw->status.rxfifo_cnt;
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
    return UART_LL_FIFO_DEF_LEN - hw->status.txfifo_cnt;
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
 * @param  full_thrhd The full threshold value of the rxfifo. `full_thrhd` should be less than `UART_LL_FIFO_DEF_LEN`.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_rxfifo_full_thr(uart_dev_t *hw, uint16_t full_thrhd)
{
    hw->conf1.rxfifo_full_thrhd = full_thrhd;
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
    hw->conf1.txfifo_empty_thrhd = empty_thrhd;
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
        hw->hwfc_conf_sync.rx_flow_thrhd = rx_thrs;
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
        hw->swfc_conf1.xon_threshold = flow_ctrl->xon_thrd;
        hw->swfc_conf1.xoff_threshold = flow_ctrl->xoff_thrd;
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->swfc_conf0_sync, xon_char, flow_ctrl->xon_char);
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->swfc_conf0_sync, xoff_char, flow_ctrl->xoff_char);
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->at_cmd_char_sync, char_num, cmd_char->char_num);
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
FORCE_INLINE_ATTR void uart_ll_set_wakeup_thrd(uart_dev_t *hw, uint32_t wakeup_thrd)
{
    hw->sleep_conf2.active_threshold = wakeup_thrd - UART_LL_MIN_WAKEUP_THRESH;
}

/**
 * @brief  Configure the UART work in normal mode.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return None.
 */
FORCE_INLINE_ATTR void uart_ll_set_mode_normal(uart_dev_t *hw)
{
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
            uart_ll_set_mode_collision_detect(hw);
            break;
        case UART_MODE_RS485_APP_CTRL:
            uart_ll_set_mode_rs485_app_ctrl(hw);
            break;
        case UART_MODE_RS485_HALF_DUPLEX:
            uart_ll_set_mode_rs485_half_duplex(hw);
            break;
        case UART_MODE_IRDA:
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
    *char_num = HAL_FORCE_READ_U32_REG_FIELD(hw->at_cmd_char_sync, char_num);
}

/**
 * @brief  Get the UART wakeup threshold value.
 *
 * @param  hw Beginning address of the peripheral registers.
 *
 * @return The UART wakeup threshold value.
 */
FORCE_INLINE_ATTR uint32_t uart_ll_get_wakeup_thrd(uart_dev_t *hw)
{
    return hw->sleep_conf2.active_threshold + UART_LL_MIN_WAKEUP_THRESH;
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
    return ((hw->status.txfifo_cnt == 0) && (hw->fsm_status.st_utx_out == 0));
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
    return UART_RX_TOUT_THRHD_V;
}

/**
 * @brief  Configure the auto baudrate.
 *
 * @param  hw Beginning address of the peripheral registers.
 * @param  enable Boolean marking whether the auto baudrate should be enabled or not.
 */
FORCE_INLINE_ATTR void uart_ll_set_autobaud_en(uart_dev_t *hw, bool enable)
{
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
    REG_CLR_BIT(UART_SWFC_CONF0_SYNC_REG(uart_num), UART_FORCE_XON);
    REG_SET_BIT(UART_SWFC_CONF0_SYNC_REG(uart_num), UART_SW_FLOW_CON_EN | UART_FORCE_XOFF);
    uart_ll_update(UART_LL_GET_HW(uart_num));
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
    REG_CLR_BIT(UART_SWFC_CONF0_SYNC_REG(uart_num), UART_FORCE_XOFF);
    REG_SET_BIT(UART_SWFC_CONF0_SYNC_REG(uart_num), UART_FORCE_XON);
    REG_CLR_BIT(UART_SWFC_CONF0_SYNC_REG(uart_num), UART_SW_FLOW_CON_EN | UART_FORCE_XON);
    uart_ll_update(UART_LL_GET_HW(uart_num));
}

/**
 * @brief  Get UART finite-state machine status.
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
