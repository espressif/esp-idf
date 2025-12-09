Bootloader Image Format
=======================

:link_to_translation:`zh_CN:[中文]`

The bootloader image consists of the same structures as the application image, see :ref:`Application Image Structures <app-image-structures>`. The only difference is in the :ref:`image-format-bootloader-description` structure.

To get information about the bootloader image, please run the following command:

.. code-block::

    esptool --chip {IDF_TARGET_PATH_NAME} image-info ./build/bootloader/bootloader.bin

The resultant output will resemble the following:

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

Bootloader Description
----------------------

The ``DRAM0`` segment of the bootloader binary starts with the :cpp:type:`esp_bootloader_desc_t` structure which carries specific fields describing the bootloader. This structure is located at a fixed offset = sizeof(:cpp:type:`esp_image_header_t`) + sizeof(:cpp:type:`esp_image_segment_header_t`).

 * ``magic_byte``: the magic byte for the esp_bootloader_desc structure
 * ``reserved``: reserved for the future IDF use
 * ``secure_version``: the secure version used by the bootloader anti-rollback feature, see :ref:`CONFIG_BOOTLOADER_ANTI_ROLLBACK_ENABLE`.
 * ``version``: bootloader version, see :ref:`CONFIG_BOOTLOADER_PROJECT_VER`
 * ``idf_ver``: ESP-IDF version. [#f1]_
 * ``date`` and ``time``: compile date and time
 * ``reserved2``: reserved for the future IDF use

.. [#f1] The maximum length is 32 characters, including null-termination character.

To get the :cpp:type:`esp_bootloader_desc_t` structure from the running bootloader, use :cpp:func:`esp_bootloader_get_description`.

To get the :cpp:type:`esp_bootloader_desc_t` structure from a running application, use :cpp:func:`esp_ota_get_bootloader_description`.

API Reference
-------------

.. include-build-file:: inc/esp_bootloader_desc.inc
