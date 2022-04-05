/*
 * SPDX-FileCopyrightText: 1998-2001,2003-2011,2013 Stewart Heitmann
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*******************************************************************************
 * arg_end: Implements the error handling utilities
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

static void arg_end_resetfn(struct arg_end* parent) {
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}

static void arg_end_errorfn(void* parent, arg_dstr_t ds, int error, const char* argval, const char* progname) {
    /* suppress unreferenced formal parameter warning */
    (void)parent;

    progname = progname ? progname : "";
    argval = argval ? argval : "";

    arg_dstr_catf(ds, "%s: ", progname);
    switch (error) {
        case ARG_ELIMIT:
            arg_dstr_cat(ds, "too many errors to display");
            break;
        case ARG_EMALLOC:
            arg_dstr_cat(ds, "insufficient memory");
            break;
        case ARG_ENOMATCH:
            arg_dstr_catf(ds, "unexpected argument \"%s\"", argval);
            break;
        case ARG_EMISSARG:
            arg_dstr_catf(ds, "option \"%s\" requires an argument", argval);
            break;
        case ARG_ELONGOPT:
            arg_dstr_catf(ds, "invalid option \"%s\"", argval);
            break;
        default:
            arg_dstr_catf(ds, "invalid option \"-%c\"", error);
            break;
    }

    arg_dstr_cat(ds, "\n");
}

struct arg_end* arg_end(int maxcount) {
    size_t nbytes;
    struct arg_end* result;

    nbytes = sizeof(struct arg_end) + (size_t)maxcount * sizeof(int) /* storage for int error[maxcount] array*/
             + (size_t)maxcount * sizeof(void*)                      /* storage for void* parent[maxcount] array */
             + (size_t)maxcount * sizeof(char*);                     /* storage for char* argval[maxcount] array */

    result = (struct arg_end*)xmalloc(nbytes);

    /* init the arg_hdr struct */
    result->hdr.flag = ARG_TERMINATOR;
    result->hdr.shortopts = NULL;
    result->hdr.longopts = NULL;
    result->hdr.datatype = NULL;
    result->hdr.glossary = NULL;
    result->hdr.mincount = 1;
    result->hdr.maxcount = maxcount;
    result->hdr.parent = result;
    result->hdr.resetfn = (arg_resetfn*)arg_end_resetfn;
    result->hdr.scanfn = NULL;
    result->hdr.checkfn = NULL;
    result->hdr.errorfn = (arg_errorfn*)arg_end_errorfn;

    /* store error[maxcount] array immediately after struct arg_end */
    result->error = (int*)(result + 1);

    /* store parent[maxcount] array immediately after error[] array */
    result->parent = (void**)(result->error + maxcount);

    /* store argval[maxcount] array immediately after parent[] array */
    result->argval = (const char**)(result->parent + maxcount);

    ARG_TRACE(("arg_end(%d) returns %p\n", maxcount, result));
    return result;
}

void arg_print_errors_ds(arg_dstr_t ds, struct arg_end* end, const char* progname) {
    int i;
    ARG_TRACE(("arg_errors()\n"));
    for (i = 0; i < end->count; i++) {
        struct arg_hdr* errorparent = (struct arg_hdr*)(end->parent[i]);
        if (errorparent->errorfn)
            errorparent->errorfn(end->parent[i], ds, end->error[i], end->argval[i], progname);
    }
}

void arg_print_errors(FILE* fp, struct arg_end* end, const char* progname) {
    arg_dstr_t ds = arg_dstr_create();
    arg_print_errors_ds(ds, end, progname);
    fputs(arg_dstr_cstr(ds), fp);
    arg_dstr_destroy(ds);
}
