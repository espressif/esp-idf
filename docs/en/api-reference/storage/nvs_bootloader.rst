NVS Bootloader
==============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

This guide provides an overview of the NVS (Non-Volatile Storage) functionality available for custom bootloader code, along with its limitations.

Due to the constraints of the bootloader runtime environment, custom bootloader code cannot use the full NVS API directly. Instead, NVS provides a simplified API that offers read-only access to NVS data.

The API supports reading all NVS datatypes except for blobs. One call to the API can read multiple NVS entries at once. Values can be read from different namespaces within the same NVS partition. The array of input-output structures serves as placeholders for the data read from NVS, with a fixed size of up to 8 bytes.

To read string entries, the API requires the caller to provide a buffer and its size, due to the heap memory allocation restriction in the bootloader.

Reading encrypted NVS partitions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The API also supports decrypting NVS data, if the NVS partition is encrypted using one of the schemes as mentioned in the :doc:`nvs_encryption` guide.

Applications are expected to follow the steps below in order to enable decryption of a NVS partition using the :cpp:func:`nvs_bootloader_read` API:

    1. Populate the NVS security configuration structure :cpp:type:`nvs_sec_cfg_t` according to the selected NVS encryption scheme (Please refer to :doc:`nvs_encryption` for more details).
    2. Read NVS security configuration set by the specified security scheme using the :cpp:func:`nvs_bootloader_read_security_cfg` API.
    3. Initialise the NVS flash partition with the above read security configuration using the :cpp:func:`nvs_bootloader_secure_init` API.
    4. Perform NVS read operations using the :cpp:func:`nvs_bootloader_read` API.
    5. Deinitialise and clear the security configuration of the NVS flash partition using the :cpp:func:`nvs_bootloader_secure_deinit` API.

.. only:: SOC_HMAC_SUPPORTED

    .. note::

        While using the HMAC-based scheme, the above workflow can be used without enabling any of the config options for NVS encryption - :ref:`CONFIG_NVS_ENCRYPTION`, :ref:`CONFIG_NVS_SEC_KEY_PROTECTION_SCHEME` -> ``CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC`` and :ref:`CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID` to encrypt the default as well as custom NVS partitions with :cpp:func:`nvs_flash_secure_init` API.


Application Example
-------------------

You can find code examples in :example:`storage/nvs_bootloader` (in the :example:`storage` directory of ESP-IDF examples).

This section demonstrates how to prepare data in the input-output structure for various data types, namespaces, and keys. It includes an example of reading string data from NVS.

The example also shows how to check if a read operation was successful, or if there were inconsistencies in the data or if certain values were not found in NVS. The example prints the values (or error codes) returned by the API to the console.

API Reference
-------------

.. include-build-file:: inc/nvs_bootloader.inc
