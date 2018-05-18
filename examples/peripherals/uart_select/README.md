# UART Select Example

The UART select example is for demonstrating the use of `select()` for
synchronous I/O multiplexing on the UART interface. The example waits for a
character from UART using `select()` until a blocking read without delay or a
successful non-blocking read is possible.

Please note that the same result can be achieved by using `uart_read_bytes()`
but the use of `select()` allows to use it together with other virtual
file system (VFS) drivers, e.g. LWIP sockets. For a more comprehensive example
please refer to `system/select`.

See the README.md file in the upper level 'examples' directory for more information about examples.
