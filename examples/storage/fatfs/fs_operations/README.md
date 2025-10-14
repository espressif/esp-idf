| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# FATFS Filesystem Operations Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates some of the POSIX functions available for working with the FATFS filesystem.
Including basic read and write operations, as well as creating moving, and deleting files and directories.

## Overview

1. Partition labeled `storage` is mounted (and formatted if necessary) as FATFS filesystem to `/spiflash` mountpoint.

2. All existing files and directories in the root directory are deleted.

3. File `hello.txt` is created and written to.

4. File `hello.txt` is inspected using `fstat` function showing file size and last modification time.

5. File `hello.txt` is written to again, appending to the end of the file.

6. File `hello.txt` is read from and the contents are printed to the console.

7. New directory `new_dir` is created.

8. All files and directories in the root directory are listed.

9. File `hello.txt` is moved and renamed to `new_dir/hello_renamed.txt`.

## How to use example

### Build and flash

To run the example, type the following command:

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
I (323) example: Mounting FAT filesystem
I (333) example: Deleting everything in /spiflash:
I (333) example: Deleting everything in /spiflash/new_dir:
I (673) example: Creating a file
I (683) example: Writing to the file
I (733) example: File stats:
	File size:                13 bytes
	File modification time:   Thu Jan  1 00:00:00 1970

I (743) example: Wait for 1 seconds
I (1743) example: Write more to the file
I (1743) example: File stats:
	File size:                26 bytes
	File modification time:   Thu Jan  1 00:00:00 1970

I (1743) example: Go to the beginning of the file
I (1753) example: Reading from file:
Hello World!
Hello World!

I (1753) example: Closing file
I (1993) example: Listing files in /spiflash:
/spiflash:
    file     : hello.txt
I (1993) example: Creating a new directory
I (2383) example: Listing files in /spiflash:
/spiflash:
    file     : hello.txt
    directory: new_dir
I (2383) example: Rename a file
I (2503) example: Listing files in /spiflash:
/spiflash:
    directory: new_dir
I (2503) example: Listing files in /spiflash/new_dir:
/spiflash/new_dir:
    file     : hello_renamed.txt
I (2513) example: Unmounting FAT filesystem
I (2643) example: Done
...
```

The logic of the example is contained in a [single source file](./main/fatfs_fs_operations_example_main.c),
and it should be relatively simple to match points in its execution with the log outputs above.
