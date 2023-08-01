#!/usr/bin/env bash

set -euo pipefail

if [ -z "${IDF_PATH:-}" ]; then
    echo "IDF_PATH must be set before running this script"
    exit 1
fi

if [ -z "${IDF_TARGET:-}" ]; then
    echo "IDF_TARGET must be set before running this script"
    exit 1
fi

case $IDF_TARGET in
    esp32)
        PREFIX=xtensa-esp32-elf-
        ;;
    esp32s2)
        PREFIX=xtensa-esp32s2-elf-
        ;;
    esp32s3)
        PREFIX=xtensa-esp32s3-elf-
        ;;
    esp32c2|esp32c3|esp32c6|esp32h2)
        PREFIX=riscv32-esp-elf-
        ;;
    *)
        echo "Invalid IDF_TARGET value: \"${IDF_TARGET}\""
        exit 1
        ;;
esac
LIB_DIR=${IDF_TARGET}

ELF_FILE=test.elf

FAILURES=0

function check_md5()
{
    FILENAME=$1
    SYMBOL=$2

    ${PREFIX}ld --unresolved-symbols=ignore-all --entry 0 -o ${ELF_FILE} \
        -u ${SYMBOL} \
    ${IDF_PATH}/components/esp_coex/lib/${LIB_DIR}/*.a
    GDB_COMMAND="printf \"%s\\n\", (const char*) ${SYMBOL}"
    MD5_FROM_LIB=$(${PREFIX}gdb -n -batch ${ELF_FILE} -ex "${GDB_COMMAND}")
    MD5_FROM_HEADER=$(md5sum ${FILENAME} | cut -c 1-7)

    echo "Checking ${FILENAME}:"
    echo "  ${MD5_FROM_HEADER} - from header file"
    echo "  ${MD5_FROM_LIB} - from library"
    if [ "${MD5_FROM_LIB}" != "${MD5_FROM_HEADER}" ]; then
        echo "  error: MD5 mismatch!"
        FAILURES=$(($FAILURES+1))
    fi
}

echo "Checking libraries for target ${IDF_TARGET}..."
check_md5 ${IDF_PATH}/components/esp_coex/include/private/esp_coexist_adapter.h g_coex_adapter_funcs_md5

case $IDF_TARGET in
    esp32c6|esp32h2)
    check_md5 ${IDF_PATH}/components/esp_coex/include/esp_coex_i154.h g_coex_i154_funcs_md5
    ;;
esac

if [ $FAILURES -gt 0 ]; then
    exit 1
fi
