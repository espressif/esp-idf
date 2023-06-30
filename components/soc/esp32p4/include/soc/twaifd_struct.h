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

/** Group: ID register */
/** Type of device_id register
 *  TWAI FD device id status register
 */
typedef union {
    struct {
        /** device_id : R/W; bitpos: [31:0]; default: 51965;
         *  Represents whether CAN IP function is mapped correctly on its base address.
         */
        uint32_t device_id:32;
    };
    uint32_t val;
} twaifd_device_id_reg_t;


/** Group: Configuration register */
/** Type of mode_setting register
 *  TWAI FD mode setting register
 */
typedef union {
    struct {
        /** sw_reset : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to reset the TWAI FD controller.\\
         *  0: invalid\\
         *  1: reset.\\
         */
        uint32_t sw_reset:1;
        /** listen_only_mode : R/W; bitpos: [1]; default: 0;
         *  bus monitor enable
         */
        uint32_t listen_only_mode:1;
        /** self_test_mode : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable the self test mode.\\
         *  0: disable\\
         *  1: enable\\
         */
        uint32_t self_test_mode:1;
        /** accept_filter_mode : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable the usage of acceptance filters.\\
         *  0: disable\\
         *  1: enable\\
         */
        uint32_t accept_filter_mode:1;
        /** flexible_data_rate : R/W; bitpos: [4]; default: 1;
         *  Configures whether or not to support flexible data rate.\\
         *  0: not support\\
         *  1: support\\
         */
        uint32_t flexible_data_rate:1;
        /** rtr_frm_behavior : R/W; bitpos: [5]; default: 0;
         *  time_triggered transmission mode
         */
        uint32_t rtr_frm_behavior:1;
        /** rom : R/W; bitpos: [6]; default: 0;
         *  a\\
         */
        uint32_t rom:1;
        /** ack_behavior : R/W; bitpos: [7]; default: 0;
         *  Configures the acknowledge behavior.\\
         *  0: normal behavior.\\
         *  1: acknowledge is not sent.\\
         */
        uint32_t ack_behavior:1;
        /** test_mode : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to enable the triple sampling mode.\\
         *  0: disable\\
         *  1: enable\\
         */
        uint32_t test_mode:1;
        /** rxbam : R/W; bitpos: [9]; default: 1;
         *  a\\
         */
        uint32_t rxbam:1;
        uint32_t reserved_10:6;
        /** limit_retx_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to enable the limit of retransmission.\\
         *  0: disable\\
         *  1: enable\\
         */
        uint32_t limit_retx_en:1;
        /** retx_thres : R/W; bitpos: [20:17]; default: 0;
         *  Configures the threshold of retransmission attempts. \\
         */
        uint32_t retx_thres:4;
        /** ilbp : R/W; bitpos: [21]; default: 0;
         *  acknowledge forbidden mode
         */
        uint32_t ilbp:1;
        /** ctrl_en : R/W; bitpos: [22]; default: 0;
         *  Configures whether or not to enable the twai FD controller.\\
         *  0: disable\\
         *  1: enable\\
         */
        uint32_t ctrl_en:1;
        /** fd_type : R/W; bitpos: [23]; default: 0;
         *  Configure the twai fd frame type.\\
         *  0: ISO CAN FD\\
         *  1: CAN FD 1.0\\
         */
        uint32_t fd_type:1;
        /** pex : R/W; bitpos: [24]; default: 0;
         *  protocol expection mode\\
         */
        uint32_t pex:1;
        /** tbfbo : R/W; bitpos: [25]; default: 1;
         *  a\\
         */
        uint32_t tbfbo:1;
        /** fdrf : R/W; bitpos: [26]; default: 0;
         *  a\\
         */
        uint32_t fdrf:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} twaifd_mode_setting_reg_t;

/** Type of command register
 *  TWAI FD command register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** rxrpmv : WO; bitpos: [1]; default: 0;
         *  a\\
         */
        uint32_t rxrpmv:1;
        /** release_rx_buf : WO; bitpos: [2]; default: 0;
         *  Configures whether or not to delete all data from the receive buffer.\\
         *  0: invalid\\
         *  1: delete\\
         */
        uint32_t release_rx_buf:1;
        /** clr_overrun_flg : WO; bitpos: [3]; default: 0;
         *  Configures whether or not to clear the data overrun flag.\\
         *  0: invalid\\
         *  1: clear\\
         */
        uint32_t clr_overrun_flg:1;
        /** ercrst : WO; bitpos: [4]; default: 0;
         *  a\\
         */
        uint32_t ercrst:1;
        /** rxfcrst : WO; bitpos: [5]; default: 0;
         *  a\\
         */
        uint32_t rxfcrst:1;
        /** txfcrst : WO; bitpos: [6]; default: 0;
         *  a\\
         */
        uint32_t txfcrst:1;
        /** cpexs : WO; bitpos: [7]; default: 0;
         *  a\\
         */
        uint32_t cpexs:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} twaifd_command_reg_t;

/** Type of bit_timing register
 *  TWAI FD bit-timing  register
 */
typedef union {
    struct {
        /** prop : R/W; bitpos: [6:0]; default: 5;
         *  Configures the propagation segment of nominal bit rate.\\
         *  Measurement unit: time quanta\\
         */
        uint32_t prop:7;
        /** ph1 : R/W; bitpos: [12:7]; default: 3;
         *  Configures the phase 1 segment of nominal bit rate.\\
         *  Measurement unit: time quanta\\
         */
        uint32_t ph1:6;
        /** ph2 : R/W; bitpos: [18:13]; default: 5;
         *  Configures the phase 2 segment of nominal bit rate.\\
         *  Measurement unit: time quanta\\
         */
        uint32_t ph2:6;
        /** brp : R/W; bitpos: [26:19]; default: 16;
         *  Configures the baud-rate prescaler of nominal bit rate.\\
         *  Measurement unit: cycle of core clock.
         */
        uint32_t brp:8;
        /** sjw : R/W; bitpos: [31:27]; default: 2;
         *  Represents the synchronization jump width in nominal bit time.\\
         *  Measurement unit: time quanta\\
         */
        uint32_t sjw:5;
    };
    uint32_t val;
} twaifd_bit_timing_reg_t;

