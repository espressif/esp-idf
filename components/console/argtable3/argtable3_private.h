/*
 * SPDX-FileCopyrightText: 2013-2019 Tom G. Huang
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*******************************************************************************
 * argtable3_private: Declares private types, constants, and interfaces
 *
 * This file is part of the argtable3 library.
 *
 * Copyright (C) 2013-2019 Tom G. Huang
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

#include <stdlib.h>

#define ARG_ENABLE_TRACE 0
#define ARG_ENABLE_LOG 0

#ifdef __cplusplus
extern "C" {
#endif

enum { ARG_ERR_MINCOUNT = 1, ARG_ERR_MAXCOUNT, ARG_ERR_BADINT, ARG_ERR_OVERFLOW, ARG_ERR_BADDOUBLE, ARG_ERR_BADDATE, ARG_ERR_REGNOMATCH };

typedef void(arg_panicfn)(const char* fmt, ...);

#if defined(_MSC_VER)
#define ARG_TRACE(x)                                               \
    __pragma(warning(push)) __pragma(warning(disable : 4127)) do { \
        if (ARG_ENABLE_TRACE)                                      \
            dbg_printf x;                                          \
    }                                                              \
    while (0)                                                      \
    __pragma(warning(pop))

#define ARG_LOG(x)                                                 \
    __pragma(warning(push)) __pragma(warning(disable : 4127)) do { \
        if (ARG_ENABLE_LOG)                                        \
            dbg_printf x;                                          \
    }                                                              \
    while (0)                                                      \
    __pragma(warning(pop))
#else
#define ARG_TRACE(x)          \
    do {                      \
        if (ARG_ENABLE_TRACE) \
            dbg_printf x;     \
    } while (0)

#define ARG_LOG(x)          \
    do {                    \
        if (ARG_ENABLE_LOG) \
            dbg_printf x;   \
    } while (0)
#endif

/*
 * Rename a few generic names to unique names.
 * They can be a problem for the platforms like NuttX, where
 * the namespace is flat for everything including apps and libraries.
 */
#define	xmalloc argtable3_xmalloc
#define	xcalloc argtable3_xcalloc
#define	xrealloc argtable3_xrealloc
#define	xfree argtable3_xfree

extern void dbg_printf(const char* fmt, ...);
extern void arg_set_panic(arg_panicfn* proc);
extern void* xmalloc(size_t size);
extern void* xcalloc(size_t count, size_t size);
extern void* xrealloc(void* ptr, size_t size);
extern void xfree(void* ptr);

struct arg_hashtable_entry {
    void *k, *v;
    unsigned int h;
    struct arg_hashtable_entry* next;
};

typedef struct arg_hashtable {
    unsigned int tablelength;
    struct arg_hashtable_entry** table;
    unsigned int entrycount;
    unsigned int loadlimit;
    unsigned int primeindex;
    unsigned int (*hashfn)(const void* k);
    int (*eqfn)(const void* k1, const void* k2);
} arg_hashtable_t;

/**
 * @brief Create a hash table.
 *
 * @param   minsize   minimum initial size of hash table
 * @param   hashfn    function for hashing keys
 * @param   eqfn      function for determining key equality
 * @return            newly created hash table or NULL on failure
 */
arg_hashtable_t* arg_hashtable_create(unsigned int minsize, unsigned int (*hashfn)(const void*), int (*eqfn)(const void*, const void*));

/**
 * @brief This function will cause the table to expand if the insertion would take
 * the ratio of entries to table size over the maximum load factor.
 *
 * This function does not check for repeated insertions with a duplicate key.
 * The value returned when using a duplicate key is undefined -- when
 * the hash table changes size, the order of retrieval of duplicate key
 * entries is reversed.
 * If in doubt, remove before insert.
 *
 * @param   h   the hash table to insert into
 * @param   k   the key - hash table claims ownership and will free on removal
 * @param   v   the value - does not claim ownership
 * @return      non-zero for successful insertion
 */
