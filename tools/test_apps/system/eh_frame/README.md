| Supported Targets | ESP32-C2 | ESP32-C3 |
| ----------------- | -------- | -------- |

# Building and running

The main goal of this test is to check whether the compiler/linker generates non-empty sections `.eh_frame` and `.eh_frame_hdr`.

Thus, as soon as this example compiles we can considered it passed. However, it will also print the addresses of both sections on the UART.

In order to build and run the example, use the following commands:

```
idf.py set-target <esp32c3/esp32c2>
idf.py build
idf.py flash monitor
```
