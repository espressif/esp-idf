/*
 * SPDX-FileCopyrightText: 1998-2001,2003-2011,2013 Stewart Heitmann
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*******************************************************************************
 * arg_rex: Implements the regex command-line option
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

#ifndef _TREX_H_
#define _TREX_H_

/*
 * This module uses the T-Rex regular expression library to implement the regex
 * logic. Here is the copyright notice of the library:
 *
 * Copyright (C) 2003-2006 Alberto Demichelis
 *
 * This software is provided 'as-is', without any express
 * or implied warranty. In no event will the authors be held
 * liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for
 * any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented;
 *      you must not claim that you wrote the original software.
 *      If you use this software in a product, an acknowledgment
 *      in the product documentation would be appreciated but
 *      is not required.
 *
 *   2. Altered source versions must be plainly marked as such,
 *      and must not be misrepresented as being the original software.
 *
 *   3. This notice may not be removed or altered from any
 *      source distribution.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define TRexChar char
#define MAX_CHAR 0xFF
#define _TREXC(c) (c)
#define trex_strlen strlen
#define trex_printf printf

#ifndef TREX_API
#define TREX_API extern
#endif

#define TRex_True 1
#define TRex_False 0

#define TREX_ICASE ARG_REX_ICASE

typedef unsigned int TRexBool;
typedef struct TRex TRex;

typedef struct {
    const TRexChar* begin;
    int len;
} TRexMatch;

#if defined(__clang__)
TREX_API TRex* trex_compile(const TRexChar* pattern, const TRexChar** error, int flags) __attribute__((optnone));
#elif defined(__GNUC__)
TREX_API TRex* trex_compile(const TRexChar* pattern, const TRexChar** error, int flags) __attribute__((optimize(0)));
#else
TREX_API TRex* trex_compile(const TRexChar* pattern, const TRexChar** error, int flags);
#endif
TREX_API void trex_free(TRex* exp);
TREX_API TRexBool trex_match(TRex* exp, const TRexChar* text);
TREX_API TRexBool trex_search(TRex* exp, const TRexChar* text, const TRexChar** out_begin, const TRexChar** out_end);
TREX_API TRexBool
trex_searchrange(TRex* exp, const TRexChar* text_begin, const TRexChar* text_end, const TRexChar** out_begin, const TRexChar** out_end);
TREX_API int trex_getsubexpcount(TRex* exp);
TREX_API TRexBool trex_getsubexp(TRex* exp, int n, TRexMatch* subexp);

#ifdef __cplusplus
}
#endif

#endif

struct privhdr {
    const char* pattern;
    int flags;
};

static void arg_rex_resetfn(struct arg_rex* parent) {
    ARG_TRACE(("%s:resetfn(%p)\n", __FILE__, parent));
    parent->count = 0;
}

static int arg_rex_scanfn(struct arg_rex* parent, const char* argval) {
    int errorcode = 0;
    const TRexChar* error = NULL;
    TRex* rex = NULL;
    TRexBool is_match = TRex_False;

    if (parent->count == parent->hdr.maxcount) {
        /* maximum number of arguments exceeded */
        errorcode = ARG_ERR_MAXCOUNT;
    } else if (!argval) {
        /* a valid argument with no argument value was given. */
        /* This happens when an optional argument value was invoked. */
        /* leave parent argument value unaltered but still count the argument. */
        parent->count++;
    } else {
        struct privhdr* priv = (struct privhdr*)parent->hdr.priv;

        /* test the current argument value for a match with the regular expression */
        /* if a match is detected, record the argument value in the arg_rex struct */

        rex = trex_compile(priv->pattern, &error, priv->flags);
        is_match = trex_match(rex, argval);
        if (!is_match)
            errorcode = ARG_ERR_REGNOMATCH;
        else
            parent->sval[parent->count++] = argval;

        trex_free(rex);
    }

    ARG_TRACE(("%s:scanfn(%p) returns %d\n", __FILE__, parent, errorcode));
    return errorcode;
}

