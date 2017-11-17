/*******************************************************************************
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

/*******************************************************************************
 * This file is part of the argtable3 library.
 *
 * Copyright (C) 2013 Tom G. Huang
 * <tomghuang@gmail.com>
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

#ifndef ARG_UTILS_H
#define ARG_UTILS_H

#define ARG_ENABLE_TRACE 0
#define ARG_ENABLE_LOG 1

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    EMINCOUNT = 1,
    EMAXCOUNT,
    EBADINT,
    EOVERFLOW,
    EBADDOUBLE,
    EBADDATE,
    EREGNOMATCH
};


#if defined(_MSC_VER)
#define ARG_TRACE(x) \
    __pragma(warning(push)) \
    __pragma(warning(disable:4127)) \
    do { if (ARG_ENABLE_TRACE) dbg_printf x; } while (0) \
    __pragma(warning(pop))

#define ARG_LOG(x) \
    __pragma(warning(push)) \
    __pragma(warning(disable:4127)) \
    do { if (ARG_ENABLE_LOG) dbg_printf x; } while (0) \
    __pragma(warning(pop))
#else
#define ARG_TRACE(x) \
    do { if (ARG_ENABLE_TRACE) dbg_printf x; } while (0)

#define ARG_LOG(x) \
    do { if (ARG_ENABLE_LOG) dbg_printf x; } while (0)
#endif 

extern void dbg_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif

/*******************************************************************************
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

#include <stdarg.h>
#include <stdio.h>


void dbg_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

/*	$Id: getopt.h,v 1.1 2009/10/16 19:50:28 rodney Exp rodney $ */
/*	$OpenBSD: getopt.h,v 1.1 2002/12/03 20:24:29 millert Exp $	*/
/*	$NetBSD: getopt.h,v 1.4 2000/07/07 10:43:54 ad Exp $	*/

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Dieter Baron and Thomas Klausner.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _GETOPT_H_
#define _GETOPT_H_

#if 0
#include <sys/cdefs.h>
#endif

/*
 * GNU-like getopt_long() and 4.4BSD getsubopt()/optreset extensions
 */
#define no_argument        0
#define required_argument  1
#define optional_argument  2

struct option {
	/* name of long option */
	const char *name;
	/*
	 * one of no_argument, required_argument, and optional_argument:
	 * whether option takes an argument
	 */
	int has_arg;
	/* if not NULL, set *flag to val when option found */
	int *flag;
	/* if flag not NULL, value to set *flag to; else return value */
	int val;
};

#ifdef __cplusplus
extern "C" {
#endif

int	 getopt_long(int, char * const *, const char *,
	    const struct option *, int *);
int	 getopt_long_only(int, char * const *, const char *,
	    const struct option *, int *);
#ifndef _GETOPT_DEFINED
#define _GETOPT_DEFINED
int	 getopt(int, char * const *, const char *);
int	 getsubopt(char **, char * const *, char **);

extern   char *optarg;                  /* getopt(3) external variables */
extern   int opterr;
extern   int optind;
extern   int optopt;
extern   int optreset;
extern   char *suboptarg;               /* getsubopt(3) external variable */
#endif /* _GETOPT_DEFINED */
 
#ifdef __cplusplus
}
#endif
#endif /* !_GETOPT_H_ */
/*	$Id: getopt_long.c,v 1.1 2009/10/16 19:50:28 rodney Exp rodney $	*/
/*	$OpenBSD: getopt_long.c,v 1.23 2007/10/31 12:34:57 chl Exp $	*/
/*	$NetBSD: getopt_long.c,v 1.15 2002/01/31 22:43:40 tv Exp $	*/

/*
 * Copyright (c) 2002 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F39502-99-1-0512.
 */

#ifndef lint
static const char rcsid[]="$Id: getopt_long.c,v 1.1 2009/10/16 19:50:28 rodney Exp rodney $";
#endif /* lint */
/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Dieter Baron and Thomas Klausner.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

// Define this to replace system getopt
//
//#define	REPLACE_GETOPT		/* use this getopt as the system getopt(3) */

#ifdef REPLACE_GETOPT
int	opterr = 1;		/* if error message should be printed */
int	optind = 1;		/* index into parent argv vector */
int	optopt = '?';		/* character checked for validity */
int	optreset;		/* reset getopt */
char    *optarg;		/* argument associated with option */


#define PRINT_ERROR	((opterr) && (*options != ':'))

#define FLAG_PERMUTE	0x01	/* permute non-options to the end of argv */
#define FLAG_ALLARGS	0x02	/* treat non-options as args to option "-1" */
#define FLAG_LONGONLY	0x04	/* operate as getopt_long_only */

/* return values */
#define	BADCH		(int)'?'
#define	BADARG		((*options == ':') ? (int)':' : (int)'?')
#define	INORDER 	(int)1

#define	EMSG		""

static int getopt_internal(int, char * const *, const char *,
			   const struct option *, int *, int);
static int parse_long_options(char * const *, const char *,
			      const struct option *, int *, int);
static int gcd(int, int);
static void permute_args(int, int, int, char * const *);

static char *place = EMSG; /* option letter processing */

/* XXX: set optreset to 1 rather than these two */
static int nonopt_start = -1; /* first non option argument (for permute) */
static int nonopt_end = -1;   /* first option after non options (for permute) */

/* Error messages */
static const char recargchar[] = "option requires an argument -- %c";
static const char recargstring[] = "option requires an argument -- %s";
static const char ambig[] = "ambiguous option -- %.*s";
static const char noarg[] = "option doesn't take an argument -- %.*s";
static const char illoptchar[] = "unknown option -- %c";
static const char illoptstring[] = "unknown option -- %s";


#if defined(_WIN32) || defined(ESP_PLATFORM)

/* Windows needs warnx().  We change the definition though:
 *  1. (another) global is defined, opterrmsg, which holds the error message
 *  2. errors are always printed out on stderr w/o the program name
 * Note that opterrmsg always gets set no matter what opterr is set to.  The
 * error message will not be printed if opterr is 0 as usual.
 */

#include <stdio.h>
#include <stdarg.h>

extern char opterrmsg[128];
char opterrmsg[128]; /* buffer for the last error message */

static void warnx(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
    /*
    Make sure opterrmsg is always zero-terminated despite the _vsnprintf()
    implementation specifics and manually suppress the warning.
    */
    memset(opterrmsg, 0, sizeof opterrmsg);
	if (fmt != NULL)
		vsnprintf(opterrmsg, sizeof(opterrmsg) - 1, fmt, ap);
	va_end(ap);
#if defined(_WIN32)
#pragma warning(suppress: 6053)
#endif
	fprintf(stderr, "%s\n", opterrmsg);
}

#else
#include <err.h>
#endif /*_WIN32*/

/*
 * Compute the greatest common divisor of a and b.
 */
static int
gcd(int a, int b)
{
	int c;

	c = a % b;
	while (c != 0) {
		a = b;
		b = c;
		c = a % b;
	}

	return (b);
}

/*
 * Exchange the block from nonopt_start to nonopt_end with the block
 * from nonopt_end to opt_end (keeping the same order of arguments
 * in each block).
 */
static void
permute_args(int panonopt_start, int panonopt_end, int opt_end,
	char * const *nargv)
{
	int cstart, cyclelen, i, j, ncycle, nnonopts, nopts, pos;
	char *swap;

	/*
	 * compute lengths of blocks and number and size of cycles
	 */
	nnonopts = panonopt_end - panonopt_start;
	nopts = opt_end - panonopt_end;
	ncycle = gcd(nnonopts, nopts);
	cyclelen = (opt_end - panonopt_start) / ncycle;

	for (i = 0; i < ncycle; i++) {
		cstart = panonopt_end+i;
		pos = cstart;
		for (j = 0; j < cyclelen; j++) {
			if (pos >= panonopt_end)
				pos -= nnonopts;
			else
				pos += nopts;
			swap = nargv[pos];
			/* LINTED const cast */
			((char **) nargv)[pos] = nargv[cstart];
			/* LINTED const cast */
			((char **)nargv)[cstart] = swap;
		}
	}
}

/*
 * parse_long_options --
 *	Parse long options in argc/argv argument vector.
 * Returns -1 if short_too is set and the option does not match long_options.
 */
static int
parse_long_options(char * const *nargv, const char *options,
	const struct option *long_options, int *idx, int short_too)
{
	char *current_argv, *has_equal;
	size_t current_argv_len;
	int i, match;

	current_argv = place;
	match = -1;

	optind++;

	if ((has_equal = strchr(current_argv, '=')) != NULL) {
		/* argument found (--option=arg) */
		current_argv_len = has_equal - current_argv;
		has_equal++;
	} else
		current_argv_len = strlen(current_argv);

	for (i = 0; long_options[i].name; i++) {
		/* find matching long option */
		if (strncmp(current_argv, long_options[i].name,
		    current_argv_len))
			continue;

		if (strlen(long_options[i].name) == current_argv_len) {
			/* exact match */
			match = i;
			break;
		}
		/*
		 * If this is a known short option, don't allow
		 * a partial match of a single character.
		 */
		if (short_too && current_argv_len == 1)
			continue;

		if (match == -1)	/* partial match */
			match = i;
		else {
			/* ambiguous abbreviation */
			if (PRINT_ERROR)
				warnx(ambig, (int)current_argv_len,
				     current_argv);
			optopt = 0;
			return (BADCH);
		}
	}
	if (match != -1) {		/* option found */
		if (long_options[match].has_arg == no_argument
		    && has_equal) {
			if (PRINT_ERROR)
				warnx(noarg, (int)current_argv_len,
				     current_argv);
			/*
			 * XXX: GNU sets optopt to val regardless of flag
			 */
			if (long_options[match].flag == NULL)
				optopt = long_options[match].val;
			else
				optopt = 0;
			return (BADARG);
		}
		if (long_options[match].has_arg == required_argument ||
		    long_options[match].has_arg == optional_argument) {
			if (has_equal)
				optarg = has_equal;
			else if (long_options[match].has_arg ==
			    required_argument) {
				/*
				 * optional argument doesn't use next nargv
				 */
				optarg = nargv[optind++];
			}
		}
		if ((long_options[match].has_arg == required_argument)
		    && (optarg == NULL)) {
			/*
			 * Missing argument; leading ':' indicates no error
			 * should be generated.
			 */
			if (PRINT_ERROR)
				warnx(recargstring,
				    current_argv);
			/*
			 * XXX: GNU sets optopt to val regardless of flag
			 */
			if (long_options[match].flag == NULL)
				optopt = long_options[match].val;
			else
				optopt = 0;
			--optind;
			return (BADARG);
		}
	} else {			/* unknown option */
		if (short_too) {
			--optind;
			return (-1);
		}
		if (PRINT_ERROR)
			warnx(illoptstring, current_argv);
		optopt = 0;
		return (BADCH);
	}
	if (idx)
		*idx = match;
	if (long_options[match].flag) {
		*long_options[match].flag = long_options[match].val;
		return (0);
	} else
		return (long_options[match].val);
}

/*
 * getopt_internal --
 *	Parse argc/argv argument vector.  Called by user level routines.
 */
static int
getopt_internal(int nargc, char * const *nargv, const char *options,
	const struct option *long_options, int *idx, int flags)
{
	char *oli;				/* option letter list index */
	int optchar, short_too;
	static int posixly_correct = -1;

	if (options == NULL)
		return (-1);

	/*
	 * Disable GNU extensions if POSIXLY_CORRECT is set or options
	 * string begins with a '+'.
	 */
	if (posixly_correct == -1)
		posixly_correct = (getenv("POSIXLY_CORRECT") != NULL);
	if (posixly_correct || *options == '+')
		flags &= ~FLAG_PERMUTE;
	else if (*options == '-')
		flags |= FLAG_ALLARGS;
	if (*options == '+' || *options == '-')
		options++;

	/*
	 * XXX Some GNU programs (like cvs) set optind to 0 instead of
	 * XXX using optreset.  Work around this braindamage.
	 */
	if (optind == 0)
		optind = optreset = 1;

	optarg = NULL;
	if (optreset)
		nonopt_start = nonopt_end = -1;
start:
	if (optreset || !*place) {		/* update scanning pointer */
		optreset = 0;
		if (optind >= nargc) {          /* end of argument vector */
			place = EMSG;
			if (nonopt_end != -1) {
				/* do permutation, if we have to */
				permute_args(nonopt_start, nonopt_end,
				    optind, nargv);
				optind -= nonopt_end - nonopt_start;
			}
			else if (nonopt_start != -1) {
				/*
				 * If we skipped non-options, set optind
				 * to the first of them.
				 */
				optind = nonopt_start;
			}
			nonopt_start = nonopt_end = -1;
			return (-1);
		}
		if (*(place = nargv[optind]) != '-' ||
		    (place[1] == '\0' && strchr(options, '-') == NULL)) {
			place = EMSG;		/* found non-option */
			if (flags & FLAG_ALLARGS) {
				/*
				 * GNU extension:
				 * return non-option as argument to option 1
				 */
				optarg = nargv[optind++];
				return (INORDER);
			}
			if (!(flags & FLAG_PERMUTE)) {
				/*
				 * If no permutation wanted, stop parsing
				 * at first non-option.
				 */
				return (-1);
			}
			/* do permutation */
			if (nonopt_start == -1)
				nonopt_start = optind;
			else if (nonopt_end != -1) {
				permute_args(nonopt_start, nonopt_end,
				    optind, nargv);
				nonopt_start = optind -
				    (nonopt_end - nonopt_start);
				nonopt_end = -1;
			}
			optind++;
			/* process next argument */
			goto start;
		}
		if (nonopt_start != -1 && nonopt_end == -1)
			nonopt_end = optind;

		/*
		 * If we have "-" do nothing, if "--" we are done.
		 */
		if (place[1] != '\0' && *++place == '-' && place[1] == '\0') {
			optind++;
			place = EMSG;
			/*
			 * We found an option (--), so if we skipped
			 * non-options, we have to permute.
			 */
			if (nonopt_end != -1) {
				permute_args(nonopt_start, nonopt_end,
				    optind, nargv);
				optind -= nonopt_end - nonopt_start;
			}
			nonopt_start = nonopt_end = -1;
			return (-1);
		}
	}

	/*
	 * Check long options if:
	 *  1) we were passed some
	 *  2) the arg is not just "-"
	 *  3) either the arg starts with -- we are getopt_long_only()
	 */
	if (long_options != NULL && place != nargv[optind] &&
	    (*place == '-' || (flags & FLAG_LONGONLY))) {
		short_too = 0;
		if (*place == '-')
			place++;		/* --foo long option */
		else if (*place != ':' && strchr(options, *place) != NULL)
			short_too = 1;		/* could be short option too */

		optchar = parse_long_options(nargv, options, long_options,
		    idx, short_too);
		if (optchar != -1) {
			place = EMSG;
			return (optchar);
		}
	}

	if ((optchar = (int)*place++) == (int)':' ||
	    (optchar == (int)'-' && *place != '\0') ||
	    (oli = strchr(options, optchar)) == NULL) {
		/*
		 * If the user specified "-" and  '-' isn't listed in
		 * options, return -1 (non-option) as per POSIX.
		 * Otherwise, it is an unknown option character (or ':').
		 */
		if (optchar == (int)'-' && *place == '\0')
			return (-1);
		if (!*place)
			++optind;
		if (PRINT_ERROR)
			warnx(illoptchar, optchar);
		optopt = optchar;
		return (BADCH);
	}
	if (long_options != NULL && optchar == 'W' && oli[1] == ';') {
		/* -W long-option */
		if (*place)			/* no space */
			/* NOTHING */;
		else if (++optind >= nargc) {	/* no arg */
			place = EMSG;
			if (PRINT_ERROR)
				warnx(recargchar, optchar);
			optopt = optchar;
			return (BADARG);
		} else				/* white space */
			place = nargv[optind];
		optchar = parse_long_options(nargv, options, long_options,
		    idx, 0);
		place = EMSG;
		return (optchar);
	}
	if (*++oli != ':') {			/* doesn't take argument */
		if (!*place)
			++optind;
	} else {				/* takes (optional) argument */
		optarg = NULL;
		if (*place)			/* no white space */
			optarg = place;
		else if (oli[1] != ':') {	/* arg not optional */
			if (++optind >= nargc) {	/* no arg */
				place = EMSG;
				if (PRINT_ERROR)
					warnx(recargchar, optchar);
				optopt = optchar;
				return (BADARG);
			} else
				optarg = nargv[optind];
		}
		place = EMSG;
		++optind;
	}
	/* dump back option letter */
	return (optchar);
}


