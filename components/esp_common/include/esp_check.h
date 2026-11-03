/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @cond */
#if CONFIG_COMPILER_OPTIMIZATION_CHECKS_ADDRESS_ONLY
// Report a failed silent check by its address, which addr2line resolves to the check's file and line.
// `rc` is the error that failed the check, or ESP_OK when a condition was false.
void _esp_check_failed(const char *tag, esp_err_t rc);
// A version of _esp_check_failed() that can be called from an ISR or while the cache is disabled
void _esp_check_failed_isr(esp_err_t rc);
// A failed check is identified by the return address of its call to the reporter, so each check
// needs a call of its own. Two optimisations would take that away: the call becoming a tail call,
// which reports the caller instead, and the failure paths of checks that return the same error
// being merged into one shared call, which reports one of those checks for all of them. The empty
// asm after the call prevents both: code following the call keeps it out of tail position, and an
// operand unique to each check makes every failure path different, so none can be merged.
#define _ESP_CHECK_FAILED(log_tag, rc) do { _esp_check_failed(log_tag, rc); __asm__ __volatile__("" :: "i"(__COUNTER__)); } while (0)
#define _ESP_CHECK_FAILED_ISR(rc) do { _esp_check_failed_isr(rc); __asm__ __volatile__("" :: "i"(__COUNTER__)); } while (0)
#else
#define _ESP_CHECK_FAILED(log_tag, rc) do { } while (0)
#define _ESP_CHECK_FAILED_ISR(rc) do { } while (0)
#endif
/** @endcond */

/**
 * Macro which can be used to check the error code. If the code is not ESP_OK, it prints the message and returns.
 */
#if defined(CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT) || defined(CONFIG_COMPILER_OPTIMIZATION_CHECKS_ADDRESS_ONLY)
#define ESP_RETURN_ON_ERROR(x, log_tag, format, ...) do {                                       \
        (void)log_tag;                                                                          \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            _ESP_CHECK_FAILED(log_tag, err_rc_);                                                \
            return err_rc_;                                                                     \
        }                                                                                       \
    } while(0)

/**
 * A version of ESP_RETURN_ON_ERROR() macro that can be called from ISR.
 */
#define ESP_RETURN_ON_ERROR_ISR(x, log_tag, format, ...) do {                                   \
        (void)log_tag;                                                                          \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            _ESP_CHECK_FAILED_ISR(err_rc_);                                                     \
            return err_rc_;                                                                     \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the error code. If the code is not ESP_OK, it prints the message and returns.
 * This macro is used when the function returns void.
 */
#define ESP_RETURN_VOID_ON_ERROR(x, log_tag, format, ...) do {                                         \
        (void)log_tag;                                                                          \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            _ESP_CHECK_FAILED(log_tag, err_rc_);                                                \
            return;                                                                             \
        }                                                                                       \
    } while(0)

/**
 * A version of ESP_RETURN_VOID_ON_ERROR() macro that can be called from ISR.
 */
#define ESP_RETURN_VOID_ON_ERROR_ISR(x, log_tag, format, ...) do {                                     \
        (void)log_tag;                                                                          \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            _ESP_CHECK_FAILED_ISR(err_rc_);                                                     \
            return;                                                                             \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the error code. If the code is not ESP_OK, it prints the message,
 * sets the local variable 'ret' to the code, and then exits by jumping to 'goto_tag'.
 */
#define ESP_GOTO_ON_ERROR(x, goto_tag, log_tag, format, ...) do {                               \
        (void)log_tag;                                                                          \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            _ESP_CHECK_FAILED(log_tag, err_rc_);                                                \
            ret = err_rc_;                                                                      \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while(0)

/**
 * A version of ESP_GOTO_ON_ERROR() macro that can be called from ISR.
 */
#define ESP_GOTO_ON_ERROR_ISR(x, goto_tag, log_tag, format, ...) do {                           \
        (void)log_tag;                                                                          \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            _ESP_CHECK_FAILED_ISR(err_rc_);                                                     \
            ret = err_rc_;                                                                      \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message
 * and returns with the supplied 'err_code'.
 */
#define ESP_RETURN_ON_FALSE(a, err_code, log_tag, format, ...) do {                             \
        (void)log_tag;                                                                          \
        if (unlikely(!(a))) {                                                                   \
            _ESP_CHECK_FAILED(log_tag, ESP_OK);                                                 \
            return err_code;                                                                    \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message
 * and returns without a value.
 */
#define ESP_RETURN_VOID_ON_FALSE(a, log_tag, format, ...) do {                                  \
        (void)log_tag;                                                                          \
        if (unlikely(!(a))) {                                                                   \
            _ESP_CHECK_FAILED(log_tag, ESP_OK);                                                 \
            return;                                                                             \
        }                                                                                       \
    } while(0)

