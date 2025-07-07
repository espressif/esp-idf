/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The Lowlevel layer for TWAI is not public api, don't use in application code.
 ******************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "soc/pcr_reg.h"
#include "soc/pcr_struct.h"
#include "soc/twaifd_reg.h"
#include "soc/twaifd_struct.h"
#include "hal/twai_types.h"
#include "hal/assert.h"
#include "hal/misc.h"

#define TWAIFD_LL_GET_HW(num) (((num) == 0) ? (&TWAI0) : (&TWAI1))

#define TWAI_LL_BRP_MIN                 1
#define TWAI_LL_BRP_MAX                 255
#define TWAI_LL_TSEG1_MIN               0
#define TWAI_LL_TSEG2_MIN               1
#define TWAI_LL_TSEG1_MAX               TWAIFD_PH1
#define TWAI_LL_TSEG2_MAX               TWAIFD_PH2
#define TWAI_LL_SJW_MAX                 TWAIFD_SJW

#define TWAIFD_IDENTIFIER_BASE_S        18      // Start bit of std_id in IDENTIFIER_W of TX buffer or RX buffer

#define TWAIFD_LL_ERR_BIT_ERR           0x0     // Bit Error
#define TWAIFD_LL_ERR_CRC_ERR           0x1     // CRC Error
#define TWAIFD_LL_ERR_FRM_ERR           0x2     // Form Error
#define TWAIFD_LL_ERR_ACK_ERR           0x3     // Acknowledge Error
#define TWAIFD_LL_ERR_STUF_ERR          0x4     // Stuff Error

#define TWAIFD_LL_SSP_SRC_MEAS_OFFSET   0x0     // Using Measured Transmitter delay + SSP_OFFSET
#define TWAIFD_LL_SSP_SRC_NO_SSP        0x1     // SSP is disabled
#define TWAIFD_LL_SSP_SRC_OFFSET_ONLY   0x2     // Using SSP_OFFSET only

#define TWAIFD_LL_TX_CMD_EMPTY          TWAIFD_TXCE     // Set tx buffer to "Empty" state
#define TWAIFD_LL_TX_CMD_READY          TWAIFD_TXCR     // Set tx buffer to "Ready" state
#define TWAIFD_LL_TX_CMD_ABORT          TWAIFD_TXCA     // Set tx buffer to "Aborted" state

#define TWAIFD_LL_HW_CMD_RST_ERR_CNT    TWAIFD_ERCRST   // Error Counters Reset
#define TWAIFD_LL_HW_CMD_RST_RX_CNT     TWAIFD_RXFCRST  // Clear RX bus traffic counter
#define TWAIFD_LL_HW_CMD_RST_TX_CNT     TWAIFD_TXFCRST  // Clear TX bus traffic counter

#define TWAIFD_LL_INTR_TX_DONE          TWAIFD_TXBHCI_INT_ST// Transmit finish (ok or error)
#define TWAIFD_LL_INTR_TX_SUCCESS       TWAIFD_TXI_INT_ST   // Transmit success without error
#define TWAIFD_LL_INTR_RX_NOT_EMPTY     TWAIFD_RBNEI_INT_ST // RX buffer not empty interrupt
#define TWAIFD_LL_INTR_RX_FULL          TWAIFD_RXFI_INT_ST  // RX buffer full interrupt
#define TWAIFD_LL_INTR_ERR_WARN         TWAIFD_EWLI_INT_ST  // Error Interrupt
#define TWAIFD_LL_INTR_BUS_ERR          TWAIFD_BEI_INT_ST   // Bus error interrupt
#define TWAIFD_LL_INTR_FSM_CHANGE       TWAIFD_FCSI_INT_ST  // Fault confinement state changed interrupt
#define TWAIFD_LL_INTR_ARBI_LOST        TWAIFD_ALI_INT_ST   // Arbitration Lost Interrupt
#define TWAIFD_LL_INTR_DATA_OVERRUN     TWAIFD_DOI_INT_ST   // Data Overrun Interrupt

#define TWAI_LL_DRIVER_INTERRUPTS   (TWAIFD_LL_INTR_TX_DONE | TWAIFD_LL_INTR_RX_NOT_EMPTY | TWAIFD_LL_INTR_RX_FULL | \
                                    TWAIFD_LL_INTR_ERR_WARN | TWAIFD_LL_INTR_BUS_ERR | TWAIFD_LL_INTR_FSM_CHANGE | \
                                    TWAIFD_LL_INTR_ARBI_LOST | TWAIFD_LL_INTR_DATA_OVERRUN)

