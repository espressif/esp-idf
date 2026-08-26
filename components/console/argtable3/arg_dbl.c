/*
 * SPDX-FileCopyrightText: 1998-2001,2003-2011,2013 Stewart Heitmann
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*******************************************************************************
 * arg_dbl: Implements the double command-line option
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

static void arg_dbl_resetfn(struct arg_dbl* parent) {
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}

static int arg_dbl_scanfn(struct arg_dbl* parent, const char* argval) {
    int errorcode = 0;

    if (parent->count == parent->hdr.maxcount) {
        /* maximum number of arguments exceeded */
        errorcode = ARG_ERR_MAXCOUNT;
    } else if (!argval) {
        /* a valid argument with no argument value was given. */
        /* This happens when an optional argument value was invoked. */
        /* leave parent argument value unaltered but still count the argument. */
        parent->count++;
    } else {
        double val;
        char* end;

        /* extract double from argval into val */
        val = strtod(argval, &end);

        /* if success then store result in parent->dval[] array otherwise return error*/
        if (*end == 0)
            parent->dval[parent->count++] = val;
        else
            errorcode = ARG_ERR_BADDOUBLE;
    }

    ARG_TRACE(("%s:scanfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}

static int arg_dbl_checkfn(struct arg_dbl* parent) {
    int errorcode = (parent->count < parent->hdr.mincount) ? ARG_ERR_MINCOUNT : 0;

    ARG_TRACE(("%s:checkfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}

static void arg_dbl_errorfn(struct arg_dbl* parent, arg_dstr_t ds, int errorcode, const char* argval, const char* progname) {
    const char* shortopts = parent->hdr.shortopts;
    const char* longopts = parent->hdr.longopts;
    const char* datatype = parent->hdr.datatype;

    /* make argval NULL safe */
    argval = argval ? argval : "";

    arg_dstr_catf(ds, "%s: ", progname);
    switch (errorcode) {
        case ARG_ERR_MINCOUNT:
            arg_dstr_cat(ds, "missing option ");
            arg_print_option_ds(ds, shortopts, longopts, datatype, "\n");
            break;

        case ARG_ERR_MAXCOUNT:
            arg_dstr_cat(ds, "excess option ");
            arg_print_option_ds(ds, shortopts, longopts, argval, "\n");
            break;

        case ARG_ERR_BADDOUBLE:
            arg_dstr_catf(ds, "invalid argument \"%s\" to option ", argval);
            arg_print_option_ds(ds, shortopts, longopts, datatype, "\n");
            break;
    }
}

struct arg_dbl* arg_dbl0(const char* shortopts, const char* longopts, const char* datatype, const char* glossary) {
    return arg_dbln(shortopts, longopts, datatype, 0, 1, glossary);
}

struct arg_dbl* arg_dbl1(const char* shortopts, const char* longopts, const char* datatype, const char* glossary) {
    return arg_dbln(shortopts, longopts, datatype, 1, 1, glossary);
}

struct arg_dbl* arg_dbln(const char* shortopts, const char* longopts, const char* datatype, int mincount, int maxcount, const char* glossary) {
    size_t nbytes;
    struct arg_dbl* result;
    size_t addr;
    size_t rem;

    /* foolproof things by ensuring maxcount is not less than mincount */
    maxcount = (maxcount < mincount) ? mincount : maxcount;

    nbytes = sizeof(struct arg_dbl)             /* storage for struct arg_dbl */
             + (size_t)(maxcount + 1) * sizeof(double); /* storage for dval[maxcount] array plus one extra for padding to memory boundary */

    result = (struct arg_dbl*)xmalloc(nbytes);

    /* init the arg_hdr struct */
    result->hdr.flag = ARG_HASVALUE;
    result->hdr.shortopts = shortopts;
    result->hdr.longopts = longopts;
    result->hdr.datatype = datatype ? datatype : "<double>";
    result->hdr.glossary = glossary;
    result->hdr.mincount = mincount;
    result->hdr.maxcount = maxcount;
    result->hdr.parent = result;
    result->hdr.resetfn = (arg_resetfn*)arg_dbl_resetfn;
    result->hdr.scanfn = (arg_scanfn*)arg_dbl_scanfn;
    result->hdr.checkfn = (arg_checkfn*)arg_dbl_checkfn;
    result->hdr.errorfn = (arg_errorfn*)arg_dbl_errorfn;

    /* Store the dval[maxcount] array on the first double boundary that
     * immediately follows the arg_dbl struct. We do the memory alignment
     * purely for SPARC and Motorola systems. They require floats and
     * doubles to be aligned on natural boundaries.
     */
    addr = (size_t)(result + 1);
    rem = addr % sizeof(double);
    result->dval = (double*)(addr + sizeof(double) - rem);
    ARG_TRACE(("addr=%p, dval=%p, sizeof(double)=%d rem=%d\n", addr, result->dval, (int)sizeof(double), (int)rem));

    result->count = 0;

    ARG_TRACE(("arg_dbln() returns %p\n", result));
    return result;
}
