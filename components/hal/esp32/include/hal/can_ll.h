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
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for CAN

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#warning hal/can_ll.h is deprecated, please use hal/twai_ll.h instead

#include "hal/twai_ll.h"
#include "hal/can_types.h"
#include "soc/can_periph.h"

/* ------------------------- Defines and Typedefs --------------------------- */

#define CAN_LL_STATUS_RBS       TWAI_LL_STATUS_RBS
#define CAN_LL_STATUS_DOS       TWAI_LL_STATUS_DOS
#define CAN_LL_STATUS_TBS       TWAI_LL_STATUS_TBS
#define CAN_LL_STATUS_TCS       TWAI_LL_STATUS_TCS
#define CAN_LL_STATUS_RS        TWAI_LL_STATUS_RS
#define CAN_LL_STATUS_TS        TWAI_LL_STATUS_TS
#define CAN_LL_STATUS_ES        TWAI_LL_STATUS_ES
#define CAN_LL_STATUS_BS        TWAI_LL_STATUS_BS

#define CAN_LL_INTR_RI          TWAI_LL_INTR_RI
#define CAN_LL_INTR_TI          TWAI_LL_INTR_TI
#define CAN_LL_INTR_EI          TWAI_LL_INTR_EI
#define CAN_LL_INTR_EPI         TWAI_LL_INTR_EPI
#define CAN_LL_INTR_ALI         TWAI_LL_INTR_ALI
#define CAN_LL_INTR_BEI         TWAI_LL_INTR_BEI

typedef twai_ll_frame_buffer_t can_ll_frame_buffer_t;

/* ---------------------------- Mode Register ------------------------------- */

static inline void can_ll_enter_reset_mode(can_dev_t *hw)
{
    twai_ll_enter_reset_mode(hw);
}

static inline void can_ll_exit_reset_mode(can_dev_t *hw)
{
    twai_ll_exit_reset_mode(hw);
}

static inline bool can_ll_is_in_reset_mode(can_dev_t *hw)
{
    return twai_ll_is_in_reset_mode(hw);
}

static inline void can_ll_set_mode(can_dev_t *hw, can_mode_t mode)
{
    twai_ll_set_mode(hw, mode);
}

/* --------------------------- Command Register ----------------------------- */

static inline void can_ll_set_cmd_tx(can_dev_t *hw)
{
    twai_ll_set_cmd_tx(hw);
}

static inline void can_ll_set_cmd_tx_single_shot(can_dev_t *hw)
{
    twai_ll_set_cmd_tx_single_shot(hw);
}

static inline void can_ll_set_cmd_abort_tx(can_dev_t *hw)
{
    twai_ll_set_cmd_abort_tx(hw);
}

static inline void can_ll_set_cmd_release_rx_buffer(can_dev_t *hw)
{
    twai_ll_set_cmd_release_rx_buffer(hw);
}

static inline void can_ll_set_cmd_clear_data_overrun(can_dev_t *hw)
{
    twai_ll_set_cmd_clear_data_overrun(hw);
}

static inline void can_ll_set_cmd_self_rx_request(can_dev_t *hw)
{
    twai_ll_set_cmd_self_rx_request(hw);
}

static inline void can_ll_set_cmd_self_rx_single_shot(can_dev_t *hw)
{
    twai_ll_set_cmd_self_rx_single_shot(hw);
}

/* --------------------------- Status Register ------------------------------ */

static inline uint32_t can_ll_get_status(can_dev_t *hw)
{
    return twai_ll_get_status(hw);
}

static inline bool can_ll_is_fifo_overrun(can_dev_t *hw)
{
    return twai_ll_is_fifo_overrun(hw);
}

static inline bool can_ll_is_last_tx_successful(can_dev_t *hw)
{
    return twai_ll_is_last_tx_successful(hw);
}

/* -------------------------- Interrupt Register ---------------------------- */

static inline uint32_t can_ll_get_and_clear_intrs(can_dev_t *hw)
{
    return twai_ll_get_and_clear_intrs(hw);
}

