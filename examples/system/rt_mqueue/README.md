| Supported Targets | ESP32 | ESP32-C3 | Linux |
| ----------------- | ----- | -------- | ----- |

# POSIX Message Queue Example

A simple example using a POSIX message queue. Two tasks are reading from and writing to a POSIX message queue.

## How to use example

### Hardware Required

This example should be able to run on any supported Espressif SoC development board.

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.


## Example Output

If you see the following infinite console output, your example should be running correctly:

```
sending: 0
received: 0
sending: 1
received: 1
sending: 2
received: 2
sending: 3
received: 3
...
```
