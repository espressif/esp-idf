#!/usr/bin/env bash
cd build
python $IDF_PATH/components/esptool_py/esptool/esptool.py -p $1 write_flash @flash_project_args
python -m esp_idf_monitor -p $1 idf_as_lib.elf
