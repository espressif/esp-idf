// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

#include "esp_err.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Macro which can be used to check the error code. If the code is not ESP_OK, it prints the message and returns.
 */
#if defined(CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT)
#define ESP_RETURN_ON_ERROR(x, log_tag, format, ...) do {                                       \
        (void)log_tag;                                                                          \
        esp_err_t err_rc_ = (x);                                                                \
        if (unlikely(err_rc_ != ESP_OK)) {                                                      \
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
            return err_rc_;                                                                     \
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
            return err_code;                                                                    \
        }                                                                                       \
    } while(0)

/**
 * A version of ESP_RETURN_ON_FALSE() macro that can be called from ISR.
 */
#define ESP_RETURN_ON_FALSE_ISR(a, err_code, log_tag, format, ...) do {                         \
        (void)log_tag;                                                                          \
        if (unlikely(!(a))) {                                                                   \
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
            ret = err_code;                                                                     \
            goto goto_tag;                                                                      \
        }                                                                                       \
    } while (0)

#else // !CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT

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

#endif // !CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT


#ifdef __cplusplus
}
#endif
