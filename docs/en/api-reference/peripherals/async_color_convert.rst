=============================
Asynchronous Color Conversion
=============================

:link_to_translation:`zh_CN:[中文]`

This document introduces the Async Color Convert driver in ESP-IDF. The table of contents is as follows:

.. contents::
    :local:
    :depth: 2

Overview
========

{IDF_TARGET_NAME} provides a DMA2D engine that can offload 2D copy and color conversion work from the CPU.

This driver is useful when your application needs to:

- convert an image from one pixel format to another
- copy only a window of a larger image
- queue multiple conversions without doing the work on the CPU
- move between RGB and UYVY formats while selecting the RGB/YUV conversion standard

The Async Color Convert driver wraps DMA2D request preparation, queueing, and completion handling into a small API that supports both:

- asynchronous submission with an ISR callback
- a simpler blocking API built on top of the same request path

Quick Start
===========

If you are new to this driver, start with the simplest workflow:

1. Install the driver
2. Prepare one :cpp:type:`async_color_convert_request_t`
3. Submit the conversion through either the blocking or non-blocking API
4. Consume the converted output buffer after the conversion completes
5. Either submit another request or uninstall the driver when finished

The typical usage flow is:

.. mermaid::

    flowchart TD
        install["Install driver<br/>esp_async_color_convert_install_dma2d"] --> request["Prepare request<br/>async_color_convert_request_t"]
        request --> blocking["Blocking path<br/>esp_color_convert_blocking"]
        request --> nonBlocking["Non-blocking path<br/>esp_async_color_convert"]
        nonBlocking --> callback["Wait for callback or task notification"]
        blocking --> result["Use converted buffer"]
        callback --> result
        result --> request
        result --> uninstall["Optional cleanup<br/>esp_async_color_convert_uninstall"]

Scenario 1: Start with One Blocking Conversion
==============================================

The easiest way to learn the API is to convert one image and wait until the conversion is complete.

The following flow mirrors the :example:`peripherals/dma/async_color_convert` example. It converts one embedded UYVY422 image into BGR24 and then lets the application consume the converted output:

.. code:: c

    async_color_convert_handle_t conv_hdl = NULL; // Driver handle returned by the install API
    async_color_convert_config_t config = {
        .backlog = 1,          // One in-flight request is enough for this simple blocking example
        .dma_burst_size = 16,  // Start with the default burst size used by the example
    };
    // Create one Async Color Convert driver instance backed by DMA2D.
    ESP_ERROR_CHECK(esp_async_color_convert_install_dma2d(&config, &conv_hdl));

    async_color_convert_request_t req = {
        .src_buffer = sample_96x64_uyvy_yuv_start, // Source image can be in flash or RAM, as long as DMA can access it
        .src_stride = 96,                          // Source image row stride, in pixels
        .src_height = 64,                          // Source image height, in pixels
        .src_x = 0,                                // Start from the left edge of the source image
        .src_y = 0,
        .dst_buffer = dst_bgr,                    // Destination buffer in DMA-capable RAM
        .dst_stride = 96,                         // Destination image row stride, in pixels
        .dst_height = 64,                         // Destination image height, in pixels
        .dst_x = 0,                               // Write the converted output from the top-left corner
        .dst_y = 0,
        .copy_width = 96,                         // Convert the full image width, in pixels
        .copy_height = 64,                        // Convert the full image height, in pixels
        .src_color_format = ESP_COLOR_FOURCC_UYVY,   // Source pixels are UYVY422
        .dst_color_format = ESP_COLOR_FOURCC_BGR24,  // Destination pixels are BGR24 (used as RGB888 in this driver)
        .color_conv_std = COLOR_CONV_STD_RGB_YUV_BT601, // RGB/YUV standard used for this conversion pair
    };

    // Wait until DMA2D finishes the conversion. -1 means wait forever.
    ESP_ERROR_CHECK(esp_color_convert_blocking(conv_hdl, &req, -1));

    // Release the driver after all conversions are done.
    ESP_ERROR_CHECK(esp_async_color_convert_uninstall(conv_hdl));

