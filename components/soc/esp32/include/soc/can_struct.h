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
#ifndef _SOC_CAN_STRUCT_H_
#define _SOC_CAN_STRUCT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------- Register Definitions -------------------------- */

/* The CAN peripheral's registers are 8bits, however the ESP32 can only access
 * peripheral registers every 32bits. Therefore each CAN register is mapped to
 * the least significant byte of every 32bits.
 */
typedef union {
    struct {
        uint32_t byte: 8;                                   /* LSB */
        uint32_t reserved24: 24;                            /* Internal Reserved */
    };
    uint32_t val;
} can_reg_t;

typedef union {
    struct {
        uint32_t reset: 1;                                  /* MOD.0 Reset Mode */
        uint32_t listen_only: 1;                            /* MOD.1 Listen Only Mode */
        uint32_t self_test: 1;                              /* MOD.2 Self Test Mode */
        uint32_t acceptance_filter: 1;                      /* MOD.3 Acceptance Filter Mode */
        uint32_t reserved28: 28;                            /* Internal Reserved. MOD.4 Sleep Mode not supported */
    };
    uint32_t val;
} can_mode_reg_t;

typedef union {
    struct {
        uint32_t tx_req: 1;                                 /* CMR.0 Transmission Request */
        uint32_t abort_tx: 1;                               /* CMR.1 Abort Transmission */
        uint32_t release_rx_buff: 1;                        /* CMR.2 Release Receive Buffer */
        uint32_t clear_data_overrun: 1;                     /* CMR.3 Clear Data Overrun */
        uint32_t self_rx_req: 1;                            /* CMR.4 Self Reception Request */
        uint32_t reserved27: 27;                            /* Internal Reserved */
    };
    uint32_t val;
} can_cmd_reg_t;

typedef union {
    struct {
        uint32_t rx_buff: 1;                                /* SR.0 Receive Buffer Status */
        uint32_t data_overrun: 1;                           /* SR.1 Data Overrun Status */
        uint32_t tx_buff: 1;                                /* SR.2 Transmit Buffer Status */
        uint32_t tx_complete: 1;                            /* SR.3 Transmission Complete Status */
        uint32_t rx: 1;                                     /* SR.4 Receive Status */
        uint32_t tx: 1;                                     /* SR.5 Transmit Status */
        uint32_t error: 1;                                  /* SR.6 Error Status */
        uint32_t bus: 1;                                    /* SR.7 Bus Status */
        uint32_t reserved24: 24;                            /* Internal Reserved */
    };
    uint32_t val;
} can_status_reg_t;

typedef union {
    struct {
        uint32_t rx: 1;                                     /* IR.0 Receive Interrupt */
        uint32_t tx: 1;                                     /* IR.1 Transmit Interrupt */
        uint32_t err_warn: 1;                               /* IR.2 Error Interrupt */
        uint32_t data_overrun: 1;                           /* IR.3 Data Overrun Interrupt */
        uint32_t reserved1: 1;                              /* Internal Reserved (Wake-up not supported) */
        uint32_t err_passive: 1;                            /* IR.5 Error Passive Interrupt */
        uint32_t arb_lost: 1;                               /* IR.6 Arbitration Lost Interrupt */
        uint32_t bus_err: 1;                                /* IR.7 Bus Error Interrupt */
        uint32_t reserved24: 24;                            /* Internal Reserved */
    };
    uint32_t val;
} can_intr_reg_t;

typedef union {
    struct {
        uint32_t rx: 1;                                     /* IER.0 Receive Interrupt Enable */
        uint32_t tx: 1;                                     /* IER.1 Transmit Interrupt Enable */
        uint32_t err_warn: 1;                               /* IER.2 Error Interrupt Enable */
        uint32_t data_overrun: 1;                           /* IER.3 Data Overrun Interrupt Enable */
        uint32_t brp_div: 1;                                /* THIS IS NOT AN INTERRUPT. brp_div will prescale BRP by 2. Only available on ESP32 Revision 2 or later. Reserved otherwise */
        uint32_t err_passive: 1;                            /* IER.5 Error Passive Interrupt Enable */
        uint32_t arb_lost: 1;                               /* IER.6 Arbitration Lost Interrupt Enable */
        uint32_t bus_err: 1;                                /* IER.7 Bus Error Interrupt Enable */
        uint32_t reserved24: 24;                            /* Internal Reserved */
    };
    uint32_t val;
} can_intr_en_reg_t;

typedef union {
    struct {
        uint32_t baud_rate_prescaler: 6;                    /* BTR0[5:0] Baud Rate Prescaler */
        uint32_t sync_jump_width: 2;                        /* BTR0[7:6] Synchronization Jump Width*/
        uint32_t reserved24: 24;                            /* Internal Reserved */
    };
    uint32_t val;
} can_bus_tim_0_reg_t;

