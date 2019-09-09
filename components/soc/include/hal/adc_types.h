#pragma once

typedef enum {
    ADC_CHANNEL_0 = 0, /*!< ADC channel */
    ADC_CHANNEL_1,     /*!< ADC channel */
    ADC_CHANNEL_2,     /*!< ADC channel */
    ADC_CHANNEL_3,     /*!< ADC channel */
    ADC_CHANNEL_4,     /*!< ADC channel */
    ADC_CHANNEL_5,     /*!< ADC channel */
    ADC_CHANNEL_6,     /*!< ADC channel */
    ADC_CHANNEL_7,     /*!< ADC channel */
    ADC_CHANNEL_8,     /*!< ADC channel */
    ADC_CHANNEL_9,     /*!< ADC channel */
    ADC_CHANNEL_MAX,
} adc_channel_t;

typedef enum {
    ADC_ATTEN_DB_0   = 0,  /*!<The input voltage of ADC will be reduced to about 1/1 */
    ADC_ATTEN_DB_2_5 = 1,  /*!<The input voltage of ADC will be reduced to about 1/1.34 */
    ADC_ATTEN_DB_6   = 2,  /*!<The input voltage of ADC will be reduced to about 1/2 */
    ADC_ATTEN_DB_11  = 3,  /*!<The input voltage of ADC will be reduced to about 1/3.6*/
    ADC_ATTEN_MAX,
} adc_atten_t;

typedef enum {
    ADC_I2S_DATA_SRC_IO_SIG = 0, /*!< I2S data from GPIO matrix signal  */
    ADC_I2S_DATA_SRC_ADC = 1,    /*!< I2S data from ADC */
    ADC_I2S_DATA_SRC_MAX,
} adc_i2s_source_t;

typedef enum {
    ADC_WIDTH_BIT_9  = 0, /*!< ADC capture width is 9Bit*/
    ADC_WIDTH_BIT_10 = 1, /*!< ADC capture width is 10Bit*/
    ADC_WIDTH_BIT_11 = 2, /*!< ADC capture width is 11Bit*/
    ADC_WIDTH_BIT_12 = 3, /*!< ADC capture width is 12Bit*/
    ADC_WIDTH_MAX,
} adc_bits_width_t;