static int arg_rex_checkfn(struct arg_rex* parent) {
    int errorcode = (parent->count < parent->hdr.mincount) ? ARG_ERR_MINCOUNT : 0;
#if 0
    struct privhdr *priv = (struct privhdr*)parent->hdr.priv;

    /* free the regex "program" we constructed in resetfn */
    regfree(&(priv->regex));

    /*printf("%s:checkfn(%p) returns %d\n",__FILE__,parent,errorcode);*/
#endif
    return errorcode;
}

static void arg_rex_errorfn(struct arg_rex* parent, arg_dstr_t ds, int errorcode, const char* argval, const char* progname) {
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

        case ARG_ERR_REGNOMATCH:
            arg_dstr_cat(ds, "illegal value  ");
            arg_print_option_ds(ds, shortopts, longopts, argval, "\n");
            break;

        default: {
        #if 0
            char errbuff[256];
            regerror(errorcode, NULL, errbuff, sizeof(errbuff));
            printf("%s\n", errbuff);
        #endif
        } break;
    }
}

struct arg_rex* arg_rex0(const char* shortopts, const char* longopts, const char* pattern, const char* datatype, int flags, const char* glossary) {
    return arg_rexn(shortopts, longopts, pattern, datatype, 0, 1, flags, glossary);
}

struct arg_rex* arg_rex1(const char* shortopts, const char* longopts, const char* pattern, const char* datatype, int flags, const char* glossary) {
    return arg_rexn(shortopts, longopts, pattern, datatype, 1, 1, flags, glossary);
}

struct arg_rex* arg_rexn(const char* shortopts,
                         const char* longopts,
                         const char* pattern,
                         const char* datatype,
                         int mincount,
                         int maxcount,
                         int flags,
                         const char* glossary) {
    size_t nbytes;
    struct arg_rex* result;
    struct privhdr* priv;
    int i;
    const TRexChar* error = NULL;
    TRex* rex = NULL;

    if (!pattern) {
        printf("argtable: ERROR - illegal regular expression pattern \"(NULL)\"\n");
        printf("argtable: Bad argument table.\n");
        return NULL;
    }

    /* foolproof things by ensuring maxcount is not less than mincount */
    maxcount = (maxcount < mincount) ? mincount : maxcount;

    nbytes = sizeof(struct arg_rex)      /* storage for struct arg_rex */
             + sizeof(struct privhdr)    /* storage for private arg_rex data */
             + (size_t)maxcount * sizeof(char*); /* storage for sval[maxcount] array */

    /* init the arg_hdr struct */
    result = (struct arg_rex*)xmalloc(nbytes);
    result->hdr.flag = ARG_HASVALUE;
    result->hdr.shortopts = shortopts;
    result->hdr.longopts = longopts;
    result->hdr.datatype = datatype ? datatype : pattern;
    result->hdr.glossary = glossary;
    result->hdr.mincount = mincount;
    result->hdr.maxcount = maxcount;
    result->hdr.parent = result;
    result->hdr.resetfn = (arg_resetfn*)arg_rex_resetfn;
    result->hdr.scanfn = (arg_scanfn*)arg_rex_scanfn;
    result->hdr.checkfn = (arg_checkfn*)arg_rex_checkfn;
    result->hdr.errorfn = (arg_errorfn*)arg_rex_errorfn;

    /* store the arg_rex_priv struct immediately after the arg_rex struct */
    result->hdr.priv = result + 1;
    priv = (struct privhdr*)(result->hdr.priv);
    priv->pattern = pattern;
    priv->flags = flags;

    /* store the sval[maxcount] array immediately after the arg_rex_priv struct */
    result->sval = (const char**)(priv + 1);
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
    if (rex == NULL) {
        ARG_LOG(("argtable: %s \"%s\"\n", error ? error : _TREXC("undefined"), priv->pattern));
        ARG_LOG(("argtable: Bad argument table.\n"));
    }

    trex_free(rex);

    ARG_TRACE(("arg_rexn() returns %p\n", result));
    return result;
}

