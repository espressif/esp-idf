RGB 接口的 LCD
==============

:link_to_translation:`en:[English]`

RGB LCD 面板的分配步骤只需一步，即调用函数 :cpp:func:`esp_lcd_new_rgb_panel`，其中包含通过 :cpp:type:`esp_lcd_rgb_panel_config_t` 指定的各种配置。

    - :cpp:member:`esp_lcd_rgb_panel_config_t::clk_src` 选择 RGB LCD 控制器的时钟源。可用的时钟源参见 :cpp:type:`lcd_clock_source_t`。
    - :cpp:member:`esp_lcd_rgb_panel_config_t::data_width` 设置 RGB 接口使用的数据线数量。目前支持 8 根或 16 根。
    - :cpp:member:`esp_lcd_rgb_panel_config_t::bits_per_pixel` 设置每像素的位数 (bpp)。该字段与 :cpp:member:`esp_lcd_rgb_panel_config_t::data_width` 有所不同。默认情况下，如果将此字段设置为 0，驱动程序会自动调整 bpp 与 :cpp:member:`esp_lcd_rgb_panel_config_t::data_width` 等值。但在某些情况下，bpp 与数据线数量必须不同。例如，串行 RGB 接口的 LCD 只需要 ``8`` 条数据线，但颜色深度可以达到 ``RGB888``，此时 :cpp:member:`esp_lcd_rgb_panel_config_t::bits_per_pixel` 应设置为 ``24``。
    - :cpp:member:`esp_lcd_rgb_panel_config_t::hsync_gpio_num`、:cpp:member:`esp_lcd_rgb_panel_config_t::vsync_gpio_num`、:cpp:member:`esp_lcd_rgb_panel_config_t::de_gpio_num`、:cpp:member:`esp_lcd_rgb_panel_config_t::pclk_gpio_num`、:cpp:member:`esp_lcd_rgb_panel_config_t::disp_gpio_num` 以及 :cpp:member:`esp_lcd_rgb_panel_config_t::data_gpio_nums` 都是 RGB LCD 控制器使用的 GPIO 管脚。未使用到的管脚需设置为 `-1`。
    - :cpp:member:`esp_lcd_rgb_panel_config_t::dma_burst_size` 设置 DMA 突发传输大小，该值必须是 2 的幂次方。
    - :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px` 设置弹性 buffer 的大小。仅在“弹性 buffer”模式下需要设置此字段。详情请参阅 :ref:`bounce_buffer_with_single_psram_frame_buffer`。
    - :cpp:member:`esp_lcd_rgb_panel_config_t::timings` 设置 LCD 面板的特定时序参数。包括 LCD 分辨率和消隐间隔在内的必要参数列表见 :cpp:type:`esp_lcd_rgb_timing_t`，请依据 LCD 技术规格书填写参数。
    - :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` 设置是否从 PSRAM 中分配 frame buffer。详情请参阅 :ref:`single_frame_buffer_in_psram`。
    - :cpp:member:`esp_lcd_rgb_panel_config_t::num_fbs` 设置由驱动程序分配的 frame buffer 的数量。为了向后兼容，``0`` 表示分配 ``一个`` frame buffer。如果不想分配任何 frame buffer，请设置 :cpp:member:`esp_lcd_rgb_panel_config_t::no_fb`。
    - :cpp:member:`esp_lcd_rgb_panel_config_t::no_fb` 可决定是否分配 frame buffer。设置该字段后将不分配 frame buffer。这也被称为 :ref:`bounce_buffer_only` 模式。

.. note::

    - 当 :cpp:member:`esp_lcd_rgb_panel_config_t::data_width` 为 8 时：

        - 像素时钟 PCLK 频率建议值需小于 80 MHz；
        - 如果同时通过 :cpp:func:`esp_lcd_rgb_panel_set_yuv_conversion` 配置了 YUV 与 RGB 之间的格式转换，则 PCLK 频率建议值需小于 60 MHz。

    - 当 :cpp:member:`esp_lcd_rgb_panel_config_t::data_width` 为 16 时：

        - PCLK 频率建议值需小于 40 MHz；
        - 如果同时配置了 YUV 与 RGB 之间的格式转换，则 PCLK 频率建议值需小于 30 MHz。

