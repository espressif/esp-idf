#! /bin/bash

if [ "$(xtensa-esp32-elf-gcc -dumpversion)" = "5.2.0" ]; then
    EXPECTED_OUTPUT="expected_output"
else
    # GCC_NOT_5_2_0 just a hint to remove later
    EXPECTED_OUTPUT="expected_output_new_CT"
fi

{ coverage debug sys \
    && coverage erase \
    && coverage run -a --source=espcoredump ../espcoredump.py info_corefile -m -t b64 -c coredump.b64 test.elf &> output \
    && diff ${EXPECTED_OUTPUT} output \
    && coverage run -a --source=espcoredump ./test_espcoredump.py \
    && coverage report \
; } || { echo 'The test for espcoredump has failed!'; exit 1; }
