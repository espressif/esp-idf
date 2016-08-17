/* Misc TRAX API function definitions.

   Copyright (c) 2007-2012 Tensilica Inc.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

#ifndef _TRAX_API_H_
#define _TRAX_API_H_

#include <xtensa-isa.h>
#include <xtensa-params.h>
#include "tpack.h"
#include "traxreg.h"

#include "xdm-regs.h"

/*  Flags for trax_stop():  */
#define TRAX_STOP_HALT	0x0001	/* halt immediately, don't wait for post-stop-trigger capture */
#define TRAX_STOP_QUIET	0x0002	/* don't display informative messages */


/*
 *  Describes a TRAX channel (based on tpack).
 */
typedef struct {
    tpack_channel	chan;		/* channel structure header */
    /*  Per TRAX unit information:  */
    int			trax_version;	/* TRAX_ID_VER(id), one of TRAX_VER_xxx macros */
    unsigned long	trax_tram_size;	/* size of trace RAM in bytes */
    int			trax_erratum10;	/* set if TRAX 1.0 erratum workarounds needed */
    int			trax_erratum20;	/* set if TRAX 2.0 erratum workaround needed (PR 22161)*/
    int			trax_erratum20_size;
    int			trax_has_busy;	/* has trace-busy feature */
    int			trax_has_atb;	/* has ATB feature */
    /*FIXME: add various features:  coresight regs (don't call it that), APB, ATB, TRAM, ... */
} trax_channel;


/*  Prototypes:  */

/*  TRAX Protocol API:  */
extern int  trax_read_register(tpack_channel *tchan, int regno, unsigned *value);
extern int  trax_write_register(tpack_channel *tchan, int regno, unsigned value);
extern int  trax_read_memory(tpack_channel *tchan, int address, int size, unsigned char *pdata);
extern int  trax_fill_memory(tpack_channel *tchan, int address, int size, tpack_u32 pattern);
extern int  trax_enumerate_devices(tpack_channel *tchan, int * buf, int * size);

/*  TRAX Network API:  */
extern unsigned long  trax_ram_size(tpack_channel *traxchan);
extern unsigned long  trax_ram_size_addr(tpack_channel *traxchan);
extern int  trax_create_tracefile(tpack_channel *traxchan, int size, unsigned char * data,
				char *filename, int hflags, const char *toolver);
extern int  trax_memaccess_safe(tpack_channel *traxchan, const char *what);
extern int  trax_start(tpack_channel *traxchan, int flags);
extern int  trax_stop(tpack_channel *traxchan, int flags);
extern int  trax_halt(tpack_channel *traxchan, int flags);
extern int  trax_save(tpack_channel *traxchan, char *filename, int flags, const char *toolver, int erratum);

/*  TRAX Misc API (no network dependencies):  */
int trax_fixed_hw(unsigned * regs);
extern int  trax_display_id(unsigned id, const char *prefix);
extern int  trax_display_summary(unsigned id,
				 unsigned status, 
				 unsigned control, 
				 unsigned address, 
				 unsigned delay, 
				 unsigned trigger, 
				 unsigned match, 
				 unsigned startaddr,
				 unsigned endaddr,
				 const char *prefix);

/*  Other:  */

#endif /* _TRAX_API_H_ */

