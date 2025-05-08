/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "esp_assert.h"
#include "esp_macros.h"
#include "esp_log_config.h"
#include "sdkconfig.h"

/**
 * @file esp_log_args.h
 * @brief Macros and utilities for managing argument types in binary logging.
 *
 * This header file provides definitions and helper macros for handling
 * and constructing an array of argument types used in binary logging.
 * These utilities facilitate efficient logging by encoding argument type
 * for compact storage.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of argument types for logging.
 *
 * This enumeration defines the types of arguments that can be passed to logging functions.
 */
typedef enum {
    ESP_LOG_ARGS_TYPE_NONE    = 0, /**< Indicates the end of arguments. */
    ESP_LOG_ARGS_TYPE_32BITS  = 1, /**< For variables that are 4 bytes in size, such as int, uint32_t, etc. */
    ESP_LOG_ARGS_TYPE_64BITS  = 2, /**< For variables that are 8 bytes in size, such as uint64_t, double, float, etc. */
    ESP_LOG_ARGS_TYPE_POINTER = 3, /**< For variables that are pointers to strings or other data, such as const char *, const char [], void *. */
} esp_log_args_type_t;

#define  ESP_LOG_ARGS_TYPE_LEN (2) /**< Length of the enumeration of argument types. */
#define  ESP_LOG_ARGS_TYPE_MASK ((1 << ESP_LOG_ARGS_TYPE_LEN) - 1) /*!< Mask for the enumeration of argument types. */

/**
 * @brief Structure to indicate the end of arguments.
 *
 * This structure is used to indicate the end of arguments in the logging functions.
 *
 * @note This structure is intended for internal use only.
 */
typedef struct {
    unsigned tmp;
} esp_log_args_end_t;

#if defined(__cplusplus) && (__cplusplus >  201703L)
#if ESP_LOG_MODE_BINARY_EN
#define ESP_LOG_ARGS(...) , ESP_LOG_ARGS_TYPE(__VA_ARGS__) __VA_OPT__(,) __VA_ARGS__
#else
#define ESP_LOG_ARGS(...) __VA_OPT__(,) __VA_ARGS__
#endif
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#if ESP_LOG_MODE_BINARY_EN
#define ESP_LOG_ARGS(...) , ESP_LOG_ARGS_TYPE(__VA_ARGS__), ##__VA_ARGS__
#else
#define ESP_LOG_ARGS(...) , ##__VA_ARGS__
#endif
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))

/**
 * @brief Defines a static array of argument types for binary logging.
 *
 * This macro creates a static array which consists of char elements,
 * where each char stores up to 4 types of arguments. The array is null-terminated.
 * The format of the array is as follows:
 *
 * - Each element is a packed structure of up to 4 argument types (esp_log_args_t).
 * - The argument type occupies 2 bits (ESP_LOG_ARGS_TYPE_LEN).
 * - If fewer than four arguments are provided, the remaining fields are filled
 *   with ESP_LOG_ARGS_TYPE_NONE.
 * - The array is terminated with a null byte (0) to mark the end of the argument
 *   type list.
 *
 * The pointer to this array is passed to esp_log() in front of the user arguments.
 * This allows the format argument to be excluded from the flash, as esp_log()
 * will rely on this array to extract the correct type of each user argument from the
 * va_list.
 */
#if ESP_LOG_MODE_BINARY_EN
#define ESP_LOG_ARGS_TYPE(...) (__extension__({ static const char __c[] = { __VA_OPT__(ESP_LOG_INIT_ARG_TYPE(ESP_VA_NARG(__VA_ARGS__), __VA_ARGS__), ) 0 }; (const char*)&__c; }))
#else
#define ESP_LOG_ARGS_TYPE(...)
#endif

