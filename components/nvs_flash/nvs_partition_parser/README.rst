NVS Partition Parser Utility
============================

Introduction
------------

The utility :component_file:`nvs_flash/nvs_partition_parser/nvs_read.py` loads and parses an NVS storage partition for easier debugging / data extraction.  
The utility also has an integrity check feature which scans the partition for potential errors.

Encrypted partitions
--------------------

This utility doesn't support decryption. To decrypt the NVS partition use the :doc:`NVS Partition Generator Utility </api-reference/storage/nvs_partition_gen>` which does support NVS partition encryption/decryption.

Usage
-----

The utility provides 6 different output styles with `-d` or `--dump` option:
    - `all` (default) - Prints all entries with metadata 
    - `written` - Prints only written entries with metadata
    - `minimal` - Prints written `namespace:key = value` pairs
    - `namespaces` - Prints all written namespaces
    - `blobs` - Prints all blobs & strings (reconstructs them if they are chunked)
    - `storage_info` - Prints entry states count for every page

.. note:: There is also `none` option which will not print anything. This can be used with the integrity check option if the contents of NVS partition are not relevant.

The utility also provides an integrity check feature via `-i` or `--integrity-check` option. This option scans through the entire partition and prints potential errors. This option can be used with '-d none' option which will print only the potential errors.