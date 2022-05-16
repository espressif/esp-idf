LCD
===

Introduction
------------

ESP chips can generate various kinds of timings that needed by common LCDs on the market, like SPI LCD, I80 LCD (a.k.a Intel 8080 parallel LCD), RGB LCD, I2C LCD, etc. The ``esp_lcd`` component is officially to support those LCDs with a group of universal APIs across chips.

Functional Overview
-------------------

In ``esp_lcd``, an LCD panel is represented by :c:type:`esp_lcd_panel_handle_t`, which plays the role of an **abstract frame buffer**, regardless of the frame memory is allocated inside ESP chip or in external LCD controller. Based on the location of the frame buffer, the LCD panel allocation functions are mainly grouped into the following categories:

-  ``RGB LCD panel`` - is simply based on a group of specific synchronous signals indicating where to start and stop a frame.

-  ``Controller based LCD panel`` involves multiple steps to get a panel handle, like bus allocation, IO device registration and controller driver install.

After we get the LCD handle, the remaining LCD operations are the same for different LCD interfaces and vendors.

Application Example
-------------------

LCD examples are located under: :example:`peripherals/lcd`:

* Jpeg decoding and LCD display - :example:`peripherals/lcd/tjpgd`
* LVGL porting and animation UI - :example:`peripherals/lcd/lvgl`

API Reference
-------------

.. include-build-file:: inc/lcd_types.inc
.. include-build-file:: inc/esp_lcd_types.inc
.. include-build-file:: inc/esp_lcd_panel_io.inc
.. include-build-file:: inc/esp_lcd_panel_ops.inc
.. include-build-file:: inc/esp_lcd_panel_rgb.inc
.. include-build-file:: inc/esp_lcd_panel_vendor.inc
