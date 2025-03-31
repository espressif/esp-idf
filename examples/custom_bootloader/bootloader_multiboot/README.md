| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Bootloader override

(See the README.md file in the upper level for more information about bootloader examples.)

The purpose of this example is to show how to override the second-stage bootloader from a regular project and make it so that it can load any IDF-based project.

This example will compile two other examples, `hello_world` and `console`, and embed them in the flash, in their own partition. The goal is to give the user the possibility to choose which image to boot. As such, this example will also compile these two other examples.

A custom partition table is required to manage all the images, please check `partitions.csv`.

## How to use this example

Simply compile it:
```
idf.py build
```

And flash it with the following commands:
```
idf.py flash
```

This custom bootloader will clear the monitor and show all the bootable partitions found:
```
Please select a partition to boot.

FOUND 3 BOOT PARTITIONS:
     default
     hello_world
     console
```

Note that the names shown in the menu are the names of the projects (set when compiled) and not the partitions names.

It is possible to choose the image to boot using the arrow keys and the `enter` to validate.

## Manage the bootable images

The bootable images are described in the `partitions.csv` file. A `factory` partition must be present at all time since it represents the default image to boot.

Any additional image must be stored in its own partition which, of course, must be big enough to store the whole bootable image, be of type `app` and have `ota_n`, where `n` is between 0 and 15 included, as a subtype.

## Organization of this example

This project contains an application, in the `main` directory that represents a user program that does nothing more than printing a message on the standard output.
It also contains a `bootloader_components` directory that, as its name states, contains a component that will override the current bootloader implementation.

Below is a short explanation of the files in the project folder.

```
├── CMakeLists.txt
│   partitions.csv                              Custom partition table, containing all the bootable images
├── main
│   ├── CMakeLists.txt
│   └── bootloader_multiboot_example_main.c     Example bootable application (always present)
├── bootloader_components
│   └── main
│       ├── CMakeLists.txt
│       └── bootloader_start.c                  Implementation of the second stage bootloader
└── README.md                                   This is the file you are currently reading
```

As stated in the `README.md` file in the upper level, when the bootloader components are named `main`, it overrides the whole second stage bootloader code.

Please note that this example also relies on the `hello_world` and `console` examples, before building, make sure these two examples have a clean environment and a valid `sdkconfig`.

