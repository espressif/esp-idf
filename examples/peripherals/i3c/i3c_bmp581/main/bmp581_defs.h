/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#define BMP581_SET_BITSLICE(regvar, bitname, val) ((regvar & ~bitname##_MSK) | ((val << bitname##_POS) & bitname##_MSK))

#define BMP581_GET_LSB(var)                         (uint8_t)(var & BMP581_SET_LOW_BYTE)
#define BMP581_GET_MSB(var)                         (uint8_t)((var & BMP581_SET_HIGH_BYTE) >> 8)

#define BMP581_SET_BIT_VAL_0(reg_data, bitname)     (reg_data & ~(bitname##_MSK))

#define BMP581_SET_BITS_POS_0(reg_data, bitname, data) ((reg_data & ~(bitname##_MSK)) | (data & bitname##_MSK))

#define BMP581_ENABLE                               0x01
#define BMP581_DISABLE                              0x00

/* Powermode selection */
#define BMP581_POWERMODE_STANDBY                    0x00
#define BMP581_POWERMODE_NORMAL                     0x01
#define BMP581_POWERMODE_FORCED                     0x02
#define BMP581_POWERMODE_CONTINOUS                  0x03
#define BMP581_POWERMODE_DEEP_STANDBY               0x04

/* Register addresses */
#define BMP581_REG_CHIP_ID                          0x01
#define BMP581_REG_REV_ID                           0x02
#define BMP581_REG_CHIP_STATUS                      0x11
#define BMP581_REG_DRIVE_CONFIG                     0x13
#define BMP581_REG_INT_CONFIG                       0x14
#define BMP581_REG_INT_SOURCE                       0x15
#define BMP581_REG_FIFO_CONFIG                      0x16
#define BMP581_REG_FIFO_COUNT                       0x17
#define BMP581_REG_FIFO_SEL                         0x18
#define BMP581_REG_TEMP_DATA_XLSB                   0x1D
#define BMP581_REG_TEMP_DATA_LSB                    0x1E
#define BMP581_REG_TEMP_DATA_MSB                    0x1F
#define BMP581_REG_PRESS_DATA_XLSB                  0x20
#define BMP581_REG_PRESS_DATA_LSB                   0x21
#define BMP581_REG_PRESS_DATA_MSB                   0x22
#define BMP581_REG_INT_STATUS                       0x27
#define BMP581_REG_STATUS                           0x28
#define BMP581_REG_FIFO_DATA                        0x29
#define BMP581_REG_NVM_ADDR                         0x2B
#define BMP581_REG_NVM_DATA_LSB                     0x2C
#define BMP581_REG_NVM_DATA_MSB                     0x2D
#define BMP581_REG_DSP_CONFIG                       0x30
#define BMP581_REG_DSP_IIR                          0x31
#define BMP581_REG_OOR_THR_P_LSB                    0x32
#define BMP581_REG_OOR_THR_P_MSB                    0x33
#define BMP581_REG_OOR_RANGE                        0x34
#define BMP581_REG_OOR_CONFIG                       0x35
#define BMP581_REG_OSR_CONFIG                       0x36
#define BMP581_REG_ODR_CONFIG                       0x37
#define BMP581_REG_OSR_EFF                          0x38
#define BMP581_REG_CMD                              0x7E

/* NVM addresses */
#define BMP581_NVM_START_ADDR                       0x20
#define BMP581_NVM_END_ADDR                         0x22

/* Interface settings */
#define BMP581_SPI_RD_MASK                          0x80

/* Delay definition */
#define BMP581_DELAY_US_SOFT_RESET                  2000
#define BMP581_DELAY_US_STANDBY                     2500
#define BMP581_DELAY_US_NVM_READY_READ              800
#define BMP581_DELAY_US_NVM_READY_WRITE             10000

/* Soft reset command */
#define BMP581_SOFT_RESET_CMD                       0xB6

/* NVM command */
#define BMP581_NVM_FIRST_CMND                       0x5D
#define BMP581_NVM_READ_ENABLE_CMND                 0xA5
#define BMP581_NVM_WRITE_ENABLE_CMND                0xA0

/* Deepstandby enable/disable */
#define BMP581_DEEP_ENABLED                         0
#define BMP581_DEEP_DISABLED                        1