GPIO 矩阵与 IOMUX 管脚
----------------------

.. only:: esp32s31

    {IDF_TARGET_NAME} 上，RGB LCD 驱动支持在满足专用管脚条件时自动使用 IOMUX，无需额外配置开关：

    - 当 ``data_gpio_nums``、``hsync_gpio_num``、``vsync_gpio_num``、``pclk_gpio_num`` 和 ``de_gpio_num`` 都配置为 RGB 外设的专用 IOMUX 管脚时，驱动会自动绕过 GPIO 矩阵。
    - 只要上述任一信号未使用专用 IOMUX 管脚，驱动就会自动回退为 GPIO 矩阵路由。

    在较高像素时钟下，建议优先使用 IOMUX 管脚以获得更稳定的时序表现。

    {IDF_TARGET_NAME} 上 RGB 接口可用的专用 IOMUX 管脚如下：

    - 当 ``data_width = 8`` 时，使用 ``DATA[0:7]``
    - 当 ``data_width = 16`` 时，使用 ``DATA[0:15]``
    - 当 ``data_width = 24`` 时，使用 ``DATA[0:23]``

    .. list-table::
       :widths: 35 65
       :header-rows: 1

       * - 信号
         - GPIO
       * - DATA[0:7]
         - 8, 9, 10, 11, 12, 13, 14, 15
       * - DATA[8:15]
         - 16, 17, 18, 19, 33, 34, 35, 36
       * - DATA[16:23]
         - 37, 38, 39, 2, 3, 4, 5, 7
       * - HSYNC
         - 44
       * - VSYNC
         - 45
       * - PCLK
         - 40
       * - DE
         - 43

.. only:: not esp32s31

    在 {IDF_TARGET_NAME} 上，RGB 信号通过 GPIO 矩阵路由。

RGB LCD frame buffer 操作模式
------------------------------

大多数情况下，RGB LCD 驱动程序应至少维护一个屏幕大小的 frame buffer。根据 frame buffer 数量和位置的不同，驱动程序提供了几种不同的 buffer 模式。

内部存储器中的单 frame buffer 模式
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

该模式为默认模式，操作最简单，且无需指定标志或弹性 buffer 选项。从内部存储器中分配 frame buffer，帧数据通过 DMA 直接读取到 LCD 上，无需 CPU 干预即可起效，但会占用相当一部分内部存储器。

.. code:: c

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // 并行模式下像素格式为 RGB565，数据宽度为 16 位
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
            // 其他 GPIO
            // 此处 GPIO 的数量应与上文中 "data_width" 的值相同
            ...
        },
        // 参照 LCD 规格书，填写时序参数
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

PSRAM 中的单 frame buffer
^^^^^^^^^^^^^^^^^^^^^^^^^

