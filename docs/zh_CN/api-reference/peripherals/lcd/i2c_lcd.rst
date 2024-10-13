I2C 接口的 LCD
---------------

:link_to_translation:`en:[English]`

#. 创建 I2C 总线。详细信息，请参阅 :doc:`I2C API 文档 </api-reference/peripherals/i2c>`。

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

#. 从 I2C 总线分配一个 LCD IO 设备句柄。在此步骤中，需要提供以下信息：

    - :cpp:member:`esp_lcd_panel_io_i2c_config_t::dev_addr` 设置 LCD 控制器芯片的 I2C 设备地址。LCD 驱动程序使用此地址与 LCD 控制器芯片通信。
    - :cpp:member:`esp_lcd_panel_io_i2c_config_t::scl_speed_hz` 设置 I2C 时钟频率 (Hz)。该值不应超过 LCD 规格书中推荐的范围。
    - :cpp:member:`esp_lcd_panel_io_i2c_config_t::lcd_cmd_bits` 和 :cpp:member:`esp_lcd_panel_io_i2c_config_t::lcd_param_bits` 分别设置 LCD 控制器芯片可识别的命令及参数的位宽。不同芯片对位宽要求不同，请提前参阅 LCD 规格书。

    .. code-block:: c

        esp_lcd_panel_io_handle_t io_handle = NULL;
        esp_lcd_panel_io_i2c_config_t io_config = {
            .dev_addr = EXAMPLE_I2C_HW_ADDR,
            .scl_speed_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .control_phase_bytes = 1, // 参阅 LCD 规格
            .dc_bit_offset = 6,       // 参阅 LCD 规格
            .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
            .lcd_param_bits = EXAMPLE_LCD_CMD_BITS,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

#. 安装 LCD 控制器驱动程序。LCD 控制器驱动程序负责向 LCD 控制器芯片发送命令和参数。在此步骤中，需要指定上一步骤中分配到的 I2C IO 设备句柄以及一些面板特定配置：

    - :cpp:member:`esp_lcd_panel_dev_config_t::reset_gpio_num` 设置 LCD 的硬件复位 GPIO 编号。如果 LCD 没有硬件复位管脚，则将此设置为 ``-1``。
    - :cpp:member:`esp_lcd_panel_dev_config_t::bits_per_pixel` 设置像素颜色数据的位宽。LCD 驱动程序使用此值计算要发送到 LCD 控制器芯片的字节数。

    .. code-block:: c

        esp_lcd_panel_handle_t panel_handle = NULL;
        esp_lcd_panel_dev_config_t panel_config = {
            .bits_per_pixel = 1,
            .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

API 参考
--------

.. include-build-file:: inc/esp_lcd_io_i2c.inc
