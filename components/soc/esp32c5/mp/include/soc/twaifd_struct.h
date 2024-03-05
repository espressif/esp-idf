/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: ID register */
/** Type of device_id_version register
 *  TWAI FD device id status register
 */
typedef union {
    struct {
        /** device_id : RO; bitpos: [15:0]; default: 51965;
         *  Represents whether CAN IP function is mapped correctly on its base address.
         */
        uint32_t device_id:16;
        /** ver_minor : RO; bitpos: [23:16]; default: 4;
         *  TWAI FD IP version
         */
        uint32_t ver_minor:8;
        /** ver_major : RO; bitpos: [31:24]; default: 2;
         *  TWAI FD IP version
         */
        uint32_t ver_major:8;
    };
    uint32_t val;
} twaifd_device_id_version_reg_t;


/** Group: Configuration register */
/** Type of mode_settings register
 *  TWAI FD mode setting register
 */
typedef union {
    struct {
        /** rst : WO; bitpos: [0]; default: 0;
         *  Soft reset. Writing logic 1 resets CTU CAN FD. After writing logic 1, logic 0 does
         *  not need to be written, this bit
         *  is automatically cleared.
         *  0: invalid
         *  1: reset.
         */
        uint32_t rst:1;
        /** bmm : R/W; bitpos: [1]; default: 0;
         *  Bus monitoring mode. In this mode CTU CAN FD only receives frames and sends only
         *  recessive bits on CAN
         *  bus. When a dominant bit is sent, it is re-routed internally so that bus value is
         *  not changed. When this mode is
         *  enabled, CTU CAN FD will not transmit any frame from TXT Buffers,
         *  0b0 - BMM_DISABLED - Bus monitoring mode disabled.
         *  0b1 - BMM_ENABLED - Bus monitoring mode enabled.
         */
        uint32_t bmm:1;
        /** stm : R/W; bitpos: [2]; default: 0;
         *  Self Test Mode. In this mode transmitted frame is considered valid even if dominant
         *  acknowledge was not received.
         *  0b0 - STM_DISABLED - Self test mode disabled.
         *  0b1 - STM_ENABLED - Self test mode enabled.
         */
        uint32_t stm:1;
        /** afm : R/W; bitpos: [3]; default: 0;
         *  Acceptance Filters Mode. If enabled, only RX frames which pass Frame filters are
         *  stored in RX buffer. If disabled,
         *  every received frame is stored to RX buffer. This bit has meaning only if there is
         *  at least one filter available.
         *  Otherwise, this bit is reserved.
         *  0b0 - AFM_DISABLED - Acceptance filter mode disabled
         *  0b1 - AFM_ENABLED - Acceptance filter mode enabled
         */
        uint32_t afm:1;
        /** fde : R/W; bitpos: [4]; default: 1;
         *  Flexible data rate enable. When flexible data rate is enabled CTU CAN FD recognizes
         *  CAN FD frames (FDF bit
         *  = 1).
         *  0b0 - FDE_DISABLE - Flexible data-rate support disabled.
         *  0b1 - FDE_ENABLE - Flexible data-rate support enabled.
         */
        uint32_t fde:1;
        /** tttm : R/W; bitpos: [5]; default: 0;
         *  Time triggered transmission mode.
         *  0b0 - TTTM_DISABLED -
         *  0b1 - TTTM_ENABLED -
         */
        uint32_t tttm:1;
        /** rom : R/W; bitpos: [6]; default: 0;
         *  Restricted operation mode.
         *  0b0 - ROM_DISABLED - Restricted operation mode is disabled.
         *  0b1 - ROM_ENABLED - Restricted operation mode is enabled.
         */
        uint32_t rom:1;
        /** acf : R/W; bitpos: [7]; default: 0;
         *  Acknowledge Forbidden Mode. When enabled, acknowledge is not sent even if received
         *  CRC matches the calculated
         *  one.
         *  0b0 - ACF_DISABLED - Acknowledge forbidden mode disabled.
         *  0b1 - ACF_ENABLED - Acknowledge forbidden mode enabled.
         */
        uint32_t acf:1;
        /** tstm : R/W; bitpos: [8]; default: 0;
         *  Test Mode. In test mode several registers have special features. Reffer to
         *  description of Test mode for further
         *  Details.
         */
        uint32_t tstm:1;
        /** rxbam : R/W; bitpos: [9]; default: 1;
         *  RX Buffer automatic mode.
         *  0b0 - RXBAM_DISABLED -
         *  0b1 - RXBAM_ENABLED -
         */
        uint32_t rxbam:1;
        /** txbbm : R/W; bitpos: [10]; default: 0;
         *  TXT Buffer Backup mode\\
         *  0b0 - TXBBM_DISABLED - TXT Buffer Backup mode disabled\\
         *  0b1 - TXBBM_ENABLED - TXT Buffer Backup mode enabled\\
         */
        uint32_t txbbm:1;
        /** sam : R/W; bitpos: [11]; default: 0;
         *  Self-acknowledge mode.\\
         *  0b0 - SAM_DISABLE - Do not send dominant ACK bit when CTU CAN FD sends Acknowledge
         *  bit.\\
         *  0b1 - SAM_ENABLE - Send dominant ACK bit when CTU CAN FD transmits CAN frame.\\
         */
        uint32_t sam:1;
        uint32_t reserved_12:4;
        /** rtrle : R/W; bitpos: [16]; default: 0;
         *  Retransmitt Limit Enable. If enabled, CTU CAN FD only attempts to retransmitt each
         *  frame up to RTR_TH
         *  times.
         *  0b0 - RTRLE_DISABLED - Retransmitt limit is disabled.
         *  0b1 - RTRLE_ENABLED - Retransmitt limit is enabled.
         */
        uint32_t rtrle:1;
        /** rtrth : R/W; bitpos: [20:17]; default: 0;
         *  Retransmitt Limit Threshold. Maximal amount of retransmission attempts when
         *  SETTINGS[RTRLE] is en-
         *  Abled.
         */
        uint32_t rtrth:4;
        /** ilbp : R/W; bitpos: [21]; default: 0;
         *  Internal Loop Back mode. When enabled, CTU CAN FD receives any frame it transmitts.
         *  0b0 - INT_LOOP_DISABLED - Internal loop-back is disabled.
         *  0b1 - INT_LOOP_ENABLED - Internal loop-back is enabled.
         */
        uint32_t ilbp:1;
        /** ena : R/W; bitpos: [22]; default: 0;
         *  Main enable bit of CTU CAN FD. When enabled, CTU CAN FD communicates on CAN bus.
         *  When disabled, it
         *  is bus-off and does not take part of CAN bus communication.
         *  0b0 - CTU_CAN_DISABLED - The CAN Core is disabled.
         *  0b1 - CTU_CAN_ENABLED - The CAN Core is enabled.
         */
        uint32_t ena:1;
        /** nisofd : R/W; bitpos: [23]; default: 0;
         *  Non ISO FD. When this bit is set, CTU CAN FD is compliant to NON-ISO CAN FD
         *  specification (no stuff
         *  count field). This bit should be modified only when SETTINGS[ENA]=0.
         *  0b0 - ISO_FD - The CAN Controller conforms to ISO CAN FD specification.
         *  0b1 - NON_ISO_FD - The CAN Controller conforms to NON ISO CAN FD specification.
         *  CANFD 1.0
         */
        uint32_t nisofd:1;
        /** pex : R/W; bitpos: [24]; default: 0;
         *  Protocol exception handling. When this bit is set, CTU CAN FD will start
         *  integration upon detection of protocol
         *  exception. This should be modified only when SETTINGS[ENA] = ’0’.
         *  0b0 - PROTOCOL_EXCEPTION_DISABLED - Protocol exception handling is disabled.
         *  0b1 - PROTOCOL_EXCEPTION_ENABLED - Protocol exception handling is enabled.
         */
        uint32_t pex:1;
        /** tbfbo : R/W; bitpos: [25]; default: 1;
         *  All TXT buffers shall go to "TX failed" state when CTU CAN FD becomes bus-off.
         *  0b0 - TXTBUF_FAILED_BUS_OFF_DISABLED - TXT Buffers dont go to "TX failed" state
         *  when CTU CAN
         *  FD becomes bus-off.
         *  0b1 - TXTBUF_FAILED_BUS_OFF_ENABLED - TXT Buffers go to "TX failed" state when CTU
         *  CAN FD
         *  becomes bus-off.
         */
        uint32_t tbfbo:1;
        /** fdrf : R/W; bitpos: [26]; default: 0;
         *  Frame filters drop Remote frames.
         *  0b0 - DROP_RF_DISABLED - Frame filters accept RTR frames.
         *  0b1 - DROP_RF_ENABLED - Frame filters drop RTR frames.
         */
        uint32_t fdrf:1;
        /** pchke : R/W; bitpos: [27]; default: 0;
         *  Enable Parity checks in TXT Buffers and RX Buffer.
         */
        uint32_t pchke:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} twaifd_mode_settings_reg_t;

/** Type of command register
 *  TWAI FD command register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** rxrpmv : WO; bitpos: [1]; default: 0;
         *  RX Buffer read pointer move.
         */
        uint32_t rxrpmv:1;
        /** rrb : WO; bitpos: [2]; default: 0;
         *  Release RX Buffer. This command flushes RX buffer and resets its memory pointers.
         *  0: invalid
         *  1: delete
         */
        uint32_t rrb:1;
        /** cdo : WO; bitpos: [3]; default: 0;
         *  Clear Data Overrun flag in RX buffer.
         *  0: invalid
         *  1: clear
         */
        uint32_t cdo:1;
        /** ercrst : WO; bitpos: [4]; default: 0;
         *  Error Counters Reset. When unit is bus off, issuing this command will request
         *  erasing TEC, REC counters after
         *  128 consecutive ocurrences of 11 recessive bits. Upon completion, TEC and REC are
         *  erased and fault confinement
         *  State is set to error-active. When unit is not bus-off, or when unit is bus-off due
         *  to being disabled (SETTINGS[ENA]
         *  = ’0’), this command has no effect.
         */
        uint32_t ercrst:1;
        /** rxfcrst : WO; bitpos: [5]; default: 0;
         *  Clear RX bus traffic counter (RX_COUNTER register).
         */
        uint32_t rxfcrst:1;
        /** txfcrst : WO; bitpos: [6]; default: 0;
         *  Clear TX bus traffic counter (TX_COUNTER register).
         */
        uint32_t txfcrst:1;
        /** cpexs : WO; bitpos: [7]; default: 0;
         *  Clear Protocol exception status (STATUS[PEXS]).
         */
        uint32_t cpexs:1;
        /** crxpe : WO; bitpos: [8]; default: 0;
         *  Clear STATUS[RXPE] flag.
         */
        uint32_t crxpe:1;
        /** ctxpe : WO; bitpos: [9]; default: 0;
         *  Clear STATUS[TXPE] flag.
         */
        uint32_t ctxpe:1;
        /** ctxdpe : WO; bitpos: [10]; default: 0;
         *  Clear STATUS[TXDPE] flag.
         */
        uint32_t ctxdpe:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} twaifd_command_reg_t;

