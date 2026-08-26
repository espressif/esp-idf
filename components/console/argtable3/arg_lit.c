/*
 * SPDX-FileCopyrightText: 1998-2001,2003-2011,2013 Stewart Heitmann
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*******************************************************************************
 * arg_lit: Implements the literature command-line option
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

static void arg_lit_resetfn(struct arg_lit* parent) {
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}

static int arg_lit_scanfn(struct arg_lit* parent, const char* argval) {
    int errorcode = 0;
    if (parent->count < parent->hdr.maxcount)
        parent->count++;
    else
        errorcode = ARG_ERR_MAXCOUNT;

    ARG_TRACE(("%s:scanfn(%p,%s) returns %d\n", __FILE__, parent, argval, errorcode));
    return errorcode;
}

static int arg_lit_checkfn(struct arg_lit* parent) {
    int errorcode = (parent->count < parent->hdr.mincount) ? ARG_ERR_MINCOUNT : 0;
    ARG_TRACE(("%s:checkfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}

static void arg_lit_errorfn(struct arg_lit* parent, arg_dstr_t ds, int errorcode, const char* argval, const char* progname) {
    const char* shortopts = parent->hdr.shortopts;
    const char* longopts = parent->hdr.longopts;
    const char* datatype = parent->hdr.datatype;

    switch (errorcode) {
        case ARG_ERR_MINCOUNT:
            arg_dstr_catf(ds, "%s: missing option ", progname);
            arg_print_option_ds(ds, shortopts, longopts, datatype, "\n");
            arg_dstr_cat(ds, "\n");
            break;

        case ARG_ERR_MAXCOUNT:
            arg_dstr_catf(ds, "%s: extraneous option ", progname);
            arg_print_option_ds(ds, shortopts, longopts, datatype, "\n");
            break;
    }

    ARG_TRACE(("%s:errorfn(%p, %p, %d, %s, %s)\n", __FILE__, parent, ds, errorcode, argval, progname));
}

struct arg_lit* arg_lit0(const char* shortopts, const char* longopts, const char* glossary) {
    return arg_litn(shortopts, longopts, 0, 1, glossary);
}

struct arg_lit* arg_lit1(const char* shortopts, const char* longopts, const char* glossary) {
    return arg_litn(shortopts, longopts, 1, 1, glossary);
}

struct arg_lit* arg_litn(const char* shortopts, const char* longopts, int mincount, int maxcount, const char* glossary) {
    struct arg_lit* result;

    /* foolproof things by ensuring maxcount is not less than mincount */
    maxcount = (maxcount < mincount) ? mincount : maxcount;

    result = (struct arg_lit*)xmalloc(sizeof(struct arg_lit));

    /* init the arg_hdr struct */
    result->hdr.flag = 0;
    result->hdr.shortopts = shortopts;
    result->hdr.longopts = longopts;
    result->hdr.datatype = NULL;
    result->hdr.glossary = glossary;
    result->hdr.mincount = mincount;
    result->hdr.maxcount = maxcount;
    result->hdr.parent = result;
    result->hdr.resetfn = (arg_resetfn*)arg_lit_resetfn;
    result->hdr.scanfn = (arg_scanfn*)arg_lit_scanfn;
    result->hdr.checkfn = (arg_checkfn*)arg_lit_checkfn;
    result->hdr.errorfn = (arg_errorfn*)arg_lit_errorfn;

    /* init local variables */
    result->count = 0;

    ARG_TRACE(("arg_litn() returns %p\n", result));
    return result;
}
