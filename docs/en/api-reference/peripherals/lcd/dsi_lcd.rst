MIPI DSI Interfaced LCD
=======================

:link_to_translation:`zh_CN:[中文]`

#. Create a DSI bus, and it will initialize the D-PHY as well.

    .. code-block:: c

        esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
        esp_lcd_dsi_bus_config_t bus_config = {
            .bus_id = 0, // index from 0, specify the DSI host to use
            .num_data_lanes = 2, // Number of data lanes to use, can't set a value that exceeds the chip's capability
            .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT, // Clock source for the DPHY
            .lane_bit_rate_mbps = EXAMPLE_MIPI_DSI_LANE_BITRATE_MBPS, // Bit rate of the data lanes, in Mbps
        };
        ESP_ERROR_CHECK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

#. Derive the DBI interface from the DSI bus. The DBI interface mostly is used as the control IO layer in the esp_lcd component. This interface provides the functions to read or write the configuration registers inside the LCD device. In this step, you need to provide the following information:

    - :cpp:member:`esp_lcd_dbi_io_config_t::virtual_channel` sets the virtual channel number to use. The virtual channel is a logical channel that is used to multiplex the data from different sources. If you only have one LCD connected, you can set this to ``0``.
    - :cpp:member:`esp_lcd_dbi_io_config_t::lcd_cmd_bits` and :cpp:member:`esp_lcd_dbi_io_config_t::lcd_param_bits` set the bit width of the command and parameter that recognized by the LCD controller chip. This is chip specific, you should refer to your LCD spec in advance.

    .. code-block:: c

        esp_lcd_panel_io_handle_t mipi_dbi_io = NULL;
        esp_lcd_dbi_io_config_t dbi_config = {
            .virtual_channel = 0,
            .lcd_cmd_bits = 8,   // according to the LCD spec
            .lcd_param_bits = 8, // according to the LCD spec
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

#. Install the LCD controller driver. The LCD controller driver is responsible for sending the commands and parameters to the LCD controller chip. In this step, you need to specify the MIPI DBI IO handle that allocated in the last step, and some panel specific configurations:

    - :cpp:member:`esp_lcd_panel_dev_config_t::bits_per_pixel` sets the bit width of each pixel. The LCD driver uses this value to calculate the number of bytes to send to the LCD controller chip.
    - :cpp:member:`esp_lcd_panel_dev_config_t::reset_gpio_num` sets the GPIO number of the reset pin. If the LCD controller chip does not have a reset pin, you can set this value to ``-1``.
    - :cpp:member:`esp_lcd_panel_dev_config_t::rgb_ele_order` sets the RGB element order of the pixel data, it can be **RGB** or **BGR**.

    .. code-block:: c

        esp_lcd_panel_handle_t ili9881c_ctrl_panel = NULL;
        esp_lcd_panel_dev_config_t lcd_dev_config = {
            .bits_per_pixel = 24, // MIPI LCD usually uses 24 bit (i.e., RGB888) per pixel
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
            .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_ili9881c(mipi_dbi_io, &lcd_dev_config, &ili9881c_ctrl_panel));

#. With the LCD control panel that returned in the last step, you can reset the LCD device followed by a basic initialization. After that, you can turn on the display.

    .. code-block:: c

        ESP_ERROR_CHECK(esp_lcd_panel_reset(ili9881c_ctrl_panel));
        ESP_ERROR_CHECK(esp_lcd_panel_init(ili9881c_ctrl_panel));
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(ili9881c_ctrl_panel, true));

#. However, you still can't send pixel data to the MIPI LCD with the control panel, because MIPI LCD has a high resolution and there's no GRAM in the LCD controller. We need to maintain the LCD frame buffer and flush it to the LCD via the MIPI DSI DPI interface. To allocate a DPI data panel, you need to provide many essential parameters, including the DPI clock frequency, the pixel format, the video timing, and so on.

    - :cpp:member:`esp_lcd_dpi_panel_config_t::virtual_channel` sets the virtual channel number to use. Like the DBI interface, we also need to set the virtual channel for the DPI interface. If you only have one LCD connected, you can set this to ``0``.
    - :cpp:member:`esp_lcd_dpi_panel_config_t::dpi_clk_src` sets the clock source for the DPI interface. The available clock sources are listed in :cpp:type:`mipi_dsi_dpi_clock_source_t`.
    - :cpp:member:`esp_lcd_dpi_panel_config_t::dpi_clock_freq_mhz` sets the DPI clock frequency in MHz. Higher pixel clock frequency results in higher refresh rate, but may cause flickering if the DMA bandwidth is not sufficient or the LCD controller chip does not support high pixel clock frequency.
    - :cpp:member:`esp_lcd_dpi_panel_config_t::pixel_format` sets the pixel format of the pixel data. The available pixel formats are listed in :cpp:type:`lcd_color_rgb_pixel_format_t`. We usually use **RGB888** for MIPI LCD to get the best color depth.
    - :cpp:member:`esp_lcd_dpi_panel_config_t::video_timing` sets the LCD panel specific timing parameters. All required parameters are listed in the :cpp:type:`esp_lcd_video_timing_t`, including the LCD resolution and blanking porches. Please fill them according to the datasheet of your LCD.
    - :cpp:member:`esp_lcd_dpi_panel_config_t::extra_dpi_panel_flags::use_dma2d` sets whether to use the 2D DMA peripheral to copy the user data to the frame buffer, asynchronously.

    .. code-block:: c

        esp_lcd_panel_handle_t mipi_dpi_panel = NULL;
        esp_lcd_dpi_panel_config_t dpi_config = {
            .virtual_channel = 0,
            .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
            .dpi_clock_freq_mhz = 1 * 1000,
            .pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB888,
            .video_timing = {
                .h_size = EXAMPLE_MIPI_DSI_LCD_H_RES,
                .v_size = EXAMPLE_MIPI_DSI_LCD_V_RES,
                .hsync_back_porch = EXAMPLE_MIPI_DSI_LCD_HBP,
                .hsync_pulse_width = EXAMPLE_MIPI_DSI_LCD_HSYNC,
                .hsync_front_porch = EXAMPLE_MIPI_DSI_LCD_HFP,
                .vsync_back_porch = EXAMPLE_MIPI_DSI_LCD_VBP,
                .vsync_pulse_width = EXAMPLE_MIPI_DSI_LCD_VSYNC,
                .vsync_front_porch = EXAMPLE_MIPI_DSI_LCD_VFP,
            },
            .flags.use_dma2d = true,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_dpi(mipi_dsi_bus, &dpi_config, &mipi_dpi_panel));
        ESP_ERROR_CHECK(esp_lcd_panel_init(mipi_dpi_panel));

API Reference
-------------

.. include-build-file:: inc/esp_lcd_mipi_dsi.inc
