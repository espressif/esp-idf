RGB Interfaced LCD
==================

:link_to_translation:`zh_CN:[中文]`

RGB LCD panel is created by :cpp:func:`esp_lcd_new_rgb_panel`, with various configurations specified in :cpp:type:`esp_lcd_rgb_panel_config_t`.

    - :cpp:member:`esp_lcd_rgb_panel_config_t::clk_src` selects the clock source of the RGB LCD controller. The available clock sources are listed in :cpp:type:`lcd_clock_source_t`.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::data_width` sets number of data lines consumed by the RGB interface. It can be 8/16/24.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::bits_per_pixel` specifies the number of bits per pixel. This differs from :cpp:member:`esp_lcd_rgb_panel_config_t::data_width`. By default, if this field is set to 0, the driver will automatically match the bpp to the value set in :cpp:member:`esp_lcd_rgb_panel_config_t::data_width`. However, in some scenarios, these values need to be different. For instance, a serial RGB interfaced LCD might only require ``8`` data lines, but the color depth could be ``RGB888``, meaning :cpp:member:`esp_lcd_rgb_panel_config_t::bits_per_pixel` should be set to ``24``.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::hsync_gpio_num`, :cpp:member:`esp_lcd_rgb_panel_config_t::vsync_gpio_num`, :cpp:member:`esp_lcd_rgb_panel_config_t::de_gpio_num`, :cpp:member:`esp_lcd_rgb_panel_config_t::pclk_gpio_num`, :cpp:member:`esp_lcd_rgb_panel_config_t::disp_gpio_num` and :cpp:member:`esp_lcd_rgb_panel_config_t::data_gpio_nums` are GPIO pins consumed by the RGB LCD controller. If any of them are not used, please set them to ``-1``.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::dma_burst_size` specifies the size of the DMA transfer burst. Ensure this value is a power of 2.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px` specifies the size of the bounce buffer. This is required only for the "bounce buffer" mode. For more details, see :ref:`bounce_buffer_with_single_psram_frame_buffer`.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::timings` specifies the timing parameters unique to the LCD panel. These parameters, detailed in :cpp:type:`esp_lcd_rgb_timing_t`, include the LCD resolution and blanking porches. Ensure they are set according to your LCD's datasheet.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` determines if the frame buffer should be allocated from PSRAM. For further details, see :ref:`single_frame_buffer_in_psram`.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::num_fbs` specifies how many frame buffers the driver should allocate. For backward compatibility, setting this to ``0`` will allocate a single frame buffer. If you don't want to allocate any frame buffer, use :cpp:member:`esp_lcd_rgb_panel_config_t::no_fb` instead.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::no_fb` determines whether frame buffer will be allocated. When it is set, no frame buffer will be allocated. This is also called the :ref:`bounce_buffer_only` mode.

RGB LCD Frame Buffer Operation Modes
------------------------------------

Most of the time, the RGB LCD driver should maintain at least one screen sized frame buffer. According to the number and location of the frame buffer, the driver provides several different buffer modes.

Single Frame Buffer in Internal Memory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is the default and simplest and you do not have to specify flags or bounce buffer options. A frame buffer is allocated from the internal memory. The frame data is read out by DMA to the LCD verbatim. It needs no CPU intervention to function, but it has the downside that it uses up a fair bit of the limited amount of internal memory.

.. code:: c

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // RGB565 in parallel mode, thus 16 bits in width
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = EXAMPLE_PIN_NUM_DISP_EN,
        .pclk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .vsync_gpio_num = EXAMPLE_PIN_NUM_VSYNC,
        .hsync_gpio_num = EXAMPLE_PIN_NUM_HSYNC,
        .de_gpio_num = EXAMPLE_PIN_NUM_DE,
        .data_gpio_nums = {
            EXAMPLE_PIN_NUM_DATA0,
            EXAMPLE_PIN_NUM_DATA1,
            EXAMPLE_PIN_NUM_DATA2,
            // other GPIOs
            // The number of GPIOs here should be the same to the value of "data_width" above
            ...
        },
        // The timing parameters should refer to your LCD spec
        .timings = {
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .h_res = EXAMPLE_LCD_H_RES,
            .v_res = EXAMPLE_LCD_V_RES,
            .hsync_back_porch = 40,
            .hsync_front_porch = 20,
            .hsync_pulse_width = 1,
            .vsync_back_porch = 8,
            .vsync_front_porch = 4,
            .vsync_pulse_width = 1,
        },
    };
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

.. _single_frame_buffer_in_psram:

Single Frame Buffer in PSRAM
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you have PSRAM and prefer to store the frame buffer there instead of using the limited internal memory, the LCD peripheral can utilize EDMA to fetch frame data directly from PSRAM, bypassing the internal cache. This can be enabled by setting :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` to ``true``. The trade-off is that when both the CPU and EDMA need access to PSRAM, the bandwidth is **shared** between them, meaning EDMA and the CPU each get half. If other peripherals are also using EDMA, a high pixel clock might cause LCD peripheral starvation, leading to display corruption. However, with a sufficiently low pixel clock, this approach minimizes CPU intervention.

