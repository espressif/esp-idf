Migrate Tools to ESP-IDF 5.0
============================

IDF Monitor
-----------

IDF Monitor follows the custom console baud-rate (:ref:`CONFIG_ESP_CONSOLE_UART_BAUDRATE`) by default instead of 115200. Setting a custom baud rate is not supported from menuconfig anymore. A custom baud-rate can be specified from command line with the ``idf.py monitor -b <baud>`` command or through setting environment variables. Please note that the baud-rate argument has been renamed from ``-B`` to ``-b`` in order to be consistent with the global baud-rate ``idf.py -b <baud>``. Run ``idf.py monitor --help`` for more information.

Deprecated commands
-------------------

``idf.py`` sub-commands and ``cmake`` targets are unified to contain ``-`` instead of ``_``. The following changes have been made. Deprecated sub-commands and targets produce a warning. It is advised to migrate to the new ones.

.. list-table:: Target and sub-command deprecation
   :widths: 50 50
   :header-rows: 1

   * - Old name
     - New name
   * - efuse_common_table
     - efuse-common-table
   * - efuse_custom_table
     - efuse-custom-table
   * - erase_flash
     - erase-flash
   * - partition_table
     - partition-table
   * - partition_table-flash
     - partition-table-flash
   * - post_debug
     - post-debug
   * - show_efuse_table
     - show-efuse-table
   * - erase_otadata
     - erase-otadata
   * - read_otadata
     - read-otadata
