#!/bin/bash

cd ${IDF_PATH}/tools/unit-test-app

AVAL_CONFIGS=""
CONFIGS=$(ls configs)
if [ $1 == "esp32" ]; then
    #echo 'Searching for configs for target "'$1'"'
    for FILE in $CONFIGS
    do
        grep 'CONFIG_IDF_TARGET' <configs/$FILE > /dev/null
        if [ $? -ne 0 ]; then
            # If CONFIG_IDF_TARGET not found, implies ESP32
            AVAL_CONFIGS="$AVAL_CONFIGS $FILE"
        fi
        grep -E '^CONFIG_IDF_TARGET="?'$1'"?$' <configs/$FILE > /dev/null
        if [ $? -eq 0 ]; then
            AVAL_CONFIGS="$AVAL_CONFIGS $FILE"
        fi
    done
else
    #echo 'Searching for configs for target "'$1'"'
    for FILE in $CONFIGS
    do
        grep -E '^CONFIG_IDF_TARGET="?'$1'"?$' <configs/$FILE > /dev/null
        if [ $? -eq 0 ]; then
            AVAL_CONFIGS="$AVAL_CONFIGS $FILE"
        fi
    done
fi

echo $AVAL_CONFIGS
