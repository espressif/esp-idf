NVS Partition Parser Utility
============================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

The utility :component_file:`nvs_flash/nvs_partition_tool/nvs_tool.py` loads and parses an NVS storage partition for easier debugging and data extraction. The utility also features integrity check which scans the partition for potential errors. Data blobs are encoded in `base64` format.

Encrypted Partitions
--------------------

This utility does not support decryption. To decrypt the NVS partition, please use the :doc:`NVS Partition Generator Utility </api-reference/storage/nvs_partition_gen>` which does support NVS partition encryption and decryption.

Usage
-----

There are two output format styles available with the `-f` or `--format` option:
    - `json` - All of the output is printed as a JSON.
    - `text` - The output is printed as a human-readable text with different selectable output styles mentioned below.

For the `text` output format, the utility provides six different output styles with the `-d` or `--dump` option:
    - `all` (default) - Prints all entries with metadata.
    - `written` - Prints only written entries with metadata.
    - `minimal` - Prints written `namespace:key = value` pairs.
    - `namespaces` - Prints all written namespaces
    - `blobs` - Prints all blobs and strings (reconstructs them if they are chunked).
    - `storage_info` - Prints entry states count for every page.

.. note:: There is also a `none` option which will not print anything. This can be used with the integrity check option if the NVS partition contents are irrelevant.

The utility also provides an integrity check feature via the `-i` or `--integrity-check` option (available only with the `text` format as it would invalidate the `json` output). This feature scans through the entire partition and prints potential errors. It can be used with the `-d none` option which will print only the potential errors.
