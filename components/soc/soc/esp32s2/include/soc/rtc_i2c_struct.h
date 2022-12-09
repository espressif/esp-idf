// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_RTC_I2C_STRUCT_H_
#define _SOC_RTC_I2C_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t period:        20;             /*time period that scl = 0*/
            uint32_t reserved20:    12;
        };
        uint32_t val;
    } scl_low;
    union {
        struct {
            uint32_t sda_force_out:        1;       /*1=push pull  0=open drain*/
            uint32_t scl_force_out:        1;       /*1=push pull  0=open drain*/
            uint32_t ms_mode:              1;       /*1=master  0=slave*/
            uint32_t trans_start:          1;       /*force start*/
            uint32_t tx_lsb_first:         1;       /*transit lsb first*/
            uint32_t rx_lsb_first:         1;       /*receive lsb first*/
            uint32_t reserved6:           23;
            uint32_t i2c_ctrl_clk_gate_en: 1;
            uint32_t i2c_reset:            1;       /*rtc i2c sw reset*/
            uint32_t i2cclk_en:            1;       /*rtc i2c reg clk gating*/
        };
        uint32_t val;
    } ctrl;
    union {
        struct {
            uint32_t ack_rec:             1;        /*ack response*/
            uint32_t slave_rw:            1;        /*slave read or write*/
            uint32_t arb_lost:            1;        /*arbitration is lost*/
            uint32_t bus_busy:            1;        /*bus is busy*/
            uint32_t slave_addressed:     1;        /*slave reg sub address*/
            uint32_t byte_trans:          1;        /*One byte transit done*/
            uint32_t op_cnt:              2;        /*which operation is working*/
            uint32_t reserved8:           8;
            uint32_t shift:               8;        /*shifter content*/
            uint32_t scl_main_state_last: 3;        /*i2c last main status*/
            uint32_t reserved27:          1;
            uint32_t scl_state_last:      3;        /*scl last status*/
            uint32_t reserved31:          1;
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t time_out:  20;                 /*time out threshold*/
            uint32_t reserved20:12;
        };
        uint32_t val;
    } timeout;
    union {
        struct {
            uint32_t addr:         15;              /*slave address*/
            uint32_t reserved15:   16;
            uint32_t en_10bit:      1;              /*i2c 10bit mode enable*/
        };
        uint32_t val;
    } slave_addr;
    union {
        struct {
            uint32_t period:         20;            /*time period that scl = 1*/
            uint32_t reserved20:     12;
        };
        uint32_t val;
    } scl_high;
    union {
        struct {
            uint32_t sda_duty_num:20;               /*time period for SDA to toggle after SCL goes low*/
            uint32_t reserved20:  12;
        };
        uint32_t val;
    } sda_duty;
    union {
        struct {
            uint32_t scl_start_period:20;           /*time period for SCL to toggle after I2C start is triggered*/
            uint32_t reserved20:      12;
        };
        uint32_t val;
    } scl_start_period;
    union {
        struct {
            uint32_t scl_stop_period:20;            /*time period for SCL to stop after I2C end is triggered*/
            uint32_t reserved20:     12;
        };
        uint32_t val;
    } scl_stop_period;
    union {
        struct {
            uint32_t slave_tran_comp:          1;   /*clear slave transit complete interrupt*/
            uint32_t arbitration_lost:         1;   /*clear arbitration lost interrupt*/
            uint32_t master_tran_comp:         1;   /*clear master transit complete interrupt*/
            uint32_t trans_complete:           1;   /*clear transit complete interrupt*/
            uint32_t time_out:                 1;   /*clear time out interrupt*/
            uint32_t ack_err:                  1;   /*clear ack error interrupt*/
            uint32_t rx_data:                  1;   /*clear receive data interrupt*/
            uint32_t tx_data:                  1;   /*clear transit load data complete interrupt*/
            uint32_t detect_start:             1;   /*clear detect start interrupt*/
            uint32_t reserved9:               23;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t slave_tran_comp:          1;   /*slave transit complete interrupt raw*/
            uint32_t arbitration_lost:         1;   /*arbitration lost interrupt raw*/
            uint32_t master_tran_comp:         1;   /*master transit complete interrupt raw*/
            uint32_t trans_complete:           1;   /*transit complete interrupt raw*/
            uint32_t time_out:                 1;   /*time out interrupt raw*/
            uint32_t ack_err:                  1;   /*ack error interrupt raw*/
            uint32_t rx_data:                  1;   /*receive data interrupt raw*/
            uint32_t tx_data:                  1;   /*transit data interrupt raw*/
            uint32_t detect_start:             1;   /*detect start interrupt raw*/
            uint32_t reserved9:               23;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t slave_tran_comp:         1;    /*slave transit complete interrupt state*/
            uint32_t arbitration_lost:        1;    /*arbitration lost interrupt state*/
            uint32_t master_tran_comp:        1;    /*master transit complete interrupt state*/
            uint32_t trans_complete:          1;    /*transit complete interrupt state*/
            uint32_t time_out:                1;    /*time out interrupt state*/
            uint32_t ack_err:                 1;    /*ack error interrupt state*/
            uint32_t rx_data:                 1;    /*receive data interrupt state*/
            uint32_t tx_data:                 1;    /*transit data interrupt state*/
            uint32_t detect_start:            1;    /*detect start interrupt state*/
            uint32_t reserved9:              23;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t slave_tran_comp:          1;   /*enable slave transit complete interrupt*/
            uint32_t arbitration_lost:         1;   /*enable arbitration lost interrupt*/
            uint32_t master_tran_comp:         1;   /*enable master transit complete interrupt*/
            uint32_t trans_complete:           1;   /*enable transit complete interrupt*/
            uint32_t time_out:                 1;   /*enable time out interrupt*/
            uint32_t ack_err:                  1;   /*enable eack error interrupt*/
            uint32_t rx_data:                  1;   /*enable receive data interrupt*/
            uint32_t tx_data:                  1;   /*enable transit data interrupt*/
            uint32_t detect_start:             1;   /*enable detect start interrupt*/
            uint32_t reserved9:               23;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t i2c_rdata:     8;              /*data received*/
            uint32_t slave_tx_data: 8;              /*data sent by slave*/
            uint32_t reserved16:   15;
            uint32_t i2c_done:      1;              /*i2c done*/
        };
        uint32_t val;
    } fifo_data;
    union {
        struct {
            uint32_t command0:     14;              /*command0*/
            uint32_t reserved14:   17;
            uint32_t done:          1;              /*command0_done*/
        };
        uint32_t val;
    } command[16];
    uint32_t reserved_78;
    uint32_t reserved_7c;
    uint32_t reserved_80;
    uint32_t reserved_84;
    uint32_t reserved_88;
    uint32_t reserved_8c;
    uint32_t reserved_90;
    uint32_t reserved_94;
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
    uint32_t reserved_f8;
    union {
        struct {
            uint32_t i2c_date:  28;
            uint32_t reserved28: 4;
        };
        uint32_t val;
    } date;
} rtc_i2c_dev_t;
extern rtc_i2c_dev_t RTC_I2C;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_RTC_I2C_STRUCT_H_ */