/*
 * getopt --
 *	Parse argc/argv argument vector.
 *
 * [eventually this will replace the BSD getopt]
 */
int
getopt(int nargc, char * const *nargv, const char *options)
{

	/*
	 * We don't pass FLAG_PERMUTE to getopt_internal() since
	 * the BSD getopt(3) (unlike GNU) has never done this.
	 *
	 * Furthermore, since many privileged programs call getopt()
	 * before dropping privileges it makes sense to keep things
	 * as simple (and bug-free) as possible.
	 */
	return (getopt_internal(nargc, nargv, options, NULL, NULL, 0));
}


/*
 * getopt_long --
 *	Parse argc/argv argument vector.
 */
int
getopt_long(int nargc, char * const *nargv, const char *options,
    const struct option *long_options, int *idx)
{

	return (getopt_internal(nargc, nargv, options, long_options, idx,
	    FLAG_PERMUTE));
}

/*
 * getopt_long_only --
 *	Parse argc/argv argument vector.
 */
int
getopt_long_only(int nargc, char * const *nargv, const char *options,
    const struct option *long_options, int *idx)
{

	return (getopt_internal(nargc, nargv, options, long_options, idx,
	    FLAG_PERMUTE|FLAG_LONGONLY));
}
#endif /* REPLACE_GETOPT */
/*******************************************************************************
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

#include <stdlib.h>
#include <string.h>

#include "argtable3.h"


char * arg_strptime(const char *buf, const char *fmt, struct tm *tm);


static void arg_date_resetfn(struct arg_date *parent)
{
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}


static int arg_date_scanfn(struct arg_date *parent, const char *argval)
{
    int errorcode = 0;

    if (parent->count == parent->hdr.maxcount)
    {
        errorcode = EMAXCOUNT;
    }
    else if (!argval)
    {
        /* no argument value was given, leave parent->tmval[] unaltered but still count it */
        parent->count++;
    }
    else
    {
        const char *pend;
        struct tm tm = parent->tmval[parent->count];

        /* parse the given argument value, store result in parent->tmval[] */
        pend = arg_strptime(argval, parent->format, &tm);
        if (pend && pend[0] == '\0')
            parent->tmval[parent->count++] = tm;
        else
            errorcode = EBADDATE;
    }

    ARG_TRACE(("%s:scanfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}


static int arg_date_checkfn(struct arg_date *parent)
{
    int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT : 0;

    ARG_TRACE(("%s:checkfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}


static void arg_date_errorfn(
    struct arg_date *parent,
    FILE *fp,
    int errorcode,
    const char *argval,
    const char *progname)
{
    const char *shortopts = parent->hdr.shortopts;
    const char *longopts  = parent->hdr.longopts;
    const char *datatype  = parent->hdr.datatype;

    /* make argval NULL safe */
    argval = argval ? argval : "";

    fprintf(fp, "%s: ", progname);
    switch(errorcode)
    {
    case EMINCOUNT:
        fputs("missing option ", fp);
        arg_print_option(fp, shortopts, longopts, datatype, "\n");
        break;

    case EMAXCOUNT:
        fputs("excess option ", fp);
        arg_print_option(fp, shortopts, longopts, argval, "\n");
        break;

    case EBADDATE:
    {
        struct tm tm;
        char buff[200];

        fprintf(fp, "illegal timestamp format \"%s\"\n", argval);
        memset(&tm, 0, sizeof(tm));
        arg_strptime("1999-12-31 23:59:59", "%F %H:%M:%S", &tm);
        strftime(buff, sizeof(buff), parent->format, &tm);
        printf("correct format is \"%s\"\n", buff);
        break;
    }
    }
}


struct arg_date * arg_date0(
    const char * shortopts,
    const char * longopts,
    const char * format,
    const char *datatype,
    const char *glossary)
{
    return arg_daten(shortopts, longopts, format, datatype, 0, 1, glossary);
}


struct arg_date * arg_date1(
    const char * shortopts,
    const char * longopts,
    const char * format,
    const char *datatype,
    const char *glossary)
{
    return arg_daten(shortopts, longopts, format, datatype, 1, 1, glossary);
}


struct arg_date * arg_daten(
    const char * shortopts,
    const char * longopts,
    const char * format,
    const char *datatype,
    int mincount,
    int maxcount,
    const char *glossary)
{
    size_t nbytes;
    struct arg_date *result;

    /* foolproof things by ensuring maxcount is not less than mincount */
    maxcount = (maxcount < mincount) ? mincount : maxcount;

    /* default time format is the national date format for the locale */
    if (!format)
        format = "%x";

    nbytes = sizeof(struct arg_date)         /* storage for struct arg_date */
        + maxcount * sizeof(struct tm);    /* storage for tmval[maxcount] array */

    /* allocate storage for the arg_date struct + tmval[] array.    */
    /* we use calloc because we want the tmval[] array zero filled. */
    result = (struct arg_date *)calloc(1, nbytes);
    if (result)
    {
        /* init the arg_hdr struct */
        result->hdr.flag      = ARG_HASVALUE;
        result->hdr.shortopts = shortopts;
        result->hdr.longopts  = longopts;
        result->hdr.datatype  = datatype ? datatype : format;
        result->hdr.glossary  = glossary;
        result->hdr.mincount  = mincount;
        result->hdr.maxcount  = maxcount;
        result->hdr.parent    = result;
        result->hdr.resetfn   = (arg_resetfn *)arg_date_resetfn;
        result->hdr.scanfn    = (arg_scanfn *)arg_date_scanfn;
        result->hdr.checkfn   = (arg_checkfn *)arg_date_checkfn;
        result->hdr.errorfn   = (arg_errorfn *)arg_date_errorfn;

        /* store the tmval[maxcount] array immediately after the arg_date struct */
        result->tmval  = (struct tm *)(result + 1);

        /* init the remaining arg_date member variables */
        result->count = 0;
        result->format = format;
    }

    ARG_TRACE(("arg_daten() returns %p\n", result));
    return result;
}


/*-
 * Copyright (c) 1997, 1998, 2005, 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code was contributed to The NetBSD Foundation by Klaus Klein.
 * Heavily optimised by David Laight
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <ctype.h>
#include <string.h>
#include <time.h>

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E                   0x01
#define ALT_O                   0x02
#define LEGAL_ALT(x)            { if (alt_format & ~(x)) return (0); }
#define TM_YEAR_BASE   (1900)

static int conv_num(const char * *, int *, int, int);

static const char *day[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
    "Friday", "Saturday"
};

static const char *abday[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char *mon[12] = {
    "January", "February", "March", "April", "May", "June", "July",
    "August", "September", "October", "November", "December"
};

static const char *abmon[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char *am_pm[2] = {
    "AM", "PM"
};


static int arg_strcasecmp(const char *s1, const char *s2)
{
    const unsigned char *us1 = (const unsigned char *)s1;
    const unsigned char *us2 = (const unsigned char *)s2;
    while (tolower(*us1) == tolower(*us2++))
        if (*us1++ == '\0')
            return 0;

    return tolower(*us1) - tolower(*--us2);
}


static int arg_strncasecmp(const char *s1, const char *s2, size_t n)
{
    if (n != 0)
    {
        const unsigned char *us1 = (const unsigned char *)s1;
        const unsigned char *us2 = (const unsigned char *)s2;
        do
        {
            if (tolower(*us1) != tolower(*us2++))
                return tolower(*us1) - tolower(*--us2);

            if (*us1++ == '\0')
                break;
        } while (--n != 0);
    }

    return 0;
}


char * arg_strptime(const char *buf, const char *fmt, struct tm *tm)
{
    char c;
    const char *bp;
    size_t len = 0;
    int alt_format, i, split_year = 0;

    bp = buf;

    while ((c = *fmt) != '\0') {
        /* Clear `alternate' modifier prior to new conversion. */
        alt_format = 0;

        /* Eat up white-space. */
        if (isspace((int) c)) {
            while (isspace((int) *bp))
                bp++;

            fmt++;
            continue;
        }

        if ((c = *fmt++) != '%')
            goto literal;


again:
        switch (c = *fmt++)
        {
        case '%': /* "%%" is converted to "%". */
literal:
            if (c != *bp++)
                return (0);
            break;

        /*
         * "Alternative" modifiers. Just set the appropriate flag
         * and start over again.
         */
        case 'E': /* "%E?" alternative conversion modifier. */
            LEGAL_ALT(0);
            alt_format |= ALT_E;
            goto again;

        case 'O': /* "%O?" alternative conversion modifier. */
            LEGAL_ALT(0);
            alt_format |= ALT_O;
            goto again;

        /*
         * "Complex" conversion rules, implemented through recursion.
         */
        case 'c': /* Date and time, using the locale's format. */
            LEGAL_ALT(ALT_E);
            bp = arg_strptime(bp, "%x %X", tm);
            if (!bp)
                return (0);
            break;

        case 'D': /* The date as "%m/%d/%y". */
            LEGAL_ALT(0);
            bp = arg_strptime(bp, "%m/%d/%y", tm);
            if (!bp)
                return (0);
            break;

        case 'R': /* The time as "%H:%M". */
            LEGAL_ALT(0);
            bp = arg_strptime(bp, "%H:%M", tm);
            if (!bp)
                return (0);
            break;

        case 'r': /* The time in 12-hour clock representation. */
            LEGAL_ALT(0);
            bp = arg_strptime(bp, "%I:%M:%S %p", tm);
            if (!bp)
                return (0);
            break;

        case 'T': /* The time as "%H:%M:%S". */
            LEGAL_ALT(0);
            bp = arg_strptime(bp, "%H:%M:%S", tm);
            if (!bp)
                return (0);
            break;

        case 'X': /* The time, using the locale's format. */
            LEGAL_ALT(ALT_E);
            bp = arg_strptime(bp, "%H:%M:%S", tm);
            if (!bp)
                return (0);
            break;

        case 'x': /* The date, using the locale's format. */
            LEGAL_ALT(ALT_E);
            bp = arg_strptime(bp, "%m/%d/%y", tm);
            if (!bp)
                return (0);
            break;

        /*
         * "Elementary" conversion rules.
         */
        case 'A': /* The day of week, using the locale's form. */
        case 'a':
            LEGAL_ALT(0);
            for (i = 0; i < 7; i++) {
                /* Full name. */
                len = strlen(day[i]);
                if (arg_strncasecmp(day[i], bp, len) == 0)
                    break;

                /* Abbreviated name. */
                len = strlen(abday[i]);
                if (arg_strncasecmp(abday[i], bp, len) == 0)
                    break;
            }

            /* Nothing matched. */
            if (i == 7)
                return (0);

            tm->tm_wday = i;
            bp += len;
            break;

        case 'B': /* The month, using the locale's form. */
        case 'b':
        case 'h':
            LEGAL_ALT(0);
            for (i = 0; i < 12; i++) {
                /* Full name. */
                len = strlen(mon[i]);
                if (arg_strncasecmp(mon[i], bp, len) == 0)
                    break;

                /* Abbreviated name. */
                len = strlen(abmon[i]);
                if (arg_strncasecmp(abmon[i], bp, len) == 0)
                    break;
            }

            /* Nothing matched. */
            if (i == 12)
                return (0);

            tm->tm_mon = i;
            bp += len;
            break;

        case 'C': /* The century number. */
            LEGAL_ALT(ALT_E);
            if (!(conv_num(&bp, &i, 0, 99)))
                return (0);

            if (split_year) {
                tm->tm_year = (tm->tm_year % 100) + (i * 100);
            } else {
                tm->tm_year = i * 100;
                split_year = 1;
            }
            break;

        case 'd': /* The day of month. */
        case 'e':
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &tm->tm_mday, 1, 31)))
                return (0);
            break;

        case 'k': /* The hour (24-hour clock representation). */
            LEGAL_ALT(0);
        /* FALLTHROUGH */
        case 'H':
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &tm->tm_hour, 0, 23)))
                return (0);
            break;

        case 'l': /* The hour (12-hour clock representation). */
            LEGAL_ALT(0);
        /* FALLTHROUGH */
        case 'I':
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &tm->tm_hour, 1, 12)))
                return (0);
            if (tm->tm_hour == 12)
                tm->tm_hour = 0;
            break;

        case 'j': /* The day of year. */
            LEGAL_ALT(0);
            if (!(conv_num(&bp, &i, 1, 366)))
                return (0);
            tm->tm_yday = i - 1;
            break;

        case 'M': /* The minute. */
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &tm->tm_min, 0, 59)))
                return (0);
            break;

        case 'm': /* The month. */
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &i, 1, 12)))
                return (0);
            tm->tm_mon = i - 1;
            break;

        case 'p': /* The locale's equivalent of AM/PM. */
            LEGAL_ALT(0);
            /* AM? */
            if (arg_strcasecmp(am_pm[0], bp) == 0) {
                if (tm->tm_hour > 11)
                    return (0);

                bp += strlen(am_pm[0]);
                break;
            }
            /* PM? */
            else if (arg_strcasecmp(am_pm[1], bp) == 0) {
                if (tm->tm_hour > 11)
                    return (0);

                tm->tm_hour += 12;
                bp += strlen(am_pm[1]);
                break;
            }

            /* Nothing matched. */
            return (0);

        case 'S': /* The seconds. */
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &tm->tm_sec, 0, 61)))
                return (0);
            break;

        case 'U': /* The week of year, beginning on sunday. */
        case 'W': /* The week of year, beginning on monday. */
            LEGAL_ALT(ALT_O);
            /*
             * XXX This is bogus, as we can not assume any valid
             * information present in the tm structure at this
             * point to calculate a real value, so just check the
             * range for now.
             */
            if (!(conv_num(&bp, &i, 0, 53)))
                return (0);
            break;

        case 'w': /* The day of week, beginning on sunday. */
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
                return (0);
            break;

        case 'Y': /* The year. */
            LEGAL_ALT(ALT_E);
            if (!(conv_num(&bp, &i, 0, 9999)))
                return (0);

            tm->tm_year = i - TM_YEAR_BASE;
            break;

        case 'y': /* The year within 100 years of the epoch. */
            LEGAL_ALT(ALT_E | ALT_O);
            if (!(conv_num(&bp, &i, 0, 99)))
                return (0);

            if (split_year) {
                tm->tm_year = ((tm->tm_year / 100) * 100) + i;
                break;
            }
            split_year = 1;
            if (i <= 68)
                tm->tm_year = i + 2000 - TM_YEAR_BASE;
            else
                tm->tm_year = i + 1900 - TM_YEAR_BASE;
            break;

        /*
         * Miscellaneous conversions.
         */
        case 'n': /* Any kind of white-space. */
        case 't':
            LEGAL_ALT(0);
            while (isspace((int) *bp))
                bp++;
            break;


        default: /* Unknown/unsupported conversion. */
            return (0);
        }


    }

    /* LINTED functional specification */
    return ((char *)bp);
}


