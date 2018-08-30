#! /bin/bash

{ coverage debug sys \
    && coverage erase &> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --archives app.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --files app.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --archive_details libdriver.a app.map &>> output \
    && coverage run -a $IDF_PATH/tools/test_idf_size/test_idf_size.py &>> output \
    && diff output expected_output \
    && coverage report \
; } || { echo 'The test for idf_size has failed. Please examine the artifacts.' ; exit 1; }
