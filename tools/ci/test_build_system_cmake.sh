#!/bin/bash
#
# Test the build system for basic consistency (Cmake/idf.py version)
#
# A bash script that tests some likely build failure scenarios in a row
#
# Assumes PWD is an out-of-tree build directory, and will create a
# subdirectory inside it to run build tests in.
#
# Environment variables:
# IDF_PATH - must be set
# ESP_IDF_TEMPLATE_GIT - Can override git clone source for template app. Otherwise github.
# NOCLEANUP - Set to '1' if you want the script to leave its temporary directory when done, for post-mortem.
#
#
# Internals:
# * The tests run in sequence & the system keeps track of all failures to print at the end.
# * BUILD directory is set to default BUILD_DIR_BASE
# * The "print_status" function both prints a status line to the log and keeps track of which test is running.
# * Calling the "failure" function prints a failure message to the log and also adds to the list of failures to print at the end.
# * The function "assert_built" tests for a file relative to the BUILD directory.
# * The function "take_build_snapshot" can be paired with the functions "assert_rebuilt" and "assert_not_rebuilt" to compare file timestamps and verify if they were rebuilt or not since the snapshot was taken.
#
# To add a new test case, add it to the end of the run_tests function. Note that not all test cases do comprehensive cleanup
# (although very invasive ones like appending CRLFs to all files take a copy of the esp-idf tree), however the clean_build_dir
# function can be used to force-delete all files from the build output directory.

# Set up some variables
#
# override ESP_IDF_TEMPLATE_GIT to point to a local dir if you're testing and want fast iterations
[ -z ${ESP_IDF_TEMPLATE_GIT} ] && ESP_IDF_TEMPLATE_GIT=https://github.com/espressif/esp-idf-template.git

# uncomment next line to produce a lot more debug output
#export V=1

export PATH="$IDF_PATH/tools:$PATH"  # for idf.py

