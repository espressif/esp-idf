NVS Bootloader
==============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

This guide provides an overview of the NVS functionality available for custom bootloader code, along with its limitations.

Due to the constraints of the bootloader runtime environment, custom bootloader code cannot use the full NVS API directly. Instead, NVS provides a simplified API that offers read-only access to NVS data.

The API supports reading all NVS datatypes except for blobs. One call to the API can read multiple NVS entries at once. Values can be read from different namespaces within the same NVS partition. The array of input-output structures serves as placeholders for the data read from NVS, with a fixed size of up to 8 bytes.

To read string entries, the API requires the caller to provide a buffer and its size, due to the heap memory allocation restriction in the bootloader.

Application Example
-------------------

You can find code examples in the :example:`storage` directory of ESP-IDF examples:

:example:`storage/nvs_bootloader`

This section demonstrates how to prepare data in the input-output structure for various data types, namespaces, and keys. It includes an example of reading string data from NVS.

The example also shows how to check if a read operation was successful, or if there were inconsistencies in the data or if certain values were not found in NVS. The example prints the values (or error codes) returned by the API to the console.

API Reference
-------------

.. include-build-file:: inc/nvs_bootloader.inc
