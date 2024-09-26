/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/param.h>

/* realpath logic:
 * 1. prepend CWD (/)
 * 2. iterate over components (search until next '/' or end of line)
 *    - empty, skip the component
 *    - if it is '.', skip the component
 *    - if it is '..'
 *      - and out_level == 0, ??? ('/..')
 *      - otherwise, reverse-search for '/', set out_pos to that - 1, decrement out_level
 *    - otherwise, add the component to output, increment out_level
 */

char * realpath(const char *file_name, char *resolved_name)
{
    char * out_path = resolved_name;
    if (out_path == NULL) {
        /* allowed as an extension, allocate memory for the output path */
        out_path = malloc(PATH_MAX);
        if (out_path == NULL) {
            errno = ENOMEM;
            return NULL;
        }
    }

    /* canonical path starts with / */
    strlcpy(out_path, "/", PATH_MAX);

    /* pointers moving over the input and output path buffers */
    const char* in_ptr = file_name;
    char* out_ptr = out_path + 1;
    /* number of path components in the output buffer */
    size_t out_depth = 0;

    while (*in_ptr) {
        /* "path component" is the part between two '/' path separators.
         * locate the next path component in the input path:
         */
        const char* end_of_path_component = strchrnul(in_ptr, '/');
        size_t path_component_len = end_of_path_component - in_ptr;

        if (path_component_len == 0 ||
                (path_component_len == 1 && in_ptr[0] == '.')) {
            /* empty path component or '.' - nothing to do */
        } else if (path_component_len == 2 && in_ptr[0] == '.' && in_ptr[1] == '.') {
            /* '..' - remove one path component from the output */
            if (out_depth == 0) {
                /* nothing to remove */
            } else if (out_depth == 1) {
                /* there is only one path component in output;
                 * remove it, but keep the leading separator
                 */
                out_ptr = out_path + 1;
                *out_ptr = '\0';
                out_depth = 0;
            } else {
                /* remove last path component and the separator preceding it */
                char * prev_sep = strrchr(out_path, '/');
                assert(prev_sep > out_path);  /* this shouldn't be the leading separator */
                out_ptr = prev_sep;
                *out_ptr = '\0';
                --out_depth;
            }
        } else {
            /* copy path component to output; +1 is for the separator  */
            if (out_ptr - out_path + 1 + path_component_len > PATH_MAX - 1) {
                /* output buffer insufficient */
                errno = E2BIG;
                goto fail;
            } else {
                /* add separator if necessary */
                if (out_depth > 0) {
                    *out_ptr = '/';
                    ++out_ptr;
                }
                memcpy(out_ptr, in_ptr, path_component_len);
                out_ptr += path_component_len;
                *out_ptr = '\0';
                ++out_depth;
            }
        }
        /* move input pointer to separator right after this path component */
        in_ptr += path_component_len;
        if (*in_ptr != '\0') {
            /* move past it unless already at the end of the input string */
            ++in_ptr;
        }
    }
    return out_path;

fail:
    if (resolved_name == NULL) {
        /* out_path was allocated, free it */
        free(out_path);
    }
    return NULL;
}

char * getcwd(char *buf, size_t size)
{
    if (buf == NULL) {
        return strdup("/");
    }
    strlcpy(buf, "/", size);
    return buf;
}

int chdir(const char *path)
{
    (void) path;
    errno = ENOSYS;
    return -1;
}