/**
 * @brief Enable the bus clock and module clock for twai module
 *
 * @param twai_id Hardware ID
 * @param enable true to enable, false to disable
 */
static inline void twaifd_ll_enable_bus_clock(uint8_t twai_id, bool enable)
{
    PCR.twai[twai_id].twai_conf.twai_clk_en = enable;
}

/**
 * @brief Reset the twai module
 *
 * @param twai_id Hardware ID
 */
static inline void twaifd_ll_reset_register(uint8_t twai_id)
{
    PCR.twai[twai_id].twai_conf.twai_rst_en = 1;
    PCR.twai[twai_id].twai_conf.twai_rst_en = 0;
}

/**
 * @brief Set clock source for TWAI module
 *
 * @param twai_id Hardware ID
 * @param clk_src Clock source
 */
static inline void twaifd_ll_set_clock_source(uint8_t twai_id, twai_clock_source_t clk_src)
{
    PCR.twai[twai_id].twai_func_clk_conf.twai_func_clk_sel = (clk_src == TWAI_CLK_SRC_XTAL) ? 0 : 1;
}

/**
 * @brief Enable TWAI module clock source
 *
 * @param twai_id Hardware ID
 * @param enable true to enable, false to disable
 */
static inline void twaifd_ll_enable_clock(uint8_t twai_id, bool enable)
{
    PCR.twai[twai_id].twai_func_clk_conf.twai_func_clk_en = enable;
    if (enable) {
        while (!PCR.twai[twai_id].twai_conf.twai_ready);
    }
}

/**
 * @brief Waits for pending changes to take effect in the hardware.
 *
 * @param hw Pointer to the hardware structure.
 */
static inline void twaifd_ll_waiting_state_change(twaifd_dev_t *hw)
{
    while (!hw->int_stat.fcsi_int_st); // Wait until the change is applied
}


/* ---------------------------- Mode Register ------------------------------- */
// WARNING!! Following 'mode_settings' should in same spin_lock` !!!

/**
 * @brief Reset hardware.
 *
 * @param hw Pointer to hardware structure.
 */
static inline void twaifd_ll_reset(twaifd_dev_t *hw)
{
    hw->mode_settings.rst = 1;
}

/**
 * @brief Enable or disable hardware.
 *
 * @param hw Pointer to hardware structure.
 * @param enable Boolean flag to enable (true) or disable (false).
 */
static inline void twaifd_ll_enable_hw(twaifd_dev_t *hw, bool enable)
{
    hw->mode_settings.ena = enable;
}

/**
 * @brief   Set operating mode of TWAI controller
 *
 * @param hw Start address of the TWAI registers
 * @param listen_only Listen only mode (a.k.a. bus monitoring mode)
 * @param self_test Self test mode
 * @param loopback Loopback mode
 */
static inline void twaifd_ll_set_mode(twaifd_dev_t *hw, bool listen_only, bool self_test, bool loopback)
{
    //mode should be changed under disabled
    HAL_ASSERT(hw->mode_settings.ena == 0);

    twaifd_mode_settings_reg_t opmode = {.val = hw->mode_settings.val};
    opmode.stm = self_test;
    opmode.bmm = listen_only;
    opmode.ilbp = loopback;

    hw->mode_settings.val = opmode.val;
}

/**
 * @brief Set the TX retransmission limit.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param limit Retransmission limit (0-15, or negative for infinite).
 */
static inline void twaifd_ll_set_tx_retrans_limit(twaifd_dev_t *hw, int8_t limit)
{
    HAL_ASSERT(limit <= (int8_t)TWAIFD_RTRTH_V); // Check the limit is valid
    hw->mode_settings.rtrle = (limit >= 0);  // Enable/disable retransmissions
    hw->mode_settings.rtrth = limit;         // Set the limit
}

/**
 * set bit rate flexible between nominal field and data field
 * when set this bit, all frame will be regarded as CANFD frame, even though nominal bit rate and data bit rate are the same
 */
static inline void twaifd_ll_enable_fd_mode(twaifd_dev_t *hw, bool ena)
{
    hw->mode_settings.fde = ena;
}

/**
 * @brief Enable or disable the RX fifo automatic increase when read to register
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param ena Set to true to enable RX automatic mode, false to disable.
 */
static inline void twaifd_ll_enable_rxfifo_auto_incrase(twaifd_dev_t *hw, bool ena)
{
    hw->mode_settings.rxbam = ena;
}

/**
 * @brief Enable or disable the filter.
 *
 * @param hw Pointer to hardware structure.
 * @param enable `true` to enable, `false` to disable.
 */
