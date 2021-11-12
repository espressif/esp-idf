#!/usr/bin/env bash

{ coverage debug sys \
    && coverage erase &> output \
    && echo -e "\n***\nRunning idf_size.py..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py on bootloader..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py bootloader.map &>> output \
    && echo -e "\n***\nRunning idf_size.py --archives..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --archives app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py --files..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --files app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py --archive_details..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --archive_details libdriver.a app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff with bootloader..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --diff bootloader.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff with itself..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --diff app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff with another app..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --diff app2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff with app in reverse order..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app2.map --diff app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --archives with bootloader..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --archives --diff bootloader.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --archives with itself..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --archives --diff app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --archives with another app..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --archives --diff app2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --archives with app in reverse order..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app2.map --archives --diff app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --files with bootloader..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --files --diff bootloader.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --files with itself..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --files --diff app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --files with another app..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --files --diff app2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --files with app in reverse order..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app2.map --files --diff app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --archive_details with bootloader..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --archive_details libdriver.a --diff bootloader.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --archive_details with bootloader..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --archive_details libc.a --diff bootloader.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --archive_details with itself..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --archive_details libdriver.a --diff app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --archive_details with another app..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --archive_details libdriver.a --diff app2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --archive_details with app in reverse order..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app2.map --archive_details libdriver.a --diff app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff --archive_details with another app..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --archive_details libfreertos.a --diff app2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py for esp32s2..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --target esp32s2 app_esp32s2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py for esp32s2 (target autodetected)..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app_esp32s2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py on bootloader for esp32s2..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --target esp32s2 bootloader_esp32s2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py on bootloader for esp32s2 (target autodetected)..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py bootloader_esp32s2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py --archives for esp32s2..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --target esp32s2 --archives app_esp32s2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py --files for esp32s2..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --target esp32s2 --files app_esp32s2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py --archive_details for esp32s2..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --target esp32s2 --archive_details libdriver.a app_esp32s2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff with another app (different target)..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map --diff app_esp32s2.map &>> output \
    && echo -e "\n***\nRunning idf_size.py diff with another app (wrong target)..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --target esp32s2 app.map --diff app2.map &>> output \
    && echo -e "\n***\nProducing JSON output..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json app.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json --archives app.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json --files app.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json --archive_details libdriver.a app.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json app.map --diff app2.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json --archives app.map --diff app2.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json --files app.map --diff app2.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json --archive_details libdriver.a app.map --diff app2.map &>> output \
    && echo -e "\n***\nProducing JSON file output..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json --output-file output.json app.map &>> output \
    && echo -e "\n***\nProducing text file output..." &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py -o output.txt app.map &>> output \
    && echo -e "\n***\nRunning idf_size_tests.py..." &>> output \
    && coverage run -a $IDF_PATH/tools/test_idf_size/test_idf_size.py &>> output \
    && echo -e "\n\nComparing expected output..." \
    && diff -Z output expected_output \
    && echo -e "\n\nComparing expected json output..." \
    && diff -Z output.json expected_output.json \
    && echo -e "\n\nComparing expected text output..." \
    && diff -Z output.txt expected_output.txt \
    && coverage report \
; } || { echo 'The test for idf_size has failed. Please examine the artifacts.' ; exit 1; }

# Note: "diff -Z is used because some versions of Python print trailing whitespace for JSON pretty-printing, and some don't