如果不想将 frame buffer 存储在有限的内部存储器中，而是将其存储在 PSRAM 中，则 LCD 外设将绕过内部 cache，使用 EDMA 直接从 PSRAM 中获取帧数据。将 :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` 设置为 ``true`` 就可以启用此功能。该模式的缺点在于，当 CPU 和 EDMA 同时需要访问 PSRAM 时，二者将 **共享** 带宽，即 CPU 与 EDMA 各自获取一半的带宽。若此时还有其他外设也在使用 EDMA，并且像素时钟频率很高，则可能导致 LCD 外设的带宽不足，造成显示损坏。但如果像素时钟频率较低，就不会出现这种问题，且只需极少的 CPU 干预。

.. only:: esp32s3

    PSRAM 与主 flash（用来存储固件二进制文件）共享同一个 SPI 总线，但二者不能同时使用总线。若主 flash 还用来存储其他文件（例如，:doc:`SPIFFS </api-reference/storage/spiffs>`），则将共享底层 SPI 总线的带宽，造成显示损坏。此时可调用 :cpp:func:`esp_lcd_rgb_panel_set_pclk` 降低像素时钟频率。


.. code:: c

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // 并行模式下像素格式为 RGB565，数据宽度为 16 位
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
            // 其他 GPIO
            // 此处 GPIO 的数量应与上文中 "data_width" 的值相同
            ...
        },
        // 参照 LCD 规格书，填写时序参数
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
        .flags.fb_in_psram = true, // 从 PSRAM 中分配 frame buffer
    };
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

.. _double_frame_buffer_in_psram:

PSRAM 中的双 frame buffer
^^^^^^^^^^^^^^^^^^^^^^^^^

为避免 LCD 显示撕裂的问题，可以使用两个屏幕大小的 frame buffer。在这种模式下，由于内部存储器空间有限，因而只能从 PSRAM 中分配 frame buffer。CPU 写入的 frame buffer 和 EDMA 读取的 frame buffer 是完全不同且相互独立的两个区域。只有当写入操作完成、且当前帧已发送到 LCD 时，EDMA 才会在两个 frame buffer 之间切换。该模式的缺点在于，必须确保两个 frame buffer 之间同步。

.. code:: c

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // 并行模式下像素格式为 RGB565，数据宽度为 16 位
        .num_fbs = 2,     // 分配双 frame buffer
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
            // 其他 GPIO
            // 此处 GPIO 的数量应与上文中 "data_width" 的值相同
            ...
        },
        // 参照 LCD 规格书，填写时序参数
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
        .flags.fb_in_psram = true, // 从 PSRAM 中分配 frame buffer
    };
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

.. _user_custom_frame_buffer:

用户自定义 frame buffer
^^^^^^^^^^^^^^^^^^^^^^^^^

用户可以提供自己分配的 frame buffer，而不是让驱动程序分配。该模式下，用户需要自己管理 frame buffer 的生命周期。

.. code:: c

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // 并行模式下像素格式为 RGB565，数据宽度为 16 位
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
            // 其他 GPIO
            // 此处 GPIO 的数量应与上文中 "data_width" 的值相同
            ...
        },
        // 参照 LCD 规格书，填写时序参数
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
        .user_fbs[0] = user_frame_buffer, // 使用用户自定义 frame buffer
    };
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

.. _bounce_buffer_with_single_psram_frame_buffer:

bounce buffer 与 PSRAM frame buffer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在该模式下，从内部存储器中分配出两个 ``bounce buffer`` 和一个位于 PSRAM 中的主 frame buffer。若想选择此模式，可设置 :cpp:member:`esp_lcd_rgb_panel_config_t::fb_in_psram` 标志并额外指定 :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px` 值（非零）。bounce buffer 只要能容纳几行显示数据即可，存储量远远低于主 frame buffer。LCD 可通过 DMA 从其中一个 bounce buffer 里读取数据，与此同时中断例程通过 CPU DCache 将数据从主 PSRAM frame buffer 复制到另一个 bounce buffer 中。一旦 LCD 完成对 bounce buffer 的数据读取，两个 buffer 将交换位置，CPU 可以填充另一个 bounce buffer。这种模式的优点在于，可以实现更高的像素时钟频率。bounce buffer 的存储量比 EDMA 路径中的 FIFO 大，即便短时间内带宽需求激增，该模式下 bounce buffer 也能有效应对。而缺点在于，CPU 使用量大幅增加，并且如果禁用外部存储器的 cache（例如，禁止通过 OTA 或 NVS 写入主 flash），LCD 将 **无法** 工作。

.. note::

    强烈建议在此模式下启用 Kconfig 选项：:menuitem:`CONFIG_SPIRAM_XIP_FROM_PSRAM`，开启“PSRAM XIP（就地执行）”功能，使 CPU 能从 PSRAM 里而不是主 flash 中提取指令和只读数据。此外，即使想通过 SPI 1 写入主 flash，外部存储器 cache 也不会被禁用，应用程序便能正常显示 OTA 进度条。

