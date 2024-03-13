#!/usr/bin/env bash
#
# Usage: build_template_app.sh [-p]
# -p: optional, if specified, do a fast build for every chip target with cmake and only the default
#     config. Otherwise permutations of (chip target, build system, configs) which were not built in
#     the fast build will be built.
#
# Needs to be called under IDF root folder

set -euo pipefail

TEMPLATE_APP_PATH="esp-idf-template"

gen_configs() {
    # CONFIG_COMPILER_OPTIMIZATION_NONE with flag -O0
    echo "CONFIG_COMPILER_OPTIMIZATION_NONE=y" > ${TEMPLATE_APP_PATH}/sdkconfig.ci.O0

    # CONFIG_COMPILER_OPTIMIZATION_SIZE with flag -Os
    echo "CONFIG_COMPILER_OPTIMIZATION_SIZE=y" > ${TEMPLATE_APP_PATH}/sdkconfig.ci.Os
    echo "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_SIZE=y" >> ${TEMPLATE_APP_PATH}/sdkconfig.ci.Os

    # CONFIG_COMPILER_OPTIMIZATION_PERF with flag -O2
    echo "CONFIG_COMPILER_OPTIMIZATION_PERF=y" > ${TEMPLATE_APP_PATH}/sdkconfig.ci.O2
    echo "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_PERF=y" >> ${TEMPLATE_APP_PATH}/sdkconfig.ci.O2
    # -O2 makes the bootloader too large to fit in the default space, otherwise(!)
    echo "CONFIG_PARTITION_TABLE_OFFSET=0x10000" >> ${TEMPLATE_APP_PATH}/sdkconfig.ci.O2

    # This part will be built in earlier stage (pre_build job) with only cmake. Built with make in later stage
    # CONFIG_COMPILER_OPTIMIZATION_DEBUG with flag -Og
    echo "CONFIG_COMPILER_OPTIMIZATION_DEBUG=y" > ${TEMPLATE_APP_PATH}/sdkconfig.ci2.Og
    echo "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_DEBUG=y" >> ${TEMPLATE_APP_PATH}/sdkconfig.ci2.Og
    # -Og makes the bootloader too large to fit in the default space, otherwise(!)
    echo "CONFIG_PARTITION_TABLE_OFFSET=0x10000" >> ${TEMPLATE_APP_PATH}/sdkconfig.ci2.Og

    # Needs to be built with specific extra flags
    # Same as O2, but also disable assertions.
    cp ${TEMPLATE_APP_PATH}/sdkconfig.ci.O2 ${TEMPLATE_APP_PATH}/sdkconfig.ci3.no_assert
    echo "CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE=y" >> ${TEMPLATE_APP_PATH}/sdkconfig.ci3.no_assert
}

get_config_str() {
    CONFIG_STR=
    until [ $# -eq 0 ]
    do
        CONFIG_STR+=" --config $1"
        shift
    done
    echo ${CONFIG_STR}
}

build_stage2() {
    CONFIG_STR=$(get_config_str sdkconfig.ci.*=)

    # Override EXTRA_CFLAGS and EXTRA_CXXFLAGS in the environment
    export EXTRA_CFLAGS=${PEDANTIC_CFLAGS/-Werror=unused-variable -Werror=unused-but-set-variable -Werror=unused-function/}
    export EXTRA_CXXFLAGS=${PEDANTIC_CXXFLAGS/-Werror=unused-variable -Werror=unused-but-set-variable -Werror=unused-function/}
    python -m idf_build_apps build -vv \
        -p ${TEMPLATE_APP_PATH} \
        -t all \
        ${CONFIG_STR} \
        --work-dir ${BUILD_PATH}/cmake \
        --build-dir ${BUILD_DIR} \
        --build-log ${BUILD_LOG_CMAKE} \
        --size-file size.json \
        --keep-going \
        --collect-size-info size_info.txt \
        --default-build-targets esp32 esp32s2 esp32s3 esp32c2 esp32c3 esp32c5 esp32c6 esp32h2 esp32p4 esp32c61
}

build_stage1() {
    CONFIG_STR=$(get_config_str sdkconfig.ci2.*=)
    python -m idf_build_apps build -vv \
        -p ${TEMPLATE_APP_PATH} \
        -t all \
        ${CONFIG_STR} \
        --work-dir ${BUILD_PATH}/cmake \
        --build-dir ${BUILD_DIR} \
        --build-log ${BUILD_LOG_CMAKE} \
        --size-file size.json \
        --collect-size-info size_info.txt \
        --default-build-targets esp32 esp32s2 esp32s3 esp32c2 esp32c3 esp32c5 esp32c6 esp32h2 esp32p4 esp32c61
}

# TODO: IDF-9197 remove the additional test for esp32c5 beta3
build_c5beta3() {
    # Update the config to select C5 beta3
    C5BETA3_CFG="CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION=y\nCONFIG_IDF_TARGET_ESP32C5_MP_VERSION=n"
    if [ $1 = 1 ]
    then
        echo "${C5BETA3_CFG}" >> ${TEMPLATE_APP_PATH}/sdkconfig.ci2.Og
        CONFIG_STR=$(get_config_str sdkconfig.ci2.*=)
        KEEP_GOING=""
    else
        echo "${C5BETA3_CFG}" >> ${TEMPLATE_APP_PATH}/sdkconfig.ci.O0
        echo "${C5BETA3_CFG}" >> ${TEMPLATE_APP_PATH}/sdkconfig.ci.Os
        echo "${C5BETA3_CFG}" >> ${TEMPLATE_APP_PATH}/sdkconfig.ci.O2
        CONFIG_STR=$(get_config_str sdkconfig.ci.*=)
        KEEP_GOING=" --keep-going"
    fi
    python -m idf_build_apps build -vv \
           -p ${TEMPLATE_APP_PATH} \
           -t esp32c5 \
           ${CONFIG_STR} \
           --work-dir ${BUILD_PATH}/cmake \
           --build-dir ${BUILD_DIR} \
           --build-log ${BUILD_LOG_CMAKE} \
           --size-file size.json \
           ${KEEP_GOING} \
           --collect-size-info size_info.txt \
           --default-build-targets esp32c5
}

# Default arguments
# STAGE:
# 1 (-p): fast build, 2 (default): regular build
STAGE=2

# Parse arguments: -p for STAGE
for arg in $*
do
    if [ ${arg} = "-p" ]; then
        STAGE=1
    fi
done

mkdir -p ${BUILD_PATH}/cmake
mkdir -p ${LOG_PATH}
rm -f scan.json

gen_configs

if [ ${STAGE} = 1 ]
then
    build_stage1
else
    build_stage2
fi

# TODO: IDF-9197 remove the additional test for esp32c5 beta3
echo "Build ESP32-C5 beta3 additionally"
build_c5beta3 ${STAGE}
