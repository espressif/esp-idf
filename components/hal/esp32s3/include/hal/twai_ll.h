/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for TWAI

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "esp_assert.h"
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/twai_types.h"
#include "soc/twai_periph.h"
#include "soc/twai_struct.h"
#include "soc/system_struct.h"

#define TWAI_LL_GET_HW(controller_id) ((controller_id == 0) ? (&TWAI) : NULL)

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------- Defines and Typedefs --------------------------- */

#define TWAI_LL_STATUS_RBS      (0x1 << 0)      //Receive Buffer Status
#define TWAI_LL_STATUS_DOS      (0x1 << 1)      //Data Overrun Status
#define TWAI_LL_STATUS_TBS      (0x1 << 2)      //Transmit Buffer Status
#define TWAI_LL_STATUS_TCS      (0x1 << 3)      //Transmission Complete Status
#define TWAI_LL_STATUS_RS       (0x1 << 4)      //Receive Status
#define TWAI_LL_STATUS_TS       (0x1 << 5)      //Transmit Status
#define TWAI_LL_STATUS_ES       (0x1 << 6)      //Error Status
#define TWAI_LL_STATUS_BS       (0x1 << 7)      //Bus Status
#define TWAI_LL_STATUS_MS       (0x1 << 8)      //Miss Status

#define TWAI_LL_INTR_RI         (0x1 << 0)      //Receive Interrupt
#define TWAI_LL_INTR_TI         (0x1 << 1)      //Transmit Interrupt
#define TWAI_LL_INTR_EI         (0x1 << 2)      //Error Interrupt
//Data overrun interrupt not supported in SW due to HW peculiarities
#define TWAI_LL_INTR_EPI        (0x1 << 5)      //Error Passive Interrupt
#define TWAI_LL_INTR_ALI        (0x1 << 6)      //Arbitration Lost Interrupt
#define TWAI_LL_INTR_BEI        (0x1 << 7)      //Bus Error Interrupt

/*
 * The following frame structure has an NEARLY identical bit field layout to
 * each byte of the TX buffer. This allows for formatting and parsing frames to
 * be done outside of time critical regions (i.e., ISRs). All the ISR needs to
 * do is to copy byte by byte to/from the TX/RX buffer. The two reserved bits in
 * TX buffer are used in the frame structure to store the self_reception and
 * single_shot flags which in turn indicate the type of transmission to execute.
 */
typedef union {
    struct {
        struct {
            uint8_t dlc: 4;             //Data length code (0 to 8) of the frame
            uint8_t self_reception: 1;  //This frame should be transmitted using self reception command
            uint8_t single_shot: 1;     //This frame should be transmitted using single shot command
            uint8_t rtr: 1;             //This frame is a remote transmission request
            uint8_t frame_format: 1;    //Format of the frame (1 = extended, 0 = standard)
        };
        union {
            struct {
                uint8_t id[2];          //11 bit standard frame identifier
                uint8_t data[8];        //Data bytes (0 to 8)
                uint8_t reserved8[2];
            } standard;
            struct {
                uint8_t id[4];          //29 bit extended frame identifier
                uint8_t data[8];        //Data bytes (0 to 8)
            } extended;
        };
    };
    uint8_t bytes[13];
} __attribute__((packed)) twai_ll_frame_buffer_t;

ESP_STATIC_ASSERT(sizeof(twai_ll_frame_buffer_t) == 13, "TX/RX buffer type should be 13 bytes");

/* ---------------------------- Reset and Clock Control ------------------------------ */

/**
 * @brief Enable the bus clock for twai module
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void twai_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    SYSTEM.perip_clk_en0.can_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define twai_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; twai_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the twai module
 *
 * @param group_id Group ID
 */
