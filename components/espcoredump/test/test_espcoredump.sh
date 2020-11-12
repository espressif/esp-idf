#!/usr/bin/env bash

{ coverage debug sys \
    && coverage erase \
    && coverage run -a --source=espcoredump ../espcoredump.py --gdb-timeout-sec 5 info_corefile -m -t b64 -c coredump.b64 -s core.elf test.elf &> output \
    && diff expected_output output \
    && coverage run -a --source=espcoredump ../espcoredump.py --gdb-timeout-sec 5 info_corefile -m -t elf -c core.elf test.elf &> output2 \
    && diff expected_output output2 \
    && coverage run -a --source=espcoredump ./test_espcoredump.py \
    && coverage report \
; } || { echo 'The test for espcoredump has failed!'; exit 1; }
