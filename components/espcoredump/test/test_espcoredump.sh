#!/usr/bin/env bash

{ coverage debug sys \
    && coverage erase \
    && coverage run -a --source=corefile ../espcoredump.py --chip esp32 --gdb-timeout-sec 5 info_corefile -m -t b64 -c coredump.b64 -s core.elf test.elf &> output \
    && diff expected_output output \
    && coverage run -a --source=corefile ../espcoredump.py --chip esp32 --gdb-timeout-sec 5 info_corefile -m -t elf -c core.elf test.elf &> output2 \
    && diff expected_output output2 \
    && coverage run -a --source=corefile ./test_espcoredump.py \
    && coverage report ../corefile/elf.py ../corefile/gdb.py ../corefile/loader.py ../corefile/xtensa.py ../espcoredump.py \
; } || { echo 'The test for espcoredump has failed!'; exit 1; }