.. note::

    由于 PSRAM 带宽不足，此模式还可能存在另一个问题。例如，从 PSRAM 中分配绘图 buffer，且 buffer 中的数据被复制到 CPU 核 1 上的内部 frame buffer 中，此时在 CPU 核 0 上，DMA EOF ISR 也在进行内存复制。这种情况下，两个内核都通过 cache 访问 PSRAM 并共享 PSRAM 的带宽，DMA EOF ISR 复制内存的时间大大增加。驱动程序无法及时切换 bounce buffer，造成 LCD 屏幕移位。在 ESP32-S3 上，驱动程序可以检测到这种情况并在 LCD 的 VSYNC 中断处理程序中执行重新启动，但仍会出现屏幕闪烁现象。

.. code:: c

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16, // 并行模式下像素格式为 RGB565，数据宽度为 16 位
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .bounce_buffer_size_px = 10 * EXAMPLE_LCD_H_RES, // 从内部存储器中分配 bounce buffer，足够存储 10 行数据
        .disp_gpio_num = EXAMPLE_PIN_NUM_DISP_EN,
        .pclk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .vsync_gpio_num = EXAMPLE_PIN_NUM_VSYNC,
        .hsync_gpio_num = EXAMPLE_PIN_NUM_HSYNC,
        .de_gpio_num = EXAMPLE_PIN_NUM_DE,
        .data_gpio_nums = {
            EXAMPLE_PIN_NUM_DATA0,
            EXAMPLE_PIN_NUM_DATA1,
            EXAMPLE_PIN_NUM_DATA2,
            // 其他 GPIO
            // 此处 GPIO 的数量应与上文中 "data_width" 的值相同
            ...
        },
        // 参照 LCD 规格书，填写时序参数
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
        .flags.fb_in_psram = true, // 从 PSRAM 中分配 frame buffer
    };
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

.. _bounce_buffer_only:

只应用 bounce buffer
^^^^^^^^^^^^^^^^^^^^

该模式与 :ref:`bounce_buffer_with_single_psram_frame_buffer` 模式类似，但 LCD 驱动程序不会初始化 PSRAM frame buffer。相反，该模式依赖用户提供的回调函数来填充 bounce buffer。LCD 驱动程序无需指定写入像素的来源，因此回调函数可以执行一些操作：例如，将较小的每像素 8 位 PSRAM frame buffer 即时转换为 16 位 LCD 数据，甚至还可以转换为无 frame buffer 图形。若想选择此模式，可以设置 :cpp:member:`esp_lcd_rgb_panel_config_t::no_fb` 标志并提供 :cpp:member:`esp_lcd_rgb_panel_config_t::bounce_buffer_size_px` 值。然后通过调用 :cpp:func:`esp_lcd_rgb_panel_register_event_callbacks` 注册回调函数 :cpp:member:`esp_lcd_rgb_panel_event_callbacks_t::on_bounce_empty`。

.. only:: esp32s3

    .. note::

        虽说在设计良好的嵌入式应用程序中, DMA 传递数据的速度不应该赶不上 LCD 读取数据的速度。但理论上，此种情况还是有可能出现的。在 {IDF_TARGET_NAME} 的硬件中，这种情况会导致 LCD 在 DMA 等待数据时单纯输出 dummy 字节。若以流式传输运行 DMA，则 DMA 会将读取到的数据传输到某个 LCD 地址，同时 LCD 也会将数据输出到某个 LCD 地址，但上述两个地址可能会不同步，导致图像 **永久** 偏移。
        为防止类似情况发生，可以启用 :menuitem:`CONFIG_LCD_RGB_RESTART_IN_VSYNC` 选项，以便驱动程序在 VBlank 中断时自动重启 DMA；或者也可以调用 :cpp:func:`esp_lcd_rgb_panel_restart`，手动重启 DMA。请注意，调用 :cpp:func:`esp_lcd_rgb_panel_restart` 不会立即重启 DMA，DMA 只会在下一个 VSYNC 事件中重启。