static inline void twaifd_ll_enable_filter_mode(twaifd_dev_t* hw, bool enable)
{
    // Must be called when hardware is disabled.
    HAL_ASSERT(hw->mode_settings.ena == 0);
    hw->mode_settings.afm = enable;
}

/**
 * @brief Set remote frame filtering behaviour.
 *
 * @param hw Pointer to hardware structure.
 * @param en True to drop, false to Receive to next filter
 */
static inline void twaifd_ll_filter_block_rtr(twaifd_dev_t* hw, bool en)
{
    hw->mode_settings.fdrf = en;
}

/**
 * @brief Enable or disable the time-triggered transmission mode for the TWAI-FD peripheral.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param enable Set to true to enable time-triggered transmission mode, false to disable.
 */
static inline void twaifd_ll_enable_time_trig_trans_mode(twaifd_dev_t* hw, bool enable)
{
    hw->mode_settings.tttm = enable;
}

/* --------------------------- Command Register ----------------------------- */
/**
 * @brief Set command to TWAIFD hardware
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param commands command code refer to `TWAIFD_LL_HW_CMD_`.
 */
static inline void twaifd_ll_set_operate_cmd(twaifd_dev_t *hw, uint32_t commands)
{
    hw->command.val = commands;
    while (hw->command.val & commands);
}

/* -------------------------- Interrupt Register ---------------------------- */

/**
 * @brief   Set which interrupts are enabled
 *
 * @param hw Start address of the TWAI registers
 * @param intr_mask mask of interrupts to enable
 */
static inline void twaifd_ll_enable_intr(twaifd_dev_t *hw, uint32_t intr_mask)
{
    hw->int_ena_set.val = intr_mask;
    hw->int_ena_clr.val = ~intr_mask;
}

/**
 * @brief Get the interrupt status of the TWAI-FD peripheral.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @return The current interrupt status as a 32-bit value, used with `TWAIFD_LL_INTR_`.
 */
__attribute__((always_inline))
static inline uint32_t twaifd_ll_get_intr_status(twaifd_dev_t *hw)
{
    return hw->int_stat.val;
}

/**
 * @brief Clear the specified interrupt status of the TWAI-FD peripheral.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param intr_mask The interrupt mask specifying which interrupts to clear.
 */
__attribute__((always_inline))
static inline void twaifd_ll_clr_intr_status(twaifd_dev_t *hw, uint32_t intr_mask)
{
    // this register is write to clear
    hw->int_stat.val = intr_mask;
}

/* ------------------------ Bus Timing Registers --------------------------- */
/**
 * @brief Check if the brp value valid
 *
 * @param brp Bit rate prescaler value
 * @return true or False
 */
static inline bool twaifd_ll_check_brp_validation(uint32_t brp)
{
    return (brp >= TWAI_LL_BRP_MIN) && (brp <= TWAI_LL_BRP_MAX);
}

/**
 * @brief Set bus timing nominal bit rate
 *
 * @param hw Start address of the TWAI registers
 * @param timing_param timing params
 */
static inline void twaifd_ll_set_nominal_bitrate(twaifd_dev_t *hw, const twai_timing_advanced_config_t *timing_param)
{
    twaifd_btr_reg_t reg_w = {.val = 0};
    HAL_FORCE_MODIFY_U32_REG_FIELD(reg_w, brp, timing_param->brp);
    reg_w.prop = timing_param->prop_seg;
    reg_w.ph1 = timing_param->tseg_1;
    reg_w.ph2 = timing_param->tseg_2;
    reg_w.sjw = timing_param->sjw;

    hw->btr.val = reg_w.val;
}

/**
 * @brief Set bus timing for FD data section bit rate
 *
 * @param hw Start address of the TWAI registers
 * @param timing_param_fd FD timing params
 */
static inline void twaifd_ll_set_fd_bitrate(twaifd_dev_t *hw, const twai_timing_advanced_config_t *timing_param_fd)
{
    twaifd_btr_fd_reg_t reg_w = {.val = 0};
    HAL_FORCE_MODIFY_U32_REG_FIELD(reg_w, brp_fd, timing_param_fd->brp);
    reg_w.prop_fd = timing_param_fd->prop_seg;
    reg_w.ph1_fd = timing_param_fd->tseg_1;
    reg_w.ph2_fd = timing_param_fd->tseg_2;
    reg_w.sjw_fd = timing_param_fd->sjw;

    hw->btr_fd.val = reg_w.val;
}

/**
 * @brief Secondary Sample Point (SSP) config for data bitrate
 *
 * @param hw Start address of the TWAI registers
 * @param ssp_src_code Secondary point mode config, see TWAIFD_LL_SSP_SRC_xxx.
 * @param offset_val Secondary sampling point position is configured as delay from Sync_Seg in multiples of System clock
 */
