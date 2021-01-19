.. include:: ../../../../components/nvs_flash/README.rst

NVS Partition Generator Utility
-------------------------------

This utility helps generate NVS partition binary files which can be flashed separately on a dedicated partition via a flashing utility. Key-value pairs to be flashed onto the partition can be provided via a CSV file. For more details, please refer to :doc:`NVS Partition Generator Utility <nvs_partition_gen>`.

Application Example
-------------------

You can find two code examples in the :example:`storage` directory of ESP-IDF examples:

:example:`storage/nvs_rw_value`

  Demonstrates how to read a single integer value from, and write it to NVS.

  The value checked in this example holds the number of the {IDF_TARGET_NAME} module restarts. The value's function as a counter is only possible due to its storing in NVS.

  The example also shows how to check if a read / write operation was successful, or if a certain value has not been initialized in NVS. The diagnostic procedure is provided in plain text to help you track the program flow and capture any issues on the way.

:example:`storage/nvs_rw_blob`

  Demonstrates how to read a single integer value and a blob (binary large object), and write them to NVS to preserve this value between {IDF_TARGET_NAME} module restarts.

    * value - tracks the number of the {IDF_TARGET_NAME} module soft and hard restarts.
    * blob - contains a table with module run times. The table is read from NVS to dynamically allocated RAM. A new run time is added to the table on each manually triggered soft restart, and then the added run time is written to NVS. Triggering is done by pulling down GPIO0.

  The example also shows how to implement the diagnostic procedure to check if the read / write operation was successful.

:example:`storage/nvs_rw_value_cxx`

  This example does exactly the same as :example:`storage/nvs_rw_value`, except that it uses the C++ nvs handle class.


API Reference
-------------

.. include-build-file:: inc/nvs_flash.inc

.. include-build-file:: inc/nvs.inc


