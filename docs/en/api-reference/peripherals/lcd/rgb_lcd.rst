RGB Interfaced LCD
==================

:link_to_translation:`zh_CN:[中文]`

RGB LCD panel is allocated in one step: :cpp:func:`esp_lcd_new_rgb_panel`, with various configurations specified by :cpp:type:`esp_lcd_rgb_panel_config_t`.

    - :cpp:member:`esp_lcd_rgb_panel_config_t::clk_src` selects the clock source for the RGB LCD controller. The available clock sources are listed in :cpp:type:`lcd_clock_source_t`.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::data_width` sets number of data lines used by the RGB interface. Currently, the supported value can be 8 or 16.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::bits_per_pixel` sets the number of bits per pixel. This is different from :cpp:member:`esp_lcd_rgb_panel_config_t::data_width`. By default, if you set this field to 0, the driver will automatically adjust the bpp to the value set in :cpp:member:`esp_lcd_rgb_panel_config_t::data_width`. But in some cases, these two values must be different. For example, a serial RGB interfaced LCD only needs ``8`` data lines, but the color width can reach to ``RGB888``, i.e., the :cpp:member:`esp_lcd_rgb_panel_config_t::bits_per_pixel` should be set to ``24``.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::hsync_gpio_num`, :cpp:member:`esp_lcd_rgb_panel_config_t::vsync_gpio_num`, :cpp:member:`esp_lcd_rgb_panel_config_t::de_gpio_num`, :cpp:member:`esp_lcd_rgb_panel_config_t::pclk_gpio_num`, :cpp:member:`esp_lcd_rgb_panel_config_t::disp_gpio_num` and :cpp:member:`esp_lcd_rgb_panel_config_t::data_gpio_nums` are GPIO pins used by the RGB LCD controller. If any of them are not used, please set them to `-1`.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::dma_burst_size` sets the DMA transfer burst size. The value must be a power of 2.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px` sets the size of bounce buffer. This is only necessary for a so-called "bounce buffer" mode. Please refer to :ref:`bounce_buffer_with_single_psram_frame_buffer` for more information.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::timings` sets the LCD panel specific timing parameters. All required parameters are listed in the :cpp:type:`esp_lcd_rgb_timing_t`, including the LCD resolution and blanking porches. Please fill them according to the datasheet of your LCD.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` sets whether to allocate the frame buffer from PSRAM or not. Please refer to :ref:`single_frame_buffer_in_psram` for more information.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::num_fbs` sets the number of frame buffers allocated by the driver. For backward compatibility, ``0`` means to allocate ``one`` frame buffer. Please use :cpp:member:`esp_lcd_rgb_panel_config_t::no_fb` if you do not want to allocate any frame buffer.
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

If you have PSRAM and want to store the frame buffer there rather than in the limited internal memory, the LCD peripheral will use EDMA to fetch frame data directly from the PSRAM, bypassing the internal cache. You can enable this feature by setting the :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` to ``true``. The downside of this is that when both the CPU as well as EDMA need access to the PSRAM, the bandwidth will be **shared** between them, that is, EDMA gets half and the CPU gets the other half. If there are other peripherals using EDMA as well, with a high enough pixel clock, they may cause starvation of the LCD peripheral, resulting in display corruption. However, if the pixel clock is low enough to avoid this issue, it provides a solution with minimal CPU intervention.

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

To avoid tearing effect, using two screen sized frame buffers is the easiest approach. In this mode, the frame buffer can only be allocated from PSRAM, because of the limited internal memory. The frame buffer that the CPU write to and the frame buffer that the EDMA read from are guaranteed to be different and independent. The EDMA will only switch between the two frame buffers when the previous write operation is finished and the current frame has been sent to the LCD. The downside of this mode is that, you have to maintain the synchronization between the two frame buffers.

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

This mode allocates two so-called ``bounce buffers`` from the internal memory, and a main frame buffer that is still in PSRAM. This mode is selected by setting the :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` flag and additionally specifying a non-zero :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px` value. The bounce buffers only need to be large enough to hold a few lines of display data, which is significantly less than the main frame buffer. The LCD peripheral uses DMA to read data from one of the bounce buffers, and meanwhile an interrupt routine uses the CPU DCache to copy data from the main PSRAM frame buffer into the other bounce buffer. Once the LCD peripheral has finished reading the bounce buffer, the two buffers change place and the CPU can fill the others. The advantage of this mode is that, you can achieve higher pixel clock frequency. As the bounce buffers are larger than the FIFOs in the EDMA path, this method is also more robust against short bandwidth spikes. The downside is a major increase in CPU use and that the LCD **CAN NOT** work if we disable the cache of the external memory, via e.g., OTA or NVS write to the main flash.

.. note::

    It is highly recommended to turn on the "PSRAM XIP (Execute In Place)" feature in this mode by enabling the Kconfig options: :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS` and :ref:`CONFIG_SPIRAM_RODATA`, which allows the CPU to fetch instructions and readonly data from the PSRAM instead of the main flash. What is more, the external memory cache will not be disabled even if you attempt to write to the main flash through SPI 1. This makes it possible to display an OTA progress bar for your application.

