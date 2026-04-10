* [中文版](./README_CN.md)

# Documentation Source Folder

This folder contains source files of **ESP-IDF documentation** available in [English](https://docs.espressif.com/projects/esp-idf/en/latest/) and [Chinese](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/).

The sources do not render well in GitHub and some information is not visible without building the documentation.

Use the actual documentation, which is generated within about 20 minutes of each commit:

# Hosted Documentation

* English: https://docs.espressif.com/projects/esp-idf/en/latest/
* Chinese: https://docs.espressif.com/projects/esp-idf/zh_CN/latest/

After clicking any link to [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/index.html), go to the top of the sidebar, then make sure you have the correct Espressif chip (target) and ESP-IDF version selected in the dropdown menus. You can also find a link at the bottom right to download the HTML version.

# Building Documentation

The documentation is built using the Python package `esp-docs`, which can be installed by running:

```
pip install esp-docs
```

For a summary of available options, run:

```
build-docs --help
```

For more information, see the `esp-docs` documentation at https://github.com/espressif/esp-docs/blob/master/README.md
