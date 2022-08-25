LCD
===

Introduction
------------

ESP chips can generate various kinds of timings that needed by common LCDs on the market, like SPI LCD, I80 LCD (a.k.a Intel 8080 parallel LCD), RGB/SRGB LCD, I2C LCD, etc. The ``esp_lcd`` component is officially to support those LCDs with a group of universal APIs across chips.

Functional Overview
-------------------

In ``esp_lcd``, an LCD panel is represented by :cpp:type:`esp_lcd_panel_handle_t`, which plays the role of an **abstract frame buffer**, regardless of the frame memory is allocated inside ESP chip or in external LCD controller. Based on the location of the frame buffer, the LCD panel allocation functions are mainly grouped into the following categories:

-  ``RGB LCD panel`` - is simply based on a group of specific synchronous signals indicating where to start and stop a frame.
-  ``Controller based LCD panel`` involves multiple steps to get a panel handle, like bus allocation, IO device registration and controller driver install.

After we get the LCD handle, the remaining LCD operations are the same for different LCD interfaces and vendors.

.. only:: SOC_LCD_RGB_SUPPORTED

    RGB Interfaced LCD
    ------------------

    RGB LCD panel is allocated in one step: :cpp:func:`esp_lcd_new_rgb_panel`, with various configurations specified by :cpp:type:`esp_lcd_rgb_panel_config_t`.

    - :cpp:member:`esp_lcd_rgb_panel_config_t::clk_src` selects the clock source for the RGB LCD controller. The available clock sources are listed in :cpp:type:`lcd_clock_source_t`.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::data_width` set number of data lines used by the RGB interface. Currently, the supported value can be 8 or 16.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::bits_per_pixel` set the number of bits per pixel. This is different from :cpp:member:`esp_lcd_rgb_panel_config_t::data_width`. By default, if you set this field to 0, the driver will automatically adjust the bpp to the :cpp:member:`esp_lcd_rgb_panel_config_t::data_width`. But in some cases, these two value must be different. For example, a Serial RGB interface LCD only needs ``8`` data lines, but the color width can reach to ``RGB888``, i.e. the :cpp:member:`esp_lcd_rgb_panel_config_t::bits_per_pixel` should be set to ``24``.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::hsync_gpio_num`, :cpp:member:`esp_lcd_rgb_panel_config_t::vsync_gpio_num`, :cpp:member:`esp_lcd_rgb_panel_config_t::de_gpio_num`, :cpp:member:`esp_lcd_rgb_panel_config_t::pclk_gpio_num`, :cpp:member:`esp_lcd_rgb_panel_config_t::disp_gpio_num` and :cpp:member:`esp_lcd_rgb_panel_config_t::data_gpio_nums` are the GPIO pins used by the RGB LCD controller. If some of them are not used, please set it to `-1`.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::sram_trans_align` and :cpp:member:`esp_lcd_rgb_panel_config_t::psram_trans_align` set the alignment of the allocated frame buffer. Internally, the DMA transfer ability will adjust against these alignment values. A higher alignment value can lead to a bigger DMA burst size. Please note, the alignment value must be a power of 2.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px` set the size of bounce buffer. This is only necessary for a so-called "bounce buffer" mode. Please refer to :ref:`bounce_buffer_with_single_psram_frame_buffer` for more information.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::timings` sets the LCD panel specific timing parameters. All required parameters are listed in the :cpp:type:`esp_lcd_rgb_timing_t`, including the LCD resolution and blanking porches. Please fill them according to the datasheet of your LCD.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` sets whether to allocate the frame buffer from PSRAM or not. Please refer to :ref:`single_frame_buffer_in_psram` for more information.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::double_fb` sets whether to enable the double frame buffer mode. Please refer to :ref:`double_frame_buffer_in_psram` for more information.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::no_fb` sets whether to use the :ref:`bounce_buffer_only` mode.

    RGB LCD Frame Buffer Operation Modes
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Most of the time, the RGB LCD driver should maintain at least one screen sized frame buffer. According to the number and location of the frame buffer, the driver provides several different buffer modes.

    Single Frame Buffer in Internal Memory
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    This is the default and simplest and you don't have to specify flags or bounce buffer options. A frame buffer is allocated from the internal memory. The frame data is read out by DMA to the LCD verbatim. It needs no CPU intervention to function, but it has the downside that it uses up a fair bit of the limited amount of internal memory.

    .. code:: c

        esp_lcd_panel_handle_t panel_handle = NULL;
        esp_lcd_rgb_panel_config_t panel_config = {
            .data_width = 16, // RGB565 in parallel mode, thus 16bit in width
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
                // The number of GPIOs here should be the same to the value of `data_width` above
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
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    If you have PSRAM and want to store the frame buffer there rather than in the limited internal memory, the LCD peripheral will use EDMA to fetch frame data directly from the PSRAM, bypassing the internal cache. You can enable this feature by setting the :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` to ``true``. The downside of this is that when both the CPU as well as EDMA need access to the PSRAM, the bandwidth will be **shared** between them, that is, EDMA gets half and the CPUs the other half. If there're other peripherals using EDMA as well, with a high enough pixel clock this can lead to starvation of the LCD peripheral, leading to display corruption. However, if the pixel clock is low enough for this not to be an issue, this is a solution that uses almost no CPU intervention.

    .. code:: c

        esp_lcd_panel_handle_t panel_handle = NULL;
        esp_lcd_rgb_panel_config_t panel_config = {
            .data_width = 16, // RGB565 in parallel mode, thus 16bit in width
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
                // The number of GPIOs here should be the same to the value of `data_width` above
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
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    To avoid tearing effect, using two screen sized frame buffers is the easiest approach. In this mode, the frame buffer can only be allocated from PSRAM, because of the limited internal memory. The frame buffer that the CPU write to and the frame buffer that the EDMA read from are guaranteed to be different and independent. The EDMA will only switch between the two frame buffers when the previous write operation is finished and the current frame has been sent to the LCD. The downside of this mode is that, you have to maintain the synchronization between the two frame buffers.

    .. code:: c

        esp_lcd_panel_handle_t panel_handle = NULL;
        esp_lcd_rgb_panel_config_t panel_config = {
            .data_width = 16, // RGB565 in parallel mode, thus 16bit in width
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
                // The number of GPIOs here should be the same to the value of `data_width` above
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
            .flags.double_fb = true,   // allocate double frame buffer
        };
        ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

    .. _bounce_buffer_with_single_psram_frame_buffer:

    Bounce Buffer with Single PSRAM Frame Buffer
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    This mode allocates two so-called ``bounce buffers`` from the internal memory, and a main frame buffer that is still in PSRAM. This mode is selected by setting the :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` flag and additionally specifying a non-zero :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px` value. The bounce buffers only need to be large enough to hold a few lines of display data, which is significantly less than the main frame buffer. The LCD peripheral will use DMA to read data from one of the bounce buffers, and meanwhile an interrupt routine will use the CPU DCache to copy data from the main PSRAM frame buffer into the other bounce buffer. Once the LCD peripheral has finished reading the bounce buffer, the two buffers change place and the CPU can fill the others. The advantage of this mode is that, you can achieve higher pixel clock frequency. As the bounce buffers are larger than the FIFOs in the EDMA path, this method is also more robust against short bandwidth spikes. The downside is a major increase in CPU use and the LCD **CAN'T** work if the cache is disabled by flash operations, e.g. OTA or NVS write.

    .. code:: c

        esp_lcd_panel_handle_t panel_handle = NULL;
        esp_lcd_rgb_panel_config_t panel_config = {
            .data_width = 16, // RGB565 in parallel mode, thus 16bit in width
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
                // The number of GPIOs here should be the same to the value of `data_width` above
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

    Note that this mode also allows for a :cpp:member:`esp_lcd_rgb_panel_config_t::bb_invalidate_cache` flag to be set. Enabling this frees up the cache lines after they're used to read out the frame buffer data from PSRAM, but it may lead to slight corruption if the other core writes data to the frame buffer at the exact time the cache lines are freed up. (Technically, a write to the frame buffer can be ignored if it falls between the cache writeback and the cache invalidate calls.)

    .. _bounce_buffer_only:

    Bounce Buffer Only
    ~~~~~~~~~~~~~~~~~~

    This mode is similar to the :ref:`bounce_buffer_with_single_psram_frame_buffer`, but there is no PSRAM frame buffer initialized by the LCD driver. Instead, the user supplies a callback function that is responsible for filling the bounce buffers. As this driver does not care where the written pixels come from, this allows for the callback doing e.g. on-the-fly conversion from a smaller, 8-bit-per-pixel PSRAM frame buffer to an 16-bit LCD, or even procedurally-generated frame-buffer-less graphics. This option is selected by setting the :cpp:member:`esp_lcd_rgb_panel_config_t::no_fb` flag and supplying a :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px` value. And then register the :cpp:member:`esp_lcd_rgb_panel_event_callbacks_t::on_bounce_empty` callback by calling :cpp:func:`esp_lcd_rgb_panel_register_event_callbacks`.

    .. note::

        It should never happen in a well-designed embedded application, but it can in theory be possible that the DMA cannot deliver data as fast as the LCD consumes it. In the {IDF_TARGET_NAME} hardware, this leads to the LCD simply outputting dummy bytes while DMA waits for data. If we were to run DMA in a stream fashion, this would mean a de-sync between the LCD address the DMA reads the data for and the LCD address the LCD peripheral thinks it outputs data for, leading to a **permanently** shifted image.
        In order to stop this from happening, you can enable the :ref:`CONFIG_LCD_RGB_RESTART_IN_VSYNC` option, so the driver will restart the DMA in the VBlank interrupt; this way we always know where it starts.

Application Example
-------------------

LCD examples are located under: :example:`peripherals/lcd`:

* Universal SPI LCD example with SPI touch - :example:`peripherals/lcd/spi_lcd_touch`
* Jpeg decoding and LCD display - :example:`peripherals/lcd/tjpgd`
* i80 controller based LCD and LVGL animation UI - :example:`peripherals/lcd/i80_controller`
* RGB panel example with scatter chart UI - :example:`peripherals/lcd/rgb_panel`
* I2C interfaced OLED display scrolling text - :example:`peripherals/lcd/i2c_oled`

Other LCD drivers
-----------------

Drivers for some LCD and touch controllers are available in `IDF Component Registry <https://components.espressif.com/search/lcd>`_. The list of available and planned drivers with links is in this `table <https://github.com/espressif/esp-bsp/blob/master/LCD.md>`__.

API Reference
-------------

.. include-build-file:: inc/lcd_types.inc
.. include-build-file:: inc/esp_lcd_types.inc
.. include-build-file:: inc/esp_lcd_panel_io.inc
.. include-build-file:: inc/esp_lcd_panel_ops.inc
.. include-build-file:: inc/esp_lcd_panel_rgb.inc
.. include-build-file:: inc/esp_lcd_panel_vendor.inc
