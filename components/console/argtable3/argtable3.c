/*
 * SPDX-FileCopyrightText: 1998-2001,2003-2011,2013 Stewart Heitmann
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*******************************************************************************
 * argtable3: Implements the main interfaces of the library
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
#if ARG_REPLACE_GETOPT == 1
#include "arg_getopt.h"
#else
#include <getopt.h>
#endif
#else
#if ARG_REPLACE_GETOPT == 0
#include <getopt.h>
#endif
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

static void arg_register_error(struct arg_end* end, void* parent, int error, const char* argval) {
    /* printf("arg_register_error(%p,%p,%d,%s)\n",end,parent,error,argval); */
    if (end->count < end->hdr.maxcount) {
        end->error[end->count] = error;
        end->parent[end->count] = parent;
        end->argval[end->count] = argval;
        end->count++;
    } else {
        end->error[end->hdr.maxcount - 1] = ARG_ELIMIT;
        end->parent[end->hdr.maxcount - 1] = end;
        end->argval[end->hdr.maxcount - 1] = NULL;
    }
}

/*
 * Return index of first table entry with a matching short option
 * or -1 if no match was found.
 */
static int find_shortoption(struct arg_hdr** table, char shortopt) {
    int tabindex;
    for (tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
        if (table[tabindex]->shortopts && strchr(table[tabindex]->shortopts, shortopt))
            return tabindex;
    }
    return -1;
}

struct longoptions {
    int getoptval;
    int noptions;
    struct option* options;
};

#if 0
static
void dump_longoptions(struct longoptions * longoptions)
{
    int i;
    printf("getoptval = %d\n", longoptions->getoptval);
    printf("noptions  = %d\n", longoptions->noptions);
    for (i = 0; i < longoptions->noptions; i++)
    {
        printf("options[%d].name    = \"%s\"\n",
               i,
               longoptions->options[i].name);
        printf("options[%d].has_arg = %d\n", i, longoptions->options[i].has_arg);
        printf("options[%d].flag    = %p\n", i, longoptions->options[i].flag);
        printf("options[%d].val     = %d\n", i, longoptions->options[i].val);
    }
}
#endif

static struct longoptions* alloc_longoptions(struct arg_hdr** table) {
    struct longoptions* result;
    size_t nbytes;
    int noptions = 1;
    size_t longoptlen = 0;
    int tabindex;
    int option_index = 0;
    char* store;

    /*
     * Determine the total number of option structs required
     * by counting the number of comma separated long options
     * in all table entries and return the count in noptions.
     * note: noptions starts at 1 not 0 because we getoptlong
     * requires a NULL option entry to terminate the option array.
     * While we are at it, count the number of chars required
     * to store private copies of all the longoption strings
     * and return that count in logoptlen.
     */
    tabindex = 0;
    do {
        const char* longopts = table[tabindex]->longopts;
        longoptlen += (longopts ? strlen(longopts) : 0) + 1;
        while (longopts) {
            noptions++;
            longopts = strchr(longopts + 1, ',');
        }
    } while (!(table[tabindex++]->flag & ARG_TERMINATOR));
    /*printf("%d long options consuming %d chars in total\n",noptions,longoptlen);*/

    /* allocate storage for return data structure as: */
    /* (struct longoptions) + (struct options)[noptions] + char[longoptlen] */
    nbytes = sizeof(struct longoptions) + sizeof(struct option) * (size_t)noptions + longoptlen;
    result = (struct longoptions*)xmalloc(nbytes);

    result->getoptval = 0;
    result->noptions = noptions;
    result->options = (struct option*)(result + 1);
    store = (char*)(result->options + noptions);

    for (tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
        const char* longopts = table[tabindex]->longopts;

        while (longopts && *longopts) {
            char* storestart = store;

            /* copy progressive longopt strings into the store */
            while (*longopts != 0 && *longopts != ',')
                *store++ = *longopts++;
            *store++ = 0;
            if (*longopts == ',')
                longopts++;
            /*fprintf(stderr,"storestart=\"%s\"\n",storestart);*/

            result->options[option_index].name = storestart;
            result->options[option_index].flag = &(result->getoptval);
            result->options[option_index].val = tabindex;
            if (table[tabindex]->flag & ARG_HASOPTVALUE)
                result->options[option_index].has_arg = 2;
            else if (table[tabindex]->flag & ARG_HASVALUE)
                result->options[option_index].has_arg = 1;
            else
                result->options[option_index].has_arg = 0;

            option_index++;
        }
    }
    /* terminate the options array with a zero-filled entry */
    result->options[option_index].name = 0;
    result->options[option_index].has_arg = 0;
    result->options[option_index].flag = 0;
    result->options[option_index].val = 0;

