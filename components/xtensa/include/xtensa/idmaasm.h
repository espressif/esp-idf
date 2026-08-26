/* $Id: //depot/dev/Foxhill/Xtensa/OS/include/xtensa/mpuasm.h#5 $ */

/*
 * Copyright (c) 2016 Cadence Design Systems, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _IDMAASM_H_
#define _IDMAASM_H_

#if XCHAL_HAVE_IDMA
#include <xtensa/idma.h>
#endif

/*
 * Macro for restore IDMA regs
 *
 * Parameters:
 * 	a_save				=>	address register containing pointer to IDMA save area
 * 	a_temp1, a_temp2, a_temp3.	=>  address register temporaries
 */
//   IDMA_REG_SETTINGS,
//   IDMA_REG_TIMEOUT,
//   IDMA_REG_DESC_START,
//   IDMA_REG_CONTROL,
//   IDMA_REG_USERPRIV,

.macro _idma_restore a_save, a_temp1, a_temp2, a_temp3
#if XCHAL_HAVE_IDMA
	l32i	\a_temp1, \a_save, 0
	movi	\a_temp3, idmareg_base
	movi	\a_temp2, IDMA_REG_SETTINGS
	add	\a_temp2, \a_temp2, \a_temp3
	wer	\a_temp1, \a_temp2
	l32i	\a_temp1, \a_save, 4
	movi	\a_temp2, IDMA_REG_TIMEOUT
	add	\a_temp2, \a_temp2, \a_temp3
	wer	\a_temp1, \a_temp2
	l32i	\a_temp1, \a_save, 8
	movi	\a_temp2, IDMA_REG_DESC_START
	add	\a_temp2, \a_temp2, \a_temp3
	wer	\a_temp1, \a_temp2
	l32i	\a_temp1, \a_save, 12
	movi	\a_temp2, IDMA_REG_CONTROL
	add	\a_temp2, \a_temp2, \a_temp3
	wer	\a_temp1, \a_temp2
	l32i	\a_temp1, \a_save, 16
	movi	\a_temp2, IDMA_REG_USERPRIV
	add	\a_temp2, \a_temp2, \a_temp3
	wer	\a_temp1, \a_temp2
#endif
.endm

#endif //_IDMAASM_H_
