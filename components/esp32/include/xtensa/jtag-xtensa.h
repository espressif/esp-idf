/* Copyright (c) 2011-2012 Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of Tensilica Inc.
// They may not be modified, copied, reproduced, distributed, or disclosed to
// third parties in any manner, medium, or form, in whole or in part, without
// the prior written consent of Tensilica Inc.
*/

#ifndef _JTAG_XTENSA_H_
#define _JTAG_XTENSA_H_


/* ---------------- JTAG registers ------------------ */

/* -- ER and later JTAG registers */
typedef enum {
	regIR,
	regBypass,
	regNAR,
	regNDR,
	regIdcode,
	regPWRCTL,
	regPWRSTAT,
	regJtagMAX, 
} xtensaJtagReg;

/* -- pre-ER JTAG registers */
typedef enum {
	regOldIR,
	regOldBypass,
	regOldDIRW,
	regOldDIR,
	regOldDDR,
	regOldDOSR,
	regOldESR,
	regOldDCR,
	regOldTraxNDR,
	regOldTraxNAR,
	regOldMAX
} xtensaOldJtagReg;


/* ---------------- JTAG Instructions ------------------ */

/* -- pre-ER JTAG instructions */
typedef enum {
	ji_EnableOCD = 0x11,
	ji_DebugInt,
	ji_RetDebugInt,  // TBD: remove
	ji_DisRetOCD,    // TBD: remove
	ji_ExecuteDI,
	ji_LoadDI,
	ji_ScanDDR,
	ji_ReadDOSR,
	ji_ScanDCR,
	ji_LoadWDI,
	ji_TRAX   = 0x1c,
	ji_BYPASS = 0x1f,
} xtensaJtagInstruction;

typedef enum {
	OCDNormalMode,
	OCDRunMode,
	OCDHaltMode,
	OCDStepMode
} xtensaMode;

typedef struct {
	xtensaMode mode;
	int DRsel;
	XTMP_core core;
	XTMP_tap tap;
	int core_num;
	jtagReg_t *jtagRegs;
	void *dap;             // used for ARM DAP only
	bool isBig;
	int dir_array_option;  // used by pre-ER devices only 
	// for testing, below - FIXME - delete later
	int ocdReg;
	unsigned int wr_data;
	XTMP_event start_OCD_trans;
	bool data_cycle;
	bool data_pending;
} coreSlaveData_t;


enum OCD_ACCESS_TYPE{
   NEXUS_ACCESS,
   CS_ACCESS,
};

// pre-ER Xtensa initializiation
EXTERN XTMP_deviceStatus
XTMP_jtagCoreSlaveEX(XTMP_component component, XTMP_jtagSlave slave, void* mydata);

extern char *OCDrd;
extern char *OCDwr;

#endif
