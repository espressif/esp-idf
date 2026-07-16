| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Bootloader extra component

(See the README.md file in the upper level for more information about bootloader examples.)

The purpose of this example is to show how to add bootloader components that are not placed in the conventional `bootloader_components` directory.

The bootloader always uses the `bootloader_components` folder name as the project-local search path for bootloader components. Use this folder for bootloader-specific code that belongs to the project, such as hooks or overrides.

Use the `BOOTLOADER_EXTRA_COMPONENT_DIRS` property when bootloader components already live somewhere else, for example in a shared components directory or in a component reused by several projects. Each path can point either to a directory containing multiple components or directly to a single component. If the extra location contains components that are not needed in a particular bootloader build, list them by component name in `BOOTLOADER_IGNORE_EXTRA_COMPONENT`.

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

This project contains a `main` directory that represents an application. It also has a `bootloader_components` directory with `my_boot_hooks`, a bootloader-specific hook component that belongs to this project.

The `extra_bootloader_components/` directory demonstrates an extra path that contains several components. `extra_component1` is required by `my_boot_hooks`, while `extra_component2` is included directly from `BOOTLOADER_EXTRA_COMPONENT_DIRS` without being required by another component. `extra_component3` is present in the same directory but is excluded from this bootloader build with `BOOTLOADER_IGNORE_EXTRA_COMPONENT`.

The `extra_component4/` and `extra_component5/` directories demonstrate extra paths that point directly to individual components. `extra_component4` is included directly, while `extra_component5` is excluded with `BOOTLOADER_IGNORE_EXTRA_COMPONENT`.

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
│   ├── extra_component1
│   │   ├── CMakeLists.txt
│   │   └── extra_component1.c                 Implementation of the extra component
│   ├── extra_component2
│   │   ├── CMakeLists.txt
│   │   └── extra_component2.c                 Implementation of the 2nd extra component
│   └── extra_component3
│       ├── CMakeLists.txt
│       └── extra_component3.c                 Extra component excluded from this bootloader build
├── extra_component4
│   ├── CMakeLists.txt
│   └── extra_component4.c                     Extra component included directly
├── extra_component5
│   ├── CMakeLists.txt
│   └── extra_component5.c                     Extra component excluded directly
└── README.md                  This is the file you are currently reading
```