function run_tests()
{
    FAILURES=
    STATUS="Starting"
    print_status "Checking prerequisites"
    [ -z ${IDF_PATH} ] && echo "IDF_PATH is not set. Need path to esp-idf installation." && exit 2

    print_status "Cloning template from ${ESP_IDF_TEMPLATE_GIT}..."
    git clone ${ESP_IDF_TEMPLATE_GIT} template
    cd template
    if [ -z $CHECKOUT_REF_SCRIPT ]; then
        git checkout ${CI_BUILD_REF_NAME} || echo "Using esp-idf-template default branch..."
    else
        $CHECKOUT_REF_SCRIPT esp-idf-template
    fi

    print_status "Try to clean fresh directory..."
    idf.py fullclean || exit $?

    # all relative to the build directory
    BOOTLOADER_BINS="bootloader/bootloader.elf bootloader/bootloader.bin"
    APP_BINS="app-template.elf app-template.bin"
    PARTITION_BIN="partition_table/partition-table.bin"

    print_status "Initial clean build"
    # if build fails here, everything fails
    idf.py build || exit $?

    # check all the expected build artifacts from the clean build
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Updating component source file rebuilds component"
    # touch a file & do a build
    take_build_snapshot
    touch ${IDF_PATH}/components/esp32/cpu_start.c
    idf.py build || failure "Failed to partial build"
    assert_rebuilt ${APP_BINS} esp32/libesp32.a esp32/CMakeFiles/esp32.dir/cpu_start.c.obj
    assert_not_rebuilt lwip/liblwip.a freertos/libfreertos.a ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Bootloader source file rebuilds bootloader"
    take_build_snapshot
    touch ${IDF_PATH}/components/bootloader/subproject/main/bootloader_start.c
    idf.py build || failure "Failed to partial build bootloader"
    assert_rebuilt ${BOOTLOADER_BINS} bootloader/main/CMakeFiles/main.dir/bootloader_start.c.obj
    assert_not_rebuilt ${APP_BINS} ${PARTITION_BIN}

    print_status "Partition CSV file rebuilds partitions"
    take_build_snapshot
    touch ${IDF_PATH}/components/partition_table/partitions_singleapp.csv
    idf.py build || failure "Failed to build partition table"
    assert_rebuilt ${PARTITION_BIN}
    assert_not_rebuilt app-template.bin app-template.elf ${BOOTLOADER_BINS}

    print_status "Partial build doesn't compile anything by default"
    take_build_snapshot
    # verify no build files are refreshed by a partial make
    ALL_BUILD_FILES=$(find ${BUILD} -type f | sed "s@${BUILD}/@@" | grep -v '^.')
    idf.py build || failure "Partial build failed"
    assert_not_rebuilt ${ALL_BUILD_FILES}

    print_status "Moving BUILD_DIR_BASE out of tree"
    clean_build_dir
    OUTOFTREE_BUILD=${TESTDIR}/alt_build
    idf.py -B "${OUTOFTREE_BUILD}" build || failure "Failed to build with out-of-tree build dir"
    NEW_BUILD_FILES=$(find ${OUTOFREE_BUILD} -type f)
    if [ -z "${NEW_BUILD_FILES}" ]; then
        failure "No files found in new build directory!"
    fi
    DEFAULT_BUILD_FILES=$(find ${BUILD} -mindepth 1)
    if [ -n "${DEFAULT_BUILD_FILES}" ]; then
        failure "Some files were incorrectly put into the default build directory: ${DEFAULT_BUILD_FILES}"
    fi

    print_status "BUILD_DIR_BASE inside default build directory"
    clean_build_dir
    idf.py -B "build/subdirectory" build || failure "Failed to build with build dir as subdir"
    NEW_BUILD_FILES=$(find ${BUILD}/subdirectory -type f)
    if [ -z "${NEW_BUILD_FILES}" ]; then
        failure "No files found in new build directory!"
    fi

    print_status "Can still clean build if all text files are CRLFs"
    clean_build_dir
    find . -path .git -prune -exec unix2dos {} \; # CRLFify template dir
    # make a copy of esp-idf and CRLFify it
    CRLF_ESPIDF=${TESTDIR}/esp-idf-crlf
    mkdir -p ${CRLF_ESPIDF}
    cp -r ${IDF_PATH}/* ${CRLF_ESPIDF}
    # don't CRLFify executable files, as Linux will fail to execute them
    find ${CRLF_ESPIDF} -name .git -prune -name build -prune -type f ! -perm 755 -exec unix2dos {} \;
    IDF_PATH=${CRLF_ESPIDF} idf.py build || failure "Failed to build with CRLFs in source"
    # do the same checks we do for the clean build
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Updating rom ld file should re-link app and bootloader"
    clean_build_dir
    idf.py build
    take_build_snapshot
    sleep 1  # ninja may ignore if the timestamp delta is too low
    cp ${IDF_PATH}/components/esp32/ld/esp32.rom.ld .
    echo "/* (Build test comment) */" >> ${IDF_PATH}/components/esp32/ld/esp32.rom.ld
    tail ${IDF_PATH}/components/esp32/ld/esp32.rom.ld
    idf.py build || failure "Failed to rebuild with modified linker script"
    assert_rebuilt ${APP_BINS} ${BOOTLOADER_BINS}
    mv esp32.rom.ld ${IDF_PATH}/components/esp32/ld/

    print_status "Updating app-only ld file should only re-link app"
    take_build_snapshot
    cp ${IDF_PATH}/components/esp32/ld/esp32.common.ld .
    sleep 1  # ninja may ignore if the timestamp delta is too low
    echo "/* (Build test comment) */" >> ${IDF_PATH}/components/esp32/ld/esp32.common.ld
    idf.py build || failure "Failed to rebuild with modified linker script"
    assert_rebuilt ${APP_BINS}
    assert_not_rebuilt ${BOOTLOADER_BINS}
    mv esp32.common.ld ${IDF_PATH}/components/esp32/ld/

    print_status "sdkconfig update triggers full recompile"
    clean_build_dir
    idf.py build
    take_build_snapshot
    # need to actually change config, or cmake is too smart to rebuild
    sed -i s/^\#\ CONFIG_FREERTOS_UNICORE\ is\ not\ set/CONFIG_FREERTOS_UNICORE=y/ sdkconfig
    idf.py build
    # check the sdkconfig.h file was rebuilt
    assert_rebuilt config/sdkconfig.h
    # pick one each of .c, .cpp, .S that #includes sdkconfig.h
    # and therefore should rebuild
    assert_rebuilt newlib/CMakeFiles/newlib.dir/syscall_table.c.obj
    assert_rebuilt nvs_flash/CMakeFiles/nvs_flash.dir/src/nvs_api.cpp.obj
    assert_rebuilt freertos/CMakeFiles/freertos.dir/xtensa_vectors.S.obj

    print_status "Updating project CMakeLists.txt triggers full recompile"
    clean_build_dir
    idf.py build
    take_build_snapshot
    # Need to actually change the build config, or CMake won't do anything
    cp CMakeLists.txt CMakeLists.bak
    sed -i 's/^project(/add_compile_options("-DUSELESS_MACRO_DOES_NOTHING=1")\nproject\(/' CMakeLists.txt
    idf.py build || failure "Build failed"
    mv CMakeLists.bak CMakeLists.txt
    # similar to previous test
    assert_rebuilt newlib/CMakeFiles/newlib.dir/syscall_table.c.obj
    assert_rebuilt nvs_flash/CMakeFiles/nvs_flash.dir/src/nvs_api.cpp.obj
    assert_rebuilt freertos/CMakeFiles/freertos.dir/xtensa_vectors.S.obj

    print_status "Can build with Ninja (no idf.py)"
    clean_build_dir
    (cd build && cmake -G Ninja .. && ninja)  || failure "Ninja build failed"
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Can build with GNU Make (no idf.py)"
    clean_build_dir
    mkdir build
    (cd build && cmake -G "Unix Makefiles" .. && make) || failure "Make build failed"
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Can build with IDF_PATH set via cmake cache not environment"
    clean_build_dir
    cp CMakeLists.txt CMakeLists.bak
    sed -i 's/ENV{IDF_PATH}/{IDF_PATH}/' CMakeLists.txt
    export IDF_PATH_BACKUP="$IDF_PATH"
    (unset IDF_PATH &&
         cd build &&
         cmake -G Ninja .. -DIDF_PATH=${IDF_PATH_BACKUP} &&
         ninja) || failure "Ninja build failed"
    mv CMakeLists.bak CMakeLists.txt
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Can build with IDF_PATH unset and inferred by build system"
    clean_build_dir
    cp CMakeLists.txt CMakeLists.bak
    sed -i "s%\$ENV{IDF_PATH}%${IDF_PATH}%" CMakeLists.txt  # expand to a hardcoded path
    (unset IDF_PATH && cd build &&
         cmake -G Ninja .. && ninja) || failure "Ninja build failed"
    mv CMakeLists.bak CMakeLists.txt
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "sdkconfig should have contents both files: sdkconfig and sdkconfig.defaults"
    idf.py clean > /dev/null;
    idf.py fullclean > /dev/null;
    rm -f sdkconfig.defaults;
    rm -f sdkconfig;
    echo "CONFIG_PARTITION_TABLE_OFFSET=0x10000" >> sdkconfig.defaults;
    echo "CONFIG_PARTITION_TABLE_TWO_OTA=y" >> sdkconfig;
    idf.py reconfigure > /dev/null;
    grep "CONFIG_PARTITION_TABLE_OFFSET=0x10000" sdkconfig || failure "The define from sdkconfig.defaults should be into sdkconfig"
    grep "CONFIG_PARTITION_TABLE_TWO_OTA=y" sdkconfig || failure "The define from sdkconfig should be into sdkconfig"

    print_status "All tests completed"
    if [ -n "${FAILURES}" ]; then
        echo "Some failures were detected:"
        echo -e "${FAILURES}"
        exit 1
    else
        echo "Build tests passed."
    fi
}