/** Type of int_stat register
 *  TWAI FD command register
 */
typedef union {
    struct {
        /** rxi_int_st : R/W1C; bitpos: [0]; default: 0;
         *  The masked interrupt status of TWAIFD_RXI_INT.
         *  Frame received interrupt.
         */
        uint32_t rxi_int_st:1;
        /** txi_int_st : R/W1C; bitpos: [1]; default: 0;
         *  The masked interrupt status of TWAIFD_TXI_INT.
         *  Frame transmitted interrupt.
         */
        uint32_t txi_int_st:1;
        /** ewli_int_st : R/W1C; bitpos: [2]; default: 0;
         *  The masked interrupt status of TWAIFD_EWLI_INT.
         *  Error warning limit interrupt. When both TEC and REC are lower than EWL and one of
         *  the becomes equal to or
         *  higher than EWL, or when both TEC and REC become less than EWL, this interrupt is
         *  generated. When Interrupt
         *  is cleared and REC, or TEC is still equal to or higher than EWL, Interrupt is not
         *  generated again.
         */
        uint32_t ewli_int_st:1;
        /** doi_int_st : R/W1C; bitpos: [3]; default: 0;
         *  The masked interrupt status of TWAIFD_DOI_INT.
         *  Data overrun interrupt. Before this  Interrupt is cleared , STATUS[DOR] must be
         *  cleared to avoid setting of this
         *  Interrupt again.
         */
        uint32_t doi_int_st:1;
        /** fcsi_int_st : R/W1C; bitpos: [4]; default: 0;
         *  The masked interrupt status of TWAIFD_FCSI_INT.
         *  Fault confinement state changed interrupt. Interrupt is set when node turns
         *  error-passive (from error-active),
         *  bus-off (from error-passive) or error-active (from bus-off after reintegration or
         *  from error-passive).
         */
        uint32_t fcsi_int_st:1;
        /** ali_int_st : R/W1C; bitpos: [5]; default: 0;
         *  The masked interrupt status of TWAIFD_ALI_INT.
         *  Arbitration lost interrupt.
         */
        uint32_t ali_int_st:1;
        /** bei_int_st : R/W1C; bitpos: [6]; default: 0;
         *  The masked interrupt status of TWAIFD_BEI_INT.
         *  Bus error interrupt.
         */
        uint32_t bei_int_st:1;
        /** ofi_int_st : R/W1C; bitpos: [7]; default: 0;
         *  The masked interrupt status of TWAIFD_OFI_INT.
         *  Overload frame interrupt.
         */
        uint32_t ofi_int_st:1;
        /** rxfi_int_st : R/W1C; bitpos: [8]; default: 0;
         *  The masked interrupt status of TWAIFD_RXFI_INT.
         *  RX buffer full interrupt.
         */
        uint32_t rxfi_int_st:1;
        /** bsi_int_st : R/W1C; bitpos: [9]; default: 0;
         *  The masked interrupt status of TWAIFD_BSI_INT.
         *  Bit rate shifted interrupt.
         */
        uint32_t bsi_int_st:1;
        /** rbnei_int_st : R/W1C; bitpos: [10]; default: 0;
         *  The masked interrupt status of TWAIFD_RBNEI_INT.
         *  RX buffer not empty interrupt. Clearing this interrupt and not reading out content
         *  of RX Buffer via RX_DATA
         *  will re-activate the interrupt.
         */
        uint32_t rbnei_int_st:1;
        /** txbhci_int_st : R/W1C; bitpos: [11]; default: 0;
         *  The masked interrupt status of TWAIFD_TXBHCI_INT.
         *  TXT buffer HW command interrupt. Anytime TXT buffer receives HW command from CAN
         *  Core which
         *  changes TXT buffer state to "TX OK", "Error" or "Aborted", this interrupt will be
         *  generated.
         */
        uint32_t txbhci_int_st:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} twaifd_int_stat_reg_t;

/** Type of btr register
 *  TWAI FD bit-timing  register
 */
typedef union {
    struct {
        /** prop : R/W; bitpos: [6:0]; default: 5;
         *  Configures the propagation segment of nominal bit rate.
         *  Measurement unit: time quanta
         */
        uint32_t prop:7;
        /** ph1 : R/W; bitpos: [12:7]; default: 3;
         *  Configures the phase 1 segment of nominal bit rate.
         *  Measurement unit: time quanta
         */
        uint32_t ph1:6;
        /** ph2 : R/W; bitpos: [18:13]; default: 5;
         *  Configures the phase 2 segment of nominal bit rate.
         *  Measurement unit: time quanta
         */
        uint32_t ph2:6;
        /** brp : R/W; bitpos: [26:19]; default: 10;
         *  Configures the baud-rate prescaler of nominal bit rate.
         *  Measurement unit: cycle of core clock.
         */
        uint32_t brp:8;
        /** sjw : R/W; bitpos: [31:27]; default: 2;
         *  Represents the synchronization jump width in nominal bit time.
         *  Measurement unit: time quanta
         */
        uint32_t sjw:5;
    };
    uint32_t val;
} twaifd_btr_reg_t;

/** Type of btr_fd register
 *  TWAI FD bit-timing of FD register
 */
typedef union {
    struct {
        /** prop_fd : R/W; bitpos: [5:0]; default: 3;
         *  Configures the propagation segment of data bit rate.
         *  Measurement unit: time quanta
         */
        uint32_t prop_fd:6;
        uint32_t reserved_6:1;
        /** ph1_fd : R/W; bitpos: [11:7]; default: 3;
         *  Configures the phase 1 segment of data bit rate.
         *  Measurement unit: time quanta
         */
        uint32_t ph1_fd:5;
        uint32_t reserved_12:1;
        /** ph2_fd : R/W; bitpos: [17:13]; default: 3;
         *  Configures the phase 2 segment of data bit rate.
         *  Measurement unit: time quanta
         */
        uint32_t ph2_fd:5;
        uint32_t reserved_18:1;
        /** brp_fd : R/W; bitpos: [26:19]; default: 4;
         *  Configures the baud-rate prescaler of data bit rate.
         *  Measurement unit: cycle of core clock.
         */
        uint32_t brp_fd:8;
        /** sjw_fd : R/W; bitpos: [31:27]; default: 2;
         *  Represents the synchronization jump width in data bit time.
         *  Measurement unit: time quanta
         */
        uint32_t sjw_fd:5;
    };
    uint32_t val;
} twaifd_btr_fd_reg_t;

/** Type of trv_delay_ssp_cfg register
 *  TWAI FD transmit delay & secondary sample point configuration register
 */
typedef union {
    struct {
        /** trv_delay_value : RO; bitpos: [6:0]; default: 0;
         *  Measured Transmitter delay in multiple of minimal Time quanta.
         */
        uint32_t trv_delay_value:7;
        uint32_t reserved_7:9;
        /** ssp_offset : R/W; bitpos: [23:16]; default: 10;
         *  Secondary sampling point offset. Value is given as multiple of minimal Time quanta.
         */
        uint32_t ssp_offset:8;
        /** ssp_src : R/W; bitpos: [25:24]; default: 0;
         *  Source of Secondary sampling point.
         *  0b00 - SSP_SRC_MEAS_N_OFFSET - SSP position = TRV_DELAY (Measured Transmitter
         *  delay) + SSP_OFFSET.
         *  0b01 - SSP_SRC_NO_SSP - SSP is not used. Transmitter uses regular Sampling Point
         *  during data bit rate.
         *  0b10 - SSP_SRC_OFFSET - SSP position = SSP_OFFSET. Measured Transmitter delay value
         *  is ignored.
         */
        uint32_t ssp_src:2;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} twaifd_trv_delay_ssp_cfg_reg_t;

/** Type of timer_clk_en register
 *  TWAIFD timer clock force enable register.
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Set this bit to force enable TWAIFD register configuration clock signal.
         */
        uint32_t clk_en:1;
        /** force_rxbuf_mem_clk_on : R/W; bitpos: [1]; default: 0;
         *  Set this bit to force enable TWAIFD RX buffer ram clock signal.
         */
        uint32_t force_rxbuf_mem_clk_on:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} twaifd_timer_clk_en_reg_t;

/** Type of timer_int_raw register
 *  TWAIFD raw interrupt register.
 */
typedef union {
    struct {
        /** timer_overflow_int_raw : R/SS/WTC; bitpos: [0]; default: 0;
         *  The raw bit signal for read_done interrupt.
         */
        uint32_t timer_overflow_int_raw:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} twaifd_timer_int_raw_reg_t;

/** Type of timer_int_st register
 *  TWAIFD interrupt status register.
 */
typedef union {
    struct {
        /** timer_overflow_int_st : RO; bitpos: [0]; default: 0;
         *  The status signal for read_done interrupt.
         */
        uint32_t timer_overflow_int_st:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} twaifd_timer_int_st_reg_t;

/** Type of timer_int_ena register
 *  TWAIFD interrupt enable register.
 */
typedef union {
    struct {
        /** timer_overflow_int_ena : R/W; bitpos: [0]; default: 0;
         *  The enable signal for read_done interrupt.
         */
        uint32_t timer_overflow_int_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} twaifd_timer_int_ena_reg_t;

/** Type of timer_int_clr register
 *  TWAIFD interrupt clear register.
 */
typedef union {
    struct {
        /** timer_overflow_int_clr : WT; bitpos: [0]; default: 0;
         *  The clear signal for read_done interrupt.
         */
        uint32_t timer_overflow_int_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} twaifd_timer_int_clr_reg_t;

/** Type of timer_cfg register
 *  TWAI FD timer configure register.
 */
typedef union {
    struct {
        /** timer_ce : R/W; bitpos: [0]; default: 0;
         *  TWAI FD timer enable register.
         *  1b0: Not enable
         *  1b1: Enable timer
         */
        uint32_t timer_ce:1;
        /** timer_clr : WT; bitpos: [1]; default: 0;
         *  TWAI FD timer clear register.
         *  1b0: Not enable
         *  1b1: Enable to clear value
         */
        uint32_t timer_clr:1;
        /** timer_set : WT; bitpos: [2]; default: 0;
         *  TWAI FD timer set register.
         *  1b0: Not enable
         *  1b1: Enable to set value to ld_val.
         */
        uint32_t timer_set:1;
        uint32_t reserved_3:5;
        /** timer_up_dn : R/W; bitpos: [8]; default: 1;
         *  TWAI FD timer up/down count register.
         *  1b0: Count-down
         *  1b1: Count-up
         */
        uint32_t timer_up_dn:1;
        uint32_t reserved_9:7;
        /** timer_step : R/W; bitpos: [31:16]; default: 0;
         *  TWAI FD timer count step register, step=reg_timer_step+1
         */
        uint32_t timer_step:16;
    };
    uint32_t val;
} twaifd_timer_cfg_reg_t;

/** Type of timer_ld_val_l register
 *  TWAI FD timer pre-load value low register.
 */
typedef union {
    struct {
        /** timer_ld_val_l : R/W; bitpos: [31:0]; default: 0;
         *  TWAI FD timer count pre-load value register, low part.
         */
        uint32_t timer_ld_val_l:32;
    };
    uint32_t val;
} twaifd_timer_ld_val_l_reg_t;

/** Type of timer_ld_val_h register
 *  TWAI FD timer pre-load value high register.
 */
typedef union {
    struct {
        /** timer_ld_val_h : R/W; bitpos: [31:0]; default: 0;
         *  TWAI FD timer pre-load value register, high part.
         *  If timestamp valid bit-width less than 33, this field is ignored.
         */
        uint32_t timer_ld_val_h:32;
    };
    uint32_t val;
} twaifd_timer_ld_val_h_reg_t;

/** Type of timer_ct_val_l register
 *  TWAI FD timer count-to value low register.
 */
typedef union {
    struct {
        /** timer_ct_val_l : R/W; bitpos: [31:0]; default: 4294967295;
         *  TWAI FD timer count-to value register, low part.
         */
        uint32_t timer_ct_val_l:32;
    };
    uint32_t val;
} twaifd_timer_ct_val_l_reg_t;

/** Type of timer_ct_val_h register
 *  TWAI FD timer count-to value high register.
 */
typedef union {
    struct {
        /** timer_ct_val_h : R/W; bitpos: [31:0]; default: 4294967295;
         *  TWAI FD timer count-to value register, high part.
         *  If timestamp valid bit-width less than 33, this field is ignored.
         */
        uint32_t timer_ct_val_h:32;
    };
    uint32_t val;
} twaifd_timer_ct_val_h_reg_t;


/** Group: Status register */
/** Type of status register
 *  TWAI FD status register
 */
typedef union {
    struct {
        /** rxne : RO; bitpos: [0]; default: 0;
         *  RX buffer not empty. This bit is 1 when least one frame is stored in RX buffer.
         *  0: empty
         *  1: not empty
         */
        uint32_t rxne:1;
        /** dor : RO; bitpos: [1]; default: 0;
         *  Data Overrun flag. This bit is set when frame was dropped due to lack of space in
         *  RX buffer. This bit can be
         *  cleared by COMMAND[RRB].
         *  0: not overrun
         *  1: overrun
         */
        uint32_t dor:1;
        /** txnf : RO; bitpos: [2]; default: 1;
         *  TXT buffers status. This bit is set if at least one TXT buffer is in "Empty" state.
         *  0: not full
         *  1: full
         */
        uint32_t txnf:1;
        /** eft : RO; bitpos: [3]; default: 0;
         *  Error frame is being transmitted at the moment.
         *  0: not being transmitted
         *  1: being transmitted
         */
        uint32_t eft:1;
        /** rxs : RO; bitpos: [4]; default: 0;
         *  CTU CAN FD is receiver of CAN Frame.
         *  0: not receiving
         *  1: receiving
         */
        uint32_t rxs:1;
        /** txs : RO; bitpos: [5]; default: 0;
         *  CTU CAN FD is transmitter of CAN Frame.
         *  0: not transmitting
         *  1: transmitting
         */
        uint32_t txs:1;
        /** ewl : RO; bitpos: [6]; default: 0;
         *  TX Error counter (TEC) or RX Error counter (REC) is equal to, or higher than Error
         *  warning limit (EWL).
         *  0: not reached
         *  1: reached
         */
        uint32_t ewl:1;
        /** idle : RO; bitpos: [7]; default: 1;
         *  Bus is idle (no frame is being transmitted/received) or CTU CAN FD is bus-off.
         *  0: active
         *  1: not active
         */
        uint32_t idle:1;
        /** pexs : RO; bitpos: [8]; default: 0;
         *  Protocol exception status (flag). Set when Protocol exception occurs. Cleared by
         *  writing COMMAND[CPEXS]=1.
         */
        uint32_t pexs:1;
        /** rxpe : RO; bitpos: [9]; default: 0;
         *  Set when parity error is detected during read of CAN frame from RX Buffer via
         *  RX_DATA register.
         */
        uint32_t rxpe:1;
        /** txpe : RO; bitpos: [10]; default: 0;
         *  TXT Buffers Parity Error flag. Set When Parity Error is detected in a TXT Buffer
         *  during transmission from this
         *  Buffer.
         */
        uint32_t txpe:1;
        /** txdpe : RO; bitpos: [11]; default: 0;
         *  TXT Buffer double parity error. Set in TXT Buffer Backup mode when parity error is
         *  detected in "backup"
         *  TXT Buffer.
         */
        uint32_t txdpe:1;
        uint32_t reserved_12:4;
        /** stcnt : RO; bitpos: [16]; default: 1;
         *  Support of Traffic counters. When this bit is 1, Traffic counters are present.
         */
        uint32_t stcnt:1;
        /** strgs : RO; bitpos: [17]; default: 1;
         *  Support of Test Registers for memory testability. When this bit is 1, Test
         *  Registers are present.
         */
        uint32_t strgs:1;
        /** sprt : RO; bitpos: [18]; default: 1;
         *  Support of Parity protection on each word of TXT Buffer RAM and RX Buffer RAM.
         */
        uint32_t sprt:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} twaifd_status_reg_t;

/** Type of rx_mem_info register
 *  TWAI FD rx memory information register
 */
typedef union {
    struct {
        /** rx_buff_size : RO; bitpos: [12:0]; default: 128;
         *  Size of RX buffer in 32-bit words.
         */
        uint32_t rx_buff_size:13;
        uint32_t reserved_13:3;
        /** rx_free : RO; bitpos: [28:16]; default: 128;
         *  Number of free 32 bit words in RX buffer.
         */
        uint32_t rx_free:13;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_rx_mem_info_reg_t;

/** Type of rx_pointers register
 *  TWAI FD rx memory pointer information register
 */
typedef union {
    struct {
        /** rx_wpp : RO; bitpos: [11:0]; default: 0;
         *  Write pointer position in RX buffer. Upon store of received frame write pointer is
         *  updated.
         */
        uint32_t rx_wpp:12;
        uint32_t reserved_12:4;
        /** rx_rpp : RO; bitpos: [27:16]; default: 0;
         *  Read pointer position in RX buffer. Upon read of received frame read pointer is
         *  updated.
         */
        uint32_t rx_rpp:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} twaifd_rx_pointers_reg_t;

/** Type of rx_status_rx_settings register
 *  TWAI FD rx status & setting register
 */
typedef union {
    struct {
        /** rxe : RO; bitpos: [0]; default: 1;
         *  Represents whether or not the RX buffer is empty. RX buffer is empty. There is no
         *  CAN Frame stored in it.
         *  0: not empty
         *  1: empty
         */
        uint32_t rxe:1;
        /** rxf : RO; bitpos: [1]; default: 0;
         *  Represents whether or not the RX buffer is full. RX buffer is full, all memory
         *  words of RX buffer are occupied.
         *  0: not full
         *  1: full
         */
        uint32_t rxf:1;
        /** rxmof : RO; bitpos: [2]; default: 0;
         *  Represents the number of received frame in RX buffer.
         *  RX Buffer middle of frame. When RXMOF = 1, next read from RX_DATA register will
         *  return other than first
         *  word (FRAME_FORMAT_W) of CAN frame.
         */
        uint32_t rxmof:1;
        uint32_t reserved_3:1;
        /** rxfrc : RO; bitpos: [14:4]; default: 0;
         *  RX buffer frame count. Number of CAN frames stored in RX buffer.
         */
        uint32_t rxfrc:11;
        uint32_t reserved_15:1;
        /** rtsop : R/W; bitpos: [16]; default: 0;
         *  Receive buffer timestamp option. This register should be modified only when
         *  SETTINGS[ENA]=0.
         *  0b0 - RTS_END - Timestamp of received frame in RX FIFO is captured in last bit of
         *  EOF field.
         *  0b1 - RTS_BEG - Timestamp of received frame in RX FIFO is captured in SOF field.
         */
        uint32_t rtsop:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} twaifd_rx_status_rx_settings_reg_t;

/** Type of tx_status register
 *  TWAI FD TX buffer status register
 */
typedef union {
    struct {
        /** txtb0_state : RO; bitpos: [3:0]; default: 8;
         *  Status of TXT buffer 1.
         *  0b0000 - TXT_NOT_EXIST - TXT buffer does not exist in the core (applies only to TXT
         *  buffers 3-8, when CTU
         *  CAN FD was synthesized with less than 8 TXT buffers).
         *  0b0001 - TXT_RDY - TXT buffer is in "Ready" state, it is waiting for CTU CAN FD to
         *  start transmission from it.
         *  0b0010 - TXT_TRAN - TXT buffer is in "TX in progress" state. CTU CAN FD is
         *  transmitting frame.
         *  0b0011 - TXT_ABTP - TXT buffer is in "Abort in progress" state.
         *  0b0100 - TXT_TOK - TXT buffer is in "TX OK" state.
         *  0b0110 - TXT_ERR - TXT buffer is in "Failed" state.
         *  0b0111 - TXT_ABT - TXT buffer is in "Aborted" state.
         *  0b1000 - TXT_ETY - TXT buffer is in "Empty" state.
         */
        uint32_t txtb0_state:4;
        /** tx2s : RO; bitpos: [7:4]; default: 8;
         *  Status of TXT buffer 2. Bit field meaning is analogous to TX1S.
         */
        uint32_t tx2s:4;
        /** tx3s : RO; bitpos: [11:8]; default: 8;
         *  Status of TXT buffer 3. Bit field meaning is analogous to TX1S.
         */
        uint32_t tx3s:4;
        /** tx4s : RO; bitpos: [15:12]; default: 8;
         *  Status of TXT buffer 4. Bit field meaning is analogous to TX1S.
         */
        uint32_t tx4s:4;
        /** tx5s : RO; bitpos: [19:16]; default: 0;
         *  Status of TXT buffer 5. Bit field meaning is analogous to TX1S.
         */
        uint32_t tx5s:4;
        /** tx6s : RO; bitpos: [23:20]; default: 0;
         *  Status of TXT buffer 6. Bit field meaning is analogous to TX1S.
         */
        uint32_t tx6s:4;
        /** tx7s : RO; bitpos: [27:24]; default: 0;
         *  Status of TXT buffer 7. Bit field meaning is analogous to TX1S.
         */
        uint32_t tx7s:4;
        /** tx8s : RO; bitpos: [31:28]; default: 0;
         *  Status of TXT buffer 8. Bit field meaning is analogous to TX1S.
         */
        uint32_t tx8s:4;
    };
    uint32_t val;
} twaifd_tx_status_reg_t;

/** Type of err_capt_retr_ctr_alc_ts_info register
 *  TWAI FD error capture & retransmit counter & arbitration lost & timestamp
 *  integration information register
 */
typedef union {
    struct {
        /** err_pos : RO; bitpos: [4:0]; default: 31;
         *  0b00000 - ERC_POS_SOF - Error in Start of Frame
         *  0b00001 - ERC_POS_ARB - Error in Arbitration Filed
         *  0b00010 - ERC_POS_CTRL - Error in Control field
         *  0b00011 - ERC_POS_DATA - Error in Data Field
         *  0b00100 - ERC_POS_CRC - Error in CRC Field
         *  0b00101 - ERC_POS_ACK - Error in CRC delimiter, ACK field or ACK delimiter
         *  0b00110 - ERC_POS_EOF - Error in End of frame field
         *  0b00111 - ERC_POS_ERR - Error during Error frame
         *  0b01000 - ERC_POS_OVRL - Error in Overload frame
         *  0b11111 - ERC_POS_OTHER - Other position of error
         */
        uint32_t err_pos:5;
        /** err_type : RO; bitpos: [7:5]; default: 0;
         *  Type of last error.
         *  0b000 - ERC_BIT_ERR - Bit Error
         *  0b001 - ERC_CRC_ERR - CRC Error
         *  0b010 - ERC_FRM_ERR - Form Error
         *  0b011 - ERC_ACK_ERR - Acknowledge Error
         *  0b100 - ERC_STUF_ERR - Stuff Error
         */
        uint32_t err_type:3;
        /** retr_ctr_val : RO; bitpos: [11:8]; default: 0;
         *  Current value of retransmitt counter.
         */
        uint32_t retr_ctr_val:4;
        uint32_t reserved_12:4;
        /** alc_bit : RO; bitpos: [20:16]; default: 0;
         *  Arbitration lost capture bit position. If ALC_ID_FIELD = ALC_BASE_ID then bit index
         *  of BASE identifier
         *  in which arbitration was lost is given as: 11 - ALC_VAL. If ALC_ID_FIELD =
         *  ALC_EXTENSION then bit index of
         *  EXTENDED identifier in which arbitration was lost is given as: 18 - ALC_VAL. For
         *  other values of ALC_ID_FIELD,
         *  this value is undefined.
         */
        uint32_t alc_bit:5;
        /** alc_id_field : RO; bitpos: [23:21]; default: 0;
         *  Part of CAN Identifier in which arbitration was lost.
         *  0b000 - ALC_RSVD - Unit did not loose arbitration since last reset.
         *  0b001 - ALC_BASE_ID - Arbitration was lost during base identifier.
         *  0b010 - ALC_SRR_RTR - Arbitration was lost during first bit after base identifier
         *  (SRR of Extended Frame, RTR
         *  bit of CAN 2.0 Base Frame)
         *  0b011 - ALC_IDE - Arbitration was lost during IDE bit.
         *  0b100 - ALC_EXTENSION - Arbitration was lost during Identifier extension.
         *  0b101 - ALC_RTR - Arbitration was lost during RTR bit after Identifier extension!
         */
        uint32_t alc_id_field:3;
        /** ts_bits : RO; bitpos: [29:24]; default: 0;
         *  Number of active bits of CTU CAN FD time-base minus 1 (0x3F = 64 bit time-base).
         */
        uint32_t ts_bits:6;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} twaifd_err_capt_retr_ctr_alc_ts_info_reg_t;

/** Type of yolo register
 *  TWAI FD transmitted frame counter register
 */
typedef union {
    struct {
        /** yolo_val : RO; bitpos: [31:0]; default: 3735928559;
         *  What else could be in this register??
         */
        uint32_t yolo_val:32;
    };
    uint32_t val;
} twaifd_yolo_reg_t;

/** Type of timestamp_low register
 *  TWAI FD transmitted frame counter register
 */
typedef union {
    struct {
        /** timestamp_low : RO; bitpos: [31:0]; default: 0;
         *  Bits 31:0 of time base.
         */
        uint32_t timestamp_low:32;
    };
    uint32_t val;
} twaifd_timestamp_low_reg_t;

/** Type of timestamp_high register
 *  TWAI FD transmitted frame counter register
 */
typedef union {
    struct {
        /** timestamp_high : RO; bitpos: [31:0]; default: 0;
         *  Bits 63:32 of time base.
         */
        uint32_t timestamp_high:32;
    };
    uint32_t val;
} twaifd_timestamp_high_reg_t;


/** Group: interrupt register */
/** Type of int_ena_set register
 *  TWAI FD interrupt enable register
 */
typedef union {
    struct {
        /** rxi_int_ena_mask : R/W1S; bitpos: [0]; default: 0;
         *  Write 1 to enable TWAIFD_RXI_INT.
         */
        uint32_t rxi_int_ena_mask:1;
        /** txi_int_ena_mask : R/W1S; bitpos: [1]; default: 0;
         *  Write 1 to enable TWAIFD_TXI_INT.
         */
        uint32_t txi_int_ena_mask:1;
        /** ewli_int_ena_mask : R/W1S; bitpos: [2]; default: 0;
         *  Write 1 to enable TWAIFD_EWLI_INT.
         */
        uint32_t ewli_int_ena_mask:1;
        /** doi_int_ena_mask : R/W1S; bitpos: [3]; default: 0;
         *  Write 1 to enable TWAIFD_DOI_INT.
         */
        uint32_t doi_int_ena_mask:1;
        /** fcsi_int_ena_mask : R/W1S; bitpos: [4]; default: 0;
         *  Write 1 to enable TWAIFD_FCSI_INT.
         */
        uint32_t fcsi_int_ena_mask:1;
        /** ali_int_ena_mask : R/W1S; bitpos: [5]; default: 0;
         *  Write 1 to enable TWAIFD_ALI_INT.
         */
        uint32_t ali_int_ena_mask:1;
        /** bei_int_ena_mask : R/W1S; bitpos: [6]; default: 0;
         *  Write 1 to enable TWAIFD_BEI_INT.
         */
        uint32_t bei_int_ena_mask:1;
        /** ofi_int_ena_mask : R/W1S; bitpos: [7]; default: 0;
         *  Write 1 to enable TWAIFD_OFI_INT.
         */
        uint32_t ofi_int_ena_mask:1;
        /** rxfi_int_ena_mask : R/W1S; bitpos: [8]; default: 0;
         *  Write 1 to enable TWAIFD_RXFI_INT.
         */
        uint32_t rxfi_int_ena_mask:1;
        /** bsi_int_ena_mask : R/W1S; bitpos: [9]; default: 0;
         *  Write 1 to enable TWAIFD_BSI_INT.
         */
        uint32_t bsi_int_ena_mask:1;
        /** rbnei_int_ena_mask : R/W1S; bitpos: [10]; default: 0;
         *  Write 1 to enable TWAIFD_RBNEI_INT.
         */
        uint32_t rbnei_int_ena_mask:1;
        /** txbhci_int_ena_mask : R/W1S; bitpos: [11]; default: 0;
         *  Write 1 to enable TWAIFD_TXBHCI_INT.
         */
        uint32_t txbhci_int_ena_mask:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} twaifd_int_ena_set_reg_t;

/** Type of int_ena_clr register
 *  TWAI FD interrupt enable clear register
 */
typedef union {
    struct {
        /** rxi_int_ena_clr : WO; bitpos: [0]; default: 0;
         *  Write 1 to clear TWAIFD_RXI_INT_ENA .
         */
        uint32_t rxi_int_ena_clr:1;
        /** txi_int_ena_clr : WO; bitpos: [1]; default: 0;
         *  Write 1 to clear TWAIFD_TXI_INT_ENA .
         */
        uint32_t txi_int_ena_clr:1;
        /** ewli_int_ena_clr : WO; bitpos: [2]; default: 0;
         *  Write 1 to clear TWAIFD_EWLI_INT_ENA .
         */
        uint32_t ewli_int_ena_clr:1;
        /** doi_int_ena_clr : WO; bitpos: [3]; default: 0;
         *  Write 1 to clear TWAIFD_DOI_INT_ENA .
         */
        uint32_t doi_int_ena_clr:1;
        /** fcsi_int_ena_clr : WO; bitpos: [4]; default: 0;
         *  Write 1 to clear TWAIFD_FCSI_INT_ENA .
         */
        uint32_t fcsi_int_ena_clr:1;
        /** ali_int_ena_clr : WO; bitpos: [5]; default: 0;
         *  Write 1 to clear TWAIFD_ALI_INT_ENA .
         */
        uint32_t ali_int_ena_clr:1;
        /** bei_int_ena_clr : WO; bitpos: [6]; default: 0;
         *  Write 1 to clear TWAIFD_BEI_INT_ENA .
         */
        uint32_t bei_int_ena_clr:1;
        /** ofi_int_ena_clr : WO; bitpos: [7]; default: 0;
         *  Write 1 to clear TWAIFD_OFI_INT_ENA .
         */
        uint32_t ofi_int_ena_clr:1;
        /** rxfi_int_ena_clr : WO; bitpos: [8]; default: 0;
         *  Write 1 to clear TWAIFD_RXFI_INT_ENA .
         */
        uint32_t rxfi_int_ena_clr:1;
        /** bsi_int_ena_clr : WO; bitpos: [9]; default: 0;
         *  Write 1 to clear TWAIFD_BSI_INT_ENA .
         */
        uint32_t bsi_int_ena_clr:1;
        /** rbnei_int_ena_clr : WO; bitpos: [10]; default: 0;
         *  Write 1 to clear TWAIFD_RBNEI_INT_ENA .
         */
        uint32_t rbnei_int_ena_clr:1;
        /** txbhci_int_ena_clr : WO; bitpos: [11]; default: 0;
         *  Write 1 to clear TWAIFD_TXBHCI_INT_ENA .
         */
        uint32_t txbhci_int_ena_clr:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} twaifd_int_ena_clr_reg_t;

/** Type of int_mask_set register
 *  TWAI FD interrupt mask register
 */
typedef union {
    struct {
        /** rxi_int_mask_set : R/W1S; bitpos: [0]; default: 0;
         *  Write 1 to mask TWAIFD_RXI_INT.
         */
        uint32_t rxi_int_mask_set:1;
        /** txi_int_mask_set : R/W1S; bitpos: [1]; default: 0;
         *  Write 1 to mask TWAIFD_TXI_INT.
         */
        uint32_t txi_int_mask_set:1;
        /** ewli_int_mask_set : R/W1S; bitpos: [2]; default: 0;
         *  Write 1 to mask TWAIFD_EWLI_INT.
         */
        uint32_t ewli_int_mask_set:1;
        /** doi_int_mask_set : R/W1S; bitpos: [3]; default: 0;
         *  Write 1 to mask TWAIFD_DOI_INT.
         */
        uint32_t doi_int_mask_set:1;
        /** fcsi_int_mask_set : R/W1S; bitpos: [4]; default: 0;
         *  Write 1 to mask TWAIFD_FCSI_INT.
         */
        uint32_t fcsi_int_mask_set:1;
        /** ali_int_mask_set : R/W1S; bitpos: [5]; default: 0;
         *  Write 1 to mask TWAIFD_ALI_INT.
         */
        uint32_t ali_int_mask_set:1;
        /** bei_int_mask_set : R/W1S; bitpos: [6]; default: 0;
         *  Write 1 to mask TWAIFD_BEI_INT.
         */
        uint32_t bei_int_mask_set:1;
        /** ofi_int_mask_set : R/W1S; bitpos: [7]; default: 0;
         *  Write 1 to mask TWAIFD_OFI_INT.
         */
        uint32_t ofi_int_mask_set:1;
        /** rxfi_int_mask_set : R/W1S; bitpos: [8]; default: 0;
         *  Write 1 to mask TWAIFD_RXFI_INT.
         */
        uint32_t rxfi_int_mask_set:1;
        /** bsi_int_mask_set : R/W1S; bitpos: [9]; default: 0;
         *  Write 1 to mask TWAIFD_BSI_INT.
         */
        uint32_t bsi_int_mask_set:1;
        /** rbnei_int_mask_set : R/W1S; bitpos: [10]; default: 0;
         *  Write 1 to mask TWAIFD_RBNEI_INT.
         */
        uint32_t rbnei_int_mask_set:1;
        /** txbhci_int_mask_set : R/W1S; bitpos: [11]; default: 0;
         *  Write 1 to mask TWAIFD_TXBHCI_INT.
         */
        uint32_t txbhci_int_mask_set:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} twaifd_int_mask_set_reg_t;

/** Type of int_mask_clr register
 *  TWAI FD interrupt mask clear register
 */
typedef union {
    struct {
        /** rxi_int_mask_clr : WO; bitpos: [0]; default: 0;
         *  Write 1 to clear TWAIFD_RXI_INT_MASK_CLR .
         */
        uint32_t rxi_int_mask_clr:1;
        /** txi_int_mask_clr : WO; bitpos: [1]; default: 0;
         *  Write 1 to clear TWAIFD_TXI_INT_MASK_CLR .
         */
        uint32_t txi_int_mask_clr:1;
        /** ewli_int_mask_clr : WO; bitpos: [2]; default: 0;
         *  Write 1 to clear TWAIFD_EWLI_INT_MASK_CLR .
         */
        uint32_t ewli_int_mask_clr:1;
        /** doi_int_mask_clr : WO; bitpos: [3]; default: 0;
         *  Write 1 to clear TWAIFD_DOI_INT_MASK_CLR .
         */
        uint32_t doi_int_mask_clr:1;
        /** fcsi_int_mask_clr : WO; bitpos: [4]; default: 0;
         *  Write 1 to clear TWAIFD_FCSI_INT_MASK_CLR .
         */
        uint32_t fcsi_int_mask_clr:1;
        /** ali_int_mask_clr : WO; bitpos: [5]; default: 0;
         *  Write 1 to clear TWAIFD_ALI_INT_MASK_CLR .
         */
        uint32_t ali_int_mask_clr:1;
        /** bei_int_mask_clr : WO; bitpos: [6]; default: 0;
         *  Write 1 to clear TWAIFD_BEI_INT_MASK_CLR .
         */
        uint32_t bei_int_mask_clr:1;
        /** ofi_int_mask_clr : WO; bitpos: [7]; default: 0;
         *  Write 1 to clear TWAIFD_OFI_INT_MASK_CLR .
         */
        uint32_t ofi_int_mask_clr:1;
        /** rxfi_int_mask_clr : WO; bitpos: [8]; default: 0;
         *  Write 1 to clear TWAIFD_RXFI_INT_MASK_CLR .
         */
        uint32_t rxfi_int_mask_clr:1;
        /** bsi_int_mask_clr : WO; bitpos: [9]; default: 0;
         *  Write 1 to clear TWAIFD_BSI_INT_MASK_CLR .
         */
        uint32_t bsi_int_mask_clr:1;
        /** rbnei_int_mask_clr : WO; bitpos: [10]; default: 0;
         *  Write 1 to clear TWAIFD_RBNEI_INT_MASK_CLR .
         */
        uint32_t rbnei_int_mask_clr:1;
        /** txbhci_int_mask_clr : WO; bitpos: [11]; default: 0;
         *  Write 1 to clear TWAIFD_TXBHCI_INT_MASK_CLR .
         */
        uint32_t txbhci_int_mask_clr:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} twaifd_int_mask_clr_reg_t;


/** Group: error confinement register */
/** Type of ewl_erp_fault_state register
 *  TWAI FD error threshold and status register
 */
typedef union {
    struct {
        /** ew_limit : R/W; bitpos: [7:0]; default: 96;
         *  Error warning limit. If error warning limit is reached interrupt can be generated.
         *  Error warning limit
         *  indicates heavily disturbed bus.
         */
        uint32_t ew_limit:8;
        /** erp_limit : R/W; bitpos: [15:8]; default: 128;
         *  Error Passive Limit. When one of error counters (REC/TEC) exceeds this value, Fault
         *  confinement state
         *  changes to error-passive.
         */
        uint32_t erp_limit:8;
        /** era : RO; bitpos: [16]; default: 0;
         *  Represents the fault state of error active.
         */
        uint32_t era:1;
        /** erp : RO; bitpos: [17]; default: 0;
         *  Represents the fault state of error passive.
         */
        uint32_t erp:1;
        /** bof : RO; bitpos: [18]; default: 1;
         *  Represents the fault state of bus off.
         */
        uint32_t bof:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} twaifd_ewl_erp_fault_state_reg_t;

/** Type of rec_tec register
 *  TWAI FD error counters status register
 */
typedef union {
    struct {
        /** rec_val : RO; bitpos: [8:0]; default: 0;
         *  Represents the receiver error counter value.
         */
        uint32_t rec_val:9;
        uint32_t reserved_9:7;
        /** tec_val : RO; bitpos: [24:16]; default: 0;
         *  Represents the transmitter error counter value.
         */
        uint32_t tec_val:9;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} twaifd_rec_tec_reg_t;

/** Type of err_norm_err_fd register
 *  TWAI FD special error counters status register
 */
typedef union {
    struct {
        /** err_norm_val : RO; bitpos: [15:0]; default: 0;
         *  Represents the number of error in the nominal bit time.
         */
        uint32_t err_norm_val:16;
        /** err_fd_val : RO; bitpos: [31:16]; default: 0;
         *  Represents the number of error in the data bit time.
         */
        uint32_t err_fd_val:16;
    };
    uint32_t val;
} twaifd_err_norm_err_fd_reg_t;

/** Type of ctr_pres register
 *  TWAI FD error counters pre-define configuration register
 */
typedef union {
    struct {
        /** ctpv : WO; bitpos: [8:0]; default: 0;
         *  Configures the pre-defined value to set the error counter.
         */
        uint32_t ctpv:9;
        /** ptx : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to set the receiver error counter into the value of
         *  pre-defined value.
         *  0: invalid
         *  1: set
         */
        uint32_t ptx:1;
        /** prx : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to set the transmitter error counter into the value of
         *  pre-defined value.
         *  0: invalid
         *  1: set
         */
        uint32_t prx:1;
        /** enorm : WO; bitpos: [11]; default: 0;
         *  Configures whether or not to erase the error counter of nominal bit time.
         *  0: invalid
         *  1: erase
         */
        uint32_t enorm:1;
        /** efd : WO; bitpos: [12]; default: 0;
         *  Configures whether or not to erase the error counter of data bit time.
         *  0: invalid
         *  1: erase
         */
        uint32_t efd:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} twaifd_ctr_pres_reg_t;


/** Group: filter register */
/** Type of filter_a_mask register
 *  TWAI FD filter A mask value register
 */
typedef union {
    struct {
        /** bit_mask_a_val : R/W; bitpos: [28:0]; default: 0;
         *  Filter A mask. The identifier format is the same as in IDENTIFIER_W of TXT buffer
         *  or RX
         *  buffer. If filter A is not present, writes to this register have no effect and read
         *  will return all zeroes.
         */
        uint32_t bit_mask_a_val:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_a_mask_reg_t;

/** Type of filter_a_val register
 *  TWAI FD filter A bit value register
 */
typedef union {
    struct {
        /** bit_val_a_val : R/W; bitpos: [28:0]; default: 0;
         *  Filter A value. The identifier format is the same as in IDENTIFIER_W of TXT buffer
         *  or RX buffer.
         *  If filter A is not present, writes to this register have no effect and read will
         *  return all zeroes.
         */
        uint32_t bit_val_a_val:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_a_val_reg_t;

/** Type of filter_b_mask register
 *  TWAI FD filter B mask value register
 */
typedef union {
    struct {
        /** bit_mask_b_val : R/W; bitpos: [28:0]; default: 0;
         *  Filter B mask. The identifier format is the same as in IDENTIFIER_W of TXT buffer
         *  or RX
         *  buffer. If filter A is not present, writes to this register have no effect and read
         *  will return all zeroes.
         */
        uint32_t bit_mask_b_val:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_b_mask_reg_t;

/** Type of filter_b_val register
 *  TWAI FD filter B bit value register
 */
typedef union {
    struct {
        /** bit_val_b_val : R/W; bitpos: [28:0]; default: 0;
         *  Filter B value. The identifier format is the same as in IDENTIFIER_W of TXT buffer
         *  or RX buffer.
         *  If filter A is not present, writes to this register have no effect and read will
         *  return all zeroes.
         */
        uint32_t bit_val_b_val:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_b_val_reg_t;

/** Type of filter_c_mask register
 *  TWAI FD filter C mask value register
 */
typedef union {
    struct {
        /** bit_mask_c_val : R/W; bitpos: [28:0]; default: 0;
         *  Filter C mask. The identifier format is the same as in IDENTIFIER_W of TXT buffer
         *  or RX
         *  buffer. If filter A is not present, writes to this register have no effect and read
         *  will return all zeroes.
         */
        uint32_t bit_mask_c_val:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_c_mask_reg_t;

/** Type of filter_c_val register
 *  TWAI FD filter C bit value register
 */
typedef union {
    struct {
        /** bit_val_c_val : R/W; bitpos: [28:0]; default: 0;
         *  Filter C value. The identifier format is the same as in IDENTIFIER_W of TXT buffer
         *  or RX buffer.
         *  If filter A is not present, writes to this register have no effect and read will
         *  return all zeroes.
         */
        uint32_t bit_val_c_val:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_c_val_reg_t;

/** Type of filter_ran_low register
 *  TWAI FD filter range low value register
 */
typedef union {
    struct {
        /** bit_ran_low_val : R/W; bitpos: [28:0]; default: 0;
         *  Filter Range Low threshold. The identifier format is the same as in IDENTIFIER_W of
         *  TXT
         *  buffer or RX buffer. If Range filter is not supported, writes to this register have
         *  no effect and read will return all
         *  Zeroes.
         */
        uint32_t bit_ran_low_val:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_ran_low_reg_t;

/** Type of filter_ran_high register
 *  TWAI FD filter range high value register
 */
typedef union {
    struct {
        /** bit_ran_high_val : R/W; bitpos: [28:0]; default: 0;
         *  Range filter High threshold. The identifier format is the same as in IDENTIFIER_W
         *  of TXT
         *  buffer or RX buffer. If Range filter is not supported, writes to this register have
         *  no effect and read will return all
         *  Zeroes.
         */
        uint32_t bit_ran_high_val:29;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} twaifd_filter_ran_high_reg_t;

/** Type of filter_control_filter_status register
 *  TWAI FD filter control register
 */
typedef union {
    struct {
        /** fanb : R/W; bitpos: [0]; default: 1;
         *  CAN Basic Frame is accepted by filter A.
         */
        uint32_t fanb:1;
        /** fane : R/W; bitpos: [1]; default: 1;
         *  CAN Extended Frame is accepted by Filter A.
         */
        uint32_t fane:1;
        /** fafb : R/W; bitpos: [2]; default: 1;
         *  CAN FD Basic Frame is accepted by filter A.
         */
        uint32_t fafb:1;
        /** fafe : R/W; bitpos: [3]; default: 1;
         *  CAN FD Extended Frame is accepted by filter A.
         */
        uint32_t fafe:1;
        /** fbnb : R/W; bitpos: [4]; default: 0;
         *  CAN Basic Frame is accepted by filter B.
         */
        uint32_t fbnb:1;
        /** fbne : R/W; bitpos: [5]; default: 0;
         *  CAN Extended Frame is accepted by Filter B.
         */
        uint32_t fbne:1;
        /** fbfb : R/W; bitpos: [6]; default: 0;
         *  CAN FD Basic Frame is accepted by filter B.
         */
        uint32_t fbfb:1;
        /** fbfe : R/W; bitpos: [7]; default: 0;
         *  CAN FD Extended Frame is accepted by filter B.
         */
        uint32_t fbfe:1;
        /** fcnb : R/W; bitpos: [8]; default: 0;
         *  CAN Basic Frame is accepted by filter C.
         */
        uint32_t fcnb:1;
        /** fcne : R/W; bitpos: [9]; default: 0;
         *  CAN Extended Frame is accepted by Filter C.
         */
        uint32_t fcne:1;
        /** fcfb : R/W; bitpos: [10]; default: 0;
         *  CAN FD Basic Frame is accepted by filter C.
         */
        uint32_t fcfb:1;
        /** fcfe : R/W; bitpos: [11]; default: 0;
         *  CAN FD Extended Frame is accepted by filter C.
         */
        uint32_t fcfe:1;
        /** frnb : R/W; bitpos: [12]; default: 0;
         *  CAN Basic Frame is accepted by Range filter.
         */
        uint32_t frnb:1;
        /** frne : R/W; bitpos: [13]; default: 0;
         *  CAN Extended Frame is accepted by Range filter.
         */
        uint32_t frne:1;
        /** frfb : R/W; bitpos: [14]; default: 0;
         *  CAN FD Basic Frame is accepted by Range filter.
         */
        uint32_t frfb:1;
        /** frfe : R/W; bitpos: [15]; default: 0;
         *  CAN FD Extended Frame is accepted by Range filter.
         */
        uint32_t frfe:1;
        /** sfa : RO; bitpos: [16]; default: 1;
         *  Logic 1 when Filter A is available. Otherwise logic 0.
         */
        uint32_t sfa:1;
        /** sfb : RO; bitpos: [17]; default: 1;
         *  Logic 1 when Filter B is available. Otherwise logic 0.
         */
        uint32_t sfb:1;
        /** sfc : RO; bitpos: [18]; default: 1;
         *  Logic 1 when Filter C is available. Otherwise logic 0.
         */
        uint32_t sfc:1;
        /** sfr : RO; bitpos: [19]; default: 1;
         *  Logic 1 when Range Filter is available. Otherwise logic 0.
         */
        uint32_t sfr:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} twaifd_filter_control_filter_status_reg_t;


/** Group: receiver register */
/** Type of rx_data register
 *  TWAI FD received data register
 */
typedef union {
    struct {
        /** rx_data : RO; bitpos: [31:0]; default: 0;
         *  RX buffer data at read pointer position in FIFO. By reading from this register,
         *  read pointer is auto-
         *  matically increased, as long as there is next data word stored in RX buffer. First
         *  stored word in the buffer is
         *  FRAME_FORMAT_W, next IDENTIFIER_W etc. This register shall be read by 32 bit access.
         */
        uint32_t rx_data:32;
    };
    uint32_t val;
} twaifd_rx_data_reg_t;


/** Group: transmitter register */
/** Type of tx_command_txtb_info register
 *  TWAI FD TXT buffer command & information register
 */
typedef union {
    struct {
        /** txce : WO; bitpos: [0]; default: 0;
         *  Issues "set empty" command.
         */
        uint32_t txce:1;
        /** txcr : WO; bitpos: [1]; default: 0;
         *  Issues "set ready" command.
         */
        uint32_t txcr:1;
        /** txca : WO; bitpos: [2]; default: 0;
         *  Issues "set abort" command.
         */
        uint32_t txca:1;
        uint32_t reserved_3:5;
        /** txb1 : WO; bitpos: [8]; default: 0;
         *  Command is issued to TXT Buffer 1.
         */
        uint32_t txb1:1;
        /** txb2 : WO; bitpos: [9]; default: 0;
         *  Command is issued to TXT Buffer 2.
         */
        uint32_t txb2:1;
        /** txb3 : WO; bitpos: [10]; default: 0;
         *  Command is issued to TXT Buffer 3. If number of TXT Buffers is less than 3, this
         *  field is reserved and has no
         *  Function.
         */
        uint32_t txb3:1;
        /** txb4 : WO; bitpos: [11]; default: 0;
         *  Command is issued to TXT Buffer 4. If number of TXT Buffers is less than 4, this
         *  field is reserved and has no
         *  Function.
         */
        uint32_t txb4:1;
        /** txb5 : WO; bitpos: [12]; default: 0;
         *  Command is issued to TXT Buffer 5. If number of TXT Buffers is less than 5, this
         *  field is reserved and has no
         *  Function.
         */
        uint32_t txb5:1;
        /** txb6 : WO; bitpos: [13]; default: 0;
         *  Command is issued to TXT Buffer 6. If number of TXT Buffers is less than 6, this
         *  field is reserved and has no
         *  Function.
         */
        uint32_t txb6:1;
        /** txb7 : WO; bitpos: [14]; default: 0;
         *  Command is issued to TXT Buffer 7. If number of TXT Buffers is less than 7, this
         *  field is reserved and has no
         *  Function.
         */
        uint32_t txb7:1;
        /** txb8 : WO; bitpos: [15]; default: 0;
         *  Command is issued to TXT Buffer 8. If number of TXT Buffers is less than 8, this
         *  field is reserved and has no
         *  Function.
         */
        uint32_t txb8:1;
        /** txt_buffer_count : RO; bitpos: [19:16]; default: 4;
         *  Number of TXT buffers present in CTU CAN FD. Lowest buffer is always 1. Highest
         *  buffer
         *  is at index equal to number of present buffers.
         */
        uint32_t txt_buffer_count:4;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} twaifd_tx_command_txtb_info_reg_t;

/** Type of tx_priority register
 *  TWAI FD TXT buffer command & information register
 */
typedef union {
    struct {
        /** txt1p : R/W; bitpos: [2:0]; default: 1;
         *  Priority of TXT buffer 1.
         */
        uint32_t txt1p:3;
        uint32_t reserved_3:1;
        /** txt2p : R/W; bitpos: [6:4]; default: 0;
         *  Priority of TXT buffer 2.
         */
        uint32_t txt2p:3;
        uint32_t reserved_7:1;
        /** txt3p : R/W; bitpos: [10:8]; default: 0;
         *  Priority of TXT buffer 3. If number of TXT Buffers is less than 3, this field is
         *  reserved and has no function.
         */
        uint32_t txt3p:3;
        uint32_t reserved_11:1;
        /** txt4p : R/W; bitpos: [14:12]; default: 0;
         *  Priority of TXT buffer 4. If number of TXT Buffers is less than 4, this field is
         *  reserved and has no function.
         */
        uint32_t txt4p:3;
        uint32_t reserved_15:1;
        /** txt5p : R/W; bitpos: [18:16]; default: 0;
         *  Priority of TXT buffer 5. If number of TXT Buffers is less than 5, this field is
         *  reserved and has no function.
         */
        uint32_t txt5p:3;
        uint32_t reserved_19:1;
        /** txt6p : R/W; bitpos: [22:20]; default: 0;
         *  Priority of TXT buffer 6. If number of TXT Buffers is less than 6, this field is
         *  reserved and has no function.
         */
        uint32_t txt6p:3;
        uint32_t reserved_23:1;
        /** txt7p : R/W; bitpos: [26:24]; default: 0;
         *  Priority of TXT buffer 7. If number of TXT Buffers is less than 7, this field is
         *  reserved and has no function.
         */
        uint32_t txt7p:3;
        uint32_t reserved_27:1;
        /** txt8p : R/W; bitpos: [30:28]; default: 0;
         *  Priority of TXT buffer 8. If number of TXT Buffers is less than 8, this field is
         *  reserved and has no function.
         */
        uint32_t txt8p:3;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} twaifd_tx_priority_reg_t;


/** Group: controller register */
/** Type of rx_fr_ctr register
 *  TWAI FD received frame counter register
 */
typedef union {
    struct {
        /** rx_fr_ctr_val : RO; bitpos: [31:0]; default: 0;
         *  Number of received frames by CTU CAN FD.
         */
        uint32_t rx_fr_ctr_val:32;
    };
    uint32_t val;
} twaifd_rx_fr_ctr_reg_t;

/** Type of tx_fr_ctr register
 *  TWAI FD transmitted frame counter register
 */
typedef union {
    struct {
        /** tx_ctr_val : RO; bitpos: [31:0]; default: 0;
         *  Number of transmitted frames by CTU CAN FD.
         */
        uint32_t tx_ctr_val:32;
    };
    uint32_t val;
} twaifd_tx_fr_ctr_reg_t;

/** Type of debug register
 *  TWAI FD debug register
 */
typedef union {
    struct {
        /** stuff_count : RO; bitpos: [2:0]; default: 0;
         *  Actual stuff count modulo 8 as definned in ISO FD protocol. Stuff count is erased
         *  in the beginning
         *  of CAN frame and increased by one with each stuff bit until Stuff count field in
         *  ISO FD frame. Then it stays fixed
         *  until the beginning of next frame. In non-ISO FD or normal CAN stuff bits are
         *  counted until the end of a frame.
         *  Note that this field is NOT gray encoded as defined in ISO FD standard. Stuff count
         *  is calculated only as long as
         *  controller is transceiving on the bus. During the reception this value remains
         *  fixed!
         */
        uint32_t stuff_count:3;
        /** destuff_count : RO; bitpos: [5:3]; default: 0;
         *  Actual de-stuff count modulo 8 as defined in ISO FD protocol. De-Stuff count is
         *  erased in the
         *  beginning of the frame and increased by one with each de-stuffed bit until Stuff
         *  count field in ISO FD Frame. Then
         *  it stays fixed until beginning of next frame. In non-ISO FD or normal CAN de-stuff
         *  bits are counted until the end
         *  of the frame. Note that this field is NOT grey encoded as defined in ISO FD
         *  standard. De-stuff count is calculated
         *  in both. Transceiver as well as receiver.
         */
        uint32_t destuff_count:3;
        /** pc_arb : RO; bitpos: [6]; default: 0;
         *  Protocol control state machine is in Arbitration field.
         */
        uint32_t pc_arb:1;
        /** pc_con : RO; bitpos: [7]; default: 0;
         *  Protocol control state machine is in Control field.
         */
        uint32_t pc_con:1;
        /** pc_dat : RO; bitpos: [8]; default: 0;
         *  Protocol control state machine is in Data field.
         */
        uint32_t pc_dat:1;
        /** pc_stc : RO; bitpos: [9]; default: 0;
         *  Protocol control state machine is in Stuff Count field.
         */
        uint32_t pc_stc:1;
        /** pc_crc : RO; bitpos: [10]; default: 0;
         *  Protocol control state machine is in CRC field.
         */
        uint32_t pc_crc:1;
        /** pc_crcd : RO; bitpos: [11]; default: 0;
         *  Protocol control state machine is in CRC Delimiter field.
         */
        uint32_t pc_crcd:1;
        /** pc_ack : RO; bitpos: [12]; default: 0;
         *  Protocol control state machine is in ACK field.
         */
        uint32_t pc_ack:1;
        /** pc_ackd : RO; bitpos: [13]; default: 0;
         *  Protocol control state machine is in ACK Delimiter field.
         */
        uint32_t pc_ackd:1;
        /** pc_eof : RO; bitpos: [14]; default: 0;
         *  Protocol control state machine is in End of file field.
         */
        uint32_t pc_eof:1;
        /** pc_int : RO; bitpos: [15]; default: 0;
         *  Protocol control state machine is in Intermission field.
         */
        uint32_t pc_int:1;
        /** pc_susp : RO; bitpos: [16]; default: 0;
         *  Protocol control state machine is in Suspend transmission field.
         */
        uint32_t pc_susp:1;
        /** pc_ovr : RO; bitpos: [17]; default: 0;
         *  Protocol control state machine is in Overload field.
         */
        uint32_t pc_ovr:1;
        /** pc_sof : RO; bitpos: [18]; default: 0;
         *  Protocol control state machine is in Start of frame field.
         */
        uint32_t pc_sof:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} twaifd_debug_reg_t;


/** Group: version register */
/** Type of date_ver register
 *  TWAI FD date version
 */
typedef union {
    struct {
        /** date_ver : R/W; bitpos: [31:0]; default: 36774224;
         *  TWAI FD version
         */
        uint32_t date_ver:32;
    };
    uint32_t val;
} twaifd_date_ver_reg_t;


typedef struct {
    volatile twaifd_device_id_version_reg_t device_id_version;
    volatile twaifd_mode_settings_reg_t mode_settings;
    volatile twaifd_status_reg_t status;
    volatile twaifd_command_reg_t command;
    volatile twaifd_int_stat_reg_t int_stat;
    volatile twaifd_int_ena_set_reg_t int_ena_set;
    volatile twaifd_int_ena_clr_reg_t int_ena_clr;
    volatile twaifd_int_mask_set_reg_t int_mask_set;
    volatile twaifd_int_mask_clr_reg_t int_mask_clr;
    volatile twaifd_btr_reg_t btr;
    volatile twaifd_btr_fd_reg_t btr_fd;
    volatile twaifd_ewl_erp_fault_state_reg_t ewl_erp_fault_state;
    volatile twaifd_rec_tec_reg_t rec_tec;
    volatile twaifd_err_norm_err_fd_reg_t err_norm_err_fd;
    volatile twaifd_ctr_pres_reg_t ctr_pres;
    volatile twaifd_filter_a_mask_reg_t filter_a_mask;
    volatile twaifd_filter_a_val_reg_t filter_a_val;
    volatile twaifd_filter_b_mask_reg_t filter_b_mask;
    volatile twaifd_filter_b_val_reg_t filter_b_val;
    volatile twaifd_filter_c_mask_reg_t filter_c_mask;
    volatile twaifd_filter_c_val_reg_t filter_c_val;
    volatile twaifd_filter_ran_low_reg_t filter_ran_low;
    volatile twaifd_filter_ran_high_reg_t filter_ran_high;
    volatile twaifd_filter_control_filter_status_reg_t filter_control_filter_status;
    volatile twaifd_rx_mem_info_reg_t rx_mem_info;
    volatile twaifd_rx_pointers_reg_t rx_pointers;
    volatile twaifd_rx_status_rx_settings_reg_t rx_status_rx_settings;
    volatile twaifd_rx_data_reg_t rx_data;
    volatile twaifd_tx_status_reg_t tx_status;
    volatile twaifd_tx_command_txtb_info_reg_t tx_command_txtb_info;
    volatile twaifd_tx_priority_reg_t tx_priority;
    volatile twaifd_err_capt_retr_ctr_alc_ts_info_reg_t err_capt_retr_ctr_alc_ts_info;
    volatile twaifd_trv_delay_ssp_cfg_reg_t trv_delay_ssp_cfg;
    volatile twaifd_rx_fr_ctr_reg_t rx_fr_ctr;
    volatile twaifd_tx_fr_ctr_reg_t tx_fr_ctr;
    volatile twaifd_debug_reg_t debug;
    volatile twaifd_yolo_reg_t yolo;
    volatile twaifd_timestamp_low_reg_t timestamp_low;
    volatile twaifd_timestamp_high_reg_t timestamp_high;
    uint32_t reserved_09c[974];
    volatile twaifd_timer_clk_en_reg_t timer_clk_en;
    volatile twaifd_timer_int_raw_reg_t timer_int_raw;
    volatile twaifd_timer_int_st_reg_t timer_int_st;
    volatile twaifd_timer_int_ena_reg_t timer_int_ena;
    volatile twaifd_timer_int_clr_reg_t timer_int_clr;
    volatile twaifd_timer_cfg_reg_t timer_cfg;
    volatile twaifd_timer_ld_val_l_reg_t timer_ld_val_l;
    volatile twaifd_timer_ld_val_h_reg_t timer_ld_val_h;
    volatile twaifd_timer_ct_val_l_reg_t timer_ct_val_l;
    volatile twaifd_timer_ct_val_h_reg_t timer_ct_val_h;
    volatile twaifd_date_ver_reg_t date_ver;
} twaifd_dev_t;

extern twaifd_dev_t TWAI0;
extern twaifd_dev_t TWAI1;

#ifndef __cplusplus
_Static_assert(sizeof(twaifd_dev_t) == 0x1000, "Invalid size of twaifd_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
