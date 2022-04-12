/*
 * SPDX-FileCopyrightText: 2013-2019 Tom G. Huang
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*******************************************************************************
 * arg_hashtable: Implements the hash table utilities
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

#ifndef ARG_AMALGAMATION
#include "argtable3_private.h"
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * This hash table module is adapted from the C hash table implementation by
 * Christopher Clark. Here is the copyright notice from the library:
 *
 * Copyright (c) 2002, Christopher Clark
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * * Neither the name of the original author; nor the names of any contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Credit for primes table: Aaron Krowne
 * http://br.endernet.org/~akrowne/
 * http://planetmath.org/encyclopedia/GoodHashTablePrimes.html
 */
static const unsigned int primes[] = {53,       97,       193,      389,       769,       1543,      3079,      6151,      12289,
                                      24593,    49157,    98317,    196613,    393241,    786433,    1572869,   3145739,   6291469,
                                      12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};
const unsigned int prime_table_length = sizeof(primes) / sizeof(primes[0]);
const float max_load_factor = (float)0.65;

static unsigned int enhanced_hash(arg_hashtable_t* h, const void* k) {
    /*
     * Aim to protect against poor hash functions by adding logic here.
     * The logic is taken from Java 1.4 hash table source.
     */
    unsigned int i = h->hashfn(k);
    i += ~(i << 9);
    i ^= ((i >> 14) | (i << 18)); /* >>> */
    i += (i << 4);
    i ^= ((i >> 10) | (i << 22)); /* >>> */
    return i;
}

static unsigned int index_for(unsigned int tablelength, unsigned int hashvalue) {
    return (hashvalue % tablelength);
}

arg_hashtable_t* arg_hashtable_create(unsigned int minsize, unsigned int (*hashfn)(const void*), int (*eqfn)(const void*, const void*)) {
    arg_hashtable_t* h;
    unsigned int pindex;
    unsigned int size = primes[0];

    /* Check requested hash table isn't too large */
    if (minsize > (1u << 30))
        return NULL;

    /*
     * Enforce size as prime. The reason is to avoid clustering of values
     * into a small number of buckets (yes, distribution). A more even
     *  distributed hash table will perform more consistently.
     */
    for (pindex = 0; pindex < prime_table_length; pindex++) {
        if (primes[pindex] > minsize) {
            size = primes[pindex];
            break;
        }
    }

    h = (arg_hashtable_t*)xmalloc(sizeof(arg_hashtable_t));
    h->table = (struct arg_hashtable_entry**)xmalloc(sizeof(struct arg_hashtable_entry*) * size);
    memset(h->table, 0, size * sizeof(struct arg_hashtable_entry*));
    h->tablelength = size;
    h->primeindex = pindex;
    h->entrycount = 0;
    h->hashfn = hashfn;
    h->eqfn = eqfn;
    h->loadlimit = (unsigned int)ceil(size * (double)max_load_factor);
    return h;
}

static int arg_hashtable_expand(arg_hashtable_t* h) {
    /* Double the size of the table to accommodate more entries */
    struct arg_hashtable_entry** newtable;
    struct arg_hashtable_entry* e;
    unsigned int newsize;
    unsigned int i;
    unsigned int index;

    /* Check we're not hitting max capacity */
    if (h->primeindex == (prime_table_length - 1))
        return 0;
    newsize = primes[++(h->primeindex)];

    newtable = (struct arg_hashtable_entry**)xmalloc(sizeof(struct arg_hashtable_entry*) * newsize);
    memset(newtable, 0, newsize * sizeof(struct arg_hashtable_entry*));
    /*
     * This algorithm is not 'stable': it reverses the list
     * when it transfers entries between the tables
     */
    for (i = 0; i < h->tablelength; i++) {
        while (NULL != (e = h->table[i])) {
            h->table[i] = e->next;
            index = index_for(newsize, e->h);
            e->next = newtable[index];
            newtable[index] = e;
        }
    }

    xfree(h->table);
    h->table = newtable;
    h->tablelength = newsize;
    h->loadlimit = (unsigned int)ceil(newsize * (double)max_load_factor);
    return -1;
}

unsigned int arg_hashtable_count(arg_hashtable_t* h) {
    return h->entrycount;
}

void arg_hashtable_insert(arg_hashtable_t* h, void* k, void* v) {
    /* This method allows duplicate keys - but they shouldn't be used */
    unsigned int index;
    struct arg_hashtable_entry* e;
    if ((h->entrycount + 1) > h->loadlimit) {
        /*
         * Ignore the return value. If expand fails, we should
         * still try cramming just this value into the existing table
         * -- we may not have memory for a larger table, but one more
         * element may be ok. Next time we insert, we'll try expanding again.
         */
        arg_hashtable_expand(h);
    }
    e = (struct arg_hashtable_entry*)xmalloc(sizeof(struct arg_hashtable_entry));
    e->h = enhanced_hash(h, k);
    index = index_for(h->tablelength, e->h);
    e->k = k;
    e->v = v;
    e->next = h->table[index];
    h->table[index] = e;
    h->entrycount++;
}

void* arg_hashtable_search(arg_hashtable_t* h, const void* k) {
    struct arg_hashtable_entry* e;
    unsigned int hashvalue;
    unsigned int index;

    hashvalue = enhanced_hash(h, k);
    index = index_for(h->tablelength, hashvalue);
    e = h->table[index];
    while (e != NULL) {
        /* Check hash value to short circuit heavier comparison */
        if ((hashvalue == e->h) && (h->eqfn(k, e->k)))
            return e->v;
        e = e->next;
    }
    return NULL;
}

