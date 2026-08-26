| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Using ESP-IDF in Custom CMake Projects

This example illustrates using ESP-IDF components as libraries in custom CMake projects. The application
in this example can run on either host or on an ESP32, and the appropriate libraries are linked
to the executable depending on which target is specified. If the target is an ESP32, the libraries
created from ESP-IDF components are linked. On the other hand, stub libraries are linked if example
is meant to be run on the host to simulate the same application behavior.

The application in this example is equivalent to the `hello_world` example under `examples/get-started/hello_world`.

## Example Flow

Users looking at this example should focus on the [top-level CMakeLists.txt file](./CMakeLists.txt). This builds an
application that can run on the target without relying on the typical ESP-IDF application template.

### Output

```
Hello world!
This is ESP32 chip with 2 CPU cores, WiFi/BT/BLE, silicon revision 0, 4MB external flash
Restarting in 10 seconds...
Restarting in 9 seconds...
Restarting in 8 seconds...
Restarting in 7 seconds...
Restarting in 6 seconds...
Restarting in 5 seconds...
Restarting in 4 seconds...
Restarting in 3 seconds...
Restarting in 2 seconds...
Restarting in 1 seconds...
Restarting in 0 seconds...
```

## Building this Example

To build this example, the user can either run [build-esp32.sh](./build-esp32.sh) to build for the ESP32
or run [build.sh](./build.sh) to build for the host:

```bash
# Builds the example for ESP32
./build-esp32.sh
```

Note: To build for a different target SoC, copy the `build-esp32.sh` file and change the `-DTARGET=esp32` clause on the second line.

or

```bash
# Builds the example to run on host
./build.sh
```

## Flashing and Running this Example

To flash and run the example, users can run either  [run-esp32.sh](./run-esp32.sh) or [run.sh](./run.sh) depending
on what the example was built for. In the case of ``run-esp32.sh``, the port needs to be specified:

```bash
# Run the example on device connected to /dev/ttyUSB1
./run-esp32.sh /dev/ttyUSB1
```

or

```bash
# Run the example on the host
./run.sh
```

## Configuring this Example

To modify the example ESP-IDF project configuration, first create the CMake build directory. This can be done by running `build-esp32.sh` or by running only the first two lines in `build-esp32.sh` (which won't build the actual project yet).

Then execute the menuconfig build target in the build directory:

```bash
cmake --build build -- menuconfig
```

If using ninja directly:

```bash
ninja -C build menuconfig
```

Note: ESP-IDF project configuration isn't used by the host CMake builds, the config is only read when the project is built using the ESP-IDF build system.

---

There is a discussion on using ESP-IDF in custom CMake projects in the programming guide under `API Guides` -> `Build System` -> `Using ESP-IDF in Custom CMake Projects`