static inline void twai_ll_reset_register(int group_id)
{
    (void)group_id;
    SYSTEM.perip_rst_en0.can_rst = 1;
    SYSTEM.perip_rst_en0.can_rst = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define twai_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; twai_ll_reset_register(__VA_ARGS__)

/* ---------------------------- Peripheral Control Register ----------------- */

/**
 * @brief Enable TWAI module clock
 *
 * @param hw Start address of the TWAI registers
 * @param en true to enable, false to disable
 */
__attribute__((always_inline))
static inline void twai_ll_enable_clock(twai_dev_t *hw, bool en)
{
    (void)hw;
}

/**
 * @brief Set clock source for TWAI module
 *
 * @param hw Start address of the TWAI registers
 * @param clk_src Clock source
 */
__attribute__((always_inline))
static inline void twai_ll_set_clock_source(twai_dev_t *hw, twai_clock_source_t clk_src)
{
    (void)hw;
    HAL_ASSERT(clk_src == TWAI_CLK_SRC_APB);
}

/* ---------------------------- Mode Register ------------------------------- */

/**
 * @brief   Enter reset mode
 *
 * When in reset mode, the TWAI controller is effectively disconnected from the
 * TWAI bus and will not participate in any bus activates. Reset mode is required
 * in order to write the majority of configuration registers.
 *
 * @param hw Start address of the TWAI registers
 *
 * @note Reset mode is automatically entered on BUS OFF condition
 */
__attribute__((always_inline))
static inline void twai_ll_enter_reset_mode(twai_dev_t *hw)
{
    hw->mode_reg.rm = 1;
}

/**
 * @brief   Exit reset mode
 *
 * When not in reset mode, the TWAI controller will take part in bus activities
 * (e.g., send/receive/acknowledge messages and error frames) depending on the
 * operating mode.
 *
 * @param hw Start address of the TWAI registers
 *
 * @note Reset mode must be exit to initiate BUS OFF recovery
 */
__attribute__((always_inline))
static inline void twai_ll_exit_reset_mode(twai_dev_t *hw)
{
    hw->mode_reg.rm = 0;
}

/**
 * @brief   Check if in reset mode
 * @param hw Start address of the TWAI registers
 * @return true if in reset mode
 */
__attribute__((always_inline))
static inline bool twai_ll_is_in_reset_mode(twai_dev_t *hw)
{
    return hw->mode_reg.rm;
}

/**
 * @brief   Set operating mode of TWAI controller
 *
 * @param hw Start address of the TWAI registers
 * @param mode Operating mode
 *
 * @note Must be called in reset mode
 */
__attribute__((always_inline))
static inline void twai_ll_set_mode(twai_dev_t *hw, twai_mode_t mode)
{
    if (mode == TWAI_MODE_NORMAL) {           //Normal Operating mode
        hw->mode_reg.lom = 0;
        hw->mode_reg.stm = 0;
    } else if (mode == TWAI_MODE_NO_ACK) {    //Self Test Mode (No Ack)
        hw->mode_reg.lom = 0;
        hw->mode_reg.stm = 1;
    } else if (mode == TWAI_MODE_LISTEN_ONLY) {       //Listen Only Mode
        hw->mode_reg.lom = 1;
        hw->mode_reg.stm = 0;
    }
}

/* --------------------------- Command Register ----------------------------- */

/**
 * @brief   Set TX command
 *
 * Setting the TX command will cause the TWAI controller to attempt to transmit
 * the frame stored in the TX buffer. The TX buffer will be occupied (i.e.,
 * locked) until TX completes.
 *
 * @param hw Start address of the TWAI registers
 *
 * @note Transmit commands should be called last (i.e., after handling buffer
 *       release and clear data overrun) in order to prevent the other commands
 *       overwriting this latched TX bit with 0.
 */
__attribute__((always_inline))
static inline void twai_ll_set_cmd_tx(twai_dev_t *hw)
{
    hw->command_reg.tr = 1;
}

/**
 * @brief   Set single shot TX command
 *
 * Similar to setting TX command, but the TWAI controller will not automatically
 * retry transmission upon an error (e.g., due to an acknowledgement error).
 *
 * @param hw Start address of the TWAI registers
 *
 * @note Transmit commands should be called last (i.e., after handling buffer
 *       release and clear data overrun) in order to prevent the other commands
 *       overwriting this latched TX bit with 0.
 */
__attribute__((always_inline))
static inline void twai_ll_set_cmd_tx_single_shot(twai_dev_t *hw)
{
    hw->command_reg.val = 0x03; //Set command_reg.tr and command_reg.at simultaneously for single shot transmittion request
}

/**
 * @brief   Aborts TX
 *
 * Frames awaiting TX will be aborted. Frames already being TX are not aborted.
 * Transmission Complete Status bit is automatically set to 1.
 * Similar to setting TX command, but the TWAI controller will not automatically
 * retry transmission upon an error (e.g., due to acknowledge error).
 *
 * @param hw Start address of the TWAI registers
 *
 * @note Transmit commands should be called last (i.e., after handling buffer
 *       release and clear data overrun) in order to prevent the other commands
 *       overwriting this latched TX bit with 0.
 */
__attribute__((always_inline))
static inline void twai_ll_set_cmd_abort_tx(twai_dev_t *hw)
{
    hw->command_reg.at = 1;
}

/**
 * @brief   Release RX buffer
 *
 * Rotates RX buffer to the next frame in the RX FIFO.
 *
 * @param hw Start address of the TWAI registers
 */
__attribute__((always_inline))
static inline void twai_ll_set_cmd_release_rx_buffer(twai_dev_t *hw)
{
    hw->command_reg.rrb = 1;
}

/**
 * @brief   Clear data overrun
 *
 * Clears the data overrun status bit
 *
 * @param hw Start address of the TWAI registers
 */
__attribute__((always_inline))
static inline void twai_ll_set_cmd_clear_data_overrun(twai_dev_t *hw)
{
    hw->command_reg.cdo = 1;
}

/**
 * @brief   Set self reception single shot command
 *
 * Similar to setting TX command, but the TWAI controller also simultaneously
 * receive the transmitted frame and is generally used for self testing
 * purposes. The TWAI controller will not ACK the received message, so consider
 * using the NO_ACK operating mode.
 *
 * @param hw Start address of the TWAI registers
 *
 * @note Transmit commands should be called last (i.e., after handling buffer
 *       release and clear data overrun) in order to prevent the other commands
 *       overwriting this latched TX bit with 0.
 */
__attribute__((always_inline))
static inline void twai_ll_set_cmd_self_rx_request(twai_dev_t *hw)
{
    hw->command_reg.srr = 1;
}

/**
 * @brief   Set self reception request command
 *
 * Similar to setting the self reception request, but the TWAI controller will
 * not automatically retry transmission upon an error (e.g., due to and
 * acknowledgement error).
 *
 * @param hw Start address of the TWAI registers
 *
 * @note Transmit commands should be called last (i.e., after handling buffer
 *       release and clear data overrun) in order to prevent the other commands
 *       overwriting this latched TX bit with 0.
 */
__attribute__((always_inline))
static inline void twai_ll_set_cmd_self_rx_single_shot(twai_dev_t *hw)
{
    hw->command_reg.val = 0x12; //Set command_reg.srr and command_reg.at simultaneously for single shot self reception request
}

/* --------------------------- Status Register ------------------------------ */

/**
 * @brief   Get all status bits
 *
 * @param hw Start address of the TWAI registers
 * @return Status bits
 */
__attribute__((always_inline))
static inline uint32_t twai_ll_get_status(twai_dev_t *hw)
{
    return hw->status_reg.val;
}

/**
 * @brief   Check if RX FIFO overrun status bit is set
 *
 * @param hw Start address of the TWAI registers
 * @return Overrun status bit
 */
__attribute__((always_inline))
static inline bool twai_ll_is_fifo_overrun(twai_dev_t *hw)
{
    return hw->status_reg.dos;
}

/**
 * @brief   Check if previously TX was successful
 *
 * @param hw Start address of the TWAI registers
 * @return Whether previous TX was successful
 */
__attribute__((always_inline))
static inline bool twai_ll_is_last_tx_successful(twai_dev_t *hw)
{
    return hw->status_reg.tcs;
}

/* -------------------------- Interrupt Register ---------------------------- */

/**
 * @brief   Get currently set interrupts
 *
 * Reading the interrupt registers will automatically clear all interrupts
 * except for the Receive Interrupt.
 *
 * @param hw Start address of the TWAI registers
 * @return Bit mask of set interrupts
 */
__attribute__((always_inline))
static inline uint32_t twai_ll_get_and_clear_intrs(twai_dev_t *hw)
{
    return hw->interrupt_reg.val;
}

/* ----------------------- Interrupt Enable Register ------------------------ */

/**
 * @brief   Set which interrupts are enabled
 *
 * @param hw Start address of the TWAI registers
 * @param Bit mask of interrupts to enable
 *
 * @note Must be called in reset mode
 */
__attribute__((always_inline))
static inline void twai_ll_set_enabled_intrs(twai_dev_t *hw, uint32_t intr_mask)
{
    hw->interrupt_enable_reg.val = intr_mask;
}

/* ------------------------ Bus Timing Registers --------------------------- */

/**
 * @brief Check if the brp value valid
 *
 * @param brp Bit rate prescaler value
 * @return true or False
 */
__attribute__((always_inline))
static inline bool twai_ll_check_brp_validation(uint32_t brp)
{
    bool valid = (brp >= SOC_TWAI_BRP_MIN) && (brp <= SOC_TWAI_BRP_MAX);
    // should be an even number
    valid = valid && !(brp & 0x01);
    return valid;
}

/**
 * @brief   Set bus timing
 *
 * @param hw Start address of the TWAI registers
 * @param brp Baud Rate Prescaler
 * @param sjw Synchronization Jump Width
 * @param tseg1 Timing Segment 1
 * @param tseg2 Timing Segment 2
 * @param triple_sampling Triple Sampling enable/disable
 *
 * @note Must be called in reset mode
 * @note ESP32S3 brp can be any even number between 2 to 32768
 */
__attribute__((always_inline))
static inline void twai_ll_set_bus_timing(twai_dev_t *hw, uint32_t brp, uint32_t sjw, uint32_t tseg1, uint32_t tseg2, bool triple_sampling)
{
    hw->bus_timing_0_reg.brp = (brp / 2) - 1;
    hw->bus_timing_0_reg.sjw = sjw - 1;
    hw->bus_timing_1_reg.tseg1 = tseg1 - 1;
    hw->bus_timing_1_reg.tseg2 = tseg2 - 1;
    hw->bus_timing_1_reg.sam = triple_sampling;
}

/* ----------------------------- ALC Register ------------------------------- */

/**
 * @brief   Clear Arbitration Lost Capture Register
 *
 * Reading the ALC register rearms the Arbitration Lost Interrupt
 *
 * @param hw Start address of the TWAI registers
 */
__attribute__((always_inline))
static inline void twai_ll_clear_arb_lost_cap(twai_dev_t *hw)
{
    (void)hw->arbitration_lost_captue_reg.val;
}

/* ----------------------------- ECC Register ------------------------------- */

/**
 * @brief   Clear Error Code Capture register
 *
 * Reading the ECC register rearms the Bus Error Interrupt
 *
 * @param hw Start address of the TWAI registers
 */
__attribute__((always_inline))
static inline void twai_ll_clear_err_code_cap(twai_dev_t *hw)
{
    (void)hw->error_code_capture_reg.val;
}

/* ----------------------------- EWL Register ------------------------------- */

/**
 * @brief   Set Error Warning Limit
 *
 * @param hw Start address of the TWAI registers
 * @param ewl Error Warning Limit
 *
 * @note Must be called in reset mode
 */
__attribute__((always_inline))
static inline void twai_ll_set_err_warn_lim(twai_dev_t *hw, uint32_t ewl)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->error_warning_limit_reg, ewl, ewl);
}

