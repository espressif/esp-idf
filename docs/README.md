# Documentation Source Folder

This folder contains source files of **ESP-IDF documentation** available in [English](https://docs.espressif.com/projects/esp-idf/en/latest/) and [中文](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/).

The sources do not render well in GitHub and some information is not visible at all.

Use actual documentation generated within about 20 minutes on each commit:

# Hosted Documentation

* English: https://docs.espressif.com/projects/esp-idf/en/latest/
* 中文: https://docs.espressif.com/projects/esp-idf/zh_CN/latest/

After you click any link to [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/index.html), go to the top of the sidebar, then make sure you have the correct Espressif chip (target) and ESP-IDF version selected in the dropdown menus. You can also find a link on the bottom right to download the PDF version.

# Building Documentation

The documentation is built using the python package `esp-docs`, which can be installed by running:

```
pip install esp-docs
```

For a summary of available options, run:

```
build-docs --help
```

For more information see the `esp-docs` documentation at https://github.com/espressif/esp-docs/blob/master/README.md
