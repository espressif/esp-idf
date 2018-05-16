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
#include <stdint.h>
#include "common/bt_trace.h"
#include "osi/allocator.h"
#include "osi/buffer.h"
#include "common/bt_defs.h"
#include "common/bt_trace.h"

struct buffer_t {
    buffer_t *root;
    size_t refcount;
    size_t length;
    uint8_t data[];
};

buffer_t *buffer_new(size_t size)
{
    assert(size > 0);

    buffer_t *buffer = osi_malloc(sizeof(buffer_t) + size);
    if (!buffer) {
        OSI_TRACE_ERROR("%s unable to allocate buffer of %zu bytes.", __func__, size);
        return NULL;
    }

    buffer->root = buffer;
    buffer->refcount = 1;
    buffer->length = size;

    return buffer;
}

buffer_t *buffer_new_ref(const buffer_t *buf)
{
    assert(buf != NULL);
    return buffer_new_slice(buf, buf->length);
}

buffer_t *buffer_new_slice(const buffer_t *buf, size_t slice_size)
{
    assert(buf != NULL);
    assert(slice_size > 0);
    assert(slice_size <= buf->length);

    buffer_t *ret = osi_calloc(sizeof(buffer_t));
    if (!ret) {
        OSI_TRACE_ERROR("%s unable to allocate new buffer for slice of length %zu.", __func__, slice_size);
        return NULL;
    }

    ret->root = buf->root;
    ret->refcount = SIZE_MAX;
    ret->length = slice_size;

    ++buf->root->refcount;

    return ret;
}

void buffer_free(buffer_t *buffer)
{
    if (!buffer) {
        return;
    }

    if (buffer->root != buffer) {
        // We're a leaf node. Delete the root node if we're the last referent.
        if (--buffer->root->refcount == 0) {
            osi_free(buffer->root);
        }
        osi_free(buffer);
    } else if (--buffer->refcount == 0) {
        // We're a root node. Roots are only deleted when their refcount goes to 0.
        osi_free(buffer);
    }
}

void *buffer_ptr(const buffer_t *buf)
{
    assert(buf != NULL);
    return buf->root->data + buf->root->length - buf->length;
}

size_t buffer_length(const buffer_t *buf)
{
    assert(buf != NULL);
    return buf->length;
}