/**
 * @brief   Get Error Warning Limit
 *
 * @param hw Start address of the TWAI registers
 * @return Error Warning Limit
 */
__attribute__((always_inline))
static inline uint32_t twai_ll_get_err_warn_lim(twai_dev_t *hw)
{
    return hw->error_warning_limit_reg.val;
}

/* ------------------------ RX Error Count Register ------------------------- */

/**
 * @brief   Get RX Error Counter
 *
 * @param hw Start address of the TWAI registers
 * @return REC value
 *
 * @note REC is not frozen in reset mode. Listen only mode will freeze it. A BUS
 *       OFF condition automatically sets the REC to 0.
 */
__attribute__((always_inline))
static inline uint32_t twai_ll_get_rec(twai_dev_t *hw)
{
    return hw->rx_error_counter_reg.val;
}

/**
 * @brief   Set RX Error Counter
 *
 * @param hw Start address of the TWAI registers
 * @param rec REC value
 *
 * @note Must be called in reset mode
 */
__attribute__((always_inline))
static inline void twai_ll_set_rec(twai_dev_t *hw, uint32_t rec)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->rx_error_counter_reg, rxerr, rec);
}

/* ------------------------ TX Error Count Register ------------------------- */

/**
 * @brief   Get TX Error Counter
 *
 * @param hw Start address of the TWAI registers
 * @return TEC value
 *
 * @note A BUS OFF condition will automatically set this to 128
 */
