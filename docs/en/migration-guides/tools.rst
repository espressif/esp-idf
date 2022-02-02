Migrate Tools to ESP-IDF 5.0
============================

IDF Monitor
-----------

IDF Monitor follows the custom console baud-rate (:ref:`CONFIG_ESP_CONSOLE_UART_BAUDRATE`) by default instead of 115200. Setting a custom baud rate is not supported from menuconfig anymore. A custom baud-rate can be specified from command line with the ``idf.py monitor -B <baud>`` command or through setting environment variables. Run ``idf.py monitor --help`` for more information.
