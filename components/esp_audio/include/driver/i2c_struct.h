// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _DRIVER_II2C_STRUCT_H_
#define _DRIVER_II2C_STRUCT_H_


typedef struct I2C_DEV{
	union {
		struct {
			unsigned int scl_low_period: 14;
			unsigned int reserved14: 18;
		} fld;
		uint32_t val;
	} SCL_LOW_PERIOD;
	union {
		struct {
			unsigned int sda_force_out: 1;
			unsigned int scl_force_out: 1;
			unsigned int sample_scl_level: 1;
			unsigned int reserved3: 1;
			unsigned int ms_mode: 1;
			unsigned int trans_start: 1;
			unsigned int tx_lsb_first: 1;
			unsigned int rx_lsb_first: 1;
			unsigned int clk_en: 1;
			unsigned int reserved9: 23;
		} fld;
		uint32_t val;
	} CTR;
	union {
		struct {
			unsigned int ack_rec: 1;
			unsigned int slave_rw: 1;
			unsigned int time_out: 1;
			unsigned int arb_lost: 1;
			unsigned int bus_busy: 1;
			unsigned int slave_addressed: 1;
			unsigned int byte_trans: 1;
			unsigned int reserved7: 1;
			unsigned int rxfifo_cnt: 6;
			unsigned int reserved14: 4;
			unsigned int txfifo_cnt: 6;
			unsigned int scl_main_state_last: 3;
			unsigned int reserved27: 1;
			unsigned int scl_state_last: 3;
			unsigned int reserved31: 1;
		} fld;
		uint32_t val;
	} SR;
	union {
		struct {
			unsigned int time_out_reg: 20;
			unsigned int reserved20: 12;
		} fld;
		uint32_t val;
	} TO;
	union {
		struct {
			unsigned int slave_addr: 15;
			unsigned int reserved15: 16;
			unsigned int addr_10bit_en: 1;
		} fld;
		uint32_t val;
	} SLAVE_ADDR;
	union {
		struct {
			unsigned int rxfifo_start_addr: 5;
			unsigned int rxfifo_end_addr: 5;
			unsigned int txfifo_start_addr: 5;
			unsigned int txfifo_end_addr: 5;
			unsigned int reserved20: 12;
		} fld;
		uint32_t val;
	} RXFIFO_ST;
	union {
		struct {
			unsigned int rxfifo_full_thrhd: 5;
			unsigned int txfifo_empty_thrhd: 5;
			unsigned int nonfifo_en: 1;
			unsigned int fifo_addr_cfg_en: 1;
			unsigned int rx_fifo_rst: 1;
			unsigned int tx_fifo_rst: 1;
			unsigned int nonfifo_rx_thres: 6;
			unsigned int nonfifo_tx_thres: 6;
			unsigned int reserved26: 6;
		} fld;
		uint32_t val;
	} I2C_FIFO_CONF;
	union {
		struct {
			unsigned int fifo_data: 8;
			unsigned int reserved8: 24;
		} fld;
		uint32_t val;
	} I2C_DATA;
	union {
		struct {
			unsigned int rxfifo_full_int_raw: 1;
			unsigned int txfifo_empty_int_raw: 1;
			unsigned int rxfifo_ovf_int_raw: 1;
			unsigned int end_detect_int_raw: 1;
			unsigned int slave_tran_comp_int_raw: 1;
			unsigned int arbitration_lost_int_raw: 1;
			unsigned int master_tran_comp_int_raw: 1;
			unsigned int trans_complete_int_raw: 1;
			unsigned int time_out_int_raw: 1;
			unsigned int trans_start_int_raw: 1;
			unsigned int ack_err_int_raw: 1;
			unsigned int rx_rec_full_int_raw: 1;
			unsigned int tx_send_empty_int_raw: 1;
			unsigned int reserved13: 19;
		} fld;
		uint32_t val;
	} I2C_INT_RAW;
	union {
		struct {
			unsigned int rxfifo_full_int_clr: 1;
			unsigned int txfifo_empty_int_clr: 1;
			unsigned int rxfifo_ovf_int_clr: 1;
			unsigned int end_detect_int_clr: 1;
			unsigned int slave_tran_comp_int_clr: 1;
			unsigned int arbitration_lost_int_clr: 1;
			unsigned int master_tran_comp_int_clr: 1;
			unsigned int trans_complete_int_clr: 1;
			unsigned int time_out_int_clr: 1;
			unsigned int trans_start_int_clr: 1;
			unsigned int ack_err_int_clr: 1;
			unsigned int rx_rec_full_int_clr: 1;
			unsigned int tx_send_empty_int_clr: 1;
			unsigned int reserved13: 19;
		} fld;
		uint32_t val;
	} I2C_INT_CLR;
	union {
		struct {
			unsigned int rxfifo_full_int_ena: 1;
			unsigned int txfifo_empty_int_ena: 1;
			unsigned int rxfifo_ovf_int_ena: 1;
			unsigned int end_detect_int_ena: 1;
			unsigned int slave_tran_comp_int_ena: 1;
			unsigned int arbitration_lost_int_ena: 1;
			unsigned int master_tran_comp_int_ena: 1;
			unsigned int trans_complete_int_ena: 1;
			unsigned int time_out_int_ena: 1;
			unsigned int trans_start_int_ena: 1;
			unsigned int ack_err_int_ena: 1;
			unsigned int rx_rec_full_int_ena: 1;
			unsigned int tx_send_empty_int_ena: 1;
			unsigned int reserved13: 19;
		} fld;
		uint32_t val;
	} I2C_INT_ENA;
	union {
		struct {
			unsigned int rxfifo_full_int_st: 1;
			unsigned int txfifo_empty_int_st: 1;
			unsigned int rxfifo_ovf_int_st: 1;
			unsigned int end_detect_int_st: 1;
			unsigned int slave_tran_comp_int_st: 1;
			unsigned int arbitration_lost_int_st: 1;
			unsigned int master_tran_comp_int_st: 1;
			unsigned int trans_complete_int_st: 1;
			unsigned int time_out_int_st: 1;
			unsigned int trans_start_int_st: 1;
			unsigned int ack_err_int_st: 1;
			unsigned int rx_rec_full_int_st: 1;
			unsigned int tx_send_empty_int_st: 1;
			unsigned int reserved13: 19;
		} fld;
		uint32_t val;
	} I2C_INT_STATUS;
	union {
		struct {
			unsigned int sda_hold_time: 10;
			unsigned int reserved10: 22;
		} fld;
		uint32_t val;
	} SDA_HOLD;
	union {
		struct {
			unsigned int sda_sample_time: 10;
			unsigned int reserved10: 22;
		} fld;
		uint32_t val;
	} SDA_SAMPLE;
	union {
		struct {
			unsigned int scl_high_period: 14;
			unsigned int reserved14: 18;
		} fld;
		uint32_t val;
	} SCL_HIGH_PERIOD;
	uint32_t reserved_3c;
	union {
		struct {
			unsigned int scl_start_hold_time: 10;
			unsigned int reserved10: 22;
		} fld;
		uint32_t val;
	} SCL_START_HOLD;
	union {
		struct {
			unsigned int scl_rstart_setup_time: 10;
			unsigned int reserved10: 22;
		} fld;
		uint32_t val;
	} SCL_RSTART_SETUP;
	union {
		struct {
			unsigned int scl_stop_hold_time: 14;
			unsigned int reserved14: 18;
		} fld;
		uint32_t val;
	} SCL_STOP_HOLD;
	union {
		struct {
			unsigned int scl_stop_setup_time: 10;
			unsigned int reserved10: 22;
		} fld;
		uint32_t val;
	} SCL_STOP_SETUP;
	union {
		struct {
			unsigned int scl_filter_thres: 3;
			unsigned int scl_filter_en: 1;
			unsigned int reserved4: 28;
		} fld;
		uint32_t val;
	} SCL_FILTER_CFG;
	union {
		struct {
			unsigned int sda_filter_thres: 3;
			unsigned int sda_filter_en: 1;
			unsigned int reserved4: 28;
		} fld;
		uint32_t val;
	} SDA_FILTER_CFG;
	union {
		struct {
			//unsigned int command: 14;
			unsigned int command_byte_num: 8;
			unsigned int command_ack_en: 1;
			unsigned int command_ack_exp: 1;
			unsigned int command_ack_val: 1;
			unsigned int command_op_code: 3;
			unsigned int reserved14: 17;
			unsigned int command_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD[16];
#if 0
	union {
		struct {
			unsigned int command1: 14;
			unsigned int reserved14: 17;
			unsigned int command1_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD1;
	union {
		struct {
			unsigned int command2: 14;
			unsigned int reserved14: 17;
			unsigned int command2_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD2;
	union {
		struct {
			unsigned int command3: 14;
			unsigned int reserved14: 17;
			unsigned int command3_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD3;
	union {
		struct {
			unsigned int command4: 14;
			unsigned int reserved14: 17;
			unsigned int command4_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD4;
	union {
		struct {
			unsigned int command5: 14;
			unsigned int reserved14: 17;
			unsigned int command5_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD5;
	union {
		struct {
			unsigned int command6: 14;
			unsigned int reserved14: 17;
			unsigned int command6_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD6;
	union {
		struct {
			unsigned int command7: 14;
			unsigned int reserved14: 17;
			unsigned int command7_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD7;
	union {
		struct {
			unsigned int command8: 14;
			unsigned int reserved14: 17;
			unsigned int command8_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD8;
	union {
		struct {
			unsigned int command9: 14;
			unsigned int reserved14: 17;
			unsigned int command9_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD9;
	union {
		struct {
			unsigned int command10: 14;
			unsigned int reserved14: 17;
			unsigned int command10_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD10;
	union {
		struct {
			unsigned int command11: 14;
			unsigned int reserved14: 17;
			unsigned int command11_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD11;
	union {
		struct {
			unsigned int command12: 14;
			unsigned int reserved14: 17;
			unsigned int command12_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD12;
	union {
		struct {
			unsigned int command13: 14;
			unsigned int reserved14: 17;
			unsigned int command13_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD13;
	union {
		struct {
			unsigned int command14: 14;
			unsigned int reserved14: 17;
			unsigned int command14_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD14;
	union {
		struct {
			unsigned int command15: 14;
			unsigned int reserved14: 17;
			unsigned int command15_done: 1;
		} fld;
		uint32_t val;
	} I2C_COMD15;
#endif
	uint32_t reserved_98;
	uint32_t reserved_9c;
	uint32_t reserved_a0;
	uint32_t reserved_a4;
	uint32_t reserved_a8;
	uint32_t reserved_ac;
	uint32_t reserved_b0;
	uint32_t reserved_b4;
	uint32_t reserved_b8;
	uint32_t reserved_bc;
	uint32_t reserved_c0;
	uint32_t reserved_c4;
	uint32_t reserved_c8;
	uint32_t reserved_cc;
	uint32_t reserved_d0;
	uint32_t reserved_d4;
	uint32_t reserved_d8;
	uint32_t reserved_dc;
	uint32_t reserved_e0;
	uint32_t reserved_e4;
	uint32_t reserved_e8;
	uint32_t reserved_ec;
	uint32_t reserved_f0;
	uint32_t reserved_f4;
	//struct {
	//	uint32_t val;
	//}I2C_DATE;
	uint32_t reserved_fc;
	//struct {
	//	uint32_t val;
	//}I2C_FIFO_START_ADDR;
}I2C_TypeDef;


#endif
