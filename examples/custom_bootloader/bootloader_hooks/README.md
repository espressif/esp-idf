| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Bootloader hooks

(See the README.md file in the upper level for more information about bootloader examples.)

The purpose of this example is to show how to add hooks to the 2nd stage bootloader.

## Usage of this example:

Simply compile it:
```
idf.py build
```

Then flash it and open the monitor with the following command:
```
idf.py flash monitor
```

If everything went well, the bootloader output should be as followed:
```
I (24) HOOK: This hook is called BEFORE bootloader initialization
I (37) boot: [...]
[...]
I (60) HOOK: This hook is called AFTER bootloader initialization
```

And finally the application will start and show the message:
```
User application is loaded and running.
```

## Organisation of this example

This project contains an application, in the `main` directory that represents a user program.
It also contains a `bootloader_components` that, as it name states, contains a component compiled and linked with the bootloader.

Below is a short explanation of files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c                 User application
├── bootloader_components
│   └── my_boot_hooks
│       ├── CMakeLists.txt   
│       └── hooks.c            Implementation of the hooks to execute on boot
└── README.md                  This is the file you are currently reading
```
Bootloader hooks are **not** supported in legacy `make` build system. They are only supported with `CMake` build system.

## Note about including weak symbols

The components in ESP-IDF are compiled as static libraries. Moreover, the bootloaders' hooks are declared as `weak`. Thus, when
defining hooks for the bootloader, we **must** tell the compiler to always include our library (`my_boot_hooks`) in the link process.
To achieve this, we need to define an extra symbol: `bootloader_hooks_include`. In our case, this symbol is a function defined in
`bootloader_components/my_boot_hooks/hooks.c`. This will make the linker include all the symbols contained in that file.
