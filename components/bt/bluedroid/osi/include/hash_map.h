/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef _HASH_MAP_H_
#define _HASH_MAP_H_

#include <stdbool.h>
#include <stdint.h>

struct hash_map_t;
typedef struct hash_map_t hash_map_t;

typedef struct hash_map_entry_t {
    const void *key;
    void *data;
    const hash_map_t *hash_map;
} hash_map_entry_t;

typedef size_t hash_index_t;

// Takes a key structure and returns a hash value.
typedef hash_index_t (*hash_index_fn)(const void *key);
typedef bool (*hash_map_iter_cb)(hash_map_entry_t *hash_entry, void *context);

typedef bool (*key_equality_fn)(const void *x, const void *y);

typedef void (*key_free_fn)(void *data);
typedef void (*data_free_fn)(void *data);

// Returns a new, empty hash_map. Returns NULL if not enough memory could be allocated
// for the hash_map structure. The returned hash_map must be freed with |hash_map_free|.
// The |num_bucket| specifies the number of hashable buckets for the map and must not
// be zero.  The |hash_fn| specifies a hash function to be used and must not be NULL.
// The |key_fn| and |data_fn| are called whenever a hash_map element is removed from
// the hash_map. They can be used to release resources held by the hash_map element,
// e.g.  memory or file descriptor.  |key_fn| and |data_fn| may be NULL if no cleanup
// is necessary on element removal. |equality_fn| is used to check for key equality.
// If |equality_fn| is NULL, default pointer equality is used.
hash_map_t *hash_map_new(
    size_t size,
    hash_index_fn hash_fn,
    key_free_fn key_fn,
    data_free_fn data_fn,
    key_equality_fn equality_fn);

// Frees the hash_map. This function accepts NULL as an argument, in which case it
// behaves like a no-op.
void hash_map_free(hash_map_t *hash_map);

// Returns true if the hash_map is empty (has no elements), false otherwise.
// Note that a NULL |hash_map| is not the same as an empty |hash_map|. This function
// does not accept a NULL |hash_map|.
//bool hash_map_is_empty(const hash_map_t *hash_map);

// Returns the number of elements in the hash map.  This function does not accept a
// NULL |hash_map|.
//size_t hash_map_size(const hash_map_t *hash_map);

// Returns the number of buckets in the hash map.  This function does not accept a
// NULL |hash_map|.
//size_t hash_map_num_buckets(const hash_map_t *hash_map);

// Returns true if the hash_map has a valid entry for the presented key.
// This function does not accept a NULL |hash_map|.
bool hash_map_has_key(const hash_map_t *hash_map, const void *key);

// Returns the element indexed by |key| in the hash_map without removing it. |hash_map|
// may not be NULL.  Returns NULL if no entry indexed by |key|.
void *hash_map_get(const hash_map_t *hash_map, const void *key);

// Sets the value |data| indexed by |key| into the |hash_map|. Neither |data| nor
// |hash_map| may be NULL.  This function does not make copies of |data| nor |key|
// so the pointers must remain valid at least until the element is removed from the
// hash_map or the hash_map is freed.  Returns true if |data| could be set, false
// otherwise (e.g. out of memory).
bool hash_map_set(hash_map_t *hash_map, const void *key, void *data);

// Removes data indexed by |key| from the hash_map. |hash_map| may not be NULL.
// If |key_fn| or |data_fn| functions were specified in |hash_map_new|, they
// will be called back with |key| or |data| respectively. This function returns true
// if |key| was found in the hash_map and removed, false otherwise.
bool hash_map_erase(hash_map_t *hash_map, const void *key);

// Removes all elements in the hash_map. Calling this function will return the hash_map
// to the same state it was in after |hash_map_new|. |hash_map| may not be NULL.
void hash_map_clear(hash_map_t *hash_map);

// Iterates through the entire |hash_map| and calls |callback| for each data
// element and passes through the |context| argument. If the hash_map is
// empty, |callback| will never be called. It is not safe to mutate the
// hash_map inside the callback. Neither |hash_map| nor |callback| may be NULL.
// If |callback| returns false, the iteration loop will immediately exit.
void hash_map_foreach(hash_map_t *hash_map, hash_map_iter_cb callback, void *context);

#endif /* _HASH_MAP_H_ */