function print_status()
{
    echo "******** $1"
    STATUS="$1"
}

function failure()
{
    echo "!!!!!!!!!!!!!!!!!!!"
    echo "FAILURE: $1"
    echo "!!!!!!!!!!!!!!!!!!!"
    FAILURES="${FAILURES}${STATUS} :: $1\n"
}

TESTDIR=${PWD}/build_system_tests_$$
mkdir -p ${TESTDIR}
# set NOCLEANUP=1 if you want to keep the test directory around
# for post-mortem debugging
[ -z ${NOCLEANUP} ] && trap "rm -rf ${TESTDIR}" EXIT KILL

SNAPSHOT=${TESTDIR}/snapshot
BUILD=${TESTDIR}/template/build


# copy all the build output to a snapshot directory
function take_build_snapshot()
{
    rm -rf ${SNAPSHOT}
    cp -ap ${TESTDIR}/template/build ${SNAPSHOT}
}

# verify that all the arguments are present in the build output directory
function assert_built()
{
    until [ -z "$1" ]; do
        if [ ! -f "${BUILD}/$1" ]; then
            failure "File $1 should be in the build output directory"
        fi
        shift
    done
}

# Test if a file has been rebuilt.
function file_was_rebuilt()
{
    # can't use [ a -ot b ] here as -ot only gives second resolution
    # but stat -c %y seems to be microsecond at least for tmpfs, ext4..
    if [ "$(stat -c %y ${SNAPSHOT}/$1)" != "$(stat -c %y ${BUILD}/$1)" ]; then
        return 0
    else
        return 1
    fi
}

# verify all the arguments passed in were rebuilt relative to the snapshot
function assert_rebuilt()
{
    until [ -z "$1" ]; do
        assert_built "$1"
        if [ ! -f "${SNAPSHOT}/$1" ]; then
            failure "File $1 should be in original build snapshot"
        fi
        if ! file_was_rebuilt "$1"; then
            failure "File $1 should have been rebuilt"
        fi
        shift
    done
}

# verify all the arguments are in the build directory & snapshot,
# but were not rebuilt
function assert_not_rebuilt()
{
    until [ -z "$1" ]; do
        assert_built "$1"
        if [ ! -f "${SNAPSHOT}/$1" ]; then
            failure "File $1 should be in snapshot build directory"
        fi
        if file_was_rebuilt "$1"; then
            failure "File $1 should not have been rebuilt"
        fi
        shift
    done
}

# do a "clean" that doesn't depend on idf.py
function clean_build_dir()
{
    rm -rf --preserve-root ${BUILD}/* ${BUILD}/.*
}

cd ${TESTDIR}
run_tests
