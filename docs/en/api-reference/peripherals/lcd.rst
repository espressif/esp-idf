LCD
===

Introduction
------------

ESP chips can generate various kinds of timings that needed by common LCDs on the market, like SPI LCD, I80 LCD (a.k.a Intel 8080 parallel LCD), RGB/SRGB LCD, I2C LCD, etc. The ``esp_lcd`` component is officially to support those LCDs with a group of universal APIs across chips.

Functional Overview
-------------------

In ``esp_lcd``, an LCD panel is represented by :cpp:type:`esp_lcd_panel_handle_t`, which plays the role of an **abstract frame buffer**, regardless of the frame memory is allocated inside ESP chip or in external LCD controller. Based on the location of the frame buffer and the hardware connection interface, the LCD panel drivers are mainly grouped into the following categories:

.. list::

    - Controller based LCD driver involves multiple steps to get a panel handle, like bus allocation, IO device registration and controller driver install. The frame buffer is located in the controller's internal GRAM (Graphical RAM). ESP-IDF provides only a limited number of LCD controller drivers out of the box (e.g., ST7789, SSD1306), :ref:`more_controller_based_lcd_drivers` are maintained in the `Espressif Component Registry <https://components.espressif.com/>`__.
    - :ref:`spi_lcd_panel` describes the steps to install the SPI LCD IO driver and then get the panel handle.
    - :ref:`i2c_lcd_panel` describes the steps to install the I2C LCD IO driver and then get the panel handle.
    :SOC_LCD_I80_SUPPORTED: - :ref:`i80_lcd_panel` describes the steps to install the I80 LCD IO driver and then get the panel handle.
    :SOC_LCD_RGB_SUPPORTED: - :ref:`rgb_lcd_panel` - is based on a group of specific synchronous signals indicating where to start and stop a frame. The frame buffer is allocated on the ESP side. The driver install steps are much simplified because we don't need to install any IO interface driver in this case.
    - :ref:`lcd_panel_operations` - provides a set of APIs to operate the LCD panel, like turning on/off the display, setting the orientation, etc. These operations are common for either controller-based LCD panel driver or RGB LCD panel driver.

.. _spi_lcd_panel:

SPI Interfaced LCD
------------------

#. Create an SPI bus. Please refer to :doc:`SPI Master API doc </api-reference/peripherals/spi_master>` for more details.

    .. code-block:: c

        spi_bus_config_t buscfg = {
            .sclk_io_num = EXAMPLE_PIN_NUM_SCLK,
            .mosi_io_num = EXAMPLE_PIN_NUM_MOSI,
            .miso_io_num = EXAMPLE_PIN_NUM_MISO,
            .quadwp_io_num = -1, // Quad SPI LCD driver is not yet supported
            .quadhd_io_num = -1, // Quad SPI LCD driver is not yet supported
            .max_transfer_sz = EXAMPLE_LCD_H_RES * 80 * sizeof(uint16_t), // transfer 80 lines of pixels (assume pixel is RGB565) at most in one SPI transaction
        };
        ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO)); // Enable the DMA feature

#. Allocate an LCD IO device handle from the SPI bus. In this step, you need to provide the following information:

    - :cpp:member:`esp_lcd_panel_io_spi_config_t::dc_gpio_num`: Sets the gpio number for the DC signal line (some LCD calls this ``RS`` line). The LCD driver will use this GPIO to switch between sending command and sending data.
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::cs_gpio_num`: Sets the gpio number for the CS signal line. The LCD driver will use this GPIO to select the LCD chip. If the SPI bus only has one device attached (i.e. this LCD), you can set the gpio number to ``-1`` to occupy the bus exclusively.
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::pclk_hz` sets the frequency of the pixel clock, in Hz. The value should not exceed the range recommended in the LCD spec.
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::spi_mode` sets the SPI mode. The LCD driver will use this mode to communicate with the LCD. For the meaning of the SPI mode, please refer to the :doc:`SPI Master API doc </api-reference/peripherals/spi_master>`.
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::lcd_cmd_bits` and :cpp:member:`esp_lcd_panel_io_spi_config_t::lcd_param_bits` set the bit width of the command and parameter that recognized by the LCD controller chip. This is chip specific, you should refer to your LCD spec in advance.
    - :cpp:member:`esp_lcd_panel_io_spi_config_t::trans_queue_depth` sets the depth of the SPI transaction queue. A bigger value means more transactions can be queued up, but it also consumes more memory.

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
        // Attach the LCD to the SPI bus
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