static int conv_num(const char * *buf, int *dest, int llim, int ulim)
{
    int result = 0;

    /* The limit also determines the number of valid digits. */
    int rulim = ulim;

    if (**buf < '0' || **buf > '9')
        return (0);

    do {
        result *= 10;
        result += *(*buf)++ - '0';
        rulim /= 10;
    } while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

    if (result < llim || result > ulim)
        return (0);

    *dest = result;
    return (1);
}
/*******************************************************************************
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

#include <stdlib.h>

#include "argtable3.h"


static void arg_dbl_resetfn(struct arg_dbl *parent)
{
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}


static int arg_dbl_scanfn(struct arg_dbl *parent, const char *argval)
{
    int errorcode = 0;

    if (parent->count == parent->hdr.maxcount)
    {
        /* maximum number of arguments exceeded */
        errorcode = EMAXCOUNT;
    }
    else if (!argval)
    {
        /* a valid argument with no argument value was given. */
        /* This happens when an optional argument value was invoked. */
        /* leave parent argument value unaltered but still count the argument. */
        parent->count++;
    }
    else
    {
        double val;
        char *end;

        /* extract double from argval into val */
        val = strtod(argval, &end);

        /* if success then store result in parent->dval[] array otherwise return error*/
        if (*end == 0)
            parent->dval[parent->count++] = val;
        else
            errorcode = EBADDOUBLE;
    }

    ARG_TRACE(("%s:scanfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}


static int arg_dbl_checkfn(struct arg_dbl *parent)
{
    int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT : 0;
    
    ARG_TRACE(("%s:checkfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}


static void arg_dbl_errorfn(
    struct arg_dbl *parent,
    FILE *fp,
    int errorcode,
    const char *argval,
    const char *progname)
{
    const char *shortopts = parent->hdr.shortopts;
    const char *longopts  = parent->hdr.longopts;
    const char *datatype  = parent->hdr.datatype;

    /* make argval NULL safe */
    argval = argval ? argval : "";

    fprintf(fp, "%s: ", progname);
    switch(errorcode)
    {
    case EMINCOUNT:
        fputs("missing option ", fp);
        arg_print_option(fp, shortopts, longopts, datatype, "\n");
        break;

    case EMAXCOUNT:
        fputs("excess option ", fp);
        arg_print_option(fp, shortopts, longopts, argval, "\n");
        break;

    case EBADDOUBLE:
        fprintf(fp, "invalid argument \"%s\" to option ", argval);
        arg_print_option(fp, shortopts, longopts, datatype, "\n");
        break;
    }
}


struct arg_dbl * arg_dbl0(
    const char * shortopts,
    const char * longopts,
    const char *datatype,
    const char *glossary)
{
    return arg_dbln(shortopts, longopts, datatype, 0, 1, glossary);
}


struct arg_dbl * arg_dbl1(
    const char * shortopts,
    const char * longopts,
    const char *datatype,
    const char *glossary)
{
    return arg_dbln(shortopts, longopts, datatype, 1, 1, glossary);
}


struct arg_dbl * arg_dbln(
    const char * shortopts,
    const char * longopts,
    const char *datatype,
    int mincount,
    int maxcount,
    const char *glossary)
{
    size_t nbytes;
    struct arg_dbl *result;

    /* foolproof things by ensuring maxcount is not less than mincount */
    maxcount = (maxcount < mincount) ? mincount : maxcount;

    nbytes = sizeof(struct arg_dbl)     /* storage for struct arg_dbl */
             + (maxcount + 1) * sizeof(double); /* storage for dval[maxcount] array plus one extra for padding to memory boundary */

    result = (struct arg_dbl *)malloc(nbytes);
    if (result)
    {
        size_t addr;
        size_t rem;

        /* init the arg_hdr struct */
        result->hdr.flag      = ARG_HASVALUE;
        result->hdr.shortopts = shortopts;
        result->hdr.longopts  = longopts;
        result->hdr.datatype  = datatype ? datatype : "<double>";
        result->hdr.glossary  = glossary;
        result->hdr.mincount  = mincount;
        result->hdr.maxcount  = maxcount;
        result->hdr.parent    = result;
        result->hdr.resetfn   = (arg_resetfn *)arg_dbl_resetfn;
        result->hdr.scanfn    = (arg_scanfn *)arg_dbl_scanfn;
        result->hdr.checkfn   = (arg_checkfn *)arg_dbl_checkfn;
        result->hdr.errorfn   = (arg_errorfn *)arg_dbl_errorfn;

        /* Store the dval[maxcount] array on the first double boundary that
         * immediately follows the arg_dbl struct. We do the memory alignment
         * purely for SPARC and Motorola systems. They require floats and
         * doubles to be aligned on natural boundaries.
         */
        addr = (size_t)(result + 1);
        rem  = addr % sizeof(double);
        result->dval  = (double *)(addr + sizeof(double) - rem);
        ARG_TRACE(("addr=%p, dval=%p, sizeof(double)=%d rem=%d\n", addr, result->dval, (int)sizeof(double), (int)rem));

        result->count = 0;
    }
    
    ARG_TRACE(("arg_dbln() returns %p\n", result));
    return result;
}
/*******************************************************************************
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

#include <stdlib.h>

#include "argtable3.h"


static void arg_end_resetfn(struct arg_end *parent)
{
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}

static void arg_end_errorfn(
    void *parent,
    FILE *fp,
    int error,
    const char *argval,
    const char *progname)
{
    /* suppress unreferenced formal parameter warning */
    (void)parent;

    progname = progname ? progname : "";
    argval = argval ? argval : "";

    fprintf(fp, "%s: ", progname);
    switch(error)
    {
    case ARG_ELIMIT:
        fputs("too many errors to display", fp);
        break;
    case ARG_EMALLOC:
        fputs("insufficent memory", fp);
        break;
    case ARG_ENOMATCH:
        fprintf(fp, "unexpected argument \"%s\"", argval);
        break;
    case ARG_EMISSARG:
        fprintf(fp, "option \"%s\" requires an argument", argval);
        break;
    case ARG_ELONGOPT:
        fprintf(fp, "invalid option \"%s\"", argval);
        break;
    default:
        fprintf(fp, "invalid option \"-%c\"", error);
        break;
    }
    
    fputc('\n', fp);
}


struct arg_end * arg_end(int maxcount)
{
    size_t nbytes;
    struct arg_end *result;

    nbytes = sizeof(struct arg_end)
             + maxcount * sizeof(int)     /* storage for int error[maxcount] array*/
             + maxcount * sizeof(void *)  /* storage for void* parent[maxcount] array */
             + maxcount * sizeof(char *); /* storage for char* argval[maxcount] array */

    result = (struct arg_end *)malloc(nbytes);
    if (result)
    {
        /* init the arg_hdr struct */
        result->hdr.flag      = ARG_TERMINATOR;
        result->hdr.shortopts = NULL;
        result->hdr.longopts  = NULL;
        result->hdr.datatype  = NULL;
        result->hdr.glossary  = NULL;
        result->hdr.mincount  = 1;
        result->hdr.maxcount  = maxcount;
        result->hdr.parent    = result;
        result->hdr.resetfn   = (arg_resetfn *)arg_end_resetfn;
        result->hdr.scanfn    = NULL;
        result->hdr.checkfn   = NULL;
        result->hdr.errorfn   = (arg_errorfn *)arg_end_errorfn;

        /* store error[maxcount] array immediately after struct arg_end */
        result->error = (int *)(result + 1);

        /* store parent[maxcount] array immediately after error[] array */
        result->parent = (void * *)(result->error + maxcount );

        /* store argval[maxcount] array immediately after parent[] array */
        result->argval = (const char * *)(result->parent + maxcount );
    }

    ARG_TRACE(("arg_end(%d) returns %p\n", maxcount, result));
    return result;
}


void arg_print_errors(FILE * fp, struct arg_end * end, const char * progname)
{
    int i;
    ARG_TRACE(("arg_errors()\n"));
    for (i = 0; i < end->count; i++)
    {
        struct arg_hdr *errorparent = (struct arg_hdr *)(end->parent[i]);
        if (errorparent->errorfn)
            errorparent->errorfn(end->parent[i],
                                 fp,
                                 end->error[i],
                                 end->argval[i],
                                 progname);
    }
}
/*******************************************************************************
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

#include <string.h>
#include <stdlib.h>

#include "argtable3.h"

#ifdef WIN32
# define FILESEPARATOR1 '\\'
# define FILESEPARATOR2 '/'
#else
# define FILESEPARATOR1 '/'
# define FILESEPARATOR2 '/'
#endif


static void arg_file_resetfn(struct arg_file *parent)
{
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}


/* Returns ptr to the base filename within *filename */
static const char * arg_basename(const char *filename)
{
    const char *result = NULL, *result1, *result2;

    /* Find the last occurrence of eother file separator character. */
    /* Two alternative file separator chars are supported as legal  */
    /* file separators but not both together in the same filename.  */
    result1 = (filename ? strrchr(filename, FILESEPARATOR1) : NULL);
    result2 = (filename ? strrchr(filename, FILESEPARATOR2) : NULL);

    if (result2)
        result = result2 + 1;  /* using FILESEPARATOR2 (the alternative file separator) */

    if (result1)
        result = result1 + 1;  /* using FILESEPARATOR1 (the preferred file separator) */

    if (!result)
        result = filename;  /* neither file separator was found so basename is the whole filename */

    /* special cases of "." and ".." are not considered basenames */
    if (result && ( strcmp(".", result) == 0 || strcmp("..", result) == 0 ))
        result = filename + strlen(filename);

    return result;
}


/* Returns ptr to the file extension within *basename */
static const char * arg_extension(const char *basename)
{
    /* find the last occurrence of '.' in basename */
    const char *result = (basename ? strrchr(basename, '.') : NULL);

    /* if no '.' was found then return pointer to end of basename */
    if (basename && !result)
        result = basename + strlen(basename);

    /* special case: basenames with a single leading dot (eg ".foo") are not considered as true extensions */
    if (basename && result == basename)
        result = basename + strlen(basename);

    /* special case: empty extensions (eg "foo.","foo..") are not considered as true extensions */
    if (basename && result && result[1] == '\0')
        result = basename + strlen(basename);

    return result;
}


static int arg_file_scanfn(struct arg_file *parent, const char *argval)
{
    int errorcode = 0;

    if (parent->count == parent->hdr.maxcount)
    {
        /* maximum number of arguments exceeded */
        errorcode = EMAXCOUNT;
    }
    else if (!argval)
    {
        /* a valid argument with no argument value was given. */
        /* This happens when an optional argument value was invoked. */
        /* leave parent arguiment value unaltered but still count the argument. */
        parent->count++;
    }
    else
    {
        parent->filename[parent->count]  = argval;
        parent->basename[parent->count]  = arg_basename(argval);
        parent->extension[parent->count] =
            arg_extension(parent->basename[parent->count]);                                /* only seek extensions within the basename (not the file path)*/
        parent->count++;
    }

    ARG_TRACE(("%s4:scanfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}


static int arg_file_checkfn(struct arg_file *parent)
{
    int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT : 0;
    
    ARG_TRACE(("%s:checkfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}


static void arg_file_errorfn(
    struct arg_file *parent,
    FILE *fp,
    int errorcode,
    const char *argval,
    const char *progname)
{
    const char *shortopts = parent->hdr.shortopts;
    const char *longopts  = parent->hdr.longopts;
    const char *datatype  = parent->hdr.datatype;

    /* make argval NULL safe */
    argval = argval ? argval : "";

    fprintf(fp, "%s: ", progname);
    switch(errorcode)
    {
    case EMINCOUNT:
        fputs("missing option ", fp);
        arg_print_option(fp, shortopts, longopts, datatype, "\n");
        break;

    case EMAXCOUNT:
        fputs("excess option ", fp);
        arg_print_option(fp, shortopts, longopts, argval, "\n");
        break;

    default:
        fprintf(fp, "unknown error at \"%s\"\n", argval);
    }
}


struct arg_file * arg_file0(
    const char * shortopts,
    const char * longopts,
    const char *datatype,
    const char *glossary)
{
    return arg_filen(shortopts, longopts, datatype, 0, 1, glossary);
}


struct arg_file * arg_file1(
    const char * shortopts,
    const char * longopts,
    const char *datatype,
    const char *glossary)
{
    return arg_filen(shortopts, longopts, datatype, 1, 1, glossary);
}


struct arg_file * arg_filen(
    const char * shortopts,
    const char * longopts,
    const char *datatype,
    int mincount,
    int maxcount,
    const char *glossary)
{
    size_t nbytes;
    struct arg_file *result;

    /* foolproof things by ensuring maxcount is not less than mincount */
    maxcount = (maxcount < mincount) ? mincount : maxcount;

    nbytes = sizeof(struct arg_file)      /* storage for struct arg_file */
             + sizeof(char *) * maxcount  /* storage for filename[maxcount] array */
             + sizeof(char *) * maxcount  /* storage for basename[maxcount] array */
             + sizeof(char *) * maxcount; /* storage for extension[maxcount] array */

    result = (struct arg_file *)malloc(nbytes);
    if (result)
    {
        int i;

        /* init the arg_hdr struct */
        result->hdr.flag      = ARG_HASVALUE;
        result->hdr.shortopts = shortopts;
        result->hdr.longopts  = longopts;
        result->hdr.glossary  = glossary;
        result->hdr.datatype  = datatype ? datatype : "<file>";
        result->hdr.mincount  = mincount;
        result->hdr.maxcount  = maxcount;
        result->hdr.parent    = result;
        result->hdr.resetfn   = (arg_resetfn *)arg_file_resetfn;
        result->hdr.scanfn    = (arg_scanfn *)arg_file_scanfn;
        result->hdr.checkfn   = (arg_checkfn *)arg_file_checkfn;
        result->hdr.errorfn   = (arg_errorfn *)arg_file_errorfn;

        /* store the filename,basename,extension arrays immediately after the arg_file struct */
        result->filename  = (const char * *)(result + 1);
        result->basename  = result->filename + maxcount;
        result->extension = result->basename + maxcount;
        result->count = 0;

        /* foolproof the string pointers by initialising them with empty strings */
        for (i = 0; i < maxcount; i++)
        {
            result->filename[i] = "";
            result->basename[i] = "";
            result->extension[i] = "";
        }
    }
    
    ARG_TRACE(("arg_filen() returns %p\n", result));
    return result;
}
/*******************************************************************************
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

#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#include "argtable3.h"


static void arg_int_resetfn(struct arg_int *parent)
{
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}


/* strtol0x() is like strtol() except that the numeric string is    */
/* expected to be prefixed by "0X" where X is a user supplied char. */
/* The string may optionally be prefixed by white space and + or -  */
/* as in +0X123 or -0X123.                                          */
/* Once the prefix has been scanned, the remainder of the numeric   */
/* string is converted using strtol() with the given base.          */
/* eg: to parse hex str="-0X12324", specify X='X' and base=16.      */
/* eg: to parse oct str="+0o12324", specify X='O' and base=8.       */
/* eg: to parse bin str="-0B01010", specify X='B' and base=2.       */
/* Failure of conversion is indicated by result where *endptr==str. */
static long int strtol0X(const char * str,
                         const char * *endptr,
                         char X,
                         int base)
{
    long int val;               /* stores result */
    int s = 1;                    /* sign is +1 or -1 */
    const char *ptr = str;        /* ptr to current position in str */

    /* skip leading whitespace */
    while (isspace((int) *ptr))
        ptr++;
    /* printf("1) %s\n",ptr); */

    /* scan optional sign character */
    switch (*ptr)
    {
    case '+':
        ptr++;
        s = 1;
        break;
    case '-':
        ptr++;
        s = -1;
        break;
    default:
        s = 1;
        break;
    }
    /* printf("2) %s\n",ptr); */

    /* '0X' prefix */
    if ((*ptr++) != '0')
    {
        /* printf("failed to detect '0'\n"); */
        *endptr = str;
        return 0;
    }
    /* printf("3) %s\n",ptr); */
    if (toupper((int) *ptr++) != toupper((int) X))
    {
        /* printf("failed to detect '%c'\n",X); */
        *endptr = str;
        return 0;
    }
    /* printf("4) %s\n",ptr); */

    /* attempt conversion on remainder of string using strtol() */
    val = strtol(ptr, (char * *)endptr, base);
    if (*endptr == ptr)
    {
        /* conversion failed */
        *endptr = str;
        return 0;
    }

    /* success */
    return s * val;
}


/* Returns 1 if str matches suffix (case insensitive).    */
/* Str may contain trailing whitespace, but nothing else. */
static int detectsuffix(const char *str, const char *suffix)
{
    /* scan pairwise through strings until mismatch detected */
    while( toupper((int) *str) == toupper((int) *suffix) )
    {
        /* printf("'%c' '%c'\n", *str, *suffix); */

        /* return 1 (success) if match persists until the string terminator */
        if (*str == '\0')
            return 1;

        /* next chars */
        str++;
        suffix++;
    }
    /* printf("'%c' '%c' mismatch\n", *str, *suffix); */

    /* return 0 (fail) if the matching did not consume the entire suffix */
    if (*suffix != 0)
        return 0;   /* failed to consume entire suffix */

    /* skip any remaining whitespace in str */
    while (isspace((int) *str))
        str++;

    /* return 1 (success) if we have reached end of str else return 0 (fail) */
    return (*str == '\0') ? 1 : 0;
}


static int arg_int_scanfn(struct arg_int *parent, const char *argval)
{
    int errorcode = 0;

    if (parent->count == parent->hdr.maxcount)
    {
        /* maximum number of arguments exceeded */
        errorcode = EMAXCOUNT;
    }
    else if (!argval)
    {
        /* a valid argument with no argument value was given. */
        /* This happens when an optional argument value was invoked. */
        /* leave parent arguiment value unaltered but still count the argument. */
        parent->count++;
    }
    else
    {
        long int val;
        const char *end;

        /* attempt to extract hex integer (eg: +0x123) from argval into val conversion */
        val = strtol0X(argval, &end, 'X', 16);
        if (end == argval)
        {
            /* hex failed, attempt octal conversion (eg +0o123) */
            val = strtol0X(argval, &end, 'O', 8);
            if (end == argval)
            {
                /* octal failed, attempt binary conversion (eg +0B101) */
                val = strtol0X(argval, &end, 'B', 2);
                if (end == argval)
                {
                    /* binary failed, attempt decimal conversion with no prefix (eg 1234) */
                    val = strtol(argval, (char * *)&end, 10);
                    if (end == argval)
                    {
                        /* all supported number formats failed */
                        return EBADINT;
                    }
                }
            }
        }

        /* Safety check for integer overflow. WARNING: this check    */
        /* achieves nothing on machines where size(int)==size(long). */
        if ( val > INT_MAX || val < INT_MIN )
            errorcode = EOVERFLOW;

        /* Detect any suffixes (KB,MB,GB) and multiply argument value appropriately. */
        /* We need to be mindful of integer overflows when using such big numbers.   */
        if (detectsuffix(end, "KB"))             /* kilobytes */
        {
            if ( val > (INT_MAX / 1024) || val < (INT_MIN / 1024) )
                errorcode = EOVERFLOW;          /* Overflow would occur if we proceed */
            else
                val *= 1024;                    /* 1KB = 1024 */
        }
        else if (detectsuffix(end, "MB"))        /* megabytes */
        {
            if ( val > (INT_MAX / 1048576) || val < (INT_MIN / 1048576) )
                errorcode = EOVERFLOW;          /* Overflow would occur if we proceed */
            else
                val *= 1048576;                 /* 1MB = 1024*1024 */
        }
        else if (detectsuffix(end, "GB"))        /* gigabytes */
        {
            if ( val > (INT_MAX / 1073741824) || val < (INT_MIN / 1073741824) )
                errorcode = EOVERFLOW;          /* Overflow would occur if we proceed */
            else
                val *= 1073741824;              /* 1GB = 1024*1024*1024 */
        }
        else if (!detectsuffix(end, ""))
            errorcode = EBADINT;                /* invalid suffix detected */

        /* if success then store result in parent->ival[] array */
        if (errorcode == 0)
            parent->ival[parent->count++] = val;
    }

    /* printf("%s:scanfn(%p,%p) returns %d\n",__FILE__,parent,argval,errorcode); */
    return errorcode;
}


static int arg_int_checkfn(struct arg_int *parent)
{
    int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT : 0;
    /*printf("%s:checkfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
    return errorcode;
}


static void arg_int_errorfn(
    struct arg_int *parent,
    FILE *fp,
    int errorcode,
    const char *argval,
    const char *progname)
{
    const char *shortopts = parent->hdr.shortopts;
    const char *longopts  = parent->hdr.longopts;
    const char *datatype  = parent->hdr.datatype;

    /* make argval NULL safe */
    argval = argval ? argval : "";

    fprintf(fp, "%s: ", progname);
    switch(errorcode)
    {
    case EMINCOUNT:
        fputs("missing option ", fp);
        arg_print_option(fp, shortopts, longopts, datatype, "\n");
        break;

    case EMAXCOUNT:
        fputs("excess option ", fp);
        arg_print_option(fp, shortopts, longopts, argval, "\n");
        break;

    case EBADINT:
        fprintf(fp, "invalid argument \"%s\" to option ", argval);
        arg_print_option(fp, shortopts, longopts, datatype, "\n");
        break;

    case EOVERFLOW:
        fputs("integer overflow at option ", fp);
        arg_print_option(fp, shortopts, longopts, datatype, " ");
        fprintf(fp, "(%s is too large)\n", argval);
        break;
    }
}


struct arg_int * arg_int0(
    const char *shortopts,
    const char *longopts,
    const char *datatype,
    const char *glossary)
{
    return arg_intn(shortopts, longopts, datatype, 0, 1, glossary);
}


struct arg_int * arg_int1(
    const char *shortopts,
    const char *longopts,
    const char *datatype,
    const char *glossary)
{
    return arg_intn(shortopts, longopts, datatype, 1, 1, glossary);
}


struct arg_int * arg_intn(
    const char *shortopts,
    const char *longopts,
    const char *datatype,
    int mincount,
    int maxcount,
    const char *glossary)
{
    size_t nbytes;
    struct arg_int *result;

    /* foolproof things by ensuring maxcount is not less than mincount */
    maxcount = (maxcount < mincount) ? mincount : maxcount;

    nbytes = sizeof(struct arg_int)    /* storage for struct arg_int */
             + maxcount * sizeof(int); /* storage for ival[maxcount] array */

    result = (struct arg_int *)malloc(nbytes);
    if (result)
    {
        /* init the arg_hdr struct */
        result->hdr.flag      = ARG_HASVALUE;
        result->hdr.shortopts = shortopts;
        result->hdr.longopts  = longopts;
        result->hdr.datatype  = datatype ? datatype : "<int>";
        result->hdr.glossary  = glossary;
        result->hdr.mincount  = mincount;
        result->hdr.maxcount  = maxcount;
        result->hdr.parent    = result;
        result->hdr.resetfn   = (arg_resetfn *)arg_int_resetfn;
        result->hdr.scanfn    = (arg_scanfn *)arg_int_scanfn;
        result->hdr.checkfn   = (arg_checkfn *)arg_int_checkfn;
        result->hdr.errorfn   = (arg_errorfn *)arg_int_errorfn;

        /* store the ival[maxcount] array immediately after the arg_int struct */
        result->ival  = (int *)(result + 1);
        result->count = 0;
    }
    
    ARG_TRACE(("arg_intn() returns %p\n", result));
    return result;
}
/*******************************************************************************
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

#include <stdlib.h>

#include "argtable3.h"


static void arg_lit_resetfn(struct arg_lit *parent)
{
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}


static int arg_lit_scanfn(struct arg_lit *parent, const char *argval)
{
    int errorcode = 0;
    if (parent->count < parent->hdr.maxcount )
        parent->count++;
    else
        errorcode = EMAXCOUNT;

    ARG_TRACE(("%s:scanfn(%p,%s) returns %d\n", __FILE__, parent, argval,
               errorcode));
    return errorcode;
}


static int arg_lit_checkfn(struct arg_lit *parent)
{
    int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT : 0;
    ARG_TRACE(("%s:checkfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}


static void arg_lit_errorfn(
    struct arg_lit *parent,
    FILE *fp,
    int errorcode,
    const char *argval,
    const char *progname)
{
    const char *shortopts = parent->hdr.shortopts;
    const char *longopts  = parent->hdr.longopts;
    const char *datatype  = parent->hdr.datatype;

    switch(errorcode)
    {
    case EMINCOUNT:
        fprintf(fp, "%s: missing option ", progname);
        arg_print_option(fp, shortopts, longopts, datatype, "\n");
        fprintf(fp, "\n");
        break;

    case EMAXCOUNT:
        fprintf(fp, "%s: extraneous option ", progname);
        arg_print_option(fp, shortopts, longopts, datatype, "\n");
        break;
    }

    ARG_TRACE(("%s:errorfn(%p, %p, %d, %s, %s)\n", __FILE__, parent, fp,
               errorcode, argval, progname));
}


struct arg_lit * arg_lit0(
    const char * shortopts,
    const char * longopts,
    const char * glossary)
{
    return arg_litn(shortopts, longopts, 0, 1, glossary);
}


struct arg_lit * arg_lit1(
    const char *shortopts,
    const char *longopts,
    const char *glossary)
{
    return arg_litn(shortopts, longopts, 1, 1, glossary);
}


struct arg_lit * arg_litn(
    const char *shortopts,
    const char *longopts,
    int mincount,
    int maxcount,
    const char *glossary)
{
    struct arg_lit *result;

    /* foolproof things by ensuring maxcount is not less than mincount */
    maxcount = (maxcount < mincount) ? mincount : maxcount;

    result = (struct arg_lit *)malloc(sizeof(struct arg_lit));
    if (result)
    {
        /* init the arg_hdr struct */
        result->hdr.flag      = 0;
        result->hdr.shortopts = shortopts;
        result->hdr.longopts  = longopts;
        result->hdr.datatype  = NULL;
        result->hdr.glossary  = glossary;
        result->hdr.mincount  = mincount;
        result->hdr.maxcount  = maxcount;
        result->hdr.parent    = result;
        result->hdr.resetfn   = (arg_resetfn *)arg_lit_resetfn;
        result->hdr.scanfn    = (arg_scanfn *)arg_lit_scanfn;
        result->hdr.checkfn   = (arg_checkfn *)arg_lit_checkfn;
        result->hdr.errorfn   = (arg_errorfn *)arg_lit_errorfn;

        /* init local variables */
        result->count = 0;
    }
    
    ARG_TRACE(("arg_litn() returns %p\n", result));
    return result;
}
/*******************************************************************************
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

#include <stdlib.h>

#include "argtable3.h"

struct arg_rem *arg_rem(const char *datatype, const char *glossary)
{
    struct arg_rem *result = (struct arg_rem *)malloc(sizeof(struct arg_rem));
    if (result)
    {
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
    }

    ARG_TRACE(("arg_rem() returns %p\n", result));
    return result;
}

/*******************************************************************************
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

#include <stdlib.h>
#include <string.h>

#include "argtable3.h"


#ifndef _TREX_H_
#define _TREX_H_
/***************************************************************
	T-Rex a tiny regular expression library

	Copyright (C) 2003-2006 Alberto Demichelis

	This software is provided 'as-is', without any express
	or implied warranty. In no event will the authors be held
	liable for any damages arising from the use of this software.

	Permission is granted to anyone to use this software for
	any purpose, including commercial applications, and to alter
	it and redistribute it freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented;
		you must not claim that you wrote the original software.
		If you use this software in a product, an acknowledgment
		in the product documentation would be appreciated but
		is not required.

		2. Altered source versions must be plainly marked as such,
		and must not be misrepresented as being the original software.

		3. This notice may not be removed or altered from any
		source distribution.

****************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNICODE
#define TRexChar unsigned short
#define MAX_CHAR 0xFFFF
#define _TREXC(c) L##c
#define trex_strlen wcslen
#define trex_printf wprintf
#else
#define TRexChar char
#define MAX_CHAR 0xFF
#define _TREXC(c) (c)
#define trex_strlen strlen
#define trex_printf printf
#endif

#ifndef TREX_API
#define TREX_API extern
#endif

#define TRex_True 1
#define TRex_False 0

#define TREX_ICASE ARG_REX_ICASE

typedef unsigned int TRexBool;
typedef struct TRex TRex;

typedef struct {
	const TRexChar *begin;
	int len;
} TRexMatch;

TREX_API TRex *trex_compile(const TRexChar *pattern, const TRexChar **error, int flags);
TREX_API void trex_free(TRex *exp);
TREX_API TRexBool trex_match(TRex* exp, const TRexChar* text);
TREX_API TRexBool trex_search(TRex* exp, const TRexChar* text, const TRexChar** out_begin, const TRexChar** out_end);
TREX_API TRexBool trex_searchrange(TRex* exp, const TRexChar* text_begin, const TRexChar* text_end, const TRexChar** out_begin, const TRexChar** out_end);
TREX_API int trex_getsubexpcount(TRex* exp);
TREX_API TRexBool trex_getsubexp(TRex* exp, int n, TRexMatch *subexp);

#ifdef __cplusplus
}
#endif

#endif



struct privhdr
{
    const char *pattern;
    int flags;
};


static void arg_rex_resetfn(struct arg_rex *parent)
{
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}

static int arg_rex_scanfn(struct arg_rex *parent, const char *argval)
{
    int errorcode = 0;
    const TRexChar *error = NULL;
    TRex *rex = NULL;
    TRexBool is_match = TRex_False;

    if (parent->count == parent->hdr.maxcount )
    {
        /* maximum number of arguments exceeded */
        errorcode = EMAXCOUNT;
    }
    else if (!argval)
    {
        /* a valid argument with no argument value was given. */
        /* This happens when an optional argument value was invoked. */
        /* leave parent argument value unaltered but still count the argument. */
        parent->count++;
    }
    else
    {
        struct privhdr *priv = (struct privhdr *)parent->hdr.priv;

        /* test the current argument value for a match with the regular expression */
        /* if a match is detected, record the argument value in the arg_rex struct */

        rex = trex_compile(priv->pattern, &error, priv->flags);
        is_match = trex_match(rex, argval);
        if (!is_match)
            errorcode = EREGNOMATCH;
        else
            parent->sval[parent->count++] = argval;

        trex_free(rex);
    }

    ARG_TRACE(("%s:scanfn(%p) returns %d\n",__FILE__,parent,errorcode));
    return errorcode;
}

static int arg_rex_checkfn(struct arg_rex *parent)
{
    int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT : 0;
    //struct privhdr *priv = (struct privhdr*)parent->hdr.priv;

    /* free the regex "program" we constructed in resetfn */
    //regfree(&(priv->regex));

    /*printf("%s:checkfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
    return errorcode;
}

static void arg_rex_errorfn(struct arg_rex *parent,
                    FILE *fp,
                    int errorcode,
                    const char *argval,
                    const char *progname)
{
    const char *shortopts = parent->hdr.shortopts;
    const char *longopts  = parent->hdr.longopts;
    const char *datatype  = parent->hdr.datatype;

    /* make argval NULL safe */
    argval = argval ? argval : "";

    fprintf(fp, "%s: ", progname);
    switch(errorcode)
    {
    case EMINCOUNT:
        fputs("missing option ", fp);
        arg_print_option(fp, shortopts, longopts, datatype, "\n");
        break;

    case EMAXCOUNT:
        fputs("excess option ", fp);
        arg_print_option(fp, shortopts, longopts, argval, "\n");
        break;

    case EREGNOMATCH:
        fputs("illegal value  ", fp);
        arg_print_option(fp, shortopts, longopts, argval, "\n");
        break;

    default:
    {
        //char errbuff[256];
        //regerror(errorcode, NULL, errbuff, sizeof(errbuff));
        //printf("%s\n", errbuff);
    }
    break;
    }
}


struct arg_rex * arg_rex0(const char * shortopts,
                          const char * longopts,
                          const char * pattern,
                          const char *datatype,
                          int flags,
                          const char *glossary)
{
    return arg_rexn(shortopts,
                    longopts,
                    pattern,
                    datatype,
                    0,
                    1,
                    flags,
                    glossary);
}

struct arg_rex * arg_rex1(const char * shortopts,
                          const char * longopts,
                          const char * pattern,
                          const char *datatype,
                          int flags,
                          const char *glossary)
{
    return arg_rexn(shortopts,
                    longopts,
                    pattern,
                    datatype,
                    1,
                    1,
                    flags,
                    glossary);
}


struct arg_rex * arg_rexn(const char * shortopts,
                          const char * longopts,
                          const char * pattern,
                          const char *datatype,
                          int mincount,
                          int maxcount,
                          int flags,
                          const char *glossary)
{
    size_t nbytes;
    struct arg_rex *result;
    struct privhdr *priv;
    int i;
    const TRexChar *error = NULL;
    TRex *rex = NULL;

    if (!pattern)
    {
        printf(
            "argtable: ERROR - illegal regular expression pattern \"(NULL)\"\n");
        printf("argtable: Bad argument table.\n");
        return NULL;
    }

    /* foolproof things by ensuring maxcount is not less than mincount */
    maxcount = (maxcount < mincount) ? mincount : maxcount;

    nbytes = sizeof(struct arg_rex)       /* storage for struct arg_rex */
             + sizeof(struct privhdr)     /* storage for private arg_rex data */
             + maxcount * sizeof(char *);  /* storage for sval[maxcount] array */

    result = (struct arg_rex *)malloc(nbytes);
    if (result == NULL)
        return result;

    /* init the arg_hdr struct */
    result->hdr.flag      = ARG_HASVALUE;
    result->hdr.shortopts = shortopts;
    result->hdr.longopts  = longopts;
    result->hdr.datatype  = datatype ? datatype : pattern;
    result->hdr.glossary  = glossary;
    result->hdr.mincount  = mincount;
    result->hdr.maxcount  = maxcount;
    result->hdr.parent    = result;
    result->hdr.resetfn   = (arg_resetfn *)arg_rex_resetfn;
    result->hdr.scanfn    = (arg_scanfn *)arg_rex_scanfn;
    result->hdr.checkfn   = (arg_checkfn *)arg_rex_checkfn;
    result->hdr.errorfn   = (arg_errorfn *)arg_rex_errorfn;

    /* store the arg_rex_priv struct immediately after the arg_rex struct */
    result->hdr.priv  = result + 1;
    priv = (struct privhdr *)(result->hdr.priv);
    priv->pattern = pattern;
    priv->flags = flags;

    /* store the sval[maxcount] array immediately after the arg_rex_priv struct */
    result->sval  = (const char * *)(priv + 1);
    result->count = 0;

    /* foolproof the string pointers by initializing them to reference empty strings */
    for (i = 0; i < maxcount; i++)
        result->sval[i] = "";

    /* here we construct and destroy a regex representation of the regular
     * expression for no other reason than to force any regex errors to be
     * trapped now rather than later. If we don't, then errors may go undetected
     * until an argument is actually parsed.
     */

    rex = trex_compile(priv->pattern, &error, priv->flags);
    if (rex == NULL)
    {
        ARG_LOG(("argtable: %s \"%s\"\n", error ? error : _TREXC("undefined"), priv->pattern));
        ARG_LOG(("argtable: Bad argument table.\n"));
    }

    trex_free(rex);

    ARG_TRACE(("arg_rexn() returns %p\n", result));
    return result;
}



/* see copyright notice in trex.h */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <setjmp.h>

#ifdef _UINCODE
#define scisprint iswprint
#define scstrlen wcslen
#define scprintf wprintf
#define _SC(x) L(x)
#else
#define scisprint isprint
#define scstrlen strlen
#define scprintf printf
#define _SC(x) (x)
#endif

#ifdef _DEBUG
#include <stdio.h>

static const TRexChar *g_nnames[] =
{
	_SC("NONE"),_SC("OP_GREEDY"),	_SC("OP_OR"),
	_SC("OP_EXPR"),_SC("OP_NOCAPEXPR"),_SC("OP_DOT"),	_SC("OP_CLASS"),
	_SC("OP_CCLASS"),_SC("OP_NCLASS"),_SC("OP_RANGE"),_SC("OP_CHAR"),
	_SC("OP_EOL"),_SC("OP_BOL"),_SC("OP_WB")
};

#endif
#define OP_GREEDY		(MAX_CHAR+1) // * + ? {n}
#define OP_OR			(MAX_CHAR+2)
#define OP_EXPR			(MAX_CHAR+3) //parentesis ()
#define OP_NOCAPEXPR	(MAX_CHAR+4) //parentesis (?:)
#define OP_DOT			(MAX_CHAR+5)
#define OP_CLASS		(MAX_CHAR+6)
#define OP_CCLASS		(MAX_CHAR+7)
#define OP_NCLASS		(MAX_CHAR+8) //negates class the [^
#define OP_RANGE		(MAX_CHAR+9)
#define OP_CHAR			(MAX_CHAR+10)
#define OP_EOL			(MAX_CHAR+11)
#define OP_BOL			(MAX_CHAR+12)
#define OP_WB			(MAX_CHAR+13)

#define TREX_SYMBOL_ANY_CHAR ('.')
#define TREX_SYMBOL_GREEDY_ONE_OR_MORE ('+')
#define TREX_SYMBOL_GREEDY_ZERO_OR_MORE ('*')
#define TREX_SYMBOL_GREEDY_ZERO_OR_ONE ('?')
#define TREX_SYMBOL_BRANCH ('|')
#define TREX_SYMBOL_END_OF_STRING ('$')
#define TREX_SYMBOL_BEGINNING_OF_STRING ('^')
#define TREX_SYMBOL_ESCAPE_CHAR ('\\')


typedef int TRexNodeType;

typedef struct tagTRexNode{
	TRexNodeType type;
	int left;
	int right;
	int next;
}TRexNode;

struct TRex{
	const TRexChar *_eol;
	const TRexChar *_bol;
	const TRexChar *_p;
	int _first;
	int _op;
	TRexNode *_nodes;
	int _nallocated;
	int _nsize;
	int _nsubexpr;
	TRexMatch *_matches;
	int _currsubexp;
	void *_jmpbuf;
	const TRexChar **_error;
	int _flags;
};

static int trex_list(TRex *exp);

static int trex_newnode(TRex *exp, TRexNodeType type)
{
	TRexNode n;
	int newid;
	n.type = type;
	n.next = n.right = n.left = -1;
	if(type == OP_EXPR)
		n.right = exp->_nsubexpr++;
	if(exp->_nallocated < (exp->_nsize + 1)) {
		exp->_nallocated *= 2;
		exp->_nodes = (TRexNode *)realloc(exp->_nodes, exp->_nallocated * sizeof(TRexNode));
	}
	exp->_nodes[exp->_nsize++] = n;
	newid = exp->_nsize - 1;
	return (int)newid;
}

static void trex_error(TRex *exp,const TRexChar *error)
{
	if(exp->_error) *exp->_error = error;
	longjmp(*((jmp_buf*)exp->_jmpbuf),-1);
}

static void trex_expect(TRex *exp, int n){
	if((*exp->_p) != n)
		trex_error(exp, _SC("expected paren"));
	exp->_p++;
}

static TRexChar trex_escapechar(TRex *exp)
{
	if(*exp->_p == TREX_SYMBOL_ESCAPE_CHAR){
		exp->_p++;
		switch(*exp->_p) {
		case 'v': exp->_p++; return '\v';
		case 'n': exp->_p++; return '\n';
		case 't': exp->_p++; return '\t';
		case 'r': exp->_p++; return '\r';
		case 'f': exp->_p++; return '\f';
		default: return (*exp->_p++);
		}
	} else if(!scisprint((int) *exp->_p)) trex_error(exp,_SC("letter expected"));
	return (*exp->_p++);
}

static int trex_charclass(TRex *exp,int classid)
{
	int n = trex_newnode(exp,OP_CCLASS);
	exp->_nodes[n].left = classid;
	return n;
}

static int trex_charnode(TRex *exp,TRexBool isclass)
{
	TRexChar t;
	if(*exp->_p == TREX_SYMBOL_ESCAPE_CHAR) {
		exp->_p++;
		switch(*exp->_p) {
			case 'n': exp->_p++; return trex_newnode(exp,'\n');
			case 't': exp->_p++; return trex_newnode(exp,'\t');
			case 'r': exp->_p++; return trex_newnode(exp,'\r');
			case 'f': exp->_p++; return trex_newnode(exp,'\f');
			case 'v': exp->_p++; return trex_newnode(exp,'\v');
			case 'a': case 'A': case 'w': case 'W': case 's': case 'S':
			case 'd': case 'D': case 'x': case 'X': case 'c': case 'C':
			case 'p': case 'P': case 'l': case 'u':
				{
				t = *exp->_p; exp->_p++;
				return trex_charclass(exp,t);
				}
			case 'b':
			case 'B':
				if(!isclass) {
					int node = trex_newnode(exp,OP_WB);
					exp->_nodes[node].left = *exp->_p;
					exp->_p++;
					return node;
				} //else default
			default:
				t = *exp->_p; exp->_p++;
				return trex_newnode(exp,t);
		}
	}
	else if(!scisprint((int) *exp->_p)) {

		trex_error(exp,_SC("letter expected"));
	}
	t = *exp->_p; exp->_p++;
	return trex_newnode(exp,t);
}
static int trex_class(TRex *exp)
{
	int ret = -1;
	int first = -1,chain;
	if(*exp->_p == TREX_SYMBOL_BEGINNING_OF_STRING){
		ret = trex_newnode(exp,OP_NCLASS);
		exp->_p++;
	}else ret = trex_newnode(exp,OP_CLASS);

	if(*exp->_p == ']') trex_error(exp,_SC("empty class"));
	chain = ret;
	while(*exp->_p != ']' && exp->_p != exp->_eol) {
		if(*exp->_p == '-' && first != -1){
			int r,t;
			if(*exp->_p++ == ']') trex_error(exp,_SC("unfinished range"));
			r = trex_newnode(exp,OP_RANGE);
			if(first>*exp->_p) trex_error(exp,_SC("invalid range"));
			if(exp->_nodes[first].type == OP_CCLASS) trex_error(exp,_SC("cannot use character classes in ranges"));
			exp->_nodes[r].left = exp->_nodes[first].type;
			t = trex_escapechar(exp);
			exp->_nodes[r].right = t;
            exp->_nodes[chain].next = r;
			chain = r;
			first = -1;
		}
		else{
			if(first!=-1){
				int c = first;
				exp->_nodes[chain].next = c;
				chain = c;
				first = trex_charnode(exp,TRex_True);
			}
			else{
				first = trex_charnode(exp,TRex_True);
			}
		}
	}
	if(first!=-1){
		int c = first;
		exp->_nodes[chain].next = c;
		chain = c;
		first = -1;
	}
	/* hack? */
	exp->_nodes[ret].left = exp->_nodes[ret].next;
	exp->_nodes[ret].next = -1;
	return ret;
}

static int trex_parsenumber(TRex *exp)
{
	int ret = *exp->_p-'0';
	int positions = 10;
	exp->_p++;
	while(isdigit((int) *exp->_p)) {
		ret = ret*10+(*exp->_p++-'0');
		if(positions==1000000000) trex_error(exp,_SC("overflow in numeric constant"));
		positions *= 10;
	};
	return ret;
}

static int trex_element(TRex *exp)
{
	int ret = -1;
	switch(*exp->_p)
	{
	case '(': {
		int expr,newn;
		exp->_p++;


		if(*exp->_p =='?') {
			exp->_p++;
			trex_expect(exp,':');
			expr = trex_newnode(exp,OP_NOCAPEXPR);
		}
		else
			expr = trex_newnode(exp,OP_EXPR);
		newn = trex_list(exp);
		exp->_nodes[expr].left = newn;
		ret = expr;
		trex_expect(exp,')');
			  }
			  break;
	case '[':
		exp->_p++;
		ret = trex_class(exp);
		trex_expect(exp,']');
		break;
	case TREX_SYMBOL_END_OF_STRING: exp->_p++; ret = trex_newnode(exp,OP_EOL);break;
	case TREX_SYMBOL_ANY_CHAR: exp->_p++; ret = trex_newnode(exp,OP_DOT);break;
	default:
		ret = trex_charnode(exp,TRex_False);
		break;
	}

	{
		TRexBool isgreedy = TRex_False;
		unsigned short p0 = 0, p1 = 0;
		switch(*exp->_p){
			case TREX_SYMBOL_GREEDY_ZERO_OR_MORE: p0 = 0; p1 = 0xFFFF; exp->_p++; isgreedy = TRex_True; break;
			case TREX_SYMBOL_GREEDY_ONE_OR_MORE: p0 = 1; p1 = 0xFFFF; exp->_p++; isgreedy = TRex_True; break;
			case TREX_SYMBOL_GREEDY_ZERO_OR_ONE: p0 = 0; p1 = 1; exp->_p++; isgreedy = TRex_True; break;
			case '{':
				exp->_p++;
				if(!isdigit((int) *exp->_p)) trex_error(exp,_SC("number expected"));
				p0 = (unsigned short)trex_parsenumber(exp);
				/*******************************/
				switch(*exp->_p) {
			case '}':
				p1 = p0; exp->_p++;
				break;
			case ',':
				exp->_p++;
				p1 = 0xFFFF;
				if(isdigit((int) *exp->_p)){
					p1 = (unsigned short)trex_parsenumber(exp);
				}
				trex_expect(exp,'}');
				break;
			default:
				trex_error(exp,_SC(", or } expected"));
		}
		/*******************************/
		isgreedy = TRex_True;
		break;

		}
		if(isgreedy) {
			int nnode = trex_newnode(exp,OP_GREEDY);
			exp->_nodes[nnode].left = ret;
			exp->_nodes[nnode].right = ((p0)<<16)|p1;
			ret = nnode;
		}
	}
	if((*exp->_p != TREX_SYMBOL_BRANCH) && (*exp->_p != ')') && (*exp->_p != TREX_SYMBOL_GREEDY_ZERO_OR_MORE) && (*exp->_p != TREX_SYMBOL_GREEDY_ONE_OR_MORE) && (*exp->_p != '\0')) {
		int nnode = trex_element(exp);
		exp->_nodes[ret].next = nnode;
	}

	return ret;
}

static int trex_list(TRex *exp)
{
	int ret=-1,e;
	if(*exp->_p == TREX_SYMBOL_BEGINNING_OF_STRING) {
		exp->_p++;
		ret = trex_newnode(exp,OP_BOL);
	}
	e = trex_element(exp);
	if(ret != -1) {
		exp->_nodes[ret].next = e;
	}
	else ret = e;

	if(*exp->_p == TREX_SYMBOL_BRANCH) {
		int temp,tright;
		exp->_p++;
		temp = trex_newnode(exp,OP_OR);
		exp->_nodes[temp].left = ret;
		tright = trex_list(exp);
		exp->_nodes[temp].right = tright;
		ret = temp;
	}
	return ret;
}

static TRexBool trex_matchcclass(int cclass,TRexChar ch)
{
    int c = ch;
	switch(cclass) {
	case 'a': return isalpha(c)?TRex_True:TRex_False;
	case 'A': return !isalpha(c)?TRex_True:TRex_False;
	case 'w': return (isalnum(c) || c == '_')?TRex_True:TRex_False;
	case 'W': return (!isalnum(c) && c != '_')?TRex_True:TRex_False;
	case 's': return isspace(c)?TRex_True:TRex_False;
	case 'S': return !isspace(c)?TRex_True:TRex_False;
	case 'd': return isdigit(c)?TRex_True:TRex_False;
	case 'D': return !isdigit(c)?TRex_True:TRex_False;
	case 'x': return isxdigit(c)?TRex_True:TRex_False;
	case 'X': return !isxdigit(c)?TRex_True:TRex_False;
	case 'c': return iscntrl(c)?TRex_True:TRex_False;
	case 'C': return !iscntrl(c)?TRex_True:TRex_False;
	case 'p': return ispunct(c)?TRex_True:TRex_False;
	case 'P': return !ispunct(c)?TRex_True:TRex_False;
	case 'l': return islower(c)?TRex_True:TRex_False;
	case 'u': return isupper(c)?TRex_True:TRex_False;
	}
	return TRex_False; /*cannot happen*/
}

static TRexBool trex_matchclass(TRex* exp,TRexNode *node,TRexChar c)
{
	do {
		switch(node->type) {
			case OP_RANGE:
				if (exp->_flags & TREX_ICASE)
				{
					if(c >= toupper(node->left) && c <= toupper(node->right)) return TRex_True;
					if(c >= tolower(node->left) && c <= tolower(node->right)) return TRex_True;
				}
				else
				{
					if(c >= node->left && c <= node->right) return TRex_True;
				}
				break;
			case OP_CCLASS:
				if(trex_matchcclass(node->left,c)) return TRex_True;
				break;
			default:
				if (exp->_flags & TREX_ICASE)
				{
					if (c == tolower(node->type) || c == toupper(node->type)) return TRex_True;
				}
				else
				{
					if(c == node->type)return TRex_True;
				}

		}
	} while((node->next != -1) && (node = &exp->_nodes[node->next]));
	return TRex_False;
}

static const TRexChar *trex_matchnode(TRex* exp,TRexNode *node,const TRexChar *str,TRexNode *next)
{

	TRexNodeType type = node->type;
	switch(type) {
	case OP_GREEDY: {
		//TRexNode *greedystop = (node->next != -1) ? &exp->_nodes[node->next] : NULL;
		TRexNode *greedystop = NULL;
		int p0 = (node->right >> 16)&0x0000FFFF, p1 = node->right&0x0000FFFF, nmaches = 0;
		const TRexChar *s=str, *good = str;

		if(node->next != -1) {
			greedystop = &exp->_nodes[node->next];
		}
		else {
			greedystop = next;
		}

		while((nmaches == 0xFFFF || nmaches < p1)) {

			const TRexChar *stop;
			if(!(s = trex_matchnode(exp,&exp->_nodes[node->left],s,greedystop)))
				break;
			nmaches++;
			good=s;
			if(greedystop) {
				//checks that 0 matches satisfy the expression(if so skips)
				//if not would always stop(for instance if is a '?')
				if(greedystop->type != OP_GREEDY ||
				(greedystop->type == OP_GREEDY && ((greedystop->right >> 16)&0x0000FFFF) != 0))
				{
					TRexNode *gnext = NULL;
					if(greedystop->next != -1) {
						gnext = &exp->_nodes[greedystop->next];
					}else if(next && next->next != -1){
						gnext = &exp->_nodes[next->next];
					}
					stop = trex_matchnode(exp,greedystop,s,gnext);
					if(stop) {
						//if satisfied stop it
						if(p0 == p1 && p0 == nmaches) break;
						else if(nmaches >= p0 && p1 == 0xFFFF) break;
						else if(nmaches >= p0 && nmaches <= p1) break;
					}
				}
			}

			if(s >= exp->_eol)
				break;
		}
		if(p0 == p1 && p0 == nmaches) return good;
		else if(nmaches >= p0 && p1 == 0xFFFF) return good;
		else if(nmaches >= p0 && nmaches <= p1) return good;
		return NULL;
	}
	case OP_OR: {
			const TRexChar *asd = str;
			TRexNode *temp=&exp->_nodes[node->left];
			while( (asd = trex_matchnode(exp,temp,asd,NULL)) ) {
				if(temp->next != -1)
					temp = &exp->_nodes[temp->next];
				else
					return asd;
			}
			asd = str;
			temp = &exp->_nodes[node->right];
			while( (asd = trex_matchnode(exp,temp,asd,NULL)) ) {
				if(temp->next != -1)
					temp = &exp->_nodes[temp->next];
				else
					return asd;
			}
			return NULL;
			break;
	}
	case OP_EXPR:
	case OP_NOCAPEXPR:{
			TRexNode *n = &exp->_nodes[node->left];
			const TRexChar *cur = str;
			int capture = -1;
			if(node->type != OP_NOCAPEXPR && node->right == exp->_currsubexp) {
				capture = exp->_currsubexp;
				exp->_matches[capture].begin = cur;
				exp->_currsubexp++;
			}

			do {
				TRexNode *subnext = NULL;
				if(n->next != -1) {
					subnext = &exp->_nodes[n->next];
				}else {
					subnext = next;
				}
				if(!(cur = trex_matchnode(exp,n,cur,subnext))) {
					if(capture != -1){
						exp->_matches[capture].begin = 0;
						exp->_matches[capture].len = 0;
					}
					return NULL;
				}
			} while((n->next != -1) && (n = &exp->_nodes[n->next]));

			if(capture != -1)
				exp->_matches[capture].len = cur - exp->_matches[capture].begin;
			return cur;
	}
	case OP_WB:
		if((str == exp->_bol && !isspace((int) *str))
		 || ((str == exp->_eol && !isspace((int) *(str-1))))
		 || ((!isspace((int) *str) && isspace((int) *(str+1))))
		 || ((isspace((int) *str) && !isspace((int) *(str+1)))) ) {
			return (node->left == 'b')?str:NULL;
		}
		return (node->left == 'b')?NULL:str;
	case OP_BOL:
		if(str == exp->_bol) return str;
		return NULL;
	case OP_EOL:
		if(str == exp->_eol) return str;
		return NULL;
	case OP_DOT:
		str++;
		return str;
	case OP_NCLASS:
	case OP_CLASS:
		if(trex_matchclass(exp,&exp->_nodes[node->left],*str)?(type == OP_CLASS?TRex_True:TRex_False):(type == OP_NCLASS?TRex_True:TRex_False)) {
                        str++;
			return str;
		}
		return NULL;
	case OP_CCLASS:
		if(trex_matchcclass(node->left,*str)) {
                        str++;
			return str;
		}
		return NULL;
	default: /* char */
		if (exp->_flags & TREX_ICASE)
		{
			if(*str != tolower(node->type) && *str != toupper(node->type)) return NULL;
		}
		else
		{
			if (*str != node->type) return NULL;
		}
		str++;
		return str;
	}
	return NULL;
}

/* public api */
TRex *trex_compile(const TRexChar *pattern,const TRexChar **error,int flags)
{
	TRex *exp = (TRex *)malloc(sizeof(TRex));
	exp->_eol = exp->_bol = NULL;
	exp->_p = pattern;
	exp->_nallocated = (int)scstrlen(pattern) * sizeof(TRexChar);
	exp->_nodes = (TRexNode *)malloc(exp->_nallocated * sizeof(TRexNode));
	exp->_nsize = 0;
	exp->_matches = 0;
	exp->_nsubexpr = 0;
	exp->_first = trex_newnode(exp,OP_EXPR);
	exp->_error = error;
	exp->_jmpbuf = malloc(sizeof(jmp_buf));
	exp->_flags = flags;
	if(setjmp(*((jmp_buf*)exp->_jmpbuf)) == 0) {
		int res = trex_list(exp);
		exp->_nodes[exp->_first].left = res;
		if(*exp->_p!='\0')
			trex_error(exp,_SC("unexpected character"));
#ifdef _DEBUG
		{
			int nsize,i;
			TRexNode *t;
			nsize = exp->_nsize;
			t = &exp->_nodes[0];
			scprintf(_SC("\n"));
			for(i = 0;i < nsize; i++) {
				if(exp->_nodes[i].type>MAX_CHAR)
					scprintf(_SC("[%02d] %10s "),i,g_nnames[exp->_nodes[i].type-MAX_CHAR]);
				else
					scprintf(_SC("[%02d] %10c "),i,exp->_nodes[i].type);
				scprintf(_SC("left %02d right %02d next %02d\n"),exp->_nodes[i].left,exp->_nodes[i].right,exp->_nodes[i].next);
			}
			scprintf(_SC("\n"));
		}
#endif
		exp->_matches = (TRexMatch *) malloc(exp->_nsubexpr * sizeof(TRexMatch));
		memset(exp->_matches,0,exp->_nsubexpr * sizeof(TRexMatch));
	}
	else{
		trex_free(exp);
		return NULL;
	}
	return exp;
}

void trex_free(TRex *exp)
{
	if(exp)	{
		if(exp->_nodes) free(exp->_nodes);
		if(exp->_jmpbuf) free(exp->_jmpbuf);
		if(exp->_matches) free(exp->_matches);
		free(exp);
	}
}

TRexBool trex_match(TRex* exp,const TRexChar* text)
{
	const TRexChar* res = NULL;
	exp->_bol = text;
	exp->_eol = text + scstrlen(text);
	exp->_currsubexp = 0;
	res = trex_matchnode(exp,exp->_nodes,text,NULL);
	if(res == NULL || res != exp->_eol)
		return TRex_False;
	return TRex_True;
}

TRexBool trex_searchrange(TRex* exp,const TRexChar* text_begin,const TRexChar* text_end,const TRexChar** out_begin, const TRexChar** out_end)
{
	const TRexChar *cur = NULL;
	int node = exp->_first;
	if(text_begin >= text_end) return TRex_False;
	exp->_bol = text_begin;
	exp->_eol = text_end;
	do {
		cur = text_begin;
		while(node != -1) {
			exp->_currsubexp = 0;
			cur = trex_matchnode(exp,&exp->_nodes[node],cur,NULL);
			if(!cur)
				break;
			node = exp->_nodes[node].next;
		}
		text_begin++;
	} while(cur == NULL && text_begin != text_end);

	if(cur == NULL)
		return TRex_False;

	--text_begin;

	if(out_begin) *out_begin = text_begin;
	if(out_end) *out_end = cur;
	return TRex_True;
}

TRexBool trex_search(TRex* exp,const TRexChar* text, const TRexChar** out_begin, const TRexChar** out_end)
{
	return trex_searchrange(exp,text,text + scstrlen(text),out_begin,out_end);
}

int trex_getsubexpcount(TRex* exp)
{
	return exp->_nsubexpr;
}

TRexBool trex_getsubexp(TRex* exp, int n, TRexMatch *subexp)
{
	if( n<0 || n >= exp->_nsubexpr) return TRex_False;
	*subexp = exp->_matches[n];
	return TRex_True;
}
/*******************************************************************************
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

#include <stdlib.h>

#include "argtable3.h"


static void arg_str_resetfn(struct arg_str *parent)
{
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}


static int arg_str_scanfn(struct arg_str *parent, const char *argval)
{
    int errorcode = 0;

    if (parent->count == parent->hdr.maxcount)
    {
        /* maximum number of arguments exceeded */
        errorcode = EMAXCOUNT;
    }
    else if (!argval)
    {
        /* a valid argument with no argument value was given. */
        /* This happens when an optional argument value was invoked. */
        /* leave parent arguiment value unaltered but still count the argument. */
        parent->count++;
    }
    else
    {
        parent->sval[parent->count++] = argval;
    }

    ARG_TRACE(("%s:scanfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}


static int arg_str_checkfn(struct arg_str *parent)
{
    int errorcode = (parent->count < parent->hdr.mincount) ? EMINCOUNT : 0;
    
    ARG_TRACE(("%s:checkfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}


static void arg_str_errorfn(
    struct arg_str *parent,
    FILE *fp,
    int errorcode,
    const char *argval,
    const char *progname)
{
    const char *shortopts = parent->hdr.shortopts;
    const char *longopts  = parent->hdr.longopts;
    const char *datatype  = parent->hdr.datatype;

    /* make argval NULL safe */
    argval = argval ? argval : "";

    fprintf(fp, "%s: ", progname);
    switch(errorcode)
    {
    case EMINCOUNT:
        fputs("missing option ", fp);
        arg_print_option(fp, shortopts, longopts, datatype, "\n");
        break;

    case EMAXCOUNT:
        fputs("excess option ", fp);
        arg_print_option(fp, shortopts, longopts, argval, "\n");
        break;
    }
}


struct arg_str * arg_str0(
    const char *shortopts,
    const char *longopts,
    const char *datatype,
    const char *glossary)
{
    return arg_strn(shortopts, longopts, datatype, 0, 1, glossary);
}


struct arg_str * arg_str1(
    const char *shortopts,
    const char *longopts,
    const char *datatype,
    const char *glossary)
{
    return arg_strn(shortopts, longopts, datatype, 1, 1, glossary);
}


struct arg_str * arg_strn(
    const char *shortopts,
    const char *longopts,
    const char *datatype,
    int mincount,
    int maxcount,
    const char *glossary)
{
    size_t nbytes;
    struct arg_str *result;

    /* should not allow this stupid error */
    /* we should return an error code warning this logic error */
    /* foolproof things by ensuring maxcount is not less than mincount */
    maxcount = (maxcount < mincount) ? mincount : maxcount;

    nbytes = sizeof(struct arg_str)     /* storage for struct arg_str */
             + maxcount * sizeof(char *); /* storage for sval[maxcount] array */

    result = (struct arg_str *)malloc(nbytes);
    if (result)
    {
        int i;

        /* init the arg_hdr struct */
        result->hdr.flag      = ARG_HASVALUE;
        result->hdr.shortopts = shortopts;
        result->hdr.longopts  = longopts;
        result->hdr.datatype  = datatype ? datatype : "<string>";
        result->hdr.glossary  = glossary;
        result->hdr.mincount  = mincount;
        result->hdr.maxcount  = maxcount;
        result->hdr.parent    = result;
        result->hdr.resetfn   = (arg_resetfn *)arg_str_resetfn;
        result->hdr.scanfn    = (arg_scanfn *)arg_str_scanfn;
        result->hdr.checkfn   = (arg_checkfn *)arg_str_checkfn;
        result->hdr.errorfn   = (arg_errorfn *)arg_str_errorfn;

        /* store the sval[maxcount] array immediately after the arg_str struct */
        result->sval  = (const char * *)(result + 1);
        result->count = 0;

        /* foolproof the string pointers by initialising them to reference empty strings */
        for (i = 0; i < maxcount; i++)
            result->sval[i] = "";
    }
    
    ARG_TRACE(("arg_strn() returns %p\n", result));
    return result;
}
/*******************************************************************************
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

#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "argtable3.h"

static
void arg_register_error(struct arg_end *end,
                        void *parent,
                        int error,
                        const char *argval)
{
    /* printf("arg_register_error(%p,%p,%d,%s)\n",end,parent,error,argval); */
    if (end->count < end->hdr.maxcount)
    {
        end->error[end->count] = error;
        end->parent[end->count] = parent;
        end->argval[end->count] = argval;
        end->count++;
    }
    else
    {
        end->error[end->hdr.maxcount - 1]  = ARG_ELIMIT;
        end->parent[end->hdr.maxcount - 1] = end;
        end->argval[end->hdr.maxcount - 1] = NULL;
    }
}


/*
 * Return index of first table entry with a matching short option
 * or -1 if no match was found.
 */
static
int find_shortoption(struct arg_hdr * *table, char shortopt)
{
    int tabindex;
    for(tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++)
    {
        if (table[tabindex]->shortopts &&
            strchr(table[tabindex]->shortopts, shortopt))
            return tabindex;
    }
    return -1;
}


struct longoptions
{
    int getoptval;
    int noptions;
    struct option *options;
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

static
struct longoptions * alloc_longoptions(struct arg_hdr * *table)
{
    struct longoptions *result;
    size_t nbytes;
    int noptions = 1;
    size_t longoptlen = 0;
    int tabindex;

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
    do
    {
        const char *longopts = table[tabindex]->longopts;
        longoptlen += (longopts ? strlen(longopts) : 0) + 1;
        while (longopts)
        {
            noptions++;
            longopts = strchr(longopts + 1, ',');
        }
    } while(!(table[tabindex++]->flag & ARG_TERMINATOR));
    /*printf("%d long options consuming %d chars in total\n",noptions,longoptlen);*/


    /* allocate storage for return data structure as: */
    /* (struct longoptions) + (struct options)[noptions] + char[longoptlen] */
    nbytes = sizeof(struct longoptions)
             + sizeof(struct option) * noptions
             + longoptlen;
    result = (struct longoptions *)malloc(nbytes);
    if (result)
    {
        int option_index = 0;
        char *store;

        result->getoptval = 0;
        result->noptions = noptions;
        result->options = (struct option *)(result + 1);
        store = (char *)(result->options + noptions);

        for(tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++)
        {
            const char *longopts = table[tabindex]->longopts;

            while(longopts && *longopts)
            {
                char *storestart = store;

                /* copy progressive longopt strings into the store */
                while (*longopts != 0 && *longopts != ',')
                    *store++ = *longopts++;
                *store++ = 0;
                if (*longopts == ',')
                    longopts++;
                /*fprintf(stderr,"storestart=\"%s\"\n",storestart);*/

                result->options[option_index].name    = storestart;
                result->options[option_index].flag    = &(result->getoptval);
                result->options[option_index].val     = tabindex;
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
        result->options[option_index].name    = 0;
        result->options[option_index].has_arg = 0;
        result->options[option_index].flag    = 0;
        result->options[option_index].val     = 0;
    }

    /*dump_longoptions(result);*/
    return result;
}

static
char * alloc_shortoptions(struct arg_hdr * *table)
{
    char *result;
    size_t len = 2;
    int tabindex;

    /* determine the total number of option chars required */
    for(tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++)
    {
        struct arg_hdr *hdr = table[tabindex];
        len += 3 * (hdr->shortopts ? strlen(hdr->shortopts) : 0);
    }

    result = malloc(len);
    if (result)
    {
        char *res = result;

        /* add a leading ':' so getopt return codes distinguish    */
        /* unrecognised option and options missing argument values */
        *res++ = ':';

        for(tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++)
        {
            struct arg_hdr *hdr = table[tabindex];
            const char *shortopts = hdr->shortopts;
            while(shortopts && *shortopts)
            {
                *res++ = *shortopts++;
                if (hdr->flag & ARG_HASVALUE)
                    *res++ = ':';
                if (hdr->flag & ARG_HASOPTVALUE)
                    *res++ = ':';
            }
        }
        /* null terminate the string */
        *res = 0;
    }

    /*printf("alloc_shortoptions() returns \"%s\"\n",(result?result:"NULL"));*/
    return result;
}


/* return index of the table terminator entry */
static
int arg_endindex(struct arg_hdr * *table)
{
    int tabindex = 0;
    while (!(table[tabindex]->flag & ARG_TERMINATOR))
        tabindex++;
    return tabindex;
}


static
void arg_parse_tagged(int argc,
                      char * *argv,
                      struct arg_hdr * *table,
                      struct arg_end *endtable)
{
    struct longoptions *longoptions;
    char *shortoptions;
    int copt;

    /*printf("arg_parse_tagged(%d,%p,%p,%p)\n",argc,argv,table,endtable);*/

    /* allocate short and long option arrays for the given opttable[].   */
    /* if the allocs fail then put an error msg in the last table entry. */
    longoptions  = alloc_longoptions(table);
    shortoptions = alloc_shortoptions(table);
    if (!longoptions || !shortoptions)
    {
        /* one or both memory allocs failed */
        arg_register_error(endtable, endtable, ARG_EMALLOC, NULL);
        /* free anything that was allocated (this is null safe) */
        free(shortoptions);
        free(longoptions);
        return;
    }

    /*dump_longoptions(longoptions);*/

    /* reset getopts internal option-index to zero, and disable error reporting */
    optind = 0;
    opterr = 0;

    /* fetch and process args using getopt_long */
    while( (copt =
                getopt_long(argc, argv, shortoptions, longoptions->options,
                            NULL)) != -1)
    {
        /*
           printf("optarg='%s'\n",optarg);
           printf("optind=%d\n",optind);
           printf("copt=%c\n",(char)copt);
           printf("optopt=%c (%d)\n",optopt, (int)(optopt));
         */
        switch(copt)
        {
        case 0:
        {
            int tabindex = longoptions->getoptval;
            void *parent  = table[tabindex]->parent;
            /*printf("long option detected from argtable[%d]\n", tabindex);*/
            if (optarg && optarg[0] == 0 &&
                (table[tabindex]->flag & ARG_HASVALUE))
            {
                /* printf(": long option %s requires an argument\n",argv[optind-1]); */
                arg_register_error(endtable, endtable, ARG_EMISSARG,
                                   argv[optind - 1]);
                /* continue to scan the (empty) argument value to enforce argument count checking */
            }
            if (table[tabindex]->scanfn)
            {
                int errorcode = table[tabindex]->scanfn(parent, optarg);
                if (errorcode != 0)
                    arg_register_error(endtable, parent, errorcode, optarg);
            }
        }
        break;

        case '?':
            /*
             * getopt_long() found an unrecognised short option.
             * if it was a short option its value is in optopt
             * if it was a long option then optopt=0
             */
            switch (optopt)
            {
            case 0:
                /*printf("?0 unrecognised long option %s\n",argv[optind-1]);*/
                arg_register_error(endtable, endtable, ARG_ELONGOPT,
                                   argv[optind - 1]);
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
            arg_register_error(endtable, endtable, ARG_EMISSARG,
                               argv[optind - 1]);
            break;

        default:
        {
            /* getopt_long() found a valid short option */
            int tabindex = find_shortoption(table, (char)copt);
            /*printf("short option detected from argtable[%d]\n", tabindex);*/
            if (tabindex == -1)
            {
                /* should never get here - but handle it just in case */
                /*printf("unrecognised short option %d\n",copt);*/
                arg_register_error(endtable, endtable, copt, NULL);
            }
            else
            {
                if (table[tabindex]->scanfn)
                {
                    void *parent  = table[tabindex]->parent;
                    int errorcode = table[tabindex]->scanfn(parent, optarg);
                    if (errorcode != 0)
                        arg_register_error(endtable, parent, errorcode, optarg);
                }
            }
            break;
        }
        }
    }

    free(shortoptions);
    free(longoptions);
}


static
void arg_parse_untagged(int argc,
                        char * *argv,
                        struct arg_hdr * *table,
                        struct arg_end *endtable)
{
    int tabindex = 0;
    int errorlast = 0;
    const char *optarglast = NULL;
    void *parentlast = NULL;

    /*printf("arg_parse_untagged(%d,%p,%p,%p)\n",argc,argv,table,endtable);*/
    while (!(table[tabindex]->flag & ARG_TERMINATOR))
    {
        void *parent;
        int errorcode;

        /* if we have exhausted our argv[optind] entries then we have finished */
        if (optind >= argc)
        {
            /*printf("arg_parse_untagged(): argv[] exhausted\n");*/
            return;
        }

        /* skip table entries with non-null long or short options (they are not untagged entries) */
        if (table[tabindex]->longopts || table[tabindex]->shortopts)
        {
            /*printf("arg_parse_untagged(): skipping argtable[%d] (tagged argument)\n",tabindex);*/
            tabindex++;
            continue;
        }

        /* skip table entries with NULL scanfn */
        if (!(table[tabindex]->scanfn))
        {
            /*printf("arg_parse_untagged(): skipping argtable[%d] (NULL scanfn)\n",tabindex);*/
            tabindex++;
            continue;
        }

        /* attempt to scan the current argv[optind] with the current     */
        /* table[tabindex] entry. If it succeeds then keep it, otherwise */
        /* try again with the next table[] entry.                        */
        parent = table[tabindex]->parent;
        errorcode = table[tabindex]->scanfn(parent, argv[optind]);
        if (errorcode == 0)
        {
            /* success, move onto next argv[optind] but stay with same table[tabindex] */
            /*printf("arg_parse_untagged(): argtable[%d] successfully matched\n",tabindex);*/
            optind++;

            /* clear the last tentative error */
            errorlast = 0;
        }
        else
        {
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
    if (errorlast)
    {
        arg_register_error(endtable, parentlast, errorlast, optarglast);
        optind++;
    }

    /* only get here when not all argv[] entries were consumed */
    /* register an error for each unused argv[] entry */
    while (optind < argc)
    {
        /*printf("arg_parse_untagged(): argv[%d]=\"%s\" not consumed\n",optind,argv[optind]);*/
        arg_register_error(endtable, endtable, ARG_ENOMATCH, argv[optind++]);
    }

    return;
}


static
void arg_parse_check(struct arg_hdr * *table, struct arg_end *endtable)
{
    int tabindex = 0;
    /* printf("arg_parse_check()\n"); */
    do
    {
        if (table[tabindex]->checkfn)
        {
            void *parent  = table[tabindex]->parent;
            int errorcode = table[tabindex]->checkfn(parent);
            if (errorcode != 0)
                arg_register_error(endtable, parent, errorcode, NULL);
        }
    } while(!(table[tabindex++]->flag & ARG_TERMINATOR));
}


static
void arg_reset(void * *argtable)
{
    struct arg_hdr * *table = (struct arg_hdr * *)argtable;
    int tabindex = 0;
    /*printf("arg_reset(%p)\n",argtable);*/
    do
    {
        if (table[tabindex]->resetfn)
            table[tabindex]->resetfn(table[tabindex]->parent);
    } while(!(table[tabindex++]->flag & ARG_TERMINATOR));
}


int arg_parse(int argc, char * *argv, void * *argtable)
{
    struct arg_hdr * *table = (struct arg_hdr * *)argtable;
    struct arg_end *endtable;
    int endindex;
    char * *argvcopy = NULL;

    /*printf("arg_parse(%d,%p,%p)\n",argc,argv,argtable);*/

    /* reset any argtable data from previous invocations */
    arg_reset(argtable);

    /* locate the first end-of-table marker within the array */
    endindex = arg_endindex(table);
    endtable = (struct arg_end *)table[endindex];

    /* Special case of argc==0.  This can occur on Texas Instruments DSP. */
    /* Failure to trap this case results in an unwanted NULL result from  */
    /* the malloc for argvcopy (next code block).                         */
    if (argc == 0)
    {
        /* We must still perform post-parse checks despite the absence of command line arguments */
        arg_parse_check(table, endtable);

        /* Now we are finished */
        return endtable->count;
    }

    argvcopy = (char **)malloc(sizeof(char *) * (argc + 1));
    if (argvcopy)
    {
        int i;

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
        free(argvcopy);
    }
    else
    {
        /* memory alloc failed */
        arg_register_error(endtable, endtable, ARG_EMALLOC, NULL);
    }

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
static
void arg_cat(char * *pdest, const char *src, size_t *pndest)
{
    char *dest = *pdest;
    char *end  = dest + *pndest;

    /*locate null terminator of dest string */
    while(dest < end && *dest != 0)
        dest++;

    /* concat src string to dest string */
    while(dest < end && *src != 0)
        *dest++ = *src++;

    /* null terminate dest string */
    *dest = 0;

    /* update *pdest and *pndest */
    *pndest = end - dest;
    *pdest  = dest;
}


static
void arg_cat_option(char *dest,
                    size_t ndest,
                    const char *shortopts,
                    const char *longopts,
                    const char *datatype,
                    int optvalue)
{
    if (shortopts)
    {
        char option[3];

        /* note: option array[] is initialiazed dynamically here to satisfy   */
        /* a deficiency in the watcom compiler wrt static array initializers. */
        option[0] = '-';
        option[1] = shortopts[0];
        option[2] = 0;

        arg_cat(&dest, option, &ndest);
        if (datatype)
        {
            arg_cat(&dest, " ", &ndest);
            if (optvalue)
            {
                arg_cat(&dest, "[", &ndest);
                arg_cat(&dest, datatype, &ndest);
                arg_cat(&dest, "]", &ndest);
            }
            else
                arg_cat(&dest, datatype, &ndest);
        }
    }
    else if (longopts)
    {
        size_t ncspn;

        /* add "--" tag prefix */
        arg_cat(&dest, "--", &ndest);

        /* add comma separated option tag */
        ncspn = strcspn(longopts, ",");
        strncat(dest, longopts, (ncspn < ndest) ? ncspn : ndest);

        if (datatype)
        {
            arg_cat(&dest, "=", &ndest);
            if (optvalue)
            {
                arg_cat(&dest, "[", &ndest);
                arg_cat(&dest, datatype, &ndest);
                arg_cat(&dest, "]", &ndest);
            }
            else
                arg_cat(&dest, datatype, &ndest);
        }
    }
    else if (datatype)
    {
        if (optvalue)
        {
            arg_cat(&dest, "[", &ndest);
            arg_cat(&dest, datatype, &ndest);
            arg_cat(&dest, "]", &ndest);
        }
        else
            arg_cat(&dest, datatype, &ndest);
    }
}

static
void arg_cat_optionv(char *dest,
                     size_t ndest,
                     const char *shortopts,
                     const char *longopts,
                     const char *datatype,
                     int optvalue,
                     const char *separator)
{
    separator = separator ? separator : "";

    if (shortopts)
    {
        const char *c = shortopts;
        while(*c)
        {
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

    if (longopts)
    {
        const char *c = longopts;
        while(*c)
        {
            size_t ncspn;

            /* add "--" tag prefix */
            arg_cat(&dest, "--", &ndest);

            /* add comma separated option tag */
            ncspn = strcspn(c, ",");
            strncat(dest, c, (ncspn < ndest) ? ncspn : ndest);
            c += ncspn;

            /* add given separator in place of comma */
            if (*c == ',')
            {
                arg_cat(&dest, separator, &ndest);
                c++;
            }
        }
    }

    if (datatype)
    {
        if (longopts)
            arg_cat(&dest, "=", &ndest);
        else if (shortopts)
            arg_cat(&dest, " ", &ndest);

        if (optvalue)
        {
            arg_cat(&dest, "[", &ndest);
            arg_cat(&dest, datatype, &ndest);
            arg_cat(&dest, "]", &ndest);
        }
        else
            arg_cat(&dest, datatype, &ndest);
    }
}


/* this function should be deprecated because it doesnt consider optional argument values (ARG_HASOPTVALUE) */
void arg_print_option(FILE *fp,
                      const char *shortopts,
                      const char *longopts,
                      const char *datatype,
                      const char *suffix)
{
    char syntax[200] = "";
    suffix = suffix ? suffix : "";

    /* there is no way of passing the proper optvalue for optional argument values here, so we must ignore it */
    arg_cat_optionv(syntax,
                    sizeof(syntax),
                    shortopts,
                    longopts,
                    datatype,
                    0,
                    "|");

    fputs(syntax, fp);
    fputs(suffix, fp);
}


/*
 * Print a GNU style [OPTION] string in which all short options that
 * do not take argument values are presented in abbreviated form, as
 * in: -xvfsd, or -xvf[sd], or [-xvsfd]
 */
static
void arg_print_gnuswitch(FILE *fp, struct arg_hdr * *table)
{
    int tabindex;
    char *format1 = " -%c";
    char *format2 = " [-%c";
    char *suffix = "";

    /* print all mandatory switches that are without argument values */
    for(tabindex = 0;
        table[tabindex] && !(table[tabindex]->flag & ARG_TERMINATOR);
        tabindex++)
    {
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
        fprintf(fp, format1, table[tabindex]->shortopts[0]);
        format1 = "%c";
        format2 = "[%c";
    }

    /* print all optional switches that are without argument values */
    for(tabindex = 0;
        table[tabindex] && !(table[tabindex]->flag & ARG_TERMINATOR);
        tabindex++)
    {
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
        fprintf(fp, format2, table[tabindex]->shortopts[0]);
        format2 = "%c";
        suffix = "]";
    }

    fprintf(fp, "%s", suffix);
}


void arg_print_syntax(FILE *fp, void * *argtable, const char *suffix)
{
    struct arg_hdr * *table = (struct arg_hdr * *)argtable;
    int i, tabindex;

    /* print GNU style [OPTION] string */
    arg_print_gnuswitch(fp, table);

    /* print remaining options in abbreviated style */
    for(tabindex = 0;
        table[tabindex] && !(table[tabindex]->flag & ARG_TERMINATOR);
        tabindex++)
    {
        char syntax[200] = "";
        const char *shortopts, *longopts, *datatype;

        /* skip short options without arg values (they were printed by arg_print_gnu_switch) */
        if (table[tabindex]->shortopts &&
            !(table[tabindex]->flag & ARG_HASVALUE))
            continue;

        shortopts = table[tabindex]->shortopts;
        longopts  = table[tabindex]->longopts;
        datatype  = table[tabindex]->datatype;
        arg_cat_option(syntax,
                       sizeof(syntax),
                       shortopts,
                       longopts,
                       datatype,
                       table[tabindex]->flag & ARG_HASOPTVALUE);

        if (strlen(syntax) > 0)
        {
            /* print mandatory instances of this option */
            for (i = 0; i < table[tabindex]->mincount; i++)
                fprintf(fp, " %s", syntax);

            /* print optional instances enclosed in "[..]" */
            switch ( table[tabindex]->maxcount - table[tabindex]->mincount )
            {
            case 0:
                break;
            case 1:
                fprintf(fp, " [%s]", syntax);
                break;
            case 2:
                fprintf(fp, " [%s] [%s]", syntax, syntax);
                break;
            default:
                fprintf(fp, " [%s]...", syntax);
                break;
            }
        }
    }

    if (suffix)
        fprintf(fp, "%s", suffix);
}


void arg_print_syntaxv(FILE *fp, void * *argtable, const char *suffix)
{
    struct arg_hdr * *table = (struct arg_hdr * *)argtable;
    int i, tabindex;

    /* print remaining options in abbreviated style */
    for(tabindex = 0;
        table[tabindex] && !(table[tabindex]->flag & ARG_TERMINATOR);
        tabindex++)
    {
        char syntax[200] = "";
        const char *shortopts, *longopts, *datatype;

        shortopts = table[tabindex]->shortopts;
        longopts  = table[tabindex]->longopts;
        datatype  = table[tabindex]->datatype;
        arg_cat_optionv(syntax,
                        sizeof(syntax),
                        shortopts,
                        longopts,
                        datatype,
                        table[tabindex]->flag & ARG_HASOPTVALUE,
                        "|");

        /* print mandatory options */
        for (i = 0; i < table[tabindex]->mincount; i++)
            fprintf(fp, " %s", syntax);

        /* print optional args enclosed in "[..]" */
        switch ( table[tabindex]->maxcount - table[tabindex]->mincount )
        {
        case 0:
            break;
        case 1:
            fprintf(fp, " [%s]", syntax);
            break;
        case 2:
            fprintf(fp, " [%s] [%s]", syntax, syntax);
            break;
        default:
            fprintf(fp, " [%s]...", syntax);
            break;
        }
    }

    if (suffix)
        fprintf(fp, "%s", suffix);
}


void arg_print_glossary(FILE *fp, void * *argtable, const char *format)
{
    struct arg_hdr * *table = (struct arg_hdr * *)argtable;
    int tabindex;

    format = format ? format : "  %-20s %s\n";
    for (tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++)
    {
        if (table[tabindex]->glossary)
        {
            char syntax[200] = "";
            const char *shortopts = table[tabindex]->shortopts;
            const char *longopts  = table[tabindex]->longopts;
            const char *datatype  = table[tabindex]->datatype;
            const char *glossary  = table[tabindex]->glossary;
            arg_cat_optionv(syntax,
                            sizeof(syntax),
                            shortopts,
                            longopts,
                            datatype,
                            table[tabindex]->flag & ARG_HASOPTVALUE,
                            ", ");
            fprintf(fp, format, syntax, glossary);
        }
    }
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
void arg_print_formatted( FILE *fp,
                          const unsigned lmargin,
                          const unsigned rmargin,
                          const char *text )
{
    const unsigned textlen = strlen( text );
    unsigned line_start = 0;
    unsigned line_end = textlen + 1;
    const unsigned colwidth = (rmargin - lmargin) + 1;

    /* Someone doesn't like us... */
    if ( line_end < line_start )
    { fprintf( fp, "%s\n", text ); }

    while (line_end - 1 > line_start )
    {
        /* Eat leading whitespaces. This is essential because while
           wrapping lines, there will often be a whitespace at beginning
           of line */
        while ( isspace((int) *(text + line_start)) )
        { line_start++; }

        if ((line_end - line_start) > colwidth )
        { line_end = line_start + colwidth; }

        /* Find last whitespace, that fits into line */
        while ( ( line_end > line_start )
                && ( line_end - line_start > colwidth )
                && !isspace((int) *(text + line_end)))
        { line_end--; }

        /* Do not print trailing whitespace. If this text
           has got only one line, line_end now points to the
           last char due to initialization. */
        line_end--;

        /* Output line of text */
        while ( line_start < line_end )
        {
            fputc(*(text + line_start), fp );
            line_start++;
        }
        fputc( '\n', fp );

        /* Initialize another line */
        if ( line_end + 1 < textlen )
        {
            unsigned i;

            for (i = 0; i < lmargin; i++ )
            { fputc( ' ', fp ); }

            line_end = textlen;
        }

        /* If we have to print another line, get also the last char. */
        line_end++;

    } /* lines of text */
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
void arg_print_glossary_gnu(FILE *fp, void * *argtable )
{
    struct arg_hdr * *table = (struct arg_hdr * *)argtable;
    int tabindex;

    for(tabindex = 0; !(table[tabindex]->flag & ARG_TERMINATOR); tabindex++)
    {
        if (table[tabindex]->glossary)
        {
            char syntax[200] = "";
            const char *shortopts = table[tabindex]->shortopts;
            const char *longopts  = table[tabindex]->longopts;
            const char *datatype  = table[tabindex]->datatype;
            const char *glossary  = table[tabindex]->glossary;

            if ( !shortopts && longopts )
            {
                /* Indent trailing line by 4 spaces... */
                memset( syntax, ' ', 4 );
                *(syntax + 4) = '\0';
            }

            arg_cat_optionv(syntax,
                            sizeof(syntax),
                            shortopts,
                            longopts,
                            datatype,
                            table[tabindex]->flag & ARG_HASOPTVALUE,
                            ", ");

            /* If syntax fits not into column, print glossary in new line... */
            if ( strlen(syntax) > 25 )
            {
                fprintf( fp, "  %-25s %s\n", syntax, "" );
                *syntax = '\0';
            }

            fprintf( fp, "  %-25s ", syntax );
            arg_print_formatted( fp, 28, 79, glossary );
        }
    } /* for each table entry */

    fputc( '\n', fp );
}


/**
 * Checks the argtable[] array for NULL entries and returns 1
 * if any are found, zero otherwise.
 */
int arg_nullcheck(void * *argtable)
{
    struct arg_hdr * *table = (struct arg_hdr * *)argtable;
    int tabindex;
    /*printf("arg_nullcheck(%p)\n",argtable);*/

    if (!table)
        return 1;

    tabindex = 0;
    do
    {
        /*printf("argtable[%d]=%p\n",tabindex,argtable[tabindex]);*/
        if (!table[tabindex])
            return 1;
    } while(!(table[tabindex++]->flag & ARG_TERMINATOR));

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
void arg_free(void * *argtable)
{
    struct arg_hdr * *table = (struct arg_hdr * *)argtable;
    int tabindex = 0;
    int flag;
    /*printf("arg_free(%p)\n",argtable);*/
    do
    {
        /*
           if we encounter a NULL entry then somewhat incorrectly we presume
           we have come to the end of the array. It isnt strictly true because
           an intermediate entry could be NULL with other non-NULL entries to follow.
           The subsequent argtable entries would then not be freed as they should.
         */
        if (table[tabindex] == NULL)
            break;

        flag = table[tabindex]->flag;
        free(table[tabindex]);
        table[tabindex++] = NULL;

    } while(!(flag & ARG_TERMINATOR));
}

/* frees each non-NULL element of argtable[], where n is the size of the number of entries in the array */
void arg_freetable(void * *argtable, size_t n)
{
    struct arg_hdr * *table = (struct arg_hdr * *)argtable;
    size_t tabindex = 0;
    /*printf("arg_freetable(%p)\n",argtable);*/
    for (tabindex = 0; tabindex < n; tabindex++)
    {
        if (table[tabindex] == NULL)
            continue;

        free(table[tabindex]);
        table[tabindex] = NULL;
    };
}

