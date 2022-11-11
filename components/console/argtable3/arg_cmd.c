/*
 * SPDX-FileCopyrightText: 2013-2019 Tom G. Huang
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*******************************************************************************
 * arg_cmd: Provides the sub-command mechanism
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

#include "argtable3.h"

#ifndef ARG_AMALGAMATION
#include "argtable3_private.h"
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MODULE_VERSION_SIZE 128

static arg_hashtable_t* s_hashtable = NULL;
static char* s_module_name = NULL;
static int s_mod_ver_major = 0;
static int s_mod_ver_minor = 0;
static int s_mod_ver_patch = 0;
static char* s_mod_ver_tag = NULL;
static char* s_mod_ver = NULL;

void arg_set_module_name(const char* name) {
    size_t slen;

    xfree(s_module_name);
    slen = strlen(name);
    s_module_name = (char*)xmalloc(slen + 1);
    memset(s_module_name, 0, slen + 1);

#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__)) || (defined(__STDC_SECURE_LIB__) && defined(__STDC_WANT_SECURE_LIB__))
    strncpy_s(s_module_name, slen + 1, name, slen);
#else
    memcpy(s_module_name, name, slen);
#endif
}

void arg_set_module_version(int major, int minor, int patch, const char* tag) {
    size_t slen_tag, slen_ds;
    arg_dstr_t ds;

    s_mod_ver_major = major;
    s_mod_ver_minor = minor;
    s_mod_ver_patch = patch;

    xfree(s_mod_ver_tag);
    slen_tag = strlen(tag);
    s_mod_ver_tag = (char*)xmalloc(slen_tag + 1);
    memset(s_mod_ver_tag, 0, slen_tag + 1);

#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__)) || (defined(__STDC_SECURE_LIB__) && defined(__STDC_WANT_SECURE_LIB__))
    strncpy_s(s_mod_ver_tag, slen_tag + 1, tag, slen_tag);
#else
    memcpy(s_mod_ver_tag, tag, slen_tag);
#endif

    ds = arg_dstr_create();
    arg_dstr_catf(ds, "%d.", s_mod_ver_major);
    arg_dstr_catf(ds, "%d.", s_mod_ver_minor);
    arg_dstr_catf(ds, "%d.", s_mod_ver_patch);
    arg_dstr_cat(ds, s_mod_ver_tag);

    xfree(s_mod_ver);
    slen_ds = strlen(arg_dstr_cstr(ds));
    s_mod_ver = (char*)xmalloc(slen_ds + 1);
    memset(s_mod_ver, 0, slen_ds + 1);

#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__)) || (defined(__STDC_SECURE_LIB__) && defined(__STDC_WANT_SECURE_LIB__))
    strncpy_s(s_mod_ver, slen_ds + 1, arg_dstr_cstr(ds), slen_ds);
#else
    memcpy(s_mod_ver, arg_dstr_cstr(ds), slen_ds);
#endif

    arg_dstr_destroy(ds);
}

static unsigned int hash_key(const void* key) {
    const char* str = (const char*)key;
    int c;
    unsigned int hash = 5381;

    while ((c = *str++) != 0)
        hash = ((hash << 5) + hash) + (unsigned int)c; /* hash * 33 + c */

    return hash;
}

static int equal_keys(const void* key1, const void* key2) {
    char* k1 = (char*)key1;
    char* k2 = (char*)key2;
    return (0 == strcmp(k1, k2));
}

void arg_cmd_init(void) {
    s_hashtable = arg_hashtable_create(32, hash_key, equal_keys);
}

void arg_cmd_uninit(void) {
    arg_hashtable_destroy(s_hashtable, 1);
}

void arg_cmd_register(const char* name, arg_cmdfn* proc, const char* description) {
    arg_cmd_info_t* cmd_info;
    size_t slen_name;
    void* k;

    assert(strlen(name) < ARG_CMD_NAME_LEN);
    assert(strlen(description) < ARG_CMD_DESCRIPTION_LEN);

    /* Check if the command already exists. */
    /* If the command exists, replace the existing command. */
    /* If the command doesn't exist, insert the command. */
    cmd_info = (arg_cmd_info_t*)arg_hashtable_search(s_hashtable, name);
    if (cmd_info) {
        arg_hashtable_remove(s_hashtable, name);
        cmd_info = NULL;
    }

    cmd_info = (arg_cmd_info_t*)xmalloc(sizeof(arg_cmd_info_t));
    memset(cmd_info, 0, sizeof(arg_cmd_info_t));

#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__)) || (defined(__STDC_SECURE_LIB__) && defined(__STDC_WANT_SECURE_LIB__))
    strncpy_s(cmd_info->name, ARG_CMD_NAME_LEN, name, strlen(name));
    strncpy_s(cmd_info->description, ARG_CMD_DESCRIPTION_LEN, description, strlen(description));
#else
    memcpy(cmd_info->name, name, strlen(name));
    memcpy(cmd_info->description, description, strlen(description));
#endif

    cmd_info->proc = proc;

    slen_name = strlen(name);
    k = xmalloc(slen_name + 1);
    memset(k, 0, slen_name + 1);

#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__)) || (defined(__STDC_SECURE_LIB__) && defined(__STDC_WANT_SECURE_LIB__))
    strncpy_s((char*)k, slen_name + 1, name, slen_name);
#else
    memcpy((char*)k, name, slen_name);
#endif

    arg_hashtable_insert(s_hashtable, k, cmd_info);
}

