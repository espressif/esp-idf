/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------- Register Layout ------------------------------ */

/* The TWAI peripheral's registers are 8bits, however the ESP32 can only access
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
            uint32_t reserved4: 28;             /* Internal Reserved. MOD.4 Sleep Mode not supported */
        };
        uint32_t val;
    } mode_reg;                                 /* Address 0x0000 */
    union {
        struct {
            uint32_t tr: 1;                     /* CMR.0 Transmission Request */
            uint32_t at: 1;                     /* CMR.1 Abort Transmission */
            uint32_t rrb: 1;                    /* CMR.2 Release Receive Buffer */
            uint32_t cdo: 1;                    /* CMR.3 Clear Data Overrun */
            uint32_t srr: 1;                    /* CMR.4 Self Reception Request */
            uint32_t reserved5: 27;             /* Internal Reserved */
        };
        uint32_t val;
    } command_reg;                              /* Address 0x0004 */
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
            uint32_t reserved8: 24;             /* Internal Reserved */
        };
        uint32_t val;
    } status_reg;                               /* Address 0x0008 */
    union {
        struct {
            uint32_t ri: 1;                     /* IR.0 Receive Interrupt */
            uint32_t ti: 1;                     /* IR.1 Transmit Interrupt */
            uint32_t ei: 1;                     /* IR.2 Error Interrupt */
            uint32_t doi: 1;                    /* IR.3 Data Overrun Interrupt */
            uint32_t reserved4: 1;              /* Internal Reserved (Wake-up not supported) */
            uint32_t epi: 1;                    /* IR.5 Error Passive Interrupt */
            uint32_t ali: 1;                    /* IR.6 Arbitration Lost Interrupt */
            uint32_t bei: 1;                    /* IR.7 Bus Error Interrupt */
            uint32_t reserved8: 24;             /* Internal Reserved */
        };
        uint32_t val;
    } interrupt_reg;                           /* Address 0x000C */
    union {
        struct {
            uint32_t rie: 1;                    /* IER.0 Receive Interrupt Enable */
            uint32_t tie: 1;                    /* IER.1 Transmit Interrupt Enable */
            uint32_t eie: 1;                    /* IER.2 Error Interrupt Enable */
            uint32_t doie: 1;                   /* IER.3 Data Overrun Interrupt Enable */
            uint32_t brp_div: 1;                /* THIS IS NOT AN INTERRUPT. brp_div will prescale BRP by 2. Only available on ESP32 Revision 2 or later. Reserved otherwise */
            uint32_t epie: 1;                   /* IER.5 Error Passive Interrupt Enable */
            uint32_t alie: 1;                   /* IER.6 Arbitration Lost Interrupt Enable */
            uint32_t beie: 1;                   /* IER.7 Bus Error Interrupt Enable */
            uint32_t reserved8: 24;             /* Internal Reserved */
        };
        uint32_t val;
    } interrupt_enable_reg;                     /* Address 0x0010 */
    uint32_t reserved_14;
    union {
        struct {
            uint32_t brp: 6;                    /* BTR0[5:0] Baud Rate Prescaler */
            uint32_t sjw: 2;                    /* BTR0[7:6] Synchronization Jump Width*/
            uint32_t reserved8: 24;             /* Internal Reserved */
        };
        uint32_t val;
    } bus_timing_0_reg;                         /* Address 0x0018 */
    union {
        struct {
            uint32_t tseg1: 4;                  /* BTR1[3:0] Timing Segment 1 */
            uint32_t tseg2: 3;                  /* BTR1[6:4] Timing Segment 2 */
            uint32_t sam: 1;                    /* BTR1.7 Sampling*/
            uint32_t reserved8: 24;             /* Internal Reserved */
        };
        uint32_t val;
    } bus_timing_1_reg;                         /* Address 0x001C */
    uint32_t reserved_20;                       /* Address 0x0020 (Output control not supported) */
    uint32_t reserved_24;                       /* Address 0x0024 (Test Register not supported) */
    uint32_t reserved_28;                       /* Address 0x0028 */

    //Capture and Counter Registers
    union {
        struct {
            uint32_t alc: 5;                    /* ALC[4:0] Arbitration lost capture */
            uint32_t reserved5: 27;             /* Internal Reserved */
        };
        uint32_t val;
    } arbitration_lost_captue_reg;              /* Address 0x002C */
    union {
        struct {
            uint32_t seg: 5;                    /* ECC[4:0] Error Code Segment 0 to 5 */
            uint32_t dir: 1;                    /* ECC.5 Error Direction (TX/RX) */
            uint32_t errc: 2;                   /* ECC[7:6] Error Code */
            uint32_t reserved8: 24;             /* Internal Reserved */
        };
        uint32_t val;
    } error_code_capture_reg;                   /* Address 0x0030 */
    union {
        struct {
            uint32_t ewl: 8;                    /* EWL[7:0] Error Warning Limit */
            uint32_t reserved8: 24;             /* Internal Reserved */
        };
        uint32_t val;
    } error_warning_limit_reg;                  /* Address 0x0034 */
    union {
        struct {
            uint32_t rxerr: 8;                  /* RXERR[7:0] Receive Error Counter */
            uint32_t reserved8: 24;             /* Internal Reserved */
        };
        uint32_t val;
    } rx_error_counter_reg;                     /* Address 0x0038 */
    union {
        struct {
            uint32_t txerr: 8;                  /* TXERR[7:0] Receive Error Counter */
            uint32_t reserved8: 24;             /* Internal Reserved */
        };
        uint32_t val;
    } tx_error_counter_reg;                     /* Address 0x003C */

    //Shared Registers (TX Buff/RX Buff/Acc Filter)
    union {
        struct {
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
        } acceptance_filter;
        union {
            struct {
                uint32_t byte: 8;               /* TX/RX Byte X [7:0] */
                uint32_t reserved24: 24;        /* Internal Reserved */
            };
            uint32_t val;
        } tx_rx_buffer[13];
    };                                          /* Address 0x0040 - 0x0070 */

    //Misc Registers
    union {
        struct {
            uint32_t rmc: 7;                    /* RMC[6:0] RX Message Counter */
            uint32_t reserved7: 25;             /* Internal Reserved */
        };
        uint32_t val;
    } rx_message_counter_reg;                   /* Address 0x0074 */
    uint32_t reserved_78;                       /* Address 0x0078 (RX Buffer Start Address not supported) */
    union {
        struct {
            uint32_t cd: 3;                     /* CDR[2:0] CLKOUT frequency selector based of fOSC */
            uint32_t co: 1;                     /* CDR.3 CLKOUT enable/disable */
            uint32_t reserved4: 3;              /* Internal Reserved. RXINTEN and CBP not supported */
            uint32_t cm: 1;                     /* CDR.7 Register Layout. Basic:0 Extended:1 */
            uint32_t reserved6: 24;             /* Internal Reserved  */
        };
        uint32_t val;
    } clock_divider_reg;                        /* Address 0x007C */
} twai_dev_t;

#ifndef __cplusplus
_Static_assert(sizeof(twai_dev_t) == 128, "TWAI registers should be 32 * 4 bytes");
#endif

extern twai_dev_t TWAI;

#ifdef __cplusplus
}
#endif
