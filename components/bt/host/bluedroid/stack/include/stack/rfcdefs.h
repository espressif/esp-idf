/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/****************************************************************************
 *
 *  This file contains definitions for the RFCOMM protocol
 *
 ****************************************************************************/

#ifndef RFCDEFS_H
#define RFCDEFS_H

#define PORT_MAX_RFC_PORTS              31

/*
**  If nothing is negotiated MTU should be 127
*/
#define RFCOMM_DEFAULT_MTU              127

/*
** Define used by RFCOMM TS frame types
*/
#define RFCOMM_SABME                    0x2F
#define RFCOMM_UA                       0x63
#define RFCOMM_DM                       0x0F
#define RFCOMM_DISC                     0x43
#define RFCOMM_UIH                      0xEF

/*
** Defenitions for the TS control frames
*/
#define RFCOMM_CTRL_FRAME_LEN           3
#define RFCOMM_MIN_OFFSET               5 /* ctrl 2 , len 1 or 2 bytes, credit 1 byte */
#define RFCOMM_DATA_OVERHEAD            (RFCOMM_MIN_OFFSET + 1)  /* add 1 for checksum */

#define RFCOMM_EA                       1
#define RFCOMM_EA_MASK                  0x01
#define RFCOMM_CR_MASK                  0x02
#define RFCOMM_SHIFT_CR                 1
#define RFCOMM_SHIFT_DLCI               2
#define RFCOMM_SHIFT_DLCI2              6
#define RFCOMM_PF                       0x10
#define RFCOMM_PF_MASK                  0x10
#define RFCOMM_PF_OFFSET                4
#define RFCOMM_SHIFT_LENGTH1            1
#define RFCOMM_SHIFT_LENGTH2            7
#define RFCOMM_SHIFT_MX_CTRL_TYPE       2

#define RFCOMM_INITIATOR_CMD            1
#define RFCOMM_INITIATOR_RSP            0
#define RFCOMM_RESPONDER_CMD            0
#define RFCOMM_RESPONDER_RSP            1

#define RFCOMM_PARSE_CTRL_FIELD(ea, cr, dlci, p_data)       \
{                                                           \
    ea = *p_data & RFCOMM_EA;                               \
    cr = (*p_data & RFCOMM_CR_MASK) >> RFCOMM_SHIFT_CR;     \
    dlci = *p_data++ >> RFCOMM_SHIFT_DLCI;                  \
    if (!ea) dlci += *p_data++ << RFCOMM_SHIFT_DLCI2;       \
}

#define RFCOMM_FORMAT_CTRL_FIELD(p_data, ea, cr, dlci)      \
    *p_data++ = ea | cr | (dlci << RFCOMM_SHIFT_DLCI)

#define RFCOMM_PARSE_TYPE_FIELD(type, pf, p_data)           \
{                                                           \
    type = *p_data & ~RFCOMM_PF_MASK;                       \
    pf   = (*p_data++ & RFCOMM_PF_MASK) >> RFCOMM_PF_OFFSET;\
}

#define RFCOMM_FORMAT_TYPE_FIELD(p_data, type, pf)          \
    *p_data++ = (type | (pf << RFCOMM_PF_OFFSET))           \
{                                                           \
    type = *p_data & ~RFCOMM_PF_MASK;                       \
    pf   = (*p_data++ & RFCOMM_PF_MASK) >> RFCOMM_PF_OFFSET;\
}

#define RFCOMM_PARSE_LEN_FIELD(ea, length, p_data)          \
{                                                           \
    ea = (*p_data & RFCOMM_EA);                             \
    length = (*p_data++ >> RFCOMM_SHIFT_LENGTH1);           \
    if (!ea) length += (*p_data++ << RFCOMM_SHIFT_LENGTH2); \
}

#define RFCOMM_FRAME_IS_CMD(initiator, cr)                  \
    (( (initiator) && !(cr)) || (!(initiator) &&  (cr)))

#define RFCOMM_FRAME_IS_RSP(initiator, cr)                  \
    (( (initiator) &&  (cr)) || (!(initiator) && !(cr)))

#define RFCOMM_CR(initiator, is_command)                    \
    (( ( (initiator) &&  (is_command))                      \
    || (!(initiator) && !(is_command))) << 1)

#define RFCOMM_I_CR(is_command) ((is_command) ? 0x02 : 0x00)

#define RFCOMM_MAX_DLCI             61

#define RFCOMM_VALID_DLCI(dlci)                             \
    (((dlci) == 0) || (((dlci) >= 2) && ((dlci) <= RFCOMM_MAX_DLCI)))


/* Port Negotiation (PN) */
#define RFCOMM_PN_DLCI_MASK         0x3F

#define RFCOMM_PN_FRAM_TYPE_UIH     0x00
#define RFCOMM_PN_FRAME_TYPE_MASK   0x0F

#define RFCOMM_PN_CONV_LAYER_MASK   0xF0
#define RFCOMM_PN_CONV_LAYER_TYPE_1 0
#define RFCOMM_PN_CONV_LAYER_CBFC_I 0xF0
#define RFCOMM_PN_CONV_LAYER_CBFC_R 0xE0

#define RFCOMM_PN_PRIORITY_MASK     0x3F
#define RFCOMM_PN_PRIORITY_0        0

#define RFCOMM_PN_K_MASK            0x07

#define RFCOMM_T1_DSEC              0 /* None negotiable in RFCOMM */
#define RFCOMM_N2                   0 /* Number of retransmissions */
#define RFCOMM_K                    0 /* Window size */
#define RFCOMM_K_MAX                7 /* Max value of K for credit based flow control */

