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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ---------------------------- Register Layout ------------------------------ */

/* The TWAI peripheral's registers are 8bits, however the ESP32-C3 can only access
 * peripheral registers every 32bits. Therefore each TWAI register is mapped to
 * the least significant byte of every 32bits.
 */

typedef volatile struct twai_dev_s {
    //Configuration and Control Registers
    union {
        struct {
            uint32_t rm: 1;                     /* MOD.0 Reset Mode */
            uint32_t lom: 1;                    /* MOD.1 Listen Only Mode */
            uint32_t stm: 1;                    /* MOD.2 Self Test Mode */
            uint32_t afm: 1;                    /* MOD.3 Acceptance Filter Mode */
            uint32_t reserved28: 28;            /* Internal Reserved. MOD.4 Sleep Mode not supported */
        };
        uint32_t val;
    } mode_reg;                                 /* Address 0 */
    union {
        struct {
            uint32_t tr: 1;                     /* CMR.0 Transmission Request */
            uint32_t at: 1;                     /* CMR.1 Abort Transmission */
            uint32_t rrb: 1;                    /* CMR.2 Release Receive Buffer */
            uint32_t cdo: 1;                    /* CMR.3 Clear Data Overrun */
            uint32_t srr: 1;                    /* CMR.4 Self Reception Request */
            uint32_t reserved27: 27;            /* Internal Reserved */
        };
        uint32_t val;
    } command_reg;                              /* Address 1 */
    union {
        struct {
            uint32_t rbs: 1;                    /* SR.0 Receive Buffer Status */
            uint32_t dos: 1;                    /* SR.1 Data Overrun Status */
            uint32_t tbs: 1;                    /* SR.2 Transmit Buffer Status */
            uint32_t tcs: 1;                    /* SR.3 Transmission Complete Status */
            uint32_t rs: 1;                     /* SR.4 Receive Status */
            uint32_t ts: 1;                     /* SR.5 Transmit Status */
            uint32_t es: 1;                     /* SR.6 Error Status */
            uint32_t bs: 1;                     /* SR.7 Bus Status */
            uint32_t ms: 1;                     /* SR.8 Miss Status */
            uint32_t reserved23: 23;            /* Internal Reserved */
        };
        uint32_t val;
    } status_reg;                               /* Address 2 */
    union {
        struct {
            uint32_t ri: 1;                     /* IR.0 Receive Interrupt */
            uint32_t ti: 1;                     /* IR.1 Transmit Interrupt */
            uint32_t ei: 1;                     /* IR.2 Error Interrupt */
            uint32_t reserved2: 2;              /* Internal Reserved (Data Overrun interrupt and Wake-up not supported) */
            uint32_t epi: 1;                    /* IR.5 Error Passive Interrupt */
            uint32_t ali: 1;                    /* IR.6 Arbitration Lost Interrupt */
            uint32_t bei: 1;                    /* IR.7 Bus Error Interrupt */
            uint32_t reserved24: 24;            /* Internal Reserved */
        };
        uint32_t val;
    } interrupt_reg;                           /* Address 3 */
    union {
        struct {
            uint32_t rie: 1;                    /* IER.0 Receive Interrupt Enable */
            uint32_t tie: 1;                    /* IER.1 Transmit Interrupt Enable */
            uint32_t eie: 1;                    /* IER.2 Error Interrupt Enable */
            uint32_t reserved2: 2;              /* Internal Reserved (Data Overrun interrupt and Wake-up not supported) */
            uint32_t epie: 1;                   /* IER.5 Error Passive Interrupt Enable */
            uint32_t alie: 1;                   /* IER.6 Arbitration Lost Interrupt Enable */
            uint32_t beie: 1;                   /* IER.7 Bus Error Interrupt Enable */
            uint32_t reserved24: 24;            /* Internal Reserved */
        };
        uint32_t val;
    } interrupt_enable_reg;                     /* Address 4 */
    uint32_t reserved_05;                       /* Address 5 */
    union {
        struct {
            uint32_t brp: 13;                   /* BTR0[12:0] Baud Rate Prescaler */
            uint32_t reserved1: 1;              /* Internal Reserved */
            uint32_t sjw: 2;                    /* BTR0[15:14] Synchronization Jump Width*/
            uint32_t reserved16: 16;            /* Internal Reserved */
        };
        uint32_t val;
    } bus_timing_0_reg;                         /* Address 6 */
    union {
        struct {
            uint32_t tseg1: 4;                  /* BTR1[3:0] Timing Segment 1 */
            uint32_t tseg2: 3;                  /* BTR1[6:4] Timing Segment 2 */
            uint32_t sam: 1;                    /* BTR1.7 Sampling*/
            uint32_t reserved24: 24;            /* Internal Reserved */
        };
        uint32_t val;
    } bus_timing_1_reg;                         /* Address 7 */
    uint32_t reserved_08;                       /* Address 8 (Output control not supported) */
    uint32_t reserved_09;                       /* Address 9 (Test Register not supported) */
    uint32_t reserved_10;                       /* Address 10 */

    //Capture and Counter Registers
    union {
        struct {
            uint32_t alc: 5;                    /* ALC[4:0] Arbitration lost capture */
            uint32_t reserved27: 27;            /* Internal Reserved */
        };
        uint32_t val;
    } arbitration_lost_captue_reg;              /* Address 11 */
    union {
        struct {
            uint32_t seg: 5;                    /* ECC[4:0] Error Code Segment 0 to 5 */
            uint32_t dir: 1;                    /* ECC.5 Error Direction (TX/RX) */
            uint32_t errc: 2;                   /* ECC[7:6] Error Code */
            uint32_t reserved24: 24;            /* Internal Reserved */
        };
        uint32_t val;
    } error_code_capture_reg;                   /* Address 12 */
    union {
        struct {
            uint32_t ewl: 8;                    /* EWL[7:0] Error Warning Limit */
            uint32_t reserved24: 24;            /* Internal Reserved */
        };
        uint32_t val;
    } error_warning_limit_reg;                  /* EWLR[7:0] Error Warning Limit: Address 13 */
    union {
        struct {
            uint32_t rxerr: 8;                  /* RXERR[7:0] Receive Error Counter */
            uint32_t reserved24: 24;            /* Internal Reserved */
        };
        uint32_t val;
    } rx_error_counter_reg;                     /* Address 12 */
    union {
        struct {
            uint32_t txerr: 8;                  /* TXERR[7:0] Receive Error Counter */
            uint32_t reserved24: 24;            /* Internal Reserved */
        };
        uint32_t val;
    } tx_error_counter_reg;                     /* Address 15 */

    //Shared Registers (TX Buff/RX Buff/Acc Filter)
    union {
        struct {
            union {
                struct {
                    uint32_t byte: 8;           /* ACRx[7:0] Acceptance Code */
                    uint32_t reserved24: 24;    /* Internal Reserved */
                };
                uint32_t val;
            } acr[4];
            union {
                struct {
                    uint32_t byte: 8;           /* AMRx[7:0] Acceptance Mask */
                    uint32_t reserved24: 24;    /* Internal Reserved */
                };
                uint32_t val;
            } amr[4];
            uint32_t reserved32[5];
        } acceptance_filter;
        union {
            struct {
                uint32_t byte: 8;
                uint32_t reserved24: 24;
            };
            uint32_t val;
        } tx_rx_buffer[13];
    };                                          /* Address 16-28 TX/RX Buffer and Acc Filter*/;

    //Misc Registers
    union {
        struct {
            uint32_t rmc: 7;                    /* RMC[6:0] RX Message Counter */
            uint32_t reserved25: 25;            /* Internal Reserved */
        };
        uint32_t val;
    } rx_message_counter_reg;                   /* Address 29 */
    uint32_t reserved_30;                       /* Address 30 (RX Buffer Start Address not supported) */
    union {
        struct {
            uint32_t cd: 8;                     /* CDR[7:0] CLKOUT frequency selector based of fOSC */
            uint32_t co: 1;                     /* CDR.8 CLKOUT enable/disable */
            uint32_t reserved24: 23;            /* Internal Reserved  */
        };
        uint32_t val;
    } clock_divider_reg;                        /* Address 31 */
} twai_dev_t;

_Static_assert(sizeof(twai_dev_t) == 128, "TWAI registers should be 32 * 4 bytes");

extern twai_dev_t TWAI;

#ifdef __cplusplus
}
#endif
