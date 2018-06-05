/* 
 * Customer ID=11656; Build=0x5f626; Copyright (c) 2009-2013 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc.  They may be adapted and modified by bona fide
 * purchasers for internal use, but neither the original nor any
 * adapted or modified version may be disclosed or distributed to
 * third parties in any manner, medium, or form, in whole or in part,
 * without the prior written consent of Tensilica Inc.
 *
 * This software and its derivatives are to be executed solely on
 * products incorporating a Tensilica processor.
 */

// Utility routines for returning pass/fail status in HW simulations

#ifndef XT_REF_TESTBENCH_H
#define XT_REF_TESTBENCH_H 1

#ifdef __cplusplus
extern "C" {
#endif

// Exit routines for HW simulation
extern int diag_pass();
extern int diag_fail();

// Set exit status for HW simulation
int set_diag_status(int stat);

// Setup for user power toggling
extern int setup_power_toggle();

// Return exit status location
extern unsigned int* testbench_exit_location();
// Return power toggle location
extern unsigned int* testbench_power_toggle_location();


// Change exit status location
// You must also change the plusarg "+DVMagicExit" sent to the HW simulator
// or change the argument "--exit_location" sent to the ISS
extern unsigned int* set_testbench_exit_location(unsigned int*);
// Change power toggle location
// You must also change the plusarg "+DVPowerLoc" sent to the HW simulator
extern unsigned int* set_testbench_power_toggle_location(unsigned int*);

// Exit routines with status message
//
static inline
int pass(const char *msg) 
{
  return diag_pass();
}

static inline
int fail(const char *msg) 
{
  return diag_fail();
}

#define POWER_TOGGLE_ON 1
#define POWER_TOGGLE_OFF 0

// Routine to turn on and off power toggle
//   Does a magic write that Monitors.v intercepts and appropriately turns 
//   SAIF dumping on and offf
//   
extern volatile unsigned int *_reftb_power_toggle;

__attribute__ ((always_inline))
static inline
int set_power_toggle(int val) 
{
#ifdef __XTENSA__
  *_reftb_power_toggle = val;
#endif
  return val;
}

#ifdef __cplusplus
}
#endif

#endif // XT_REF_TESTBENCH_H

