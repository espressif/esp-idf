// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "ssl_dbg.h"

struct err_error_st {
    /* file contains the filename where the error occurred. */
    const char *file;
    /* packed contains the error library and reason, as packed by ERR_PACK. */
    uint32_t packed;
    /* line contains the line number where the error occurred. */
    uint32_t line;
};

#define ERR_NUM_ERRORS 4

typedef struct err_state_st {
    /* errors contains the ERR_NUM_ERRORS most recent errors, organised as a ring
     * buffer. */
    struct err_error_st errors[ERR_NUM_ERRORS];
    /* top contains the index one past the most recent error. If |top| equals
     * |bottom| then the queue is empty. */
    unsigned top;
    /* bottom contains the index of the last error in the queue. */
    unsigned bottom;
} ERR_STATE;

#if CONFIG_OPENSSL_ERROR_STACK
static ERR_STATE s_err_state = { 0 };
#endif

void ERR_clear_error(void)
{
#if CONFIG_OPENSSL_ERROR_STACK
    memset(&s_err_state.errors[0], 0, sizeof(struct err_state_st));
    s_err_state.top = s_err_state.bottom = 0;
#endif
}

static uint32_t ERR_get_peek_error_internal(const char **file, int *line, bool peak)
{
#if CONFIG_OPENSSL_ERROR_STACK
    if (s_err_state.top == s_err_state.bottom) {
        return 0;
    }
    unsigned new_bottom = (s_err_state.bottom + 1) % ERR_NUM_ERRORS;
    int err = s_err_state.errors[new_bottom].packed;

    if (file) {
        *file = s_err_state.errors[new_bottom].file;
    }
    if (line) {
        *line = s_err_state.errors[new_bottom].line;
    }

    if (peak == false) {
        memset(&s_err_state.errors[new_bottom], 0, sizeof(struct err_error_st));
        s_err_state.bottom = new_bottom;
    }

    return err;
#else
    return 0;
#endif
}

uint32_t ERR_get_error(void)
{
    return ERR_get_peek_error_internal(NULL, NULL, false);
}

uint32_t ERR_peek_error(void)
{
    return ERR_get_peek_error_internal(NULL, NULL, true);
}

uint32_t ERR_peek_last_error(void)
{
    return ERR_get_peek_error_internal(NULL, NULL, true);
}

uint32_t ERR_peek_error_line_data(const char **file, int *line, const char **data, int *flags)
{
    return ERR_get_peek_error_internal(file, line, true);
}

uint32_t ERR_get_error_line_data(const char **file, int *line, const char **data, int *flags)
{
    return ERR_get_peek_error_internal(file, line, false);
}

const char *ERR_reason_error_string(uint32_t packed_error)
{
    return NULL;
}

void ERR_put_error(int library, int unused, int reason, const char *file, unsigned line)
{
#if CONFIG_OPENSSL_ERROR_STACK
    s_err_state.top = (s_err_state.top + 1) % ERR_NUM_ERRORS;
    if (s_err_state.top == s_err_state.bottom) {
        s_err_state.bottom = (s_err_state.bottom + 1) % ERR_NUM_ERRORS;
    }

    s_err_state.errors[s_err_state.top].packed = (uint32_t)library<<24 | abs(reason);
    s_err_state.errors[s_err_state.top].file = file;
    s_err_state.errors[s_err_state.top].line = line;
#endif
}