__attribute__((always_inline))
static inline uint32_t twai_ll_get_tec(twai_dev_t *hw)
{
    return hw->tx_error_counter_reg.val;
}

/**
 * @brief   Set TX Error Counter
 *
 * @param hw Start address of the TWAI registers
 * @param tec TEC value
 *
 * @note Must be called in reset mode
 */
__attribute__((always_inline))
static inline void twai_ll_set_tec(twai_dev_t *hw, uint32_t tec)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->tx_error_counter_reg, txerr, tec);
}

/* ---------------------- Acceptance Filter Registers ----------------------- */

/**
 * @brief   Set Acceptance Filter
 * @param hw Start address of the TWAI registers
 * @param code Acceptance Code
 * @param mask Acceptance Mask
 * @param single_filter Whether to enable single filter mode
 *
 * @note Must be called in reset mode
 */
__attribute__((always_inline))
static inline void twai_ll_set_acc_filter(twai_dev_t *hw, uint32_t code, uint32_t mask, bool single_filter)
{
    uint32_t code_swapped = HAL_SWAP32(code);
    uint32_t mask_swapped = HAL_SWAP32(mask);
    for (int i = 0; i < 4; i++) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->acceptance_filter.acr[i], byte, ((code_swapped >> (i * 8)) & 0xFF));
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->acceptance_filter.amr[i], byte, ((mask_swapped >> (i * 8)) & 0xFF));
    }
    hw->mode_reg.afm = single_filter;
}

