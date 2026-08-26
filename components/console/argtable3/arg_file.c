/*
 * SPDX-FileCopyrightText: 1998-2001,2003-2011,2013 Stewart Heitmann
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*******************************************************************************
 * arg_file: Implements the file command-line option
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
#include <string.h>

#ifdef WIN32
#define FILESEPARATOR1 '\\'
#define FILESEPARATOR2 '/'
#else
#define FILESEPARATOR1 '/'
#define FILESEPARATOR2 '/'
#endif

static void arg_file_resetfn(struct arg_file* parent) {
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}

/* Returns ptr to the base filename within *filename */
static const char* arg_basename(const char* filename) {
    const char *result = NULL, *result1, *result2;

    /* Find the last occurrence of eother file separator character. */
    /* Two alternative file separator chars are supported as legal  */
    /* file separators but not both together in the same filename.  */
    result1 = (filename ? strrchr(filename, FILESEPARATOR1) : NULL);
    result2 = (filename ? strrchr(filename, FILESEPARATOR2) : NULL);

    if (result2)
        result = result2 + 1; /* using FILESEPARATOR2 (the alternative file separator) */

    if (result1)
        result = result1 + 1; /* using FILESEPARATOR1 (the preferred file separator) */

    if (!result)
        result = filename; /* neither file separator was found so basename is the whole filename */

    /* special cases of "." and ".." are not considered basenames */
    if (result && (strcmp(".", result) == 0 || strcmp("..", result) == 0))
        result = filename + strlen(filename);

    return result;
}

/* Returns ptr to the file extension within *basename */
static const char* arg_extension(const char* basename) {
    /* find the last occurrence of '.' in basename */
    const char* result = (basename ? strrchr(basename, '.') : NULL);

    /* if no '.' was found then return pointer to end of basename */
    if (basename && !result)
        result = basename + strlen(basename);

    /* special case: basenames with a single leading dot (eg ".foo") are not considered as true extensions */
    if (basename && result == basename)
        result = basename + strlen(basename);

    /* special case: empty extensions (eg "foo.","foo..") are not considered as true extensions */
    if (basename && result && strlen(result) == 1)
        result = basename + strlen(basename);

    return result;
}

static int arg_file_scanfn(struct arg_file* parent, const char* argval) {
    int errorcode = 0;

    if (parent->count == parent->hdr.maxcount) {
        /* maximum number of arguments exceeded */
        errorcode = ARG_ERR_MAXCOUNT;
    } else if (!argval) {
        /* a valid argument with no argument value was given. */
        /* This happens when an optional argument value was invoked. */
        /* leave parent arguiment value unaltered but still count the argument. */
        parent->count++;
    } else {
        parent->filename[parent->count] = argval;
        parent->basename[parent->count] = arg_basename(argval);
        parent->extension[parent->count] =
                arg_extension(parent->basename[parent->count]); /* only seek extensions within the basename (not the file path)*/
        parent->count++;
    }

    ARG_TRACE(("%s4:scanfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}

static int arg_file_checkfn(struct arg_file* parent) {
    int errorcode = (parent->count < parent->hdr.mincount) ? ARG_ERR_MINCOUNT : 0;

    ARG_TRACE(("%s:checkfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}

static void arg_file_errorfn(struct arg_file* parent, arg_dstr_t ds, int errorcode, const char* argval, const char* progname) {
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

        default:
            arg_dstr_catf(ds, "unknown error at \"%s\"\n", argval);
    }
}

struct arg_file* arg_file0(const char* shortopts, const char* longopts, const char* datatype, const char* glossary) {
    return arg_filen(shortopts, longopts, datatype, 0, 1, glossary);
}

struct arg_file* arg_file1(const char* shortopts, const char* longopts, const char* datatype, const char* glossary) {
    return arg_filen(shortopts, longopts, datatype, 1, 1, glossary);
}

struct arg_file* arg_filen(const char* shortopts, const char* longopts, const char* datatype, int mincount, int maxcount, const char* glossary) {
    size_t nbytes;
    struct arg_file* result;
    int i;

    /* foolproof things by ensuring maxcount is not less than mincount */
    maxcount = (maxcount < mincount) ? mincount : maxcount;

    nbytes = sizeof(struct arg_file)     /* storage for struct arg_file */
             + sizeof(char*) * (size_t)maxcount  /* storage for filename[maxcount] array */
             + sizeof(char*) * (size_t)maxcount  /* storage for basename[maxcount] array */
             + sizeof(char*) * (size_t)maxcount; /* storage for extension[maxcount] array */

    result = (struct arg_file*)xmalloc(nbytes);

    /* init the arg_hdr struct */
    result->hdr.flag = ARG_HASVALUE;
    result->hdr.shortopts = shortopts;
    result->hdr.longopts = longopts;
    result->hdr.glossary = glossary;
    result->hdr.datatype = datatype ? datatype : "<file>";
    result->hdr.mincount = mincount;
    result->hdr.maxcount = maxcount;
    result->hdr.parent = result;
    result->hdr.resetfn = (arg_resetfn*)arg_file_resetfn;
    result->hdr.scanfn = (arg_scanfn*)arg_file_scanfn;
    result->hdr.checkfn = (arg_checkfn*)arg_file_checkfn;
    result->hdr.errorfn = (arg_errorfn*)arg_file_errorfn;

    /* store the filename,basename,extension arrays immediately after the arg_file struct */
    result->filename = (const char**)(result + 1);
    result->basename = result->filename + maxcount;
    result->extension = result->basename + maxcount;
    result->count = 0;

    /* foolproof the string pointers by initialising them with empty strings */
    for (i = 0; i < maxcount; i++) {
        result->filename[i] = "";
        result->basename[i] = "";
        result->extension[i] = "";
    }

    ARG_TRACE(("arg_filen() returns %p\n", result));
    return result;
}
