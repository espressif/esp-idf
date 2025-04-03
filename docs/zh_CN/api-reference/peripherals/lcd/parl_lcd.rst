并行 IO 模拟 SPI 或 I80 接口的 LCD
--------------------------------------------------------------

:link_to_translation:`en:[English]`

并行 IO 并不是总线型的外设，驱动直接为 LCD 创建并行 IO 设备。目前驱动支持 SPI (1 bit 数据位宽) 和 I80 (8 bit 数据位宽 )模式。

#. 调用 :cpp:func:`esp_lcd_new_panel_io_parl` 创建并行 IO 设备。请设置以下参数：

    - :cpp:member:`esp_lcd_panel_io_parl_config_t::clk_src` 设置并行 IO 设备的时钟源。请注意，不同的 ESP 芯片可能有不同的默认时钟源。
    - :cpp:member:`esp_lcd_panel_io_parl_config_t::clk_gpio_num` 设置像素时钟的 GPIO 编号（在某些 LCD 规格书中也被称为 ``WR`` 或者 ``SCLK``）
    - :cpp:member:`esp_lcd_panel_io_parl_config_t::dc_gpio_num` 设置数据或命令选择管脚的 GPIO 编号（在某些 LCD 规格书中也被称为 ``RS``）
    - :cpp:member:`esp_lcd_panel_io_parl_config_t::cs_gpio_num` 设置 CS 信号线的 GPIO 编号。（注意，Parallel IO LCD 驱动仅支持单个 LCD 设备）。
    - :cpp:member:`esp_lcd_panel_io_parl_config_t::data_width` 设置数据的位宽（仅支持 ``1`` 位或 ``8`` 位）
    - :cpp:member:`esp_lcd_panel_io_parl_config_t::data_gpio_nums` 是数据总线的 GPIO 编号数组。GPIO 的数量应与 :cpp:member:`esp_lcd_panel_io_parl_config_t::data_width` 的值等同。
    - :cpp:member:`esp_lcd_panel_io_parl_config_t::max_transfer_bytes` 设置单次传输的最大字节数。
    - :cpp:member:`esp_lcd_panel_io_parl_config_t::dma_burst_size` 设置 dma burst 传输的字节数。
    - :cpp:member:`esp_lcd_panel_io_parl_config_t::pclk_hz` 设置像素时钟的频率 (Hz)。较高的像素时钟频率会带来较高的刷新率，但如果 DMA 带宽不足或 LCD 控制器芯片不支持高像素时钟频率，则可能会导致显示异常。
    - :cpp:member:`esp_lcd_panel_io_parl_config_t::dc_levels` 设置 DC 数据选择和命令选择的有效电平。
    - :cpp:member:`esp_lcd_panel_io_parl_config_t::lcd_cmd_bits` 和 :cpp:member:`esp_lcd_panel_io_spi_config_t::lcd_param_bits` 分别设置 LCD 控制器芯片可识别的命令及参数的位宽。不同芯片对位宽要求不同，请提前参阅 LCD 规格书。
    - :cpp:member:`esp_lcd_panel_io_parl_config_t::trans_queue_depth` 设置并行 IO 传输队列的深度。该值越大，可以排队的传输越多，但消耗的内存也越多。

    .. code-block:: c

        esp_lcd_panel_io_handle_t io_handle = NULL;
        esp_lcd_panel_io_parl_config_t io_config = {
            .clk_src = PARLIO_CLK_SRC_DEFAULT,
            .dc_gpio_num = EXAMPLE_PIN_NUM_DC,
            .clk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
            .data_gpio_nums = {
                EXAMPLE_PIN_NUM_DATA0, // 驱动 SPI 接口的 LCD 时需要设置 DATA0，驱动 I80 接口的 LCD 时需要设置 DATA0~7
            },
            .data_width = 1, // 驱动 SPI 接口的 LCD 时数据宽度为 1，驱动 I80 接口的 LCD 时数据宽度为 8
            .max_transfer_bytes = EXAMPLE_LCD_H_RES * 100 * sizeof(uint16_t), // 单次最多可传输 100 行像素（假设像素格式为 RGB565）
            .dma_burst_size = EXAMPLE_DMA_BURST_SIZE,
            .cs_gpio_num = EXAMPLE_PIN_NUM_CS,
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .trans_queue_depth = 10,
            .dc_levels = {
                .dc_cmd_level = 0,
                .dc_data_level = 1,
            },
            .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
            .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        };

        ESP_ERROR_CHECK(esp_lcd_new_panel_io_parl(&io_config, io_handle));

    .. only:: not SOC_PARLIO_SUPPORT_I80_LCD

        .. note::

            注意，由于硬件限制，{IDF_TARGET_NAME} 不能通过并行 IO 模拟驱动 I80 接口 LCD。

#. 安装 LCD 控制器驱动程序。LCD 控制器驱动程序负责向 LCD 控制器芯片发送命令和参数。在此步骤中，需要指定上一步骤中分配到的并行 IO 设备句柄以及一些面板特定配置：

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
        // 为 ST7789 创建 LCD 面板句柄，并指定并行 IO 设备句柄
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

API 参考
--------

.. include-build-file:: inc/esp_lcd_io_parl.inc
