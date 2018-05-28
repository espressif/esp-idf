/* This file contains utility functions that can be used for polling TRAX
 * or executing higher level save functionality
 * It assumes that print subroutines and file I/O routines are available
 * on the system
 */

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

#ifndef _TRAX_UTIL_H
#define _TRAX_UTIL_H


#ifdef __cplusplus
extern "C" {
#endif

/* User can use this function if he wants to generate a tracefile output.
 * Internally it calls trax_get_trace in a loop until it realizes that
 * the entire trace has been read.
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * filename	: user specified output trace file name. If the file does not
 * 		  exist, it would create the new file, else would append to it
 *
 * returns      : 0 if successful, -1 if unsuccessful
 */
int trax_save (trax_context *context, char *filename);

/* Displays a brief machine readable status.
 *
 * context	: pointer to structure which contains information about the 
 * 		  current TRAX session
 * returns      : 0 if successful, -1 if unsuccessful
 */
int trax_poll (trax_context *context);

#ifdef __cplusplus
}
#endif

#endif
