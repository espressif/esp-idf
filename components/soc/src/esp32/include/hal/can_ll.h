// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for CAN

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal/can_types.h"
#include "soc/can_periph.h"

/* ------------------------- Defines and Typedefs --------------------------- */

#define CAN_LL_STATUS_RBS       (0x1 << 0)
#define CAN_LL_STATUS_DOS       (0x1 << 1)
#define CAN_LL_STATUS_TBS       (0x1 << 2)
#define CAN_LL_STATUS_TCS       (0x1 << 3)
#define CAN_LL_STATUS_RS        (0x1 << 4)
#define CAN_LL_STATUS_TS        (0x1 << 5)
#define CAN_LL_STATUS_ES        (0x1 << 6)
#define CAN_LL_STATUS_BS        (0x1 << 7)

#define CAN_LL_INTR_RI          (0x1 << 0)
#define CAN_LL_INTR_TI          (0x1 << 1)
#define CAN_LL_INTR_EI          (0x1 << 2)
//Data overrun interrupt not supported in SW due to HW peculiarities
#define CAN_LL_INTR_EPI         (0x1 << 5)
#define CAN_LL_INTR_ALI         (0x1 << 6)
#define CAN_LL_INTR_BEI         (0x1 << 7)

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
} __attribute__((packed)) can_ll_frame_buffer_t;

/* ---------------------------- Mode Register ------------------------------- */

/**
 * @brief   Enter reset mode
 *
 * When in reset mode, the CAN controller is effectively disconnected from the
 * CAN bus and will not participate in any bus activates. Reset mode is required
 * in order to write the majority of configuration registers.
 *
 * @param hw Start address of the CAN registers
 * @return true if reset mode was entered successfully
 *
 * @note Reset mode is automatically entered on BUS OFF condition
 */
static inline bool can_ll_enter_reset_mode(can_dev_t *hw)
{
    hw->mode_reg.rm = 1;
    return hw->mode_reg.rm;
}

/**
 * @brief   Exit reset mode
 *
 * When not in reset mode, the CAN controller will take part in bus activities
 * (e.g., send/receive/acknowledge messages and error frames) depending on the
 * operating mode.
 *
 * @param hw Start address of the CAN registers
 * @return true if reset mode was exit successfully
 *
 * @note Reset mode must be exit to initiate BUS OFF recovery
 */
static inline bool can_ll_exit_reset_mode(can_dev_t *hw)
{
    hw->mode_reg.rm = 0;
    return !(hw->mode_reg.rm);
}

/**
 * @brief   Check if in reset mode
 * @param hw Start address of the CAN registers
 * @return true if in reset mode
 */
static inline bool can_ll_is_in_reset_mode(can_dev_t *hw)
{
    return hw->mode_reg.rm;
}

/**
 * @brief   Set operating mode of CAN controller
 * 
 * @param hw Start address of the CAN registers
 * @param mode Operating mode
 * 
 * @note Must be called in reset mode
 */
static inline void can_ll_set_mode(can_dev_t *hw, can_mode_t mode)
{
    if (mode == CAN_MODE_NORMAL) {           //Normal Operating mode
        hw->mode_reg.lom = 0;
        hw->mode_reg.stm = 0;
    } else if (mode == CAN_MODE_NO_ACK) {    //Self Test Mode (No Ack)
        hw->mode_reg.lom = 0;
        hw->mode_reg.stm = 1;
    } else if (mode == CAN_MODE_LISTEN_ONLY) {       //Listen Only Mode
        hw->mode_reg.lom = 1;
        hw->mode_reg.stm = 0;
    }
}

/* --------------------------- Command Register ----------------------------- */

/**
 * @brief   Set TX command
 *
 * Setting the TX command will cause the CAN controller to attempt to transmit
 * the frame stored in the TX buffer. The TX buffer will be occupied (i.e.,
 * locked) until TX completes.
 *
 * @param hw Start address of the CAN registers
 *
 * @note Transmit commands should be called last (i.e., after handling buffer
 *       release and clear data overrun) in order to prevent the other commands
 *       overwriting this latched TX bit with 0.
 */
static inline void can_ll_set_cmd_tx(can_dev_t *hw)
{
    hw->command_reg.tr = 1;
}

