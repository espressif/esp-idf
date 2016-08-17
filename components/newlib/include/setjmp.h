/*
	setjmp.h
	stubs for future use.
*/

#ifndef _SETJMP_H_
#define _SETJMP_H_

#include "_ansi.h"
#include <machine/setjmp.h>

_BEGIN_STD_C

#ifdef __GNUC__
void	_EXFUN(longjmp,(jmp_buf __jmpb, int __retval))
			__attribute__ ((__noreturn__));
#else
void	_EXFUN(longjmp,(jmp_buf __jmpb, int __retval));
#endif
int	_EXFUN(setjmp,(jmp_buf __jmpb));
#define setjmp(env) setjmp(env)


_END_STD_C

#endif /* _SETJMP_H_ */

