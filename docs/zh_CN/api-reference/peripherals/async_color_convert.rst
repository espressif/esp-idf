================
异步色彩格式转换
================

:link_to_translation:`en:[English]`

本文介绍 ESP-IDF 中的异步色彩转换驱动。目录如下：

.. contents::
    :local:
    :depth: 2

概述
====

{IDF_TARGET_NAME} 提供 DMA2D 引擎，可以把 2D 拷贝和色彩转换工作从 CPU 卸载到硬件执行。

这个驱动适合用于：

- 将图像从一种像素格式转换为另一种像素格式
- 只转换大图中的一个矩形窗口
- 将多个转换请求排队，而不是让 CPU 自己做像素搬运
- 在 RGB 和 UYVY 格式之间转换，并选择 RGB/YUV 转换标准

异步色彩转换驱动对 DMA2D 的请求准备、队列管理和完成通知做了封装，同时提供两种使用方式：

- 带 ISR 回调通知的异步提交接口
- 基于同一路径实现、对新手更友好的阻塞接口

快速开始
========

如果你是第一次使用这个驱动，建议从最简单的流程开始：

1. 安装驱动
2. 准备一个 :cpp:type:`async_color_convert_request_t`
3. 通过阻塞或非阻塞 API 发起转换
4. 在转换完成后使用输出 buffer
5. 继续提交新请求，或在结束时卸载驱动

典型使用流程如下：

.. mermaid::

    flowchart TD
        install["安装驱动<br/>esp_async_color_convert_install_dma2d"] --> request["准备请求<br/>async_color_convert_request_t"]
        request --> blocking["阻塞路径<br/>esp_color_convert_blocking"]
        request --> nonBlocking["非阻塞路径<br/>esp_async_color_convert"]
        nonBlocking --> callback["等待回调或任务通知"]
        blocking --> result["使用转换结果 buffer"]
        callback --> result
        result --> request
        result --> uninstall["可选清理<br/>esp_async_color_convert_uninstall"]

场景 1：先从一次阻塞转换开始
============================

理解这个驱动的最简单方式，就是先完成一次转换，并在函数返回时直接拿到结果。

下面的流程与 :example:`peripherals/dma/async_color_convert` 示例一致。它把一个嵌入在 flash 中的 UYVY422 图像转换为 BGR24，然后由应用继续处理转换后的输出：

.. code:: c

    async_color_convert_handle_t conv_hdl = NULL; // 安装驱动后返回的句柄，后续 API 都要用到它
    async_color_convert_config_t config = {
        .backlog = 1,          // 这个阻塞示例一次只处理一个请求，因此 1 就够了
        .dma_burst_size = 16,  // 先使用示例里的默认 burst 大小即可
    };
    // 创建一个基于 DMA2D 后端的异步色彩转换驱动实例。
    ESP_ERROR_CHECK(esp_async_color_convert_install_dma2d(&config, &conv_hdl));

    async_color_convert_request_t req = {
        .src_buffer = sample_96x64_uyvy_yuv_start, // 源图像可以在 flash 或者 RAM 中，只要 DMA 可访问即可
        .src_stride = 96,                          // 源图像的行跨度，单位是像素
        .src_height = 64,                          // 源图像高度，单位是像素
        .src_x = 0,                                // 从源图像左上角开始取窗口
        .src_y = 0,
        .dst_buffer = dst_bgr,                     // 目标 buffer 位于 DMA 可访问的 RAM 中
        .dst_stride = 96,                          // 目标图像的行跨度，单位是像素
        .dst_height = 64,                          // 目标图像高度，单位是像素
        .dst_x = 0,                                // 从目标图像左上角开始写入结果
        .dst_y = 0,
        .copy_width = 96,                          // 转换整张图的宽度，单位是像素
        .copy_height = 64,                         // 转换整张图的高度，单位是像素
        .src_color_format = ESP_COLOR_FOURCC_UYVY,    // 源像素格式为 UYVY422
        .dst_color_format = ESP_COLOR_FOURCC_BGR24,   // 目标像素格式为 BGR24（本驱动里用它表示 RGB888）
        .color_conv_std = COLOR_CONV_STD_RGB_YUV_BT601, // 该 RGB/YUV 转换使用的标准
    };

    // 阻塞等待 DMA2D 完成转换。-1 表示一直等到完成为止。
    ESP_ERROR_CHECK(esp_color_convert_blocking(conv_hdl, &req, -1));

    // 所有转换结束后，释放驱动资源。
    ESP_ERROR_CHECK(esp_async_color_convert_uninstall(conv_hdl));

