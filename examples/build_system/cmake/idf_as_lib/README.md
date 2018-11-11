# Using ESP-IDF in Custom CMake Projects

This example illustrates using ESP-IDF components as libraries in custom CMake projects. This builds
an equivalent application to the `hello_world` example under `examples/get-started/hello_world`.

## Example Flow

Users looking at this example should focus on the [top-level CMakeLists.txt file](./CMakeLists.txt). This builds an
application that can run on targets without relying on the typical ESP-IDF application template. The application itself
follows a similar code flow to the aforementioned `hello_world` example.

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

To build this example, run the following commands from this directory:

```bash
# Create a build directory, and change location to that directory.
mkdir build; cd build
# Invoke CMake, specifying the top-level CMakeLists.txt directory and toolchain file to use. This will generate
# the build system files.
cmake .. -DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-esp32.cmake -DIDF_TARGET=esp32
# Build using the generated build system files.
cmake --build .
```

Or, execute `build.sh` script, which contains the same commands mentioned above.

## Flashing and Running this Example

To flash this example, we will have to invoke `esptool.py` and `idf_monitor.py` manually. While still in the build directory:

### Flashing to target

```bash
# Write project binaries to flash.
esptool.py --port /dev/ttyUSB0 write_flash @flash_project_args
```

### Running on target

```bash
# Monitor the output of the flashed firmware.
idf_monitor.py --port /dev/ttyUSB0 idf_as_lib.elf
```

Of course, you should replace the specified ports in the commands specified above to the proper one where your device
is connected.

---

There is a discussion on using ESP-IDF in custom CMake projects in the programming guide under `API Guides` -> `Build System (CMake)` -> `Using ESP-IDF in Custom CMake Projects`