/* ------------------------- TX/RX Buffer Registers ------------------------- */

/**
 * @brief   Copy a formatted TWAI frame into TX buffer for transmission
 *
 * @param hw Start address of the TWAI registers
 * @param tx_frame Pointer to formatted frame
 *
 * @note Call twai_ll_format_frame_buffer() to format a frame
 */
__attribute__((always_inline))
static inline void twai_ll_set_tx_buffer(twai_dev_t *hw, twai_ll_frame_buffer_t *tx_frame)
{
    //Copy formatted frame into TX buffer
    for (int i = 0; i < 13; i++) {
        hw->tx_rx_buffer[i].val = tx_frame->bytes[i];
    }
}

/**
 * @brief   Copy a received frame from the RX buffer for parsing
 *
 * @param hw Start address of the TWAI registers
 * @param rx_frame Pointer to store formatted frame
 *
 * @note Call twai_ll_parse_frame_buffer() to parse the formatted frame
 */
__attribute__((always_inline))
static inline void twai_ll_get_rx_buffer(twai_dev_t *hw, twai_ll_frame_buffer_t *rx_frame)
{
    //Copy RX buffer registers into frame
    for (int i = 0; i < 13; i++) {
        rx_frame->bytes[i] =  HAL_FORCE_READ_U32_REG_FIELD(hw->tx_rx_buffer[i], byte);
    }
}

/**
 * @brief   Format contents of a TWAI frame into layout of TX Buffer
 *
 * This function encodes a message into a frame structure. The frame structure
 * has an identical layout to the TX buffer, allowing the frame structure to be
 * directly copied into TX buffer.
 *
 * @param[in] 11bit or 29bit ID
 * @param[in] dlc Data length code
 * @param[in] data Pointer to an 8 byte array containing data. NULL if no data
 * @param[in] format Type of TWAI frame
 * @param[in] single_shot Frame will not be retransmitted on failure
 * @param[in] self_rx Frame will also be simultaneously received
 * @param[out] tx_frame Pointer to store formatted frame
 */
__attribute__((always_inline))
static inline void twai_ll_format_frame_buffer(uint32_t id, uint8_t dlc, const uint8_t *data,
                                              uint32_t flags, twai_ll_frame_buffer_t *tx_frame)
{
    bool is_extd = flags & TWAI_MSG_FLAG_EXTD;
    bool is_rtr = flags & TWAI_MSG_FLAG_RTR;

    //Set frame information
    tx_frame->dlc = dlc;
    tx_frame->frame_format = is_extd;
    tx_frame->rtr = is_rtr;
    tx_frame->self_reception = (flags & TWAI_MSG_FLAG_SELF) ? 1 : 0;
    tx_frame->single_shot = (flags & TWAI_MSG_FLAG_SS) ? 1 : 0;

    //Set ID. The ID registers are big endian and left aligned, therefore a bswap will be required
    if (is_extd) {
        uint32_t id_temp = HAL_SWAP32((id & TWAI_EXTD_ID_MASK) << 3); //((id << 3) >> 8*(3-i))
        for (int i = 0; i < 4; i++) {
            tx_frame->extended.id[i] = (id_temp >> (8 * i)) & 0xFF;
        }
    } else {
        uint32_t id_temp =  HAL_SWAP16((id & TWAI_STD_ID_MASK) << 5); //((id << 5) >> 8*(1-i))
        for (int i = 0; i < 2; i++) {
            tx_frame->standard.id[i] = (id_temp >> (8 * i)) & 0xFF;
        }
    }

    uint8_t *data_buffer = (is_extd) ? tx_frame->extended.data : tx_frame->standard.data;
    if (!is_rtr) {  //Only copy data if the frame is a data frame (i.e not a remote frame)
        for (int i = 0; (i < dlc) && (i < TWAI_FRAME_MAX_DLC); i++) {
            data_buffer[i] = data[i];
        }
    }
}

