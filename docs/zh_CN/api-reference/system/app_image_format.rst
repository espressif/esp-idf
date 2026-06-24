应用程序镜像格式
================

:link_to_translation:`en:[English]`

.. _app-image-structures:

应用程序镜像结构
----------------------------

应用程序镜像包含以下内容：

1. :cpp:type:`esp_image_header_t` 结构体描述了 SPI flash 的模式和内存段的数量。
2. :cpp:type:`esp_image_segment_header_t` 结构体描述了每个段、每个段的长度及其在 {IDF_TARGET_NAME} 内存中的位置，此描述后接长度为 ``data_len`` 的数据。镜像中每个段的数据偏移量的计算方式如下：

    * 0 段的偏移量 = sizeof(:cpp:type:`esp_image_header_t`) + sizeof(:cpp:type:`esp_image_segment_header_t`)
    * 1 段的偏移量 = 0 段的偏移量 + 0 段长度 + sizeof(:cpp:type:`esp_image_segment_header_t`)
    * 2 段的偏移量 = 1 段的偏移量 + 1 段长度 + sizeof(:cpp:type:`esp_image_segment_header_t`)
    * ...

``segment_count`` 字段定义了每个段的数量，存储在 :cpp:type:`esp_image_header_t` 中。各段段数量不能超过 :c:macro:`ESP_IMAGE_MAX_SEGMENTS`。

运行以下命令，获取镜像段列表：

.. code-block::

    esptool --chip {IDF_TARGET_PATH_NAME} image-info build/app.bin

.. code-block::

    esptool v5.0.2
    Image size: 137312 bytes

    ESP32 Image Header
    ==================
    Image version: 1
    Entry point: 0x40081214
    Segments: 6
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
        0  0x0711c  0x3f400020  0x00000018  DROM
        1  0x0241c  0x3ffb0000  0x0000713c  BYTE_ACCESSIBLE, DRAM
        2  0x06ab0  0x40080000  0x00009560  IRAM
        3  0x0b724  0x400d0020  0x00010018  IROM
        4  0x060c0  0x40086ab0  0x0001b744  IRAM
        5  0x00024  0x50000000  0x0002180c  RTC_DATA

    ESP32 Image Footer
    ==================
    Checksum: 0x4b (valid)
    Validation hash: 8808f05a62fe1a6e1e6830414b95229454b012eb2001511ca434d34e9e63c962 (valid)

    Application Information
    =======================
    Project name: hello_world
    App version: qa-test-esp32c61-master-2025070
    Compile time: Aug 12 2025 16:36:40
    ELF file SHA256: 10972f521b52276e988631d7408de388d639437118e8217c366f2bd93b52e1b6
    ESP-IDF: v6.0-dev-1692-g7aad0d47e66-dirt
    Minimal eFuse block revision: 0.0
    Maximal eFuse block revision: 0.99
    MMU page size: 64 KB
    Secure version: 0

应用程序启动时，ESP-IDF 日志中也会包含段的相关信息：

.. code-block::

    I (443) esp_image: segment 0: paddr=0x00020020 vaddr=0x3f400020 size=0x13ce0 ( 81120) map
    I (489) esp_image: segment 1: paddr=0x00033d08 vaddr=0x3ff80000 size=0x00000 ( 0) load
    I (530) esp_image: segment 2: paddr=0x00033d10 vaddr=0x3ff80000 size=0x00000 ( 0) load
    I (571) esp_image: segment 3: paddr=0x00033d18 vaddr=0x3ffb0000 size=0x028e0 ( 10464) load
    I (612) esp_image: segment 4: paddr=0x00036600 vaddr=0x3ffb28e0 size=0x00000 ( 0) load
    I (654) esp_image: segment 5: paddr=0x00036608 vaddr=0x40080000 size=0x00400 ( 1024) load
    I (695) esp_image: segment 6: paddr=0x00036a10 vaddr=0x40080400 size=0x09600 ( 38400) load
    I (737) esp_image: segment 7: paddr=0x00040018 vaddr=0x400d0018 size=0x62e4c (405068) map
    I (847) esp_image: segment 8: paddr=0x000a2e6c vaddr=0x40089a00 size=0x06cec ( 27884) load
    I (888) esp_image: segment 9: paddr=0x000a9b60 vaddr=0x400c0000 size=0x00000 ( 0) load
    I (929) esp_image: segment 10: paddr=0x000a9b68 vaddr=0x50000000 size=0x00004 ( 4) load
    I (971) esp_image: segment 11: paddr=0x000a9b74 vaddr=0x50000004 size=0x00000 ( 0) load
    I (1012) esp_image: segment 12: paddr=0x000a9b7c vaddr=0x50000004 size=0x00000 ( 0) load

