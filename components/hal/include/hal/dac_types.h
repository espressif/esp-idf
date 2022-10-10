#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DAC_CHAN_0 = 0,     /*!< DAC channel 0 is GPIO25(ESP32) / GPIO17(ESP32S2) */
    DAC_CHAN_1 = 1,     /*!< DAC channel 1 is GPIO26(ESP32) / GPIO18(ESP32S2) */
    DAC_CHANNEL_1 __attribute__((deprecated("please use 'DAC_CHAN_0' instead"))) = 0,    /*!< Alias of 'DAC_CHAN_0', now the channel index start from '0' */
    DAC_CHANNEL_2 __attribute__((deprecated("please use 'DAC_CHAN_1' instead"))) = 1,    /*!< Alias of 'DAC_CHAN_1', now the channel index start from '0' */
} dac_channel_t;

/**
 * @brief The attenuation of the amplitude of the cosine wave generator. The max amplitude is VDD3P3_RTC.
 */
typedef enum {
    DAC_COSINE_ATTEN_DEFAULT    = 0x0,      /*!< No attenuation to the DAC cosine wave amplitude. Default. */
    DAC_COSINE_ATTEN_DB_0       = 0x0,      /*!< Original amplitude of the DAC cosine wave, equals to DAC_COSINE_ATTEN_DEFAULT */
    DAC_COSINE_ATTEN_DB_6       = 0x1,      /*!< 1/2 amplitude of the DAC cosine wave */
    DAC_COSINE_ATTEN_DB_12      = 0x2,      /*!< 1/4 amplitude of the DAC cosine wave */
    DAC_COSINE_ATTEN_DB_18      = 0x3,      /*!< 1/8 amplitude of the DAC cosine wave */
} dac_cosine_atten_t;

/**
 * @brief Set the phase of the cosine wave generator output.
 * @note  Only 0 or 180 are supported,
 *        it will be set to 0 as default if configured to an unsupported phase.
 */
typedef enum {
    DAC_COSINE_PHASE_0   = 0x02,   /*!< Phase shift +0° */
    DAC_COSINE_PHASE_180 = 0x03, /*!< Phase shift +180° */
} dac_cosine_phase_t;

#ifdef __cplusplus
}
#endif