void arg_cmd_unregister(const char* name) {
    arg_hashtable_remove(s_hashtable, name);
}

int arg_cmd_dispatch(const char* name, int argc, char* argv[], arg_dstr_t res) {
    arg_cmd_info_t* cmd_info = arg_cmd_info(name);

    assert(cmd_info != NULL);
    assert(cmd_info->proc != NULL);

    return cmd_info->proc(argc, argv, res);
}

arg_cmd_info_t* arg_cmd_info(const char* name) {
    return (arg_cmd_info_t*)arg_hashtable_search(s_hashtable, name);
}

unsigned int arg_cmd_count(void) {
    return arg_hashtable_count(s_hashtable);
}

arg_cmd_itr_t arg_cmd_itr_create(void) {
    return (arg_cmd_itr_t)arg_hashtable_itr_create(s_hashtable);
}

int arg_cmd_itr_advance(arg_cmd_itr_t itr) {
    return arg_hashtable_itr_advance((arg_hashtable_itr_t*)itr);
}

char* arg_cmd_itr_key(arg_cmd_itr_t itr) {
    return (char*)arg_hashtable_itr_key((arg_hashtable_itr_t*)itr);
}

arg_cmd_info_t* arg_cmd_itr_value(arg_cmd_itr_t itr) {
    return (arg_cmd_info_t*)arg_hashtable_itr_value((arg_hashtable_itr_t*)itr);
}

void arg_cmd_itr_destroy(arg_cmd_itr_t itr) {
    arg_hashtable_itr_destroy((arg_hashtable_itr_t*)itr);
}

int arg_cmd_itr_search(arg_cmd_itr_t itr, void* k) {
    return arg_hashtable_itr_search((arg_hashtable_itr_t*)itr, s_hashtable, k);
}

static const char* module_name(void) {
    if (s_module_name == NULL || strlen(s_module_name) == 0)
        return "<name>";

    return s_module_name;
}

static const char* module_version(void) {
    if (s_mod_ver == NULL || strlen(s_mod_ver) == 0)
        return "0.0.0.0";

    return s_mod_ver;
}

void arg_make_get_help_msg(arg_dstr_t res) {
    arg_dstr_catf(res, "%s v%s\n", module_name(), module_version());
    arg_dstr_catf(res, "Please type '%s help' to get more information.\n", module_name());
}

void arg_make_help_msg(arg_dstr_t ds, char* cmd_name, void** argtable) {
    arg_cmd_info_t* cmd_info = (arg_cmd_info_t*)arg_hashtable_search(s_hashtable, cmd_name);
    if (cmd_info) {
        arg_dstr_catf(ds, "%s: %s\n", cmd_name, cmd_info->description);
    }

    arg_dstr_cat(ds, "Usage:\n");
    arg_dstr_catf(ds, "  %s", module_name());

    arg_print_syntaxv_ds(ds, argtable, "\n \nAvailable options:\n");
    arg_print_glossary_ds(ds, argtable, "  %-23s %s\n");

    arg_dstr_cat(ds, "\n");
}

void arg_make_syntax_err_msg(arg_dstr_t ds, void** argtable, struct arg_end* end) {
    arg_print_errors_ds(ds, end, module_name());
    arg_dstr_cat(ds, "Usage: \n");
    arg_dstr_catf(ds, "  %s", module_name());
    arg_print_syntaxv_ds(ds, argtable, "\n");
    arg_dstr_cat(ds, "\n");
}

int arg_make_syntax_err_help_msg(arg_dstr_t ds, char* name, int help, int nerrors, void** argtable, struct arg_end* end, int* exitcode) {
    /* help handling
     * note: '-h|--help' takes precedence over error reporting
     */
    if (help > 0) {
        arg_make_help_msg(ds, name, argtable);
        *exitcode = EXIT_SUCCESS;
        return 1;
    }

    /* syntax error handling */
    if (nerrors > 0) {
        arg_make_syntax_err_msg(ds, argtable, end);
        *exitcode = EXIT_FAILURE;
        return 1;
    }

    return 0;
}
