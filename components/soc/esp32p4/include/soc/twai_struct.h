/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of mode register
 *  TWAI mode register.
 */
typedef union {
    struct {
        /** reset_mode : R/W; bitpos: [0]; default: 1;
         *  1: reset, detection of a set reset mode bit results in aborting the current
         *  transmission/reception of a message and entering the reset mode. 0: normal, on the
         *  '1-to-0' transition of the reset mode bit, the TWAI controller returns to the
         *  operating mode.
         */
        uint32_t reset_mode:1;
        /** listen_only_mode : R/W; bitpos: [1]; default: 0;
         *  1: listen only, in this mode the TWAI controller would give no acknowledge to the
         *  TWAI-bus, even if a message is received successfully. The error counters are
         *  stopped at the current value. 0: normal.
         */
        uint32_t listen_only_mode:1;
        /** self_test_mode : R/W; bitpos: [2]; default: 0;
         *  1: self test, in this mode a full node test is possible without any other active
         *  node on the bus using the self reception request command. The TWAI controller will
         *  perform a successful transmission, even if there is no acknowledge received. 0:
         *  normal, an acknowledge is required for successful transmission.
         */
        uint32_t self_test_mode:1;
        /** acceptance_filter_mode : R/W; bitpos: [3]; default: 0;
         *  1:single, the single acceptance filter option is enabled (one filter with the
         *  length of 32 bit is active). 0:dual, the dual acceptance filter option is enabled
         *  (two filters, each with the length of 16 bit are active).
         */
        uint32_t acceptance_filter_mode:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} twai_mode_reg_t;

/** Type of cmd register
 *  TWAI command register.
 */
typedef union {
    struct {
        /** tx_request : WO; bitpos: [0]; default: 0;
         *  1: present, a message shall be transmitted. 0: absent
         */
        uint32_t tx_request:1;
        /** abort_tx : WO; bitpos: [1]; default: 0;
         *  1: present, if not already in progress, a pending transmission request is
         *  cancelled. 0: absent
         */
        uint32_t abort_tx:1;
        /** release_buffer : WO; bitpos: [2]; default: 0;
         *  1: released, the receive buffer, representing the message memory space in the
         *  RXFIFO is released. 0: no action
         */
        uint32_t release_buffer:1;
        /** clear_data_overrun : WO; bitpos: [3]; default: 0;
         *  1: clear, the data overrun status bit is cleared. 0: no action.
         */
        uint32_t clear_data_overrun:1;
        /** self_rx_request : WO; bitpos: [4]; default: 0;
         *  1: present, a message shall be transmitted and received simultaneously. 0: absent.
         */
        uint32_t self_rx_request:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} twai_cmd_reg_t;

/** Type of bus_timing_0 register
 *  Bit timing configuration register 0.
 */
typedef union {
    struct {
        /** baud_presc : R/W; bitpos: [13:0]; default: 0;
         *  The period of the TWAI system clock is programmable and determines the individual
         *  bit timing. Software has R/W permission in reset mode and RO permission in
         *  operation mode.
         */
        uint32_t baud_presc:14;
        /** sync_jump_width : R/W; bitpos: [15:14]; default: 0;
         *  The synchronization jump width defines the maximum number of clock cycles a bit
         *  period may be shortened or lengthened. Software has R/W permission in reset mode
         *  and RO in operation mode.
         */
        uint32_t sync_jump_width:2;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} twai_bus_timing_0_reg_t;

/** Type of bus_timing_1 register
 *  Bit timing configuration register 1.
 */
typedef union {
    struct {
        /** time_segment1 : R/W; bitpos: [3:0]; default: 0;
         *  The number of clock cycles in TSEG1 per bit timing. Software has R/W permission in
         *  reset mode and RO in operation mode.
         */
        uint32_t time_segment1:4;
        /** time_segment2 : R/W; bitpos: [6:4]; default: 0;
         *  The number of clock cycles in TSEG2 per bit timing. Software has R/W permission in
         *  reset mode and RO in operation mode.
         */
        uint32_t time_segment2:3;
        /** time_sampling : R/W; bitpos: [7]; default: 0;
         *  1: triple, the bus is sampled three times. 0: single, the bus is sampled once.
         *  Software has R/W permission in reset mode and RO in operation mode.
         */
        uint32_t time_sampling:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} twai_bus_timing_1_reg_t;

/** Type of err_warning_limit register
 *  TWAI error threshold configuration register.
 */
typedef union {
    struct {
        /** err_warning_limit : R/W; bitpos: [7:0]; default: 96;
         *  The threshold that trigger error warning interrupt when this interrupt is enabled.
         *  Software has R/W permission in reset mode and RO in operation mode.
         */
        uint32_t err_warning_limit:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} twai_err_warning_limit_reg_t;

/** Type of clock_divider register
 *  Clock divider register.
 */
typedef union {
    struct {
        /** cd : R/W; bitpos: [7:0]; default: 0;
         *  These bits are used to define the frequency at the external CLKOUT pin.
         */
        uint32_t cd:8;
        /** clock_off : R/W; bitpos: [8]; default: 0;
         *  1: Disable the external CLKOUT pin. 0: Enable the external CLKOUT pin. Software has
         *  R/W permission in reset mode and RO in operation mode.
         */
        uint32_t clock_off:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} twai_clock_divider_reg_t;

/** Type of sw_standby_cfg register
 *  Software configure standby pin directly.
 */
typedef union {
    struct {
        /** sw_standby_en : R/W; bitpos: [0]; default: 0;
         *  Enable standby pin.
         */
        uint32_t sw_standby_en:1;
        /** sw_standby_clr : R/W; bitpos: [1]; default: 1;
         *  Clear standby pin.
         */
        uint32_t sw_standby_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} twai_sw_standby_cfg_reg_t;

/** Type of hw_cfg register
 *  Hardware configure standby pin.
 */
typedef union {
    struct {
        /** hw_standby_en : R/W; bitpos: [0]; default: 0;
         *  Enable function that hardware control standby pin.
         */
        uint32_t hw_standby_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} twai_hw_cfg_reg_t;

/** Type of hw_standby_cnt register
 *  Configure standby counter.
 */
typedef union {
    struct {
        /** standby_wait_cnt : R/W; bitpos: [31:0]; default: 1;
         *  Configure the number of cycles before standby becomes high when TWAI_HW_STANDBY_EN
         *  is enabled.
         */
        uint32_t standby_wait_cnt:32;
    };
    uint32_t val;
} twai_hw_standby_cnt_reg_t;

/** Type of idle_intr_cnt register
 *  Configure idle interrupt counter.
 */
typedef union {
    struct {
        /** idle_intr_cnt : R/W; bitpos: [31:0]; default: 1;
         *  Configure the number of cycles before triggering idle interrupt.
         */
        uint32_t idle_intr_cnt:32;
    };
    uint32_t val;
} twai_idle_intr_cnt_reg_t;

/** Type of eco_cfg register
 *  ECO configuration register.
 */
typedef union {
    struct {
        /** rdn_ena : R/W; bitpos: [0]; default: 0;
         *  Enable eco module.
         */
        uint32_t rdn_ena:1;
        /** rdn_result : RO; bitpos: [1]; default: 1;
         *  Output of eco module.
         */
        uint32_t rdn_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} twai_eco_cfg_reg_t;


/** Group: Status Registers */
/** Type of status register
 *  TWAI status register.
 */
typedef union {
    struct {
        /** status_receive_buffer : RO; bitpos: [0]; default: 0;
         *  1: full, one or more complete messages are available in the RXFIFO. 0: empty, no
         *  message is available
         */
        uint32_t status_receive_buffer:1;
        /** status_overrun : RO; bitpos: [1]; default: 0;
         *  1: overrun, a message was lost because there was not enough space for that message
         *  in the RXFIFO. 0: absent, no data overrun has occurred since the last clear data
         *  overrun command was given
         */
        uint32_t status_overrun:1;
        /** status_transmit_buffer : RO; bitpos: [2]; default: 0;
         *  1: released, the CPU may write a message into the transmit buffer. 0: locked, the
         *  CPU cannot access the transmit buffer, a message is either waiting for transmission
         *  or is in the process of being transmitted
         */
        uint32_t status_transmit_buffer:1;
        /** status_transmission_complete : RO; bitpos: [3]; default: 0;
         *  1: complete, last requested transmission has been successfully completed. 0:
         *  incomplete, previously requested transmission is not yet completed
         */
        uint32_t status_transmission_complete:1;
        /** status_receive : RO; bitpos: [4]; default: 0;
         *  1: receive, the TWAI controller is receiving a message. 0: idle
         */
        uint32_t status_receive:1;
        /** status_transmit : RO; bitpos: [5]; default: 0;
         *  1: transmit, the TWAI controller is transmitting a message. 0: idle
         */
        uint32_t status_transmit:1;
        /** status_err : RO; bitpos: [6]; default: 0;
         *  1: error, at least one of the error counters has reached or exceeded the CPU
         *  warning limit defined by the Error Warning Limit Register (EWLR). 0: ok, both error
         *  counters are below the warning limit
         */
        uint32_t status_err:1;
        /** status_node_bus_off : RO; bitpos: [7]; default: 0;
         *  1: bus-off, the TWAI controller is not involved in bus activities. 0: bus-on, the
         *  TWAI controller is involved in bus activities
         */
        uint32_t status_node_bus_off:1;
        /** status_miss : RO; bitpos: [8]; default: 0;
         *  1: current message is destroyed because of FIFO overflow.
         */
        uint32_t status_miss:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} twai_status_reg_t;

/** Type of arb_lost_cap register
 *  TWAI arbiter lost capture register.
 */
typedef union {
    struct {
        /** arbitration_lost_capture : RO; bitpos: [4:0]; default: 0;
         *  This register contains information about the bit position of losing arbitration.
         */
        uint32_t arbitration_lost_capture:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} twai_arb_lost_cap_reg_t;

/** Type of err_code_cap register
 *  TWAI error info capture register.
 */
typedef union {
    struct {
        /** err_capture_code_segment : RO; bitpos: [4:0]; default: 0;
         *  This register contains information about the location of errors on the bus.
         */
        uint32_t err_capture_code_segment:5;
        /** err_capture_code_direction : RO; bitpos: [5]; default: 0;
         *  1: RX, error occurred during reception. 0: TX, error occurred during transmission.
         */
        uint32_t err_capture_code_direction:1;
        /** err_capture_code_type : RO; bitpos: [7:6]; default: 0;
         *  00: bit error. 01: form error. 10:stuff error. 11:other type of error.
         */
        uint32_t err_capture_code_type:2;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} twai_err_code_cap_reg_t;

/** Type of rx_err_cnt register
 *  Rx error counter register.
 */
typedef union {
    struct {
        /** rx_err_cnt : R/W; bitpos: [7:0]; default: 0;
         *  The RX error counter register reflects the current value of the transmit error
         *  counter. Software has R/W permission in reset mode and RO in operation mode.
         */
        uint32_t rx_err_cnt:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} twai_rx_err_cnt_reg_t;

/** Type of tx_err_cnt register
 *  Tx error counter register.
 */
typedef union {
    struct {
        /** tx_err_cnt : R/W; bitpos: [7:0]; default: 0;
         *  The TX error counter register reflects the current value of the transmit error
         *  counter. Software has R/W permission in reset mode and RO in operation mode.
         */
        uint32_t tx_err_cnt:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} twai_tx_err_cnt_reg_t;

/** Type of rx_message_counter register
 *  Received message counter register.
 */
typedef union {
    struct {
        /** rx_message_counter : RO; bitpos: [6:0]; default: 0;
         *  Reflects the number of messages available within the RXFIFO. The value is
         *  incremented with each receive event and decremented by the release receive buffer
         *  command.
         */
        uint32_t rx_message_counter:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} twai_rx_message_counter_reg_t;


/** Group: Interrupt Registers */
/** Type of interrupt register
 *  Interrupt signals' register.
 */
typedef union {
    struct {
        /** receive_int_st : RO; bitpos: [0]; default: 0;
         *  1: this bit is set while the receive FIFO is not empty and the RIE bit is set
         *  within the interrupt enable register. 0: reset
         */
        uint32_t receive_int_st:1;
        /** transmit_int_st : RO; bitpos: [1]; default: 0;
         *  1: this bit is set whenever the transmit buffer status changes from '0-to-1'
         *  (released) and the TIE bit is set within the interrupt enable register. 0: reset
         */
        uint32_t transmit_int_st:1;
        /** err_warning_int_st : RO; bitpos: [2]; default: 0;
         *  1: this bit is set on every change (set and clear) of either the error status or
         *  bus status bits and the EIE bit is set within the interrupt enable register. 0:
         *  reset
         */
        uint32_t err_warning_int_st:1;
        /** data_overrun_int_st : RO; bitpos: [3]; default: 0;
         *  1: this bit is set on a '0-to-1' transition of the data overrun status bit and the
         *  DOIE bit is set within the interrupt enable register. 0: reset
         */
        uint32_t data_overrun_int_st:1;
        /** ts_counter_ovfl_int_st : RO; bitpos: [4]; default: 0;
         *  1: this bit is set then the timestamp counter reaches the maximum value and
         *  overflow.
         */
        uint32_t ts_counter_ovfl_int_st:1;
        /** err_passive_int_st : RO; bitpos: [5]; default: 0;
         *  1: this bit is set whenever the TWAI controller has reached the error passive
         *  status (at least one error counter exceeds the protocol-defined level of 127) or if
         *  the TWAI controller is in the error passive status and enters the error active
         *  status again and the EPIE bit is set within the interrupt enable register. 0: reset
         */
        uint32_t err_passive_int_st:1;
        /** arbitration_lost_int_st : RO; bitpos: [6]; default: 0;
         *  1: this bit is set when the TWAI controller lost the arbitration and becomes a
         *  receiver and the ALIE bit is set within the interrupt enable register. 0: reset
         */
        uint32_t arbitration_lost_int_st:1;
        /** bus_err_int_st : RO; bitpos: [7]; default: 0;
         *  1: this bit is set when the TWAI controller detects an error on the TWAI-bus and
         *  the BEIE bit is set within the interrupt enable register. 0: reset
         */
        uint32_t bus_err_int_st:1;
        /** idle_int_st : RO; bitpos: [8]; default: 0;
         *  1: this bit is set when the TWAI controller detects state of TWAI become IDLE and
         *  this interrupt enable bit is set within the interrupt enable register. 0: reset
         */
        uint32_t idle_int_st:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} twai_interrupt_status_reg_t;

/** Type of interrupt_enable register
 *  Interrupt enable register.
 */
typedef union {
    struct {
        /** ext_receive_int_ena : R/W; bitpos: [0]; default: 0;
         *  1: enabled, when the receive buffer status is 'full' the TWAI controller requests
         *  the respective interrupt. 0: disable
         */
        uint32_t ext_receive_int_ena:1;
        /** ext_transmit_int_ena : R/W; bitpos: [1]; default: 0;
         *  1: enabled, when a message has been successfully transmitted or the transmit buffer
         *  is accessible again (e.g. after an abort transmission command), the TWAI controller
         *  requests the respective interrupt. 0: disable
         */
        uint32_t ext_transmit_int_ena:1;
        /** ext_err_warning_int_ena : R/W; bitpos: [2]; default: 0;
         *  1: enabled, if the error or bus status change (see status register. Table 14), the
         *  TWAI controllerrequests the respective interrupt. 0: disable
         */
        uint32_t ext_err_warning_int_ena:1;
        /** ext_data_overrun_int_ena : R/W; bitpos: [3]; default: 0;
         *  1: enabled, if the data overrun status bit is set (see status register. Table 14),
         *  the TWAI controllerrequests the respective interrupt. 0: disable
         */
        uint32_t ext_data_overrun_int_ena:1;
        /** ts_counter_ovfl_int_ena : R/W; bitpos: [4]; default: 0;
         *  enable the timestamp counter overflow interrupt request.
         */
        uint32_t ts_counter_ovfl_int_ena:1;
        /** err_passive_int_ena : R/W; bitpos: [5]; default: 0;
         *  1: enabled, if the error status of the TWAI controller changes from error active to
         *  error passive or vice versa, the respective interrupt is requested. 0: disable
         */
        uint32_t err_passive_int_ena:1;
        /** arbitration_lost_int_ena : R/W; bitpos: [6]; default: 0;
         *  1: enabled, if the TWAI controller has lost arbitration, the respective interrupt
         *  is requested. 0: disable
         */
        uint32_t arbitration_lost_int_ena:1;
        /** bus_err_int_ena : R/W; bitpos: [7]; default: 0;
         *  1: enabled, if an bus error has been detected, the TWAI controller requests the
         *  respective interrupt. 0: disable
         */
        uint32_t bus_err_int_ena:1;
        /** idle_int_ena : RO; bitpos: [8]; default: 0;
         *  1: enabled, if state of TWAI become IDLE, the TWAI controller requests the
         *  respective interrupt. 0: disable
         */
        uint32_t idle_int_ena:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} twai_interrupt_enable_reg_t;


/** Group: Data Registers */
/** Type of buffer register
 *  TX RX Buffer.
 */
typedef union {
    struct {
        /** byte : R/W; bitpos: [7:0]; default: 0;
         *  In reset mode, it is acceptance code register 0 with R/W Permission. In operation
         *  mode, when software initiate write operation, it is tx data register 0 and when
         *  software initiate read operation, it is rx data register 0.
         */
        uint32_t byte:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} twai_tx_rx_buffer_reg_t;

typedef struct {
    union {
        struct {
            uint32_t byte: 8;           /* ACRx[7:0] Acceptance Code */
            uint32_t reserved8: 24;     /* Internal Reserved */
        };
        uint32_t val;
    } acr[4];
    union {
        struct {
            uint32_t byte: 8;           /* AMRx[7:0] Acceptance Mask */
            uint32_t reserved8: 24;     /* Internal Reserved */
        };
        uint32_t val;
    } amr[4];
    uint32_t reserved_60;
    uint32_t reserved_64;
    uint32_t reserved_68;
    uint32_t reserved_6c;
    uint32_t reserved_70;
} acceptance_filter_reg_t;

/** Group: Timestamp Register */
/** Type of timestamp_data register
 *  Timestamp data register
 */
typedef union {
    struct {
        /** timestamp_data : RO; bitpos: [31:0]; default: 0;
         *  Data of timestamp of a CAN frame.
         */
        uint32_t timestamp_data:32;
    };
    uint32_t val;
} twai_timestamp_data_reg_t;

/** Type of timestamp_prescaler register
 *  Timestamp configuration register
 */
typedef union {
    struct {
        /** ts_div_num : R/W; bitpos: [15:0]; default: 31;
         *  Configures the clock division number of timestamp counter.
         */
        uint32_t ts_div_num:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} twai_timestamp_prescaler_reg_t;

/** Type of timestamp_cfg register
 *  Timestamp configuration register
 */
typedef union {
    struct {
        /** ts_enable : R/W; bitpos: [0]; default: 0;
         *  enable the timestamp collection function.
         */
        uint32_t ts_enable:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} twai_timestamp_cfg_reg_t;


typedef struct {
    volatile twai_mode_reg_t mode;
    volatile twai_cmd_reg_t cmd;
    volatile twai_status_reg_t status;
    volatile twai_interrupt_status_reg_t interrupt_st;
    volatile twai_interrupt_enable_reg_t interrupt_ena;
    uint32_t reserved_014;
    volatile twai_bus_timing_0_reg_t bus_timing_0;
    volatile twai_bus_timing_1_reg_t bus_timing_1;
    uint32_t reserved_020[3];
    volatile twai_arb_lost_cap_reg_t arb_lost_cap;
    volatile twai_err_code_cap_reg_t err_code_cap;
    volatile twai_err_warning_limit_reg_t err_warning_limit;
    volatile twai_rx_err_cnt_reg_t rx_err_cnt;
    volatile twai_tx_err_cnt_reg_t tx_err_cnt;
    volatile union {
        acceptance_filter_reg_t acceptance_filter;
        twai_tx_rx_buffer_reg_t tx_rx_buffer[13];
    };
    volatile twai_rx_message_counter_reg_t rx_message_counter;
    uint32_t reserved_078;
    volatile twai_clock_divider_reg_t clock_divider;
    volatile twai_sw_standby_cfg_reg_t sw_standby_cfg;
    volatile twai_hw_cfg_reg_t hw_cfg;
    volatile twai_hw_standby_cnt_reg_t hw_standby_cnt;
    volatile twai_idle_intr_cnt_reg_t idle_intr_cnt;
    volatile twai_eco_cfg_reg_t eco_cfg;
    volatile twai_timestamp_data_reg_t timestamp_data;
    volatile twai_timestamp_prescaler_reg_t timestamp_prescaler;
    volatile twai_timestamp_cfg_reg_t timestamp_cfg;
} twai_dev_t;

extern twai_dev_t TWAI0;
extern twai_dev_t TWAI1;
extern twai_dev_t TWAI2;

#ifndef __cplusplus
_Static_assert(sizeof(twai_dev_t) == 0xa0, "Invalid size of twai_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
