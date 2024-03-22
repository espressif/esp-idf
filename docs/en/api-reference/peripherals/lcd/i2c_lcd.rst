I2C Interfaced LCD
------------------

#. Create I2C bus. Please refer to :doc:`I2C API doc </api-reference/peripherals/i2c>` for more details.

    .. code-block:: c

        i2c_master_bus_handle_t i2c_bus = NULL;
        i2c_master_bus_config_t bus_config = {
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .i2c_port = I2C_BUS_PORT,
            .sda_io_num = EXAMPLE_PIN_NUM_SDA,
            .scl_io_num = EXAMPLE_PIN_NUM_SCL,
            .flags.enable_internal_pullup = true,
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

#. Allocate an LCD IO device handle from the I2C bus. In this step, you need to provide the following information:

    - :cpp:member:`esp_lcd_panel_io_i2c_config_t::dev_addr` sets the I2C device address of the LCD controller chip. The LCD driver uses this address to communicate with the LCD controller chip.
    - :cpp:member:`esp_lcd_panel_io_i2c_config_t::scl_speed_hz` sets the I2C clock frequency in Hz. The value should not exceed the range recommended in the LCD spec.
    - :cpp:member:`esp_lcd_panel_io_i2c_config_t::lcd_cmd_bits` and :cpp:member:`esp_lcd_panel_io_i2c_config_t::lcd_param_bits` set the bit width of the command and parameter that recognized by the LCD controller chip. This is chip specific, you should refer to your LCD spec in advance.

    .. code-block:: c

        esp_lcd_panel_io_handle_t io_handle = NULL;
        esp_lcd_panel_io_i2c_config_t io_config = {
            .dev_addr = EXAMPLE_I2C_HW_ADDR,
            .scl_speed_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .control_phase_bytes = 1, // refer to LCD spec
            .dc_bit_offset = 6,       // refer to LCD spec
            .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
            .lcd_param_bits = EXAMPLE_LCD_CMD_BITS,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

#. Install the LCD controller driver. The LCD controller driver is responsible for sending the commands and parameters to the LCD controller chip. In this step, you need to specify the I2C IO device handle that allocated in the last step, and some panel specific configurations:

    - :cpp:member:`esp_lcd_panel_dev_config_t::reset_gpio_num` sets the LCD's hardware reset GPIO number. If the LCD does not have a hardware reset pin, set this to ``-1``.
    - :cpp:member:`esp_lcd_panel_dev_config_t::bits_per_pixel` sets the bit width of the pixel color data. The LCD driver uses this value to calculate the number of bytes to send to the LCD controller chip.

    .. code-block:: c

        esp_lcd_panel_handle_t panel_handle = NULL;
        esp_lcd_panel_dev_config_t panel_config = {
            .bits_per_pixel = 1,
            .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