这个流程里最重要的概念有：

- :cpp:func:`esp_async_color_convert_install_dma2d` 创建驱动实例
- :cpp:type:`async_color_convert_request_t` 描述源图像、目标图像以及要转换的窗口
- :cpp:func:`esp_color_convert_blocking` 会一直等待，直到硬件完成转换
- :cpp:func:`esp_async_color_convert_uninstall` 释放驱动资源

对于阻塞 API，``timeout_ms = -1`` 表示永久等待。其他 timeout 值目前不支持，会返回 ``ESP_ERR_INVALID_ARG``。

理解 ``async_color_convert_request_t``
--------------------------------------

这个驱动最容易出错的地方，通常不是安装驱动，而是请求参数填写不正确，因此理解 :cpp:type:`async_color_convert_request_t` 很重要。

.. important::

    在 :cpp:type:`async_color_convert_request_t` 中，所有几何相关字段的单位都是 **像素**，不是字节。包括 ``src_stride``、``src_height``、``src_x``、``src_y``、``dst_stride``、``dst_height``、``dst_x``、``dst_y``、``copy_width`` 和 ``copy_height``。

    ``src_stride`` 和 ``dst_stride`` 表示的是每一整行在内存中跨越多少像素，也就是行跨度，不是本次转换窗口的宽度。当你只转换大图中的一个窗口时，它们可以大于 ``copy_width``。

这个结构体同时描述了两件事：

- 源图像和目标图像在内存中的完整布局
- 本次实际要转换的矩形窗口

关键字段含义如下：

- :cpp:member:`async_color_convert_request_t::src_buffer`
    源图像基地址
- :cpp:member:`async_color_convert_request_t::src_stride`
    源图像的行跨度，单位为像素
- :cpp:member:`async_color_convert_request_t::src_height`
    源图像高度，单位为像素
- :cpp:member:`async_color_convert_request_t::src_x` 和 :cpp:member:`async_color_convert_request_t::src_y`
    源窗口左上角坐标
- :cpp:member:`async_color_convert_request_t::dst_buffer`
    目标图像基地址
- :cpp:member:`async_color_convert_request_t::dst_stride`
    目标图像的行跨度，单位为像素
- :cpp:member:`async_color_convert_request_t::dst_height`
    目标图像高度，单位为像素
- :cpp:member:`async_color_convert_request_t::dst_x` 和 :cpp:member:`async_color_convert_request_t::dst_y`
    转换结果写入目标图像时的左上角坐标
- :cpp:member:`async_color_convert_request_t::copy_width` 和 :cpp:member:`async_color_convert_request_t::copy_height`
    本次要转换的矩形窗口尺寸
- :cpp:member:`async_color_convert_request_t::src_color_format` 和 :cpp:member:`async_color_convert_request_t::dst_color_format`
    源和目标像素格式
- :cpp:member:`async_color_convert_request_t::color_conv_std`
    RGB/YUV 转换标准，用于 RGB 和 YUV 之间的转换

源窗口和目标窗口都必须完整落在各自图像的边界之内。

支持的转换格式
--------------

本驱动支持以下格式组合：

.. list-table::
    :header-rows: 1

    * - 源格式
      - 目标格式
      - 转换标准
    * - 与目标格式相同（跳过转换）
      - 与源格式相同（跳过转换）
      - 不适用
    * - RGB565
      - RGB888
      - 不适用
    * - RGB888
      - RGB565
      - 不适用
    * - RGB888
      - UYVY422
      - BT.601
    * - RGB888
      - UYVY422
      - BT.709
    * - UYVY422
      - RGB888
      - BT.601
    * - UYVY422
      - RGB888
      - BT.709

.. note::

    在本驱动中，RGB888 使用 ``ESP_COLOR_FOURCC_BGR24``，UYVY422 使用 ``ESP_COLOR_FOURCC_UYVY``。

    所有请求都需要设置 :cpp:member:`async_color_convert_request_t::src_color_format` 和
    :cpp:member:`async_color_convert_request_t::dst_color_format`。
    当在 RGB 和 YUV 之间转换时，还需要设置 :cpp:member:`async_color_convert_request_t::color_conv_std`。

场景 2：使用异步接口和回调函数
==============================

