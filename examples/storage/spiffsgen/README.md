# SPIFFS Image Generation on Build Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the SPIFFS image generation tool [spiffsgen.py](../../../components/spiffs/spiffsgen.py) to automatically create a SPIFFS
filesystem image from the contents of a host folder during build, with an option of
automatically flashing the created image on invocation of `idf.py -p PORT flash`.
For more information, see description of `spiffsgen.py` on the ESP-IDF Programming Guide under API Reference > Storage > SPIFFS Filesystem.

The following gives an overview of the example:

1. There is a directory `spiffs_image` from which the SPIFFS filesystem image will be created.

2. The function `spiffs_create_partition_image` is used to specify that a SPIFFS image
should be created during build for the `storage` partition. For CMake, it is called from [the main component's CMakeLists.txt](./main/CMakeLists.txt);
for Make, from the [project Makefile](./Makefile). `FLASH_IN_PROJECT` specifies that the created image
should be flashed on invocation of `idf.py -p PORT flash` together with app, bootloader, partition table, etc.
For both build systems, the image is created on the example's build directory with the output filename `storage.bin`.

3. Upon invocation of `idf.py -p PORT flash monitor`, application loads and
finds there is already a valid SPIFFS filesystem in the `storage` partition with files same as those in `spiffs_image` directory. The application is then
able to read those files.

## How to use example

### Build and flash

To run the example, type the following command:

```Makefile
# Make
make flash monitor
```
or 
```CMake
# CMake
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

Here is the example's console output:

```
...
I (10) example: Initializing SPIFFS
I (110) example: Partition size: total: 896321, used: 171935
I (110) example: Reading hello.txt
I (110) example: Read from hello.txt: Hello World!
I (110) example: Computing alice.txt MD5 hash
I (330) example: Computed MD5 hash of alice.txt: deeb71f585cbb3ae5f7976d5127faf2a
I (330) example: SPIFFS unmounted
```

The logic of the example is contained in a [single source file](./main/spiffsgen_example_main.c), and it should be relatively simple to match points in its execution with the log outputs above.
