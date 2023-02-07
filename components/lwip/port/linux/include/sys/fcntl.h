/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#ifdef LWIP_HDR_ESP_LWIPOPTS_H
// ignore when included from lwipopts.h since lwip provides all necessary definitions
#else
// otherwise include system fcntl
#include_next <sys/fcntl.h>
#endif