static inline void twaifd_ll_config_secondary_sample_point(twaifd_dev_t *hw, uint8_t ssp_src_code, uint8_t offset_val)
{
    hw->trv_delay_ssp_cfg.ssp_src = ssp_src_code;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->trv_delay_ssp_cfg, ssp_offset, offset_val);
}

/* ----------------------------- ERR Capt Register ------------------------------- */

/**
 * @brief Get the error reason flags from the TWAI-FD peripheral.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @return The error reasons, see `twai_error_flags_t`
 */
__attribute__((always_inline))
static inline twai_error_flags_t twaifd_ll_get_err_reason(twaifd_dev_t *hw)
{
    uint8_t error_code = hw->err_capt_retr_ctr_alc_ts_info.err_type;
    twai_error_flags_t errors = {
        .bit_err = error_code == TWAIFD_LL_ERR_BIT_ERR,
        .form_err = error_code == TWAIFD_LL_ERR_FRM_ERR,
        .stuff_err = error_code == TWAIFD_LL_ERR_STUF_ERR,
        .ack_err = error_code == TWAIFD_LL_ERR_ACK_ERR
    };
    return errors;
}

/* ----------------------------- EWL Register ------------------------------- */

// this func can only use in TWAIFD_MODE_TEST, and 'mode_settings.ena' must be zero
static inline void twaifd_ll_set_err_warn_limit(twaifd_dev_t *hw, uint32_t ewl)
{
    HAL_ASSERT(hw->mode_settings.tstm);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->ewl_erp_fault_state, ew_limit, ewl);
}

/**
 * @brief   Get Error Warning Limit
 *
 * @param hw Start address of the TWAI registers
 * @return Error Warning Limit
 */
__attribute__((always_inline))
static inline uint32_t twaifd_ll_get_err_warn_limit(twaifd_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->ewl_erp_fault_state, ew_limit);
}

/**
 * @brief Get the current fault state of the TWAI-FD peripheral.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @return Fault state (bus-off, error passive, or active state).
 */
__attribute__((always_inline))
static inline twai_error_state_t twaifd_ll_get_fault_state(twaifd_dev_t *hw)
{
    if (hw->ewl_erp_fault_state.bof) {
        return TWAI_ERROR_BUS_OFF;
    }
    if (hw->ewl_erp_fault_state.erp) {
        return TWAI_ERROR_PASSIVE;
    }
    return TWAI_ERROR_ACTIVE;
}

/**
 * @brief Get the error count in normal mode for the TWAI-FD peripheral.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @return Error count in normal mode.
 */
static inline uint32_t twaifd_ll_get_err_count_norm(twaifd_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->err_norm_err_fd, err_norm_val);
}

/**
 * @brief Get the error count in FD mode for the TWAI-FD peripheral.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @return Error count in FD mode.
 */
static inline uint32_t twaifd_ll_get_err_count_fd(twaifd_dev_t *hw)
{
    return HAL_FORCE_READ_U32_REG_FIELD(hw->err_norm_err_fd, err_fd_val);
}

/* ------------------------ RX Error Count Register ------------------------- */
/**
 * @brief   Get RX Error Counter
 *
 * @param hw Start address of the TWAI registers
 * @return REC value
 */
__attribute__((always_inline))
static inline uint32_t twaifd_ll_get_rec(twaifd_dev_t *hw)
{
    return hw->rec_tec.rec_val;
}

/* ------------------------ TX Error Count Register ------------------------- */
/**
 * @brief   Get TX Error Counter
 *
 * @param hw Start address of the TWAI registers
 * @return TEC value
 */
__attribute__((always_inline))
static inline uint32_t twaifd_ll_get_tec(twaifd_dev_t *hw)
{
    return hw->rec_tec.tec_val;
}

/* ---------------------- Acceptance Filter Registers ----------------------- */
/**
 * @brief Enable or disable filter to receive basic frame with std id
 *
 * @param hw Pointer to the TWAI FD hardware instance
 * @param filter_id The unique ID of the filter to configure
 * @param is_range Setting for range filter or mask filter
 * @param en True to receive, False to drop
 */
static inline void twaifd_ll_filter_enable_basic_std(twaifd_dev_t* hw, uint8_t filter_id, bool is_range, bool en)
{
    HAL_ASSERT(filter_id < (is_range ? SOC_TWAI_RANGE_FILTER_NUM : SOC_TWAI_MASK_FILTER_NUM));
    // The hw_filter_id of range_filter is indexed after mask_filter
    uint8_t hw_filter_id = is_range ? filter_id + SOC_TWAI_MASK_FILTER_NUM : filter_id;
    if (en) {
        hw->filter_control_filter_status.val |= TWAIFD_FANB << (hw_filter_id * TWAIFD_FBNB_S);
    } else {
        hw->filter_control_filter_status.val &= ~(TWAIFD_FANB << (hw_filter_id * TWAIFD_FBNB_S));
    }
}

