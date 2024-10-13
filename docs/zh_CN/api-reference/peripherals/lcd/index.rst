LCD
===

:link_to_translation:`en:[English]`

简介
----

ESP 系列芯片可以发出市场上常见的 LCD（如 SPI LCD、I2C LCD、并行 LCD (Intel 8080)、RGB/SRGB LCD、MIPI DSI LCD 等）所需的各种时序。``esp_lcd`` 组件为上述各类 LCD 提供了一个统一的抽象驱动框架。

LCD 通常由两个主要平面组成：

* **控制平面**：通过该平面，可以读取、写入 LCD 设备控制器的内部寄存器。主机通常使用该平面来初始化 LCD 电源或进行伽玛校正等。
* **数据平面**：数据平面负责向 LCD 设备传输像素数据。

功能概述
--------

在 ``esp_lcd`` 的上下文中，数据平面和控制平面都由 :cpp:type:`esp_lcd_panel_handle_t` 数据类型表示。

在某些 LCD 中，上述两个平面可能会合并为一个平面。此时，像素数据通过控制平面传输，实现类似数据平面的功能。这在 SPI LCD 和 I2C LCD 中很常见。

此外，还有一些 LCD 不需要单独的控制平面。例如，某些 RGB LCD 在上电后会自动执行必要的初始化过程，主机只需通过数据平面不断刷新像素数据即可。但要注意，并非所有 RGB LCD 都完全不需要控制平面。部分 LCD 设备同时支持多种接口，需要主机通过控制平面（例如基于 SPI 接口）发送特定命令以启用 RGB 模式。

本文将讨论如何为不同类型的 LCD 创建控制平面和数据平面。

.. toctree::
    :maxdepth: 1

    :SOC_GPSPI_SUPPORTED: spi_lcd
    :SOC_I2C_SUPPORTED: i2c_lcd
    :SOC_LCD_I80_SUPPORTED: i80_lcd
    :SOC_LCD_RGB_SUPPORTED: rgb_lcd
    :SOC_MIPI_DSI_SUPPORTED: dsi_lcd

.. note::

    ESP-IDF 仓库中自带的 LCD 设备控制器驱动程序数量有限（例如，ST7789 驱动程序是开箱即用的）。更多驱动程序可通过 `乐鑫组件注册表 <https://components.espressif.com/components?q=esp_lcd>`__ 获取。

LCD 控制面板操作
----------------

* :cpp:func:`esp_lcd_panel_reset` 可以重置 LCD 控制面板。
* :cpp:func:`esp_lcd_panel_init` 执行基本的控制面板初始化。有关特定制造商的初始化设置，请参阅 :ref:`steps_add_manufacture_init`。
* 通过组合使用 :cpp:func:`esp_lcd_panel_swap_xy` 和 :cpp:func:`esp_lcd_panel_mirror`，可以实现 LCD 屏幕旋转和 LCD 屏幕镜像功能。
* :cpp:func:`esp_lcd_panel_disp_on_off` 可以通过切断从帧 buffer 到 LCD 屏幕的输出路径来打开或关闭 LCD 屏幕。请注意，这与控制 LCD 背光不同，``esp_lcd`` 驱动程序无法控制 LCD 背光。
* :cpp:func:`esp_lcd_panel_disp_sleep` 可以通过进入睡眠模式减少 LCD 屏幕的功耗，而内部的帧 buffer 仍然保留。

LCD 数据面板操作
----------------

* :cpp:func:`esp_lcd_panel_reset` 可以重置 LCD 数据面板。
* :cpp:func:`esp_lcd_panel_init` 执行基本的数据面板初始化。
* :cpp:func:`esp_lcd_panel_draw_bitmap` 可以将绘制 buffer 刷新到 LCD 屏幕上，其中目标绘制窗口是可配置的。请注意，使用该函数需要确保绘制 buffer 是一维数组，且每行像素数据之间没有跨距。

.. _steps_add_manufacture_init:

添加特定制造商的初始化设置
--------------------------

在 ESP-IDF 中，LCD 控制器驱动程序（例如 st7789）在函数 :cpp:func:`esp_lcd_panel_init` 中只提供了基本的控制面板初始化设置，大部分设置使用默认值。部分 LCD 模组需要进行一系列特定制造商的配置（通常包含伽玛配置、电压配置等）才能正常显示。若想添加特定制造商的初始化设置，请参照以下步骤：

.. code:: c

    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    // 进行额外的配置，例如伽玛控制
    // 使用底层 IO 句柄
    // 请咨询制造商来获取特殊命令和相应的值
    esp_lcd_panel_io_tx_param(io_handle, GAMMA_CMD, (uint8_t[]) {
           GAMMA_ARRAY
        }, N);
    // 打开显示屏
    esp_lcd_panel_disp_on_off(panel_handle, true);

应用示例
--------

.. list::

    * :example:`peripherals/lcd/tjpgd` 演示了如何解码 JPEG 图像并在 SPI 接口的 LCD 上显示图像，同时周期性地旋转图像。
    :SOC_GPSPI_SUPPORTED: * :example:`peripherals/lcd/spi_lcd_touch` 演示了如何在 ESP-IDF 项目中使用 `esp_lcd` 组件为 LCD 屏幕添加自定义驱动，例如 GC9A01 或 ILI9341，以及如何启用 STMPE610 触摸控制器。
    :SOC_LCD_I80_SUPPORTED: * :example:`peripherals/lcd/i80_controller` 演示了如何将 LVGL 库移植到 `esp_lcd` 驱动层，以创建图形用户界面。
    :SOC_LCD_RGB_SUPPORTED: * :example:`peripherals/lcd/rgb_panel` 展示了如何安装 RGB 面板驱动程序，并基于 LVGL 库在屏幕上显示散点图。
    :SOC_I2C_SUPPORTED: * :example:`peripherals/lcd/i2c_oled` 演示了如何使用 `esp_lcd` 组件中的 SSD1306 面板驱动来简化移植 LVGL 库，并在 OLED 屏幕上显示滚动文本。
    :SOC_MIPI_DSI_SUPPORTED: * :example:`peripherals/lcd/mipi_dsi` 演示了如何安装 MIPI DSI LCD 驱动程序，并在屏幕上显示一个 LVGL 小部件。


API 参考
--------

.. include-build-file:: inc/lcd_types.inc
.. include-build-file:: inc/esp_lcd_types.inc
.. include-build-file:: inc/esp_lcd_panel_io.inc
.. include-build-file:: inc/esp_lcd_panel_ops.inc
.. include-build-file:: inc/esp_lcd_panel_vendor.inc
