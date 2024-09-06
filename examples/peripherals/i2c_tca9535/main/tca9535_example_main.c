#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "TCA9535.h"

void app_main()
{
	// Configure TCA9535's INT output as input with pullup
	gpio_pad_select_gpio(TCA9535_INT_GPIO);
	gpio_set_direction(TCA9535_INT_GPIO, GPIO_MODE_INPUT);
	gpio_set_pull_mode(TCA9535_INT_GPIO, GPIO_PULLUP_ONLY);

	// Init i2c driver
	esp_err_t ret = TCA9535Init();

	if (ret == ESP_OK)
		ESP_LOGI(I2C_LOG_TAG, "Initialization done");
	else
		ESP_LOGI(I2C_LOG_TAG, "Initialization failed");

	// Configure TCA9535's PORT0 as input port
	TCA9535WriteSingleRegister(TCA9535_CONFIG_REG0, 0xFF);

	while (1)
	{
		// Example of reading single register data
		uint8_t reg_data = TCA9535ReadSingleRegister(TCA9535_INPUT_REG0);
		printf("INPUT0_REG_DATA = %d\n", reg_data);
		reg_data = TCA9535ReadSingleRegister(TCA9535_INPUT_REG1);
		printf("INPUT1_REG_DATA = %d\n", reg_data);

		reg_data = TCA9535ReadSingleRegister(TCA9535_OUTPUT_REG0);
		printf("OUTPUT0_REG_DATA = %d\n", reg_data);
		reg_data = TCA9535ReadSingleRegister(TCA9535_OUTPUT_REG1);
		printf("OUTPUT1_REG_DATA = %d\n", reg_data);

		reg_data = TCA9535ReadSingleRegister(TCA9535_POLARITY_REG0);
		printf("POL0_REG_DATA = %d\n", reg_data);
		reg_data = TCA9535ReadSingleRegister(TCA9535_POLARITY_REG1);
		printf("POL1_REG_DATA = %d\n", reg_data);

		reg_data = TCA9535ReadSingleRegister(TCA9535_CONFIG_REG0);
		printf("CONF0_REG_DATA = %d\n", reg_data);
		reg_data = TCA9535ReadSingleRegister(TCA9535_CONFIG_REG1);
		printf("CONF1_REG_DATA = %d\n", reg_data);

		// Example of reading data to struct
		TCA9535_Register inputRegister;

		TCA9535ReadInput(&inputRegister);
		printf("Input0_reg_ing = %d\n", inputRegister.Port.P0.asInt);
		printf("Input0_reg_ing = %d\n", inputRegister.Port.P1.asInt);
		printf("Input_P0.2 = %d\n", inputRegister.Port.P0.bit.B2);

		// Example of setting register data with struct
		TCA9535_Register configRegister;
		
		// Configure TCA9535's PORT0 as input port
		configRegister.Port.P0.asInt = 255;
		// Configure TCA9535's PORT1 bit 3 as input port
		configRegister.Port.P1.bit.B3 = 1;

		// Write data to TCA9535
		TCA9535WriteConfig(&configRegister);

		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}

