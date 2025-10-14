/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "regi2c_pmu.h"

/**
 * @file regi2c_brownout.h
 * @brief Register definitions for brownout detector
 *
 * This file lists register fields of the brownout detector, located on an internal configuration
 * bus. These definitions are used via macros defined in regi2c_ctrl.h.
 */

#define I2C_BOD            I2C_PMU
#define I2C_BOD_HOSTID     I2C_PMU_HOSTID

#define I2C_PMU_OR_DREF_VBAT_L       17
#define I2C_PMU_OR_DREF_VBAT_L_MSB   4
#define I2C_PMU_OR_DREF_VBAT_L_LSB   2
#define I2C_PMU_OR_DREF_VBAT_H       17
#define I2C_PMU_OR_DREF_VBAT_H_MSB   7
#define I2C_PMU_OR_DREF_VBAT_H_LSB   5

#define I2C_PMU_OR_XPD_DIGDET       18
#define I2C_PMU_OR_XPD_DIGDET_MSB   0
#define I2C_PMU_OR_XPD_DIGDET_MSB   0

#define I2C_PMU_OR_DREFL_DIGDET       18
#define I2C_PMU_OR_DREFL_DIGDET_MSB   4
#define I2C_PMU_OR_DREFL_DIGDET_LSB   2
#define I2C_PMU_OR_DREFH_DIGDET       18
#define I2C_PMU_OR_DREFH_DIGDET_MSB   7
#define I2C_PMU_OR_DREFH_DIGDET_LSB   5

#define I2C_PMU_OR_DREFL_VDDA         19
#define I2C_PMU_OR_DREFL_VDDA_MSB     4
#define I2C_PMU_OR_DREFL_VDDA_LSB     2
#define I2C_PMU_OR_DREFH_VDDA         19
#define I2C_PMU_OR_DREFH_VDDA_MSB     7
#define I2C_PMU_OR_DREFH_VDDA_LSB     5

#define I2C_BIAS_OR_DREF_VBAT_CHARGER_L       20
#define I2C_BIAS_OR_DREF_VBAT_CHARGER_L_MSB   4
#define I2C_BIAS_OR_DREF_VBAT_CHARGER_L_LSB   2
#define I2C_BIAS_OR_DREF_VBAT_CHARGER_H       20
#define I2C_BIAS_OR_DREF_VBAT_CHARGER_H_MSB   7
#define I2C_BIAS_OR_DREF_VBAT_CHARGER_H_LSB   5

#define I2C_BIAS_OR_FORCE_PU_VBAT_CHARGER 20
#define I2C_BIAS_OR_FORCE_PU_VBAT_CHARGER_MSB 0
#define I2C_BIAS_OR_FORCE_PU_VBAT_CHARGER_LSB 0

/** Change the charging current by adjusting the value of the series resistor. 1kΩ + N * 0.5kΩ */
#define I2C_BIAS_OR_DRES_CHARGER     15
#define I2C_BIAS_OR_DRES_CHARGER_MSB 7
#define I2C_BIAS_OR_DRES_CHARGER_LSB 4

#define I2C_BOD_THRESHOLD_L           I2C_PMU_OR_DREFL_VDDA
#define I2C_BOD_THRESHOLD_L_MSB       I2C_PMU_OR_DREFL_VDDA_MSB
#define I2C_BOD_THRESHOLD_L_LSB       I2C_PMU_OR_DREFL_VDDA_LSB

#define I2C_BOD_THRESHOLD_H           I2C_PMU_OR_DREFH_VDDA
#define I2C_BOD_THRESHOLD_H_MSB       I2C_PMU_OR_DREFH_VDDA_MSB
#define I2C_BOD_THRESHOLD_H_LSB       I2C_PMU_OR_DREFH_VDDA_LSB
