#! /bin/bash

{ coverage debug sys \
    && coverage erase &> output \
    && coverage run -a $IDF_PATH/tools/esp_app_trace/logtrace_proc.py adc_log.trc test.elf &>> output \
    && diff output expected_output \
    && coverage report \
; } || { echo 'The test for logtrace_proc has failed. Please examine the artifacts.' ; exit 1; }
