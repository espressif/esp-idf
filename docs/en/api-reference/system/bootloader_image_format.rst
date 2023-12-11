Bootloader Image Format
=======================

:link_to_translation:`zh_CN:[中文]`

The bootloader image consists of the same structures as the application image, see :ref:`Application Image Structures <app-image-structures>`. The only difference is in the :ref:`image-format-bootloader-description` structure.

To get information about the bootloader image, please run the following command:

.. code-block::

    esptool.py --chip {IDF_TARGET_PATH_NAME} image_info build/bootloader/bootloader.bin --version 2

The resultant output will resemble the following:

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

Bootloader Description
----------------------

The ``DRAM0`` segment of the bootloader binary starts with the :cpp:type:`esp_bootloader_desc_t` structure which carries specific fields describing the bootloader. This structure is located at a fixed offset = sizeof(:cpp:type:`esp_image_header_t`) + sizeof(:cpp:type:`esp_image_segment_header_t`).

 * ``magic_byte``: the magic byte for the esp_bootloader_desc structure
 * ``reserved``: reserved for the future IDF use
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
