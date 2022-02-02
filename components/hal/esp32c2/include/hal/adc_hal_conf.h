/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define SOC_ADC1_DATA_INVERT_DEFAULT    (0)
#define SOC_ADC2_DATA_INVERT_DEFAULT    (0)

#define SOC_ADC_DIGI_DATA_INVERT_DEFAULT(PERIPH_NUM) (0)

#define SOC_ADC_FSM_RSTB_WAIT_DEFAULT       (8)
#define SOC_ADC_FSM_START_WAIT_DEFAULT      (5)
#define SOC_ADC_FSM_STANDBY_WAIT_DEFAULT    (100)
#define ADC_FSM_SAMPLE_CYCLE_DEFAULT        (2)

#define SOC_ADC_PWDET_CCT_DEFAULT           (4)

#define SOC_ADC_SAR_CLK_DIV_DEFAULT(PERIPH_NUM) ((PERIPH_NUM==0)? 2 : 1)

#define SOC_ADC_DIGI_SAR_CLK_DIV_DEFAULT     (1)
