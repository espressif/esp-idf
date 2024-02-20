App Image Format
================

:link_to_translation:`zh_CN:[中文]`

.. _app-image-structures:

Application Image Structures
----------------------------

An application image consists of the following:

1. The :cpp:type:`esp_image_header_t` structure describes the mode of SPI flash and the count of memory segments.
2. The :cpp:type:`esp_image_segment_header_t` structure describes each segment, its length, and its location in {IDF_TARGET_NAME}'s memory, followed by the data with a length of ``data_len``. The data offset for each segment in the image is calculated in the following way:

    * offset for 0 Segment = sizeof(:cpp:type:`esp_image_header_t`) + sizeof(:cpp:type:`esp_image_segment_header_t`)
    * offset for 1 Segment = offset for 0 Segment + length of 0 Segment + sizeof(:cpp:type:`esp_image_segment_header_t`)
    * offset for 2 Segment = offset for 1 Segment + length of 1 Segment + sizeof(:cpp:type:`esp_image_segment_header_t`)
    * ...

The count of each segment is defined in the ``segment_count`` field that is stored in :cpp:type:`esp_image_header_t`. The count cannot be more than :c:macro:`ESP_IMAGE_MAX_SEGMENTS`.

To get the list of your image segments, please run the following command:

.. code-block::

    esptool.py --chip {IDF_TARGET_PATH_NAME} image_info build/app.bin

.. code-block::

    esptool.py v2.3.1
    Image version: 1
    Entry point: 40080ea4
    13 segments

    Segment 1: len 0x13ce0 load 0x3f400020 file_offs 0x00000018 SOC_DROM
    Segment 2: len 0x00000 load 0x3ff80000 file_offs 0x00013d00 SOC_RTC_DRAM
    Segment 3: len 0x00000 load 0x3ff80000 file_offs 0x00013d08 SOC_RTC_DRAM
    Segment 4: len 0x028e0 load 0x3ffb0000 file_offs 0x00013d10 DRAM
    Segment 5: len 0x00000 load 0x3ffb28e0 file_offs 0x000165f8 DRAM
    Segment 6: len 0x00400 load 0x40080000 file_offs 0x00016600 SOC_IRAM
    Segment 7: len 0x09600 load 0x40080400 file_offs 0x00016a08 SOC_IRAM
    Segment 8: len 0x62e4c load 0x400d0018 file_offs 0x00020010 SOC_IROM
    Segment 9: len 0x06cec load 0x40089a00 file_offs 0x00082e64 SOC_IROM
    Segment 10: len 0x00000 load 0x400c0000 file_offs 0x00089b58 SOC_RTC_IRAM
    Segment 11: len 0x00004 load 0x50000000 file_offs 0x00089b60 SOC_RTC_DATA
    Segment 12: len 0x00000 load 0x50000004 file_offs 0x00089b6c SOC_RTC_DATA
    Segment 13: len 0x00000 load 0x50000004 file_offs 0x00089b74 SOC_RTC_DATA
    Checksum: e8 (valid)
    Validation Hash: 407089ca0eae2bbf83b4120979d3354b1c938a49cb7a0c997f240474ef2ec76b (valid)

You can also see the information on segments in the ESP-IDF logs while your application is booting:

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

    For more details on the type of memory segments and their address ranges, see **{IDF_TARGET_NAME} Technical Reference Manual** > **System and Memory** > **Embedded Memory** [`PDF <{IDF_TARGET_TRM_EN_URL}#sysmem>`__].

.. only:: not esp32

    For more details on the type of memory segments and their address ranges, see **{IDF_TARGET_NAME} Technical Reference Manual** > **System and Memory** > **Internal Memory** [`PDF <{IDF_TARGET_TRM_EN_URL}#sysmem>`__].

3. The image has a single checksum byte after the last segment. This byte is written on a sixteen byte padded boundary, so the application image might need padding.
4. If the ``hash_appended`` field from :cpp:type:`esp_image_header_t` is set then a SHA256 checksum will be appended. The value of the SHA256 hash is calculated on the range from the first byte and up to this field. The length of this field is 32 bytes.
5. If the option :ref:`CONFIG_SECURE_SIGNED_APPS_SCHEME` is set to ECDSA then the application image will have an additional 68 bytes for an ECDSA signature, which includes:

    * version word (4 bytes)
    * signature data (64 bytes)

6. If the option :ref:`CONFIG_SECURE_SIGNED_APPS_SCHEME` is set to RSA or ECDSA (V2) then the application image will have an additional signature sector of 4 KB in size. For more details on the format of this signature sector, please refer to :ref:`signature-block-format`.

.. _app-image-format-application-description:

Application Description
-----------------------

The ``DROM`` segment of the application binary starts with the :cpp:type:`esp_app_desc_t` structure which carries specific fields describing the application:

* ``magic_word``: the magic word for the :cpp:type:`esp_app_desc_t` structure
* ``secure_version``: see :doc:`Anti-rollback </api-reference/system/ota>`
* ``version``: see :doc:`App version </api-reference/system/misc_system_api>` [#f1]_
* ``project_name``: filled from ``PROJECT_NAME`` [#f1]_
* ``time`` and ``date``: compile time and date
* ``idf_ver``: version of ESP-IDF [#f1]_
* ``app_elf_sha256``: contains SHA256 hash for the application ELF file

.. [#f1] The maximum length is 32 characters, including null-termination character. For example, if the length of ``PROJECT_NAME`` exceeds 31 characters, the excess characters will be disregarded.

This structure is useful for identification of images uploaded via Over-the-Air (OTA) updates because it has a fixed offset = sizeof(:cpp:type:`esp_image_header_t`) + sizeof(:cpp:type:`esp_image_segment_header_t`). As soon as a device receives the first fragment containing this structure, it has all the information to determine whether the update should be continued with or not.

To obtain the :cpp:type:`esp_app_desc_t` structure for the currently running application, use :cpp:func:`esp_app_get_description`.

To obtain the :cpp:type:`esp_app_desc_t` structure for another OTA partition, use :cpp:func:`esp_ota_get_partition_description`.


Adding a Custom Structure to an Application
-------------------------------------------

Users also have the opportunity to have similar structure with a fixed offset relative to the beginning of the image.

The following pattern can be used to add a custom structure to your image:

.. code-block:: c

    const __attribute__((section(".rodata_custom_desc"))) esp_custom_app_desc_t custom_app_desc = { ... }

Offset for custom structure is sizeof(:cpp:type:`esp_image_header_t`) + sizeof(:cpp:type:`esp_image_segment_header_t`) + sizeof(:cpp:type:`esp_app_desc_t`).

To guarantee that the custom structure is located in the image even if it is not used, you need to add ``target_link_libraries(${COMPONENT_TARGET} "-u custom_app_desc")`` into ``CMakeLists.txt``.

API Reference
-------------

.. include-build-file:: inc/esp_app_format.inc