#. Install the LCD controller driver. The LCD controller driver is responsible for sending the commands and parameters to the LCD controller chip. In this step, you need to specify the SPI IO device handle that allocated in the last step, and some panel specific configurations:

    - :cpp:member:`esp_lcd_panel_dev_config_t::reset_gpio_num` sets the LCD's hardware reset GPIO number. If the LCD does not have a hardware reset pin, set this to ``-1``.
    - :cpp:member:`esp_lcd_panel_dev_config_t::rgb_ele_order` sets the R-G-B element order of each color data.
    - :cpp:member:`esp_lcd_panel_dev_config_t::bits_per_pixel` sets the bit width of the pixel color data. The LCD driver uses this value to calculate the number of bytes to send to the LCD controller chip.
    - :cpp:member:`esp_lcd_panel_dev_config_t::data_endian` specifies the data endian to be transmitted to the screen. No need to specify for color data within 1 byte, like RGB232. For drivers that do not support specifying data endian, this field would be ignored.

    .. code-block:: c

        esp_lcd_panel_handle_t panel_handle = NULL;
        esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
            .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
            .bits_per_pixel = 16,
        };
        // Create LCD panel handle for ST7789, with the SPI IO device handle
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

.. _i2c_lcd_panel:

I2C Interfaced LCD
------------------

#. Create I2C bus. Please refer to :doc:`I2C API doc </api-reference/peripherals/i2c>` for more details.

    .. code-block:: c

        i2c_config_t i2c_conf = {
            .mode = I2C_MODE_MASTER, // I2C LCD is a master node
            .sda_io_num = EXAMPLE_PIN_NUM_SDA,
            .scl_io_num = EXAMPLE_PIN_NUM_SCL,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        };
        ESP_ERROR_CHECK(i2c_param_config(I2C_HOST, &i2c_conf));
        ESP_ERROR_CHECK(i2c_driver_install(I2C_HOST, I2C_MODE_MASTER, 0, 0, 0));

#. Allocate an LCD IO device handle from the I2C bus. In this step, you need to provide the following information:

    - :cpp:member:`esp_lcd_panel_io_i2c_config_t::dev_addr` sets the I2C device address of the LCD controller chip. The LCD driver will use this address to communicate with the LCD controller chip.
    - :cpp:member:`esp_lcd_panel_io_i2c_config_t::lcd_cmd_bits` and :cpp:member:`esp_lcd_panel_io_i2c_config_t::lcd_param_bits` set the bit width of the command and parameter that recognized by the LCD controller chip. This is chip specific, you should refer to your LCD spec in advance.

    .. code-block:: c

        esp_lcd_panel_io_handle_t io_handle = NULL;
        esp_lcd_panel_io_i2c_config_t io_config = {
            .dev_addr = EXAMPLE_I2C_HW_ADDR,
            .control_phase_bytes = 1, // refer to LCD spec
            .dc_bit_offset = 6,       // refer to LCD spec
            .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
            .lcd_param_bits = EXAMPLE_LCD_CMD_BITS,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_HOST, &io_config, &io_handle));