typedef union {
    struct {
        uint32_t time_seg_1: 4;                             /* BTR1[3:0] Timing Segment 1 */
        uint32_t time_seg_2: 3;                             /* BTR1[6:4] Timing Segment 2 */
        uint32_t sampling: 1;                               /* BTR1.7 Sampling*/
        uint32_t reserved24: 24;                            /* Internal Reserved */
    };
    uint32_t val;
} can_bus_tim_1_reg_t;

typedef union {
    struct {
        uint32_t arbitration_lost_capture: 5;               /* ALC[4:0] Arbitration lost capture */
        uint32_t reserved27: 27;                            /* Internal Reserved */
    };
    uint32_t val;
} can_arb_lost_cap_reg_t;

typedef union {
    struct {
        uint32_t segment: 5;                                /* ECC[4:0] Error Code Segment 0 to 5 */
        uint32_t direction: 1;                              /* ECC.5 Error Direction (TX/RX) */
        uint32_t error_code: 2;                             /* ECC[7:6] Error Code */
        uint32_t reserved24: 24;                            /* Internal Reserved */
    };
    uint32_t val;
} can_err_code_cap_reg_t;

typedef struct can_acc_filter_s {
    can_reg_t code_reg[4];
    can_reg_t mask_reg[4];
    uint32_t reserved32[5];
} can_acc_filter_t;

typedef union {
    struct {
        uint32_t rx_message_counter: 5;                     /* RMC[4:0] RX Message Counter */
        uint32_t reserved27: 27;                            /* Internal Reserved */
    };
    uint32_t val;
} can_rx_msg_cnt_reg_t;

typedef union {
    struct {
        uint32_t clock_divider: 3;                          /* CDR[2:0] CLKOUT frequency selector based of fOSC */
        uint32_t clock_off: 1;                              /* CDR.3 CLKOUT enable/disable */
        uint32_t reserved3: 3;                              /* Internal Reserved. RXINTEN and CBP not supported */
        uint32_t can_mode: 1;                               /* CDR.7 BasicCAN:0 PeliCAN:1 */
        uint32_t reserved24: 24;                            /* Internal Reserved  */
    };
    uint32_t val;
} can_clk_div_reg_t;

/* ---------------------------- Register Layout ------------------------------ */

typedef volatile struct can_dev_s {
    //Configuration and Control Registers
    can_mode_reg_t mode_reg;                                /* Address 0 */
    can_cmd_reg_t command_reg;                              /* Address 1 */
    can_status_reg_t status_reg;                            /* Address 2 */
    can_intr_reg_t interrupt_reg;                           /* Address 3 */
    can_intr_en_reg_t interrupt_enable_reg;                 /* Address 4 */
    uint32_t reserved_05;                                   /* Address 5 */
    can_bus_tim_0_reg_t bus_timing_0_reg;                   /* Address 6 */
    can_bus_tim_1_reg_t bus_timing_1_reg;                   /* Address 7 */
    uint32_t reserved_08;                                   /* Address 8 (Output control not supported) */
    uint32_t reserved_09;                                   /* Address 9 (Test Register not supported) */
    uint32_t reserved_10;                                   /* Address 10 */

    //Capture and Counter Registers
    can_arb_lost_cap_reg_t arbitration_lost_captue_reg;     /* Address 11 */
    can_err_code_cap_reg_t error_code_capture_reg;          /* Address 12 */
    can_reg_t error_warning_limit_reg;                      /* EWLR[7:0] Error Warning Limit: Address 13 */
    can_reg_t rx_error_counter_reg;                         /* RXERR[7:0] Receive Error Counter: Address 14 */
    can_reg_t tx_error_counter_reg;                         /* TXERR[7:0] Transmit Error Counter: Address 15 */

    //Shared Registers (TX Buff/RX Buff/Acc Filter)
    union {
        can_acc_filter_t acceptance_filter;
        can_reg_t tx_rx_buffer[13];
    };                                                      /* Address 16-28 TX/RX Buffer and Acc Filter*/;

    //Misc Registers
    can_rx_msg_cnt_reg_t rx_message_counter_reg;            /* Address 29 */
    can_reg_t reserved_30;                                  /* Address 30 (RX Buffer Start Address not supported) */
    can_clk_div_reg_t clock_divider_reg;                    /* Address 31 */

    //Start of RX FIFO
} can_dev_t;

_Static_assert(sizeof(can_dev_t) == 128, "CAN registers should be 32 * 4 bytes");

extern can_dev_t CAN;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_CAN_STRUCT_H_ */

