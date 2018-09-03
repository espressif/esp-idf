#! /bin/bash

{ coverage debug sys \
    && coverage erase \
    && coverage run -a --source=espcoredump ../espcoredump.py info_corefile -m -t b64 -c coredump.b64 test.elf &> output \
    && diff expected_output output \
    && coverage run -a --source=espcoredump ./test_espcoredump.py \
    && coverage report \
; } || { echo 'The test for espcoredump has failed!'; exit 1; }

