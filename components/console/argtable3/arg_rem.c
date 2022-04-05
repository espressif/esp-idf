/*
 * SPDX-FileCopyrightText: 1998-2001,2003-2011,2013 Stewart Heitmann
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*******************************************************************************
 * arg_rem: Implements the rem command-line option
 *
 * This file is part of the argtable3 library.
 *
 * Copyright (C) 1998-2001,2003-2011,2013 Stewart Heitmann
 * <sheitmann@users.sourceforge.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of STEWART HEITMANN nor the  names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL STEWART HEITMANN BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include "argtable3.h"

#ifndef ARG_AMALGAMATION
#include "argtable3_private.h"
#endif

#include <stdlib.h>

struct arg_rem* arg_rem(const char* datatype, const char* glossary) {
    struct arg_rem* result = (struct arg_rem*)xmalloc(sizeof(struct arg_rem));

    result->hdr.flag = 0;
    result->hdr.shortopts = NULL;
    result->hdr.longopts = NULL;
    result->hdr.datatype = datatype;
    result->hdr.glossary = glossary;
    result->hdr.mincount = 1;
    result->hdr.maxcount = 1;
    result->hdr.parent = result;
    result->hdr.resetfn = NULL;
    result->hdr.scanfn = NULL;
    result->hdr.checkfn = NULL;
    result->hdr.errorfn = NULL;

    ARG_TRACE(("arg_rem() returns %p\n", result));
    return result;
}