.. note::

    This mode still has another problem which is also caused by insufficient PSRAM bandwidth. For example, when your draw buffers are allocated from PSRAM, and their contents are copied into the internal frame buffer on CPU Core 1, on CPU Core 0, there is another memory copy happening in the DMA EOF ISR. In this situation, both CPUs are accessing the PSRAM by cache and sharing the bandwidth of the PSRAM. This increases the memory copy time that spent in the DMA EOF ISR significantly. The driver can not switch the bounce buffer in time, thus leading to a shift on the LCD screen. Although the driver can detect such a condition and perform a restart in the LCD's VSYNC interrupt handler, you still can see a flickering on the screen.

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

Note that this mode also allows for a :cpp:member:`esp_lcd_rgb_panel_config_t::bb_invalidate_cache` flag to be set. Enabling this frees up the cache lines after they are used to read out the frame buffer data from PSRAM, but it may lead to slight corruption if the other core writes data to the frame buffer at the exact time the cache lines are freed up. (Technically, a write to the frame buffer can be ignored if it falls between the cache writeback and the cache invalidate calls.)

.. _bounce_buffer_only:

Bounce Buffer Only
^^^^^^^^^^^^^^^^^^

This mode is similar to :ref:`bounce_buffer_with_single_psram_frame_buffer`, but there is no PSRAM frame buffer initialized by the LCD driver. Instead, the user supplies a callback function that is responsible for filling the bounce buffers. As this driver does not care where the written pixels come from, this allows for the callback doing e.g., on-the-fly conversion from a smaller, 8-bit-per-pixel PSRAM frame buffer to a 16-bit LCD, or even procedurally generated frame-buffer-less graphics. This option is selected by setting the :cpp:member:`esp_lcd_rgb_panel_config_t::no_fb` flag and supplying a :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px` value. And then register the :cpp:member:`esp_lcd_rgb_panel_event_callbacks_t::on_bounce_empty` callback by calling :cpp:func:`esp_lcd_rgb_panel_register_event_callbacks`.

.. note::

    In a well-designed embedded application, situations where the DMA can not deliver data as fast as the LCD consumes it should be avoided. However, such scenarios can happen in theory. In the {IDF_TARGET_NAME} hardware, this leads to the LCD simply outputting dummy bytes while DMA waits for data. If we were to run DMA in a stream fashion, a desynchronization between the LCD address for which the DMA reads the data and the LCD address for which the LCD peripheral outputs data would occur, leading to a **permanently** shifted image.
    In order to stop this from happening, you can either enable the :ref:`CONFIG_LCD_RGB_RESTART_IN_VSYNC` option, so the driver can restart the DMA in the VBlank interrupt automatically, or call :cpp:func:`esp_lcd_rgb_panel_restart` to restart the DMA manually. Note that :cpp:func:`esp_lcd_rgb_panel_restart` does not restart the DMA immediately; instead, the DMA will be restarted in the next VSYNC event.

API Reference
-------------

.. include-build-file:: inc/esp_lcd_panel_rgb.inc