.. only:: esp32s3

    The PSRAM shares the same SPI bus with the main flash (the one stores your firmware binary). At any given time, there can only be one consumer of the SPI bus. When you also use the main flash to serve your file system (e.g., :doc:`SPIFFS </api-reference/storage/spiffs>`), the bandwidth of the underlying SPI bus will also be shared, leading to display corruption. You can use :cpp:func:`esp_lcd_rgb_panel_set_pclk` to update the pixel clock frequency to a lower value.


.. code:: c

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // RGB565 in parallel mode, thus 16 bits in width
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = EXAMPLE_PIN_NUM_DISP_EN,
        .pclk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .vsync_gpio_num = EXAMPLE_PIN_NUM_VSYNC,
        .hsync_gpio_num = EXAMPLE_PIN_NUM_HSYNC,
        .de_gpio_num = EXAMPLE_PIN_NUM_DE,
        .data_gpio_nums = {
            EXAMPLE_PIN_NUM_DATA0,
            EXAMPLE_PIN_NUM_DATA1,
            EXAMPLE_PIN_NUM_DATA2,
            // other GPIOs
            // The number of GPIOs here should be the same to the value of "data_width" above
            ...
        },
        // The timing parameters should refer to your LCD spec
        .timings = {
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .h_res = EXAMPLE_LCD_H_RES,
            .v_res = EXAMPLE_LCD_V_RES,
            .hsync_back_porch = 40,
            .hsync_front_porch = 20,
            .hsync_pulse_width = 1,
            .vsync_back_porch = 8,
            .vsync_front_porch = 4,
            .vsync_pulse_width = 1,
        },
        .flags.fb_in_psram = true, // allocate frame buffer from PSRAM
    };
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

.. _double_frame_buffer_in_psram:

Double Frame Buffer in PSRAM
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To prevent tearing effects, the simplest method is to use two screen-sized frame buffers. Given the limited internal memory, these buffers must be allocated from PSRAM. This ensures that the frame buffer being written to by the CPU and the one being read by the EDMA are always distinct and independent. The EDMA will only switch between the two buffers once the current write operation is complete and the frame has been fully transmitted to the LCD. The main drawback of this approach is the need to maintain synchronization between the two frame buffers.

.. code:: c

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // RGB565 in parallel mode, thus 16 bits in width
        .num_fbs = 2,     // allocate double frame buffer
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = EXAMPLE_PIN_NUM_DISP_EN,
        .pclk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .vsync_gpio_num = EXAMPLE_PIN_NUM_VSYNC,
        .hsync_gpio_num = EXAMPLE_PIN_NUM_HSYNC,
        .de_gpio_num = EXAMPLE_PIN_NUM_DE,
        .data_gpio_nums = {
            EXAMPLE_PIN_NUM_DATA0,
            EXAMPLE_PIN_NUM_DATA1,
            EXAMPLE_PIN_NUM_DATA2,
            // other GPIOs
            // The number of GPIOs here should be the same to the value of "data_width" above
            ...
        },
        // The timing parameters should refer to your LCD spec
        .timings = {
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .h_res = EXAMPLE_LCD_H_RES,
            .v_res = EXAMPLE_LCD_V_RES,
            .hsync_back_porch = 40,
            .hsync_front_porch = 20,
            .hsync_pulse_width = 1,
            .vsync_back_porch = 8,
            .vsync_front_porch = 4,
            .vsync_pulse_width = 1,
        },
        .flags.fb_in_psram = true, // allocate frame buffer from PSRAM
    };
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

.. _bounce_buffer_with_single_psram_frame_buffer:

Bounce Buffer with Single PSRAM Frame Buffer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This mode allocates two "bounce buffers" from internal memory and a main frame buffer in PSRAM. To enable this mode, set the :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` flag and specify a non-zero value for :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px`. The bounce buffers only need to hold a few lines of display data, which is much smaller than the main frame buffer. The LCD peripheral uses DMA to read data from one bounce buffer while an interrupt routine uses the CPU DCache to copy data from the main PSRAM frame buffer into the other bounce buffer. Once the LCD peripheral finishes reading from the bounce buffer, the buffers swap roles, allowing the CPU to fill the other one. The advantage of this mode is achieving a higher pixel clock frequency. Since the bounce buffers are larger than the FIFOs in the EDMA path, this method is also more robust against short bandwidth spikes. The downside is a significant increase in CPU usage, and the LCD **CANNOT** function if the external memory cache is disabled, such as during OTA or NVS writes to the main flash.

