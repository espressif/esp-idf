#ifndef _TEMP_SENSOR_H_
#define _TEMP_SENSOR_H_

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32S2BETA
typedef enum {
    TEMP_SENSOR_DAC_L0 = 5,     // offset = -2, range: 50℃ ~ 125℃, error < 3℃.
    TEMP_SENSOR_DAC_L1 = 7,     // offset = -1, range: 20℃ ~ 100℃, error < 2℃.
    TEMP_SENSOR_DAC_L2 = 15,    // offset =  0, range:-10℃ ~  80℃, error < 1℃.
    TEMP_SENSOR_DAC_L3 = 11,    // offset =  1, range:-30℃ ~  50℃, error < 2℃.
    TEMP_SENSOR_DAC_L4 = 10,    // offset =  2, range:-40℃ ~  20℃, error < 3℃.
    TEMP_SENSOR_DAC_DEFAULT = TEMP_SENSOR_DAC_L2,
} temp_sensor_dac_offset_t;;

typedef struct temp_sensor {
    temp_sensor_dac_offset_t dac_offset;
    uint8_t clk_div;
} temp_sensor_t;

esp_err_t temp_sensor_set_config(temp_sensor_t temps);
esp_err_t temp_sensor_get_config(temp_sensor_t *temps);
esp_err_t temp_sensor_start(void);
esp_err_t temp_sensor_stop(void);
esp_err_t temp_sensor_read(uint8_t *temp_out);
#endif

#ifdef __cplusplus
}
#endif
#endif