绘制位图钩子函数
----------------

若想使用 DMA2D 加速位图的复制，驱动程序内部已实现基于 DMA2D 的位图复制钩子函数，用户只需调用 :cpp:func:`esp_lcd_rgb_panel_enable_dma2d` 即可。

.. code-block:: c

    ESP_ERROR_CHECK(esp_lcd_rgb_panel_enable_dma2d(panel_handle));

若需更高级的应用，用户可为绘制位图添加自定义钩子，例如通过 PPA 实现旋转、缩放等操作。

.. code-block:: c

    esp_lcd_panel_hooks_t hooks = {
        .draw_bitmap_hook = custom_draw_bitmap_hook,
    };
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_hooks(panel_handle, &hooks, &user_ctx));

如果自定义钩子是异步的——例如钩子函数启动 PPA 后立即返回，真正的像素处理仍由硬件在后台执行——则必须在硬件操作真正完成后再调用 :cpp:member:`esp_lcd_draw_bitmap_hook_data_t::on_hook_end`。该回调由 RGB 面板驱动实现并填入 ``hook_data``，用户无需自行编写，只需在异步操作完成时调用它，以通知驱动结束本次绘制事务；若已注册颜色传输完成回调，也会在此时被调用。

面板驱动本身不会等待上一笔绘制结束，同步由自定义钩子自行负责。最简单的做法是串行执行（上一笔完成前不启动下一笔），如下面示例所示。若希望利用 PPA 等加速器的事务队列并发提交多笔绘制，则需要为每笔事务单独保存 ``hook_data``、保证源 buffer 在硬件完成前有效，并处理好目标区域重叠等问题。

下面是一个异步自定义钩子的示意代码：钩子启动 PPA 后立即返回，并在 PPA 完成回调中调用 ``on_hook_end``。示例采用串行方式，用信号量保证同一时间只有一笔绘制在进行：

.. code-block:: c

    typedef struct {
        esp_lcd_panel_handle_t panel;
        esp_lcd_draw_bitmap_hook_data_t hook_data;
        SemaphoreHandle_t draw_sem;
        // ... 其他字段，例如 ppa_client_handle_t
    } draw_bitmap_hook_ctx_t;

    static bool ppa_trans_done_callback(ppa_client_handle_t ppa_client, ppa_event_data_t *edata, void *user_ctx)
    {
        draw_bitmap_hook_ctx_t *ctx = (draw_bitmap_hook_ctx_t *)user_ctx;
        bool need_yield = false;

        // on_hook_end 由 RGB 面板驱动提供，操作完成后直接调用即可
        if (ctx->hook_data.on_hook_end) {
            if (ctx->hook_data.on_hook_end(ctx->panel)) {
                need_yield = true;
            }
        }

        BaseType_t task_woken = pdFALSE;
        xSemaphoreGiveFromISR(ctx->draw_sem, &task_woken);
        if (task_woken == pdTRUE) {
            need_yield = true;
        }
        return need_yield;
    }

    static esp_err_t custom_draw_bitmap_hook(esp_lcd_panel_handle_t panel,
                                             const esp_lcd_draw_bitmap_hook_data_t *hook_data,
                                             void *user_ctx)
    {
        draw_bitmap_hook_ctx_t *ctx = (draw_bitmap_hook_ctx_t *)user_ctx;

        // 最简单的同步方式：等待上一笔完成后再启动本笔
        xSemaphoreTake(ctx->draw_sem, portMAX_DELAY);

        // 保存 hook_data，供完成回调稍后调用 on_hook_end
        ctx->hook_data = *hook_data;

        // 启动异步 PPA 传输后立即返回
        // ppa_do_scale_rotate_mirror(...);
        return ESP_OK;
    }

API 参考
--------

.. include-build-file:: inc/esp_lcd_panel_rgb.inc
