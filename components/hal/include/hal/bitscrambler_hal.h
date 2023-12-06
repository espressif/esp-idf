/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#if CONFIG_SOC_BITSCRAMBLER_SUPPORTED
#include "soc/bitscrambler_struct.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 * @brief The TX and RX BitScramblers have registers that work in the same way, but that are
 * in different locations. This struct defines those registers; if we address the TX and
 * RX bitscramblers through this we can mostly use the same code for both.
 */
typedef struct {
	volatile bitscrambler_tx_inst_cfg0_reg_t *inst_cfg0;
	volatile bitscrambler_tx_inst_cfg1_reg_t *inst_cfg1;
	volatile bitscrambler_tx_lut_cfg0_reg_t *lut_cfg0;
	volatile bitscrambler_tx_lut_cfg1_reg_t *lut_cfg1;
	volatile bitscrambler_tx_tailing_bits_reg_t *trailing_bits;
	volatile bitscrambler_tx_ctrl_reg_t *ctrl;
	volatile bitscrambler_tx_state_reg_t *state;
} bitscrambler_regs_t;

extern const bitscrambler_regs_t bitscramblers_reg[2];

#ifdef __cplusplus
}
#endif

#endif