理解了阻塞流程之后，下一步就是把请求排入队列，并在硬件完成后由中断上下文中的回调通知你。

.. code:: c

    static bool color_conv_done_cb(async_color_convert_handle_t conv_hdl,
                                   async_color_convert_event_data_t *edata,
                                   void *cb_args)
    {
        BaseType_t high_task_wakeup = pdFALSE; // FreeRTOS 在 ISR 中唤醒任务时需要这个变量
        SemaphoreHandle_t sem = (SemaphoreHandle_t)cb_args; // 提交请求时传进来的用户上下文
        // 用 ISR-safe 的方式通知等待中的任务：这次转换已经完成。
        xSemaphoreGiveFromISR(sem, &high_task_wakeup);
        // 如果刚才唤醒了更高优先级任务，就请求在 ISR 退出后立刻切换过去。
        return high_task_wakeup == pdTRUE;
    }

    async_color_convert_request_t req = {
        .src_buffer = src_buf,   // 源图像基地址
        .src_stride = src_width, // 源图像的行跨度，单位是像素
        .src_height = src_height,
        .src_x = 0,
        .src_y = 0,
        .dst_buffer = dst_buf,   // 目标图像基地址
        .dst_stride = dst_width, // 目标图像的行跨度，单位是像素
        .dst_height = dst_height,
        .dst_x = 0,
        .dst_y = 0,
        .copy_width = copy_width,
        .copy_height = copy_height,
        .src_color_format = ESP_COLOR_FOURCC_RGB16,
        .dst_color_format = ESP_COLOR_FOURCC_BGR24,
    };

    // 提交一个异步请求。函数返回时，硬件可能还在执行转换。
    ESP_ERROR_CHECK(esp_async_color_convert(conv_hdl, &req, color_conv_done_cb, sem));
    // 在任务上下文中等待回调释放信号量。
    xSemaphoreTake(sem, portMAX_DELAY);

回调运行在 ISR 上下文中，因此应尽量保持简短，并且只调用 ISR-safe API，例如 ``xSemaphoreGiveFromISR`` 或 ``xQueueSendFromISR``。

运行注意事项
============

驱动配置
--------

驱动配置字段如下：

- :cpp:member:`async_color_convert_config_t::backlog`
    最大待处理请求数。``0`` 表示使用驱动默认值。
- :cpp:member:`async_color_convert_config_t::dma_burst_size`
    DMA burst 大小，单位为字节。``0`` 表示使用驱动默认值。
- :cpp:member:`async_color_convert_config_t::intr_priority`
    DMA2D 中断优先级。``0`` 表示使用默认低/中优先级。

DMA 突发大小
------------

``dma_burst_size`` 会影响 DMA 传输效率：

- 较大的突发大小可能提高吞吐量
- 较大的突发大小也可能增加总线占用，因此并不一定适合所有工作负载
- 常见的起始取值有 16、32 和 64 字节

最佳取值取决于芯片的 DMA 控制器能力，以及系统中其他活跃组件对内存带宽的共享情况。

线程安全与 ISR 规则
-------------------

- 驱动是线程安全的。不同任务提交的请求会通过内部队列串行化。
- :cpp:func:`esp_async_color_convert` 可以在任务上下文中调用，用于排队请求。
- 回调类型 :cpp:type:`async_color_convert_isr_cb_t` 运行在 ISR 上下文中。
- 不要在回调里调用阻塞 API。
- :cpp:func:`esp_color_convert_blocking` 不能在 ISR 上下文中调用。

卸载驱动
--------

当驱动不再需要时：

.. code:: c

    // 只有在所有排队请求都完成后，才能安全卸载驱动。
    ESP_ERROR_CHECK(esp_async_color_convert_uninstall(conv_hdl));

如果仍有请求未完成，:cpp:func:`esp_async_color_convert_uninstall` 会返回 :c:macro:`ESP_ERR_INVALID_STATE`。

应用示例
========

- :example:`peripherals/dma/async_color_convert` 展示了一个面向初学者的阻塞转换流程：

  - 从映射到 flash 的嵌入式 ``.yuv`` 图像直接读取输入
  - 使用 DMA2D 将图像从 UYVY422 转换为 BGR24
  - 将转换结果做 base64 编码后输出到控制台
  - 由 pytest 重建为 PNG 工件，并与 golden 参考图进行比对

API 参考
========

异步颜色转换驱动程序函数
------------------------

.. include-build-file:: inc/esp_async_color_convert.inc
