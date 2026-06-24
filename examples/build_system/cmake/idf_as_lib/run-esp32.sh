#!/usr/bin/env bash
cd build
python -m esptool -p $1 write-flash @flash_project_args
python -m esp_idf_monitor -p $1 idf_as_lib.elf
