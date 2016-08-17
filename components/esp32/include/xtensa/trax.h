/* Header file for TRAX control Library */

/*
 * Copyright (c) 2012-2013 Tensilica Inc.
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

#ifndef _TRAX_H
#define _TRAX_H

#ifdef __cplusplus
extern "C" {
#endif

#define TRAX_STOP_HALT 		0x0001
#define TRAX_STOP_QUIET 	0x0002

/* Flag values to indicate if the user wanted to reverse the pcstop
 * parameters */
#define	TRAX_PCSTOP_REVERSE	0x0001
#define TRAX_PCSTOP_NO_REVERSE	0x0000

/* Indicating whether postsize should be in terms of bytes, instructions
 * or percentage of trace size captured */
#define	TRAX_POSTSIZE_BYTES	0x0000
#define	TRAX_POSTSIZE_INSTR	0x0001
#define	TRAX_POSTSIZE_PERCENT	0x0002

/* Size of the header inside the trace file */
#define	TRAX_HEADER_SIZE	256

/* Minimum size between start and end addresses */
#define	TRAX_MIN_TRACEMEM	64

/* For basic debugging */
#define DEBUG 0

#include <stdbool.h>

#define ffs(i) __builtin_ffs(i)

/* Data structures */

/* Represents the context of the TRAX unit and the current TRAX session.
 * To be used by set and show function calls to set and show appropriate
 * parameters of appropriate TRAX unit.
 */ 

typedef struct {
  int           trax_version;		/* TRAX PC version information */
  unsigned long trax_tram_size;		/* If trace RAM is present,size of it */
  int		hflags;			/* Flags that can be used to debug, 
  					   print info, etc. */
  int		address_read_last;	/* During saving of the trace, this
  					   indicates the address from which
					   the current trace reading must
					   resume */
  unsigned long	bytes_read;		/* bytes read uptil now */
  unsigned long total_memlen;		/* Total bytes to be read based on the 
  				           trace collected in the trace RAM */
  bool		get_trace_started;	/* indicates that the first chunk of
  					   bytes (which include the header) has
					   been read */
} trax_context;


/* -----------------------TRAX Initialization ------------------------------*/

/* Initializing the trax context. Reads registers and sets values for version,
 * trace RAM size, total memory length, etc. Most of the other values are
 * initialized to their default case.
 * 
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 *
 * returns	: 0 if successful, -1 if unsuccessful, -2 if ram_size if 
 * 		  incorrect
 */
int trax_context_init_eri (trax_context *context);

/* -----------------Starting/Stopping TRAX session -------------------------*/

/* Start tracing with current parameter setting. If tracing is already in 
 * progress, an error is reported. Otherwise, tracing starts and any unsaved 
 * contents of the TraceRAM is discarded
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * returns      : 0 if successful, 1 if trace is already active,
 * 		  -1 if unsuccessful
 */
int trax_start (trax_context *context);

/* This command initiates a stop trigger or halts a trace session based of the
 * value of the flag parameter passed. In case stop trigger is initiated, any
 * selected post-stop-trigger capture proceeds normally.
 * If trace capture was not in progress, or a stop was already triggered, the 
 * return value indicates appropriately.
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * flags	: To differentiate between stopping trace without any 
 * 		  post-size-trigger capture (trax_halt) or with that.
 * 		  A zero value would stop the trace based on trigger and a 
 * 		  value of one would halt it
 *
 * returns      : 0 if successful, 1 if already stopped, -1 if unsuccessful
 */
int trax_stop_halt (trax_context *context, int flags);

/* Resets the TRAX parameters to their default values which would internally
 * involve resetting the TRAX registers. To invoke another trace session or
 * reset the current tracing mechanism, this function needs to be called as
 * it resets parameters of the context that deal with tracing information
 * 
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 *
 * returns      : 0 if successful, -1 if unsuccessful
 */
int trax_reset (trax_context *context);

/* ---------------Set/Get several TRAX parameters --------------------------*/

