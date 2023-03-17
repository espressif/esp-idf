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
    esp32c2|esp32c3|esp32c6)
        PREFIX=riscv32-esp-elf-
        ;;
    *)
        echo "Invalid IDF_TARGET value: \"${IDF_TARGET}\""
        exit 1
        ;;
esac
LIB_DIR=${IDF_TARGET}

ELF_FILE=test.elf

${PREFIX}ld --unresolved-symbols=ignore-all --entry 0 -o ${ELF_FILE} \
    -u g_esp_wifi_md5 \
    -u g_esp_wifi_he_md5 \
    -u g_wifi_crypto_funcs_md5 \
    -u g_wifi_type_md5 \
    -u g_wifi_he_type_md5 \
    -u g_wifi_osi_funcs_md5 \
    -u g_wifi_supplicant_funcs_md5 \
    ${IDF_PATH}/components/esp_wifi/lib/${LIB_DIR}/*.a

FAILURES=0

function check_md5()
{
    FILENAME=$1
    SYMBOL=$2

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
check_md5 ${IDF_PATH}/components/esp_wifi/include/esp_wifi.h g_esp_wifi_md5
check_md5 ${IDF_PATH}/components/esp_wifi/include/esp_wifi_he.h g_esp_wifi_he_md5
check_md5 ${IDF_PATH}/components/esp_wifi/include/esp_private/wifi_os_adapter.h g_wifi_osi_funcs_md5
check_md5 ${IDF_PATH}/components/esp_wifi/include/esp_wifi_crypto_types.h g_wifi_crypto_funcs_md5
check_md5 ${IDF_PATH}/components/esp_wifi/include/esp_wifi_types.h g_wifi_type_md5
check_md5 ${IDF_PATH}/components/esp_wifi/include/esp_wifi_he_types.h g_wifi_he_type_md5
check_md5 ${IDF_PATH}/components/wpa_supplicant/esp_supplicant/src/esp_wifi_driver.h g_wifi_supplicant_funcs_md5

if [ $FAILURES -gt 0 ]; then
    exit 1
fi
