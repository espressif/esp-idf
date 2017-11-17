/******************************************************************************
 *
 *  Copyright (C) 2001-2012 Broadcom Corporation
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

/******************************************************************************
 *
 *  this file contains constants and definitions for the bte project
 *
 ******************************************************************************/
#ifndef BTE_H
#define BTE_H

//#include <semaphore.h>
//#include <signal.h>
//#include <pthread.h>
#include "bt_target.h"

/* by default on shutdown, baudrate is reset 115kbits. this should NOT be need for platforms
 * that kill BTE driver and remove/reset BT chip
 */
#ifndef BTE_RESET_BAUD_ON_BT_DISABLE
#define BTE_RESET_BAUD_ON_BT_DISABLE TRUE
#endif

/* Target Modes (based on jumper settings on hardware [see user manual]) */
enum {
    /* BTE                  BBY                     */
    /* J3   J4              SW3-3   SW3-2   SW3-1   */
    /* -------------------------------------------- */
    BTE_MODE_SERIAL_APP,    /* OUT  OUT             OFF     OFF     OFF     Sample serial port application      */
    BTE_MODE_APPL,          /* IN   OUT             OFF     OFF     ON      Target used with Tester through RPC */
    BTE_MODE_RESERVED,      /* OUT  IN              OFF     ON      OFF     Reserved                            */
    BTE_MODE_SAMPLE_APPS,   /* IN   IN              OFF     ON      ON      Sample applications (ICP/HSP)       */
    BTE_MODE_DONGLE,        /* not yet supported    ON      OFF     OFF     Dongle mode                         */
    BTE_MODE_APPL_PROTOCOL_TRACE, /* this is a fake mode do allow protocol tracing in application without rpc */
    BTE_MODE_INVALID
};

extern volatile UINT8    bte_target_mode;    /* indicates the mode that the board is running in */

/* Startup options */
extern UINT32 bte_startup_options;                      /* Switch and jumper settings at startup */
void bte_get_startup_options(UINT32 *p_options);        /* Platform specific function for getting startup options */

#define BTE_OPTIONS_TARGET_MODE_MASK    0x00000007      /* bits 2-0 indicate target mode (QuickConnect: jp3 & jp4, BBY: SW3-1 & SW3-2)*/


/****************************************************************************
 * Definitions to define which type of application gets built
 ****************************************************************************/
#define BUILD_HCITOOL         FALSE
#define BUILD_L2PING          FALSE


#define LINUX_FM_DRIVER_INCLUDED        FALSE


/* hcisu userial operations. should probably go into bt_types to avoid collisions! */
#define BT_EVT_TO_HCISU_USERIAL_OP (0x0080 | BT_EVT_HCISU)
/* operation for above hcisu event */
#define BT_HCISU_USERIAL_OPEN (0)   /* open serial port calling USERIAL_Open() */
#define BT_HCISU_USERIAL_CLOSE (1)  /* close userial port */
/* options associated with close op */
#define BT_HCISU_USERIAL_CL_NO_DIS_BT   0 /* do not touch bt_wake and power gpio */
#define BT_HCISU_USERIAL_CL_DIS_BT      1 /* put power and bt_wake into defined off state to preserve
                                             power */
/* status codes for callback */
/*
#define BTE_HCISU_USERIAL_FAIL      0
#define BTE_HCISU_USERIAL_OK        1
typedef void (tUSERIAL_MSG_CBACK) (int status);
typedef struct tHCISU_USERIAL_MSG_tag {
    BT_HDR      hdr;
    tUSERIAL_MSG_CBACK *p_cback;
    UINT8       port;   // port number
    UINT8       op;
    UINT8       option; // option for operation. depends on operation
} tHCISU_USERIAL_MSG;

extern void bte_hcisu_userial_oper( tUSERIAL_MSG_CBACK *p_cback, UINT8 port, UINT8 op, UINT8 option );
*/

/* Pointer to function for sending HCI commands and data to the HCI tranport */
extern int (*p_bte_hci_send)(UINT16 port, BT_HDR *p_msg);


/* Protocol trace mask */
extern UINT32 bte_proto_trace_mask;

typedef struct tBAUD_REG_tag {
    UINT8 DHBR;
    UINT8 DLBR;
    UINT8 ExplicitBaudRate0;
    UINT8 ExplicitBaudRate1;
    UINT8 ExplicitBaudRate2;
    UINT8 ExplicitBaudRate3;
} tBAUD_REG;


extern const tBAUD_REG baud_rate_regs[];

#endif  /* BTE_H */