/* Sets the start address and end address (word aligned) of the trace in the 
 * TraceRAM. Care must be taken to ensure that the difference between the
 * start and the end addresses is atleast TRAX_MIN_TRACEMEM bytes. If not, 
 * the values are reset to default, which is 0 for startaddr and 
 * traceRAM_words -1 for endaddr
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * startaddr	: value to which the start address must be set. Can be 
 * 		  any value between 0 - (traceRAM_words - 1)
 * endaddr	: value to which the end address must be set. Can be any value
 * 		  between 0 - (traceRAM_words - 1)
 *
 * returns      : 0 if successful, -1 if unsuccessful, -2 if the difference
 * 		  between the start and end addresses is less than 
 * 		  TRAX_MIN_TRACEMEM bytes or if they are passed incorrect 
 * 		  values, -3 if memory shared option is not configured, in
 * 		  which case, start and end addresses are set to default
 * 		  values instead of those passed by the user
 */
int trax_set_ram_boundaries (trax_context *context, unsigned startaddr, 
				unsigned endaddr);

/* Shows the start address and end address(word aligned) of the trace in the 
 * TraceRAM. If incorrect, the startaddress and the endaddress values are
 * set to default, i.e. 0 for startaddr and traceRAM_words - 1 for endaddr
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * startaddr	: pointer to value which will contain the start address
 * endaddr	: pointer to value which will contain the end address
 * 
 * returns      : 0 if successful, -1 if unsuccessful
 *
 */
int trax_get_ram_boundaries (trax_context *context, unsigned *startaddr,
				unsigned *endaddr);

/* Selects stop trigger via cross-trigger input
 * 
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * value	: 0 = off (reset value), 1 = on
 *
 * returns      : 0 if successful, -1 if unsuccessful
 */
int trax_set_ctistop (trax_context *context, unsigned value);

/* Shows if stop-trigger via cross-trigger input is off or on 
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * returns      : 0 if off, 1 if on, -1 if unsuccessful
 */
int trax_get_ctistop (trax_context *context);

/* Selects stop trigger via processor-trigger input
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * value	: 0 = off (reset value), 1 = on
 *
 * returns      : 0 if successful, -1 if unsuccessful
 */
int trax_set_ptistop (trax_context *context, unsigned value);

/* Shows if stop trigger visa processor-trigger input is off or on
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * returns	: 0 if off, 1 if on, -1 if unsuccessful
 */
int trax_get_ptistop (trax_context *context);

/* Reports cross trigger output state
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * returns      : 0 if CTO bit is reset, 1 if CTO bit is set 
 */
int trax_get_cto (trax_context *context);

/* Reports processor trigger output state
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * returns      : 0 if PTO bit is reset, 1 if PTO bit is set
 */
int trax_get_pto (trax_context *context);

/* Selects condition that asserts cross trigger output
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * option	: 0 = off(reset value)/1 = ontrig/2 = onhalt
 *
 * returns      : 0 if successful, -1 if unsuccessful
 */
int trax_set_ctowhen (trax_context *context, int option);

/* Shows condition that asserted cross trigger output. It can be
 * any of: ontrig or onhalt or even off
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 *
 * returns      : 0 if off, 1 if ontrig, 2 if onhalt, -1 if unsuccessful
 */
int trax_get_ctowhen (trax_context *context);

/* Selects condition that asserts processor trigger output
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * option	: 0 = off(reset value)/1 = ontrig/2 = onhalt
 *
 * returns      : 0 if successful, -1 if unsuccessful
 */
int trax_set_ptowhen (trax_context *context, int option);


/* Shows condition that asserted processor trigger output. It can be
 * any of: ontrig or onhalt or even off
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * returns      : 0 if off, 1 if ontrig, 2 if onhalt, -1 if unsuccessful
 */
int trax_get_ptowhen (trax_context *context);

/* Selects the trace synchronization message period.
 * If ATEN enabled, we cannot allow syncper to be off, set it to reset value. 
 * Also, if no trace RAM, and ATEN enabled, set syncper to be reset value
 * i.e. 256. A value of 1 i.e. on indicates that internally the message
 * frequency is set to an optimal value. This option should be preferred
 * if the user is not sure what message frequency option to set for the
 * trace session.
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * option	: 0 = off, 1 = on, -1 = auto, 8, 16, 32, 64, 128, 
 * 		  256 (reset value)
 *
 * returns      : 0 if successful, -1 if unsuccessful, -2 if incorrect
 * 		  arguments
 */
int trax_set_syncper (trax_context *context, int option);

