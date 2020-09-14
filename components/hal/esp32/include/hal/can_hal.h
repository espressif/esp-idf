// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#warning hal/can_hal.h is deprecated, please use hal/twai_hal.h instead

#include "hal/twai_hal.h"
#include "hal/can_types.h"

/* ------------------------- Defines and Typedefs --------------------------- */

//Error active interrupt related
#define CAN_HAL_EVENT_BUS_OFF               TWAI_HAL_EVENT_BUS_OFF
#define CAN_HAL_EVENT_BUS_RECOV_CPLT        TWAI_HAL_EVENT_BUS_RECOV_CPLT
#define CAN_HAL_EVENT_BUS_RECOV_PROGRESS    TWAI_HAL_EVENT_BUS_RECOV_PROGRESS
#define CAN_HAL_EVENT_ABOVE_EWL             TWAI_HAL_EVENT_ABOVE_EWL
#define CAN_HAL_EVENT_BELOW_EWL             TWAI_HAL_EVENT_BELOW_EWL
#define CAN_HAL_EVENT_ERROR_PASSIVE         TWAI_HAL_EVENT_ERROR_PASSIVE
#define CAN_HAL_EVENT_ERROR_ACTIVE          TWAI_HAL_EVENT_ERROR_ACTIVE
#define CAN_HAL_EVENT_BUS_ERR               TWAI_HAL_EVENT_BUS_ERR
#define CAN_HAL_EVENT_ARB_LOST              TWAI_HAL_EVENT_ARB_LOST
#define CAN_HAL_EVENT_RX_BUFF_FRAME         TWAI_HAL_EVENT_RX_BUFF_FRAME
#define CAN_HAL_EVENT_TX_BUFF_FREE          TWAI_HAL_EVENT_TX_BUFF_FREE

typedef twai_hal_context_t can_hal_context_t;

typedef twai_hal_frame_t can_hal_frame_t;

/* ---------------------------- Init and Config ----------------------------- */

static inline bool can_hal_init(can_hal_context_t *hal_ctx){
    return twai_hal_init(hal_ctx);
}

static inline void can_hal_deinit(can_hal_context_t *hal_ctx)
{
    twai_hal_deinit(hal_ctx);
}

static inline void can_hal_configure(can_hal_context_t *hal_ctx, const can_timing_config_t *t_config, const can_filter_config_t *f_config, uint32_t intr_mask, uint32_t clkout_divider)
{
    twai_hal_configure(hal_ctx, t_config, f_config, intr_mask, clkout_divider);
}

/* -------------------------------- Actions --------------------------------- */

static inline void can_hal_start(can_hal_context_t *hal_ctx, can_mode_t mode)
{
    twai_hal_start(hal_ctx, mode);
}

static inline void can_hal_stop(can_hal_context_t *hal_ctx)
{
    twai_hal_stop(hal_ctx);
}

static inline void can_hal_start_bus_recovery(can_hal_context_t *hal_ctx)
{
    twai_hal_start_bus_recovery(hal_ctx);
}

static inline uint32_t can_hal_get_tec(can_hal_context_t *hal_ctx)
{
    return twai_hal_get_tec(hal_ctx);
}

static inline uint32_t can_hal_get_rec(can_hal_context_t *hal_ctx)
{
    return twai_hal_get_rec(hal_ctx);
}

static inline uint32_t can_hal_get_rx_msg_count(can_hal_context_t *hal_ctx)
{
    return twai_hal_get_rx_msg_count(hal_ctx);
}

static inline bool can_hal_check_last_tx_successful(can_hal_context_t *hal_ctx)
{
    return twai_hal_check_last_tx_successful(hal_ctx);
}

static inline bool can_hal_check_state_flags(can_hal_context_t *hal_ctx, uint32_t check_flags)
{
    return twai_hal_check_state_flags(hal_ctx, check_flags);
}

/* ----------------------------- Event Handling ----------------------------- */

static inline uint32_t can_hal_decode_interrupt_events(can_hal_context_t *hal_ctx) {
    return twai_hal_decode_interrupt_events(hal_ctx);
}

/* ------------------------------- TX and RX -------------------------------- */

static inline void can_hal_format_frame(const can_message_t *message, can_hal_frame_t *frame)
{
    twai_hal_format_frame(message, frame);
}

static inline void can_hal_parse_frame(can_hal_frame_t *frame, can_message_t *message)
{
    twai_hal_parse_frame(frame, message);
}

static inline void can_hal_set_tx_buffer_and_transmit(can_hal_context_t *hal_ctx, can_hal_frame_t *tx_frame)
{
    twai_hal_set_tx_buffer_and_transmit(hal_ctx, tx_frame);
}

static inline void can_hal_read_rx_buffer_and_clear(can_hal_context_t *hal_ctx, can_hal_frame_t *rx_frame)
{
    twai_hal_read_rx_buffer_and_clear(hal_ctx, rx_frame);
}

#ifdef __cplusplus
}
#endif
