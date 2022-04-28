/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define I2C_ULP           0x61
#define I2C_ULP_HOSTID    0

#define I2C_ULP_IR_RESETB        0
#define I2C_ULP_IR_RESETB_MSB    0
#define I2C_ULP_IR_RESETB_LSB    0

#define I2C_ULP_XPD_REG_SLP        0
#define I2C_ULP_XPD_REG_SLP_MSB    1
#define I2C_ULP_XPD_REG_SLP_LSB    1

#define I2C_ULP_DBIAS_SLP        0
#define I2C_ULP_DBIAS_SLP_MSB    7
#define I2C_ULP_DBIAS_SLP_LSB    4

#define I2C_ULP_IR_FORCE_XPD_BIAS_BUF        1
#define I2C_ULP_IR_FORCE_XPD_BIAS_BUF_MSB    1
#define I2C_ULP_IR_FORCE_XPD_BIAS_BUF_LSB    1

#define I2C_ULP_IR_FORCE_XPD_IPH        1
#define I2C_ULP_IR_FORCE_XPD_IPH_MSB    2
#define I2C_ULP_IR_FORCE_XPD_IPH_LSB    2

#define I2C_ULP_IR_FORCE_XPD_VGATE_BUF        1
#define I2C_ULP_IR_FORCE_XPD_VGATE_BUF_MSB    3
#define I2C_ULP_IR_FORCE_XPD_VGATE_BUF_LSB    3

#define I2C_ULP_IR_FORCE_DISABLE_BIAS_SLEEP        1
#define I2C_ULP_IR_FORCE_DISABLE_BIAS_SLEEP_MSB    4
#define I2C_ULP_IR_FORCE_DISABLE_BIAS_SLEEP_LSB    4

#define I2C_ULP_IR_ZOS_XPD        2
#define I2C_ULP_IR_ZOS_XPD_MSB    0
#define I2C_ULP_IR_ZOS_XPD_LSB    0

#define I2C_ULP_IR_ZOS_RSTB        2
#define I2C_ULP_IR_ZOS_RSTB_MSB    1
#define I2C_ULP_IR_ZOS_RSTB_LSB    1

#define I2C_ULP_IR_ZOS_RESTART        2
#define I2C_ULP_IR_ZOS_RESTART_MSB    2
#define I2C_ULP_IR_ZOS_RESTART_LSB    2

#define I2C_ULP_DTEST        3
#define I2C_ULP_DTEST_MSB    1
#define I2C_ULP_DTEST_LSB    0

#define I2C_ULP_ENT_BG        3
#define I2C_ULP_ENT_BG_MSB    2
#define I2C_ULP_ENT_BG_LSB    2

#define I2C_ULP_MODE_LVDET        3
#define I2C_ULP_MODE_LVDET_MSB    3
#define I2C_ULP_MODE_LVDET_LSB    3

#define I2C_ULP_DREF_LVDET        3
#define I2C_ULP_DREF_LVDET_MSB    6
#define I2C_ULP_DREF_LVDET_LSB    4

#define I2C_ULP_XPD_LVDET        3
#define I2C_ULP_XPD_LVDET_MSB    7
#define I2C_ULP_XPD_LVDET_LSB    7

#define I2C_ULP_INT_XPD_XTAL_CK_DIG_REG        4
#define I2C_ULP_INT_XPD_XTAL_CK_DIG_REG_MSB    0
#define I2C_ULP_INT_XPD_XTAL_CK_DIG_REG_LSB    0

#define I2C_ULP_INT_XPD_XTAL_BUF        4
#define I2C_ULP_INT_XPD_XTAL_BUF_MSB    1
#define I2C_ULP_INT_XPD_XTAL_BUF_LSB    1

#define I2C_ULP_INT_XPD_RC_CK        4
#define I2C_ULP_INT_XPD_RC_CK_MSB    2
#define I2C_ULP_INT_XPD_RC_CK_LSB    2

#define I2C_ULP_XTAL_DPHASE        4
#define I2C_ULP_XTAL_DPHASE_MSB    3
#define I2C_ULP_XTAL_DPHASE_LSB    3

#define I2C_ULP_INT_XPD_XTAL_LIN_REG        4
#define I2C_ULP_INT_XPD_XTAL_LIN_REG_MSB    4
#define I2C_ULP_INT_XPD_XTAL_LIN_REG_LSB    4

#define I2C_ULP_XTAL_RESTART_DC_CAL        4
#define I2C_ULP_XTAL_RESTART_DC_CAL_MSB    5
#define I2C_ULP_XTAL_RESTART_DC_CAL_LSB    5

#define I2C_ULP_XTAL_DAC        5
#define I2C_ULP_XTAL_DAC_MSB    3
#define I2C_ULP_XTAL_DAC_LSB    0

#define I2C_ULP_XTAL_DBLEED        6
#define I2C_ULP_XTAL_DBLEED_MSB    4
#define I2C_ULP_XTAL_DBLEED_LSB    0

#define I2C_ULP_XTAL_CAL_DONE        6
#define I2C_ULP_XTAL_CAL_DONE_MSB    5
#define I2C_ULP_XTAL_CAL_DONE_LSB    5

#define I2C_ULP_ZOS_DONE        6
#define I2C_ULP_ZOS_DONE_MSB    6
#define I2C_ULP_ZOS_DONE_LSB    6