/**
 * @brief   Set single shot TX command
 *
 * Similar to setting TX command, but the CAN controller will not automatically
 * retry transmission upon an error (e.g., due to an acknowledgement error).
 *
 * @param hw Start address of the CAN registers
 *
 * @note Transmit commands should be called last (i.e., after handling buffer
 *       release and clear data overrun) in order to prevent the other commands
 *       overwriting this latched TX bit with 0.
 */
static inline void can_ll_set_cmd_tx_single_shot(can_dev_t *hw)
{
    hw->command_reg.val = 0x03;     //Writing to TR and AT simultaneously
}

/**
 * @brief   Aborts TX
 *
 * Frames awaiting TX will be aborted. Frames already being TX are not aborted.
 * Transmission Complete Status bit is automatically set to 1.
 * Similar to setting TX command, but the CAN controller will not automatically
 * retry transmission upon an error (e.g., due to acknowledge error).
 *
 * @param hw Start address of the CAN registers
 *
 * @note Transmit commands should be called last (i.e., after handling buffer
 *       release and clear data overrun) in order to prevent the other commands
 *       overwriting this latched TX bit with 0.
 */
static inline void can_ll_set_cmd_abort_tx(can_dev_t *hw)
{
    hw->command_reg.at = 1;
}

/**
 * @brief   Release RX buffer
 *
 * Rotates RX buffer to the next frame in the RX FIFO.
 *
 * @param hw Start address of the CAN registers
 */
static inline void can_ll_set_cmd_release_rx_buffer(can_dev_t *hw)
{
    hw->command_reg.rrb = 1;
}

/**
 * @brief   Clear data overrun
 *
 * Clears the data overrun status bit
 *
 * @param hw Start address of the CAN registers
 */
static inline void can_ll_set_cmd_clear_data_overrun(can_dev_t *hw)
{
    hw->command_reg.cdo = 1;
}

/**
 * @brief   Set self reception single shot command
 *
 * Similar to setting TX command, but the CAN controller also simultaneously
 * receive the transmitted frame and is generally used for self testing
 * purposes. The CAN controller will not ACK the received message, so consider
 * using the NO_ACK operating mode.
 *
 * @param hw Start address of the CAN registers
 *
 * @note Transmit commands should be called last (i.e., after handling buffer
 *       release and clear data overrun) in order to prevent the other commands
 *       overwriting this latched TX bit with 0.
 */
static inline void can_ll_set_cmd_self_rx_request(can_dev_t *hw)
{
    hw->command_reg.srr = 1;
}

/**
 * @brief   Set self reception request command
 *
 * Similar to setting the self reception request, but the CAN controller will
 * not automatically retry transmission upon an error (e.g., due to and
 * acknowledgement error).
 *
 * @param hw Start address of the CAN registers
 *
 * @note Transmit commands should be called last (i.e., after handling buffer
 *       release and clear data overrun) in order to prevent the other commands
 *       overwriting this latched TX bit with 0.
 */
static inline void can_ll_set_cmd_self_rx_single_shot(can_dev_t *hw)
{
    hw->command_reg.val = 0x12;
}

/* --------------------------- Status Register ------------------------------ */

/**
 * @brief   Get all status bits
 *
 * @param hw Start address of the CAN registers
 * @return Status bits
 */
static inline uint32_t can_ll_get_status(can_dev_t *hw)
{
    return hw->status_reg.val;
}

/**
 * @brief   Check if RX FIFO overrun status bit is set
 *
 * @param hw Start address of the CAN registers
 * @return Overrun status bit
 */
static inline bool can_ll_is_fifo_overrun(can_dev_t *hw)
{
    return hw->status_reg.dos;
}

/**
 * @brief   Check if previously TX was successful
 *
 * @param hw Start address of the CAN registers
 * @return Whether previous TX was successful
 */
static inline bool can_ll_is_last_tx_successful(can_dev_t *hw)
{
    return hw->status_reg.tcs;
}

//Todo: Add stand alone status bit check functions when necessary

/* -------------------------- Interrupt Register ---------------------------- */

/**
 * @brief   Get currently set interrupts
 *
 * Reading the interrupt registers will automatically clear all interrupts
 * except for the Receive Interrupt.
 *
 * @param hw Start address of the CAN registers
 * @return Bit mask of set interrupts
 */
static inline uint32_t can_ll_get_and_clear_intrs(can_dev_t *hw)
{
    return hw->interrupt_reg.val;
}

/* ----------------------- Interrupt Enable Register ------------------------ */

/**
 * @brief   Set which interrupts are enabled
 *
 * @param hw Start address of the CAN registers
 * @param Bit mask of interrupts to enable
 *
 * @note Must be called in reset mode
 */
