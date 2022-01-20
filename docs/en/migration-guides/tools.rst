Migrate Tools to ESP-IDF 5.0
============================

IDF Monitor
-----------

IDF Monitor follows the custom console baud-rate (:ref:`CONFIG_ESP_CONSOLE_UART_BAUDRATE`) by default instead of 115200. That means if a custom UART baud-rate is set then one doesn't have to change the monitor baud-rate (:ref:`CONFIG_ESPTOOLPY_MONITOR_BAUD`) to match it.
