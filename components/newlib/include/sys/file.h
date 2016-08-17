/* Copyright (c) 2005-2006 Tensilica Inc.  ALL RIGHTS RESERVED.

   Redistribution and use in source and binary forms, with or without 
   modification, are permitted provided that the following conditions are met: 

   1. Redistributions of source code must retain the above copyright 
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
   PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL TENSILICA
   INCORPORATED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  */

#include <sys/fcntl.h>

/* Alternate names for values for the WHENCE argument to `lseek'.
   These are the same as SEEK_SET, SEEK_CUR, and SEEK_END, respectively. */
#ifndef L_SET
#define L_SET   0       /* Seek from beginning of file.  */
#define L_INCR  1       /* Seek from current position.  */
#define L_XTND  2       /* Seek from end of file.  */
#endif
