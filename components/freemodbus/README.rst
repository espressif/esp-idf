# Modbus assumptions, dependencies and constraints

1. Current implementation of Modbus has limitation: maximum frame size should be less than 120 bytes (default UART receive FIFO full threshold value).
This limitation is removed in ESP-IDF v4.2.

2. The only one instance of Modbus port can be initialized at the same time.
