SPI Interfaced LCD
------------------

#. Create an SPI bus. Please refer to :doc:`SPI Master API doc </api-reference/peripherals/spi_master>` for more details.

    .. code-block:: c

        spi_bus_config_t buscfg = {
            .sclk_io_num = EXAMPLE_PIN_NUM_SCLK,
            .mosi_io_num = EXAMPLE_PIN_NUM_MOSI,
            .miso_io_num = EXAMPLE_PIN_NUM_MISO,
            .quadwp_io_num = -1, // Quad SPI LCD driver is not yet supported
            .quadhd_io_num = -1, // Quad SPI LCD driver is not yet supported
            .max_transfer_sz = EXAMPLE_LCD_H_RES * 80 * sizeof(uint16_t), // transfer 80 lines of pixels (assume pixel is RGB565) at most in one SPI transaction
        };
        ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO)); // Enable the DMA feature

#. Allocate an LCD IO device handle from the SPI bus. In this step, you need to provide the following information:

    - :cpp:member:`esp_lcd_panel_io_spi_config_t::dc_gpio_num`: Sets the gpio number for the DC signal line (some LCD calls this ``RS`` line). The LCD driver uses this GPIO to switch between sending command and sending data.
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::cs_gpio_num`: Sets the gpio number for the CS signal line. The LCD driver uses this GPIO to select the LCD chip. If the SPI bus only has one device attached (i.e., this LCD), you can set the gpio number to ``-1`` to occupy the bus exclusively.
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::pclk_hz` sets the frequency of the pixel clock, in Hz. The value should not exceed the range recommended in the LCD spec.
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::spi_mode` sets the SPI mode. The LCD driver uses this mode to communicate with the LCD. For the meaning of the SPI mode, please refer to the :doc:`SPI Master API doc </api-reference/peripherals/spi_master>`.
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::lcd_cmd_bits` and :cpp:member:`esp_lcd_panel_io_spi_config_t::lcd_param_bits` set the bit width of the command and parameter that recognized by the LCD controller chip. This is chip specific, you should refer to your LCD spec in advance.
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::trans_queue_depth` sets the depth of the SPI transaction queue. A bigger value means more transactions can be queued up, but it also consumes more memory.

    .. code-block:: c

        esp_lcd_panel_io_handle_t io_handle = NULL;
        esp_lcd_panel_io_spi_config_t io_config = {
            .dc_gpio_num = EXAMPLE_PIN_NUM_LCD_DC,
            .cs_gpio_num = EXAMPLE_PIN_NUM_LCD_CS,
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
            .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
            .spi_mode = 0,
            .trans_queue_depth = 10,
        };
        // Attach the LCD to the SPI bus
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

#. Install the LCD controller driver. The LCD controller driver is responsible for sending the commands and parameters to the LCD controller chip. In this step, you need to specify the SPI IO device handle that allocated in the last step, and some panel specific configurations:

    - :cpp:member:`esp_lcd_panel_dev_config_t::reset_gpio_num` sets the LCD's hardware reset GPIO number. If the LCD does not have a hardware reset pin, set this to ``-1``.
    - :cpp:member:`esp_lcd_panel_dev_config_t::rgb_ele_order` sets the R-G-B element order of each color data.
    - :cpp:member:`esp_lcd_panel_dev_config_t::bits_per_pixel` sets the bit width of the pixel color data. The LCD driver uses this value to calculate the number of bytes to send to the LCD controller chip.
    - :cpp:member:`esp_lcd_panel_dev_config_t::data_endian` specifies the data endian to be transmitted to the screen. No need to specify for color data within 1 byte, like RGB232. For drivers that do not support specifying data endian, this field would be ignored.

    .. code-block:: c

        esp_lcd_panel_handle_t panel_handle = NULL;
        esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
            .bits_per_pixel = 16,
        };
        // Create LCD panel handle for ST7789, with the SPI IO device handle
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
