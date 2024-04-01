| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# NVS Partition Image Generation on Build Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the NVS image generation tool [nvs_partition_gen.py](../../../components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py) to automatically create a NVS partition image from the contents of a CSV file during build, with an option of automatically flashing the created image on invocation of `idf.py -p PORT flash`. For more information, see description of `nvs_partition_gen.py` on the ESP-IDF Programming Guide under API Reference > Storage API > NVS Partition Generator Utility.

The following gives an overview of the example:

1. There is a file `nvs_data.csv` from which the NVS partition image will be created.

2. The function `nvs_create_partition_image` is used to specify that a NVS image should be created during build for the `nvs` partition. It is called from [the main component's CMakeLists.txt](./main/CMakeLists.txt). `FLASH_IN_PROJECT` specifies that the created image should be flashed on invocation of `idf.py -p PORT flash` together with app, bootloader, partition table, etc. For both build systems, the image is created on the example's build directory with the output filename `nvs.bin`.

3. Upon invocation of `idf.py -p PORT flash monitor`, application loads and finds there is already a valid and pre-filled NVS partition in the `nvs` partition with values same as those in `nvs_data.csv` file. The application is then able to read those values.

## How to use example

### Build and flash

To run the example, type the following command:

```CMake
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

Here is the example's console output:

```
...
I (357) example: Opening Non-Volatile Storage (NVS) handle
I (357) example: Done
I (357) example: Reading values from NVS
255
-128
65535
4294967295
-2147483648
Lorem ipsum dolor sit amet, consectetur adipiscing elit.
Fusce quis risus justo.
Suspendisse egestas in nisi sit amet auctor.
Pellentesque rhoncus dictum sodales.
In justo erat, viverra at interdum eget, interdum vel dui.
I (387) example: Reading values from NVS done - all OK
```

The logic of the example is contained in a [single source file](./main/nvsgen_example_main.c), and it should be relatively simple to match points in its execution with the log outputs above.
