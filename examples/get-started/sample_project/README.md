# _Sample project_

(For general overview of examples and their usage, see the `README.md` file in the upper level 'examples' directory.)

> [!NOTE]
> After you click any link to [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/index.html), go to the top of the sidebar, then make sure you have the appropriate **Espressif chip** (target) and **ESP-IDF version** selected in the dropdown menus.

This is the example of a simplest buildable project. It is also used by the command `idf.py create-project` which copies these files to the path specified by the user and sets the project name.

This sample projects contains:

```sh
├── CMakeLists.txt  # Build configuration declaring entire project
├── main
│   ├── CMakeLists.txt  # File that registers the main component
│   └── main.c          # Source file for the main component
└── README.md           # File you are currently browsing
```

If you want to develop a project for the legacy build system based on Make that requires `Makefile` and `component.mk` files, see [esp-idf-template](https://github.com/espressif/esp-idf-template).


## Usage

For brief instructions on how to configure, build, and flash the project, see [Examples README](https://github.com/espressif/esp-idf/blob/master/examples/README.md#using-examples) > Using Examples.


## Further Development

For further steps on how to develop the project, see the following:

- Managing the project:
  - [IDF Frontend](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#start-a-new-project) document
  - ESP-IDF Getting Started video ([YouTube](https://youtu.be/J8zc8mMNKtc?t=340), [bilibili](https://www.bilibili.com/video/BV1114y1r7du/?t=336))
  - [Overview](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html#example-project) of an example project
  - [Build System](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/build-system.html) document
- Writing code:
  - Find appropriate bits of code in [application examples](https://github.com/espressif/esp-idf/tree/master/examples)
  - Write your own code following the [API references](https://docs.espressif.com/projects/esp-idf/en/stable/api-reference/index.html)


## Documentation

If you want to contribute this project as an ESP-IDF application example, please write this README based on the [example README template](https://github.com/espressif/esp-idf/blob/master/docs/TEMPLATE_EXAMPLE_README.md).