#define ESP_LOG_INIT_ARG_TYPE_N(n)                ESP_LOG_INIT_ARG_TYPE_##n
#define ESP_LOG_INIT_ARG_TYPE(n, ...)             ESP_LOG_INIT_ARG_TYPE_N(n)(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_1(a)                ESP_LOG_PACK_4_TYPES(a, (esp_log_args_end_t){0}, (esp_log_args_end_t){0}, (esp_log_args_end_t){0})
#define ESP_LOG_INIT_ARG_TYPE_2(a, b)             ESP_LOG_PACK_4_TYPES(a, b, (esp_log_args_end_t){0}, (esp_log_args_end_t){0})
#define ESP_LOG_INIT_ARG_TYPE_3(a, b, c)          ESP_LOG_PACK_4_TYPES(a, b, c, (esp_log_args_end_t){0})
#define ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d)       ESP_LOG_PACK_4_TYPES(a, b, c, d)
#define ESP_LOG_INIT_ARG_TYPE_5(a, b, c, d, ...)  ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_1(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_6(a, b, c, d, ...)  ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_2(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_7(a, b, c, d, ...)  ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_3(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_8(a, b, c, d, ...)  ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_4(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_9(a, b, c, d, ...)  ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_5(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_10(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_6(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_11(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_7(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_12(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_8(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_13(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_9(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_14(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_10(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_15(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_11(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_16(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_12(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_17(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_13(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_18(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_14(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_19(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_15(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_20(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_16(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_21(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_17(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_22(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_18(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_23(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_19(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_24(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_20(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_25(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_21(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_26(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_22(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_27(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_23(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_28(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_24(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_29(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_25(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_30(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_26(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_31(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_27(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_32(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_28(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_33(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_29(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_34(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_30(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_35(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_31(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_36(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_32(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_37(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_33(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_38(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_34(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_39(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_35(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_40(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_36(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_41(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_37(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_42(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_38(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_43(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_39(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_44(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_40(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_45(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_41(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_46(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_42(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_47(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_43(__VA_ARGS__)
#define ESP_LOG_INIT_ARG_TYPE_48(a, b, c, d, ...) ESP_LOG_INIT_ARG_TYPE_4(a, b, c, d), ESP_LOG_INIT_ARG_TYPE_44(__VA_ARGS__)

// Pack 4 types into a single byte (8 bits)
#define ESP_LOG_PACK_4_TYPES(a, b, c, d) (char) ( \
        (ESP_LOG_DETECT_TYPE(a) << (0 * ESP_LOG_ARGS_TYPE_LEN)) | \
        (ESP_LOG_DETECT_TYPE(b) << (1 * ESP_LOG_ARGS_TYPE_LEN)) | \
        (ESP_LOG_DETECT_TYPE(c) << (2 * ESP_LOG_ARGS_TYPE_LEN)) | \
        (ESP_LOG_DETECT_TYPE(d) << (3 * ESP_LOG_ARGS_TYPE_LEN))) \

#ifndef __cplusplus
#define ESP_LOG_DETECT_TYPE(arg) ( \
    _Generic((arg), \
        esp_log_args_end_t: ESP_LOG_ARGS_TYPE_NONE, \
        char*: ESP_LOG_ARGS_TYPE_POINTER, \
        const char*: ESP_LOG_ARGS_TYPE_POINTER, \
        uint8_t*: ESP_LOG_ARGS_TYPE_POINTER, \
        const uint8_t*: ESP_LOG_ARGS_TYPE_POINTER, \
        long long int: ESP_LOG_ARGS_TYPE_64BITS, \
        long long unsigned int: ESP_LOG_ARGS_TYPE_64BITS, \
        double: ESP_LOG_ARGS_TYPE_64BITS, \
        float: ESP_LOG_ARGS_TYPE_64BITS, /* float is a 32 bits var but in va_list it takes 64 bits */ \
        default: ESP_LOG_ARGS_TYPE_32BITS \
    ))
#else // __cplusplus
extern "C++" {
    template <typename T>
    struct EspLogArgType;

    template <>
    struct EspLogArgType<esp_log_args_end_t> {
        constexpr static unsigned long long log_type = ESP_LOG_ARGS_TYPE_NONE;
    };

    template <>
    struct EspLogArgType<char*> {
        constexpr static unsigned long long log_type = ESP_LOG_ARGS_TYPE_POINTER;
    };

    template <>
    struct EspLogArgType<const char*> {
        constexpr static unsigned long long log_type = ESP_LOG_ARGS_TYPE_POINTER;
    };

    template <>
    struct EspLogArgType<uint8_t*> {
        constexpr static unsigned long long log_type = ESP_LOG_ARGS_TYPE_POINTER;
    };

    template <>
    struct EspLogArgType<const uint8_t*> {
        constexpr static unsigned long long log_type = ESP_LOG_ARGS_TYPE_POINTER;
    };

    template <>
    struct EspLogArgType<long long int> {
        constexpr static unsigned long long log_type = ESP_LOG_ARGS_TYPE_64BITS;
    };

    template <>
    struct EspLogArgType<long long unsigned int> {
        constexpr static unsigned long long log_type = ESP_LOG_ARGS_TYPE_64BITS;
    };

    template <>
    struct EspLogArgType<double> {
        constexpr static unsigned long long log_type = ESP_LOG_ARGS_TYPE_64BITS;
    };

    template <>
    struct EspLogArgType<float> {
        constexpr static unsigned long long log_type = ESP_LOG_ARGS_TYPE_64BITS;
    };

    template <typename T>
    struct EspLogArgType {
        constexpr static unsigned long long log_type = ESP_LOG_ARGS_TYPE_32BITS;
    };

    template<typename T>
    constexpr unsigned long long ESP_LOG_DETECT_TYPE(const T &)
    {
        return EspLogArgType<T>::log_type;
    }
}
#endif // __cplusplus

#ifdef __cplusplus
}
#endif
