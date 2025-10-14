| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Bootloader extra component

(See the README.md file in the upper level for more information about bootloader examples.)

The purpose of this example is to show how to add a custom directory that contains a component to the bootloader build.

Registering extra components for the bootloader can be done thanks to the IDF property `BOOTLOADER_EXTRA_COMPONENT_DIRS`. It can either refer to a directory that contains several components, either refer to a single component.

## Usage of this example:

Simply compile it:
```
idf.py build
```

Then flash it and open the monitor with the following command:
```
idf.py flash monitor
```

If everything went well, the bootloader should output the following message:
```
I (60) EXTRA: This function is called from an extra component
```

And finally the application will start and show the message:
```
User application is loaded and running.
```

## Organization of this example

This project contains a `main` directory that represents an application. It also has a `bootloader_components` directory that contains a component that will be compiled and linked with the bootloader. This `bootloader_components` can contain several components, each of them would be in a different directory.

The directory `extra_bootloader_components/extra_component/` contains a component that is meant to be included in the bootloader build. To do so, the CMake property `BOOTLOADER_EXTRA_COMPONENT_DIRS` is set from the `CMakeLists.txt` file.

Below is a short explanation of files in the project folder.

```
├── CMakeLists.txt             Defines the `BOOTLOADER_EXTRA_COMPONENT_DIRS` property
├── main
│   ├── CMakeLists.txt
│   └── main.c                 User application
├── bootloader_components
│   └── my_boot_hooks
│       ├── CMakeLists.txt
│       └── hooks.c            Implementation of the hooks to execute on boot
├── extra_bootloader_components
│   └── extra_component
│       ├── CMakeLists.txt
│       └── extra_component.c  Implementation of the extra component
└── README.md                  This is the file you are currently reading
```