/**
 * @brief Enable or disable filter to receive basic frame with ext id
 *
 * @param hw Pointer to the TWAI FD hardware instance
 * @param filter_id The unique ID of the filter to configure
 * @param is_range Setting for range filter or mask filter
 * @param en True to receive, False to drop
 */
static inline void twaifd_ll_filter_enable_basic_ext(twaifd_dev_t* hw, uint8_t filter_id, bool is_range, bool en)
{
    HAL_ASSERT(filter_id < (is_range ? SOC_TWAI_RANGE_FILTER_NUM : SOC_TWAI_MASK_FILTER_NUM));
    // The hw_filter_id of range_filter is indexed after mask_filter
    uint8_t hw_filter_id = is_range ? filter_id + SOC_TWAI_MASK_FILTER_NUM : filter_id;
    if (en) {
        hw->filter_control_filter_status.val |= TWAIFD_FANE << (hw_filter_id * TWAIFD_FBNB_S);
    } else {
        hw->filter_control_filter_status.val &= ~(TWAIFD_FANE << (hw_filter_id * TWAIFD_FBNB_S));
    }
}

/**
 * @brief Enable or disable filter to receive fd frame with std id
 *
 * @param hw Pointer to the TWAI FD hardware instance
 * @param filter_id The unique ID of the filter to configure
 * @param is_range Setting for range filter or mask filter
 * @param en True to receive, False to drop
 */
static inline void twaifd_ll_filter_enable_fd_std(twaifd_dev_t* hw, uint8_t filter_id, bool is_range, bool en)
{
    HAL_ASSERT(filter_id < (is_range ? SOC_TWAI_RANGE_FILTER_NUM : SOC_TWAI_MASK_FILTER_NUM));
    // The hw_filter_id of range_filter is indexed after mask_filter
    uint8_t hw_filter_id = is_range ? filter_id + SOC_TWAI_MASK_FILTER_NUM : filter_id;
    if (en) {
        hw->filter_control_filter_status.val |= TWAIFD_FAFB << (hw_filter_id * TWAIFD_FBNB_S);
    } else {
        hw->filter_control_filter_status.val &= ~(TWAIFD_FAFB << (hw_filter_id * TWAIFD_FBNB_S));
    }
}

/**
 * @brief Enable or disable filter to receive fd frame with ext id
 *
 * @param hw Pointer to the TWAI FD hardware instance
 * @param filter_id The unique ID of the filter to configure
 * @param is_range Setting for range filter or mask filter
 * @param en True to receive, False to drop
 */
static inline void twaifd_ll_filter_enable_fd_ext(twaifd_dev_t* hw, uint8_t filter_id, bool is_range, bool en)
{
    HAL_ASSERT(filter_id < (is_range ? SOC_TWAI_RANGE_FILTER_NUM : SOC_TWAI_MASK_FILTER_NUM));
    // The hw_filter_id of range_filter is indexed after mask_filter
    uint8_t hw_filter_id = is_range ? filter_id + SOC_TWAI_MASK_FILTER_NUM : filter_id;
    if (en) {
        hw->filter_control_filter_status.val |= TWAIFD_FAFE << (hw_filter_id * TWAIFD_FBNB_S);
    } else {
        hw->filter_control_filter_status.val &= ~(TWAIFD_FAFE << (hw_filter_id * TWAIFD_FBNB_S));
    }
}

/**
 * @brief   Set Bit Acceptance Filter
 * @param hw Start address of the TWAI registers
 * @param filter_id Filter number id
 * @param is_ext Filter for ext_id or std_id
 * @param code Acceptance Code
 * @param mask Acceptance Mask
 */
static inline void twaifd_ll_filter_set_id_mask(twaifd_dev_t* hw, uint8_t filter_id, bool is_ext, uint32_t code, uint32_t mask)
{
    hw->mask_filters[filter_id].filter_mask.bit_mask_val = is_ext ? mask : (mask << TWAIFD_IDENTIFIER_BASE_S);
    hw->mask_filters[filter_id].filter_val.bit_val = is_ext ? code : (code << TWAIFD_IDENTIFIER_BASE_S);
}

/**
 * @brief   Set Range Acceptance Filter
 * @param hw Start address of the TWAI registers
 * @param filter_id Filter number id
 * @param is_ext Filter for ext_id or std_id
 * @param high The id range high limit
 * @param low  The id range low limit
 */
