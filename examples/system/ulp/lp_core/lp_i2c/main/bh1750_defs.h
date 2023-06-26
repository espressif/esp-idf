/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/***************************************************
 * BH1750 Register Addresses
 ***************************************************/
#define BH1750_I2C_ADDR                 0x23
#define BH1750_POWER_ON                 0x01
#define BH1750_CONTINUOUS_H_RES_MODE    0x10
#define BH1750_CONTINUOUS_H_RES_MODE2   0x11
#define BH1750_CONTINUOUS_L_RES_MODE    0x13
#define BH1750_ONE_TIME_H_RES_MODE      0x20
#define BH1750_ONE_TIME_H_RES_MODE2     0x21
#define BH1750_ONE_TIME_L_RES_MODE      0x23

/***************************************************
 * Example configurations
 ***************************************************/
#define EXAMPLE_RES_MODE BH1750_CONTINUOUS_H_RES_MODE
