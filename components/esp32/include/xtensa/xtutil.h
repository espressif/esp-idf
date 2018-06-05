
/* $Id$ */
/*******************************************************************************/
/* Copyright (c) 2001-2013 by Tensilica Inc.  ALL RIGHTS RESERVED.             */
/* These coded instructions, statements, and computer programs are the         */
/* copyrighted works and confidential proprietary information of Tensilica Inc.*/
/* They may not be modified, copied, reproduced, distributed, or disclosed to  */
/* third parties in any manner, medium, or form, in whole or in part, without  */
/* the prior written consent of Tensilica Inc.                                 */
/*******************************************************************************/

#ifndef XTUTIL_H
#define XTUTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

extern int  xt_putchar(int c);
extern int  xt_puts(const char * s);
extern void xt_putn(unsigned n);
extern int  xt_atoi(const char * s);
extern int  xt_printf(const char *fmt, ...);
extern int  xt_sprintf(char * buf, const char * fmt, ...);

typedef int xt_output_fn(int *, int, const void *, int);
extern xt_output_fn * xt_set_output_fn(xt_output_fn * fn);

#ifdef XTUTIL_LIB

// Only defined if building library

typedef void (xt_outbuf_fn)(void *, char *, int);

extern int  xt_vprintf(xt_outbuf_fn * out, void * outarg, const char * fmt, va_list ap);

#else

// Only defined if building application and overriding

#ifndef XTUTIL_NO_OVERRIDE

#define putchar     xt_putchar
#define puts        xt_puts
#define putn        xt_putn
#define atoi        xt_atoi
#define printf      xt_printf
#define sprintf     xt_sprintf

#endif // XTUTIL_NO_OVERRIDE

#endif // XTUTIL_LIB

#ifdef __cplusplus
}
#endif

#endif // XTUTIL_H