static inline void twaifd_ll_filter_set_range(twaifd_dev_t* hw, uint8_t filter_id, bool is_ext, uint32_t high, uint32_t low)
{
    hw->range_filters[filter_id].ran_low.bit_ran_low_val = is_ext ? low : (low << TWAIFD_IDENTIFIER_BASE_S);
    hw->range_filters[filter_id].ran_high.bit_ran_high_val = is_ext ? high : (high << TWAIFD_IDENTIFIER_BASE_S);
}

/* ------------------------- TX Buffer Registers ------------------------- */

/**
 * @brief Get the number of TX buffers available.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @return The number of TX buffers available.
 */
static inline uint32_t twaifd_ll_get_tx_buffer_quantity(twaifd_dev_t *hw)
{
    return hw->tx_command_txtb_info.txt_buffer_count;
}

/**
 * @brief Get the status of a specific TX buffer.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param buffer_idx Index of the TX buffer (0-7).
 * @return The status of the selected TX buffer.
 */
static inline uint32_t twaifd_ll_get_tx_buffer_status(twaifd_dev_t *hw, uint8_t buffer_idx)
{
    HAL_ASSERT(buffer_idx < twaifd_ll_get_tx_buffer_quantity(hw));  // Ensure buffer index is valid
    uint32_t reg_val = hw->tx_status.val;
    return reg_val & (TWAIFD_TX2S_V << (TWAIFD_TX2S_S * buffer_idx));  // Get status for buffer
}

/**
 * @brief   Set TX Buffer command
 *
 * Setting the TX command will cause the TWAI controller to attempt to transmit
 * the frame stored in the TX buffer. The TX buffer will be occupied (i.e.,
 * locked) until TX completes.
 *
 * @param hw Start address of the TWAI registers
 * @param buffer_idx
 * @param cmd The command want to set, see `TWAIFD_LL_TX_CMD_`
 */
__attribute__((always_inline))
static inline void twaifd_ll_set_tx_cmd(twaifd_dev_t *hw, uint8_t buffer_idx, uint32_t cmd)
{
    hw->tx_command_txtb_info.val = (cmd | BIT(buffer_idx + TWAIFD_TXB1_S));
}

/**
 * @brief Set the priority for a specific TX buffer.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param buffer_idx Index of the TX buffer (0-7).
 * @param priority The priority level to set for the buffer.
 */
static inline void twaifd_ll_set_tx_buffer_priority(twaifd_dev_t *hw, uint8_t buffer_idx, uint32_t priority)
{
    HAL_ASSERT(buffer_idx < twaifd_ll_get_tx_buffer_quantity(hw));  // Ensure buffer index is valid
    uint32_t reg_val = hw->tx_priority.val;
    reg_val &= ~(TWAIFD_TXT1P_V << (TWAIFD_TXT2P_S * buffer_idx));  // Clear old priority
    reg_val |= priority << (TWAIFD_TXT2P_S * buffer_idx);           // Set new priority
    hw->tx_priority.val = reg_val;
}

/**
 * @brief   Copy a formatted TWAI frame into TX buffer for transmission
 *
 * @param hw Start address of the TWAI registers
 * @param tx_frame Pointer to formatted frame
 * @param buffer_idx The tx buffer index to copy in
 *
 * @note Call twaifd_ll_format_frame_header() and twaifd_ll_format_frame_data() to format a frame
 */
__attribute__((always_inline))
static inline void twaifd_ll_mount_tx_buffer(twaifd_dev_t *hw, twaifd_frame_buffer_t *tx_frame, uint8_t buffer_idx)
{
    //Copy formatted frame into TX buffer
    for (int i = 0; i < sizeof(twaifd_frame_buffer_t) / sizeof(uint32_t); i++) {
        hw->txt_mem_cell[buffer_idx].txt_buffer.words[i] = tx_frame->words[i];
    }
}

/* ------------------------- RX Buffer Registers ------------------------- */

/**
 * @brief Get the size of the RX buffer.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @return Size of the RX buffer.
 */
static inline uint32_t twaifd_ll_get_rx_buffer_size(twaifd_dev_t *hw)
{
    return hw->rx_mem_info.rx_buff_size;
}

/**
 * @brief Get the number of frames in the RX buffer.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @return Number of frames in the RX buffer.
 */
__attribute__((always_inline))
static inline uint32_t twaifd_ll_get_rx_frame_count(twaifd_dev_t *hw)
{
    return hw->rx_status_rx_settings.rxfrc;
}

/**
 * @brief Check if the RX FIFO is empty.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @return 1 if RX FIFO is empty, 0 otherwise.
 */
