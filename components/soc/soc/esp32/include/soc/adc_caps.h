#pragma once 

#define SOC_ADC_PERIPH_NUM              (2)
#define SOC_ADC_PATT_LEN_MAX            (16)

#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM) ((PERIPH_NUM==0)? 8: 10)
#define SOC_ADC_MAX_CHANNEL_NUM         (10)

#define SOC_ADC1_DATA_INVERT_DEFAULT    (1)
#define SOC_ADC2_DATA_INVERT_DEFAULT    (1)

#define SOC_ADC_DIGI_DATA_INVERT_DEFAULT(PERIPH_NUM) (1)

#define SOC_ADC_FSM_RSTB_WAIT_DEFAULT       (8)
#define SOC_ADC_FSM_START_WAIT_DEFAULT      (SOC_ADC_DIGI_SAR_CLK_DIV_DEFAULT * 2)
#define SOC_ADC_FSM_STANDBY_WAIT_DEFAULT    (100)
#define ADC_FSM_SAMPLE_CYCLE_DEFAULT        (2)

/**
 * Check if adc support digital controller (DMA) mode.
 * @value
 *      - 1 : support;
 *      - 0 : not support;
 */
#define SOC_ADC_SUPPORT_DMA_MODE(PERIPH_NUM) ((PERIPH_NUM==0)? 1: 0)

#define SOC_ADC_PWDET_CCT_DEFAULT           (4)

#define SOC_ADC_SAR_CLK_DIV_DEFAULT(PERIPH_NUM) (2)

#define SOC_ADC_DIGI_SAR_CLK_DIV_DEFAULT    (16)
