/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_cpu.h"
#include "ulp_lp_core_print.h"

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

static void __ubsan_maybe_debugbreak(void)
{
}

__attribute__((noreturn)) static void __ubsan_default_handler(struct source_location *loc, const char *func)
{
#if CONFIG_ULP_PANIC_OUTPUT_ENABLE
    lp_core_printf("LP_CORE: Undefined behavior of type '%s' @\r\n"
                   "%s:%d\r\n", func, loc->file_name, loc->line);
#endif
    abort();
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
