#include "TCA9535.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

// ****************************************************************************
//! @brief		Initializes the I2C interface
//! @param		None
//! @return
//! 			- ESP_OK if erase operation was successful
//!   			- i2c driver error
// ****************************************************************************
esp_err_t TCA9535Init(void) 
{
	esp_err_t ret;

	int i2c_master_port = I2C_MASTER_NUM;
	i2c_config_t conf;

	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = I2C_MASTER_SDA_IO;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = I2C_MASTER_SCL_IO;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
	i2c_param_config(i2c_master_port, &conf);

	ret = i2c_driver_install(i2c_master_port, conf.mode,
					   I2C_MASTER_RX_BUF_DISABLE,
					   I2C_MASTER_TX_BUF_DISABLE, 0);

	return ret;
}

// ****************************************************************************
//! @brief		Reads single byte from specified register
//! @param		Register address
//! @return		Byte from register
// ****************************************************************************
 /* _________________________________________________________________________________________________________
 * | start | slave_addr + wr_bit +ack | reg_addr + ack | start | slave_addr + rd_bit +nack | reg_data |stop |
 * --------|--------------------------|----------------|-------|---------------------------|----------|-----|
 */
unsigned char TCA9535ReadSingleRegister(tca9535_reg_t address)
{
	uint8_t reg_data = 0;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ( TCA9535_ADDRESS << 1 ) | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ( TCA9535_ADDRESS << 1 ) | READ_BIT, ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &reg_data, NACK_VAL);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 100 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	return reg_data;
}

// ****************************************************************************
//! @brief		Writes single byte to specified register
//! @param		Register address
//! @return		
//! 			- ESP_OK if erase operation was successful
//!   			- i2c driver error
// ****************************************************************************
 /* ____________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | reg_addr + ack | write reg_data + ack  | stop |
 * --------|---------------------------|----------------|-----------------------|------|
 */
esp_err_t TCA9535WriteSingleRegister(tca9535_reg_t address, unsigned short regVal)
{
	esp_err_t ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ( TCA9535_ADDRESS << 1 ) | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, regVal, ACK_CHECK_EN);

	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 100 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	return ret;
}

// ****************************************************************************
//! @brief		Reads whole register and puts result into struct
//! @param		reg: struct pointer
//!				reg_num: register type
//! @return		
//! 			- ESP_OK if erase operation was successful
//!   			- i2c driver error
// ****************************************************************************
esp_err_t TCA9535ReadStruct(TCA9535_Register *reg, tca9535_reg_t reg_num)
{
	esp_err_t ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ( TCA9535_ADDRESS << 1 ) | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, reg_num, ACK_CHECK_EN);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ( TCA9535_ADDRESS << 1 ) | READ_BIT, ACK_CHECK_EN);
	i2c_master_read(cmd, (uint8_t*) &reg->asInt, 2, NACK_VAL);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 100 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	return ret;
}

// ****************************************************************************
//! @brief		Writes whole register with data from struct
//! @param		reg: struct pointer
//!				reg_num: register type
//! @return		
//! 			- ESP_OK if erase operation was successful
//!   			- i2c driver error
// ****************************************************************************
esp_err_t TCA9535WriteStruct(TCA9535_Register *reg, tca9535_reg_t reg_num)
{
	esp_err_t ret;
	uint8_t reg_data[2];

	memcpy(reg_data, reg, sizeof(reg_data));
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ( TCA9535_ADDRESS << 1 ) | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, reg_num, ACK_CHECK_EN);
	i2c_master_write(cmd, reg_data, 2, ACK_VAL);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 100 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	return ret;
}

esp_err_t TCA9535WriteOutput(TCA9535_Register *reg)
{
	return TCA9535WriteStruct(reg, TCA9535_OUTPUT_REG0);
}

esp_err_t TCA9535WritePolarity(TCA9535_Register *reg)
{
	return TCA9535WriteStruct(reg, TCA9535_POLARITY_REG0);
}

esp_err_t TCA9535WriteConfig(TCA9535_Register *reg)
{
	return TCA9535WriteStruct(reg, TCA9535_CONFIG_REG0);
}

esp_err_t TCA9535ReadInput(TCA9535_Register *reg)
{
	return TCA9535ReadStruct(reg, TCA9535_INPUT_REG0);
}

esp_err_t TCA9535ReadOutput(TCA9535_Register *reg)
{
	return TCA9535ReadStruct(reg, TCA9535_OUTPUT_REG0);
}

esp_err_t TCA9535ReadPolarity(TCA9535_Register *reg)
{
	return TCA9535ReadStruct(reg, TCA9535_POLARITY_REG0);
}

esp_err_t TCA9535ReadConfig(TCA9535_Register *reg)
{
	return TCA9535ReadStruct(reg, TCA9535_CONFIG_REG0);
}