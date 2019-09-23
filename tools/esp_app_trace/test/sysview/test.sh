#! /bin/bash

if [ "$(xtensa-esp32-elf-gcc -dumpversion)" = "5.2.0" ]; then
    EXPECTED_OUTPUT="expected_output"
else
    # GCC_NOT_5_2_0 just a hint to remove later
    EXPECTED_OUTPUT="expected_output_new_CT"
fi

{ coverage debug sys \
    && coverage erase &> output \
    && coverage run -a $IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -d -p cpu0.svdat cpu1.svdat test.elf &>> output \
    && diff output ${EXPECTED_OUTPUT} \
    && coverage report \
; } || { echo 'The test for sysviewtrace_proc has failed. Please examine the artifacts.' ; exit 1; }