.. note::

    For optimal performance in this mode, it is highly recommended to enable the "PSRAM XIP (Execute In Place)" feature by turning on the Kconfig option: :ref:`CONFIG_SPIRAM_XIP_FROM_PSRAM`. This allows the CPU to fetch instructions and read-only data directly from PSRAM instead of the main flash. Additionally, the external memory cache remains active even when writing to the main flash via SPI 1, making it feasible to display an OTA progress bar during your application updates.

.. note::

    This mode also faces issues due to limited PSRAM bandwidth. For instance, if your draw buffers are in PSRAM and their contents are copied to the internal frame buffer by CPU Core 1, while CPU Core 0 is performing another memory copy in the DMA EOF ISR, both CPUs will be accessing PSRAM via cache, sharing its bandwidth. This significantly increases the memory copy time in the DMA EOF ISR, causing the driver to fail in switching the bounce buffer promptly, resulting in a screen shift. Although the driver can detect this condition and restart in the LCD's VSYNC interrupt handler, you may still notice flickering on the screen.

.. code:: c

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // RGB565 in parallel mode, thus 16 bits in width
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .bounce_buffer_size_px = 10 * EXAMPLE_LCD_H_RES, // allocate 10 lines data as bounce buffer from internal memory
        .disp_gpio_num = EXAMPLE_PIN_NUM_DISP_EN,
        .pclk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .vsync_gpio_num = EXAMPLE_PIN_NUM_VSYNC,
        .hsync_gpio_num = EXAMPLE_PIN_NUM_HSYNC,
        .de_gpio_num = EXAMPLE_PIN_NUM_DE,
        .data_gpio_nums = {
            EXAMPLE_PIN_NUM_DATA0,
            EXAMPLE_PIN_NUM_DATA1,
            EXAMPLE_PIN_NUM_DATA2,
            // other GPIOs
            // The number of GPIOs here should be the same to the value of "data_width" above
            ...
        },
        // The timing parameters should refer to your LCD spec
        .timings = {
            .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
            .h_res = EXAMPLE_LCD_H_RES,
            .v_res = EXAMPLE_LCD_V_RES,
            .hsync_back_porch = 40,
            .hsync_front_porch = 20,
            .hsync_pulse_width = 1,
            .vsync_back_porch = 8,
            .vsync_front_porch = 4,
            .vsync_pulse_width = 1,
        },
        .flags.fb_in_psram = true, // allocate frame buffer from PSRAM
    };
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

.. _bounce_buffer_only:

Bounce Buffer Only
^^^^^^^^^^^^^^^^^^

This mode is similar to :ref:`bounce_buffer_with_single_psram_frame_buffer`, but there is no PSRAM frame buffer initialized by the LCD driver. Instead, the user supplies a callback function that is responsible for filling the bounce buffers. As this driver does not care where the written pixels come from, this allows for the callback doing e.g., on-the-fly conversion from a smaller, 8-bit-per-pixel PSRAM frame buffer to a 16-bit LCD, or even procedurally generated frame-buffer-less graphics. This option is selected by setting the :cpp:member:`esp_lcd_rgb_panel_config_t::no_fb` flag and supplying a :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px` value. And then register the :cpp:member:`esp_lcd_rgb_panel_event_callbacks_t::on_bounce_empty` callback by calling :cpp:func:`esp_lcd_rgb_panel_register_event_callbacks`.

.. note::

    In a well-designed embedded application, situations where the DMA cannot deliver data as fast as the LCD consumes it should be avoided. However, such scenarios can theoretically occur. In the {IDF_TARGET_NAME} hardware, this results in the LCD outputting dummy bytes while the DMA waits for data. If the DMA were to run in a continuous stream, it could cause a desynchronization between the LCD address from which the DMA reads data and the address from which the LCD peripheral outputs data, leading to a **permanently** shifted image.
    To prevent this, you can either enable the :ref:`CONFIG_LCD_RGB_RESTART_IN_VSYNC` option, allowing the driver to automatically restart the DMA during the VBlank interrupt, or call :cpp:func:`esp_lcd_rgb_panel_restart` to manually restart the DMA. Note that :cpp:func:`esp_lcd_rgb_panel_restart` does not restart the DMA immediately; instead, the DMA will be restarted at the next VSYNC event.

API Reference
-------------

.. include-build-file:: inc/esp_lcd_panel_rgb.inc
