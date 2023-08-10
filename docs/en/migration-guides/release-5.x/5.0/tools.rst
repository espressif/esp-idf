Tools
=====

:link_to_translation:`zh_CN:[中文]`

ESP-IDF Monitor
---------------

ESP-IDF Monitor makes the following changes regarding baud-rate:

- ESP-IDF monitor now uses the custom console baud-rate (:ref:`CONFIG_ESP_CONSOLE_UART_BAUDRATE`) by default instead of 115200.
- Setting a custom baud from menuconfig is no longer supported.
- A custom baud-rate can be specified from command line with the ``idf.py monitor -b <baud>`` command or through setting environment variables.
- Please note that the baud-rate argument has been renamed from ``-B`` to ``-b`` in order to be consistent with the global baud-rate ``idf.py -b <baud>``. Run ``idf.py monitor --help`` for more information.

Deprecated Commands
-------------------

``idf.py`` sub-commands and ``cmake`` target names have been unified to use hyphens (``-``) instead of underscores (``_``). Using a deprecated sub-command or target name will produce a warning. Users are advised to migrate to using the new sub-commands and target names. The following changes have been made:

.. list-table:: Deprecated Sub-command and Target Names
   :widths: 50 50
   :header-rows: 1

   * - Old Name
     - New Name
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

Esptool
-------

The ``CONFIG_ESPTOOLPY_FLASHSIZE_DETECT`` option has been renamed to :ref:`CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE` and has been disabled by default. New and existing projects migrated to ESP-IDF v5.0 have to set :ref:`CONFIG_ESPTOOLPY_FLASHSIZE`. If this is not possible due to an unknown flash size at build time, then :ref:`CONFIG_ESPTOOLPY_HEADER_FLASHSIZE_UPDATE` can be enabled. However, once enabled, to keep the digest valid, an SHA256 digest is no longer appended to the image when updating the binary header with the flash size during flashing.

Windows Environment
--------------------

The Msys/Mingw-based Windows environment support got deprecated in ESP-IDF v4.0 and was entirely removed in v5.0. Please use :ref:`get-started-windows-tools-installer` to set up a compatible environment. The options include Windows Command Line, Power Shell and the graphical user interface based on Eclipse IDE. In addition, a VS Code-based environment can be set up with the supported plugin: https://github.com/espressif/vscode-esp-idf-extension.