/**
 * A version of ESP_RETURN_ON_FALSE() macro that can be called from ISR.
 */
#define ESP_RETURN_VOID_ON_FALSE_ISR(a, log_tag, format, ...) do {                                  \
        (void)log_tag;                                                                          \
        if (unlikely(!(a))) {                                                                   \
            _ESP_CHECK_FAILED_ISR(ESP_OK);                                                      \
            return;                                                                             \
        }                                                                                       \
    } while(0)

/**
 * A version of ESP_RETURN_ON_FALSE() macro that can be called from ISR.
 */
#define ESP_RETURN_ON_FALSE_ISR(a, err_code, log_tag, format, ...) do {                         \
        (void)log_tag;                                                                          \
        if (unlikely(!(a))) {                                                                   \
            _ESP_CHECK_FAILED_ISR(ESP_OK);                                                      \
            return err_code;                                                                    \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message,
 * sets the local variable 'ret' to the supplied 'err_code', and then exits by jumping to 'goto_tag'.
 */
#define ESP_GOTO_ON_FALSE(a, err_code, goto_tag, log_tag, format, ...) do {                     \
        (void)log_tag;                                                                          \
        if (unlikely(!(a))) {                                                                   \
            _ESP_CHECK_FAILED(log_tag, ESP_OK);                                                 \
            ret = err_code;                                                                     \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while (0)

/**
 * A version of ESP_GOTO_ON_FALSE() macro that can be called from ISR.
 */
#define ESP_GOTO_ON_FALSE_ISR(a, err_code, goto_tag, log_tag, format, ...) do {                 \
        (void)log_tag;                                                                          \
        if (unlikely(!(a))) {                                                                   \
            _ESP_CHECK_FAILED_ISR(ESP_OK);                                                      \
            ret = err_code;                                                                     \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while (0)

#else // CONFIG_COMPILER_OPTIMIZATION_CHECKS_ENABLE

/**
 * In the future, we want to switch to C++20. We also want to become compatible with clang.
 * Hence, we provide two versions of the following macros. The first one is using the GNU extension \#\#__VA_ARGS__.
 * The second one is using the C++20 feature __VA_OPT__(,). This allows users to compile their code with
 * standard C++20 enabled instead of the GNU extension. Below C++20, we haven't found any good alternative to
 * using \#\#__VA_ARGS__.
 */
#if defined(__cplusplus) && (__cplusplus >  201703L)

/**
 * Macro which can be used to check the error code. If the code is not ESP_OK, it prints the message and returns.
 */
#define ESP_RETURN_ON_ERROR(x, log_tag, format, ...) do {                                                  \
        esp_err_t err_rc_ = (x);                                                                           \
        if (unlikely(err_rc_ != ESP_OK)) {                                                                 \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);        \
            return err_rc_;                                                                                \
        }                                                                                                  \
    } while(0)

/**
 * A version of ESP_RETURN_ON_ERROR() macro that can be called from ISR.
 */
#define ESP_RETURN_ON_ERROR_ISR(x, log_tag, format, ...) do {                                              \
        esp_err_t err_rc_ = (x);                                                                           \
        if (unlikely(err_rc_ != ESP_OK)) {                                                                 \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);  \
            return err_rc_;                                                                                \
        }                                                                                                  \
    } while(0)

/**
 * Macro which can be used to check the error code. If the code is not ESP_OK, it prints the message and returns.
 * This macro is used when the function returns void.
 */
#define ESP_RETURN_VOID_ON_ERROR(x, log_tag, format, ...) do {                                             \
        esp_err_t err_rc_ = (x);                                                                           \
        if (unlikely(err_rc_ != ESP_OK)) {                                                                 \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);        \
            return;                                                                                        \
        }                                                                                                  \
    } while(0)

/**
 * A version of ESP_RETURN_VOID_ON_ERROR() macro that can be called from ISR.
 */
#define ESP_RETURN_VOID_ON_ERROR_ISR(x, log_tag, format, ...) do {                                         \
        esp_err_t err_rc_ = (x);                                                                           \
        if (unlikely(err_rc_ != ESP_OK)) {                                                                 \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);  \
            return;                                                                                        \
        }                                                                                                  \
    } while(0)

