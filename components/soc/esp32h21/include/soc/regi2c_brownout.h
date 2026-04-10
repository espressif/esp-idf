/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#include "regi2c_dcdc.h"

/**
 * @file regi2c_brownout.h
 * @brief Register definitions for brownout detector
 *
 * This file lists register fields of the brownout detector, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h.
 */

#define I2C_BOD            I2C_DCDC
#define I2C_BOD_HOSTID     I2C_DCDC_HOSTID

#define I2C_BOD_THRESHOLD           0xD
#define I2C_BOD_THRESHOLD_MSB       6
#define I2C_BOD_THRESHOLD_LSB       3
