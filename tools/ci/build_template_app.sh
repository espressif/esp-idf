#!/usr/bin/env bash
#
# Usage: build_template_app.sh [-p]
# -p: optional, if specified, do a fast build for every chip target with cmake and only the default
#     config. Otherwise permutations of (chip target, build system, configs) which were not built in
#     the fast build will be built.
#
# Needs to be called under IDF root folder
#
# This script will call find_apps.py with the following arguments:
# - CMake build arguments: --work-dir {BUILD_PATH}/cmake --build-dir ${BUILD_DIR} --build-log ${BUILD_LOG_CMAKE}
# - Make build arguments: --work-dir {BUILD_PATH}/make/${BUILD_DIR} --build-dir build --build-log ${BUILD_LOG_MAKE}

set -euo pipefail

gen_configs() {
    # CONFIG_COMPILER_OPTIMIZATION_NONE with flag -O0
    echo "CONFIG_COMPILER_OPTIMIZATION_NONE=y" > esp-idf-template/sdkconfig.ci.O0
    echo "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_NONE=y" >> esp-idf-template/sdkconfig.ci.O0
    # -O0 makes the bootloader too large to fit in the default space, otherwise(!)
    echo "CONFIG_PARTITION_TABLE_OFFSET=0x10000" >> esp-idf-template/sdkconfig.ci.O0

    # CONFIG_COMPILER_OPTIMIZATION_SIZE with flag -Os
    echo "CONFIG_COMPILER_OPTIMIZATION_SIZE=y" > esp-idf-template/sdkconfig.ci.Os
    echo "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_SIZE=y" >> esp-idf-template/sdkconfig.ci.Os

    # CONFIG_COMPILER_OPTIMIZATION_PERF with flag -O2
    echo "CONFIG_COMPILER_OPTIMIZATION_PERF=y" > esp-idf-template/sdkconfig.ci.O2
    echo "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_PERF=y" >> esp-idf-template/sdkconfig.ci.O2

    # This part will be built in earlier stage (pre_build job) with only cmake. Built with make in later stage
    # CONFIG_COMPILER_OPTIMIZATION_DEFAULT with flag -Og
    echo "CONFIG_COMPILER_OPTIMIZATION_DEFAULT=y" > esp-idf-template/sdkconfig.ci2.Og
    echo "CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_DEBUG=y" >> esp-idf-template/sdkconfig.ci2.Og

    # Needs to be built with specific extra flags
    # Same as O2, but also disable assertions.
    cp esp-idf-template/sdkconfig.ci.O2 esp-idf-template/sdkconfig.ci3.no_assert
    echo "CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE=y" >> esp-idf-template/sdkconfig.ci3.no_assert
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

search_cmake() {
    TARGET=$1
    shift
    CONFIG_STR=$*
    tools/find_apps.py -vv --format json --work-dir ${BUILD_PATH}/cmake --build-dir ${BUILD_DIR} --build-log ${BUILD_LOG_CMAKE} -p esp-idf-template --build-system cmake ${CONFIG_STR} --target ${TARGET} --output scan_temp.json
    cat scan_temp.json >> scan.json
    rm scan_temp.json
}

search_make() {
    TARGET=$1
    shift
    CONFIG_STR=$*
    tools/find_apps.py -vv --format json --work-dir ${BUILD_PATH}/make/${BUILD_DIR} --build-dir build --build-log ${BUILD_LOG_MAKE} -p esp-idf-template --build-system make ${CONFIG_STR} --target ${TARGET} --output scan_temp.json
    cat scan_temp.json >> scan.json
    rm scan_temp.json
}

build() {
    tools/build_apps.py -vv --format json --keep-going --parallel-count 1 --parallel-index 1 --size-info ${SIZE_INFO_LOCATION} scan.json
    rm scan.json
}

build_stage2() {
    CONFIG_STR=$(get_config_str sdkconfig.ci.*=)
    search_cmake esp32 ${CONFIG_STR}
    search_cmake esp32s2 ${CONFIG_STR}
    search_cmake esp32s3 ${CONFIG_STR}
    search_cmake esp32c3 ${CONFIG_STR}
    search_cmake esp32h2 ${CONFIG_STR}

    CONFIG_STR=$(get_config_str sdkconfig.ci.*= sdkconfig.ci2.*=)
    search_make esp32 ${CONFIG_STR}

    build build_list_1.json

    CONFIG_STR=$(get_config_str sdkconfig.ci3.*=)
    search_make esp32 ${CONFIG_STR}
    search_cmake esp32 ${CONFIG_STR}
    search_cmake esp32s2 ${CONFIG_STR}
    search_cmake esp32s3 ${CONFIG_STR}
    search_cmake esp32c3 ${CONFIG_STR}
    search_cmake esp32h2 ${CONFIG_STR}

    # Override EXTRA_CFLAGS and EXTRA_CXXFLAGS in the environment
    export EXTRA_CFLAGS=${PEDANTIC_CFLAGS/-Werror=unused-variable -Werror=unused-but-set-variable -Werror=unused-function/}
    export EXTRA_CXXFLAGS=${PEDANTIC_CXXFLAGS/-Werror=unused-variable -Werror=unused-but-set-variable -Werror=unused-function/}
    build
}

build_stage1() {
    CONFIG_STR=$(get_config_str sdkconfig.ci2.*=)
    search_cmake esp32 ${CONFIG_STR}
    search_cmake esp32s2 ${CONFIG_STR}
    search_cmake esp32s3 ${CONFIG_STR}
    search_cmake esp32c3 ${CONFIG_STR}
    search_cmake esp32h2 ${CONFIG_STR}

    build
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

mkdir -p ${BUILD_PATH}/make
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
