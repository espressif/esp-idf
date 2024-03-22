I80 Interfaced LCD
------------------

#. Create I80 bus by :cpp:func:`esp_lcd_new_i80_bus`. You need to set up the following parameters for an Intel 8080 parallel bus:

    - :cpp:member:`esp_lcd_i80_bus_config_t::clk_src` sets the clock source of the I80 bus. Note, the default clock source may be different between ESP targets.
    - :cpp:member:`esp_lcd_i80_bus_config_t::wr_gpio_num` sets the GPIO number of the pixel clock (also referred as ``WR`` in some LCD spec)
    - :cpp:member:`esp_lcd_i80_bus_config_t::dc_gpio_num` sets the GPIO number of the data/command select pin (also referred as ``RS`` in some LCD spec)
    - :cpp:member:`esp_lcd_i80_bus_config_t::bus_width` sets the bit width of the data bus (only support ``8`` or ``16``)
    - :cpp:member:`esp_lcd_i80_bus_config_t::data_gpio_nums` is the array of the GPIO number of the data bus. The number of GPIOs should be equal to the :cpp:member:`esp_lcd_i80_bus_config_t::bus_width` value.
    - :cpp:member:`esp_lcd_i80_bus_config_t::max_transfer_bytes` sets the maximum number of bytes that can be transferred in one transaction.

    .. code-block:: c

        esp_lcd_i80_bus_handle_t i80_bus = NULL;
        esp_lcd_i80_bus_config_t bus_config = {
            .clk_src = LCD_CLK_SRC_DEFAULT,
            .dc_gpio_num = EXAMPLE_PIN_NUM_DC,
            .wr_gpio_num = EXAMPLE_PIN_NUM_PCLK,
            .data_gpio_nums = {
                EXAMPLE_PIN_NUM_DATA0,
                EXAMPLE_PIN_NUM_DATA1,
                EXAMPLE_PIN_NUM_DATA2,
                EXAMPLE_PIN_NUM_DATA3,
                EXAMPLE_PIN_NUM_DATA4,
                EXAMPLE_PIN_NUM_DATA5,
                EXAMPLE_PIN_NUM_DATA6,
                EXAMPLE_PIN_NUM_DATA7,
            },
            .bus_width = 8,
            .max_transfer_bytes = EXAMPLE_LCD_H_RES * 100 * sizeof(uint16_t), // transfer 100 lines of pixels (assume pixel is RGB565) at most in one transaction
            .psram_trans_align = EXAMPLE_PSRAM_DATA_ALIGNMENT,
            .sram_trans_align = 4,
        };
        ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

#. Allocate an LCD IO device handle from the I80 bus. In this step, you need to provide the following information:

    - :cpp:member:`esp_lcd_panel_io_i80_config_t::cs_gpio_num` sets the GPIO number of the chip select pin.
    - :cpp:member:`esp_lcd_panel_io_i80_config_t::pclk_hz` sets the pixel clock frequency in Hz. Higher pixel clock frequency results in higher refresh rate, but may cause flickering if the DMA bandwidth is not sufficient or the LCD controller chip does not support high pixel clock frequency.
    - :cpp:member:`esp_lcd_panel_io_i80_config_t::lcd_cmd_bits` and :cpp:member:`esp_lcd_panel_io_i80_config_t::lcd_param_bits` set the bit width of the command and parameter that recognized by the LCD controller chip. This is chip specific, you should refer to your LCD spec in advance.
    - :cpp:member:`esp_lcd_panel_io_i80_config_t::trans_queue_depth` sets the maximum number of transactions that can be queued in the LCD IO device. A bigger value means more transactions can be queued up, but it also consumes more memory.

    .. code-block:: c

        esp_lcd_panel_io_handle_t io_handle = NULL;
        esp_lcd_panel_io_i80_config_t io_config = {
            .cs_gpio_num = EXAMPLE_PIN_NUM_CS,
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .trans_queue_depth = 10,
            .dc_levels = {
                .dc_idle_level = 0,
                .dc_cmd_level = 0,
                .dc_dummy_level = 0,
                .dc_data_level = 1,
            },
            .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
            .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

#. Install the LCD controller driver. The LCD controller driver is responsible for sending the commands and parameters to the LCD controller chip. In this step, you need to specify the I80 IO device handle that allocated in the last step, and some panel specific configurations:

    - :cpp:member:`esp_lcd_panel_dev_config_t::bits_per_pixel` sets the bit width of the pixel color data. The LCD driver uses this value to calculate the number of bytes to send to the LCD controller chip.
    - :cpp:member:`esp_lcd_panel_dev_config_t::reset_gpio_num` sets the GPIO number of the reset pin. If the LCD controller chip does not have a reset pin, you can set this value to ``-1``.
    - :cpp:member:`esp_lcd_panel_dev_config_t::rgb_ele_order` sets the color order the pixel color data.

    .. code-block:: c

        esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
            .bits_per_pixel = 16,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
