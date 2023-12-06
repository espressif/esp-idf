/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/bitscrambler_hal.h"

const bitscrambler_regs_t bitscramblers_reg[2]={
	{ //BITSCRAMBLER_DIR_TX
		.inst_cfg0=&BITSCRAMBLER.tx_inst_cfg0,
		.inst_cfg1=&BITSCRAMBLER.tx_inst_cfg1,
		.lut_cfg0=&BITSCRAMBLER.tx_lut_cfg0,
		.lut_cfg1=&BITSCRAMBLER.tx_lut_cfg1,
		.trailing_bits=&BITSCRAMBLER.tx_tailing_bits,
		.ctrl=&BITSCRAMBLER.tx_ctrl,
		.state=&BITSCRAMBLER.tx_state,
	},
	{ //BITSCRAMBLER_DIR_RX
	  //Note that this assumes the RX and TX registers are exactly the same. If there's
	  //ever a chip where this is not the case, we need to rework this.
		.inst_cfg0=(bitscrambler_tx_inst_cfg0_reg_t*)&BITSCRAMBLER.rx_inst_cfg0,
		.inst_cfg1=(bitscrambler_tx_inst_cfg1_reg_t*)&BITSCRAMBLER.rx_inst_cfg1,
		.lut_cfg0=(bitscrambler_tx_lut_cfg0_reg_t*)&BITSCRAMBLER.rx_lut_cfg0,
		.lut_cfg1=(bitscrambler_tx_lut_cfg1_reg_t*)&BITSCRAMBLER.rx_lut_cfg1,
		.trailing_bits=(bitscrambler_tx_tailing_bits_reg_t*)&BITSCRAMBLER.rx_tailing_bits,
		.ctrl=(bitscrambler_tx_ctrl_reg_t*)&BITSCRAMBLER.rx_ctrl,
		.state=(bitscrambler_tx_state_reg_t*)&BITSCRAMBLER.rx_state,
	}
};