static inline uint32_t twaifd_ll_is_rx_buffer_empty(twaifd_dev_t *hw)
{
    return hw->rx_status_rx_settings.rxe;
}

/**
 * @brief   Copy a received frame from the RX buffer for parsing
 *
 * @param hw Start address of the TWAI registers
 * @param rx_frame Pointer to store formatted frame
 *
 * @note Call twaifd_ll_parse_frame_header() and twaifd_ll_parse_frame_data() to parse the formatted frame
 */
__attribute__((always_inline))
static inline void twaifd_ll_get_rx_frame(twaifd_dev_t *hw, twaifd_frame_buffer_t *rx_frame)
{
    // If rx_automatic_mode enabled, hw->rx_data.rx_data should 32bit access
    rx_frame->words[0] = hw->rx_data.rx_data;
    for (uint8_t i = 1; i <= rx_frame->format.rwcnt; i++) {
        rx_frame->words[i] = hw->rx_data.rx_data;
    }
    HAL_ASSERT(!hw->rx_status_rx_settings.rxmof);
}

/* ------------------------- TWAIFD frame ------------------------- */
/**
 * @brief   Format contents of a TWAI frame header into layout of TX Buffer
 *
 * This function encodes a message into a frame structure. The frame structure
 * has an identical layout to the TX buffer, allowing the frame structure to be
 * directly copied into hardware.
 *
 * @param[in] header Including DLC, ID, Format, etc.
 * @param[in] final_dlc data length code of frame.
 * @param[out] tx_frame Pointer to store formatted frame
 */
__attribute__((always_inline))
static inline void twaifd_ll_format_frame_header(const twai_frame_header_t *header, uint8_t final_dlc, twaifd_frame_buffer_t *tx_frame)
{
    HAL_ASSERT(final_dlc <= TWAIFD_FRAME_MAX_DLC);

    //Set frame information
    tx_frame->format.ide = header->ide;
    tx_frame->format.rtr = header->rtr;
    tx_frame->format.fdf = header->fdf;
    tx_frame->format.brs = header->brs;
    tx_frame->format.dlc = final_dlc;

    tx_frame->timestamp_low = header->timestamp;
    tx_frame->timestamp_high = header->timestamp >> 32;

    if (tx_frame->format.ide) {
        tx_frame->identifier.val = (header->id & TWAI_EXT_ID_MASK);
    } else {
        tx_frame->identifier.identifier_base = (header->id & TWAI_STD_ID_MASK);
    }
}

/**
 * @brief   Format contents of a TWAI data into layout of TX Buffer
 *
 * This function encodes a message into a frame structure. The frame structure
 * has an identical layout to the TX buffer, allowing the frame structure to be
 * directly copied into hardware.
 *
 * @param[in] buffer Pointer to an 8 byte array containing data.
 * @param[in] len data length of data buffer.
 * @param[out] tx_frame Pointer to store formatted frame
 */
__attribute__((always_inline))
static inline void twaifd_ll_format_frame_data(const uint8_t *buffer, uint32_t len, twaifd_frame_buffer_t *tx_frame)
{
    HAL_ASSERT(len <= TWAIFD_FRAME_MAX_LEN);
    memcpy(tx_frame->data, buffer, len);
}

/**
 * @brief   Parse formatted TWAI frame header (RX Buffer Layout) into its constituent contents
 *
 * @param[in] rx_frame Pointer to formatted frame
 * @param[out] p_frame_header Including DLC, ID, Format, etc.
 */
__attribute__((always_inline))
static inline void twaifd_ll_parse_frame_header(const twaifd_frame_buffer_t *rx_frame, twai_frame_header_t *p_frame_header)
{
    //Copy frame information
    p_frame_header->ide = rx_frame->format.ide;
    p_frame_header->rtr = rx_frame->format.rtr;
    p_frame_header->fdf = rx_frame->format.fdf;
    p_frame_header->brs = rx_frame->format.brs;
    p_frame_header->esi = rx_frame->format.esi;
    p_frame_header->dlc = rx_frame->format.dlc;

    p_frame_header->timestamp = rx_frame->timestamp_high;
    p_frame_header->timestamp <<= 32;
    p_frame_header->timestamp |= rx_frame->timestamp_low;

    if (p_frame_header->ide) {
        p_frame_header->id = (rx_frame->identifier.val & TWAI_EXT_ID_MASK);
    } else {
        // No check with 'TWAI_STD_ID_MASK' again due to register `identifier_base` already 11 bits len
        p_frame_header->id = rx_frame->identifier.identifier_base;
    }
}