/* Shows trace synchronization message period. Can be one of:
 * off, on, auto, 8, 16, 32, 64, 128, 256 (reset value)
 *
 * context	: pointer to structure which contains information about the
 * 		  current TRAX session
 * returns      : value of sync period, 0 if off, -1 if unsuccessful
 */ 
int trax_get_syncper (trax_context *context);

/* Selects stop trigger via PC match. Specifies the address or
 * address range to match against program counter. Trace stops when the
 * processor executes an instruction matching the specified address
 * or range. 
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * index	: indicates the number of stop trigger (currently there is
 * 		  only one i.e. index = 0)
 * startaddress	: start range of the address at which the stop trigger 
 * 		  should be activated
 * enaddress	: end range of the address at which the stop trigger should
 * 		  be activated
 * flags	: If non-zero, this inverts the range. i.e. trace stops
 * 		  when the processor executes an instruction that does not
 * 		  match the specified address or range
 *
 * returns      : 0 if successful, -1 if unsuccessful, -2 if incorrect
 * 		  arguments (unaligned)
 *
 * Note		: For the current version of TRAX library, the endaddress and
 * 		  startaddress can differ by at most 31 bytes and the total
 * 		  range i.e. (endaddress - startaddress + 1) has to be a power
 * 		  of two
 */
int trax_set_pcstop (trax_context *context, int index, unsigned long startaddress, 
		      unsigned long endaddress, int flags);

/* Shows the stop trigger via PC match
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * index	: container of information about the number of stop triggers
 * startaddress	: container of start range of stop trigger
 * endaddress	: container of end range of stop trigger
 * flags	: container of information whcih indicates whether the
 * 		  pc stop range is inverted or not.
 *
 * returns      : 0 if successful, -1 if unsuccessful
 */
int trax_get_pcstop (trax_context *context, int *index,
			   unsigned long *startaddress,
			   unsigned long *endaddress, int *flags);

/* This function is used to set the amount of trace to be captured past
 * the stop trigger.
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * count_unit	: contains the count of units (instructions or bytes) to be 
 * 		  captured post trigger. If 0, it implies that this is off
 * unit		: unit of measuring the count. 0 is bytes, 1 is instructions
 * 		  2 is percentage of trace
 *
 * returns      : 0 if successful, -1 if unsuccessful, -2 if incorrect
 * 		  arguments
 *
 */
int trax_set_postsize (trax_context *context, int count_unit, int unit);

/* This function shows the amount of TraceRAM in terms of the number of 
 * instructions or bytes, captured post the stop trigger
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * count_unit	: will contain the count of units(instructions or bytes) post 
 * 		  trigger
 * unit		: will contain information about the events that are counted
 * 		  0 implies that the traceRAM words consumed are counted and
 * 		  1 implies that the target processor instructions executed and
 * 		  excpetions/interrupts taken are counted
 *
 * returns      : 0 if postsize was got successfully, -1 if unsuccessful
 */
int trax_get_postsize (trax_context *context, int *count_unit, int *unit);

/* -------------------------- TRAX save routines ---------------------------*/

/* This function should be called by the user to return a chunk of
 * bytes in buf. It can be a lower layer function of save, or can be
 * called by the user explicitly. If bytes_actually_read contains a 0
 * after a call to this function has been made, it implies that the entire 
 * trace has been read successfully.
 * 
 * context		: pointer to structure which contains information about
 * 			  the current TRAX session
 * buf			: Buffer that is allocated by the user, all the trace 
 *			  data read would be put in this buffer, which can then
 *			  be used to generate a tracefile. 
 *			  The first TRAX_HEADER_SIZE of the buffer will always 
 * 			  contain the header information. 
 * bytes_to_be_read	: Indicates the bytes the user wants to read. The first
 * 			  invocation would need this parameter to be 
 * 			  TRAX_HEADER_SIZE at least.
 * 
 * returns      	: bytes actually read during the call to this function. 
 * 			  0 implies that all the bytes in the trace have been 
 * 			  read, -1 if unsuccessful read/write of
 * 			  registers or memory, -2 if trace was active while
 * 			  this function was called, -3 if user enters  
 * 			  bytes_to_be_read  < TRAX_HEADER_SIZE in the first 
 * 			  pass
 */ 
int trax_get_trace (trax_context *context, void *buf, 
                    int bytes_to_be_read);
#ifdef __cplusplus
}
#endif

#endif /* _TRAX_H */
