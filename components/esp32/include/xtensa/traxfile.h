/* TRAX file header definition.

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


#define TRAX_FHEAD_MAGIC	"TRAXdmp"
#define TRAX_FHEAD_VERSION	1

/*  Header flags:  */
#define TRAX_FHEADF_OCD_ENABLED			0x00000001	/* set if OCD was enabled while capturing trace */
#define TRAX_FHEADF_TESTDUMP			0x00000002	/* set if is a test file 
                                                                   (from 'memsave' instead of 'save') */
#define TRAX_FHEADF_OCD_ENABLED_WHILE_EXIT 	0x00000004	/* set if OCD was enabled while capturing trace and
								   we were exiting the OCD mode */

/*  Header at the start of a TRAX dump file.  */
typedef struct {
    char	magic[8];	/* 00: "TRAXdmp\0" (TRAX_FHEAD_MAGIC) */
    char	endianness;	/* 08: 0=little-endian, 1=big-endian */
    char	version;	/* 09: TRAX_FHEAD_VERSION */
    char	reserved0[2];	/* 0A: ... */
    unsigned	filesize;	/* 0C: size of the trace file, including this header */
    unsigned	trace_ofs;	/* 10: start of trace output, byte offset from start of header */
    unsigned	trace_size;	/* 14: size of trace output in bytes */
    unsigned	dumptime;	/* 18: date/time of capture save (secs since 1970-01-01), 0 if unknown */
    unsigned	flags;		/* 1C: misc flags (TRAX_FHEAD_F_xxx) */
    char	username[16];	/* 20: user doing the capture/save (up to 15 chars) */
    char	toolver[24];	/* 30: tool + version used for capture/save (up to 23 chars) */
    char	reserved2[40];	/* 48: (reserved - could be hostname used for dump (up to 39 chars)) */
    unsigned	configid[2];	/* 70: processor ConfigID values, 0 if unknown */
    unsigned	ts_freq;	/* 78: timestamp frequency, 0 if not specified */
    unsigned	reserved3;	/* 7C: (reserved) */
    unsigned	id;		/* 80: TRAX registers at time of save (0 if not read) */
    unsigned	control;
    unsigned	status; 
    unsigned	reserved4;	/* Data register (should not be read) */
    unsigned	address;
    unsigned	trigger; 
    unsigned	match;
    unsigned	delay;
    unsigned	trax_regs[24];	/*100: (total size) -- dummy allocation (FIXME) */
} trax_file_header;