/**
 * Macro which can be used to check the error code. If the code is not ESP_OK, it prints the message,
 * sets the local variable 'ret' to the code, and then exits by jumping to 'goto_tag'.
 */
#define ESP_GOTO_ON_ERROR(x, goto_tag, log_tag, format, ...) do {                                          \
        esp_err_t err_rc_ = (x);                                                                           \
        if (unlikely(err_rc_ != ESP_OK)) {                                                                 \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);        \
            ret = err_rc_;                                                                                 \
            goto goto_tag;                                                                                 \
        }                                                                                                  \
    } while(0)

/**
 * A version of ESP_GOTO_ON_ERROR() macro that can be called from ISR.
 */
#define ESP_GOTO_ON_ERROR_ISR(x, goto_tag, log_tag, format, ...) do {                                      \
        esp_err_t err_rc_ = (x);                                                                           \
        if (unlikely(err_rc_ != ESP_OK)) {                                                                 \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);  \
            ret = err_rc_;                                                                                 \
            goto goto_tag;                                                                                 \
        }                                                                                                  \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message
 * and returns with the supplied 'err_code'.
 */
#define ESP_RETURN_ON_FALSE(a, err_code, log_tag, format, ...) do {                                        \
        if (unlikely(!(a))) {                                                                              \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);        \
            return err_code;                                                                               \
        }                                                                                                  \
    } while(0)

/**
 * A version of ESP_RETURN_ON_FALSE() macro that can be called from ISR.
 */
#define ESP_RETURN_ON_FALSE_ISR(a, err_code, log_tag, format, ...) do {                                    \
        if (unlikely(!(a))) {                                                                              \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);  \
            return err_code;                                                                               \
        }                                                                                                  \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message
 * and returns without a value.
 */
#define ESP_RETURN_VOID_ON_FALSE(a, err_code, log_tag, format, ...) do {                                   \
        if (unlikely(!(a))) {                                                                              \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);        \
            return;                                                                                        \
        }                                                                                                  \
    } while(0)

/**
 * A version of ESP_RETURN_VOID_ON_FALSE() macro that can be called from ISR.
 */
#define ESP_RETURN_VOID_ON_FALSE_ISR(a, log_tag, format, ...) do {                                        \
        if (unlikely(!(a))) {                                                                              \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);  \
            return;                                                                                        \
        }                                                                                                  \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message,
 * sets the local variable 'ret' to the supplied 'err_code', and then exits by jumping to 'goto_tag'.
 */
#define ESP_GOTO_ON_FALSE(a, err_code, goto_tag, log_tag, format, ...) do {                                \
        if (unlikely(!(a))) {                                                                              \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);        \
            ret = err_code;                                                                                \
            goto goto_tag;                                                                                 \
        }                                                                                                  \
    } while (0)

/**
 * A version of ESP_GOTO_ON_FALSE() macro that can be called from ISR.
 */
#define ESP_GOTO_ON_FALSE_ISR(a, err_code, goto_tag, log_tag, format, ...) do {                            \
        if (unlikely(!(a))) {                                                                              \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);  \
            ret = err_code;                                                                                \
            goto goto_tag;                                                                                 \
        }                                                                                                  \
    } while (0)

#else // !(defined(__cplusplus) && (__cplusplus >  201703L))

/**
 * Macro which can be used to check the error code. If the code is not ESP_OK, it prints the message and returns.
 */
#define ESP_RETURN_ON_ERROR(x, log_tag, format, ...) do {                                       \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            return err_rc_;                                                                     \
        }                                                                                       \
    } while(0)

/**
 * A version of ESP_RETURN_ON_ERROR() macro that can be called from ISR.
 */
#define ESP_RETURN_ON_ERROR_ISR(x, log_tag, format, ...) do {                                   \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
            return err_rc_;                                                                     \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the error code. If the code is not ESP_OK, it prints the message and returns.
 * This macro is used when the function returns void.
 */
#define ESP_RETURN_VOID_ON_ERROR(x, log_tag, format, ...) do {                                  \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            return;                                                                             \
        }                                                                                       \
    } while(0)

/**
 * A version of ESP_RETURN_VOID_ON_ERROR() macro that can be called from ISR.
 */
#define ESP_RETURN_VOID_ON_ERROR_ISR(x, log_tag, format, ...) do {                              \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
            return;                                                                             \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the error code. If the code is not ESP_OK, it prints the message,
 * sets the local variable 'ret' to the code, and then exits by jumping to 'goto_tag'.
 */
