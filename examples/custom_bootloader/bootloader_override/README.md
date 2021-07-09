# Bootloader override

(See the README.md file in the upper level for more information about bootloader examples.)

The purpose of this example is to show how to override the second stage bootloader from a regular project.

**NOTE**: Overriding the bootloader is not supported with `Makefile` build system, it is only available with `CMake`.

## How to use example

Simply compile it:
```
idf.py build
```

And flash it with the following commands:
```
idf.py flash
```

This custom bootloader does not do more than the older bootloader, it only prints an extra message on start up:
```
[boot] Custom bootloader has been initialized correctly.
```

## Organisation of this example

This project contains an application, in the `main` directory that represents a user program.
It also contains a `bootloader_components` directory that, as it name states, contains a component that will override the current bootloader implementation.

Below is a short explanation of files in the project folder.

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

As stated in the `README.md` file in the upper level, when the bootloader components is named `main`, it overrides
the whole second stage bootloader code.
