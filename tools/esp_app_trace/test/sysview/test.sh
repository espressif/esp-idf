#! /bin/bash

{ coverage debug sys \
    && coverage erase &> output \
    && coverage run -a $IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -d -p -b test.elf cpu0.svdat cpu1.svdat &>> output \
    && diff output expected_output \
    && coverage report \
; } || { echo 'The test for sysviewtrace_proc has failed. Please examine the artifacts.' ; exit 1; }

{ coverage debug sys \
    && coverage erase &> output.json \
    && coverage run -a $IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -j -b test.elf cpu0.svdat cpu1.svdat &>> output.json \
    && diff output.json expected_output.json \
    && coverage report \
; } || { echo 'The test for sysviewtrace_proc JSON functionality has failed. Please examine the artifacts.' ; exit 1; }

