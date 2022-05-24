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

#ifdef __cplusplus
}
#endif
