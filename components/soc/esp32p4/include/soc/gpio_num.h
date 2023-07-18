/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GPIO number
 */
typedef enum {
    GPIO_NUM_NC = -1,    /*!< Use to signal not connected to S/W */
    GPIO_NUM_0 = 0,     /*!< GPIO0, input and output */
    GPIO_NUM_1 = 1,     /*!< GPIO1, input and output */
    GPIO_NUM_2 = 2,     /*!< GPIO2, input and output */
    GPIO_NUM_3 = 3,     /*!< GPIO3, input and output */
    GPIO_NUM_4 = 4,     /*!< GPIO4, input and output */
    GPIO_NUM_5 = 5,     /*!< GPIO5, input and output */
    GPIO_NUM_6 = 6,     /*!< GPIO6, input and output */
    GPIO_NUM_7 = 7,     /*!< GPIO7, input and output */
    GPIO_NUM_8 = 8,     /*!< GPIO8, input and output */
    GPIO_NUM_9 = 9,     /*!< GPIO9, input and output */
    GPIO_NUM_10 = 10,   /*!< GPIO10, input and output */
    GPIO_NUM_11 = 11,   /*!< GPIO11, input and output */
    GPIO_NUM_12 = 12,   /*!< GPIO12, input and output */
    GPIO_NUM_13 = 13,   /*!< GPIO13, input and output */
    GPIO_NUM_14 = 14,   /*!< GPIO14, input and output */
    GPIO_NUM_15 = 15,   /*!< GPIO15, input and output */
    GPIO_NUM_16 = 16,   /*!< GPIO16, input and output */
    GPIO_NUM_17 = 17,   /*!< GPIO17, input and output */
    GPIO_NUM_18 = 18,   /*!< GPIO18, input and output */
    GPIO_NUM_19 = 19,   /*!< GPIO19, input and output */
    GPIO_NUM_20 = 20,   /*!< GPIO20, input and output */
    GPIO_NUM_21 = 21,   /*!< GPIO21, input and output */
    GPIO_NUM_22 = 22,   /*!< GPIO22, input and output */
    GPIO_NUM_23 = 23,   /*!< GPIO23, input and output */
    GPIO_NUM_24 = 24,   /*!< GPIO24, input and output */
    GPIO_NUM_25 = 25,   /*!< GPIO25, input and output */
    GPIO_NUM_26 = 26,   /*!< GPIO26, input and output */
    GPIO_NUM_27 = 27,   /*!< GPIO27, input and output */
    GPIO_NUM_28 = 28,   /*!< GPIO28, input and output */
    GPIO_NUM_29 = 29,   /*!< GPIO29, input and output */
    GPIO_NUM_30 = 30,   /*!< GPIO30, input and output */
    GPIO_NUM_31 = 31,   /*!< GPIO31, input and output */
    GPIO_NUM_32 = 32,   /*!< GPIO32, input and output */
    GPIO_NUM_33 = 33,   /*!< GPIO33, input and output */
    GPIO_NUM_34 = 34,   /*!< GPIO34, input and output */
    GPIO_NUM_35 = 35,   /*!< GPIO35, input and output */
    GPIO_NUM_36 = 36,   /*!< GPIO36, input and output */
    GPIO_NUM_37 = 37,   /*!< GPIO37, input and output */
    GPIO_NUM_38 = 38,   /*!< GPIO38, input and output */
    GPIO_NUM_39 = 39,   /*!< GPIO39, input and output */
    GPIO_NUM_40 = 40,   /*!< GPIO40, input and output */
    GPIO_NUM_41 = 41,   /*!< GPIO41, input and output */
    GPIO_NUM_42 = 42,   /*!< GPIO42, input and output */
    GPIO_NUM_43 = 43,   /*!< GPIO43, input and output */
    GPIO_NUM_44 = 44,   /*!< GPIO44, input and output */
    GPIO_NUM_45 = 45,   /*!< GPIO45, input and output */
    GPIO_NUM_46 = 46,   /*!< GPIO46, input and output */
    GPIO_NUM_47 = 47,   /*!< GPIO47, input and output */
    GPIO_NUM_48 = 48,   /*!< GPIO48, input and output */
    GPIO_NUM_49 = 49,   /*!< GPIO49, input and output */
    GPIO_NUM_50 = 50,   /*!< GPIO50, input and output */
    GPIO_NUM_51 = 51,   /*!< GPIO51, input and output */
    GPIO_NUM_52 = 52,   /*!< GPIO52, input and output */
    GPIO_NUM_53 = 53,   /*!< GPIO53, input and output */
    GPIO_NUM_54 = 54,   /*!< GPIO54, input and output */
    GPIO_NUM_55 = 55,   /*!< GPIO55, input and output */
    GPIO_NUM_56 = 56,   /*!< GPIO56, input and output */
    GPIO_NUM_MAX,
} gpio_num_t;

#ifdef __cplusplus
}
#endif