    /*dump_longoptions(result);*/
    return result;
}

static char* alloc_shortoptions(struct arg_hdr** table) {
    char* result;
    size_t len = 2;
    int tabindex;
    char* res;

    /* determine the total number of option chars required */
    for (tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
        struct arg_hdr* hdr = table[tabindex];
        len += 3 * (hdr->shortopts ? strlen(hdr->shortopts) : 0);
    }

    result = xmalloc(len);

    res = result;

    /* add a leading ':' so getopt return codes distinguish    */
    /* unrecognised option and options missing argument values */
    *res++ = ':';

    for (tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
        struct arg_hdr* hdr = table[tabindex];
        const char* shortopts = hdr->shortopts;
        while (shortopts && *shortopts) {
            *res++ = *shortopts++;
            if (hdr->flag & ARG_HASVALUE)
                *res++ = ':';
            if (hdr->flag & ARG_HASOPTVALUE)
                *res++ = ':';
        }
    }
    /* null terminate the string */
    *res = 0;

    /*printf("alloc_shortoptions() returns \"%s\"\n",(result?result:"NULL"));*/
    return result;
}

/* return index of the table terminator entry */
static int arg_endindex(struct arg_hdr** table) {
    int tabindex = 0;
    while (!(table[tabindex]->flag & ARG_TERMINATOR))
        tabindex++;
    return tabindex;
}

