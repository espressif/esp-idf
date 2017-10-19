UART asynchronous example, that uses separate RX and TX tasks
=============================================================

Starts two FreeRTOS tasks:
  - One task for transmitting 'Hello world' via the UART.
  - One task for receiving from the UART.

If you'd like to see your ESP32 receive something, simply short
TXD_PIN and RXD_PIN. By doing this data transmitted on TXD_PIN will
be received on RXD_PIN. See the definitions of TXD_PIN and RXD_PIN
in ./main/uart_async_rxtxtasks_main.c.

The output for such configuration will look as follows: 

```
I (3261) TX_TASK: Wrote 11 bytes
I (4261) RX_TASK: Read 11 bytes: 'Hello world'
I (4261) RX_TASK: 0x3ffb821c   48 65 6c 6c 6f 20 77 6f  72 6c 64                 |Hello world|
...
```
The third line above prints received data in hex format, that comes handy to display non printable data bytes.