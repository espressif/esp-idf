#!/usr/bin/env bash

{ python -m coverage debug sys \
    && python -m coverage erase &> output \
    && python -m coverage run -a $IDF_PATH/tools/esp_app_trace/logtrace_proc.py adc_log.trc test.elf &>> output \
    && diff output expected_output \
    && python -m coverage report \
; } || { echo 'The test for logtrace_proc has failed. Please examine the artifacts.' ; exit 1; }
