| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Synchronous I/O multiplexing example

The example demonstrates the use of synchronous I/O multiplexing by the select()
function with UART and socket file descriptors. The example starts three tasks:
1. The first task writes periodically to the UART1 file descriptor.
2. The second task writes periodically to the socket descriptor.
3. Both UART1 and the socket are configured to act as loopbacks. The third
   task detects by the use of select() whether it is possible to read from
   UART1 or the socket, and receives the sent messages from the other tasks.

See the README.md file in the upper level 'examples' directory for more information about examples.