/* ODR settings */
#define BMP581_ODR_240_HZ                           0x00
#define BMP581_ODR_218_5_HZ                         0x01
#define BMP581_ODR_199_1_HZ                         0x02
#define BMP581_ODR_179_2_HZ                         0x03
#define BMP581_ODR_160_HZ                           0x04
#define BMP581_ODR_149_3_HZ                         0x05
#define BMP581_ODR_140_HZ                           0x06
#define BMP581_ODR_129_8_HZ                         0x07
#define BMP581_ODR_120_HZ                           0x08
#define BMP581_ODR_110_1_HZ                         0x09
#define BMP581_ODR_100_2_HZ                         0x0A
#define BMP581_ODR_89_6_HZ                          0x0B
#define BMP581_ODR_80_HZ                            0x0C
#define BMP581_ODR_70_HZ                            0x0D
#define BMP581_ODR_60_HZ                            0x0E
#define BMP581_ODR_50_HZ                            0x0F
#define BMP581_ODR_45_HZ                            0x10
#define BMP581_ODR_40_HZ                            0x11
#define BMP581_ODR_35_HZ                            0x12
#define BMP581_ODR_30_HZ                            0x13
#define BMP581_ODR_25_HZ                            0x14
#define BMP581_ODR_20_HZ                            0x15
#define BMP581_ODR_15_HZ                            0x16
#define BMP581_ODR_10_HZ                            0x17
#define BMP581_ODR_05_HZ                            0x18
#define BMP581_ODR_04_HZ                            0x19
#define BMP581_ODR_03_HZ                            0x1A
#define BMP581_ODR_02_HZ                            0x1B
#define BMP581_ODR_01_HZ                            0x1C
#define BMP581_ODR_0_5_HZ                           0x1D
#define BMP581_ODR_0_250_HZ                         0x1E
#define BMP581_ODR_0_125_HZ                         0x1F

/* Oversampling for temperature and pressure */
#define BMP581_OVERSAMPLING_1X                      0x00
#define BMP581_OVERSAMPLING_2X                      0x01
#define BMP581_OVERSAMPLING_4X                      0x02
#define BMP581_OVERSAMPLING_8X                      0x03
#define BMP581_OVERSAMPLING_16X                     0x04
#define BMP581_OVERSAMPLING_32X                     0x05
#define BMP581_OVERSAMPLING_64X                     0x06
#define BMP581_OVERSAMPLING_128X                    0x07

/* IIR filter for temperature and pressure */
#define BMP581_IIR_FILTER_BYPASS                    0x00
#define BMP581_IIR_FILTER_COEFF_1                   0x01
#define BMP581_IIR_FILTER_COEFF_3                   0x02
#define BMP581_IIR_FILTER_COEFF_7                   0x03
#define BMP581_IIR_FILTER_COEFF_15                  0x04
#define BMP581_IIR_FILTER_COEFF_31                  0x05
#define BMP581_IIR_FILTER_COEFF_63                  0x06
#define BMP581_IIR_FILTER_COEFF_127                 0x07

/*! Fifo frame configuration */
#define BMP581_FIFO_EMPTY                           0X7F
#define BMP581_FIFO_MAX_THRESHOLD_P_T_MODE          0x0F
#define BMP581_FIFO_MAX_THRESHOLD_P_MODE            0x1F

/* Macro is used to bypass both iir_t and iir_p together */
#define BMP581_IIR_BYPASS                           0xC0

/* Pressure Out-of-range count limit */
#define BMP581_OOR_COUNT_LIMIT_1                    0x00
#define BMP581_OOR_COUNT_LIMIT_3                    0x01
#define BMP581_OOR_COUNT_LIMIT_7                    0x02
#define BMP581_OOR_COUNT_LIMIT_15                   0x03

/* Interrupt configurations */
#define BMP581_INT_MODE_PULSED                      0
#define BMP581_INT_MODE_LATCHED                     1

#define BMP581_INT_POL_ACTIVE_LOW                   0
#define BMP581_INT_POL_ACTIVE_HIGH                  1

#define BMP581_INT_OD_PUSHPULL                      0
#define BMP581_INT_OD_OPENDRAIN                     1

/* NVM and Interrupt status asserted macros */
#define BMP581_INT_ASSERTED_DRDY                    0x01
#define BMP581_INT_ASSERTED_FIFO_FULL               0x02
#define BMP581_INT_ASSERTED_FIFO_THRES              0x04
#define BMP581_INT_ASSERTED_PRESSURE_OOR            0x08
#define BMP581_INT_ASSERTED_POR_SOFTRESET_COMPLETE  0x10
#define BMP581_INT_NVM_RDY                          0x02
#define BMP581_INT_NVM_ERR                          0x04
#define BMP581_INT_NVM_CMD_ERR                      0x08

/* Interrupt configurations */
#define BMP581_INT_MODE_MSK                         0x01

