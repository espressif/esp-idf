/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "unity.h"

#define CXX_UNITY_TYPE_TO_STR(x) #x

/**
 * Very simple helper macro to catch exceptions.
 *
 * @note
 *  * If there is any exception which not a child of std::exception, it will terminate the program!
 *  * If there is no exception, it will jump from the current frame without de-initializing
 *    destructors!
 */
#define TEST_THROW(expr_, exception_) \
    do { \
        bool caught = false; \
        bool caught_different = false; \
        try { \
            expr_; \
        } catch ( exception_ &e) { \
            caught = true; \
        } catch ( std::exception &e) { \
            caught_different = true; \
        } \
        TEST_ASSERT_FALSE_MESSAGE(caught_different, "ERROR: Expected " CXX_UNITY_TYPE_TO_STR(exception_) \
                ", but caught different exception."); \
        TEST_ASSERT_TRUE_MESSAGE(caught, "ERROR: Expected " CXX_UNITY_TYPE_TO_STR(exception_) \
                ", but no exception thrown."); \
    } \
    while (0)