static inline void can_ll_set_enabled_intrs(can_dev_t *hw, uint32_t intr_mask)
{
#ifdef CAN_BRP_DIV_SUPPORTED
    //ESP32 Rev 2 has brp div. Need to mask when setting
    hw->interrupt_enable_reg.val = (hw->interrupt_enable_reg.val & 0x10) | intr_mask;
#else
    hw->interrupt_enable_reg.val = intr_mask;
#endif
}

/* ------------------------ Bus Timing Registers --------------------------- */

/**
 * @brief   Set bus timing
 *
 * @param hw Start address of the CAN registers
 * @param brp Baud Rate Prescaler
 * @param sjw Synchronization Jump Width
 * @param tseg1 Timing Segment 1
 * @param tseg2 Timing Segment 2
 * @param triple_sampling Triple Sampling enable/disable
 *
 * @note Must be called in reset mode
 * @note ESP32 rev 2 or later can support a x2 brp by setting a brp_div bit,
 *       allowing the brp to go from a maximum of 128 to 256.
 */
static inline void can_ll_set_bus_timing(can_dev_t *hw, uint32_t brp, uint32_t sjw, uint32_t tseg1, uint32_t tseg2, bool triple_sampling)
{
#ifdef CAN_BRP_DIV_SUPPORTED
    if (brp > CAN_BRP_DIV_THRESH) {
        //Need to set brp_div bit
        hw->interrupt_enable_reg.brp_div = 1;
        brp /= 2;
    }
#endif
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
 * @param hw Start address of the CAN registers
 */
static inline void can_ll_clear_arb_lost_cap(can_dev_t *hw)
{
    (void)hw->arbitration_lost_captue_reg.val;
    //Todo: Decode ALC register
}

/* ----------------------------- ECC Register ------------------------------- */

/**
 * @brief   Clear Error Code Capture register
 *
 * Reading the ECC register rearms the Bus Error Interrupt
 *
 * @param hw Start address of the CAN registers
 */
static inline void can_ll_clear_err_code_cap(can_dev_t *hw)
{
    (void)hw->error_code_capture_reg.val;
    //Todo: Decode error code capture
}

/* ----------------------------- EWL Register ------------------------------- */

/**
 * @brief   Set Error Warning Limit
 *
 * @param hw Start address of the CAN registers
 * @param ewl Error Warning Limit
 *
 * @note Must be called in reset mode
 */
static inline void can_ll_set_err_warn_lim(can_dev_t *hw, uint32_t ewl)
{
    hw->error_warning_limit_reg.ewl = ewl;
}

/**
 * @brief   Get Error Warning Limit
 *
 * @param hw Start address of the CAN registers
 * @return Error Warning Limit
 */
static inline uint32_t can_ll_get_err_warn_lim(can_dev_t *hw)
{
    return hw->error_warning_limit_reg.val;
}

/* ------------------------ RX Error Count Register ------------------------- */

/**
 * @brief   Get RX Error Counter
 *
 * @param hw Start address of the CAN registers
 * @return REC value
 *
 * @note REC is not frozen in reset mode. Listen only mode will freeze it. A BUS
 *       OFF condition automatically sets the REC to 0.
 */
static inline uint32_t can_ll_get_rec(can_dev_t *hw)
{
    return hw->rx_error_counter_reg.val;
}

/**
 * @brief   Set RX Error Counter
 *
 * @param hw Start address of the CAN registers
 * @param rec REC value
 *
 * @note Must be called in reset mode
 */
static inline void can_ll_set_rec(can_dev_t *hw, uint32_t rec)
{
    hw->rx_error_counter_reg.rxerr = rec;
}

/* ------------------------ TX Error Count Register ------------------------- */

/**
 * @brief   Get TX Error Counter
 *
 * @param hw Start address of the CAN registers
 * @return TEC value
 *
 * @note A BUS OFF condition will automatically set this to 128
 */
static inline uint32_t can_ll_get_tec(can_dev_t *hw)
{
    return hw->tx_error_counter_reg.val;
}

/**
 * @brief   Set TX Error Counter
 *
 * @param hw Start address of the CAN registers
 * @param tec TEC value
 *
 * @note Must be called in reset mode
 */
static inline void can_ll_set_tec(can_dev_t *hw, uint32_t tec)
{
    hw->tx_error_counter_reg.txerr = tec;
}

/* ---------------------- Acceptance Filter Registers ----------------------- */

/**
 * @brief   Set Acceptance Filter
 * @param hw Start address of the CAN registers
 * @param code Acceptance Code
 * @param mask Acceptance Mask
 * @param single_filter Whether to enable single filter mode
 *
 * @note Must be called in reset mode
 */
static inline void can_ll_set_acc_filter(can_dev_t* hw, uint32_t code, uint32_t mask, bool single_filter)
{
    uint32_t code_swapped = __builtin_bswap32(code);
    uint32_t mask_swapped = __builtin_bswap32(mask);
    for (int i = 0; i < 4; i++) {
        hw->acceptance_filter.acr[i].byte = ((code_swapped >> (i * 8)) & 0xFF);
        hw->acceptance_filter.amr[i].byte = ((mask_swapped >> (i * 8)) & 0xFF);
    }
    hw->mode_reg.afm = single_filter;
}

/* ------------------------- TX/RX Buffer Registers ------------------------- */

/**
 * @brief   Copy a formatted CAN frame into TX buffer for transmission
 *
 * @param hw Start address of the CAN registers
 * @param tx_frame Pointer to formatted frame
 *
 * @note Call can_ll_format_frame_buffer() to format a frame
 */
static inline void can_ll_set_tx_buffer(can_dev_t *hw, can_ll_frame_buffer_t *tx_frame)
{
    //Copy formatted frame into TX buffer
    for (int i = 0; i < 13; i++) {
        hw->tx_rx_buffer[i].val = tx_frame->bytes[i];
    }
}

/**
 * @brief   Copy a received frame from the RX buffer for parsing
 *
 * @param hw Start address of the CAN registers
 * @param rx_frame Pointer to store formatted frame
 *
 * @note Call can_ll_prase_frame_buffer() to parse the formatted frame
 */
static inline void can_ll_get_rx_buffer(can_dev_t *hw, can_ll_frame_buffer_t *rx_frame)
{
    //Copy RX buffer registers into frame
    for (int i = 0; i < 13; i++) {
        rx_frame->bytes[i] =  hw->tx_rx_buffer[i].byte;
    }
}

/**
 * @brief   Format contents of a CAN frame into layout of TX Buffer
 *
 * @param[in] id 11 or 29bit ID
 * @param[in] dlc Data length code
 * @param[in] data Pointer to an 8 byte array containing data. NULL if no data
 * @param[in] format Type of CAN frame
 * @param[in] single_shot Frame will not be retransmitted on failure
 * @param[in] self_rx Frame will also be simultaneously received
 * @param[out] tx_frame Pointer to store formatted frame
 */
static inline void can_ll_format_frame_buffer(uint32_t id, uint8_t dlc, const uint8_t *data,
                                              uint32_t flags, can_ll_frame_buffer_t *tx_frame)
{
    /* This function encodes a message into a frame structure. The frame structure has
       an identical layout to the TX buffer, allowing the frame structure to be directly
       copied into TX buffer. */
    bool is_extd = flags & CAN_MSG_FLAG_EXTD;
    bool is_rtr = flags & CAN_MSG_FLAG_RTR;

    //Set frame information
    tx_frame->dlc = dlc;
    tx_frame->frame_format = is_extd;
    tx_frame->rtr = is_rtr;
    tx_frame->self_reception = (flags & CAN_MSG_FLAG_SELF) ? 1 : 0;
    tx_frame->single_shot = (flags & CAN_MSG_FLAG_SS) ? 1 : 0;

    //Set ID
    if (is_extd) {
        uint32_t id_temp = __builtin_bswap32((id & CAN_EXTD_ID_MASK) << 3); //((id << 3) >> 8*(3-i))
        for (int i = 0; i < 4; i++) {
            tx_frame->extended.id[i] = (id_temp >> (8 * i)) & 0xFF;
        }
    } else {
        uint32_t id_temp =  __builtin_bswap16((id & CAN_STD_ID_MASK) << 5); //((id << 5) >> 8*(1-i))
        for (int i = 0; i < 2; i++) {
            tx_frame->standard.id[i] = (id_temp >> (8 * i)) & 0xFF;
        }
    }

    //Set Data
    uint8_t *data_buffer = (is_extd) ? tx_frame->extended.data : tx_frame->standard.data;
    if (!is_rtr) {
        for (int i = 0; (i < dlc) && (i < CAN_FRAME_MAX_DLC); i++) {
            data_buffer[i] = data[i];
        }
    }
}

/**
 * @brief   Parse formatted CAN frame (RX Buffer Layout) into its contents
 *
 * @param[in] rx_frame Pointer to formatted frame
 * @param[out] id 11 or 29bit ID
 * @param[out] dlc Data length code
 * @param[out] data Data. Left over bytes set to 0.
 * @param[out] format Type of CAN frame
 */
static inline void can_ll_prase_frame_buffer(can_ll_frame_buffer_t *rx_frame, uint32_t *id, uint8_t *dlc,
                                             uint8_t *data, uint32_t *flags)
{
    //This function decodes a frame structure into it's constituent components.

    //Copy frame information
    *dlc = rx_frame->dlc;
    uint32_t flags_temp = 0;
    flags_temp |= (rx_frame->frame_format) ? CAN_MSG_FLAG_EXTD : 0;
    flags_temp |= (rx_frame->rtr) ? CAN_MSG_FLAG_RTR : 0;
    flags_temp |= (rx_frame->dlc > CAN_FRAME_MAX_DLC) ? CAN_MSG_FLAG_DLC_NON_COMP : 0;
    *flags = flags_temp;

    //Copy ID
    if (rx_frame->frame_format) {
        uint32_t id_temp = 0;
        for (int i = 0; i < 4; i++) {
            id_temp |= rx_frame->extended.id[i] << (8 * i);
        }
        id_temp = __builtin_bswap32(id_temp) >> 3;  //((byte[i] << 8*(3-i)) >> 3)
        *id = id_temp & CAN_EXTD_ID_MASK;
    } else {
        uint32_t id_temp = 0;
        for (int i = 0; i < 2; i++) {
            id_temp |= rx_frame->standard.id[i] << (8 * i);
        }
        id_temp = __builtin_bswap16(id_temp) >> 5;  //((byte[i] << 8*(1-i)) >> 5)
        *id = id_temp & CAN_STD_ID_MASK;
    }

    //Copy data
    uint8_t *data_buffer = (rx_frame->frame_format) ? rx_frame->extended.data : rx_frame->standard.data;
    int data_length = (rx_frame->rtr) ? 0 : ((rx_frame->dlc > CAN_FRAME_MAX_DLC) ? CAN_FRAME_MAX_DLC : rx_frame->dlc);
    for (int i = 0; i < data_length; i++) {
        data[i] = data_buffer[i];
    }
    //Set remaining bytes of data to 0
    for (int i = data_length; i < CAN_FRAME_MAX_DLC; i++) {
        data[i] = 0;
    }
}

/* ----------------------- RX Message Count Register ------------------------ */

/**
 * @brief   Get RX Message Counter
 *
 * @param hw Start address of the CAN registers
 * @return RX Message Counter
 */
static inline uint32_t can_ll_get_rx_msg_count(can_dev_t *hw)
{
    return hw->rx_message_counter_reg.val;
}

/* ------------------------- Clock Divider Register ------------------------- */

/**
 * @brief   Set CLKOUT Divider and enable/disable
 *
 * @param hw Start address of the CAN registers
 * @param divider Divider for CLKOUT. Set to 0 to disable CLKOUT
 */
static inline void can_ll_set_clkout(can_dev_t *hw, uint32_t divider)
{
    /* Configure CLKOUT. CLKOUT is a pre-scaled version of APB CLK. Divider can be
       1, or any even number from 2 to 14. Set to out of range value (0) to disable
       CLKOUT. */

    if (divider >= 2 && divider <= 14) {
        CAN.clock_divider_reg.co = 0;
        CAN.clock_divider_reg.cd = (divider / 2) - 1;
    } else if (divider == 1) {
        CAN.clock_divider_reg.co = 0;
        CAN.clock_divider_reg.cd = 7;
    } else {
        CAN.clock_divider_reg.co = 1;
        CAN.clock_divider_reg.cd = 0;
    }
}

/**
 * @brief   Set register address mapping to extended mode
 *
 * Extended mode register address mapping consists of more registers and extra
 * features.
 *
 * @param hw Start address of the CAN registers
 *
 * @note Must be called before setting any configuration
 * @note Must be called in reset mode
 */
static inline void can_ll_enable_extended_reg_layout(can_dev_t *hw)
{
    hw->clock_divider_reg.cm = 1;
}

#ifdef __cplusplus
}
#endif
