/* xmon.h - XMON definitions
 *
 * $Id: //depot/rel/Eaglenest/Xtensa/OS/xmon/xmon.h#3 $
 *
 * Copyright (c) 2001-2013 Tensilica Inc.
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

#ifndef __H_XMON
#define __H_XMON

/* Default GDB packet size */
#define GDB_PKT_SIZE 4096

/*XMON signals */
#define XMON_SIGINT    2   /*target was interrupted */
#define XMON_SIGILL    4   /*illegal instruction */
#define XMON_SIGTRAP   5   /*general exception */
#define XMON_SIGSEGV   11  /*page faults */


/* Type of log message from XMON to the application */
typedef enum {
   XMON_LOG,
   XMON_TRACE,
   XMON_ERR,
   XMON_APP,
   XMON_GDB
} xmon_log_t;

/* Return value type for xmon_proc() (see below) */
typedef enum {
   XMON_GDB_PEND,
   XMON_GDB_PKT,
   XMON_NOT_GDB
} xmon_gdb_pkt_t;

#ifdef  _cplusplus
extern "C" {
#endif

/*
 * THE FOLLOWING ROUTINES ARE USED BY USER
 */
extern int _xmon_init(char* gdbBuf, int gdbPktSize,
                       void(*xlog)(xmon_log_t type, const char* str));
//Initialize GDB communication and logging to the main app.
//For the logging to work, xlog function needs to be provided.
//gdbBuf     - pointer to a buffer XMON can use to comm. with GDB
//gdbPktSize - Size of the allocated buffer for GDB communication.
//xlog       - logger handle.


extern void _xmon_close(void);
//Main application can detach from xmon at any time


extern xmon_gdb_pkt_t _xmon_proc(char);
// Give character to XMON to check if GDB message
// Application is supposed to accumulate all the
// character in case the recognition fails and chars
// have to be sent to the original handler
// Return: XMON_GDB_PEND - send me more chars
//         XMON_GDB_PKT  - GDB message confirmed, C) not
//         XMON_NOT_GDB  - not GDB message


/*
 * THE FOLLOWING ROUTINES NEED TO BE PROVIDED BY USER
 */
extern int  _xmon_in();         // wait for character from GDB
extern void _xmon_out(char);    // output a character to GDB
extern int  _xmon_flush(void);  // flush output characters

#ifdef  _cplusplus
}
#endif

#endif
