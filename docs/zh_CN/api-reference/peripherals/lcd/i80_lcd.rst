I80 接口的 LCD
---------------

:link_to_translation:`en:[English]`

#. 调用 :cpp:func:`esp_lcd_new_i80_bus` 创建 I80 总线。请为英特尔 8080 并行总线设置以下参数：

    - :cpp:member:`esp_lcd_i80_bus_config_t::clk_src` 设置 I80 总线的时钟源。请注意，不同的 ESP 芯片可能有不同的默认时钟源。
    - :cpp:member:`esp_lcd_i80_bus_config_t::wr_gpio_num` 设置像素时钟的 GPIO 编号（在某些 LCD 规格书中也被称为 ``WR``）
    - :cpp:member:`esp_lcd_i80_bus_config_t::dc_gpio_num` 设置数据或命令选择管脚的 GPIO 编号（在某些 LCD 规格书中也被称为 ``RS``）
    - :cpp:member:`esp_lcd_i80_bus_config_t::bus_width` 设置数据总线的位宽（仅支持 ``8`` 位或 ``16`` 位）
    - :cpp:member:`esp_lcd_i80_bus_config_t::data_gpio_nums` 是数据总线的 GPIO 编号数组。GPIO 的数量应与 :cpp:member:`esp_lcd_i80_bus_config_t::bus_width` 的值等同。
    - :cpp:member:`esp_lcd_i80_bus_config_t::max_transfer_bytes` 设置单次传输的最大字节数。

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
            .max_transfer_bytes = EXAMPLE_LCD_H_RES * 100 * sizeof(uint16_t), // 单次最多可传输 100 行像素（假设像素格式为 RGB565）
            .dma_burst_size = EXAMPLE_DMA_BURST_SIZE,
        };
        ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

#. 从 I80 总线分配一个 LCD IO 设备句柄。在此步骤中，需要提供以下信息：

    - :cpp:member:`esp_lcd_panel_io_i80_config_t::cs_gpio_num` 设置片选管脚的 GPIO 编号。
    - :cpp:member:`esp_lcd_panel_io_i80_config_t::pclk_hz` 设置像素时钟频率 (Hz)。像素时钟频率越高，刷新率越高，但如果 DMA 带宽不足或 LCD 控制器芯片不支持高像素时钟频率，则可能会导致闪烁。
    - :cpp:member:`esp_lcd_panel_io_i80_config_t::lcd_cmd_bits` 和 :cpp:member:`esp_lcd_panel_io_i80_config_t::lcd_param_bits` 分别设置 LCD 控制器芯片可识别的命令及参数的位宽。不同芯片对位宽要求不同，请提前参阅 LCD 规格书。
    - :cpp:member:`esp_lcd_panel_io_i80_config_t::trans_queue_depth` 设置在 LCD IO 设备中可以排队的最大传输量。该值越大，可以排队的传输越多，但消耗的内存也越多。

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

#. 安装 LCD 控制器驱动程序。LCD 控制器驱动程序负责向 LCD 控制器芯片发送命令和参数。在此步骤中，需要指定上一步骤中分配到的 I80 IO 设备句柄以及一些面板特定配置：

    - :cpp:member:`esp_lcd_panel_dev_config_t::bits_per_pixel` 设置像素颜色数据的位宽。LCD 驱动程序使用此值计算要发送到 LCD 控制器芯片的字节数。
    - :cpp:member:`esp_lcd_panel_dev_config_t::reset_gpio_num` 设置复位管脚的 GPIO 编号。如果 LCD 控制器芯片没有复位管脚，可将此值设置为 ``-1``。
    - :cpp:member:`esp_lcd_panel_dev_config_t::rgb_ele_order` 设置像素颜色数据的颜色顺序。

    .. code-block:: c

        esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
            .bits_per_pixel = 16,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

API 参考
--------

.. include-build-file:: inc/esp_lcd_io_i80.inc