/* see copyright notice in trex.h */
#include <ctype.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

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

#ifdef ARG_REX_DEBUG
#include <stdio.h>

static const TRexChar* g_nnames[] = {_SC("NONE"),    _SC("OP_GREEDY"), _SC("OP_OR"),     _SC("OP_EXPR"),   _SC("OP_NOCAPEXPR"),
                                     _SC("OP_DOT"),  _SC("OP_CLASS"),  _SC("OP_CCLASS"), _SC("OP_NCLASS"), _SC("OP_RANGE"),
                                     _SC("OP_CHAR"), _SC("OP_EOL"),    _SC("OP_BOL"),    _SC("OP_WB")};

#endif
#define OP_GREEDY (MAX_CHAR + 1)  /*  * + ? {n} */
#define OP_OR (MAX_CHAR + 2)
#define OP_EXPR (MAX_CHAR + 3)       /* parentesis () */
#define OP_NOCAPEXPR (MAX_CHAR + 4)  /* parentesis (?:) */
#define OP_DOT (MAX_CHAR + 5)
#define OP_CLASS (MAX_CHAR + 6)
#define OP_CCLASS (MAX_CHAR + 7)
#define OP_NCLASS (MAX_CHAR + 8)  /* negates class the [^ */
#define OP_RANGE (MAX_CHAR + 9)
#define OP_CHAR (MAX_CHAR + 10)
#define OP_EOL (MAX_CHAR + 11)
#define OP_BOL (MAX_CHAR + 12)
#define OP_WB (MAX_CHAR + 13)

#define TREX_SYMBOL_ANY_CHAR ('.')
#define TREX_SYMBOL_GREEDY_ONE_OR_MORE ('+')
#define TREX_SYMBOL_GREEDY_ZERO_OR_MORE ('*')
#define TREX_SYMBOL_GREEDY_ZERO_OR_ONE ('?')
#define TREX_SYMBOL_BRANCH ('|')
#define TREX_SYMBOL_END_OF_STRING ('$')
#define TREX_SYMBOL_BEGINNING_OF_STRING ('^')
#define TREX_SYMBOL_ESCAPE_CHAR ('\\')

typedef int TRexNodeType;

typedef struct tagTRexNode {
    TRexNodeType type;
    int left;
    int right;
    int next;
} TRexNode;

struct TRex {
    const TRexChar* _eol;
    const TRexChar* _bol;
    const TRexChar* _p;
    int _first;
    int _op;
    TRexNode* _nodes;
    int _nallocated;
    int _nsize;
    int _nsubexpr;
    TRexMatch* _matches;
    int _currsubexp;
    void* _jmpbuf;
    const TRexChar** _error;
    int _flags;
};

static int trex_list(TRex* exp);

static int trex_newnode(TRex* exp, TRexNodeType type) {
    TRexNode n;
    int newid;
    n.type = type;
    n.next = n.right = n.left = -1;
    if (type == OP_EXPR)
        n.right = exp->_nsubexpr++;
    if (exp->_nallocated < (exp->_nsize + 1)) {
        exp->_nallocated *= 2;
        exp->_nodes = (TRexNode*)xrealloc(exp->_nodes, (size_t)exp->_nallocated * sizeof(TRexNode));
    }
    exp->_nodes[exp->_nsize++] = n;
    newid = exp->_nsize - 1;
    return (int)newid;
}

static void trex_error(TRex* exp, const TRexChar* error) {
    if (exp->_error)
        *exp->_error = error;
    longjmp(*((jmp_buf*)exp->_jmpbuf), -1);
}

static void trex_expect(TRex* exp, int n) {
    if ((*exp->_p) != n)
        trex_error(exp, _SC("expected paren"));
    exp->_p++;
}

