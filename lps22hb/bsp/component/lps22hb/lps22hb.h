#ifndef LPS22HB_H
#define LPS22HB_H

#include <stdint.h>
#include <esp_err.h>

// LPS22HB I2C address
#define LPS22HB_ADDRESS 0x5C

// LPS22HB registers
#define WHO_AM_I_REG    0x0F
#define CTRL_REG1       0x10
#define PRESS_OUT_XL    0x28
#define TEMP_OUT_L      0x2B

#define EXPECTED_WHO_AM_I 0xB1  // Expected device ID for LPS22HB

// Function prototypes
esp_err_t lps22hb_init();
esp_err_t lps22hb_who_am_i(uint8_t device_addr);
esp_err_t lps22hb_read_pressure(float *pressure_hPa);
esp_err_t lps22hb_read_temperature(float *temperature_C);

#endif // LPS22HB_H