#. Install the LCD controller driver. The LCD controller driver is responsible for sending the commands and parameters to the LCD controller chip. In this step, you need to specify the I2C IO device handle that allocated in the last step, and some panel specific configurations:

    - :cpp:member:`esp_lcd_panel_dev_config_t::reset_gpio_num` sets the LCD's hardware reset GPIO number. If the LCD does not have a hardware reset pin, set this to ``-1``.
    - :cpp:member:`esp_lcd_panel_dev_config_t::bits_per_pixel` sets the bit width of the pixel color data. The LCD driver will use this value to calculate the number of bytes to send to the LCD controller chip.

    .. code-block:: c

        esp_lcd_panel_handle_t panel_handle = NULL;
        esp_lcd_panel_dev_config_t panel_config = {
            .bits_per_pixel = 1,
            .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

.. only:: SOC_LCD_I80_SUPPORTED

    .. _i80_lcd_panel:

    I80 Interfaced LCD
    ------------------

    #. Create I80 bus by :cpp:func:`esp_lcd_new_i80_bus`. You need to set up the following parameters for an Intel 8080 parallel bus:

        - :cpp:member:`esp_lcd_i80_bus_config_t::clk_src` sets the clock source of the I80 bus. Note, the default clock source may be different between ESP targets.
        - :cpp:member:`esp_lcd_i80_bus_config_t::wr_gpio_num` sets the GPIO number of the pixel clock (also referred as ``WR`` in some LCD spec)
        - :cpp:member:`esp_lcd_i80_bus_config_t::dc_gpio_num` sets the GPIO number of the data/command select pin (also referred as ``RS`` in some LCD spec)
        - :cpp:member:`esp_lcd_i80_bus_config_t::bus_width` sets the bit width of the data bus (only support ``8`` or ``16``)
        - :cpp:member:`esp_lcd_i80_bus_config_t::data_gpio_nums` is the array of the GPIO number of the data bus. The number of GPIOs should be equal to the :cpp:member:`esp_lcd_i80_bus_config_t::bus_width` value.
        - :cpp:member:`esp_lcd_i80_bus_config_t::max_transfer_bytes` sets the maximum number of bytes that can be transferred in one transaction.

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
                .max_transfer_bytes = EXAMPLE_LCD_H_RES * 100 * sizeof(uint16_t), // transfer 100 lines of pixels (assume pixel is RGB565) at most in one transaction
                .psram_trans_align = EXAMPLE_PSRAM_DATA_ALIGNMENT,
                .sram_trans_align = 4,
            };
            ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    #. Allocate an LCD IO device handle from the I80 bus. In this step, you need to provide the following information:

        - :cpp:member:`esp_lcd_panel_io_i80_config_t::cs_gpio_num` sets the GPIO number of the chip select pin.
        - :cpp:member:`esp_lcd_panel_io_i80_config_t::pclk_hz` sets the pixel clock frequency in Hz. Higher pixel clock frequency will result in higher refresh rate, but may cause flickering if the DMA bandwidth is not sufficient or the LCD controller chip does not support high pixel clock frequency.
        - :cpp:member:`esp_lcd_panel_io_i80_config_t::lcd_cmd_bits` and :cpp:member:`esp_lcd_panel_io_i80_config_t::lcd_param_bits` set the bit width of the command and parameter that recognized by the LCD controller chip. This is chip specific, you should refer to your LCD spec in advance.
        - :cpp:member:`esp_lcd_panel_io_i80_config_t::trans_queue_depth` sets the maximum number of transactions that can be queued in the LCD IO device. A bigger value means more transactions can be queued up, but it also consumes more memory.

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

    #. Install the LCD controller driver. The LCD controller driver is responsible for sending the commands and parameters to the LCD controller chip. In this step, you need to specify the I80 IO device handle that allocated in the last step, and some panel specific configurations:

        - :cpp:member:`esp_lcd_panel_dev_config_t::bits_per_pixel` sets the bit width of the pixel color data. The LCD driver will use this value to calculate the number of bytes to send to the LCD controller chip.
        - :cpp:member:`esp_lcd_panel_dev_config_t::reset_gpio_num` sets the GPIO number of the reset pin. If the LCD controller chip does not have a reset pin, you can set this value to ``-1``.
        - :cpp:member:`esp_lcd_panel_dev_config_t::rgb_ele_order` sets the color order the pixel color data.

        .. code-block:: c

            esp_lcd_panel_dev_config_t panel_config = {
                .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
                .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
                .bits_per_pixel = 16,
            };
            ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    .. _more_controller_based_lcd_drivers:

.. only:: not SOC_LCD_I80_SUPPORTED

    .. _more_controller_based_lcd_drivers:

More Controller Based LCD Drivers
---------------------------------

More LCD panel drivers and touch drivers are available in `IDF Component Registry <https://components.espressif.com/search/lcd>`_. The list of available and planned drivers with links is in this `table <https://github.com/espressif/esp-bsp/blob/master/LCD.md>`_.

