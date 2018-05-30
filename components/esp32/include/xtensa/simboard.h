/*
 * Copyright (c) 2001 Tensilica Inc.
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

/*  simboard.h  -  Xtensa ISS "Board" specific definitions  */

#ifndef _INC_SIMBOARD_H_
#define _INC_SIMBOARD_H_

#include <xtensa/config/core.h>
#include <xtensa/config/system.h>


/*
 *  Device addresses.
 */

/*  System ROM:  */
#define XTBOARD_ROM_SIZE		XSHAL_ROM_SIZE
#ifdef XSHAL_ROM_VADDR
#define XTBOARD_ROM_VADDR	XSHAL_ROM_VADDR
#endif
#ifdef XSHAL_ROM_PADDR
#define XTBOARD_ROM_PADDR	XSHAL_ROM_PADDR
#endif

/*  System RAM:  */
#define XTBOARD_RAM_SIZE		XSHAL_RAM_SIZE
#ifdef XSHAL_RAM_VADDR
#define XTBOARD_RAM_VADDR	XSHAL_RAM_VADDR
#endif
#ifdef XSHAL_RAM_PADDR
#define XTBOARD_RAM_PADDR	XSHAL_RAM_PADDR
#endif


/*
 *  Things that depend on device addresses.
 */

#define XTBOARD_CACHEATTR_WRITEBACK	XSHAL_ISS_CACHEATTR_WRITEBACK
#define XTBOARD_CACHEATTR_WRITEALLOC	XSHAL_ISS_CACHEATTR_WRITEALLOC
#define XTBOARD_CACHEATTR_WRITETHRU	XSHAL_ISS_CACHEATTR_WRITETHRU
#define XTBOARD_CACHEATTR_BYPASS	XSHAL_ISS_CACHEATTR_BYPASS
#define XTBOARD_CACHEATTR_DEFAULT	XSHAL_ISS_CACHEATTR_DEFAULT

#define XTBOARD_BUSINT_PIPE_REGIONS	0
#define XTBOARD_BUSINT_SDRAM_REGIONS	0


#endif /*_INC_SIMBOARD_H_*/

