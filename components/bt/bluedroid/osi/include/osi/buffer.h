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

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct buffer_t buffer_t;

// Returns a new buffer of |size| bytes. Returns NULL if a buffer could not be
// allocated. |size| must be non-zero. The caller must release this buffer with
// |buffer_free|.
buffer_t *buffer_new(size_t size);

// Creates a new reference to the buffer |buf|. A reference is indistinguishable
// from the original: writes to the original will be reflected in the reference
// and vice versa. In other words, this function creates an alias to |buf|. The
// caller must release the returned buffer with |buffer_free|. Note that releasing
// the returned buffer does not release |buf|. |buf| must not be NULL.
buffer_t *buffer_new_ref(const buffer_t *buf);

// Creates a new reference to the last |slice_size| bytes of |buf|. See
// |buffer_new_ref| for a description of references. |slice_size| must be
// greater than 0 and may be at most |buffer_length|
// (0 < slice_size <= buffer_length). |buf| must not be NULL.
buffer_t *buffer_new_slice(const buffer_t *buf, size_t slice_size);

// Frees a buffer object. |buf| may be NULL.
void buffer_free(buffer_t *buf);

// Returns a pointer to a writeable memory region for |buf|. All references
// and slices that share overlapping bytes will also be written to when
// writing to the returned pointer. The caller may safely write up to
// |buffer_length| consecutive bytes starting at the address returned by
// this function. |buf| must not be NULL.
void *buffer_ptr(const buffer_t *buf);

// Returns the length of the writeable memory region referred to by |buf|.
// |buf| must not be NULL.
size_t buffer_length(const buffer_t *buf);

#endif /*_BUFFER_H_*/
