/* Copyright (c) 2004-2006 by Tensilica Inc.  ALL RIGHTS RESERVED.
/  These coded instructions, statements, and computer programs are the
/  copyrighted works and confidential proprietary information of Tensilica Inc.
/  They may not be modified, copied, reproduced, distributed, or disclosed to
/  third parties in any manner, medium, or form, in whole or in part, without
/  the prior written consent of Tensilica Inc.
*/

/*  sim.h 
 *
 *  Definitions and prototypes for specific ISS SIMCALLs
 *  (ie. outside the standard C library).
 */

#ifndef _INC_SIM_H_
#define _INC_SIM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Shortcuts for enabling/disabling profiling in the Xtensa ISS  */
extern void xt_iss_profile_enable(void);
extern void xt_iss_profile_disable(void);

/* Shortcut for setting the trace level in the Xtensa ISS */
extern void xt_iss_trace_level(unsigned level);

/* Generic interface for passing client commands in the Xtensa ISS:
 *   returns 0 on success, -1 on failure.
 */
extern int xt_iss_client_command(const char *client, const char *command);

/* Interface for switching simulation modes in the Xtensa ISS:
 *   returns 0 on success, -1 on failure.
 */
#define XT_ISS_CYCLE_ACCURATE 0
#define XT_ISS_FUNCTIONAL     1
extern int xt_iss_switch_mode(int mode);


/* Interface for waiting on a system synchronization event */
extern void xt_iss_event_wait(unsigned event_id);

/* Interface for firing a system synchronization event */
extern void xt_iss_event_fire(unsigned event_id);

/* Interface for invoking a user simcall action,
 * which can be registered in XTMP or XTSC.
 */
extern int xt_iss_simcall(int arg1, int arg2, int arg3,
                          int arg4, int arg5, int arg6);


#ifdef __cplusplus
}
#endif

#endif /*_INC_SIM_H_*/

