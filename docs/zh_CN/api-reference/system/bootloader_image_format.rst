引导加载程序镜像的格式
=======================

:link_to_translation:`en:[English]`

引导加载程序镜像与应用程序镜像具有相同的结构，参见 :ref:`app-image-structures`。二者唯一的区别在于 :ref:`描述引导加载程序的结构体 <image-format-bootloader-description>` 不同。

要查看关于引导加载程序镜像的更多内容，请运行以下命令：

.. code-block::

    esptool.py --chip {IDF_TARGET_PATH_NAME} image_info build/bootloader/bootloader.bin --version 2

输出结果如下形式所示：

.. code-block::

    File size: 26576 (bytes)

    ESP32 image header
    ==================
    Image version: 1
    Entry point: 0x40080658
    Segments: 4
    Flash size: 2MB
    Flash freq: 40m
    Flash mode: DIO

    ESP32 extended image header
    ===========================
    WP pin: 0xee
    Flash pins drive settings: clk_drv: 0x0, q_drv: 0x0, d_drv: 0x0, cs0_drv: 0x0, hd_drv: 0x0, wp_drv: 0x0
    Chip ID: 0
    Minimal chip revision: v0.0, (legacy min_rev = 0)
    Maximal chip revision: v3.99

    Segments information
    ====================
    Segment   Length   Load addr   File offs  Memory types
    -------  -------  ----------  ----------  ------------
        1  0x01bb0  0x3fff0030  0x00000018  BYTE_ACCESSIBLE, DRAM, DIRAM_DRAM
        2  0x03c90  0x40078000  0x00001bd0  CACHE_APP
        3  0x00004  0x40080400  0x00005868  IRAM
        4  0x00f2c  0x40080404  0x00005874  IRAM

    ESP32 image footer
    ==================
    Checksum: 0x65 (valid)
    Validation hash: 6f31a7f8512f26f6bce7c3b270f93bf6cf1ee4602c322998ca8ce27433527e92 (valid)

    Bootloader information
    ======================
    Bootloader version: 1
    ESP-IDF: v5.1-dev-4304-gcb51a3b-dirty
    Compile time: Mar 30 2023 19:14:17


.. _image-format-bootloader-description:

引导加载程序描述
----------------------

引导加载程序二进制文件的 ``DRAM0`` 段起始位置为 :cpp:type:`esp_bootloader_desc_t` 结构体，其中包含描述引导加载程序的特定字段。此结构体位置具有固定偏移量，大小为 sizeof(:cpp:type:`esp_image_header_t`) + sizeof(:cpp:type:`esp_image_segment_header_t`)。

 * ``magic_byte``：esp_bootloader_desc 结构体的魔术字节
 * ``reserved``：保留供 IDF 未来使用
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