#define RFCOMM_MSC_FC               0x02          /* Flow control*/
#define RFCOMM_MSC_RTC              0x04          /* Ready to communicate*/
#define RFCOMM_MSC_RTR              0x08          /* Ready to receive*/
#define RFCOMM_MSC_IC               0x40          /* Incomming call indicator*/
#define RFCOMM_MSC_DV               0x80          /* Data Valid*/

#define RFCOMM_MSC_SHIFT_BREAK          4
#define RFCOMM_MSC_BREAK_MASK           0xF0
#define RFCOMM_MSC_BREAK_PRESENT_MASK   0x02

#define RFCOMM_BAUD_RATE_2400           0x00
#define RFCOMM_BAUD_RATE_4800           0x01
#define RFCOMM_BAUD_RATE_7200           0x02
#define RFCOMM_BAUD_RATE_9600           0x03
#define RFCOMM_BAUD_RATE_19200          0x04
#define RFCOMM_BAUD_RATE_38400          0x05
#define RFCOMM_BAUD_RATE_57600          0x06
#define RFCOMM_BAUD_RATE_115200         0x07
#define RFCOMM_BAUD_RATE_230400         0x08

#define RFCOMM_5_BITS                   0x00
#define RFCOMM_6_BITS                   0x01
#define RFCOMM_7_BITS                   0x02
#define RFCOMM_8_BITS                   0x03

#define RFCOMM_RPN_BITS_MASK            0x03
#define RFCOMM_RPN_BITS_SHIFT           0

#define RFCOMM_ONESTOPBIT               0x00
#define RFCOMM_ONE5STOPBITS             0x01

#define RFCOMM_RPN_STOP_BITS_MASK       0x01
#define RFCOMM_RPN_STOP_BITS_SHIFT      2

#define RFCOMM_PARITY_NO                0x00
#define RFCOMM_PARITY_YES               0x01
#define RFCOMM_RPN_PARITY_MASK          0x01
#define RFCOMM_RPN_PARITY_SHIFT         3

#define RFCOMM_ODD_PARITY               0x00
#define RFCOMM_EVEN_PARITY              0x01
#define RFCOMM_MARK_PARITY              0x02
#define RFCOMM_SPACE_PARITY             0x03

#define RFCOMM_RPN_PARITY_TYPE_MASK     0x03
#define RFCOMM_RPN_PARITY_TYPE_SHIFT    4

#define RFCOMM_FC_OFF                   0x00
#define RFCOMM_FC_XONXOFF_ON_INPUT      0x01
#define RFCOMM_FC_XONXOFF_ON_OUTPUT     0x02
#define RFCOMM_FC_RTR_ON_INPUT          0x04
#define RFCOMM_FC_RTR_ON_OUTPUT         0x08
#define RFCOMM_FC_RTC_ON_INPUT          0x10
#define RFCOMM_FC_RTC_ON_OUTPUT         0x20
#define RFCOMM_FC_MASK                  0x3F

#define RFCOMM_RPN_PM_BIT_RATE          0x0001
#define RFCOMM_RPN_PM_DATA_BITS         0x0002
#define RFCOMM_RPN_PM_STOP_BITS         0x0004
#define RFCOMM_RPN_PM_PARITY            0x0008
#define RFCOMM_RPN_PM_PARITY_TYPE       0x0010
#define RFCOMM_RPN_PM_XON_CHAR          0x0020
#define RFCOMM_RPN_PM_XOFF_CHAR         0x0040
#define RFCOMM_RPN_PM_XONXOFF_ON_INPUT  0x0100
#define RFCOMM_RPN_PM_XONXOFF_ON_OUTPUT 0x0200
#define RFCOMM_RPN_PM_RTR_ON_INPUT      0x0400
#define RFCOMM_RPN_PM_RTR_ON_OUTPUT     0x0800
#define RFCOMM_RPN_PM_RTC_ON_INPUT      0x1000
#define RFCOMM_RPN_PM_RTC_ON_OUTPUT     0x2000
#define RFCOMM_RPN_PM_MASK              0x3F7F

#define RFCOMM_RLS_ERROR                0x01
#define RFCOMM_RLS_OVERRUN              0x02
#define RFCOMM_RLS_PARITY               0x04
#define RFCOMM_RLS_FRAMING              0x08

/* Multiplexor channel uses DLCI 0 */
#define RFCOMM_MX_DLCI                  0

/*
** Define RFCOMM Multiplexer message types
*/
#define RFCOMM_MX_PN                    0x80
#define RFCOMM_MX_PN_LEN                8

#define RFCOMM_MX_CLD                   0xC0
#define RFCOMM_MX_CLD_LEN               0

#define RFCOMM_MX_TEST                  0x20

#define RFCOMM_MX_FCON                  0xA0
#define RFCOMM_MX_FCON_LEN              0

#define RFCOMM_MX_FCOFF                 0x60
#define RFCOMM_MX_FCOFF_LEN             0

#define RFCOMM_MX_MSC                   0xE0
#define RFCOMM_MX_MSC_LEN_NO_BREAK      2
#define RFCOMM_MX_MSC_LEN_WITH_BREAK    3

#define RFCOMM_MX_NSC                   0x10
#define RFCOMM_MX_NSC_LEN               1

#define RFCOMM_MX_RPN                   0x90
#define RFCOMM_MX_RPN_REQ_LEN           1
#define RFCOMM_MX_RPN_LEN               8

#define RFCOMM_MX_RLS                   0x50
#define RFCOMM_MX_RLS_LEN               2
#endif