This flow introduces the most important ideas:

- :cpp:func:`esp_async_color_convert_install_dma2d` creates the driver instance
- :cpp:type:`async_color_convert_request_t` describes the source image, destination image, and conversion window
- :cpp:func:`esp_color_convert_blocking` waits until the hardware finishes the conversion
- :cpp:func:`esp_async_color_convert_uninstall` releases the driver resources

For the blocking API, ``timeout_ms = -1`` means wait forever. Other timeout values are currently unsupported and return ``ESP_ERR_INVALID_ARG``.

Understanding ``async_color_convert_request_t``
-----------------------------------------------

Most application issues come from building the request incorrectly, so it is worth understanding the structure carefully.

.. important::

    In :cpp:type:`async_color_convert_request_t`, all geometry fields are measured in **pixels**, not bytes. This includes ``src_stride``, ``src_height``, ``src_x``, ``src_y``, ``dst_stride``, ``dst_height``, ``dst_x``, ``dst_y``, ``copy_width``, and ``copy_height``.

    ``src_stride`` and ``dst_stride`` are row strides, not conversion widths. They describe how many pixels each full image row spans in memory, so they can be larger than ``copy_width`` when converting a window inside a larger image.

The structure describes two things at the same time:

- the full source and destination images in memory
- the rectangular window that should be converted

The key fields are:

- :cpp:member:`async_color_convert_request_t::src_buffer`
    Base address of the source image
- :cpp:member:`async_color_convert_request_t::src_stride`
    Source image row stride in pixels
- :cpp:member:`async_color_convert_request_t::src_height`
    Source image height in pixels
- :cpp:member:`async_color_convert_request_t::src_x` and :cpp:member:`async_color_convert_request_t::src_y`
    Top-left corner of the source window
- :cpp:member:`async_color_convert_request_t::dst_buffer`
    Base address of the destination image
- :cpp:member:`async_color_convert_request_t::dst_stride`
    Destination image row stride in pixels
- :cpp:member:`async_color_convert_request_t::dst_height`
    Destination image height in pixels
- :cpp:member:`async_color_convert_request_t::dst_x` and :cpp:member:`async_color_convert_request_t::dst_y`
    Top-left corner of where the converted window should be written
- :cpp:member:`async_color_convert_request_t::copy_width` and :cpp:member:`async_color_convert_request_t::copy_height`
    Size of the rectangle to convert
- :cpp:member:`async_color_convert_request_t::src_color_format` and :cpp:member:`async_color_convert_request_t::dst_color_format`
    Source and destination pixel formats
- :cpp:member:`async_color_convert_request_t::color_conv_std`
    RGB/YUV conversion standard, used for RGB <-> YUV conversions

Both the source window and destination window must stay within the bounds of their corresponding images.

Supported Conversions
---------------------

The following format pairs are supported by this driver:

.. list-table::
    :header-rows: 1

    * - Source format
      - Destination format
      - Conversion standard
    * - same as destination (skip conversion)
      - same as source (skip conversion)
      - N/A
    * - RGB565
      - RGB888
      - N/A
    * - RGB888
      - RGB565
      - N/A
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

    In this driver, RGB888 uses ``ESP_COLOR_FOURCC_BGR24`` and UYVY422 uses ``ESP_COLOR_FOURCC_UYVY``.

    Always set :cpp:member:`async_color_convert_request_t::src_color_format` and
    :cpp:member:`async_color_convert_request_t::dst_color_format`.
    Set :cpp:member:`async_color_convert_request_t::color_conv_std` when converting between RGB and YUV.

Scenario 2: Use the Asynchronous API with a Callback
====================================================

Once the blocking flow is clear, the next step is to queue a request and let the driver notify you from interrupt context when it is finished.

