SPI 接口的 LCD
---------------

:link_to_translation:`en:[English]`

#. 创建 SPI 总线。详情请参阅 :doc:`SPI 主机 API 文档 </api-reference/peripherals/spi_master>`。

    目前驱动支持 SPI， Quad SPI 和 Octal SPI（模拟 Intel 8080 时序）模式。

    .. code-block:: c

        spi_bus_config_t buscfg = {
            .sclk_io_num = EXAMPLE_PIN_NUM_SCLK,
            .mosi_io_num = EXAMPLE_PIN_NUM_MOSI,
            .miso_io_num = EXAMPLE_PIN_NUM_MISO,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = EXAMPLE_LCD_H_RES * 80 * sizeof(uint16_t), // 单次最多可传输 80 行像素（假设像素格式为 RGB565）
        };
        ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO)); // 启用 DMA

#. 从 SPI 总线分配一个 LCD IO 设备句柄。在此步骤中，需要提供以下信息：

    - :cpp:member:`esp_lcd_panel_io_spi_config_t::dc_gpio_num` 设置 DC 信号线的 GPIO 编号（部分 LCD 将该信号线称为 ``RS`` 线）。使用此 GPIO，LCD 驱动可以在发送命令和发送数据之间切换。
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::cs_gpio_num` 设置 CS 信号线的 GPIO 编号。使用此 GPIO，LCD 驱动可以选择 LCD 芯片。如果 SPI 总线只连接了一个设备（即此 LCD），可将 GPIO 编号设置为 ``-1``，从而独占总线。
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::pclk_hz` 设置像素时钟的频率 (Hz)。设定的频率不应超过 LCD 规格书中推荐的范围。
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::spi_mode` 设置 SPI 模式。LCD 驱动程序使用此模式与 LCD 通信。有关 SPI 模式的详细信息，请参阅 :doc:`SPI 主机 API 文档 </api-reference/peripherals/spi_master>`。
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::lcd_cmd_bits` 和 :cpp:member:`esp_lcd_panel_io_spi_config_t::lcd_param_bits` 分别设置 LCD 控制器芯片可识别的命令及参数的位宽。不同芯片对位宽要求不同，请提前参阅 LCD 规格书。
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::trans_queue_depth` 设置 SPI 传输队列的深度。该值越大，可以排队的传输越多，但消耗的内存也越多。
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::cs_ena_pretrans` 设置 SPI 在传输之前应激活 CS 信号线的 SPI 位周期数 （0-16）。
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::cs_ena_posttrans` 设置 SPI 在传输之后保持激活 CS 信号线的 SPI 位周期数 （0-16）。

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
        // 将 LCD 连接到 SPI 总线
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

#. 安装 LCD 控制器驱动程序。LCD 控制器驱动程序负责向 LCD 控制器芯片发送命令和参数。在此步骤中，需要指定上一步骤中分配到的 SPI IO 设备句柄以及一些面板特定配置：

    - :cpp:member:`esp_lcd_panel_dev_config_t::reset_gpio_num` 设置 LCD 的硬件复位 GPIO 编号。如果 LCD 没有硬件复位管脚，则将此设置为 ``-1``。
    - :cpp:member:`esp_lcd_panel_dev_config_t::rgb_ele_order` 设置每个颜色数据的 RGB 元素顺序。
    - :cpp:member:`esp_lcd_panel_dev_config_t::bits_per_pixel` 设置像素颜色数据的位宽。LCD 驱动程序使用此值计算要发送到 LCD 控制器芯片的字节数。
    - :cpp:member:`esp_lcd_panel_dev_config_t::data_endian` 指定传输到屏幕的数据的字节序。不超过一字节的颜色格式（如 RGB232）不需要指定数据字节序。若驱动程序不支持指定数据字节序，则将忽略此字段。

    .. code-block:: c

        esp_lcd_panel_handle_t panel_handle = NULL;
        esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
            .bits_per_pixel = 16,
        };
        // 为 ST7789 创建 LCD 面板句柄，并指定 SPI IO 设备句柄
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

API 参考
--------

.. include-build-file:: inc/esp_lcd_io_spi.inc