void arg_hashtable_insert(arg_hashtable_t* h, void* k, void* v);

#define ARG_DEFINE_HASHTABLE_INSERT(fnname, keytype, valuetype) \
    int fnname(arg_hashtable_t* h, keytype* k, valuetype* v) { return arg_hashtable_insert(h, k, v); }

/**
 * @brief Search the specified key in the hash table.
 *
 * @param   h   the hash table to search
 * @param   k   the key to search for  - does not claim ownership
 * @return      the value associated with the key, or NULL if none found
 */
void* arg_hashtable_search(arg_hashtable_t* h, const void* k);

#define ARG_DEFINE_HASHTABLE_SEARCH(fnname, keytype, valuetype) \
    valuetype* fnname(arg_hashtable_t* h, keytype* k) { return (valuetype*)(arg_hashtable_search(h, k)); }

/**
 * @brief Remove the specified key from the hash table.
 *
 * @param   h   the hash table to remove the item from
 * @param   k   the key to search for  - does not claim ownership
 */
void arg_hashtable_remove(arg_hashtable_t* h, const void* k);

#define ARG_DEFINE_HASHTABLE_REMOVE(fnname, keytype, valuetype) \
    void fnname(arg_hashtable_t* h, keytype* k) { arg_hashtable_remove(h, k); }

/**
 * @brief Return the number of keys in the hash table.
 *
 * @param   h   the hash table
 * @return      the number of items stored in the hash table
 */
unsigned int arg_hashtable_count(arg_hashtable_t* h);

/**
 * @brief Change the value associated with the key.
 *
 * function to change the value associated with a key, where there already
 * exists a value bound to the key in the hash table.
 * Source due to Holger Schemel.
 *
 * @name        hashtable_change
 * @param   h   the hash table
 * @param       key
 * @param       value
 */
int arg_hashtable_change(arg_hashtable_t* h, void* k, void* v);

/**
 * @brief Free the hash table and the memory allocated for each key-value pair.
 *
 * @param   h            the hash table
 * @param   free_values  whether to call 'free' on the remaining values
 */
void arg_hashtable_destroy(arg_hashtable_t* h, int free_values);

typedef struct arg_hashtable_itr {
    arg_hashtable_t* h;
    struct arg_hashtable_entry* e;
    struct arg_hashtable_entry* parent;
    unsigned int index;
} arg_hashtable_itr_t;

arg_hashtable_itr_t* arg_hashtable_itr_create(arg_hashtable_t* h);

void arg_hashtable_itr_destroy(arg_hashtable_itr_t* itr);

/**
 * @brief Return the value of the (key,value) pair at the current position.
 */
extern void* arg_hashtable_itr_key(arg_hashtable_itr_t* i);

/**
 * @brief Return the value of the (key,value) pair at the current position.
 */
extern void* arg_hashtable_itr_value(arg_hashtable_itr_t* i);

/**
 * @brief Advance the iterator to the next element. Returns zero if advanced to end of table.
 */
int arg_hashtable_itr_advance(arg_hashtable_itr_t* itr);

/**
 * @brief Remove current element and advance the iterator to the next element.
 */
int arg_hashtable_itr_remove(arg_hashtable_itr_t* itr);

/**
 * @brief Search and overwrite the supplied iterator, to point to the entry matching the supplied key.
 *
 * @return  Zero if not found.
 */
int arg_hashtable_itr_search(arg_hashtable_itr_t* itr, arg_hashtable_t* h, void* k);

#define ARG_DEFINE_HASHTABLE_ITERATOR_SEARCH(fnname, keytype) \
    int fnname(arg_hashtable_itr_t* i, arg_hashtable_t* h, keytype* k) { return (arg_hashtable_iterator_search(i, h, k)); }

#ifdef __cplusplus
}
#endif

#endif
