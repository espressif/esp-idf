#ifndef BME280_H
#define BME280_H

//Sensor data readout struct
typedef struct bme280
{
  /* data */
  uint8_t temperature;
  uint8_t humidity;
}bme280_data_t;

void bme280_custom_init(uint8_t i2c_port,
                        uint8_t sda_pin,uint8_t scl_pin,
                        bool enable_pullup,uint32_t clk_freq);
void bme280_init(void);
void bme280_get_data(bme280_data_t* sensor_data);

#endif