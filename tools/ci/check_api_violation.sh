#!/usr/bin/env bash

set -uo pipefail

# ESP-IDF should only use the ROM API that has a prefix "esp_rom_"
cd ${IDF_PATH} # git ls-files operates on working directory only, make sure we're at the top directory
deprecated_rom_apis=$(cat ${IDF_PATH}/components/esp_rom/esp32/ld/esp32.rom.api.ld | grep "esp_rom_" | cut -d "=" -f 2 | cut -d " " -f 2)
files_to_search=$(git ls-files --full-name '*.c' ':!:components/esp_rom/')
count=0
for api in $deprecated_rom_apis; do
    found_files=$(grep -E "\W+"$api"\W+" $files_to_search)
    if [ -n "$found_files" ]; then
        echo $found_files
        ((count++))
    fi
done

if [ $count -gt 0 ]; then
    echo "ROM APIs used in ESP-IDF should have an esp_rom_ prefix"
    echo "Please try to use the APIs listed in esp_rom/include/esp_rom_xxx.h"
    exit 1
fi
