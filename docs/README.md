# Documentation Source Folder

This folder contains source files of **ESP-IDF documentation** available in [English](https://docs.espressif.com/projects/esp-idf/en/latest/) and [中文](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/).

The sources do not render well in GitHub and some information is not visible at all.

Use actual documentation generated within about 20 minutes on each commit:

# Hosted Documentation

* English: https://docs.espressif.com/projects/esp-idf/en/latest/
* 中文: https://docs.espressif.com/projects/esp-idf/zh_CN/latest/

The above URLs are all for the master branch latest version. Click the drop-down in the bottom left to choose a stable version or to download a PDF.


# Building Documentation

* Install `make` and `doxygen` for your platform (`make` may already be installed as an ESP-IDF prerequisite).
* Change to either the docs/en or docs/zh_CN subdirectory and run `make html`
* `make` will probably prompt you to run a python pip install step to get some other Python-related prerequisites. Run the command as shown, then re-run `make html` to build the docs.

## For MSYS2 MINGW32 on Windows

If using Windows and the MSYS2 MINGW32 terminal, run this command before running "make html" the first time:

```
pacman -S doxygen mingw-w64-i686-python2-pillow
```

Note: Currently it is not possible to build docs on Windows without using a Unix-on-Windows layer such as MSYS2 MINGW32.
