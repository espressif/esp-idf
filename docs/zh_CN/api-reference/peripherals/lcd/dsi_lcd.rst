MIPI DSI 接口的 LCD
===================

:link_to_translation:`en:[English]`

#. 创建 DSI 总线，并对 D-PHY 进行初始化设置。

    .. code-block:: c

        esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
        esp_lcd_dsi_bus_config_t bus_config = {
            .bus_id = 0, // 从 0 开始编号，指定要使用的 DSI 主机
            .num_data_lanes = 2, // 要使用的数据通道数，不能超过芯片支持的数量
            .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT, // DPHY 的时钟源
            .lane_bit_rate_mbps = EXAMPLE_MIPI_DSI_LANE_BITRATE_MBPS, // 数据通道的比特率 (Mbps)
        };
        ESP_ERROR_CHECK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

#. 从 DSI 总线派生 DBI 接口。DBI 接口主要用作 esp_lcd 组件中的控制 IO 层。使用该接口可读写 LCD 设备内部的配置寄存器。在此步骤中，需要提供以下信息：

    - :cpp:member:`esp_lcd_dbi_io_config_t::virtual_channel` 设置虚拟通道号。虚拟通道是一种逻辑通道，用于从不同来源多路复用数据。如果只连接了一个 LCD，则将此值设置为 ``0``。
    - :cpp:member:`esp_lcd_dbi_io_config_t::lcd_cmd_bits` 和 :cpp:member:`esp_lcd_dbi_io_config_t::lcd_param_bits` 分别设置 LCD 控制器芯片可识别的命令及参数的位宽。不同芯片对位宽要求不同，请提前参阅 LCD 规格书。

    .. code-block:: c

        esp_lcd_panel_io_handle_t mipi_dbi_io = NULL;
        esp_lcd_dbi_io_config_t dbi_config = {
            .virtual_channel = 0,
            .lcd_cmd_bits = 8,   // 依据 LCD 规格书说明来设置
            .lcd_param_bits = 8, // 依据 LCD 规格书说明来设置
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

#. 安装 LCD 控制器驱动程序。LCD 控制器驱动程序负责向 LCD 控制器芯片发送命令和参数。在此步骤中，需要指定上一步骤中分配到的 MIPI DBI IO 设备句柄以及一些面板特定配置：

    - :cpp:member:`esp_lcd_panel_dev_config_t::bits_per_pixel` 设置每个像素的位宽。LCD 驱动程序使用此值计算要发送到 LCD 控制器芯片的字节数。
    - :cpp:member:`esp_lcd_panel_dev_config_t::reset_gpio_num` 设置复位管脚的 GPIO 编号。如果 LCD 控制器芯片没有硬件复位管脚，则将此设置为 ``-1``。
    - :cpp:member:`esp_lcd_panel_dev_config_t::rgb_ele_order` 设置像素数据的 RGB 元素顺序，可以是 **RGB** 或 **BGR**。

    .. code-block:: c

        esp_lcd_panel_handle_t ili9881c_ctrl_panel = NULL;
        esp_lcd_panel_dev_config_t lcd_dev_config = {
            .bits_per_pixel = 24, // MIPI LCD 上通常每像素使用 24 位（即 RGB888）
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
            .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_ili9881c(mipi_dbi_io, &lcd_dev_config, &ili9881c_ctrl_panel));

#. 使用上一步骤中返回的 LCD 控制面板，在基本初始化完成后可以重置 LCD 设备。之后，可以打开显示器。

    .. code-block:: c

        ESP_ERROR_CHECK(esp_lcd_panel_reset(ili9881c_ctrl_panel));
        ESP_ERROR_CHECK(esp_lcd_panel_init(ili9881c_ctrl_panel));
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(ili9881c_ctrl_panel, true));

#. 此时仍然无法使用控制面板向 MIPI LCD 发送像素数据，因为 MIPI LCD 具有高分辨率，而 LCD 控制器中没有 GRAM。因此需要维护 LCD 帧 buffer，并通过 MIPI DSI DPI 接口将其刷新到 LCD 屏幕上。分配 DPI 数据面板需要许多基本参数，例如 DPI 时钟频率、像素格式、视频时序等。

    - :cpp:member:`esp_lcd_dpi_panel_config_t::virtual_channel` 设置虚拟通道号。与 DBI 接口类似，DPI 接口也需要设置虚拟通道。如果只连接了一个 LCD，则将此值设置为 ``0``。
    - :cpp:member:`esp_lcd_dpi_panel_config_t::dpi_clk_src` 设置 DPI 接口的时钟源。可用的时钟源列表见 :cpp:type:`mipi_dsi_dpi_clock_source_t`。
    - :cpp:member:`esp_lcd_dpi_panel_config_t::dpi_clock_freq_mhz` 设置 DPI 时钟频率 (MHz)。像素时钟频率越高，刷新率越高，但如果 DMA 带宽不足或 LCD 控制器芯片不支持高像素时钟频率，则可能会导致闪烁。
    - :cpp:member:`esp_lcd_dpi_panel_config_t::pixel_format` 设置像素数据的像素格式。可用的像素格式见 :cpp:type:`lcd_color_rgb_pixel_format_t`。MIPI LCD 通常使用 **RGB888** 来获得最佳色彩深度。
    - :cpp:member:`esp_lcd_dpi_panel_config_t::video_timing` 设置 LCD 面板的特定时序参数。包括 LCD 分辨率和消隐间隔在内的必要参数列表见 :cpp:type:`esp_lcd_video_timing_t`，请依据 LCD 技术规格书填写参数。
    - :cpp:member:`esp_lcd_dpi_panel_config_t::extra_dpi_panel_flags::use_dma2d` 设置是否用 2D DMA 将用户数据异步复制到帧 buffer 中。

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

API 参考
--------

.. include-build-file:: inc/esp_lcd_mipi_dsi.inc
