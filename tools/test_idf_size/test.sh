#! /bin/bash

{ coverage debug sys \
    && coverage erase &> output \
    && echo -e "\n***\nRunning idf_size.py..." >> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py --archives..." >> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --archives app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py --files..." >> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --files app.map &>> output \
    && echo -e "\n***\nRunning idf_size.py --archive_details..." >> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --archive_details libdriver.a app.map &>> output \
    && echo -e "\n***]nProducing JSON output..." >> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json app.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json --archives app.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json --files app.map &>> output \
    && coverage run -a $IDF_PATH/tools/idf_size.py --json --archive_details libdriver.a app.map &>> output \
    && echo -e "\n***\nRunning idf_size_tests.py..." >> output \
    && coverage run -a $IDF_PATH/tools/test_idf_size/test_idf_size.py &>> output \
    && diff -Z output expected_output \
    && coverage report \
; } || { echo 'The test for idf_size has failed. Please examine the artifacts.' ; exit 1; }

# Note: "diff -Z is used because some versions of Python print trailing whitespace for JSON pretty-printing, and some don't
