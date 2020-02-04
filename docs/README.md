# Documentation Source Folder

This folder contains source files of **ESP-IDF documentation** available in [English](https://docs.espressif.com/projects/esp-idf/en/latest/) and [中文](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/).

The sources do not render well in GitHub and some information is not visible at all.

Use actual documentation generated within about 20 minutes on each commit:

# Hosted Documentation

* English: https://docs.espressif.com/projects/esp-idf/en/latest/
* 中文: https://docs.espressif.com/projects/esp-idf/zh_CN/latest/

The above URLs are all for the master branch latest version. Click the drop-down in the bottom left to choose a stable version or to download a PDF.


# Building Documentation

* Documentation build requres Python 3 and will not work with Python 2
* Install dependencies for ESP-IDF as per the Getting Started guide
* Install documentation Python depdendencies, ie `pip install -r $IDF_PATH/docs/requirements.txt`
* Run `./build_docs.py build` to build docs for all supported Language & Target combinations, or `./build_docs.py -t esp32 -l en build` to build docs for a single supported language & target combination only.

See [Documenting Code](https://docs.espressif.com/projects/esp-idf/en/latest/contribute/documenting-code.rst) for more information.

Available languages are `en` and `zh_CN`, targets are any target supported by ESP-IDF - for example `esp32` or `esp32s2`.