/* ----------------------- Interrupt Enable Register ------------------------ */

static inline void can_ll_set_enabled_intrs(can_dev_t *hw, uint32_t intr_mask)
{
    twai_ll_set_enabled_intrs(hw, intr_mask);
}

/* ------------------------ Bus Timing Registers --------------------------- */

static inline void can_ll_set_bus_timing(can_dev_t *hw, uint32_t brp, uint32_t sjw, uint32_t tseg1, uint32_t tseg2, bool triple_sampling)
{
    twai_ll_set_bus_timing(hw, brp, sjw, tseg1, tseg2, triple_sampling);
}

/* ----------------------------- ALC Register ------------------------------- */

static inline void can_ll_clear_arb_lost_cap(can_dev_t *hw)
{
    twai_ll_clear_arb_lost_cap(hw);
}

/* ----------------------------- ECC Register ------------------------------- */

static inline void can_ll_clear_err_code_cap(can_dev_t *hw)
{
    twai_ll_clear_err_code_cap(hw);
}

/* ----------------------------- EWL Register ------------------------------- */

static inline void can_ll_set_err_warn_lim(can_dev_t *hw, uint32_t ewl)
{
    twai_ll_set_err_warn_lim(hw, ewl);
}

static inline uint32_t can_ll_get_err_warn_lim(can_dev_t *hw)
{
    return twai_ll_get_err_warn_lim(hw);
}

/* ------------------------ RX Error Count Register ------------------------- */

static inline uint32_t can_ll_get_rec(can_dev_t *hw)
{
    return twai_ll_get_rec(hw);
}

static inline void can_ll_set_rec(can_dev_t *hw, uint32_t rec)
{
    twai_ll_set_rec(hw, rec);
}

/* ------------------------ TX Error Count Register ------------------------- */

static inline uint32_t can_ll_get_tec(can_dev_t *hw)
{
    return twai_ll_get_tec(hw);
}

static inline void can_ll_set_tec(can_dev_t *hw, uint32_t tec)
{
    twai_ll_set_tec(hw, tec);
}

/* ---------------------- Acceptance Filter Registers ----------------------- */

static inline void can_ll_set_acc_filter(can_dev_t* hw, uint32_t code, uint32_t mask, bool single_filter)
{
    twai_ll_set_acc_filter(hw, code, mask, single_filter);
}

/* ------------------------- TX/RX Buffer Registers ------------------------- */

static inline void can_ll_set_tx_buffer(can_dev_t *hw, can_ll_frame_buffer_t *tx_frame)
{
    twai_ll_set_tx_buffer(hw, tx_frame);
}

static inline void can_ll_get_rx_buffer(can_dev_t *hw, can_ll_frame_buffer_t *rx_frame)
{
    twai_ll_get_rx_buffer(hw, rx_frame);
}

static inline void can_ll_format_frame_buffer(uint32_t id, uint8_t dlc, const uint8_t *data,
                                              uint32_t flags, can_ll_frame_buffer_t *tx_frame)
{
    twai_ll_format_frame_buffer(id, dlc, data, flags, tx_frame);
}

static inline void can_ll_prase_frame_buffer(can_ll_frame_buffer_t *rx_frame, uint32_t *id, uint8_t *dlc,
                                             uint8_t *data, uint32_t *flags)
{
    twai_ll_prase_frame_buffer(rx_frame, id, dlc, data, flags);
}

/* ----------------------- RX Message Count Register ------------------------ */

static inline uint32_t can_ll_get_rx_msg_count(can_dev_t *hw)
{
    return twai_ll_get_rx_msg_count(hw);
}

/* ------------------------- Clock Divider Register ------------------------- */

static inline void can_ll_set_clkout(can_dev_t *hw, uint32_t divider)
{
    twai_ll_set_clkout(hw, divider);
}

static inline void can_ll_enable_extended_reg_layout(can_dev_t *hw)
{
    twai_ll_enable_extended_reg_layout(hw);
}

#ifdef __cplusplus
}
#endif