static void arg_parse_tagged(int argc, char** argv, struct arg_hdr** table, struct arg_end* endtable) {
    struct longoptions* longoptions;
    char* shortoptions;
    int copt;

    /*printf("arg_parse_tagged(%d,%p,%p,%p)\n",argc,argv,table,endtable);*/

    /* allocate short and long option arrays for the given opttable[].   */
    /* if the allocs fail then put an error msg in the last table entry. */
    longoptions = alloc_longoptions(table);
    shortoptions = alloc_shortoptions(table);

    /*dump_longoptions(longoptions);*/

    /* reset getopts internal option-index to zero, and disable error reporting */
    optind = 0;
    opterr = 0;

    /* fetch and process args using getopt_long */
#ifdef ARG_LONG_ONLY
    while ((copt = getopt_long_only(argc, argv, shortoptions, longoptions->options, NULL)) != -1) {
#else
    while ((copt = getopt_long(argc, argv, shortoptions, longoptions->options, NULL)) != -1) {
#endif
        /*
           printf("optarg='%s'\n",optarg);
           printf("optind=%d\n",optind);
           printf("copt=%c\n",(char)copt);
           printf("optopt=%c (%d)\n",optopt, (int)(optopt));
         */
        switch (copt) {
            case 0: {
                int tabindex = longoptions->getoptval;
                void* parent = table[tabindex]->parent;
                /*printf("long option detected from argtable[%d]\n", tabindex);*/
                if (optarg && optarg[0] == 0 && (table[tabindex]->flag & ARG_HASVALUE)) {
                    /* printf(": long option %s requires an argument\n",argv[optind-1]); */
                    arg_register_error(endtable, endtable, ARG_EMISSARG, argv[optind - 1]);
                    /* continue to scan the (empty) argument value to enforce argument count checking */
                }
                if (table[tabindex]->scanfn) {
                    int errorcode = table[tabindex]->scanfn(parent, optarg);
                    if (errorcode != 0)
                        arg_register_error(endtable, parent, errorcode, optarg);
                }
            } break;

            case '?':
                /*
                 * getopt_long() found an unrecognised short option.
                 * if it was a short option its value is in optopt
                 * if it was a long option then optopt=0
                 */
                switch (optopt) {
                    case 0:
                        /*printf("?0 unrecognised long option %s\n",argv[optind-1]);*/
                        arg_register_error(endtable, endtable, ARG_ELONGOPT, argv[optind - 1]);
                        break;
                    default:
                        /*printf("?* unrecognised short option '%c'\n",optopt);*/
                        arg_register_error(endtable, endtable, optopt, NULL);
                        break;
                }
                break;

            case ':':
                /*
                 * getopt_long() found an option with its argument missing.
                 */
                /*printf(": option %s requires an argument\n",argv[optind-1]); */
                arg_register_error(endtable, endtable, ARG_EMISSARG, argv[optind - 1]);
                break;

            default: {
                /* getopt_long() found a valid short option */
                int tabindex = find_shortoption(table, (char)copt);
                /*printf("short option detected from argtable[%d]\n", tabindex);*/
                if (tabindex == -1) {
                    /* should never get here - but handle it just in case */
                    /*printf("unrecognised short option %d\n",copt);*/
                    arg_register_error(endtable, endtable, copt, NULL);
                } else {
                    if (table[tabindex]->scanfn) {
                        void* parent = table[tabindex]->parent;
                        int errorcode = table[tabindex]->scanfn(parent, optarg);
                        if (errorcode != 0)
                            arg_register_error(endtable, parent, errorcode, optarg);
                    }
                }
                break;
            }
        }
    }

    xfree(shortoptions);
    xfree(longoptions);
}

static void arg_parse_untagged(int argc, char** argv, struct arg_hdr** table, struct arg_end* endtable) {
    int tabindex = 0;
    int errorlast = 0;
    const char* optarglast = NULL;
    void* parentlast = NULL;

    /*printf("arg_parse_untagged(%d,%p,%p,%p)\n",argc,argv,table,endtable);*/
    while (!(table[tabindex]->flag & ARG_TERMINATOR)) {
        void* parent;
        int errorcode;

        /* if we have exhausted our argv[optind] entries then we have finished */
        if (optind >= argc) {
            /*printf("arg_parse_untagged(): argv[] exhausted\n");*/
            return;
        }

        /* skip table entries with non-null long or short options (they are not untagged entries) */
        if (table[tabindex]->longopts || table[tabindex]->shortopts) {
            /*printf("arg_parse_untagged(): skipping argtable[%d] (tagged argument)\n",tabindex);*/
            tabindex++;
            continue;
        }

        /* skip table entries with NULL scanfn */
        if (!(table[tabindex]->scanfn)) {
            /*printf("arg_parse_untagged(): skipping argtable[%d] (NULL scanfn)\n",tabindex);*/
            tabindex++;
            continue;
        }

        /* attempt to scan the current argv[optind] with the current     */
        /* table[tabindex] entry. If it succeeds then keep it, otherwise */
        /* try again with the next table[] entry.                        */
        parent = table[tabindex]->parent;
        errorcode = table[tabindex]->scanfn(parent, argv[optind]);
        if (errorcode == 0) {
            /* success, move onto next argv[optind] but stay with same table[tabindex] */
            /*printf("arg_parse_untagged(): argtable[%d] successfully matched\n",tabindex);*/
            optind++;

            /* clear the last tentative error */
            errorlast = 0;
        } else {
            /* failure, try same argv[optind] with next table[tabindex] entry */
            /*printf("arg_parse_untagged(): argtable[%d] failed match\n",tabindex);*/
            tabindex++;

            /* remember this as a tentative error we may wish to reinstate later */
            errorlast = errorcode;
            optarglast = argv[optind];
            parentlast = parent;
        }
    }

    /* if a tenative error still remains at this point then register it as a proper error */
    if (errorlast) {
        arg_register_error(endtable, parentlast, errorlast, optarglast);
        optind++;
    }

    /* only get here when not all argv[] entries were consumed */
    /* register an error for each unused argv[] entry */
    while (optind < argc) {
        /*printf("arg_parse_untagged(): argv[%d]=\"%s\" not consumed\n",optind,argv[optind]);*/
        arg_register_error(endtable, endtable, ARG_ENOMATCH, argv[optind++]);
    }

    return;
}

static void arg_parse_check(struct arg_hdr** table, struct arg_end* endtable) {
    int tabindex = 0;
    /* printf("arg_parse_check()\n"); */
    do {
        if (table[tabindex]->checkfn) {
            void* parent = table[tabindex]->parent;
            int errorcode = table[tabindex]->checkfn(parent);
            if (errorcode != 0)
                arg_register_error(endtable, parent, errorcode, NULL);
        }
    } while (!(table[tabindex++]->flag & ARG_TERMINATOR));
}

static void arg_reset(void** argtable) {
    struct arg_hdr** table = (struct arg_hdr**)argtable;
    int tabindex = 0;
    /*printf("arg_reset(%p)\n",argtable);*/
    do {
        if (table[tabindex]->resetfn)
            table[tabindex]->resetfn(table[tabindex]->parent);
    } while (!(table[tabindex++]->flag & ARG_TERMINATOR));
}

int arg_parse(int argc, char** argv, void** argtable) {
    struct arg_hdr** table = (struct arg_hdr**)argtable;
    struct arg_end* endtable;
    int endindex;
    char** argvcopy = NULL;
    int i;

    /*printf("arg_parse(%d,%p,%p)\n",argc,argv,argtable);*/

    /* reset any argtable data from previous invocations */
    arg_reset(argtable);

    /* locate the first end-of-table marker within the array */
    endindex = arg_endindex(table);
    endtable = (struct arg_end*)table[endindex];

    /* Special case of argc==0.  This can occur on Texas Instruments DSP. */
    /* Failure to trap this case results in an unwanted NULL result from  */
    /* the malloc for argvcopy (next code block).                         */
    if (argc == 0) {
        /* We must still perform post-parse checks despite the absence of command line arguments */
        arg_parse_check(table, endtable);

        /* Now we are finished */
        return endtable->count;
    }

    argvcopy = (char**)xmalloc(sizeof(char*) * (size_t)(argc + 1));

    /*
        Fill in the local copy of argv[]. We need a local copy
        because getopt rearranges argv[] which adversely affects
        susbsequent parsing attempts.
        */
    for (i = 0; i < argc; i++)
        argvcopy[i] = argv[i];

    argvcopy[argc] = NULL;

    /* parse the command line (local copy) for tagged options */
    arg_parse_tagged(argc, argvcopy, table, endtable);

    /* parse the command line (local copy) for untagged options */
    arg_parse_untagged(argc, argvcopy, table, endtable);

    /* if no errors so far then perform post-parse checks otherwise dont bother */
    if (endtable->count == 0)
        arg_parse_check(table, endtable);

    /* release the local copt of argv[] */
    xfree(argvcopy);

    return endtable->count;
}

/*
 * Concatenate contents of src[] string onto *pdest[] string.
 * The *pdest pointer is altered to point to the end of the
 * target string and *pndest is decremented by the same number
 * of chars.
 * Does not append more than *pndest chars into *pdest[]
 * so as to prevent buffer overruns.
 * Its something like strncat() but more efficient for repeated
 * calls on the same destination string.
 * Example of use:
 *   char dest[30] = "good"
 *   size_t ndest = sizeof(dest);
 *   char *pdest = dest;
 *   arg_char(&pdest,"bye ",&ndest);
 *   arg_char(&pdest,"cruel ",&ndest);
 *   arg_char(&pdest,"world!",&ndest);
 * Results in:
 *   dest[] == "goodbye cruel world!"
 *   ndest  == 10
 */
static void arg_cat(char** pdest, const char* src, size_t* pndest) {
    char* dest = *pdest;
    char* end = dest + *pndest;

    /*locate null terminator of dest string */
    while (dest < end-1 && *dest != 0)
        dest++;

    /* concat src string to dest string */
    while (dest < end-1 && *src != 0)
        *dest++ = *src++;

    /* null terminate dest string */
    *dest = 0;

    /* update *pdest and *pndest */
    *pndest = (size_t)(end - dest);
    *pdest = dest;
}

static void arg_cat_option(char* dest, size_t ndest, const char* shortopts, const char* longopts, const char* datatype, int optvalue) {
    if (shortopts) {
        char option[3];

        /* note: option array[] is initialiazed dynamically here to satisfy   */
        /* a deficiency in the watcom compiler wrt static array initializers. */
        option[0] = '-';
        option[1] = shortopts[0];
        option[2] = 0;

        arg_cat(&dest, option, &ndest);
        if (datatype) {
            arg_cat(&dest, " ", &ndest);
            if (optvalue) {
                arg_cat(&dest, "[", &ndest);
                arg_cat(&dest, datatype, &ndest);
                arg_cat(&dest, "]", &ndest);
            } else
                arg_cat(&dest, datatype, &ndest);
        }
    } else if (longopts) {
        size_t ncspn;

        /* add "--" tag prefix */
        arg_cat(&dest, "--", &ndest);

        /* add comma separated option tag */
        ncspn = strcspn(longopts, ",");
#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__)) || (defined(__STDC_SECURE_LIB__) && defined(__STDC_WANT_SECURE_LIB__))
        strncat_s(dest, ndest, longopts, (ncspn < ndest) ? ncspn : ndest);
#else
        strncat(dest, longopts, (ncspn < ndest) ? ncspn : ndest);
#endif

        if (datatype) {
            arg_cat(&dest, "=", &ndest);
            if (optvalue) {
                arg_cat(&dest, "[", &ndest);
                arg_cat(&dest, datatype, &ndest);
                arg_cat(&dest, "]", &ndest);
            } else
                arg_cat(&dest, datatype, &ndest);
        }
    } else if (datatype) {
        if (optvalue) {
            arg_cat(&dest, "[", &ndest);
            arg_cat(&dest, datatype, &ndest);
            arg_cat(&dest, "]", &ndest);
        } else
            arg_cat(&dest, datatype, &ndest);
    }
}

static void arg_cat_optionv(char* dest, size_t ndest, const char* shortopts, const char* longopts, const char* datatype, int optvalue, const char* separator) {
    separator = separator ? separator : "";

    if (shortopts) {
        const char* c = shortopts;
        while (*c) {
            /* "-a|-b|-c" */
            char shortopt[3];

            /* note: shortopt array[] is initialiazed dynamically here to satisfy */
            /* a deficiency in the watcom compiler wrt static array initializers. */
            shortopt[0] = '-';
            shortopt[1] = *c;
            shortopt[2] = 0;

            arg_cat(&dest, shortopt, &ndest);
            if (*++c)
                arg_cat(&dest, separator, &ndest);
        }
    }

    /* put separator between long opts and short opts */
    if (shortopts && longopts)
        arg_cat(&dest, separator, &ndest);

    if (longopts) {
        const char* c = longopts;
        while (*c) {
            size_t ncspn;

            /* add "--" tag prefix */
            arg_cat(&dest, "--", &ndest);

            /* add comma separated option tag */
            ncspn = strcspn(c, ",");
#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__)) || (defined(__STDC_SECURE_LIB__) && defined(__STDC_WANT_SECURE_LIB__))
            strncat_s(dest, ndest, c, (ncspn < ndest) ? ncspn : ndest);
#else
            strncat(dest, c, (ncspn < ndest) ? ncspn : ndest);
#endif
            c += ncspn;

            /* add given separator in place of comma */
            if (*c == ',') {
                arg_cat(&dest, separator, &ndest);
                c++;
            }
        }
    }

    if (datatype) {
        if (longopts)
            arg_cat(&dest, "=", &ndest);
        else if (shortopts)
            arg_cat(&dest, " ", &ndest);

        if (optvalue) {
            arg_cat(&dest, "[", &ndest);
            arg_cat(&dest, datatype, &ndest);
            arg_cat(&dest, "]", &ndest);
        } else
            arg_cat(&dest, datatype, &ndest);
    }
}

void arg_print_option_ds(arg_dstr_t ds, const char* shortopts, const char* longopts, const char* datatype, const char* suffix) {
    char syntax[200] = "";
    suffix = suffix ? suffix : "";

    /* there is no way of passing the proper optvalue for optional argument values here, so we must ignore it */
    arg_cat_optionv(syntax, sizeof(syntax) - 1, shortopts, longopts, datatype, 0, "|");

    arg_dstr_cat(ds, syntax);
    arg_dstr_cat(ds, (char*)suffix);
}

/* this function should be deprecated because it doesn't consider optional argument values (ARG_HASOPTVALUE) */
void arg_print_option(FILE* fp, const char* shortopts, const char* longopts, const char* datatype, const char* suffix) {
    arg_dstr_t ds = arg_dstr_create();
    arg_print_option_ds(ds, shortopts, longopts, datatype, suffix);
    fputs(arg_dstr_cstr(ds), fp);
    arg_dstr_destroy(ds);
}

/*
 * Print a GNU style [OPTION] string in which all short options that
 * do not take argument values are presented in abbreviated form, as
 * in: -xvfsd, or -xvf[sd], or [-xvsfd]
 */
static void arg_print_gnuswitch_ds(arg_dstr_t ds, struct arg_hdr** table) {
    int tabindex;
    const char* format1 = " -%c";
    const char* format2 = " [-%c";
    const char* suffix = "";

    /* print all mandatory switches that are without argument values */
    for (tabindex = 0; table[tabindex] && !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
        /* skip optional options */
        if (table[tabindex]->mincount < 1)
            continue;

        /* skip non-short options */
        if (table[tabindex]->shortopts == NULL)
            continue;

        /* skip options that take argument values */
        if (table[tabindex]->flag & ARG_HASVALUE)
            continue;

        /* print the short option (only the first short option char, ignore multiple choices)*/
        arg_dstr_catf(ds, format1, table[tabindex]->shortopts[0]);
        format1 = "%c";
        format2 = "[%c";
    }

    /* print all optional switches that are without argument values */
    for (tabindex = 0; table[tabindex] && !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
        /* skip mandatory args */
        if (table[tabindex]->mincount > 0)
            continue;

        /* skip args without short options */
        if (table[tabindex]->shortopts == NULL)
            continue;

        /* skip args with values */
        if (table[tabindex]->flag & ARG_HASVALUE)
            continue;

        /* print first short option */
        arg_dstr_catf(ds, format2, table[tabindex]->shortopts[0]);
        format2 = "%c";
        suffix = "]";
    }

    arg_dstr_catf(ds, "%s", suffix);
}

void arg_print_syntax_ds(arg_dstr_t ds, void** argtable, const char* suffix) {
    struct arg_hdr** table = (struct arg_hdr**)argtable;
    int i, tabindex;

    /* print GNU style [OPTION] string */
    arg_print_gnuswitch_ds(ds, table);

    /* print remaining options in abbreviated style */
    for (tabindex = 0; table[tabindex] && !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
        char syntax[200] = "";
        const char *shortopts, *longopts, *datatype;

        /* skip short options without arg values (they were printed by arg_print_gnu_switch) */
        if (table[tabindex]->shortopts && !(table[tabindex]->flag & ARG_HASVALUE))
            continue;

        shortopts = table[tabindex]->shortopts;
        longopts = table[tabindex]->longopts;
        datatype = table[tabindex]->datatype;
        arg_cat_option(syntax, sizeof(syntax) - 1, shortopts, longopts, datatype, table[tabindex]->flag & ARG_HASOPTVALUE);

        if (strlen(syntax) > 0) {
            /* print mandatory instances of this option */
            for (i = 0; i < table[tabindex]->mincount; i++) {
                arg_dstr_cat(ds, " ");
                arg_dstr_cat(ds, syntax);
            }

            /* print optional instances enclosed in "[..]" */
            switch (table[tabindex]->maxcount - table[tabindex]->mincount) {
                case 0:
                    break;
                case 1:
                    arg_dstr_cat(ds, " [");
                    arg_dstr_cat(ds, syntax);
                    arg_dstr_cat(ds, "]");
                    break;
                case 2:
                    arg_dstr_cat(ds, " [");
                    arg_dstr_cat(ds, syntax);
                    arg_dstr_cat(ds, "]");
                    arg_dstr_cat(ds, " [");
                    arg_dstr_cat(ds, syntax);
                    arg_dstr_cat(ds, "]");
                    break;
                default:
                    arg_dstr_cat(ds, " [");
                    arg_dstr_cat(ds, syntax);
                    arg_dstr_cat(ds, "]...");
                    break;
            }
        }
    }

    if (suffix) {
        arg_dstr_cat(ds, (char*)suffix);
    }
}

void arg_print_syntax(FILE* fp, void** argtable, const char* suffix) {
    arg_dstr_t ds = arg_dstr_create();
    arg_print_syntax_ds(ds, argtable, suffix);
    fputs(arg_dstr_cstr(ds), fp);
    arg_dstr_destroy(ds);
}

void arg_print_syntaxv_ds(arg_dstr_t ds, void** argtable, const char* suffix) {
    struct arg_hdr** table = (struct arg_hdr**)argtable;
    int i, tabindex;

    /* print remaining options in abbreviated style */
    for (tabindex = 0; table[tabindex] && !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
        char syntax[200] = "";
        const char *shortopts, *longopts, *datatype;

        shortopts = table[tabindex]->shortopts;
        longopts = table[tabindex]->longopts;
        datatype = table[tabindex]->datatype;
        arg_cat_optionv(syntax, sizeof(syntax) - 1, shortopts, longopts, datatype, table[tabindex]->flag & ARG_HASOPTVALUE, "|");

        /* print mandatory options */
        for (i = 0; i < table[tabindex]->mincount; i++) {
            arg_dstr_cat(ds, " ");
            arg_dstr_cat(ds, syntax);
        }

        /* print optional args enclosed in "[..]" */
        switch (table[tabindex]->maxcount - table[tabindex]->mincount) {
            case 0:
                break;
            case 1:
                arg_dstr_cat(ds, " [");
                arg_dstr_cat(ds, syntax);
                arg_dstr_cat(ds, "]");
                break;
            case 2:
                arg_dstr_cat(ds, " [");
                arg_dstr_cat(ds, syntax);
                arg_dstr_cat(ds, "]");
                arg_dstr_cat(ds, " [");
                arg_dstr_cat(ds, syntax);
                arg_dstr_cat(ds, "]");
                break;
            default:
                arg_dstr_cat(ds, " [");
                arg_dstr_cat(ds, syntax);
                arg_dstr_cat(ds, "]...");
                break;
        }
    }

    if (suffix) {
        arg_dstr_cat(ds, (char*)suffix);
    }
}

void arg_print_syntaxv(FILE* fp, void** argtable, const char* suffix) {
    arg_dstr_t ds = arg_dstr_create();
    arg_print_syntaxv_ds(ds, argtable, suffix);
    fputs(arg_dstr_cstr(ds), fp);
    arg_dstr_destroy(ds);
}

void arg_print_glossary_ds(arg_dstr_t ds, void** argtable, const char* format) {
    struct arg_hdr** table = (struct arg_hdr**)argtable;
    int tabindex;

    format = format ? format : "  %-20s %s\n";
    for (tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
        if (table[tabindex]->glossary) {
            char syntax[200] = "";
            const char* shortopts = table[tabindex]->shortopts;
            const char* longopts = table[tabindex]->longopts;
            const char* datatype = table[tabindex]->datatype;
            const char* glossary = table[tabindex]->glossary;
            arg_cat_optionv(syntax, sizeof(syntax) - 1, shortopts, longopts, datatype, table[tabindex]->flag & ARG_HASOPTVALUE, ", ");
            arg_dstr_catf(ds, format, syntax, glossary);
        }
    }
}

void arg_print_glossary(FILE* fp, void** argtable, const char* format) {
    arg_dstr_t ds = arg_dstr_create();
    arg_print_glossary_ds(ds, argtable, format);
    fputs(arg_dstr_cstr(ds), fp);
    arg_dstr_destroy(ds);
}

/**
 * Print a piece of text formatted, which means in a column with a
 * left and a right margin. The lines are wrapped at whitspaces next
 * to right margin. The function does not indent the first line, but
 * only the following ones.
 *
 * See description of arg_print_formatted below.
 */
static void arg_print_formatted_ds(arg_dstr_t ds, const unsigned lmargin, const unsigned rmargin, const char* text) {
    const unsigned int textlen = (unsigned int)strlen(text);
    unsigned int line_start = 0;
    unsigned int line_end = textlen;
    const unsigned int colwidth = (rmargin - lmargin) + 1;

    assert(strlen(text) < UINT_MAX);

    /* Someone doesn't like us... */
    if (line_end < line_start) {
        arg_dstr_catf(ds, "%s\n", text);
    }

    while (line_end > line_start) {
        /* Eat leading white spaces. This is essential because while
           wrapping lines, there will often be a whitespace at beginning
           of line. Preserve newlines */
        while (isspace((int)(*(text + line_start))) && *(text + line_start) != '\n') {
            line_start++;
        }

        /* Find last whitespace, that fits into line */
        if (line_end - line_start > colwidth) {
            line_end = line_start + colwidth;

            while ((line_end > line_start) && !isspace((int)(*(text + line_end)))) {
                line_end--;
            }

            /* If no whitespace could be found, eg. the text is one long word, break the word */
            if (line_end == line_start) {
                /* Set line_end to previous value */
                line_end = line_start + colwidth;
            } else {
                /* Consume trailing spaces, except newlines */
                while ((line_end > line_start) && isspace((int)(*(text + line_end))) && *(text + line_start) != '\n') {
                    line_end--;
                }

                /* Restore the last non-space character */
                line_end++;
            }
        }

        /* Output line of text */
        while (line_start < line_end) {
            char c = *(text + line_start);

            /* If character is newline stop printing, skip this character, as a newline will be printed below. */
            if (c == '\n') {
                line_start++;
                break;
            }

            arg_dstr_catc(ds, c);
            line_start++;
        }
        arg_dstr_cat(ds, "\n");

        /* Initialize another line */
        if (line_end < textlen) {
            unsigned i;

            for (i = 0; i < lmargin; i++) {
                arg_dstr_cat(ds, " ");
            }

            line_end = textlen;
        }
    } /* lines of text */
}

/**
 * Print a piece of text formatted, which means in a column with a
 * left and a right margin. The lines are wrapped at whitspaces next
 * to right margin. The function does not indent the first line, but
 * only the following ones.
 *
 * Example:
 * arg_print_formatted( fp, 0, 5, "Some text that doesn't fit." )
 * will result in the following output:
 *
 * Some
 * text
 * that
 * doesn'
 * t fit.
 *
 * Too long lines will be wrapped in the middle of a word.
 *
 * arg_print_formatted( fp, 2, 7, "Some text that doesn't fit." )
 * will result in the following output:
 *
 * Some
 *   text
 *   that
 *   doesn'
 *   t fit.
 *
 * As you see, the first line is not indented. This enables output of
 * lines, which start in a line where output already happened.
 *
 * Author: Uli Fouquet
 */
void arg_print_formatted(FILE* fp, const unsigned lmargin, const unsigned rmargin, const char* text) {
    arg_dstr_t ds = arg_dstr_create();
    arg_print_formatted_ds(ds, lmargin, rmargin, text);
    fputs(arg_dstr_cstr(ds), fp);
    arg_dstr_destroy(ds);
}

/**
 * Prints the glossary in strict GNU format.
 * Differences to arg_print_glossary() are:
 *   - wraps lines after 80 chars
 *   - indents lines without shortops
 *   - does not accept formatstrings
 *
 * Contributed by Uli Fouquet
 */
void arg_print_glossary_gnu_ds(arg_dstr_t ds, void** argtable) {
    struct arg_hdr** table = (struct arg_hdr**)argtable;
    int tabindex;

    for (tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++) {
        if (table[tabindex]->glossary) {
            char syntax[200] = "";
            const char* shortopts = table[tabindex]->shortopts;
            const char* longopts = table[tabindex]->longopts;
            const char* datatype = table[tabindex]->datatype;
            const char* glossary = table[tabindex]->glossary;

            if (!shortopts && longopts) {
                /* Indent trailing line by 4 spaces... */
                memset(syntax, ' ', 4);
                *(syntax + 4) = '\0';
            }

            arg_cat_optionv(syntax, sizeof(syntax) - 1, shortopts, longopts, datatype, table[tabindex]->flag & ARG_HASOPTVALUE, ", ");

            /* If syntax fits not into column, print glossary in new line... */
            if (strlen(syntax) > 25) {
                arg_dstr_catf(ds, "  %-25s %s\n", syntax, "");
                *syntax = '\0';
            }

            arg_dstr_catf(ds, "  %-25s ", syntax);
            arg_print_formatted_ds(ds, 28, 79, glossary);
        }
    } /* for each table entry */

    arg_dstr_cat(ds, "\n");
}

void arg_print_glossary_gnu(FILE* fp, void** argtable) {
    arg_dstr_t ds = arg_dstr_create();
    arg_print_glossary_gnu_ds(ds, argtable);
    fputs(arg_dstr_cstr(ds), fp);
    arg_dstr_destroy(ds);
}

/**
 * Checks the argtable[] array for NULL entries and returns 1
 * if any are found, zero otherwise.
 */
int arg_nullcheck(void** argtable) {
    struct arg_hdr** table = (struct arg_hdr**)argtable;
    int tabindex;
    /*printf("arg_nullcheck(%p)\n",argtable);*/

    if (!table)
        return 1;

    tabindex = 0;
    do {
        /*printf("argtable[%d]=%p\n",tabindex,argtable[tabindex]);*/
        if (!table[tabindex])
            return 1;
    } while (!(table[tabindex++]->flag & ARG_TERMINATOR));

    return 0;
}

/*
 * arg_free() is deprecated in favour of arg_freetable() due to a flaw in its design.
 * The flaw results in memory leak in the (very rare) case that an intermediate
 * entry in the argtable array failed its memory allocation while others following
 * that entry were still allocated ok. Those subsequent allocations will not be
 * deallocated by arg_free().
 * Despite the unlikeliness of the problem occurring, and the even unlikelier event
 * that it has any deliterious effect, it is fixed regardless by replacing arg_free()
 * with the newer arg_freetable() function.
 * We still keep arg_free() for backwards compatibility.
 */
void arg_free(void** argtable) {
    struct arg_hdr** table = (struct arg_hdr**)argtable;
    int tabindex = 0;
    int flag;
    /*printf("arg_free(%p)\n",argtable);*/
    do {
        /*
           if we encounter a NULL entry then somewhat incorrectly we presume
           we have come to the end of the array. It isnt strictly true because
           an intermediate entry could be NULL with other non-NULL entries to follow.
           The subsequent argtable entries would then not be freed as they should.
         */
        if (table[tabindex] == NULL)
            break;

        flag = table[tabindex]->flag;
        xfree(table[tabindex]);
        table[tabindex++] = NULL;

    } while (!(flag & ARG_TERMINATOR));
}

/* frees each non-NULL element of argtable[], where n is the size of the number of entries in the array */
void arg_freetable(void** argtable, size_t n) {
    struct arg_hdr** table = (struct arg_hdr**)argtable;
    size_t tabindex = 0;
    /*printf("arg_freetable(%p)\n",argtable);*/
    for (tabindex = 0; tabindex < n; tabindex++) {
        if (table[tabindex] == NULL)
            continue;

        xfree(table[tabindex]);
        table[tabindex] = NULL;
    };
}

#ifdef _WIN32
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return TRUE;
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(fdwReason);
    UNREFERENCED_PARAMETER(lpvReserved);
}
#endif
