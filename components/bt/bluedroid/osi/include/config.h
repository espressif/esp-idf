// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __CONFIG_H__
#define __CONFIG_H__

// This module implements a configuration parser. Clients can query the
// contents of a configuration file through the interface provided here.
// The current implementation is read-only; mutations are only kept in
// memory. This parser supports the INI file format.

// Implementation notes:
// - Key/value pairs that are not within a section are assumed to be under
//   the |CONFIG_DEFAULT_SECTION| section.
// - Multiple sections with the same name will be merged as if they were in
//   a single section.
// - Empty sections with no key/value pairs will be treated as if they do
//   not exist. In other words, |config_has_section| will return false for
//   empty sections.
// - Duplicate keys in a section will overwrite previous values.
// - All strings are case sensitive.

#include <stdbool.h>

// The default section name to use if a key/value pair is not defined within
// a section.
#define CONFIG_DEFAULT_SECTION "Global"

typedef struct config_t config_t;
typedef struct config_section_node_t config_section_node_t;

// Creates a new config object with no entries (i.e. not backed by a file).
// This function returns a config object or NULL on error. Clients must call
// |config_free| on the returned handle when it is no longer required.
config_t *config_new_empty(void);

// Loads the specified file and returns a handle to the config file. If there
// was a problem loading the file or allocating memory, this function returns
// NULL. Clients must call |config_free| on the returned handle when it is no
// longer required. |filename| must not be NULL and must point to a readable
// file on the filesystem.
config_t *config_new(const char *filename);

// Frees resources associated with the config file. No further operations may
// be performed on the |config| object after calling this function. |config|
// may be NULL.
void config_free(config_t *config);

// Returns true if the config file contains a section named |section|. If
// the section has no key/value pairs in it, this function will return false.
// |config| and |section| must not be NULL.
bool config_has_section(const config_t *config, const char *section);

// Returns true if the config file has a key named |key| under |section|.
// Returns false otherwise. |config|, |section|, and |key| must not be NULL.
bool config_has_key(const config_t *config, const char *section, const char *key);

// Returns true if the config file has a key named |key| and the key_value.
// Returns false otherwise. |config|, |key|, and |key_value| must not be NULL.
bool config_has_key_in_section(config_t *config, const char *key, char *key_value);

// Returns the integral value for a given |key| in |section|. If |section|
// or |key| do not exist, or the value cannot be fully converted to an integer,
// this function returns |def_value|. |config|, |section|, and |key| must not
// be NULL.
int config_get_int(const config_t *config, const char *section, const char *key, int def_value);

// Returns the boolean value for a given |key| in |section|. If |section|
// or |key| do not exist, or the value cannot be converted to a boolean, this
// function returns |def_value|. |config|, |section|, and |key| must not be NULL.
bool config_get_bool(const config_t *config, const char *section, const char *key, bool def_value);

// Returns the string value for a given |key| in |section|. If |section| or
// |key| do not exist, this function returns |def_value|. The returned string
// is owned by the config module and must not be freed. |config|, |section|,
// and |key| must not be NULL. |def_value| may be NULL.
const char *config_get_string(const config_t *config, const char *section, const char *key, const char *def_value);

// Sets an integral value for the |key| in |section|. If |key| or |section| do
// not already exist, this function creates them. |config|, |section|, and |key|
// must not be NULL.
void config_set_int(config_t *config, const char *section, const char *key, int value);

// Sets a boolean value for the |key| in |section|. If |key| or |section| do
// not already exist, this function creates them. |config|, |section|, and |key|
// must not be NULL.
void config_set_bool(config_t *config, const char *section, const char *key, bool value);

// Sets a string value for the |key| in |section|. If |key| or |section| do
// not already exist, this function creates them. |config|, |section|, |key|, and
// |value| must not be NULL.
void config_set_string(config_t *config, const char *section, const char *key, const char *value, bool insert_back);

// Removes |section| from the |config| (and, as a result, all keys in the section).
// Returns true if |section| was found and removed from |config|, false otherwise.
// Neither |config| nor |section| may be NULL.
bool config_remove_section(config_t *config, const char *section);

// Removes one specific |key| residing in |section| of the |config|. Returns true
// if the section and key were found and the key was removed, false otherwise.
// None of |config|, |section|, or |key| may be NULL.
bool config_remove_key(config_t *config, const char *section, const char *key);

// Returns an iterator to the first section in the config file. If there are no
// sections, the iterator will equal the return value of |config_section_end|.
// The returned pointer must be treated as an opaque handle and must not be freed.
// The iterator is invalidated on any config mutating operation. |config| may not
// be NULL.
const config_section_node_t *config_section_begin(const config_t *config);

// Returns an iterator to one past the last section in the config file. It does not
// represent a valid section, but can be used to determine if all sections have been
// iterated over. The returned pointer must be treated as an opaque handle and must
// not be freed and must not be iterated on (must not call |config_section_next| on
// it). |config| may not be NULL.
const config_section_node_t *config_section_end(const config_t *config);

// Moves |iter| to the next section. If there are no more sections, |iter| will
// equal the value of |config_section_end|. |iter| may not be NULL and must be
// a pointer returned by either |config_section_begin| or |config_section_next|.
const config_section_node_t *config_section_next(const config_section_node_t *iter);

// Returns the name of the section referred to by |iter|. The returned pointer is
// owned by the config module and must not be freed by the caller. The pointer will
// remain valid until |config_free| is called. |iter| may not be NULL and must not
// equal the value returned by |config_section_end|.
const char *config_section_name(const config_section_node_t *iter);

// Saves |config| to a file given by |filename|. Note that this could be a destructive
// operation: if |filename| already exists, it will be overwritten. The config
// module does not preserve comments or formatting so if a config file was opened
// with |config_new| and subsequently overwritten with |config_save|, all comments
// and special formatting in the original file will be lost. Neither |config| nor
// |filename| may be NULL.
bool config_save(const config_t *config, const char *filename);

#endif /* #ifndef __CONFIG_H__ */