.. only:: esp32

    有关内存段类型和地址范围的更多详细信息，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **系统和存储器** > **片上存储器** [`PDF <{IDF_TARGET_TRM_CN_URL}#sysmem>`__]。

.. only:: not esp32

    有关内存段类型和地址范围的详细信息，请参阅 **{IDF_TARGET_NAME} 技术参考手册** > **系统和存储器** > **内部存储器** [`PDF <{IDF_TARGET_TRM_CN_URL}#sysmem>`__]。

3. 镜像有一个校验和字节，位于最后一个段之后。此字节写在一个十六字节填充边界上，因此应用程序镜像可能需要填充。
4. 如果在 :cpp:type:`esp_image_header_t` 中设置了 ``hash_appended`` 字段，则会附加 SHA256 校验和字段。SHA256 哈希值的计算范围是从第一个字节开始，到这个字段为止。该字段长度为 32 字节。
5. 如果选项 :ref:`CONFIG_SECURE_SIGNED_APPS_SCHEME` 设置为 ECDSA，那么应用程序镜像将有额外的 68 字节用于 ECDSA 签名，其中包括：

    * 版本号（4 字节）
    * 签名数据（64 字节）

6. 如果选项 :ref:`CONFIG_SECURE_SIGNED_APPS_SCHEME` 设置为 RSA 或 ECDSA (V2)，则应用程序镜像将有一个额外的签名扇区，大小为 4K 字节。关于此签名扇区格式的更多内容，请参考 :ref:`signature-block-format`。

.. _app-image-format-application-description:

应用程序描述
-----------------------

应用程序二进制文件的 ``DROM`` 段从 :cpp:type:`esp_app_desc_t` 结构体开始，该结构体中包含了用于描述应用程序的特定字段，如下所示：

* ``magic_word``：:cpp:type:`esp_app_desc_t` 结构体的魔术词
* ``secure_version``：参见 :doc:`防回滚 </api-reference/system/ota>`
* ``version``：参见 :doc:`应用程序版本 </api-reference/system/misc_system_api>` [#f1]_
* ``project_name``：通过 ``PROJECT_NAME`` 填充 [#f1]_
* ``time`` 和 ``date``：编译时间和日期
* ``idf_ver``：ESP-IDF 的版本 [#f1]_
* ``app_elf_sha256``：包含应用程序 ELF 文件的 sha256 哈希
* ``min_efuse_blk_rev_full``：镜像支持的最小 eFuse 块版本，格式为：major * 100 + minor
* ``max_efuse_blk_rev_full``：镜像支持的最大 eFuse 块版本，格式为：major * 100 + minor
* ``mmu_page_size``：MMU 页大小，以 log2 格式表示
* ``spi_flash_mode``：SPI flash 模式，取自 ``CONFIG_ESPTOOLPY_FLASHMODE_VAL``，用于 OTA 过程中的兼容性检查

.. [#f1] 最大长度为 32 个字符，其中包括 null 终止符。也就是说，如果 ``PROJECT_NAME`` 的长度超过 31 个字符，超出的字符将被忽略。

这个结构体有助于识别通过空中升级 (OTA) 上传的镜像，因为其中包含一个固定的偏移量，大小为 sizeof(:cpp:type:`esp_image_header_t`) + sizeof(:cpp:type:`esp_image_segment_header_t`)。一旦设备接收到包含此结构体的第一个段，就能根据其中的充分信息来确定是否应继续更新。

要获取当前运行的应用程序的 :cpp:type:`esp_app_desc_t` 结构体，请调用 :cpp:func:`esp_app_get_description`。

要获取另一个 OTA 分区的 :cpp:type:`esp_app_desc_t` 结构体，请调用 :cpp:func:`esp_ota_get_partition_description`。


向应用程序添加自定义结构体
-------------------------------------------

也可以自定义类似的结构体，并使其相对于镜像起始位置有一个固定的偏移量。

采用以下方式向镜像添加自定义结构体：

.. code-block:: c

    const __attribute__((section(".rodata_custom_desc"))) esp_custom_app_desc_t custom_app_desc = { ... }

自定义结构体的偏移量为 sizeof(:cpp:type:`esp_image_header_t`) + sizeof(:cpp:type:`esp_image_segment_header_t`) + sizeof(:cpp:type:`esp_app_desc_t`)。

需在 ``CMakeLists.txt`` 中添加 ``target_link_libraries(${COMPONENT_TARGET} "-u custom_app_desc")``，确保自定义结构体在未使用时也位于镜像中。

API 参考
-------------

.. include-build-file:: inc/esp_app_format.inc