/**
 * @brief   Parse formatted TWAI frame (RX Buffer Layout) into its constituent contents
 *
 * @param[in] rx_frame Pointer to formatted frame
 * @param[out] id 11 or 29bit ID
 * @param[out] dlc Data length code
 * @param[out] data Data. Left over bytes set to 0.
 * @param[out] format Type of TWAI frame
 */
__attribute__((always_inline))
static inline void twai_ll_parse_frame_buffer(twai_ll_frame_buffer_t *rx_frame, uint32_t *id, uint8_t *dlc,
                                             uint8_t *data, uint32_t *flags)
{
    //Copy frame information
    *dlc = rx_frame->dlc;
    uint32_t flags_temp = 0;
    flags_temp |= (rx_frame->frame_format) ? TWAI_MSG_FLAG_EXTD : 0;
    flags_temp |= (rx_frame->rtr) ? TWAI_MSG_FLAG_RTR : 0;
    flags_temp |= (rx_frame->dlc > TWAI_FRAME_MAX_DLC) ? TWAI_MSG_FLAG_DLC_NON_COMP : 0;
    *flags = flags_temp;

    //Copy ID. The ID registers are big endian and left aligned, therefore a bswap will be required
    if (rx_frame->frame_format) {
        uint32_t id_temp = 0;
        for (int i = 0; i < 4; i++) {
            id_temp |= rx_frame->extended.id[i] << (8 * i);
        }
        id_temp = HAL_SWAP32(id_temp) >> 3;  //((byte[i] << 8*(3-i)) >> 3)
        *id = id_temp & TWAI_EXTD_ID_MASK;
    } else {
        uint32_t id_temp = 0;
        for (int i = 0; i < 2; i++) {
            id_temp |= rx_frame->standard.id[i] << (8 * i);
        }
        id_temp = HAL_SWAP16(id_temp) >> 5;  //((byte[i] << 8*(1-i)) >> 5)
        *id = id_temp & TWAI_STD_ID_MASK;
    }

    uint8_t *data_buffer = (rx_frame->frame_format) ? rx_frame->extended.data : rx_frame->standard.data;
    //Only copy data if the frame is a data frame (i.e. not a remote frame)
    int data_length = (rx_frame->rtr) ? 0 : ((rx_frame->dlc > TWAI_FRAME_MAX_DLC) ? TWAI_FRAME_MAX_DLC : rx_frame->dlc);
    for (int i = 0; i < data_length; i++) {
        data[i] = data_buffer[i];
    }
    //Set remaining bytes of data to 0
    for (int i = data_length; i < TWAI_FRAME_MAX_DLC; i++) {
        data[i] = 0;
    }
}

/* ----------------------- RX Message Count Register ------------------------ */

/**
 * @brief   Get RX Message Counter
 *
 * @param hw Start address of the TWAI registers
 * @return RX Message Counter
 */
__attribute__((always_inline))
static inline uint32_t twai_ll_get_rx_msg_count(twai_dev_t *hw)
{
    return hw->rx_message_counter_reg.val;
}

/* ------------------------- Clock Divider Register ------------------------- */

/**
 * @brief   Set CLKOUT Divider and enable/disable
 *
 * Configure CLKOUT. CLKOUT is a pre-scaled version of APB CLK. Divider can be
 * 1, or any even number from 2 to 490. Set the divider to 0 to disable CLKOUT.
 *
 * @param hw Start address of the TWAI registers
 * @param divider Divider for CLKOUT (any even number from 2 to 490). Set to 0 to disable CLKOUT
 */
__attribute__((always_inline))
static inline void twai_ll_set_clkout(twai_dev_t *hw, uint32_t divider)
{
    if (divider >= 2 && divider <= 490) {
        hw->clock_divider_reg.co = 0;
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->clock_divider_reg, cd, (divider / 2) - 1);
    } else if (divider == 1) {
        //Setting the divider reg to max value (255) means a divider of 1
        hw->clock_divider_reg.co = 0;
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->clock_divider_reg, cd, 255);
    } else {
        hw->clock_divider_reg.co = 1;
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->clock_divider_reg, cd, 0);
    }
}

#ifdef __cplusplus
}
#endif