#define ESP_GOTO_ON_ERROR(x, goto_tag, log_tag, format, ...) do {                               \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            ret = err_rc_;                                                                      \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while(0)

/**
 * A version of ESP_GOTO_ON_ERROR() macro that can be called from ISR.
 */
#define ESP_GOTO_ON_ERROR_ISR(x, goto_tag, log_tag, format, ...) do {                           \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
            ret = err_rc_;                                                                      \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message
 * and returns with the supplied 'err_code'.
 */
#define ESP_RETURN_ON_FALSE(a, err_code, log_tag, format, ...) do {                             \
        if (unlikely(!(a))) {                                                                   \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            return err_code;                                                                    \
        }                                                                                       \
    } while(0)

/**
 * A version of ESP_RETURN_ON_FALSE() macro that can be called from ISR.
 */
#define ESP_RETURN_ON_FALSE_ISR(a, err_code, log_tag, format, ...) do {                         \
        if (unlikely(!(a))) {                                                                   \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
            return err_code;                                                                    \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message
 * and returns without a value.
 */
#define ESP_RETURN_VOID_ON_FALSE(a, log_tag, format, ...) do {                                  \
        if (unlikely(!(a))) {                                                                   \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            return;                                                                             \
        }                                                                                       \
    } while(0)

/**
 * A version of ESP_RETURN_VOID_ON_FALSE() macro that can be called from ISR.
 */
#define ESP_RETURN_VOID_ON_FALSE_ISR(a, log_tag, format, ...) do {                              \
        if (unlikely(!(a))) {                                                                   \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
            return;                                                                             \
        }                                                                                       \
    } while(0)

/**
 * Macro which can be used to check the condition. If the condition is not 'true', it prints the message,
 * sets the local variable 'ret' to the supplied 'err_code', and then exits by jumping to 'goto_tag'.
 */
#define ESP_GOTO_ON_FALSE(a, err_code, goto_tag, log_tag, format, ...) do {                     \
        if (unlikely(!(a))) {                                                                   \
            ESP_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);        \
            ret = err_code;                                                                     \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while (0)

/**
 * A version of ESP_GOTO_ON_FALSE() macro that can be called from ISR.
 */
#define ESP_GOTO_ON_FALSE_ISR(a, err_code, goto_tag, log_tag, format, ...) do {                 \
        if (unlikely(!(a))) {                                                                   \
            ESP_EARLY_LOGE(log_tag, "%s(%d): " format, __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
            ret = err_code;                                                                     \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while (0)

#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))

#endif // CONFIG_COMPILER_OPTIMIZATION_CHECKS_ENABLE

/**
 * @brief Evaluate an esp_err_t expression, run cleanup on error, and return.
 *
 * Evaluates @p x once and stores the result in a local variable `err_rc_`.
 * If the result is not ESP_OK, the macro:
 *   - executes the code passed in `__VA_ARGS__` (e.g., cleanup functions or logging),
 *   - returns `err_rc_` from the current function.
 * The cleanup code can freely use `err_rc_`.
 *
 * Example:
 * @code{c}
 * esp_err_t initialize_device(void)
 * {
 *     // Simple case: single cleanup action
 *     ESP_RETURN_ON_ERROR_CLEANUP(open_device("/dev/adc0"), close_device());
 *
 *     // Multiple cleanup actions
 *     ESP_RETURN_ON_ERROR_CLEANUP(
 *         allocate_buffer(4096),
 *         free_buffer(),
 *         close_device(),
 *         printf("Failed to initialize sensor: %s\n", esp_err_to_name(err_rc_))
 *     );
 *
 *     // Complex cleanup with conditional logic and ESP_LOG logging
 *     ESP_RETURN_ON_ERROR_CLEANUP(
 *         calibrate_sensor(),
 *         do {
 *             if (err_rc_ == ESP_ERR_INVALID_STATE) {
 *                ESP_LOGE(TAG, "Sensor in invalid state during calibration");}
 *             }
 *             free_buffer();
 *             close_device();
 *             ESP_LOGE(TAG, "Drop connection");
 *         } while (0)
 *     );
 *
 *     return ESP_OK;
 * }
 * @endcode
 */
#define ESP_RETURN_ON_ERROR_CLEANUP(x, ...) do { \
        esp_err_t err_rc_ = (x);                \
        if (unlikely(err_rc_ != ESP_OK)) {      \
            __VA_ARGS__;                        \
            return err_rc_;                     \
        }                                       \
    } while (0)

#ifdef __cplusplus
}
#endif
