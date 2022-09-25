#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "bme280.h"

//Default I2C pins and clock speed
#define DEF_SDA_PIN   18
#define DEF_SCL_PIN   19
#define DEF_CLK_FREQ  100000 //100kHz

//I2C configuration parameters (port number and struct)
i2c_port_t i2c_port_num = I2C_NUM_0;
i2c_config_t i2c_config = {
  .mode = I2C_MODE_MASTER,
  .sda_io_num = DEF_SDA_PIN,
  .scl_io_num = DEF_SCL_PIN,
  .sda_pullup_en = false,
  .scl_pullup_en = false,
  .master.clk_speed = DEF_CLK_FREQ};

//BME280 register addresses (see BME280 datasheet; Memory map section)
enum reg_addr
{
  BME280_ADDR = 0x76,
	CALIB_00_25_ADDR = 0x88,
	CALIB_26_41_ADDR = 0xE1,
	CTRL_HUM_ADDR = 0xF2,
	CTRL_MEAS_ADDR = 0xF4,
	DATA_REG_ADDR = 0xF7  
};

//BME280 compensation parameters for sensor calibration
//Check BME280 datasheet (Table 16)
typedef struct 
{
  /* data */
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;  
}comp_param_t;

//BME280 register bit fields (check BME280 datasheet)
enum bit_field
{
  //Bit fields for humidity measurement
	CTRL_HUM_OSRS_H_OVR1 = 0x01,
	CTRL_HUM_OSRS_H_OVR2 = 0x02,
	CTRL_HUM_OSRS_H_OVR4 = 0x03,
	CTRL_HUM_OSRS_H_OVR8 = 0x04,
	CTRL_HUM_OSRS_H_OVR16 = 0x05,
	//Bit fields for sleep modes
	CTRL_MEAS_MODE_SLEEP = 0x00,
	CTRL_MEAS_MODE_FORCED1 = 0x01,
	CTRL_MEAS_MODE_FORCED2 = 0x02,
	CTRL_MEAS_MODE_NORMAL = 0x03,
	//Bit fields for temperature measurement
	CTRL_MEAS_OSRS_T_OVR1 = 1<<5,
	CTRL_MEAS_OSRS_T_OVR2 = 2<<5,
	CTRL_MEAS_OSRS_T_OVR4 = 3<<5,
	CTRL_MEAS_OSRS_T_OVR8 = 4<<5,
	CTRL_MEAS_OSRS_T_OVR16 = 5<<5
};
//Size of calibration registers
#define CALIB_00_25_SIZE          26 //calibration data register (26 bytes)
#define CALIB_26_41_SIZE          16 //calibration data register (16 bytes)   
//Struct containing 'compensation parameters'
static comp_param_t comp_param;
//Buffers to store data read from calibration registers
static uint8_t calib_00_25_reg[CALIB_00_25_SIZE];
static uint8_t calib_26_41_reg[CALIB_26_41_SIZE];

/**
 * @brief Send a byte to a specified register of the BME280
 * 
 * @param reg_addr pointer to address of BME280 sensor to be written to.
 * @param data pointer to data to be written.
 */
static void bme280_dev_reg_write_byte(uint8_t* reg_addr,uint8_t* data)
{
	uint8_t data_len = 1;
	//I2C master write config
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd,(BME280_ADDR << 1),I2C_MASTER_ACK);
	i2c_master_write(cmd,reg_addr,data_len,I2C_MASTER_ACK);
	i2c_master_write(cmd,data,data_len,I2C_MASTER_ACK);
	i2c_master_stop(cmd);
	ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_port_num,cmd,pdMS_TO_TICKS(100)));
	i2c_cmd_link_delete(cmd);
}

/**
 * @brief Reads bytes of data from the sensor's data registers.
 * 
 * @param reg_addr pointer to register address to start reading from.
 * Once reading starts, the sensor increments its register address automatically
 * in order to read data from subsequent registers.
 * @param data pointer to buffer to hold data read from the registers
 * @param data_len total number of bytes to read.
 */
static void bme280_dev_reg_read_bytes(uint8_t* reg_addr,uint8_t* data,uint32_t data_len)
{
	//I2C master read config
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd,(BME280_ADDR << 1),I2C_MASTER_ACK);
	i2c_master_write(cmd,reg_addr,1,I2C_MASTER_ACK);
	//repeated start
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd,(BME280_ADDR << 1) | 1,I2C_MASTER_ACK);
	if(data_len > 1)
	{
		i2c_master_read(cmd,data,data_len - 1,I2C_MASTER_ACK);
	}
	i2c_master_read_byte(cmd,data + data_len - 1,I2C_MASTER_NACK); //read last byte
	i2c_master_stop(cmd);
	ESP_ERROR_CHECK(i2c_master_cmd_begin(i2c_port_num,cmd,pdMS_TO_TICKS(100)));
	i2c_cmd_link_delete(cmd);
}

/**
 * @brief Stores 'compensation parameters' required to calibrate
 * the sensor.
 * 
 */
static void bme280_store_comp_params(void)
{
	comp_param.dig_T1 = (calib_00_25_reg[0] | (calib_00_25_reg[1] << 8));
	comp_param.dig_T2 = (calib_00_25_reg[2] | (calib_00_25_reg[3] << 8));
	comp_param.dig_T3 = (calib_00_25_reg[4] | (calib_00_25_reg[5] << 8));
	comp_param.dig_H1 = calib_00_25_reg[25];
	comp_param.dig_H2 = (calib_26_41_reg[0] | (calib_26_41_reg[1] << 8));
	comp_param.dig_H3 = calib_26_41_reg[2];
	comp_param.dig_H4 = ((calib_26_41_reg[3] << 4) | (calib_26_41_reg[4] & 0x0F));
	comp_param.dig_H5 = (((calib_26_41_reg[4] & ~(0x0F)) >> 4) | (calib_26_41_reg[5] << 4));
	comp_param.dig_H6 = calib_26_41_reg[6];
}

