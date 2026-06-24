引导加载程序镜像的格式
=======================

:link_to_translation:`en:[English]`

引导加载程序镜像与应用程序镜像具有相同的结构，参见 :ref:`app-image-structures`。二者唯一的区别在于 :ref:`描述引导加载程序的结构体 <image-format-bootloader-description>` 不同。

要查看关于引导加载程序镜像的更多内容，请运行以下命令：

.. code-block::

    esptool --chip {IDF_TARGET_PATH_NAME} image-info ./build/bootloader/bootloader.bin

输出结果如下形式所示：

.. code-block::

    esptool v5.0.2
    Image size: 26352 bytes

    ESP32 Image Header
    ==================
    Image version: 1
    Entry point: 0x40080644
    Segments: 3
    Flash size: 2MB
    Flash freq: 40m
    Flash mode: DIO

    ESP32 Extended Image Header
    ===========================
    WP pin: 0xee (disabled)
    Flash pins drive settings: clk_drv: 0x0, q_drv: 0x0, d_drv: 0x0, cs0_drv: 0x0, hd_drv: 0x0, wp_drv: 0x0
    Chip ID: 0 (ESP32)
    Minimal chip revision: v0.0, (legacy min_rev = 0)
    Maximal chip revision: v3.99

    Segments Information
    ====================
    Segment   Length   Load addr   File offs  Memory types
    -------  -------  ----------  ----------  ------------
        0  0x018e8  0x3fff0030  0x00000018  BYTE_ACCESSIBLE, DRAM, DIRAM_DRAM
        1  0x03e58  0x40078000  0x00001908  CACHE_APP
        2  0x00f5c  0x40080400  0x00005768  IRAM

    ESP32 Image Footer
    ==================
    Checksum: 0x6b (valid)
    Validation hash: 09fdc81d436a927b5018e19073a787cd37ffce655f505ad92675edd784419034 (valid)

    Bootloader Information
    ======================
    Bootloader version: 1
    ESP-IDF: v6.0-dev-1620-g15d7e41a848-dirt
    Compile time: Aug  8 2025 16:22:1


.. _image-format-bootloader-description:

引导加载程序描述
----------------------

引导加载程序二进制文件的 ``DRAM0`` 段起始位置为 :cpp:type:`esp_bootloader_desc_t` 结构体，其中包含描述引导加载程序的特定字段。此结构体位置具有固定偏移量，大小为 sizeof(:cpp:type:`esp_image_header_t`) + sizeof(:cpp:type:`esp_image_segment_header_t`)。

 * ``magic_byte``：esp_bootloader_desc 结构体的魔术字节
 * ``reserved``：保留供 IDF 未来使用
 * ``secure_version``：引导加载程序防回滚功能使用的安全版本，请参阅:ref:`CONFIG_BOOTLOADER_ANTI_ROLLBACK_ENABLE`。
 * ``version``：引导加载程序版本，参见 :ref:`CONFIG_BOOTLOADER_PROJECT_VER`
 * ``idf_ver``：IDF 版本。[#f1]_
 * ``date`` 和 ``time``：编译日期和时间
 * ``reserved2``：保留供 IDF 未来使用

.. [#f1] 最大长度为 32 个字符，包括空终止符。

如需从正在运行的引导加载程序中获取 :cpp:type:`esp_bootloader_desc_t` 结构体，请使用 :cpp:func:`esp_bootloader_get_description`。

如需从正在运行的应用程序中获取 :cpp:type:`esp_bootloader_desc_t` 结构体，请使用 :cpp:func:`esp_ota_get_bootloader_description`。

API参考
-------------

.. include-build-file:: inc/esp_bootloader_desc.inc
