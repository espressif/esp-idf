# Examples

This directory contains a growing number of simple example projects for esp-idf. These are intended to show basic esp-idf functionality, and to provide you can use for your own projects.

# Using Examples

Building examples is the same as building any other project:

* Follow the setup instructions in the top-level esp-idf README.

* Set `IDF_PATH` environment variable to point to the path to the esp-idf top-level directory.
* Change into the directory of the example you'd like to build.
* `make menuconfig` to configure the example. Most examples require a simple WiFi SSID & password via this configuration.
* `make` to build the example.
* Follow the printed instructions to flash, or run `make flash`.

# Copying Examples

Each example is a standalone project. The examples *do not have to be inside the esp-idf directory*. You can copy an example directory to anywhere on your computer in order to make a copy that you can modify and work with.

The `IDF_PATH` environment variable is the only thing that connects the example to the rest of the `esp-idf` system.

If you're looking for a more bare-bones project to start from, try [esp-idf-template](https://github.com/espressif/esp-idf-template).

# Contributing Examples

If you have a new example you think we'd like, please consider sending it to us as a Pull Request.

Please read the esp-idf CONTRIBUTING.rst file which lays out general contribution rules.

In addition, here are some tips for creating good examples:

* A good example is documented and the basic options can be configured.
* A good example does not contain a lot of code. If there is a lot of generic code in the example, consider refactoring that code into a standalone component and then use the component's API in your example.
* Names (of files, functions, variables, etc.) inside examples should be distinguishable from names of other parts of IDF (ideally, use `example` in names.)
* Functions and variables used inside examples should be declared static where possible.
* Examples should demonstrate one distinct thing each. Avoid multi-purposed "demo" examples, split these into multiple examples instead.
* Examples must be licensed under the Apache License 2.0 or (preferably for examples) if possible you can declare the example to be Public Domain / Creative Commons Zero.