void arg_hashtable_remove(arg_hashtable_t* h, const void* k) {
    /*
     * TODO: consider compacting the table when the load factor drops enough,
     *       or provide a 'compact' method.
     */

    struct arg_hashtable_entry* e;
    struct arg_hashtable_entry** pE;
    unsigned int hashvalue;
    unsigned int index;

    hashvalue = enhanced_hash(h, k);
    index = index_for(h->tablelength, hashvalue);
    pE = &(h->table[index]);
    e = *pE;
    while (NULL != e) {
        /* Check hash value to short circuit heavier comparison */
        if ((hashvalue == e->h) && (h->eqfn(k, e->k))) {
            *pE = e->next;
            h->entrycount--;
            xfree(e->k);
            xfree(e->v);
            xfree(e);
            return;
        }
        pE = &(e->next);
        e = e->next;
    }
}

void arg_hashtable_destroy(arg_hashtable_t* h, int free_values) {
    unsigned int i;
    struct arg_hashtable_entry *e, *f;
    struct arg_hashtable_entry** table = h->table;
    if (free_values) {
        for (i = 0; i < h->tablelength; i++) {
            e = table[i];
            while (NULL != e) {
                f = e;
                e = e->next;
                xfree(f->k);
                xfree(f->v);
                xfree(f);
            }
        }
    } else {
        for (i = 0; i < h->tablelength; i++) {
            e = table[i];
            while (NULL != e) {
                f = e;
                e = e->next;
                xfree(f->k);
                xfree(f);
            }
        }
    }
    xfree(h->table);
    xfree(h);
}

arg_hashtable_itr_t* arg_hashtable_itr_create(arg_hashtable_t* h) {
    unsigned int i;
    unsigned int tablelength;

    arg_hashtable_itr_t* itr = (arg_hashtable_itr_t*)xmalloc(sizeof(arg_hashtable_itr_t));
    itr->h = h;
    itr->e = NULL;
    itr->parent = NULL;
    tablelength = h->tablelength;
    itr->index = tablelength;
    if (0 == h->entrycount)
        return itr;

    for (i = 0; i < tablelength; i++) {
        if (h->table[i] != NULL) {
            itr->e = h->table[i];
            itr->index = i;
            break;
        }
    }
    return itr;
}

void arg_hashtable_itr_destroy(arg_hashtable_itr_t* itr) {
    xfree(itr);
}

void* arg_hashtable_itr_key(arg_hashtable_itr_t* i) {
    return i->e->k;
}

void* arg_hashtable_itr_value(arg_hashtable_itr_t* i) {
    return i->e->v;
}

int arg_hashtable_itr_advance(arg_hashtable_itr_t* itr) {
    unsigned int j;
    unsigned int tablelength;
    struct arg_hashtable_entry** table;
    struct arg_hashtable_entry* next;

    if (itr->e == NULL)
        return 0; /* stupidity check */

    next = itr->e->next;
    if (NULL != next) {
        itr->parent = itr->e;
        itr->e = next;
        return -1;
    }

    tablelength = itr->h->tablelength;
    itr->parent = NULL;
    if (tablelength <= (j = ++(itr->index))) {
        itr->e = NULL;
        return 0;
    }

    table = itr->h->table;
    while (NULL == (next = table[j])) {
        if (++j >= tablelength) {
            itr->index = tablelength;
            itr->e = NULL;
            return 0;
        }
    }

    itr->index = j;
    itr->e = next;
    return -1;
}

int arg_hashtable_itr_remove(arg_hashtable_itr_t* itr) {
    struct arg_hashtable_entry* remember_e;
    struct arg_hashtable_entry* remember_parent;
    int ret;

    /* Do the removal */
    if ((itr->parent) == NULL) {
        /* element is head of a chain */
        itr->h->table[itr->index] = itr->e->next;
    } else {
        /* element is mid-chain */
        itr->parent->next = itr->e->next;
    }
    /* itr->e is now outside the hashtable */
    remember_e = itr->e;
    itr->h->entrycount--;
    xfree(remember_e->k);
    xfree(remember_e->v);

    /* Advance the iterator, correcting the parent */
    remember_parent = itr->parent;
    ret = arg_hashtable_itr_advance(itr);
    if (itr->parent == remember_e) {
        itr->parent = remember_parent;
    }
    xfree(remember_e);
    return ret;
}

int arg_hashtable_itr_search(arg_hashtable_itr_t* itr, arg_hashtable_t* h, void* k) {
    struct arg_hashtable_entry* e;
    struct arg_hashtable_entry* parent;
    unsigned int hashvalue;
    unsigned int index;

    hashvalue = enhanced_hash(h, k);
    index = index_for(h->tablelength, hashvalue);

    e = h->table[index];
    parent = NULL;
    while (e != NULL) {
        /* Check hash value to short circuit heavier comparison */
        if ((hashvalue == e->h) && (h->eqfn(k, e->k))) {
            itr->index = index;
            itr->e = e;
            itr->parent = parent;
            itr->h = h;
            return -1;
        }
        parent = e;
        e = e->next;
    }
    return 0;
}

int arg_hashtable_change(arg_hashtable_t* h, void* k, void* v) {
    struct arg_hashtable_entry* e;
    unsigned int hashvalue;
    unsigned int index;

    hashvalue = enhanced_hash(h, k);
    index = index_for(h->tablelength, hashvalue);
    e = h->table[index];
    while (e != NULL) {
        /* Check hash value to short circuit heavier comparison */
        if ((hashvalue == e->h) && (h->eqfn(k, e->k))) {
            xfree(e->v);
            e->v = v;
            return -1;
        }
        e = e->next;
    }
    return 0;
}