static TRexChar trex_escapechar(TRex* exp) {
    if (*exp->_p == TREX_SYMBOL_ESCAPE_CHAR) {
        exp->_p++;
        switch (*exp->_p) {
            case 'v':
                exp->_p++;
                return '\v';
            case 'n':
                exp->_p++;
                return '\n';
            case 't':
                exp->_p++;
                return '\t';
            case 'r':
                exp->_p++;
                return '\r';
            case 'f':
                exp->_p++;
                return '\f';
            default:
                return (*exp->_p++);
        }
    } else if (!scisprint((int)(*exp->_p)))
        trex_error(exp, _SC("letter expected"));
    return (*exp->_p++);
}

static int trex_charclass(TRex* exp, int classid) {
    int n = trex_newnode(exp, OP_CCLASS);
    exp->_nodes[n].left = classid;
    return n;
}

static int trex_charnode(TRex* exp, TRexBool isclass) {
    TRexChar t;
    if (*exp->_p == TREX_SYMBOL_ESCAPE_CHAR) {
        exp->_p++;
        switch (*exp->_p) {
            case 'n':
                exp->_p++;
                return trex_newnode(exp, '\n');
            case 't':
                exp->_p++;
                return trex_newnode(exp, '\t');
            case 'r':
                exp->_p++;
                return trex_newnode(exp, '\r');
            case 'f':
                exp->_p++;
                return trex_newnode(exp, '\f');
            case 'v':
                exp->_p++;
                return trex_newnode(exp, '\v');
            case 'a':
            case 'A':
            case 'w':
            case 'W':
            case 's':
            case 'S':
            case 'd':
            case 'D':
            case 'x':
            case 'X':
            case 'c':
            case 'C':
            case 'p':
            case 'P':
            case 'l':
            case 'u': {
                t = *exp->_p;
                exp->_p++;
                return trex_charclass(exp, t);
            }
            case 'b':
            case 'B':
                if (!isclass) {
                    int node = trex_newnode(exp, OP_WB);
                    exp->_nodes[node].left = *exp->_p;
                    exp->_p++;
                    return node;
                }
                /* fall through */
            default:
                t = *exp->_p;
                exp->_p++;
                return trex_newnode(exp, t);
        }
    } else if (!scisprint((int)(*exp->_p))) {
        trex_error(exp, _SC("letter expected"));
    }
    t = *exp->_p;
    exp->_p++;
    return trex_newnode(exp, t);
}
static int trex_class(TRex* exp) {
    int ret = -1;
    int first = -1, chain;
    if (*exp->_p == TREX_SYMBOL_BEGINNING_OF_STRING) {
        ret = trex_newnode(exp, OP_NCLASS);
        exp->_p++;
    } else
        ret = trex_newnode(exp, OP_CLASS);

    if (*exp->_p == ']')
        trex_error(exp, _SC("empty class"));
    chain = ret;
    while (*exp->_p != ']' && exp->_p != exp->_eol) {
        if (*exp->_p == '-' && first != -1) {
            int r, t;
            if (*exp->_p++ == ']')
                trex_error(exp, _SC("unfinished range"));
            r = trex_newnode(exp, OP_RANGE);
            if (first > *exp->_p)
                trex_error(exp, _SC("invalid range"));
            if (exp->_nodes[first].type == OP_CCLASS)
                trex_error(exp, _SC("cannot use character classes in ranges"));
            exp->_nodes[r].left = exp->_nodes[first].type;
            t = trex_escapechar(exp);
            exp->_nodes[r].right = t;
            exp->_nodes[chain].next = r;
            chain = r;
            first = -1;
        } else {
            if (first != -1) {
                int c = first;
                exp->_nodes[chain].next = c;
                chain = c;
                first = trex_charnode(exp, TRex_True);
            } else {
                first = trex_charnode(exp, TRex_True);
            }
        }
    }
    if (first != -1) {
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

static int trex_parsenumber(TRex* exp) {
    int ret = *exp->_p - '0';
    int positions = 10;
    exp->_p++;
    while (isdigit((int)(*exp->_p))) {
        ret = ret * 10 + (*exp->_p++ - '0');
        if (positions == 1000000000)
            trex_error(exp, _SC("overflow in numeric constant"));
        positions *= 10;
    };
    return ret;
}

static int trex_element(TRex* exp) {
    int ret = -1;
    switch (*exp->_p) {
        case '(': {
            int expr, newn;
            exp->_p++;

            if (*exp->_p == '?') {
                exp->_p++;
                trex_expect(exp, ':');
                expr = trex_newnode(exp, OP_NOCAPEXPR);
            } else
                expr = trex_newnode(exp, OP_EXPR);
            newn = trex_list(exp);
            exp->_nodes[expr].left = newn;
            ret = expr;
            trex_expect(exp, ')');
        } break;
        case '[':
            exp->_p++;
            ret = trex_class(exp);
            trex_expect(exp, ']');
            break;
        case TREX_SYMBOL_END_OF_STRING:
            exp->_p++;
            ret = trex_newnode(exp, OP_EOL);
            break;
        case TREX_SYMBOL_ANY_CHAR:
            exp->_p++;
            ret = trex_newnode(exp, OP_DOT);
            break;
        default:
            ret = trex_charnode(exp, TRex_False);
            break;
    }

    {
        TRexBool isgreedy = TRex_False;
        unsigned short p0 = 0, p1 = 0;
        switch (*exp->_p) {
            case TREX_SYMBOL_GREEDY_ZERO_OR_MORE:
                p0 = 0;
                p1 = 0xFFFF;
                exp->_p++;
                isgreedy = TRex_True;
                break;
            case TREX_SYMBOL_GREEDY_ONE_OR_MORE:
                p0 = 1;
                p1 = 0xFFFF;
                exp->_p++;
                isgreedy = TRex_True;
                break;
            case TREX_SYMBOL_GREEDY_ZERO_OR_ONE:
                p0 = 0;
                p1 = 1;
                exp->_p++;
                isgreedy = TRex_True;
                break;
            case '{':
                exp->_p++;
                if (!isdigit((int)(*exp->_p)))
                    trex_error(exp, _SC("number expected"));
                p0 = (unsigned short)trex_parsenumber(exp);
                /*******************************/
                switch (*exp->_p) {
                    case '}':
                        p1 = p0;
                        exp->_p++;
                        break;
                    case ',':
                        exp->_p++;
                        p1 = 0xFFFF;
                        if (isdigit((int)(*exp->_p))) {
                            p1 = (unsigned short)trex_parsenumber(exp);
                        }
                        trex_expect(exp, '}');
                        break;
                    default:
                        trex_error(exp, _SC(", or } expected"));
                }
                /*******************************/
                isgreedy = TRex_True;
                break;
        }
        if (isgreedy) {
            int nnode = trex_newnode(exp, OP_GREEDY);
            exp->_nodes[nnode].left = ret;
            exp->_nodes[nnode].right = ((p0) << 16) | p1;
            ret = nnode;
        }
    }
    if ((*exp->_p != TREX_SYMBOL_BRANCH) && (*exp->_p != ')') && (*exp->_p != TREX_SYMBOL_GREEDY_ZERO_OR_MORE) &&
        (*exp->_p != TREX_SYMBOL_GREEDY_ONE_OR_MORE) && (*exp->_p != '\0')) {
        int nnode = trex_element(exp);
        exp->_nodes[ret].next = nnode;
    }

    return ret;
}

static int trex_list(TRex* exp) {
    int ret = -1, e;
    if (*exp->_p == TREX_SYMBOL_BEGINNING_OF_STRING) {
        exp->_p++;
        ret = trex_newnode(exp, OP_BOL);
    }
    e = trex_element(exp);
    if (ret != -1) {
        exp->_nodes[ret].next = e;
    } else
        ret = e;

    if (*exp->_p == TREX_SYMBOL_BRANCH) {
        int temp, tright;
        exp->_p++;
        temp = trex_newnode(exp, OP_OR);
        exp->_nodes[temp].left = ret;
        tright = trex_list(exp);
        exp->_nodes[temp].right = tright;
        ret = temp;
    }
    return ret;
}

static TRexBool trex_matchcclass(int cclass, TRexChar c) {
    switch (cclass) {
        case 'a':
            return isalpha(c) ? TRex_True : TRex_False;
        case 'A':
            return !isalpha(c) ? TRex_True : TRex_False;
        case 'w':
            return (isalnum(c) || c == '_') ? TRex_True : TRex_False;
        case 'W':
            return (!isalnum(c) && c != '_') ? TRex_True : TRex_False;
        case 's':
            return isspace(c) ? TRex_True : TRex_False;
        case 'S':
            return !isspace(c) ? TRex_True : TRex_False;
        case 'd':
            return isdigit(c) ? TRex_True : TRex_False;
        case 'D':
            return !isdigit(c) ? TRex_True : TRex_False;
        case 'x':
            return isxdigit(c) ? TRex_True : TRex_False;
        case 'X':
            return !isxdigit(c) ? TRex_True : TRex_False;
        case 'c':
            return iscntrl(c) ? TRex_True : TRex_False;
        case 'C':
            return !iscntrl(c) ? TRex_True : TRex_False;
        case 'p':
            return ispunct(c) ? TRex_True : TRex_False;
        case 'P':
            return !ispunct(c) ? TRex_True : TRex_False;
        case 'l':
            return islower(c) ? TRex_True : TRex_False;
        case 'u':
            return isupper(c) ? TRex_True : TRex_False;
    }
    return TRex_False; /*cannot happen*/
}

static TRexBool trex_matchclass(TRex* exp, TRexNode* node, TRexChar c) {
    do {
        switch (node->type) {
            case OP_RANGE:
                if (exp->_flags & TREX_ICASE) {
                    if (c >= toupper(node->left) && c <= toupper(node->right))
                        return TRex_True;
                    if (c >= tolower(node->left) && c <= tolower(node->right))
                        return TRex_True;
                } else {
                    if (c >= node->left && c <= node->right)
                        return TRex_True;
                }
                break;
            case OP_CCLASS:
                if (trex_matchcclass(node->left, c))
                    return TRex_True;
                break;
            default:
                if (exp->_flags & TREX_ICASE) {
                    if (c == tolower(node->type) || c == toupper(node->type))
                        return TRex_True;
                } else {
                    if (c == node->type)
                        return TRex_True;
                }
        }
    } while ((node->next != -1) && ((node = &exp->_nodes[node->next]) != NULL));
    return TRex_False;
}

static const TRexChar* trex_matchnode(TRex* exp, TRexNode* node, const TRexChar* str, TRexNode* next) {
    TRexNodeType type = node->type;
    switch (type) {
        case OP_GREEDY: {
            /* TRexNode *greedystop = (node->next != -1) ? &exp->_nodes[node->next] : NULL; */
            TRexNode* greedystop = NULL;
            int p0 = (node->right >> 16) & 0x0000FFFF, p1 = node->right & 0x0000FFFF, nmaches = 0;
            const TRexChar *s = str, *good = str;

            if (node->next != -1) {
                greedystop = &exp->_nodes[node->next];
            } else {
                greedystop = next;
            }

            while ((nmaches == 0xFFFF || nmaches < p1)) {
                const TRexChar* stop;
                if ((s = trex_matchnode(exp, &exp->_nodes[node->left], s, greedystop)) == NULL)
                    break;
                nmaches++;
                good = s;
                if (greedystop) {
                    /* checks that 0 matches satisfy the expression(if so skips) */
                    /* if not would always stop(for instance if is a '?') */
                    if (greedystop->type != OP_GREEDY || (greedystop->type == OP_GREEDY && ((greedystop->right >> 16) & 0x0000FFFF) != 0)) {
                        TRexNode* gnext = NULL;
                        if (greedystop->next != -1) {
                            gnext = &exp->_nodes[greedystop->next];
                        } else if (next && next->next != -1) {
                            gnext = &exp->_nodes[next->next];
                        }
                        stop = trex_matchnode(exp, greedystop, s, gnext);
                        if (stop) {
                            /* if satisfied stop it */
                            if (p0 == p1 && p0 == nmaches)
                                break;
                            else if (nmaches >= p0 && p1 == 0xFFFF)
                                break;
                            else if (nmaches >= p0 && nmaches <= p1)
                                break;
                        }
                    }
                }

                if (s >= exp->_eol)
                    break;
            }
            if (p0 == p1 && p0 == nmaches)
                return good;
            else if (nmaches >= p0 && p1 == 0xFFFF)
                return good;
            else if (nmaches >= p0 && nmaches <= p1)
                return good;
            return NULL;
        }
        case OP_OR: {
            const TRexChar* asd = str;
            TRexNode* temp = &exp->_nodes[node->left];
            while ((asd = trex_matchnode(exp, temp, asd, NULL)) != NULL) {
                if (temp->next != -1)
                    temp = &exp->_nodes[temp->next];
                else
                    return asd;
            }
            asd = str;
            temp = &exp->_nodes[node->right];
            while ((asd = trex_matchnode(exp, temp, asd, NULL)) != NULL) {
                if (temp->next != -1)
                    temp = &exp->_nodes[temp->next];
                else
                    return asd;
            }
            return NULL;
            break;
        }
        case OP_EXPR:
        case OP_NOCAPEXPR: {
            TRexNode* n = &exp->_nodes[node->left];
            const TRexChar* cur = str;
            int capture = -1;
            if (node->type != OP_NOCAPEXPR && node->right == exp->_currsubexp) {
                capture = exp->_currsubexp;
                exp->_matches[capture].begin = cur;
                exp->_currsubexp++;
            }

            do {
                TRexNode* subnext = NULL;
                if (n->next != -1) {
                    subnext = &exp->_nodes[n->next];
                } else {
                    subnext = next;
                }
                if ((cur = trex_matchnode(exp, n, cur, subnext)) == NULL) {
                    if (capture != -1) {
                        exp->_matches[capture].begin = 0;
                        exp->_matches[capture].len = 0;
                    }
                    return NULL;
                }
            } while ((n->next != -1) && ((n = &exp->_nodes[n->next]) != NULL));

            if (capture != -1)
                exp->_matches[capture].len = (int)(cur - exp->_matches[capture].begin);
            return cur;
        }
        case OP_WB:
            if ((str == exp->_bol && !isspace((int)(*str))) || (str == exp->_eol && !isspace((int)(*(str - 1)))) || (!isspace((int)(*str)) && isspace((int)(*(str + 1)))) ||
                (isspace((int)(*str)) && !isspace((int)(*(str + 1))))) {
                return (node->left == 'b') ? str : NULL;
            }
            return (node->left == 'b') ? NULL : str;
        case OP_BOL:
            if (str == exp->_bol)
                return str;
            return NULL;
        case OP_EOL:
            if (str == exp->_eol)
                return str;
            return NULL;
        case OP_DOT: {
            str++;
        }
            return str;
        case OP_NCLASS:
        case OP_CLASS:
            if (trex_matchclass(exp, &exp->_nodes[node->left], *str) ? (type == OP_CLASS ? TRex_True : TRex_False)
                                                                     : (type == OP_NCLASS ? TRex_True : TRex_False)) {
                str++;
                return str;
            }
            return NULL;
        case OP_CCLASS:
            if (trex_matchcclass(node->left, *str)) {
                str++;
                return str;
            }
            return NULL;
        default: /* char */
            if (exp->_flags & TREX_ICASE) {
                if (*str != tolower(node->type) && *str != toupper(node->type))
                    return NULL;
            } else {
                if (*str != node->type)
                    return NULL;
            }
            str++;
            return str;
    }
}

/* public api */
TRex* trex_compile(const TRexChar* pattern, const TRexChar** error, int flags) {
    TRex* exp = (TRex*)xmalloc(sizeof(TRex));
    exp->_eol = exp->_bol = NULL;
    exp->_p = pattern;
    exp->_nallocated = (int)(scstrlen(pattern) * sizeof(TRexChar));
    exp->_nodes = (TRexNode*)xmalloc((size_t)exp->_nallocated * sizeof(TRexNode));
    exp->_nsize = 0;
    exp->_matches = 0;
    exp->_nsubexpr = 0;
    exp->_first = trex_newnode(exp, OP_EXPR);
    exp->_error = error;
    exp->_jmpbuf = xmalloc(sizeof(jmp_buf));
    exp->_flags = flags;
    if (setjmp(*((jmp_buf*)exp->_jmpbuf)) == 0) {
        int res = trex_list(exp);
        exp->_nodes[exp->_first].left = res;
        if (*exp->_p != '\0')
            trex_error(exp, _SC("unexpected character"));
#ifdef ARG_REX_DEBUG
        {
            int nsize, i;
            nsize = exp->_nsize;
            scprintf(_SC("\n"));
            for (i = 0; i < nsize; i++) {
                if (exp->_nodes[i].type > MAX_CHAR)
                    scprintf(_SC("[%02d] %10s "), i, g_nnames[exp->_nodes[i].type - MAX_CHAR]);
                else
                    scprintf(_SC("[%02d] %10c "), i, exp->_nodes[i].type);
                scprintf(_SC("left %02d right %02d next %02d\n"), exp->_nodes[i].left, exp->_nodes[i].right, exp->_nodes[i].next);
            }
            scprintf(_SC("\n"));
        }
#endif
        exp->_matches = (TRexMatch*)xmalloc((size_t)exp->_nsubexpr * sizeof(TRexMatch));
        memset(exp->_matches, 0, (size_t)exp->_nsubexpr * sizeof(TRexMatch));
    } else {
        trex_free(exp);
        return NULL;
    }
    return exp;
}

void trex_free(TRex* exp) {
    if (exp) {
        xfree(exp->_nodes);
        xfree(exp->_jmpbuf);
        xfree(exp->_matches);
        xfree(exp);
    }
}

TRexBool trex_match(TRex* exp, const TRexChar* text) {
    const TRexChar* res = NULL;
    exp->_bol = text;
    exp->_eol = text + scstrlen(text);
    exp->_currsubexp = 0;
    res = trex_matchnode(exp, exp->_nodes, text, NULL);
    if (res == NULL || res != exp->_eol)
        return TRex_False;
    return TRex_True;
}

TRexBool trex_searchrange(TRex* exp, const TRexChar* text_begin, const TRexChar* text_end, const TRexChar** out_begin, const TRexChar** out_end) {
    const TRexChar* cur = NULL;
    int node = exp->_first;
    if (text_begin >= text_end)
        return TRex_False;
    exp->_bol = text_begin;
    exp->_eol = text_end;
    do {
        cur = text_begin;
        while (node != -1) {
            exp->_currsubexp = 0;
            cur = trex_matchnode(exp, &exp->_nodes[node], cur, NULL);
            if (!cur)
                break;
            node = exp->_nodes[node].next;
        }
        text_begin++;
    } while (cur == NULL && text_begin != text_end);

    if (cur == NULL)
        return TRex_False;

    --text_begin;

    if (out_begin)
        *out_begin = text_begin;
    if (out_end)
        *out_end = cur;
    return TRex_True;
}

TRexBool trex_search(TRex* exp, const TRexChar* text, const TRexChar** out_begin, const TRexChar** out_end) {
    return trex_searchrange(exp, text, text + scstrlen(text), out_begin, out_end);
}

int trex_getsubexpcount(TRex* exp) {
    return exp->_nsubexpr;
}

TRexBool trex_getsubexp(TRex* exp, int n, TRexMatch* subexp) {
    if (n < 0 || n >= exp->_nsubexpr)
        return TRex_False;
    *subexp = exp->_matches[n];
    return TRex_True;
}
