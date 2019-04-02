#! /bin/bash

{ coverage debug sys \
    && coverage erase &> output \
    && coverage run -a $IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -d -p cpu0.svdat cpu1.svdat test.elf &>> output \
    && diff output expected_output \
    && coverage report \
; } || { echo 'The test for sysviewtrace_proc has failed. Please examine the artifacts.' ; exit 1; }
