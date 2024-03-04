JPEG Decoder
============

Introduction
------------

JPEG is a commonly used method of lossy compression for digital images, particularly for those images produced by digital photography. The degree of compression can be adjusted, allowing a selectable tradeoff between storage size and image quality. JPEG typically achieves 10:1 compression with little perceptible loss in image quality.

JPEG codec on {IDF_TARGET_NAME} is an image codec, which is based on the JPEG baseline standard, for compressing and decompressing images to reduce the bandwidth required to transmit images or the space required to store images, making it possible to process large-resolution images. But please note, at one time, the codec engine can only work as either encoder or decoder.

Functional Overview
-------------------

The JPEG driver offers following services:

-  `Resource Allocation <#resource-allocation>`__ - covers how to allocate JPEG resources with properly set of configurations. It also covers how to recycle the resources when they finished working.
-  `JPEG Decoder Engine <#jpeg_decoder_engine>`__ - covers behavior of JPEG decoder engine. Introduce how to use decoder engine functions to decode an image (from jpg format to raw format).
-  `Thread Safety <#thread-safety>`__ - lists which APIs are guaranteed to be thread safe by the driver.
-  `Kconfig Options <#kconfig-options>`__ - lists the supported Kconfig options that can bring different effects to the driver.

Resource Allocation
^^^^^^^^^^^^^^^^^^^

Install JPEG decoder engine
~~~~~~~~~~~~~~~~~~~~~~~~~~~

JPEG decoder engine requires the configuration that specified by :cpp:type:`jpeg_decode_engine_cfg_t`:

- :cpp:member:`jpeg_decode_engine_cfg_t::intr_priority` Set the priority of the interrupt. If set to ``0`` , then the driver will use a interrupt with low or medium priority (priority level may be one of 1,2 or 3), otherwise use the priority indicated by :cpp:member:`jpeg_decode_engine_cfg_t::intr_priority` Please use the number form (1,2,3) , not the bitmask form ((1<<1),(1<<2),(1<<3)).

If the configurations in :cpp:type:`jpeg_decode_engine_cfg_t` is specified, users can call :cpp:func:`jpeg_new_decoder_engine` to allocate and initialize a JPEG decoder engine. This function will return an JPEG decoder handle if it runs correctly. You can take following code as reference.

.. code:: c

    jpeg_decoder_handle_t jpgd_handle;

    jpeg_decode_engine_cfg_t decode_eng_cfg = {
        .intr_priority = 0,
        .timeout_ms = 40,
    };

    ESP_ERROR_CHECK(jpeg_new_decoder_engine(&decode_eng_cfg, &jpgd_handle));


Uninstall JPEG decoder engine
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If a previously installed JPEG engine is no longer needed, it's recommended to recycle the resource by calling :cpp:func:`jpeg_del_decoder_engine`, so that to release the underlying hardware.

JPEG Decoder Engine
^^^^^^^^^^^^^^^^^^^

After installing the JPEG decoder driver by :cpp:func:`jpeg_new_decoder_engine`, {IDF_TARGET_NAME} is ready to decode JPEG pictures by :cpp:func:`jpeg_decoder_process`. :cpp:func:`jpeg_decoder_process` is flexible for decoding different types of pictures by a configurable parameter called :cpp:type:`jpeg_decode_cfg_t`:

- :cpp:member:`jpeg_decode_cfg_t::output_format` Set the output raw image format.
- :cpp:member:`jpeg_decode_cfg_t::rgb_order` Set the output pixel order. (RGB or BGR).
- :cpp:member:`jpeg_decode_cfg_t::jpeg_yuv_rgb_conv_std_t` Set the output YUV and RGB conversion standard (BT601 or BT709)

Moreover, our jpeg decoder api provides a helper function which helps you get the basic information of your given image. Calling :cpp:func:`jpeg_decoder_get_info` would return the picture information structure called :cpp:func:`jpeg_decoder_get_info`. If you already know the picture basic information, this functions is unnecessary to be called.

Overall, You can take following code as reference, the code is going to decode a 1080*1920 picture.

.. code:: c

    jpeg_decode_cfg_t decode_cfg_rgb = {
        .output_format = JPEG_DECODE_OUT_FORMAT_RGB888,
        .rgb_order = JPEG_DEC_RGB_ELEMENT_ORDER_BGR,
    };

    uint8_t *bit_stream = (uint8_t*)heap_caps_aligned_calloc(JPEG_BUFFER_MALLOC_ALIGN_VALUE, 1, bit_stream_size, MALLOC_CAP_SPIRAM);
    uint8_t *out_buf = (uint8_t*)heap_caps_aligned_calloc(JPEG_BUFFER_MALLOC_ALIGN_VALUE, 1, 1920 * 1080 * 3, MALLOC_CAP_SPIRAM); // Sufficient space for output images.

    jpeg_decode_picture_info_t header_info;
    ESP_ERROR_CHECK(jpeg_decoder_get_info(bit_stream, bit_stream_size, &header_info));
    uint32_t out_size = 0;
    ESP_ERROR_CHECK(jpeg_decoder_process(jpgd_handle, &decode_cfg_rgb, bit_stream, bit_stream_size, out_buf, &out_size));

.. note::

    Firstly, in above code, you should make sure the `bit_stream` and `out_buf` should be :c:macro:`JPEG_BUFFER_MALLOC_ALIGN_VALUE` byte aligned.
    Secondly, the content of `bit_stream` buffer should not be changed until :cpp:func:`jpeg_decoder_process` returns.

Thread Safety
^^^^^^^^^^^^^

The factory function :cpp:func:`jpeg_new_decoder_engine`, :cpp:func:`jpeg_decoder_get_info`, :cpp:func:`jpeg_decoder_process`, and :cpp:func:`jpeg_del_decoder_engine` are guaranteed to be thread safe by the driver, which means, user can call them from different RTOS tasks without protection by extra locks.

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_JPEG_ENABLE_DEBUG_LOG` is used to enable the debug log at the cost of increased firmware binary size.

API Reference
-------------

.. only:: SOC_JPEG_DECODE_SUPPORTED

    .. include-build-file:: inc/jpeg_decode.inc

.. include-build-file:: inc/components/esp_driver_jpeg/include/driver/jpeg_types.inc
.. include-build-file:: inc/components/hal/include/hal/jpeg_types.inc
