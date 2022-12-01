/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*---------------------------------------------------------------
                    Single Read
---------------------------------------------------------------*/
#define ADC_HAL_DATA_INVERT_DEFAULT(PERIPH_NUM)         (0)
#define ADC_HAL_SAR_CLK_DIV_DEFAULT(PERIPH_NUM)         ((PERIPH_NUM==0)? 2 : 1)

/*---------------------------------------------------------------
                    DMA Read
---------------------------------------------------------------*/
#define ADC_HAL_DIGI_DATA_INVERT_DEFAULT(PERIPH_NUM)    (0)
#define ADC_HAL_FSM_RSTB_WAIT_DEFAULT                   (8)
#define ADC_HAL_FSM_START_WAIT_DEFAULT                  (5)
#define ADC_HAL_FSM_STANDBY_WAIT_DEFAULT                (100)
#define ADC_HAL_SAMPLE_CYCLE_DEFAULT                    (2)
#define ADC_HAL_DIGI_SAR_CLK_DIV_DEFAULT                (1)

/*---------------------------------------------------------------
                    PWDET (Power Detect)
---------------------------------------------------------------*/
#define ADC_HAL_PWDET_CCT_DEFAULT                       (4)
