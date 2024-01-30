// Copyright (c) 2016, Linaro Limited
// Modified for HelenOS use by Jiří Zárevúcky.
// Adaptations for ESP-IDF Copyright (c) 2020 Espressif Systems (Shanghai) Co. Ltd.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_rom_sys.h"
#include "esp_cpu.h"

struct source_location {
    const char *file_name;
    uint32_t line;
    uint32_t column;
};

struct type_descriptor {
    uint16_t type_kind;
    uint16_t type_info;
    char type_name[];
};

struct type_mismatch_data {
    struct source_location loc;
    struct type_descriptor *type;
    unsigned long alignment;
    unsigned char type_check_kind;
};

struct type_mismatch_data_v1 {
    struct source_location loc;
    struct type_descriptor *type;
    unsigned char log_alignment;
    unsigned char type_check_kind;
};

struct overflow_data {
    struct source_location loc;
    struct type_descriptor *type;
};

struct shift_out_of_bounds_data {
    struct source_location loc;
    struct type_descriptor *lhs_type;
    struct type_descriptor *rhs_type;
};

struct out_of_bounds_data {
    struct source_location loc;
    struct type_descriptor *array_type;
    struct type_descriptor *index_type;
};

struct unreachable_data {
    struct source_location loc;
};

struct vla_bound_data {
    struct source_location loc;
    struct type_descriptor *type;
};

struct invalid_value_data {
    struct source_location loc;
    struct type_descriptor *type;
};

struct nonnull_arg_data {
    struct source_location loc;
};

struct nonnull_return_data {
    struct source_location loc;
    struct source_location attr_loc;
};

struct pointer_overflow_data {
    struct source_location loc;
};

struct invalid_builtin_data {
    struct source_location loc;
    unsigned char kind;
};

static void __ubsan_default_handler(struct source_location *loc, const char *func) __attribute__((noreturn));

/*
 * When compiling with -fsanitize=undefined the compiler expects functions
 * with the following signatures. The functions are never called directly,
 * only when undefined behavior is detected in instrumented code.
 */
void __ubsan_handle_type_mismatch(void *data_, void *ptr_);
void __ubsan_handle_type_mismatch_v1(void *data_, void *ptr_);
void __ubsan_handle_add_overflow(void *data_, void *lhs_, void *rhs_);
void __ubsan_handle_sub_overflow(void *data_, void *lhs_, void *rhs_);
void __ubsan_handle_mul_overflow(void *data_, void *lhs_, void *rhs_);
void __ubsan_handle_negate_overflow(void *data_, void *old_val_);
void __ubsan_handle_divrem_overflow(void *data_, void *lhs_, void *rhs_);
void __ubsan_handle_shift_out_of_bounds(void *data_, void *lhs_, void *rhs_);
void __ubsan_handle_out_of_bounds(void *data_, void *idx_);
void __ubsan_handle_missing_return(void *data_);
void __ubsan_handle_vla_bound_not_positive(void *data_, void *bound_);
void __ubsan_handle_load_invalid_value(void *data_, void *val_);
void __ubsan_handle_nonnull_arg(void *data_);
void __ubsan_handle_nonnull_return(void *data_);
void __ubsan_handle_builtin_unreachable(void *data_);
void __ubsan_handle_pointer_overflow(void *data_, void *base_, void *result_);
void __ubsan_handle_invalid_builtin(void *data_);

static void __ubsan_maybe_debugbreak(void)
{
    if (esp_cpu_dbgr_is_attached()) {
        esp_cpu_dbgr_break();
    }
}

static void __ubsan_default_handler(struct source_location *loc, const char *func)
{
    /* Although the source location is available here, it is not printed:
     *
     * - We could use "snprintf", but that uses a lot of stack, and may allocate memory,
     *   so is not safe from UBSAN handler.
     * - Alternatively, "itoa" could be used. However itoa doesn't take the remaining
     *   string length as as argument and is therefore unsafe (nor does it return
     *   the number of characters written). itoa is also not present in ESP32-S2 ROM,
     *   and would need to be placed into IRAM on that chip.
     * - Third option is to print the message using esp_rom_printf, and not pass anything
     *   to esp_system_abort. However we'd like to capture this information, e.g. for the
     *   purpose of including the abort reason into core dumps.
     *
     * Since the source file and line number are already printed while decoding
     * the panic backtrace, not printing the line number here seems to be an okay choice.
     */
    char msg[60] = {};
    (void) strlcat(msg, "Undefined behavior of type ", sizeof(msg));
    (void) strlcat(msg, func + strlen("__ubsan_handle_"), sizeof(msg));
    esp_system_abort(msg);
}

void __ubsan_handle_type_mismatch(void *data_,
                                  void *ptr_)
{
    struct type_mismatch_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_type_mismatch_v1(void *data_,
                                     void *ptr)
{
    struct type_mismatch_data_v1 *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_add_overflow(void *data_,
                                 void *lhs_,
                                 void *rhs_)
{
    struct overflow_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_sub_overflow(void *data_,
                                 void *lhs_,
                                 void *rhs_)
{
    struct overflow_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_mul_overflow(void *data_,
                                 void *lhs_,
                                 void *rhs_)
{
    struct overflow_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_negate_overflow(void *data_,
                                    void *old_val_)
{
    struct overflow_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_divrem_overflow(void *data_,
                                    void *lhs_,
                                    void *rhs_)
{
    struct overflow_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_shift_out_of_bounds(void *data_,
                                        void *lhs_,
                                        void *rhs_)
{
    struct shift_out_of_bounds_data *data = data_;
    unsigned int rhs = (unsigned int)rhs_;
    if (rhs == 32) {
        return;
    }
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_out_of_bounds(void *data_,
                                  void *idx_)
{
    struct out_of_bounds_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_missing_return(void *data_)
{
    struct unreachable_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_vla_bound_not_positive(void *data_,
                                           void *bound_)
{
    struct vla_bound_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_load_invalid_value(void *data_,
                                       void *val_)
{
    struct invalid_value_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_nonnull_arg(void *data_)
{
    struct nonnull_arg_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_nonnull_return(void *data_)
{
    struct nonnull_return_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_builtin_unreachable(void *data_)
{
    struct unreachable_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_pointer_overflow(void *data_,
                                     void *base_,
                                     void *result_)
{
    struct pointer_overflow_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

void __ubsan_handle_invalid_builtin(void *data_)
{
    struct invalid_builtin_data *data = data_;
    __ubsan_maybe_debugbreak();
    __ubsan_default_handler(&data->loc, __func__);
}

/* Hook for the linker to include this object file */
void __ubsan_include(void)
{
}
