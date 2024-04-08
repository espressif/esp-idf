| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Bootloader override

(See the README.md file in the upper level for more information about bootloader examples.)

The purpose of this example is to show how to override the second-stage bootloader from a regular project.

**NOTE**: Functions called during the loading stage of the bootloader are expected to be placed in the iram_loader_seg to avoid being overwritten during loading. If you are overriding functions that are called during this stage then special care needs to be taken to avoid issues, e.g. by providing your linker script which places the required functions in the correct sections.

## How to use this example

Simply compile it:
```
idf.py build
```

And flash it with the following commands:
```
idf.py flash
```

This custom bootloader does not do more than the older bootloader, it only prints an extra message on startup:
```
[boot] Custom bootloader message defined in the KConfig file.
```

## Organization of this example

This project contains an application, in the `main` directory that represents a user program.
It also contains a `bootloader_components` directory that, as its name states, contains a component that will override the current bootloader implementation.

Below is a short explanation of the files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c                 User application
├── bootloader_components
│   └── main
│       ├── component.mk
│       ├── CMakeLists.txt
│       ├── ld/
│       │   └── ...
│       └── bootloader_start.c Implementation of the second stage bootloader
└── README.md                  This is the file you are currently reading
```

As stated in the `README.md` file in the upper level, when the bootloader components are named `main`, it overrides
the whole second stage bootloader code.

## Conditionally override the bootloader

In case you only want to override the bootloader under a certain condition (target-dependent, KConfig option, etc...), it is possible to let the bootloader project know that the `bootloader_components/main` component shall be ignored.

For example, if the custom bootloader shall not be compiled for ESP32-C3 targets, which should use the default ESP-IDF one, the `CMakeLists.txt` file in this current example must look like this:
```
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

if(${IDF_TARGET} STREQUAL "esp32c3")
    set(BOOTLOADER_IGNORE_EXTRA_COMPONENT "main")
endif()

project(main)
```

It is important to note that this can also be used for any other bootloader components than `main`. In all cases, the prefix `bootloader_component` must not be specified.
