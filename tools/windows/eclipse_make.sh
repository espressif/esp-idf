#!/bin/bash
echo "eclipse_make.sh has been replaced with eclipse_make.py. Check the Windows Eclipse docs for the new command."
echo "This shell script will continue to work until the next major release."
python ${IDF_PATH}/tools/windows/eclipse_make.py $@