.. only:: SOC_LCD_RGB_SUPPORTED

    .. _rgb_lcd_panel:

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
    - :cpp:member:`esp_lcd_rgb_panel_config_t::num_fbs` sets the number of frame buffers allocated by the driver. For backward compatibility, ``0`` means to allocate ``one`` frame buffer. Please use :cpp:member:`esp_lcd_rgb_panel_config_t::no_fb` if you don't want to allocate any frame buffer.
    - :cpp:member:`esp_lcd_rgb_panel_config_t::no_fb` if sets, no frame buffer will be allocated. This is also called the :ref:`bounce_buffer_only` mode.

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

    If you have PSRAM and want to store the frame buffer there rather than in the limited internal memory, the LCD peripheral will use EDMA to fetch frame data directly from the PSRAM, bypassing the internal cache. You can enable this feature by setting the :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` to ``true``. The downside of this is that when both the CPU as well as EDMA need access to the PSRAM, the bandwidth will be **shared** between them, that is, EDMA gets half and the CPUs get the other half. If there're other peripherals using EDMA as well, with a high enough pixel clock this can lead to starvation of the LCD peripheral, leading to display corruption. However, if the pixel clock is low enough for this not to be an issue, this is a solution that uses almost no CPU intervention.

    .. only:: esp32s3

        The PSRAM shares the same SPI bus with the main Flash (the one stores your firmware binary). At one time, there only be one consumer of the SPI bus. When you also use the main flash to serve your file system (e.g. :doc:`SPIFFS </api-reference/storage/spiffs>`), the bandwidth of the underlying SPI bus will also be shared, leading to display corruption. You can use :cpp:func:`esp_lcd_rgb_panel_set_pclk` to update the pixel clock frequency to a lower value.


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

    .. _bounce_buffer_with_single_psram_frame_buffer:

    Bounce Buffer with Single PSRAM Frame Buffer
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    This mode allocates two so-called ``bounce buffers`` from the internal memory, and a main frame buffer that is still in PSRAM. This mode is selected by setting the :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` flag and additionally specifying a non-zero :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px` value. The bounce buffers only need to be large enough to hold a few lines of display data, which is significantly less than the main frame buffer. The LCD peripheral will use DMA to read data from one of the bounce buffers, and meanwhile an interrupt routine will use the CPU DCache to copy data from the main PSRAM frame buffer into the other bounce buffer. Once the LCD peripheral has finished reading the bounce buffer, the two buffers change place and the CPU can fill the others. The advantage of this mode is that, you can achieve higher pixel clock frequency. As the bounce buffers are larger than the FIFOs in the EDMA path, this method is also more robust against short bandwidth spikes. The downside is a major increase in CPU use and the LCD **CAN'T** work if we disable the cache of the external memory, via e.g. OTA or NVS write to the main flash.

    .. note::

        It's highly recommended to turn on the "PSRAM XIP (Execute In Place)" feature in this mode by enabling the Kconfig options: :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS` and :ref:`CONFIG_SPIRAM_RODATA`, which allows the CPU to fetch instructions and readonly data from the PSRAM instead of the main flash. What's more, the external memory cache won't be disabled even if you attempt to write to the main flash through SPI1. This makes it possible to display an OTA progress bar for your application.

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
        In order to stop this from happening, you can either enable the :ref:`CONFIG_LCD_RGB_RESTART_IN_VSYNC` option, so the driver can restart the DMA in the VBlank interrupt automatically or call :cpp:func:`esp_lcd_rgb_panel_restart` to restart the DMA manually. Note :cpp:func:`esp_lcd_rgb_panel_restart` doesn't restart the DMA immediately, the DMA will still be restarted in the next VSYNC event.

    .. _lcd_panel_operations:

.. only:: not SOC_LCD_RGB_SUPPORTED

    .. _lcd_panel_operations:

LCD Panel IO Operations
-----------------------

* :cpp:func:`esp_lcd_panel_reset` can reset the LCD panel.
* Use :cpp:func:`esp_lcd_panel_swap_xy` and :cpp:func:`esp_lcd_panel_mirror`, you can rotate the LCD screen.
* :cpp:func:`esp_lcd_panel_disp_on_off` can turn on or off the LCD screen (different from LCD backlight).
* :cpp:func:`esp_lcd_panel_draw_bitmap` is the most significant function, that will do the magic to draw the user provided color buffer to the LCD screen, where the draw window is also configurable.

Application Example
-------------------

LCD examples are located under: :example:`peripherals/lcd`:

.. list::

    * Universal SPI LCD example with SPI touch - :example:`peripherals/lcd/spi_lcd_touch`
    * Jpeg decoding and LCD display - :example:`peripherals/lcd/tjpgd`
    :SOC_LCD_I80_SUPPORTED: * i80 controller based LCD and LVGL animation UI - :example:`peripherals/lcd/i80_controller`
    :SOC_LCD_RGB_SUPPORTED: * RGB panel example with scatter chart UI - :example:`peripherals/lcd/rgb_panel`
    * I2C interfaced OLED display scrolling text - :example:`peripherals/lcd/i2c_oled`

API Reference
-------------

.. include-build-file:: inc/lcd_types.inc
.. include-build-file:: inc/esp_lcd_types.inc
.. include-build-file:: inc/esp_lcd_panel_io.inc
.. include-build-file:: inc/esp_lcd_panel_ops.inc
.. include-build-file:: inc/esp_lcd_panel_rgb.inc
.. include-build-file:: inc/esp_lcd_panel_vendor.inc