.. code:: c

    static bool color_conv_done_cb(async_color_convert_handle_t conv_hdl,
                                   async_color_convert_event_data_t *edata,
                                   void *cb_args)
    {
        BaseType_t high_task_wakeup = pdFALSE; // Required by FreeRTOS when an ISR wakes a task
        SemaphoreHandle_t sem = (SemaphoreHandle_t)cb_args; // User context passed at submit time
        // Notify a waiting task that the conversion has finished.
        xSemaphoreGiveFromISR(sem, &high_task_wakeup);
        // Return true when the unblocked task should run immediately after the ISR.
        return high_task_wakeup == pdTRUE;
    }

    async_color_convert_request_t req = {
        .src_buffer = src_buf,   // Source image base address
        .src_stride = src_width, // Source image row stride, in pixels
        .src_height = src_height,
        .src_x = 0,
        .src_y = 0,
        .dst_buffer = dst_buf,   // Destination image base address
        .dst_stride = dst_width, // Destination image row stride, in pixels
        .dst_height = dst_height,
        .dst_x = 0,
        .dst_y = 0,
        .copy_width = copy_width,
        .copy_height = copy_height,
        .src_color_format = ESP_COLOR_FOURCC_RGB16,
        .dst_color_format = ESP_COLOR_FOURCC_BGR24,
    };

    // Queue one asynchronous request. The callback runs later in ISR context.
    ESP_ERROR_CHECK(esp_async_color_convert(conv_hdl, &req, color_conv_done_cb, sem));
    // Wait in task context until the callback gives the semaphore.
    xSemaphoreTake(sem, portMAX_DELAY);

The callback runs in ISR context, so keep it short and only use ISR-safe APIs such as ``xSemaphoreGiveFromISR`` or ``xQueueSendFromISR``.

Operational Notes
=================

Driver Configuration
--------------------

The driver configuration fields are:

- :cpp:member:`async_color_convert_config_t::backlog`
    Maximum number of in-flight or pending requests. ``0`` uses a driver default.
- :cpp:member:`async_color_convert_config_t::dma_burst_size`
    DMA burst size in bytes. ``0`` uses a driver default.
- :cpp:member:`async_color_convert_config_t::intr_priority`
    DMA2D interrupt priority. ``0`` uses the default low/medium priority.

DMA Burst Size
--------------

The ``dma_burst_size`` affects DMA transfer efficiency:

- Larger burst sizes may improve throughput
- Larger burst sizes can also increase bus occupancy, so they are not always best for every workload
- Common starting values are 16, 32, and 64 bytes

The best value depends on the chip's DMA controller capabilities and how much memory bandwidth is shared with other active components in the system.

Thread Safety and ISR Rules
---------------------------

- The driver is thread-safe. Requests from different tasks are serialized through the internal queue.
- :cpp:func:`esp_async_color_convert` can be called from tasks to enqueue requests.
- The callback type :cpp:type:`async_color_convert_isr_cb_t` runs in ISR context.
- Do not call blocking APIs from the callback.
- :cpp:func:`esp_color_convert_blocking` must not be called from ISR context.

Uninstalling the Driver
-----------------------

When the driver is no longer needed:

.. code:: c

    // Uninstall only after all queued conversions have completed.
    ESP_ERROR_CHECK(esp_async_color_convert_uninstall(conv_hdl));

If requests are still pending, :cpp:func:`esp_async_color_convert_uninstall` returns :c:macro:`ESP_ERR_INVALID_STATE`.

Application Example
===================

- :example:`peripherals/dma/async_color_convert` shows a beginner-friendly blocking conversion flow:

  - an embedded ``.yuv`` image is read directly from mapped flash
  - DMA2D converts the image from UYVY422 to BGR24
  - the converted output is base64-encoded and printed to the console
  - pytest reconstructs the image as a PNG artifact and compares it against a golden reference image

API Reference
=============

Async Color Convert Driver Functions
------------------------------------

.. include-build-file:: inc/esp_async_color_convert.inc
