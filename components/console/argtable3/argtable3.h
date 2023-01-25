/*
 * SPDX-FileCopyrightText: 1998-2001,2003-2011,2013 Stewart Heitmann
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*******************************************************************************
 * argtable3: Declares the main interfaces of the library
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

#ifndef ARGTABLE3
#define ARGTABLE3

#include <stdio.h> /* FILE */
#include <time.h>  /* struct tm */

#ifdef __cplusplus
extern "C" {
#endif

#define ARG_REX_ICASE 1
#define ARG_DSTR_SIZE 200
#define ARG_CMD_NAME_LEN 100
#define ARG_CMD_DESCRIPTION_LEN 256

#ifndef ARG_REPLACE_GETOPT
#define ARG_REPLACE_GETOPT 0 /* ESP-IDF-specific: use newlib-provided getopt instead of the embedded one */
#endif /* ARG_REPLACE_GETOPT */

/* bit masks for arg_hdr.flag */
enum { ARG_TERMINATOR = 0x1, ARG_HASVALUE = 0x2, ARG_HASOPTVALUE = 0x4 };

#if defined(_WIN32)
  #if defined(argtable3_EXPORTS)
    #define ARG_EXTERN __declspec(dllexport)
  #elif defined(argtable3_IMPORTS)
    #define ARG_EXTERN __declspec(dllimport)
  #else
    #define ARG_EXTERN
  #endif
#else
  #define ARG_EXTERN
#endif

typedef struct _internal_arg_dstr* arg_dstr_t;
typedef void* arg_cmd_itr_t;

typedef void(arg_resetfn)(void* parent);
typedef int(arg_scanfn)(void* parent, const char* argval);
typedef int(arg_checkfn)(void* parent);
typedef void(arg_errorfn)(void* parent, arg_dstr_t ds, int error, const char* argval, const char* progname);
typedef void(arg_dstr_freefn)(char* buf);
typedef int(arg_cmdfn)(int argc, char* argv[], arg_dstr_t res);
typedef int(arg_comparefn)(const void* k1, const void* k2);

/*
 * The arg_hdr struct defines properties that are common to all arg_xxx structs.
 * The argtable library requires each arg_xxx struct to have an arg_hdr
 * struct as its first data member.
 * The argtable library functions then use this data to identify the
 * properties of the command line option, such as its option tags,
 * datatype string, and glossary strings, and so on.
 * Moreover, the arg_hdr struct contains pointers to custom functions that
 * are provided by each arg_xxx struct which perform the tasks of parsing
 * that particular arg_xxx arguments, performing post-parse checks, and
 * reporting errors.
 * These functions are private to the individual arg_xxx source code
 * and are the pointer to them are initiliased by that arg_xxx struct's
 * constructor function. The user could alter them after construction
 * if desired, but the original intention is for them to be set by the
 * constructor and left unaltered.
 */
typedef struct arg_hdr {
    char flag;             /* Modifier flags: ARG_TERMINATOR, ARG_HASVALUE. */
    const char* shortopts; /* String defining the short options */
    const char* longopts;  /* String defiing the long options */
    const char* datatype;  /* Description of the argument data type */
    const char* glossary;  /* Description of the option as shown by arg_print_glossary function */
    int mincount;          /* Minimum number of occurences of this option accepted */
    int maxcount;          /* Maximum number of occurences if this option accepted */
    void* parent;          /* Pointer to parent arg_xxx struct */
    arg_resetfn* resetfn;  /* Pointer to parent arg_xxx reset function */
    arg_scanfn* scanfn;    /* Pointer to parent arg_xxx scan function */
    arg_checkfn* checkfn;  /* Pointer to parent arg_xxx check function */
    arg_errorfn* errorfn;  /* Pointer to parent arg_xxx error function */
    void* priv;            /* Pointer to private header data for use by arg_xxx functions */
} arg_hdr_t;

typedef struct arg_rem {
    struct arg_hdr hdr; /* The mandatory argtable header struct */
} arg_rem_t;

typedef struct arg_lit {
    struct arg_hdr hdr; /* The mandatory argtable header struct */
    int count;          /* Number of matching command line args */
} arg_lit_t;

typedef struct arg_int {
    struct arg_hdr hdr; /* The mandatory argtable header struct */
    int count;          /* Number of matching command line args */
    int* ival;          /* Array of parsed argument values */
} arg_int_t;

typedef struct arg_dbl {
    struct arg_hdr hdr; /* The mandatory argtable header struct */
    int count;          /* Number of matching command line args */
    double* dval;       /* Array of parsed argument values */
} arg_dbl_t;

typedef struct arg_str {
    struct arg_hdr hdr; /* The mandatory argtable header struct */
    int count;          /* Number of matching command line args */
    const char** sval;  /* Array of parsed argument values */
} arg_str_t;

typedef struct arg_rex {
    struct arg_hdr hdr; /* The mandatory argtable header struct */
    int count;          /* Number of matching command line args */
    const char** sval;  /* Array of parsed argument values */
} arg_rex_t;

typedef struct arg_file {
    struct arg_hdr hdr;     /* The mandatory argtable header struct */
    int count;              /* Number of matching command line args*/
    const char** filename;  /* Array of parsed filenames  (eg: /home/foo.bar) */
    const char** basename;  /* Array of parsed basenames  (eg: foo.bar) */
    const char** extension; /* Array of parsed extensions (eg: .bar) */
} arg_file_t;

typedef struct arg_date {
    struct arg_hdr hdr; /* The mandatory argtable header struct */
    const char* format; /* strptime format string used to parse the date */
    int count;          /* Number of matching command line args */
    struct tm* tmval;   /* Array of parsed time values */
} arg_date_t;

enum { ARG_ELIMIT = 1, ARG_EMALLOC, ARG_ENOMATCH, ARG_ELONGOPT, ARG_EMISSARG };
typedef struct arg_end {
    struct arg_hdr hdr;  /* The mandatory argtable header struct */
    int count;           /* Number of errors encountered */
    int* error;          /* Array of error codes */
    void** parent;       /* Array of pointers to offending arg_xxx struct */
    const char** argval; /* Array of pointers to offending argv[] string */
} arg_end_t;

typedef struct arg_cmd_info {
    char name[ARG_CMD_NAME_LEN];
    char description[ARG_CMD_DESCRIPTION_LEN];
    arg_cmdfn* proc;
} arg_cmd_info_t;

/**** arg_xxx constructor functions *********************************/

ARG_EXTERN struct arg_rem* arg_rem(const char* datatype, const char* glossary);

ARG_EXTERN struct arg_lit* arg_lit0(const char* shortopts, const char* longopts, const char* glossary);
ARG_EXTERN struct arg_lit* arg_lit1(const char* shortopts, const char* longopts, const char* glossary);
ARG_EXTERN struct arg_lit* arg_litn(const char* shortopts, const char* longopts, int mincount, int maxcount, const char* glossary);

ARG_EXTERN struct arg_int* arg_int0(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
ARG_EXTERN struct arg_int* arg_int1(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
ARG_EXTERN struct arg_int* arg_intn(const char* shortopts, const char* longopts, const char* datatype, int mincount, int maxcount, const char* glossary);

ARG_EXTERN struct arg_dbl* arg_dbl0(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
ARG_EXTERN struct arg_dbl* arg_dbl1(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
ARG_EXTERN struct arg_dbl* arg_dbln(const char* shortopts, const char* longopts, const char* datatype, int mincount, int maxcount, const char* glossary);

ARG_EXTERN struct arg_str* arg_str0(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
ARG_EXTERN struct arg_str* arg_str1(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
ARG_EXTERN struct arg_str* arg_strn(const char* shortopts, const char* longopts, const char* datatype, int mincount, int maxcount, const char* glossary);

ARG_EXTERN struct arg_rex* arg_rex0(const char* shortopts, const char* longopts, const char* pattern, const char* datatype, int flags, const char* glossary);
ARG_EXTERN struct arg_rex* arg_rex1(const char* shortopts, const char* longopts, const char* pattern, const char* datatype, int flags, const char* glossary);
ARG_EXTERN struct arg_rex* arg_rexn(const char* shortopts,
                         const char* longopts,
                         const char* pattern,
                         const char* datatype,
                         int mincount,
                         int maxcount,
                         int flags,
                         const char* glossary);

ARG_EXTERN struct arg_file* arg_file0(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
ARG_EXTERN struct arg_file* arg_file1(const char* shortopts, const char* longopts, const char* datatype, const char* glossary);
ARG_EXTERN struct arg_file* arg_filen(const char* shortopts, const char* longopts, const char* datatype, int mincount, int maxcount, const char* glossary);

ARG_EXTERN struct arg_date* arg_date0(const char* shortopts, const char* longopts, const char* format, const char* datatype, const char* glossary);
ARG_EXTERN struct arg_date* arg_date1(const char* shortopts, const char* longopts, const char* format, const char* datatype, const char* glossary);
ARG_EXTERN struct arg_date* arg_daten(const char* shortopts, const char* longopts, const char* format, const char* datatype, int mincount, int maxcount, const char* glossary);

ARG_EXTERN struct arg_end* arg_end(int maxcount);

#define ARG_DSTR_STATIC ((arg_dstr_freefn*)0)
#define ARG_DSTR_VOLATILE ((arg_dstr_freefn*)1)
#define ARG_DSTR_DYNAMIC ((arg_dstr_freefn*)3)

/**** other functions *******************************************/
ARG_EXTERN int arg_nullcheck(void** argtable);
ARG_EXTERN int arg_parse(int argc, char** argv, void** argtable);
ARG_EXTERN void arg_print_option(FILE* fp, const char* shortopts, const char* longopts, const char* datatype, const char* suffix);
ARG_EXTERN void arg_print_syntax(FILE* fp, void** argtable, const char* suffix);
ARG_EXTERN void arg_print_syntaxv(FILE* fp, void** argtable, const char* suffix);
ARG_EXTERN void arg_print_glossary(FILE* fp, void** argtable, const char* format);
ARG_EXTERN void arg_print_glossary_gnu(FILE* fp, void** argtable);
ARG_EXTERN void arg_print_formatted(FILE *fp, const unsigned lmargin, const unsigned rmargin, const char *text);
ARG_EXTERN void arg_print_errors(FILE* fp, struct arg_end* end, const char* progname);
ARG_EXTERN void arg_print_option_ds(arg_dstr_t ds, const char* shortopts, const char* longopts, const char* datatype, const char* suffix);
ARG_EXTERN void arg_print_syntax_ds(arg_dstr_t ds, void** argtable, const char* suffix);
ARG_EXTERN void arg_print_syntaxv_ds(arg_dstr_t ds, void** argtable, const char* suffix);
ARG_EXTERN void arg_print_glossary_ds(arg_dstr_t ds, void** argtable, const char* format);
ARG_EXTERN void arg_print_glossary_gnu_ds(arg_dstr_t ds, void** argtable);
ARG_EXTERN void arg_print_errors_ds(arg_dstr_t ds, struct arg_end* end, const char* progname);
ARG_EXTERN void arg_freetable(void** argtable, size_t n);

ARG_EXTERN arg_dstr_t arg_dstr_create(void);
ARG_EXTERN void arg_dstr_destroy(arg_dstr_t ds);
ARG_EXTERN void arg_dstr_reset(arg_dstr_t ds);
ARG_EXTERN void arg_dstr_free(arg_dstr_t ds);
ARG_EXTERN void arg_dstr_set(arg_dstr_t ds, char* str, arg_dstr_freefn* free_proc);
ARG_EXTERN void arg_dstr_cat(arg_dstr_t ds, const char* str);
ARG_EXTERN void arg_dstr_catc(arg_dstr_t ds, char c);
ARG_EXTERN void arg_dstr_catf(arg_dstr_t ds, const char* fmt, ...);
ARG_EXTERN char* arg_dstr_cstr(arg_dstr_t ds);

ARG_EXTERN void arg_cmd_init(void);
ARG_EXTERN void arg_cmd_uninit(void);
ARG_EXTERN void arg_cmd_register(const char* name, arg_cmdfn* proc, const char* description);
ARG_EXTERN void arg_cmd_unregister(const char* name);
ARG_EXTERN int arg_cmd_dispatch(const char* name, int argc, char* argv[], arg_dstr_t res);
ARG_EXTERN unsigned int arg_cmd_count(void);
ARG_EXTERN arg_cmd_info_t* arg_cmd_info(const char* name);
ARG_EXTERN arg_cmd_itr_t arg_cmd_itr_create(void);
ARG_EXTERN void arg_cmd_itr_destroy(arg_cmd_itr_t itr);
ARG_EXTERN int arg_cmd_itr_advance(arg_cmd_itr_t itr);
ARG_EXTERN char* arg_cmd_itr_key(arg_cmd_itr_t itr);
ARG_EXTERN arg_cmd_info_t* arg_cmd_itr_value(arg_cmd_itr_t itr);
ARG_EXTERN int arg_cmd_itr_search(arg_cmd_itr_t itr, void* k);
ARG_EXTERN void arg_mgsort(void* data, int size, int esize, int i, int k, arg_comparefn* comparefn);
ARG_EXTERN void arg_make_get_help_msg(arg_dstr_t res);
ARG_EXTERN void arg_make_help_msg(arg_dstr_t ds, char* cmd_name, void** argtable);
ARG_EXTERN void arg_make_syntax_err_msg(arg_dstr_t ds, void** argtable, struct arg_end* end);
ARG_EXTERN int arg_make_syntax_err_help_msg(arg_dstr_t ds, char* name, int help, int nerrors, void** argtable, struct arg_end* end, int* exitcode);
ARG_EXTERN void arg_set_module_name(const char* name);
ARG_EXTERN void arg_set_module_version(int major, int minor, int patch, const char* tag);

/**** deprecated functions, for back-compatibility only ********/
ARG_EXTERN void arg_free(void** argtable);

#ifdef __cplusplus
}
#endif
#endif
