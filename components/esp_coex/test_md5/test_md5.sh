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
    esp32c2|esp32c3|esp32c6|esp32h2|esp32c5|esp32c61)
        PREFIX=riscv32-esp-elf-
        ;;
    *)
        echo "Invalid IDF_TARGET value: \"${IDF_TARGET}\""
        exit 1
        ;;
esac
LIB_DIR=${IDF_TARGET}

COEX_ELF_FILE=coex.elf
TEST_ELF_FILE=test.elf

FAILURES=0

function check_md5()
{
    if [ "$1" != "$2" ]; then
        echo "  error: MD5 mismatch!"
        FAILURES=$(($FAILURES+1))
    fi
}

function check_md5_file_lib()
{
    FILENAME=$1
    SYMBOL=$2

    ${PREFIX}ld --unresolved-symbols=ignore-all --entry 0 -o ${COEX_ELF_FILE} \
        -u ${SYMBOL} \
    ${IDF_PATH}/components/esp_coex/lib/${LIB_DIR}/*.a
    GDB_COMMAND="printf \"%s\\n\", (const char*) ${SYMBOL}"
    MD5_FROM_LIB=$(${PREFIX}gdb -n -batch ${COEX_ELF_FILE} -ex "${GDB_COMMAND}")
    MD5_FROM_HEADER=$(md5sum ${FILENAME} | cut -c 1-7)
    echo "Checking ${FILENAME}:"
    echo "  ${MD5_FROM_HEADER} - from header file"
    echo "  ${MD5_FROM_LIB} - from library"
    check_md5 ${MD5_FROM_HEADER} ${MD5_FROM_LIB}
}

function check_md5_libs()
{
    COEX_SYMBOL=$1
    TEST_SYMBOL=$2
    TEST_PATH=$3

    ${PREFIX}ld --unresolved-symbols=ignore-all --entry 0 -o ${COEX_ELF_FILE} \
        -u ${COEX_SYMBOL} \
    ${IDF_PATH}/components/esp_coex/lib/${LIB_DIR}/*.a

    ${PREFIX}ld --unresolved-symbols=ignore-all --entry 0 -o ${TEST_ELF_FILE} \
        -u ${TEST_SYMBOL} \
    ${TEST_PATH}/*.a

    COEX_GDB_COMMAND="printf \"%s\\n\", (const char*) ${COEX_SYMBOL}"
    TEST_GDB_COMMAND="printf \"%s\\n\", (const char*) ${TEST_SYMBOL}"
    COEX_MD5_FROM_LIB=$(${PREFIX}gdb -n -batch ${COEX_ELF_FILE} -ex "${COEX_GDB_COMMAND}")
    TEST_MD5_FROM_LIB=$(${PREFIX}gdb -n -batch ${TEST_ELF_FILE} -ex "${TEST_GDB_COMMAND}")

    echo "Checking ${TEST_PATH}/${TEST_SYMBOL} "
    echo "  ${COEX_MD5_FROM_LIB} - from coexist library"
    echo "  ${TEST_MD5_FROM_LIB} - from test library"
    check_md5 ${COEX_MD5_FROM_LIB} ${TEST_MD5_FROM_LIB}
}

echo "Checking libraries for target ${IDF_TARGET}..."
check_md5_file_lib ${IDF_PATH}/components/esp_coex/include/private/esp_coexist_adapter.h g_coex_adapter_funcs_md5

case $IDF_TARGET in
    esp32c6|esp32h2|esp32c5)
    check_md5_file_lib ${IDF_PATH}/components/esp_coex/include/esp_coex_i154.h g_coex_i154_funcs_md5
    ;;
esac

if [[ ! "$IDF_TARGET" =~ ^(esp32h2)$ ]]; then
    check_md5_libs g_coex_basic_md5 g_wifi_coex_basic_md5 ${IDF_PATH}/components/esp_wifi/lib/${LIB_DIR}
fi

if [ $FAILURES -gt 0 ]; then
    exit 1
fi
