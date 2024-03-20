LCD
===

Introduction
------------

ESP chips can generate various kinds of timings needed by common LCDs on the market, like SPI LCD, I2C LCD, Parallel LCD (Intel 8080), RGB/SRGB LCD, MIPI DSI LCD and etc. The ``esp_lcd`` component offers an abstracted driver framework to support them in a unified way.

An LCD typically consists of two main planes:

* **Control Plane**: This plane allows us to read and write to the internal registers of the LCD device controller. Host typically uses this plane for tasks such as initializing the LCD power supply and performing gamma calibration.
* **Data Plane**: The data plane is responsible for transmitting pixel data to the LCD device.

Functional Overview
-------------------

In the context of ``esp_lcd``, both the data plane and the control plane are represented by the :cpp:type:`esp_lcd_panel_handle_t` type.

On some LCDs, these two planes may be combined into a single plane. In this configuration, pixel data is transmitted through the control plane, achieving functionality similar to that of the data plane. This merging is common in SPI LCDs and I2C LCDs.
Additionally, there are LCDs that do not require a separate control plane. For instance, certain RGB LCDs automatically execute necessary initialization procedures after power-up. Host devices only need to continuously refresh pixel data through the data plane. However, it's essential to note that not all RGB LCDs eliminate the control plane entirely. Some LCD devices can simultaneously support multiple interfaces, requiring the host to send specific commands via the control plane (such as those based on the SPI interface) to enable the RGB mode.

This document will discuss how to create the control plane and data plane, as mentioned earlier, based on different types of LCDs.

.. toctree::
    :maxdepth: 1

    :SOC_GPSPI_SUPPORTED: spi_lcd
    :SOC_I2C_SUPPORTED: i2c_lcd
    :SOC_LCD_I80_SUPPORTED: i80_lcd
    :SOC_LCD_RGB_SUPPORTED: rgb_lcd
    :SOC_MIPI_DSI_SUPPORTED: dsi_lcd

.. note::

    ESP-IDF provides only a limited number of LCD device controller drivers out of the box (e.g., ST7789), more drivers are available in the `Espressif Component Registry <https://components.espressif.com/components?q=esp_lcd>`__.

LCD Control Panel Operations
----------------------------

* :cpp:func:`esp_lcd_panel_reset` can reset the LCD control panel.
* :cpp:func:`esp_lcd_panel_init` performs a basic initialization of the control panel. To perform more manufacture specific initialization, please refer to :ref:`steps_add_manufacture_init`.
* By combining using :cpp:func:`esp_lcd_panel_swap_xy` and :cpp:func:`esp_lcd_panel_mirror`, you can achieve the functionality of rotating or mirroring the LCD screen.
* :cpp:func:`esp_lcd_panel_disp_on_off` can turn on or off the LCD screen by cutting down the output path from the frame buffer to the LCD screen. Please note, this is not controlling the LCD backlight. Backlight control is not covered by the ``esp_lcd`` driver.
* :cpp:func:`esp_lcd_panel_disp_sleep` can reduce the power consumption of the LCD screen by entering the sleep mode. The internal frame buffer is still retained.

LCD Data Panel Operations
-------------------------

* :cpp:func:`esp_lcd_panel_reset` can reset the LCD data panel.
* :cpp:func:`esp_lcd_panel_init` performs a basic initialization of the data panel.
* :cpp:func:`esp_lcd_panel_draw_bitmap` is the function which does the magic to flush the user draw buffer to the LCD screen, where the target draw window is configurable. Please note, this function expects the draw buffer is a 1-D array and there's no stride in between each lines.

.. _steps_add_manufacture_init:

Steps to Add Manufacture Specific Initialization
-------------------------------------------------

The LCD controller drivers (e.g., st7789) in ESP-IDF only provide basic initialization in the :cpp:func:`esp_lcd_panel_init`, leaving the vast majority of settings to the default values. Some LCD modules needs to set a bunch of manufacture specific configurations before it can display normally. These configurations usually include gamma, power voltage and so on. If you want to add manufacture specific initialization, please follow the steps below:

.. code:: c

    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    // set extra configurations e.g., gamma control
    // with the underlying IO handle
    // please consult your manufacture for special commands and corresponding values
    esp_lcd_panel_io_tx_param(io_handle, GAMMA_CMD, (uint8_t[]) {
           GAMMA_ARRAY
        }, N);
    // turn on the display
    esp_lcd_panel_disp_on_off(panel_handle, true);

Application Example
-------------------

.. list::

    * Software JPEG decoding and display - :example:`peripherals/lcd/tjpgd`
    :SOC_GPSPI_SUPPORTED: * Universal SPI LCD example with SPI touch - :example:`peripherals/lcd/spi_lcd_touch`
    :SOC_LCD_I80_SUPPORTED: * i80 controller based LCD and LVGL animation UI - :example:`peripherals/lcd/i80_controller`
    :SOC_LCD_RGB_SUPPORTED: * RGB panel example with scatter chart UI - :example:`peripherals/lcd/rgb_panel`
    :SOC_I2C_SUPPORTED: * I2C interfaced OLED display scrolling text - :example:`peripherals/lcd/i2c_oled`
    :SOC_MIPI_DSI_SUPPORTED: * MIPI DSI display example - :example:`peripherals/lcd/mipi_dsi`

API Reference
-------------

.. include-build-file:: inc/lcd_types.inc
.. include-build-file:: inc/esp_lcd_types.inc
.. include-build-file:: inc/esp_lcd_panel_io.inc
.. include-build-file:: inc/esp_lcd_panel_ops.inc
.. include-build-file:: inc/esp_lcd_panel_vendor.inc