/** Type of bit_timeing_fd register
 *  TWAI FD bit-timing of FD register
 */
typedef union {
    struct {
        /** prop_fd : R/W; bitpos: [5:0]; default: 3;
         *  Configures the propagation segment of data bit rate.\\
         *  Measurement unit: time quanta\\
         */
        uint32_t prop_fd:6;
        uint32_t reserved_6:1;
        /** ph1_fd : R/W; bitpos: [11:7]; default: 3;
         *  Configures the phase 1 segment of data bit rate.\\
         *  Measurement unit: time quanta\\
         */
        uint32_t ph1_fd:5;
        uint32_t reserved_12:1;
        /** ph2_fd : R/W; bitpos: [17:13]; default: 3;
         *  Configures the phase 2 segment of data bit rate.\\
         *  Measurement unit: time quanta\\
         */
        uint32_t ph2_fd:5;
        uint32_t reserved_18:1;
        /** brp_fd : R/W; bitpos: [26:19]; default: 4;
         *  Configures the baud-rate prescaler of data bit rate.\\
         *  Measurement unit: cycle of core clock.
         */
        uint32_t brp_fd:8;
        /** sjw_fd : R/W; bitpos: [31:27]; default: 2;
         *  Represents the synchronization jump width in data bit time.\\
         *  Measurement unit: time quanta\\
         */
        uint32_t sjw_fd:5;
    };
    uint32_t val;
} twaifd_bit_timeing_fd_reg_t;

/** Type of tx_cfg register
 *  TWAI FD TX buffer configuration register
 */