/**
 * @brief   Parse formatted TWAI data (RX Buffer Layout) into data buffer
 *
 * @param[in] rx_frame Pointer to formatted frame
 * @param[out] buffer Pointer to an 8 byte array to save data
 * @param[in] buffer_len_limit The buffer length limit, If less then frame data length, over length data will dropped
 */
__attribute__((always_inline))
static inline void twaifd_ll_parse_frame_data(const twaifd_frame_buffer_t *rx_frame, uint8_t *buffer, int len_limit)
{
    memcpy(buffer, rx_frame->data, len_limit);
}

/* ------------------------- Tx Rx traffic counters Register ------------------------- */
/**
 * @brief   Get RX Message Counter
 *
 * @param hw Start address of the TWAI registers
 * @return RX Message Counter
 */
static inline uint32_t twaifd_ll_get_rx_traffic_counter(twaifd_dev_t *hw)
{
    return hw->rx_fr_ctr.val;
}

/**
 * @brief   Get TX Message Counter
 *
 * @param hw Start address of the TWAI registers
 * @return TX Message Counter
 */
static inline uint32_t twaifd_ll_get_tx_traffic_counter(twaifd_dev_t *hw)
{
    return hw->tx_fr_ctr.val;
}

/* ------------------------- Timestamp Register ------------------------- */

/**
 * @brief Enable or disable the timer clock.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param enable True to enable, false to disable.
 */
static inline void twaifd_ll_timer_enable_clock(twaifd_dev_t *hw, bool enable)
{
    hw->timer_clk_en.clk_en = enable;
}

/**
 * @brief Enable or disable timer power.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param enable True to enable, false to disable.
 */
static inline void twaifd_ll_timer_enable(twaifd_dev_t *hw, bool enable)
{
    hw->timer_cfg.timer_ce = enable;
}

/**
 * @brief Set the timer step value.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param step Step value to set (actual step = step - 1).
 */
static inline void twaifd_ll_timer_set_step(twaifd_dev_t *hw, uint32_t step)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->timer_cfg, timer_step, (step - 1));
}

/**
 * @brief Set timer mode to up or down counter.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param up True for up counter, false for down counter.
 */
static inline void twaifd_ll_timer_set_direction(twaifd_dev_t *hw, bool up)
{
    hw->timer_cfg.timer_up_dn = up;
}

/**
 * @brief Clear or reset the timer count.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param clear True to clear count, false to set count.
 */
static inline void twaifd_ll_timer_clr_count(twaifd_dev_t *hw, bool clear)
{
    hw->timer_cfg.timer_clr = clear;
}

/**
 * @brief Set the timer preload value.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param load_value 64-bit load value.
 */
static inline void twaifd_ll_timer_set_preload_value(twaifd_dev_t *hw, uint64_t load_value)
{
    hw->timer_ld_val_h.val = (uint32_t)(load_value >> 32);
    hw->timer_ld_val_l.val = (uint32_t) load_value;
}

/**
 * @brief Apply preload value
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 */
static inline void twaifd_ll_timer_apply_preload_value(twaifd_dev_t *hw)
{
    hw->timer_cfg.timer_set = 1;
}

/**
 * @brief Set the timer alarm value.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @param alarm_value 64-bit alarm value.
 */
static inline void twaifd_ll_timer_set_alarm_value(twaifd_dev_t *hw, uint64_t alarm_value)
{
    hw->timer_ct_val_h.val = (uint32_t)(alarm_value >> 32);
    hw->timer_ct_val_l.val = (uint32_t) alarm_value;
}

/**
 * @brief Enable or disable timer interrupt by mask.
 *
 * @param hw Timer Group register base address
 * @param mask Mask of interrupt events
 * @param en True: enable interrupt
 *           False: disable interrupt
 */
static inline void twaifd_ll_timer_enable_intr(twaifd_dev_t *hw, uint32_t mask, bool en)
{
    if (en) {
        hw->timer_int_ena.val |= mask;
    } else {
        hw->timer_int_ena.val &= ~mask;
    }
}

/**
 * @brief Get the current timer interrupt status.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 * @return Current interrupt status.
 */
static inline uint32_t twaifd_ll_timer_get_intr_status(twaifd_dev_t *hw, uint32_t mask)
{
    return hw->timer_int_st.val & mask;
}

/**
 * @brief Clear specific timer interrupts.
 *
 * @param hw Pointer to the TWAI-FD device hardware.
 */
static inline void twaifd_ll_timer_clr_intr_status(twaifd_dev_t *hw, uint32_t mask)
{
    hw->timer_int_clr.val = mask;
}

#ifdef __cplusplus
}
#endif
