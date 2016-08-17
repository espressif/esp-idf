
/*
 * Copyright (c) 2013 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

#ifndef __XT_FEEDBACK_INCLUDED__
#define __XT_FEEDBACK_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

/* xt_feedback_save_and_reset

   Save and reset the accumulated feedback data.
*/
extern void xt_feedback_save_and_reset(void);

/* xt_feedback_enable

   Turn on feedback accumulation. Ordinarily, feedback accumulation is on
   by default. If you turn it off using xt_feedback_disable, You can turn 
   it on again via this function.
*/
extern void xt_feedback_enable (void);

/* xt_feedback_disable

   Turn off feedback accumulation. If you don't want to gather feedback for a
   portion of your code, use this function and then xt_feedback_enable when
   you want to start again.
*/
extern void xt_feedback_disable (void);

#ifdef __cplusplus
}
#endif

#endif /* __XT_FEEDBACK_INCLUDED__ */