#define BMP581_INT_POL_MSK                          0x02
#define BMP581_INT_POL_POS                          1

#define BMP581_INT_OD_MSK                           0x04
#define BMP581_INT_OD_POS                           2

#define BMP581_INT_EN_MSK                           0x08
#define BMP581_INT_EN_POS                           3

#define BMP581_INT_DRDY_EN_MSK                      0x01

#define BMP581_INT_FIFO_FULL_EN_MSK                 0x02
#define BMP581_INT_FIFO_FULL_EN_POS                 1

#define BMP581_INT_FIFO_THRES_EN_MSK                0x04
#define BMP581_INT_FIFO_THRES_EN_POS                2

#define BMP581_INT_OOR_PRESS_EN_MSK                 0x08
#define BMP581_INT_OOR_PRESS_EN_POS                 3

/* ODR configuration */
#define BMP581_ODR_MSK                              0x7C
#define BMP581_ODR_POS                              2

/* OSR configurations */
#define BMP581_TEMP_OS_MSK                          0x07

#define BMP581_PRESS_OS_MSK                         0x38
#define BMP581_PRESS_OS_POS                         3

/* Pressure enable */
#define BMP581_PRESS_EN_MSK                         0x40
#define BMP581_PRESS_EN_POS                         6

/* IIR configurations */
#define BMP581_SET_IIR_TEMP_MSK                     0x07

#define BMP581_SET_IIR_PRESS_MSK                    0x38
#define BMP581_SET_IIR_PRESS_POS                    3

#define BMP581_OOR_SEL_IIR_PRESS_MSK                0x80
#define BMP581_OOR_SEL_IIR_PRESS_POS                7

#define BMP581_SHDW_SET_IIR_TEMP_MSK                0x08
#define BMP581_SHDW_SET_IIR_TEMP_POS                3

#define BMP581_SHDW_SET_IIR_PRESS_MSK               0x20
#define BMP581_SHDW_SET_IIR_PRESS_POS               5

#define BMP581_SET_FIFO_IIR_TEMP_MSK                0x10
#define BMP581_SET_FIFO_IIR_TEMP_POS                4

#define BMP581_SET_FIFO_IIR_PRESS_MSK               0x40
#define BMP581_SET_FIFO_IIR_PRESS_POS               6

#define BMP581_IIR_FLUSH_FORCED_EN_MSK              0x04
#define BMP581_IIR_FLUSH_FORCED_EN_POS              2

/* Effective OSR configurations and ODR valid status */
#define BMP581_OSR_TEMP_EFF_MSK                     0x07

#define BMP581_OSR_PRESS_EFF_MSK                    0x38
#define BMP581_OSR_PRESS_EFF_POS                    3

#define BMP581_ODR_IS_VALID_MSK                     0x80
#define BMP581_ODR_IS_VALID_POS                     7

/* Powermode */
#define BMP581_POWERMODE_MSK                        0x03

#define BMP581_DEEP_DISABLE_MSK                     0x80
#define BMP581_DEEP_DISABLE_POS                     7

/* Fifo configurations */
#define BMP581_FIFO_THRESHOLD_MSK                   0x1F

#define BMP581_FIFO_MODE_MSK                        0x20
#define BMP581_FIFO_MODE_POS                        5

#define BMP581_FIFO_DEC_SEL_MSK                     0x1C
#define BMP581_FIFO_DEC_SEL_POS                     2

#define BMP581_FIFO_COUNT_MSK                       0x3F
#define BMP581_FIFO_FRAME_SEL_MSK                   0x03

/* Out-of-range configuration */
#define BMP581_OOR_THR_P_LSB_MSK                    0x0000FF

#define BMP581_OOR_THR_P_MSB_MSK                    0x00FF00

#define BMP581_OOR_THR_P_XMSB_MSK                   0x010000
#define BMP581_OOR_THR_P_XMSB_POS                   16

/* Macro to mask xmsb value of oor threshold from register(0x35) value */
#define BMP581_OOR_THR_P_XMSB_REG_MSK               0x01

#define BMP581_OOR_COUNT_LIMIT_MSK                  0xC0
#define BMP581_OOR_COUNT_LIMIT_POS                  6

/* NVM configuration */
#define BMP581_NVM_ADDR_MSK                         0x3F

#define BMP581_NVM_PROG_EN_MSK                      0x40
#define BMP581_NVM_PROG_EN_POS                      6

#define BMP581_NVM_DATA_LSB_MSK                     0x00FF

#define BMP581_NVM_DATA_MSB_MSK                     0xFF00