typedef union {
    struct {
        /** txt_1_allow : R/W; bitpos: [0]; default: 1;
         *  Configures whether or not allow transmitting frames from TX buffer1.\\
         *  0: not allow\\
         *  1: allow\\
         */
        uint32_t txt_1_allow:1;
        /** txt_2_allow : R/W; bitpos: [1]; default: 1;
         *  Configures whether or not allow transmitting frames from TX buffer2.\\
         *  0: not allow\\
         *  1: allow\\
         */
        uint32_t txt_2_allow:1;
        /** txt_1_commit : WT; bitpos: [2]; default: 0;
         *  Configures whether or not the frames from TX register are inserted into TX
         *  buffer1.\\
         *  0: not inserted\\
         *  1: inserted\\
         */
        uint32_t txt_1_commit:1;
        /** txt_2_commit : WT; bitpos: [3]; default: 0;
         *  Configures whether or not the frames from TX register are inserted into TX
         *  buffer2.\\
         *  0: not inserted\\
         *  1: inserted\\
         */
        uint32_t txt_2_commit:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} twaifd_tx_cfg_reg_t;

/** Type of trv_delay_ssp_cfg register
 *  TWAI FD transmit delay & secondary sample point configuration register
 */
typedef union {
    struct {
        /** trv_delay_value : RO; bitpos: [6:0]; default: 0;
         *  a\\
         */
        uint32_t trv_delay_value:7;
        uint32_t reserved_7:9;
        /** ssp_offset : R/W; bitpos: [23:16]; default: 10;
         *  a\\
         */
        uint32_t ssp_offset:8;
        /** ssp_src : R/W; bitpos: [25:24]; default: 0;
         *  a\\
         */
        uint32_t ssp_src:2;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} twaifd_trv_delay_ssp_cfg_reg_t;


/** Group: Status register */
/** Type of status register
 *  TWAI FD status register
 */
typedef union {
    struct {
        /** rx_buf_stat : RO; bitpos: [0]; default: 0;
         *  Represents whether or not the receive buffer is empty.\\
         *  0: empty\\
         *  1: not empty\\
         */
        uint32_t rx_buf_stat:1;
        /** data_overrun_flg : RO; bitpos: [1]; default: 0;
         *  Represents whether or not the receive buffer is full and the frame is
         *  overrun(lost).\\
         *  0: not overrun\\
         *  1: overrun\\
         */
        uint32_t data_overrun_flg:1;
        /** tx_buf_sat : RO; bitpos: [2]; default: 0;
         *  Represents whether or not the transmit buffer is full.\\
         *  0: not full\\
         *  1: full\\
         */
        uint32_t tx_buf_sat:1;
        /** err_frm_tx : RO; bitpos: [3]; default: 0;
         *  Represents whether or not the error frame is being transmitted.\\
         *  0: not being transmitted\\
         *  1: being transmitted\\
         */
        uint32_t err_frm_tx:1;
        /** rx_frm_stat : RO; bitpos: [4]; default: 0;
         *  Represents whether or not the controller is receiving a frame.\\
         *  0: not receiving\\
         *  1: receiving\\
         */
        uint32_t rx_frm_stat:1;
        /** tx_frm_stat : RO; bitpos: [5]; default: 0;
         *  Represents whether or not the controller is transmitting a frame.\\
         *  0: not transmitting\\
         *  1: transmitting\\
         */
        uint32_t tx_frm_stat:1;
        /** err_stat : RO; bitpos: [6]; default: 0;
         *  Represents whether or not the error warning limit is reached.\\
         *  0: not reached\\
         *  1: reached\\
         */
        uint32_t err_stat:1;
        /** bus_stat : RO; bitpos: [7]; default: 1;
         *  Represents whether or not bus is active.\\
         *  0: active\\
         *  1: not active\\
         */
        uint32_t bus_stat:1;
        /** pexs : RO; bitpos: [8]; default: 0;
         *  a\\
         */
        uint32_t pexs:1;
        /** reintegrating_wait : RO; bitpos: [9]; default: 0;
         *  fsm is in reintegrating wait status
         */
        uint32_t reintegrating_wait:1;
        uint32_t reserved_10:6;
        /** stcnt : RO; bitpos: [16]; default: 0;
         *  a\\
         */
        uint32_t stcnt:1;
        /** strgs : RO; bitpos: [17]; default: 0;
         *  a\\
         */
        uint32_t strgs:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} twaifd_status_reg_t;

/** Type of rx_mem_info register
 *  TWAI FD rx memory information register
 */
typedef union {
    struct {
        /** rx_buff_size_val : RO; bitpos: [12:0]; default: 0;
         *  Represents the size of RX buffer.\\
         */
        uint32_t rx_buff_size_val:13;
        uint32_t reserved_13:3;
        /** rx_free_ctr : RO; bitpos: [28:16]; default: 0;
         *  Represents the number of free words in RX buffer.\\
         */
        uint32_t rx_free_ctr:13;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_rx_mem_info_reg_t;

/** Type of rx_pointers register
 *  TWAI FD rx memory pointer information register
 */
typedef union {
    struct {
        /** rx_wpt_val : RO; bitpos: [11:0]; default: 0;
         *  Represents the write pointer position in RX buffer.\\
         */
        uint32_t rx_wpt_val:12;
        uint32_t reserved_12:4;
        /** rx_rpt_val : RO; bitpos: [27:16]; default: 0;
         *  Represents the read pointer position in RX buffer.\\
         */
        uint32_t rx_rpt_val:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} twaifd_rx_pointers_reg_t;

/** Type of rx_status_setting register
 *  TWAI FD tx status & setting register
 */
typedef union {
    struct {
        /** rx_empty : RO; bitpos: [0]; default: 0;
         *  Represents whether or not the RX buffer is empty.\\
         *  0: not empty\\
         *  1: empty\\
         */
        uint32_t rx_empty:1;
        /** rx_full : RO; bitpos: [1]; default: 0;
         *  Represents whether or not the RX buffer is full.\\
         *  0: not full\\
         *  1: full\\
         */
        uint32_t rx_full:1;
        uint32_t reserved_2:2;
        /** rx_frm_ctr : RO; bitpos: [14:4]; default: 0;
         *  Represents the number of received frame in RX buffer.\\
         */
        uint32_t rx_frm_ctr:11;
        uint32_t reserved_15:1;
        /** rtsop : R/W; bitpos: [16]; default: 0;
         *  a\\
         */
        uint32_t rtsop:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} twaifd_rx_status_setting_reg_t;

/** Type of tx_stat register
 *  TWAI FD TX buffer status register
 */
typedef union {
    struct {
        /** txt_1_empty : RO; bitpos: [0]; default: 0;
         *  Represents whether or not the TX buffer1 is empty.\\
         *  0: not empty\\
         *  1: empty\\
         */
        uint32_t txt_1_empty:1;
        /** txt_2_empty : RO; bitpos: [1]; default: 0;
         *  Represents whether or not the TX buffer2 is empty.\\
         *  0: not empty\\
         *  1: empty\\
         */
        uint32_t txt_2_empty:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} twaifd_tx_stat_reg_t;

/** Type of err_cap_retr_ctr_alc register
 *  TWAI FD error capture & retransmit counter & arbitration lost register
 */
typedef union {
    struct {
        /** err_type : RO; bitpos: [4:0]; default: 0;
         *  a\\
         */
        uint32_t err_type:5;
        /** err_pos : RO; bitpos: [7:5]; default: 0;
         *  a\\
         */
        uint32_t err_pos:3;
        /** retr_ctr : RO; bitpos: [11:8]; default: 0;
         *  a\\
         */
        uint32_t retr_ctr:4;
        uint32_t reserved_12:4;
        /** alc_bit : RO; bitpos: [20:16]; default: 0;
         *  a\\
         */
        uint32_t alc_bit:5;
        /** alc_id_field : RO; bitpos: [23:21]; default: 0;
         *  a\\
         */
        uint32_t alc_id_field:3;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} twaifd_err_cap_retr_ctr_alc_reg_t;


/** Group: interrupt register */
/** Type of int_raw register
 *  TWAI FD interrupt raw register
 */
typedef union {
    struct {
        /** rx_frm_suc_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of TWAIFD_RX_FRM_SUC_INT.
         */
        uint32_t rx_frm_suc_int_raw:1;
        /** tx_frm_suc_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of TWAIFD_TX_FRM_SUC_INT.
         */
        uint32_t tx_frm_suc_int_raw:1;
        /** err_warning_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of TWAIFD_ERR_WARNING_INT.
         */
        uint32_t err_warning_int_raw:1;
        /** rx_data_overrun_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of TWAIFD_RX_DATA_OVERRUN_INT.
         */
        uint32_t rx_data_overrun_int_raw:1;
        uint32_t reserved_4:1;
        /** fault_confinement_chg_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of TWAIFD_FAULT_CONFINEMENT_CHG_INT.
         */
        uint32_t fault_confinement_chg_int_raw:1;
        /** arb_lost_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of TWAIFD_ARB_LOST_INT.
         */
        uint32_t arb_lost_int_raw:1;
        /** err_detected_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt status of TWAIFD_ERR_DETECTED_INT.
         */
        uint32_t err_detected_int_raw:1;
        /** is_overload_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt status of TWAIFD_IS_OVERLOAD_INT.
         */
        uint32_t is_overload_int_raw:1;
        /** rx_buf_full_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw interrupt status of TWAIFD_RX_BUF_FULL_INT.
         */
        uint32_t rx_buf_full_int_raw:1;
        /** bit_rate_shift_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The raw interrupt status of TWAIFD_BIT_RATE_SHIFT_INT.
         */
        uint32_t bit_rate_shift_int_raw:1;
        /** rx_buf_not_empty_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw interrupt status of TWAIFD_RX_BUF_NOT_EMPTY_INT.
         */
        uint32_t rx_buf_not_empty_int_raw:1;
        /** tx_buf_status_chg_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The raw interrupt status of TWAIFD_TX_BUF_STATUS_CHG_INT.
         */
        uint32_t tx_buf_status_chg_int_raw:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} twaifd_int_raw_reg_t;

/** Type of int_ena register
 *  TWAI FD interrupt enable register
 */
typedef union {
    struct {
        /** rx_frm_suc_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable TWAIFD_RX_FRM_SUC_INT.
         */
        uint32_t rx_frm_suc_int_ena:1;
        /** tx_frm_suc_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable TWAIFD_TX_FRM_SUC_INT.
         */
        uint32_t tx_frm_suc_int_ena:1;
        /** err_warning_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable TWAIFD_ERR_WARNING_INT.
         */
        uint32_t err_warning_int_ena:1;
        /** rx_data_overrun_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable TWAIFD_RX_DATA_OVERRUN_INT.
         */
        uint32_t rx_data_overrun_int_ena:1;
        uint32_t reserved_4:1;
        /** fault_confinement_chg_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable TWAIFD_FAULT_CONFINEMENT_CHG_INT.
         */
        uint32_t fault_confinement_chg_int_ena:1;
        /** arb_lost_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable TWAIFD_ARB_LOST_INT.
         */
        uint32_t arb_lost_int_ena:1;
        /** err_detected_int_ena : R/W; bitpos: [7]; default: 0;
         *  Write 1 to enable TWAIFD_ERR_DETECTED_INT.
         */
        uint32_t err_detected_int_ena:1;
        /** is_overload_int_ena : R/W; bitpos: [8]; default: 0;
         *  Write 1 to enable TWAIFD_IS_OVERLOAD_INT.
         */
        uint32_t is_overload_int_ena:1;
        /** rx_buf_full_int_ena : R/W; bitpos: [9]; default: 0;
         *  Write 1 to enable TWAIFD_RX_BUF_FULL_INT.
         */
        uint32_t rx_buf_full_int_ena:1;
        /** bit_rate_shift_int_ena : R/W; bitpos: [10]; default: 0;
         *  Write 1 to enable TWAIFD_BIT_RATE_SHIFT_INT.
         */
        uint32_t bit_rate_shift_int_ena:1;
        /** rx_buf_not_empty_int_ena : R/W; bitpos: [11]; default: 0;
         *  Write 1 to enable TWAIFD_RX_BUF_NOT_EMPTY_INT.
         */
        uint32_t rx_buf_not_empty_int_ena:1;
        /** tx_buf_status_chg_int_ena : R/W; bitpos: [12]; default: 0;
         *  Write 1 to enable TWAIFD_TX_BUF_STATUS_CHG_INT.
         */
        uint32_t tx_buf_status_chg_int_ena:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} twaifd_int_ena_reg_t;

/** Type of int_st register
 *  TWAI FD interrupt status register
 */
typedef union {
    struct {
        /** rx_frm_suc_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of TWAIFD_RX_FRM_SUC_INT.
         */
        uint32_t rx_frm_suc_int_st:1;
        /** tx_frm_suc_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of TWAIFD_TX_FRM_SUC_INT.
         */
        uint32_t tx_frm_suc_int_st:1;
        /** err_warning_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of TWAIFD_ERR_WARNING_INT.
         */
        uint32_t err_warning_int_st:1;
        /** rx_data_overrun_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of TWAIFD_RX_DATA_OVERRUN_INT.
         */
        uint32_t rx_data_overrun_int_st:1;
        uint32_t reserved_4:1;
        /** fault_confinement_chg_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of TWAIFD_FAULT_CONFINEMENT_CHG_INT.
         */
        uint32_t fault_confinement_chg_int_st:1;
        /** arb_lost_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of TWAIFD_ARB_LOST_INT.
         */
        uint32_t arb_lost_int_st:1;
        /** err_detected_int_st : RO; bitpos: [7]; default: 0;
         *  The masked interrupt status of TWAIFD_ERR_DETECTED_INT.
         */
        uint32_t err_detected_int_st:1;
        /** is_overload_int_st : RO; bitpos: [8]; default: 0;
         *  The masked interrupt status of TWAIFD_IS_OVERLOAD_INT.
         */
        uint32_t is_overload_int_st:1;
        /** rx_buf_full_int_st : RO; bitpos: [9]; default: 0;
         *  The masked interrupt status of TWAIFD_RX_BUF_FULL_INT.
         */
        uint32_t rx_buf_full_int_st:1;
        /** bit_rate_shift_int_st : RO; bitpos: [10]; default: 0;
         *  The masked interrupt status of TWAIFD_BIT_RATE_SHIFT_INT.
         */
        uint32_t bit_rate_shift_int_st:1;
        /** rx_buf_not_empty_int_st : RO; bitpos: [11]; default: 0;
         *  The masked interrupt status of TWAIFD_RX_BUF_NOT_EMPTY_INT.
         */
        uint32_t rx_buf_not_empty_int_st:1;
        /** tx_buf_status_chg_int_st : RO; bitpos: [12]; default: 0;
         *  The masked interrupt status of TWAIFD_TX_BUF_STATUS_CHG_INT.
         */
        uint32_t tx_buf_status_chg_int_st:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} twaifd_int_st_reg_t;

/** Type of int_clr register
 *  TWAI FD interrupt clear register
 */
typedef union {
    struct {
        /** rx_frm_suc_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear TWAIFD_RX_FRM_SUC_INT.
         */
        uint32_t rx_frm_suc_int_clr:1;
        /** tx_frm_suc_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear TWAIFD_TX_FRM_SUC_INT.
         */
        uint32_t tx_frm_suc_int_clr:1;
        /** err_warning_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear TWAIFD_ERR_WARNING_INT.
         */
        uint32_t err_warning_int_clr:1;
        /** rx_data_overrun_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear TWAIFD_RX_DATA_OVERRUN_INT.
         */
        uint32_t rx_data_overrun_int_clr:1;
        uint32_t reserved_4:1;
        /** fault_confinement_chg_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear TWAIFD_FAULT_CONFINEMENT_CHG_INT.
         */
        uint32_t fault_confinement_chg_int_clr:1;
        /** arb_lost_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear TWAIFD_ARB_LOST_INT.
         */
        uint32_t arb_lost_int_clr:1;
        /** err_detected_int_clr : WT; bitpos: [7]; default: 0;
         *  Write 1 to clear TWAIFD_ERR_DETECTED_INT.
         */
        uint32_t err_detected_int_clr:1;
        /** is_overload_int_clr : WT; bitpos: [8]; default: 0;
         *  Write 1 to clear TWAIFD_IS_OVERLOAD_INT.
         */
        uint32_t is_overload_int_clr:1;
        /** rx_buf_full_int_clr : WT; bitpos: [9]; default: 0;
         *  Write 1 to clear TWAIFD_RX_BUF_FULL_INT.
         */
        uint32_t rx_buf_full_int_clr:1;
        /** bit_rate_shift_int_clr : WT; bitpos: [10]; default: 0;
         *  Write 1 to clear TWAIFD_BIT_RATE_SHIFT_INT.
         */
        uint32_t bit_rate_shift_int_clr:1;
        /** rx_buf_not_empty_int_clr : WT; bitpos: [11]; default: 0;
         *  Write 1 to clear TWAIFD_RX_BUF_NOT_EMPTY_INT.
         */
        uint32_t rx_buf_not_empty_int_clr:1;
        /** tx_buf_status_chg_int_clr : WT; bitpos: [12]; default: 0;
         *  Write 1 to clear TWAIFD_TX_BUF_STATUS_CHG_INT.
         */
        uint32_t tx_buf_status_chg_int_clr:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} twaifd_int_clr_reg_t;


/** Group: error confinement register */
/** Type of err_th_stat register
 *  TWAI FD error threshold and status register
 */
typedef union {
    struct {
        /** err_warning_thres : R/W; bitpos: [7:0]; default: 96;
         *  Configures the error warning threshold.\\
         */
        uint32_t err_warning_thres:8;
        /** err_passive_thres : R/W; bitpos: [15:8]; default: 128;
         *  Configures the error passive threshold.\\
         */
        uint32_t err_passive_thres:8;
        /** err_active : RO; bitpos: [16]; default: 1;
         *  Represents the fault state of error active.\\
         */
        uint32_t err_active:1;
        /** err_passive : RO; bitpos: [17]; default: 0;
         *  Represents the fault state of error passive.\\
         */
        uint32_t err_passive:1;
        /** bus_off : RO; bitpos: [18]; default: 0;
         *  Represents the fault state of bus off.\\
         */
        uint32_t bus_off:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} twaifd_err_th_stat_reg_t;

/** Type of error_counters register
 *  TWAI FD error counters status register
 */
typedef union {
    struct {
        /** rxc_val : RO; bitpos: [15:0]; default: 0;
         *  Represents the receiver error counter value.\\
         */
        uint32_t rxc_val:16;
        /** txc_val : RO; bitpos: [31:16]; default: 0;
         *  Represents the transmitter error counter value.\\
         */
        uint32_t txc_val:16;
    };
    uint32_t val;
} twaifd_error_counters_reg_t;

/** Type of error_counters_sp register
 *  TWAI FD special error counters status register
 */
typedef union {
    struct {
        /** err_fd_val : RO; bitpos: [15:0]; default: 0;
         *  Represents the number of error in the data bit time.\\
         */
        uint32_t err_fd_val:16;
        /** err_norm_val : RO; bitpos: [31:16]; default: 0;
         *  Represents the number of error in the nominal bit time.\\
         */
        uint32_t err_norm_val:16;
    };
    uint32_t val;
} twaifd_error_counters_sp_reg_t;

/** Type of ctr_pres register
 *  TWAI FD error counters pre-define configuration register
 */
typedef union {
    struct {
        /** ctr_pres_val : WO; bitpos: [8:0]; default: 0;
         *  Configures the pre-defined value to set the error counter.\\
         */
        uint32_t ctr_pres_val:9;
        /** ptx : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to set the receiver error counter into the value of
         *  pre-defined value.\\
         *  0: invalid\\
         *  1: set\\
         */
        uint32_t ptx:1;
        /** prx : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to set the transmitter error counter into the value of
         *  pre-defined value.\\
         *  0: invalid\\
         *  1: set\\
         */
        uint32_t prx:1;
        /** enorm : WO; bitpos: [11]; default: 0;
         *  Configures whether or not to erase the error counter of nominal bit time.\\
         *  0: invalid\\
         *  1: erase\\
         */
        uint32_t enorm:1;
        /** efd : WO; bitpos: [12]; default: 0;
         *  Configures whether or not to erase the error counter of data bit time.\\
         *  0: invalid\\
         *  1: erase\\
         */
        uint32_t efd:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} twaifd_ctr_pres_reg_t;


/** Group: receiver register */
/** Type of rx_data register
 *  TWAI FD received data register
 */
typedef union {
    struct {
        /** rx_data : RO; bitpos: [31:0]; default: 0;
         *  Data received.
         */
        uint32_t rx_data:32;
    };
    uint32_t val;
} twaifd_rx_data_reg_t;


/** Group: filter register */
/** Type of filter_a_mask register
 *  TWAI FD filter A mask value register
 */
typedef union {
    struct {
        /** bit_mask_a : R/W; bitpos: [28:0]; default: 0;
         *  filter A bit masked value.
         */
        uint32_t bit_mask_a:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_a_mask_reg_t;

/** Type of filter_a_val register
 *  TWAI FD filter A bit value register
 */
typedef union {
    struct {
        /** bit_val_a : R/W; bitpos: [28:0]; default: 0;
         *  filter A bit value.
         */
        uint32_t bit_val_a:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_a_val_reg_t;

/** Type of filter_b_mask register
 *  TWAI FD filter B mask value register
 */
typedef union {
    struct {
        /** bit_mask_b : R/W; bitpos: [28:0]; default: 0;
         *  filter A bit masked value.
         */
        uint32_t bit_mask_b:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_b_mask_reg_t;

/** Type of filter_b_val register
 *  TWAI FD filter B bit value register
 */
typedef union {
    struct {
        /** bit_val_b : R/W; bitpos: [28:0]; default: 0;
         *  filter A bit value.
         */
        uint32_t bit_val_b:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_b_val_reg_t;

/** Type of filter_c_mask register
 *  TWAI FD filter C mask value register
 */
typedef union {
    struct {
        /** bit_mask_c : R/W; bitpos: [28:0]; default: 0;
         *  filter A bit masked value.
         */
        uint32_t bit_mask_c:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_c_mask_reg_t;

/** Type of filter_c_val register
 *  TWAI FD filter C bit value register
 */
typedef union {
    struct {
        /** bit_val_c : R/W; bitpos: [28:0]; default: 0;
         *  filter A bit value.
         */
        uint32_t bit_val_c:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_c_val_reg_t;

/** Type of filter_ran_low register
 *  TWAI FD filter range low value register
 */
typedef union {
    struct {
        /** bit_ran_low : R/W; bitpos: [28:0]; default: 0;
         *  filter A bit masked value.
         */
        uint32_t bit_ran_low:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_ran_low_reg_t;

/** Type of filter_ran_high register
 *  TWAI FD filter range high value register
 */
typedef union {
    struct {
        /** bit_ran_high : R/W; bitpos: [28:0]; default: 0;
         *  filter A bit value.
         */
        uint32_t bit_ran_high:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_ran_high_reg_t;

/** Type of filter_control register
 *  TWAI FD filter control register
 */
typedef union {
    struct {
        /** fanb : R/W; bitpos: [0]; default: 1;
         *  filter A with nominal and base mode.
         */
        uint32_t fanb:1;
        /** fane : R/W; bitpos: [1]; default: 1;
         *  filter A with nominal and extended mode.
         */
        uint32_t fane:1;
        /** fafb : R/W; bitpos: [2]; default: 1;
         *  filter A with FD and base mode.
         */
        uint32_t fafb:1;
        /** fafe : R/W; bitpos: [3]; default: 1;
         *  filter A with FD and extended mode.
         */
        uint32_t fafe:1;
        /** fbnb : R/W; bitpos: [4]; default: 0;
         *  filter B with nominal and base mode.
         */
        uint32_t fbnb:1;
        /** fbne : R/W; bitpos: [5]; default: 0;
         *  filter B with nominal and extended mode.
         */
        uint32_t fbne:1;
        /** fbfb : R/W; bitpos: [6]; default: 0;
         *  filter B with FD and base mode.
         */
        uint32_t fbfb:1;
        /** fbfe : R/W; bitpos: [7]; default: 0;
         *  filter B with FD and extended mode.
         */
        uint32_t fbfe:1;
        /** fcnb : R/W; bitpos: [8]; default: 0;
         *  filter C with nominal and base mode.
         */
        uint32_t fcnb:1;
        /** fcne : R/W; bitpos: [9]; default: 0;
         *  filter C with nominal and extended mode.
         */
        uint32_t fcne:1;
        /** fcfb : R/W; bitpos: [10]; default: 0;
         *  filter C with FD and base mode.
         */
        uint32_t fcfb:1;
        /** fcfe : R/W; bitpos: [11]; default: 0;
         *  filter C with FD and extended mode.
         */
        uint32_t fcfe:1;
        /** frnb : R/W; bitpos: [12]; default: 0;
         *  filter range with nominal and base mode.
         */
        uint32_t frnb:1;
        /** frne : R/W; bitpos: [13]; default: 0;
         *  filter range with nominal and extended mode.
         */
        uint32_t frne:1;
        /** frfb : R/W; bitpos: [14]; default: 0;
         *  filter range with FD and base mode.
         */
        uint32_t frfb:1;
        /** frfe : R/W; bitpos: [15]; default: 0;
         *  filter range with FD and extended mode.
         */
        uint32_t frfe:1;
        /** sfa : RO; bitpos: [16]; default: 0;
         *  filter A status
         */
        uint32_t sfa:1;
        /** sfb : RO; bitpos: [17]; default: 0;
         *  filter B status
         */
        uint32_t sfb:1;
        /** sfc : RO; bitpos: [18]; default: 0;
         *  filter C status
         */
        uint32_t sfc:1;
        /** sfr : RO; bitpos: [19]; default: 0;
         *  filter range status
         */
        uint32_t sfr:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} twaifd_filter_control_reg_t;


/** Group: transmitter register */
/** Type of tx_data_0 register
 *  TWAI FD transmit data register 0
 */
typedef union {
    struct {
        /** dlc_tx : R/W; bitpos: [3:0]; default: 0;
         *  Configures the brs to be transmitted.
         */
        uint32_t dlc_tx:4;
        uint32_t reserved_4:1;
        /** rtr_tx : R/W; bitpos: [5]; default: 0;
         *  Configures the rtr bit to be transmitted.
         */
        uint32_t rtr_tx:1;
        /** id_type_tx : R/W; bitpos: [6]; default: 0;
         *  Configures the frame type to be transmitted.
         */
        uint32_t id_type_tx:1;
        /** fr_type_tx : R/W; bitpos: [7]; default: 0;
         *  Configures the fd type to be transmitted.
         */
        uint32_t fr_type_tx:1;
        /** tbf_tx : R/W; bitpos: [8]; default: 0;
         *  Configures the tbf bit to be transmitted.
         */
        uint32_t tbf_tx:1;
        /** brs_tx : R/W; bitpos: [9]; default: 0;
         *  Configures the brs bit to be transmitted.
         */
        uint32_t brs_tx:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} twaifd_tx_data_0_reg_t;

/** Type of tx_data_1 register
 *  TWAI FD transmit data register 1
 */
typedef union {
    struct {
        /** ts_val_u_tx : R/W; bitpos: [31:0]; default: 0;
         *  Configures the upper timestamp to be transmitted
         */
        uint32_t ts_val_u_tx:32;
    };
    uint32_t val;
} twaifd_tx_data_1_reg_t;

/** Type of tx_data_2 register
 *  TWAI FD transmit data register 2
 */
typedef union {
    struct {
        /** ts_val_l_tx : R/W; bitpos: [31:0]; default: 0;
         *  Configures the lower timestamp to be transmitted
         */
        uint32_t ts_val_l_tx:32;
    };
    uint32_t val;
} twaifd_tx_data_2_reg_t;

/** Type of tx_data_3 register
 *  TWAI FD transmit data register 3
 */
typedef union {
    struct {
        /** id_ext_tx : R/W; bitpos: [17:0]; default: 0;
         *  Configures the base ID to be transmitted
         */
        uint32_t id_ext_tx:18;
        /** id_base_tx : R/W; bitpos: [28:18]; default: 0;
         *  Configures the extended ID to be transmitted
         */
        uint32_t id_base_tx:11;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_tx_data_3_reg_t;

/** Type of tx_data_4 register
 *  TWAI FD transmit data register 4
 */
typedef union {
    struct {
        /** tx_data0 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 0th word to be transmitted
         */
        uint32_t tx_data0:32;
    };
    uint32_t val;
} twaifd_tx_data_4_reg_t;

/** Type of tx_data_5 register
 *  TWAI FD transmit data register 5
 */
typedef union {
    struct {
        /** tx_data1 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 1th word to be transmitted
         */
        uint32_t tx_data1:32;
    };
    uint32_t val;
} twaifd_tx_data_5_reg_t;

/** Type of tx_data_6 register
 *  TWAI FD transmit data register 6
 */
typedef union {
    struct {
        /** tx_data2 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 2th word to be transmitted
         */
        uint32_t tx_data2:32;
    };
    uint32_t val;
} twaifd_tx_data_6_reg_t;

/** Type of tx_data_7 register
 *  TWAI FD transmit data register 7
 */
typedef union {
    struct {
        /** tx_data3 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 3th word to be transmitted
         */
        uint32_t tx_data3:32;
    };
    uint32_t val;
} twaifd_tx_data_7_reg_t;

/** Type of tx_data_8 register
 *  TWAI FD transmit data register 8
 */
typedef union {
    struct {
        /** tx_data4 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 4th word to be transmitted
         */
        uint32_t tx_data4:32;
    };
    uint32_t val;
} twaifd_tx_data_8_reg_t;

/** Type of tx_data_9 register
 *  TWAI FD transmit data register 9
 */
typedef union {
    struct {
        /** tx_data5 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 5th word to be transmitted
         */
        uint32_t tx_data5:32;
    };
    uint32_t val;
} twaifd_tx_data_9_reg_t;

/** Type of tx_data_10 register
 *  TWAI FD transmit data register 10
 */
typedef union {
    struct {
        /** tx_data6 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 6th word to be transmitted
         */
        uint32_t tx_data6:32;
    };
    uint32_t val;
} twaifd_tx_data_10_reg_t;

/** Type of tx_data_11 register
 *  TWAI FD transmit data register 11
 */
typedef union {
    struct {
        /** tx_data7 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 7th word to be transmitted
         */
        uint32_t tx_data7:32;
    };
    uint32_t val;
} twaifd_tx_data_11_reg_t;

/** Type of tx_data_12 register
 *  TWAI FD transmit data register 12
 */
typedef union {
    struct {
        /** tx_data8 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 8th word to be transmitted
         */
        uint32_t tx_data8:32;
    };
    uint32_t val;
} twaifd_tx_data_12_reg_t;

/** Type of tx_data_13 register
 *  TWAI FD transmit data register 13
 */
typedef union {
    struct {
        /** tx_data9 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 9th word to be transmitted
         */
        uint32_t tx_data9:32;
    };
    uint32_t val;
} twaifd_tx_data_13_reg_t;

/** Type of tx_data_14 register
 *  TWAI FD transmit data register 14
 */
typedef union {
    struct {
        /** tx_data10 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 10th word to be transmitted
         */
        uint32_t tx_data10:32;
    };
    uint32_t val;
} twaifd_tx_data_14_reg_t;

/** Type of tx_data_15 register
 *  TWAI FD transmit data register 15
 */
typedef union {
    struct {
        /** tx_data11 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 11th word to be transmitted
         */
        uint32_t tx_data11:32;
    };
    uint32_t val;
} twaifd_tx_data_15_reg_t;

/** Type of tx_data_16 register
 *  TWAI FD transmit data register 16
 */
typedef union {
    struct {
        /** tx_data12 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 12th word to be transmitted
         */
        uint32_t tx_data12:32;
    };
    uint32_t val;
} twaifd_tx_data_16_reg_t;

/** Type of tx_data_17 register
 *  TWAI FD transmit data register 17
 */
typedef union {
    struct {
        /** tx_data13 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 13th word to be transmitted
         */
        uint32_t tx_data13:32;
    };
    uint32_t val;
} twaifd_tx_data_17_reg_t;

/** Type of tx_data_18 register
 *  TWAI FD transmit data register 18
 */
typedef union {
    struct {
        /** tx_data14 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 14th word to be transmitted
         */
        uint32_t tx_data14:32;
    };
    uint32_t val;
} twaifd_tx_data_18_reg_t;

/** Type of tx_data_19 register
 *  TWAI FD transmit data register 19
 */
typedef union {
    struct {
        /** tx_data15 : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 15th word to be transmitted
         */
        uint32_t tx_data15:32;
    };
    uint32_t val;
} twaifd_tx_data_19_reg_t;

/** Type of tx_cammand_info register
 *  TWAI FD TXT buffer command & information register
 */
typedef union {
    struct {
        /** txtb_sw_set_ety : R/W; bitpos: [0]; default: 0;
         *  a\\
         */
        uint32_t txtb_sw_set_ety:1;
        /** txtb_sw_set_rdy : R/W; bitpos: [1]; default: 0;
         *  a\\
         */
        uint32_t txtb_sw_set_rdy:1;
        /** txtb_sw_set_abt : R/W; bitpos: [2]; default: 0;
         *  a\\
         */
        uint32_t txtb_sw_set_abt:1;
        uint32_t reserved_3:5;
        /** txb1 : R/W; bitpos: [8]; default: 0;
         *  a\\
         */
        uint32_t txb1:1;
        /** txb2 : R/W; bitpos: [9]; default: 0;
         *  a\\
         */
        uint32_t txb2:1;
        /** txb3 : R/W; bitpos: [10]; default: 0;
         *  a\\
         */
        uint32_t txb3:1;
        /** txb4 : R/W; bitpos: [11]; default: 0;
         *  a\\
         */
        uint32_t txb4:1;
        /** txb5 : R/W; bitpos: [12]; default: 0;
         *  a\\
         */
        uint32_t txb5:1;
        /** txb6 : R/W; bitpos: [13]; default: 0;
         *  a\\
         */
        uint32_t txb6:1;
        /** txb7 : R/W; bitpos: [14]; default: 0;
         *  a\\
         */
        uint32_t txb7:1;
        /** txb8 : R/W; bitpos: [15]; default: 0;
         *  a\\
         */
        uint32_t txb8:1;
        /** txt_buf_ctr : R/W; bitpos: [19:16]; default: 0;
         *  a\\
         */
        uint32_t txt_buf_ctr:4;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} twaifd_tx_cammand_info_reg_t;


/** Group: controller register */
/** Type of rx_frm_counter register
 *  TWAI FD received frame counter register
 */
typedef union {
    struct {
        /** rx_counter_val : RO; bitpos: [31:0]; default: 0;
         *  Configures the received frame counters to enable bus traffic measurement.
         */
        uint32_t rx_counter_val:32;
    };
    uint32_t val;
} twaifd_rx_frm_counter_reg_t;

/** Type of tx_frm_counter register
 *  TWAI FD transmitted frame counter register
 */
typedef union {
    struct {
        /** tx_counter_val : RO; bitpos: [31:0]; default: 0;
         *  Configures the transcieved frame counters to enable bus traffic measurement.
         */
        uint32_t tx_counter_val:32;
    };
    uint32_t val;
} twaifd_tx_frm_counter_reg_t;


/** Group: clock register */
/** Type of clk register
 *  TWAI FD clock configuration register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  1'b1: Force clock on for register. 1'b0: Support clock only when application writes
         *  registers.
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} twaifd_clk_reg_t;


/** Group: Version register */
/** Type of date register
 *  TWAI FD version register
 */
typedef union {
    struct {
        /** twaifd_date : R/W; bitpos: [31:0]; default: 35717712;
         *  This is the version register.
         */
        uint32_t twaifd_date:32;
    };
    uint32_t val;
} twaifd_date_reg_t;


typedef struct {
    volatile twaifd_device_id_reg_t device_id;
    volatile twaifd_mode_setting_reg_t mode_setting;
    volatile twaifd_command_reg_t command;
    volatile twaifd_status_reg_t status;
    volatile twaifd_int_raw_reg_t int_raw;
    volatile twaifd_int_ena_reg_t int_ena;
    volatile twaifd_int_st_reg_t int_st;
    volatile twaifd_int_clr_reg_t int_clr;
    volatile twaifd_bit_timing_reg_t bit_timing;
    volatile twaifd_bit_timeing_fd_reg_t bit_timeing_fd;
    volatile twaifd_err_th_stat_reg_t err_th_stat;
    volatile twaifd_error_counters_reg_t error_counters;
    volatile twaifd_error_counters_sp_reg_t error_counters_sp;
    volatile twaifd_ctr_pres_reg_t ctr_pres;
    volatile twaifd_rx_mem_info_reg_t rx_mem_info;
    volatile twaifd_rx_pointers_reg_t rx_pointers;
    volatile twaifd_rx_status_setting_reg_t rx_status_setting;
    volatile twaifd_rx_data_reg_t rx_data;
    uint32_t reserved_048[6];
    volatile twaifd_filter_a_mask_reg_t filter_a_mask;
    volatile twaifd_filter_a_val_reg_t filter_a_val;
    volatile twaifd_filter_b_mask_reg_t filter_b_mask;
    volatile twaifd_filter_b_val_reg_t filter_b_val;
    volatile twaifd_filter_c_mask_reg_t filter_c_mask;
    volatile twaifd_filter_c_val_reg_t filter_c_val;
    volatile twaifd_filter_ran_low_reg_t filter_ran_low;
    volatile twaifd_filter_ran_high_reg_t filter_ran_high;
    volatile twaifd_filter_control_reg_t filter_control;
    uint32_t reserved_084[4];
    volatile twaifd_tx_stat_reg_t tx_stat;
    volatile twaifd_tx_cfg_reg_t tx_cfg;
    volatile twaifd_tx_data_0_reg_t tx_data_0;
    volatile twaifd_tx_data_1_reg_t tx_data_1;
    volatile twaifd_tx_data_2_reg_t tx_data_2;
    volatile twaifd_tx_data_3_reg_t tx_data_3;
    volatile twaifd_tx_data_4_reg_t tx_data_4;
    volatile twaifd_tx_data_5_reg_t tx_data_5;
    volatile twaifd_tx_data_6_reg_t tx_data_6;
    volatile twaifd_tx_data_7_reg_t tx_data_7;
    volatile twaifd_tx_data_8_reg_t tx_data_8;
    volatile twaifd_tx_data_9_reg_t tx_data_9;
    volatile twaifd_tx_data_10_reg_t tx_data_10;
    volatile twaifd_tx_data_11_reg_t tx_data_11;
    volatile twaifd_tx_data_12_reg_t tx_data_12;
    volatile twaifd_tx_data_13_reg_t tx_data_13;
    volatile twaifd_tx_data_14_reg_t tx_data_14;
    volatile twaifd_tx_data_15_reg_t tx_data_15;
    volatile twaifd_tx_data_16_reg_t tx_data_16;
    volatile twaifd_tx_data_17_reg_t tx_data_17;
    volatile twaifd_tx_data_18_reg_t tx_data_18;
    volatile twaifd_tx_data_19_reg_t tx_data_19;
    uint32_t reserved_0ec[24];
    volatile twaifd_tx_cammand_info_reg_t tx_cammand_info;
    uint32_t reserved_150[4];
    volatile twaifd_err_cap_retr_ctr_alc_reg_t err_cap_retr_ctr_alc;
    volatile twaifd_trv_delay_ssp_cfg_reg_t trv_delay_ssp_cfg;
    uint32_t reserved_168[6];
    volatile twaifd_rx_frm_counter_reg_t rx_frm_counter;
    volatile twaifd_tx_frm_counter_reg_t tx_frm_counter;
    uint32_t reserved_188;
    volatile twaifd_clk_reg_t clk;
    volatile twaifd_date_reg_t date;
} twaifd_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(twaifd_dev_t) == 0x194, "Invalid size of twaifd_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