/**
 * @brief Configures the BME280 sensor.
 */
static void bme280_dev_config(void)
{
  i2c_param_config(i2c_port_num,&i2c_config);
  ESP_ERROR_CHECK(i2c_driver_install(i2c_port_num,i2c_config.mode,0,0,0));
	uint8_t calib_00_25_addr = CALIB_00_25_ADDR;
	uint8_t calib_26_41_addr = CALIB_26_41_ADDR;
	bme280_dev_reg_read_bytes(&calib_00_25_addr,calib_00_25_reg,CALIB_00_25_SIZE);
	//Read the first 7 bytes of the calib26_41 register
	bme280_dev_reg_read_bytes(&calib_26_41_addr,calib_26_41_reg,7);  
  bme280_store_comp_params();
}

/**
 * @brief Initialize BME280 sensor using custom settings 
 * (check the parameters below)
 * 
 * @param i2c_port I2C port for communicating with the sensor.
 * i2c_port could be 0 or 1.
 * @param sda_pin GPIO pin to be configured as SDA line
 * @param scl_pin GPIO pin to be configured as SCL line
 * @param enable_pullup If true, pull-ups are enabled for SDA and SCL
 * @param clk_freq Speed of I2C communication
 */
void bme280_custom_init(uint8_t i2c_port,
                        uint8_t sda_pin,uint8_t scl_pin,
                        bool enable_pullup,uint32_t clk_freq)
{
  //adjustable i2c configurations
  i2c_port_num = i2c_port;
  i2c_config.sda_io_num = sda_pin;
  i2c_config.scl_io_num = scl_pin;
  i2c_config.sda_pullup_en = enable_pullup;
  i2c_config.scl_pullup_en = enable_pullup;
  i2c_config.master.clk_speed = clk_freq;  
  bme280_dev_config();
}

/**
 * @brief Initialize BME280 sensor using default settings.
 * Default settings:
 * 1. SDA pin = 18
 * 2. SCL pin = 19
 * 3. CLOCK SPEED = 100kHz
 * 4. No pull-ups on the SDA and SCL lines
 * 5. I2C port 0
 */
void bme280_init(void)
{
  bme280_dev_config();
}

/**
 * @brief Get data from BME280 sensor
 * The calculations in this function were obtained from the sensor's
 * datasheet.
 * @param sensor_data pointer to struct that will receive
 * the processed values of the measured humidity and temperature.
 */
void bme280_get_data(bme280_data_t* sensor_data)
{
	//config humidity register
	uint8_t reg_addr = CTRL_HUM_ADDR;
	uint8_t data = CTRL_HUM_OSRS_H_OVR16;
	bme280_dev_reg_write_byte(&reg_addr,&data);
	//config pressure, temperature, and enable 
	//'forced mode' (see tables 22-25 of datasheet)
	reg_addr = CTRL_MEAS_ADDR;
	data = CTRL_MEAS_OSRS_T_OVR16|CTRL_MEAS_MODE_FORCED1;
	bme280_dev_reg_write_byte(&reg_addr,&data);
	//Read raw data from sensor's data registers ['press_msb' to 'hum_lsb'].
	//see BME280 datasheet; Memory map section[Table 18]
	const uint8_t num_of_data_reg_to_read = 8;
	uint8_t raw_adc_value[num_of_data_reg_to_read];
	reg_addr = DATA_REG_ADDR;
	bme280_dev_reg_read_bytes(&reg_addr,raw_adc_value,num_of_data_reg_to_read);
	//Temperature calculation
	int32_t t_fine = 0;
	int32_t adc_T, var1, var2, T;
	adc_T = ((raw_adc_value[3] << 12) | (raw_adc_value[4] << 4) | (raw_adc_value[5] >> 4));
	var1 = ((((adc_T>>3) - ((int32_t)comp_param.dig_T1<<1))) * ((int32_t)comp_param.dig_T2)) >> 11;
	
	var2 = (((((adc_T>>4) - ((int32_t)comp_param.dig_T1)) * 
						((adc_T>>4) - ((int32_t)comp_param.dig_T1))) >> 12) * ((int32_t)comp_param.dig_T3)) >> 14;

	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	
	sensor_data->temperature = T / 100;
	//Humidity calculation
	int32_t adc_H, v_x1_u32r;
	adc_H = ((raw_adc_value[6] << 8) | raw_adc_value[7]);

	v_x1_u32r = (t_fine - ((uint32_t)76800));
	
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)comp_param.dig_H4) << 20) -
              (((int32_t)comp_param.dig_H5) * v_x1_u32r)) +
              ((int32_t)16384)) >> 15) *
              (((((((v_x1_u32r * ((int32_t)comp_param.dig_H6)) >> 10) *
              (((v_x1_u32r * ((int32_t)comp_param.dig_H3)) >> 11) +
              ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
              ((int32_t)comp_param.dig_H2) + 8192) >> 14));
	
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * 
              ((int32_t)comp_param.dig_H1)) >> 4));

	//change the ternary operators to if..else statements if you like
  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
	uint32_t rawHumidity = (uint32_t)(v_x1_u32r >> 12);
  
	sensor_data->humidity =  rawHumidity / 1024;	
}