/* This file contains functions that are hidden from the user. These are
 * protocol specific functions used to read and write TRAX registers
 * and the trace memory
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
#ifndef _TRAX_PROTO_H
#define _TRAX_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Function to read register
 * 
 * regno	: The register number to be read (not ERI addressed)
 * data		: Location where the read value is kept
 * 
 * returns	: 0 if successful, -1 if unsuccessful
 */ 
int trax_read_register_eri (int regno, unsigned *data);

/* Function to write a value into a register
 * 
 * regno	: The register number to be written (not ERI addressed)
 * value	: The value to be written at that register location
 * 
 * returns	: 0 if successful, -1 if unsuccessful
 */ 
int trax_write_register_eri (int regno, unsigned value);

/* Function to read memory
 * 
 * address	: Address of the TraceRAM memory, each location has a word
 * len		: Amount of memory in bytes, to be read
 * data		: buffer in which the read memory is stored
 * final_address: Next address to be read in the following call to this 
 * 		  function (trace save mechanism)
 *
 * returns	: 0 if successful, -1 if unsuccessful
 */ 
int trax_read_memory_eri (unsigned address, int len, int *data, 
			  unsigned *final_address);

/* Function to write a value to the memory address
 * 
 * address	: Address of the TraceRAM memory
 * value	: The value to be written inside that location
 * 
 * returns	: 0 if successful, -1 if unsuccessful
 */ 
int trax_write_memory_eri (int address, unsigned value);

/* Function to write to a  subfield of the register. 
 * Called by set and show parameter functions.
 * 
 * regno	: Register number
 * regmask	: Mask in order to toggle appropriate bits
 * value	: Value to be written in the masked location
 * 
 * returns	: 0 if successful, -1 if unsuccessful
 */
int trax_write_register_field_eri (int regno, unsigned regmask, 
				   unsigned value);

#ifdef __cplusplus
}
#endif

#endif
