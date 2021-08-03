#!/usr/bin/env bash
#
# Test the build system for basic consistency
#
# A bash script that tests some likely make failure scenarios in a row
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
        $CHECKOUT_REF_SCRIPT esp-idf-template .
    fi

    print_status "Updating template config..."
    make defconfig || exit $?

    print_status "Try to clean fresh directory..."
    MAKEFLAGS= make clean || exit $?

    BOOTLOADER_BINS="bootloader/bootloader.elf bootloader/bootloader.bin"
    APP_BINS="app-template.elf app-template.bin"
    PHY_INIT_BIN="phy_init_data.bin"

    print_status "Initial clean build"
    # if make fails here, everything fails
    make || exit $?
    # check all the expected build artifacts from the clean build
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} partitions_singleapp.bin
    [ -f ${BUILD}/partition*.bin ] || failure "A partition table should have been built"

    print_status "Updating component source file rebuilds component"
    # touch a file & do a build
    take_build_snapshot
    touch ${IDF_PATH}/components/esp_system/port/cpu_start.c
    make || failure "Failed to partial build"
    assert_rebuilt ${APP_BINS} esp_system/libesp_system.a esp_system/port/cpu_start.o
    assert_not_rebuilt lwip/liblwip.a freertos/libfreertos.a ${BOOTLOADER_BINS} partitions_singleapp.bin

    print_status "Bootloader source file rebuilds bootloader"
    take_build_snapshot
    touch ${IDF_PATH}/components/bootloader/subproject/main/bootloader_start.c
    make bootloader || failure "Failed to partial build bootloader"
    assert_rebuilt ${BOOTLOADER_BINS} bootloader/main/bootloader_start.o
    assert_not_rebuilt ${APP_BINS} partitions_singleapp.bin

    print_status "Partition CSV file rebuilds partitions"
    take_build_snapshot
    touch ${IDF_PATH}/components/partition_table/partitions_singleapp.csv
    make partition_table || failure "Failed to build partition table"
    assert_rebuilt partitions_singleapp.bin
    assert_not_rebuilt app-template.bin app-template.elf ${BOOTLOADER_BINS}

    print_status "Partial build doesn't compile anything by default"
    take_build_snapshot
    # verify no build files are refreshed by a partial make
    ALL_BUILD_FILES=$(find ${BUILD} -type f | sed "s@${BUILD}/@@")
    make || failure "Partial build failed"
    assert_not_rebuilt ${ALL_BUILD_FILES}

    print_status "Cleaning should remove all files from build"
    make clean || failure "Failed to make clean"
    ALL_BUILD_FILES=$(find ${BUILD} -type f)
    if [ -n "${ALL_BUILD_FILES}" ]; then
         failure "Files weren't cleaned: ${ALL_BUILD_FILES}"
    fi

    print_status "Bootloader build shouldn't leave build output anywhere else"
    clean_build_dir
    make bootloader
    # find wizardry: find any file not named sdkconfig.h that
    # isn't in the "bootloader" or "config" directories
    find ${BUILD} -type d \( -name bootloader -o -name config \) -prune , -type f ! -name sdkconfig.h || failure "Bootloader built files outside the bootloader or config directories"

    print_status "Moving BUILD_DIR_BASE out of tree"
    clean_build_dir
    OUTOFTREE_BUILD=${TESTDIR}/alt_build
    make BUILD_DIR_BASE=${OUTOFTREE_BUILD} || failure "Failed to build with BUILD_DIR_BASE overriden"
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
    make BUILD_DIR_BASE=build/subdirectory || failure "Failed to build with BUILD_DIR_BASE as subdir"
    NEW_BUILD_FILES=$(find ${BUILD}/subdirectory -type f)
    if [ -z "${NEW_BUILD_FILES}" ]; then
        failure "No files found in new build directory!"
    fi

    print_status "Parallel builds should work OK"
    clean_build_dir
    (make -j5 2>&1 | tee ${TESTDIR}/parallel_build.log) || failure "Failed to build in parallel"
    if grep -q "warning: jobserver unavailable" ${TESTDIR}/parallel_build.log; then
        failure "Parallel build prints 'warning: jobserver unavailable' errors"
    fi

    print_status "Can still clean build if all text files are CRLFs"
    make clean || failure "Unexpected failure to make clean"
    find . -path .git -prune -exec unix2dos {} \; # CRLFify template dir
    # make a copy of esp-idf and CRLFify it
    CRLF_ESPIDF=${TESTDIR}/esp-idf-crlf
    mkdir -p ${CRLF_ESPIDF}
    cp -r ${IDF_PATH}/* ${CRLF_ESPIDF}
    # don't CRLFify executable files, as Linux will fail to execute them
    find ${CRLF_ESPIDF} -name .git -prune -name build -prune -type f ! -perm 755 -exec unix2dos {} \;
    make IDF_PATH=${CRLF_ESPIDF} || failure "Failed to build with CRLFs in source"
    # do the same checks we do for the clean build
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} partitions_singleapp.bin
    [ -f ${BUILD}/partition*.bin ] || failure "A partition table should have been built in CRLF mode"

    print_status "Touching rom ld file should re-link app and bootloader"
    make
    take_build_snapshot
    touch ${IDF_PATH}/components/esp_rom/esp32/ld/esp32.rom.ld
    make
    assert_rebuilt ${APP_BINS} ${BOOTLOADER_BINS}

    print_status "Touching app-only template ld file should only re-link app"
    take_build_snapshot
    touch ${IDF_PATH}/components/esp_system/ld/esp32/sections.ld.in
    make
    assert_rebuilt ${APP_BINS}
    assert_not_rebuilt ${BOOTLOADER_BINS}

    print_status "Touching a linker fragment file should trigger re-link of app" # only app linker script is generated by tool for now
    take_build_snapshot
    touch ${IDF_PATH}/components/esp_common/common.lf
    make
    assert_rebuilt ${APP_BINS}
    assert_not_rebuilt ${BOOTLOADER_BINS}

    print_status "sdkconfig update triggers full recompile"
    make
    take_build_snapshot
    touch sdkconfig
    make
    # check the component_project_vars.mk file was rebuilt
    assert_rebuilt esp32/component_project_vars.mk
    # pick one each of .c, .cpp, .S that #includes sdkconfig.h
    # and therefore should rebuild
    assert_rebuilt newlib/newlib_init.o
    assert_rebuilt nvs_flash/src/nvs_api.o
    assert_rebuilt freertos/port/xtensa/xtensa_vectors.o

    print_status "Updating project Makefile triggers full recompile"
    make
    take_build_snapshot
    touch Makefile
    make
    # similar to previous test
    assert_rebuilt newlib/newlib_init.o
    assert_rebuilt nvs_flash/src/nvs_api.o
    assert_rebuilt freertos/port/xtensa/xtensa_vectors.o

    print_status "print_flash_cmd target should produce one line of output"
    make
    test $(make print_flash_cmd V=0 | wc -l | tr -d ' ') -eq 1

    print_status "Can include/exclude object files"
    echo "#error This file should not compile" > main/excluded_file.c
    echo "int required_global;" > main/included_file.c
    echo "COMPONENT_OBJEXCLUDE := excluded_file.o" >> main/component.mk
    echo "COMPONENT_OBJINCLUDE := included_file.o" >> main/component.mk
    echo "COMPONENT_ADD_LDFLAGS := -l\$(COMPONENT_NAME) -u required_global" >> main/component.mk
    make
    git checkout main/component.mk
    rm main/{included,excluded}_file.c

    print_status "Can include/exclude object files outside of component tree"
    mkdir -p extra_source_dir
    echo "#error This file should not compile" > extra_source_dir/excluded_file.c
    echo "int required_global;" > extra_source_dir/included_file.c
    echo "COMPONENT_SRCDIRS := . ../extra_source_dir" >> main/component.mk
    echo "COMPONENT_OBJEXCLUDE := ../extra_source_dir/excluded_file.o" >> main/component.mk
    echo "COMPONENT_OBJINCLUDE := ../extra_source_dir/included_file.o" >> main/component.mk
    echo "COMPONENT_ADD_LDFLAGS := -l\$(COMPONENT_NAME) -u required_global" >> main/component.mk
    make
    git checkout main/component.mk
    rm -rf extra_source_dir

    print_status "Can build without git installed on system"
    clean_build_dir
    # Make provision for getting IDF version
    echo "IDF_VER_0123456789_0123456789_0123456789" > ${IDF_PATH}/version.txt
    echo "project-version-w.z" > ${TESTDIR}/template/version.txt
    # Hide .gitmodules so that submodule check is avoided
    [ -f ${IDF_PATH}/.gitmodules ] && mv ${IDF_PATH}/.gitmodules ${IDF_PATH}/.gitmodules_backup
    # Overload `git` command
    echo -e '#!/bin/bash\ntouch ${IDF_PATH}/git_invoked' > git
    chmod +x git
    OLD_PATH=$PATH
    export PATH="$PWD:$PATH"
    make
    [ -f ${IDF_PATH}/git_invoked ] && rm ${IDF_PATH}/git_invoked && failure "git should not have been invoked in this case"
    rm -f ${IDF_PATH}/version.txt git
    rm -f ${TESTDIR}/template/version.txt
    [ -f ${IDF_PATH}/.gitmodules_backup ] && mv ${IDF_PATH}/.gitmodules_backup ${IDF_PATH}/.gitmodules
    export PATH=$OLD_PATH

    print_status "Rebuild when app version was changed"
    take_build_snapshot
    # App version
    echo "project-version-1.0" > ${TESTDIR}/template/version.txt
    make
    assert_rebuilt ${APP_BINS}
    print_status "Change app version"
    take_build_snapshot
	echo "project-version-2.0(012345678901234567890123456789)" > ${TESTDIR}/template/version.txt
	make
    assert_rebuilt ${APP_BINS}
    assert_not_rebuilt ${BOOTLOADER_BINS} esp_system/libesp_system.a

    print_status "Re-building does not change app.bin"
    take_build_snapshot
    make
    assert_not_rebuilt ${APP_BINS} ${BOOTLOADER_BINS} esp_system/libesp_system.a
    rm -f ${TESTDIR}/template/version.txt

    print_status "Get the version of app from git describe. Project is not inside IDF and do not have a tag only a hash commit."
    make >> log.log || failure "Failed to build"
    version="App \"app-template\" version: "
    version+=$(git describe --always --tags --dirty)
    grep "${version}" log.log || failure "Project version should have a hash commit"

    print_status "Get the version of app from Kconfig option"
    make clean > /dev/null
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    echo "project_version_from_txt" > ${TESTDIR}/template/version.txt
    echo "CONFIG_APP_PROJECT_VER_FROM_CONFIG=y" >> sdkconfig.defaults
    echo 'CONFIG_APP_PROJECT_VER="project_version_from_Kconfig"' >> sdkconfig.defaults
    make defconfig > /dev/null
    make >> log.log || failure "Failed to build"
    version="App \"app-template\" version: "
    version+="project_version_from_Kconfig"
    grep "${version}" log.log || failure "Project version should be from Kconfig"
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    rm -f ${TESTDIR}/template/version.txt

    print_status "Build fails if partitions don't fit in flash"
    sed -i.bak "s/CONFIG_ESPTOOLPY_FLASHSIZE.\+//" sdkconfig  # remove all flashsize config
    echo "CONFIG_ESPTOOLPY_FLASHSIZE_1MB=y" >> sdkconfig     # introduce undersize flash
    make defconfig || failure "Failed to reconfigure with smaller flash"
    ( make 2>&1 | grep "does not fit in configured flash size 1MB" ) || failure "Build didn't fail with expected flash size failure message"
    mv sdkconfig.bak sdkconfig

    print_status "Flash size is correctly set in the bootloader image header"
    # Build with the default 2MB setting
    rm sdkconfig
    make defconfig && make bootloader || failure "Failed to build bootloader"
    bin_header_match build/bootloader/bootloader.bin "0210"
    # Change to 4MB
    echo "CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y" > sdkconfig
    make defconfig && make bootloader || failure "Failed to build bootloader"
    bin_header_match build/bootloader/bootloader.bin "0220"
    # Change to QIO, bootloader should still be DIO (will change to QIO in 2nd stage bootloader)
    echo "CONFIG_FLASHMODE_QIO=y" > sdkconfig
    make defconfig && make bootloader || failure "Failed to build bootloader"
    bin_header_match build/bootloader/bootloader.bin "0210"
    # Change to 80 MHz
    echo "CONFIG_ESPTOOLPY_FLASHFREQ_80M=y" > sdkconfig
    make defconfig && make bootloader || failure "Failed to build bootloader"
    bin_header_match build/bootloader/bootloader.bin "021f"
    rm sdkconfig

    print_status "sdkconfig should have contents of all files: sdkconfig, sdkconfig.defaults, sdkconfig.defaults.IDF_TARGET"
    make clean > /dev/null;
    rm -f sdkconfig.defaults;
    rm -f sdkconfig;
    echo "CONFIG_PARTITION_TABLE_OFFSET=0x10000" >> sdkconfig.defaults;
    echo "CONFIG_ESP32_DEFAULT_CPU_FREQ_240=y" >> sdkconfig.defaults.esp32;
    echo "CONFIG_PARTITION_TABLE_TWO_OTA=y" >> sdkconfig;
    make defconfig > /dev/null;
    grep "CONFIG_PARTITION_TABLE_OFFSET=0x10000" sdkconfig || failure "The define from sdkconfig.defaults should be into sdkconfig"
    grep "CONFIG_ESP32_DEFAULT_CPU_FREQ_240=y" sdkconfig || failure "The define from sdkconfig.defaults.esp32 should be into sdkconfig"
    grep "CONFIG_PARTITION_TABLE_TWO_OTA=y" sdkconfig || failure "The define from sdkconfig should be into sdkconfig"
    rm sdkconfig sdkconfig.defaults sdkconfig.defaults.esp32
    make defconfig

    print_status "can build with phy_init_data"
    make clean > /dev/null
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    echo "CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION=y" >> sdkconfig.defaults
    make defconfig > /dev/null
    make || failure "Failed to build with PHY_INIT_DATA"
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PHY_INIT_BIN}
    rm sdkconfig
    rm sdkconfig.defaults
    make defconfig

    print_status "UF2 build works"
    rm -f -r build sdkconfig
    make defconfig
    make uf2
    assert_built ${APP_BINS} "uf2.bin"
    make uf2-app
    assert_built "uf2-app.bin"
    rm -f -r build sdkconfig

    print_status "Empty directory not treated as a component"
    mkdir -p components/esp32
    make || failure "Failed to build with empty esp32 directory in components"
    rm -rf components

    print_status "If a component directory is added to COMPONENT_DIRS, its subdirectories are not added"
    mkdir -p main/test
    touch main/test/component.mk
    echo "#error This should not be built" > main/test/test.c
    make || failure "COMPONENT_DIRS has added component subdirectory to the build"
    rm -rf main/test

    print_status "If a component directory is added to COMPONENT_DIRS, its sibling directories are not added"
    mkdir -p mycomponents/mycomponent
    touch mycomponents/mycomponent/component.mk
    # first test by adding single component directory to EXTRA_COMPONENT_DIRS
    mkdir -p mycomponents/esp32
    touch mycomponents/esp32/component.mk
    make EXTRA_COMPONENT_DIRS=$PWD/mycomponents/mycomponent || failure "EXTRA_COMPONENT_DIRS has added a sibling directory"
    rm -rf mycomponents/esp32
    # now the same thing, but add a components directory
    mkdir -p esp32
    touch esp32/component.mk
    make EXTRA_COMPONENT_DIRS=$PWD/mycomponents || failure "EXTRA_COMPONENT_DIRS has added a sibling directory"
    rm -rf esp32
    rm -rf mycomponents

    print_status "Handling deprecated Kconfig options"
    make clean > /dev/null;
    rm -f sdkconfig.defaults;
    rm -f sdkconfig;
    echo "" > ${IDF_PATH}/sdkconfig.rename;
    make defconfig > /dev/null;
    echo "CONFIG_TEST_OLD_OPTION=y" >> sdkconfig;
    echo "CONFIG_TEST_OLD_OPTION CONFIG_TEST_NEW_OPTION" >> ${IDF_PATH}/sdkconfig.rename;
    echo -e "\n\
menu \"test\"\n\
    config TEST_NEW_OPTION\n\
        bool \"test\"\n\
        default \"n\"\n\
        help\n\
            TEST_NEW_OPTION description\n\
endmenu\n" >> ${IDF_PATH}/Kconfig;
    make defconfig > /dev/null;
    grep "CONFIG_TEST_OLD_OPTION=y" sdkconfig || failure "CONFIG_TEST_OLD_OPTION should be in sdkconfig for backward compatibility"
    grep "CONFIG_TEST_NEW_OPTION=y" sdkconfig || failure "CONFIG_TEST_NEW_OPTION should be now in sdkconfig"
    grep "#define CONFIG_TEST_NEW_OPTION 1" build/include/sdkconfig.h || failure "sdkconfig.h should contain the new macro"
    grep "#define CONFIG_TEST_OLD_OPTION CONFIG_TEST_NEW_OPTION" build/include/sdkconfig.h || failure "sdkconfig.h should contain the compatibility macro"
    grep "CONFIG_TEST_OLD_OPTION=y" build/include/config/auto.conf || failure "CONFIG_TEST_OLD_OPTION should be in auto.conf for backward compatibility"
    grep "CONFIG_TEST_NEW_OPTION=y" build/include/config/auto.conf || failure "CONFIG_TEST_NEW_OPTION should be now in auto.conf"
    rm -f sdkconfig sdkconfig.defaults
    pushd ${IDF_PATH}
    git checkout -- sdkconfig.rename Kconfig
    popd

    print_status "Handling deprecated Kconfig options in sdkconfig.defaults"
    make clean;
    rm -f sdkconfig;
    echo "CONFIG_TEST_OLD_OPTION=7" > sdkconfig.defaults;
    echo "CONFIG_TEST_OLD_OPTION CONFIG_TEST_NEW_OPTION" > ${IDF_PATH}/sdkconfig.rename;
    echo -e "\n\
menu \"test\"\n\
    config TEST_NEW_OPTION\n\
        int \"TEST_NEW_OPTION\"\n\
        range 0 10\n\
        default 5\n\
        help\n\
            TEST_NEW_OPTION description\n\
endmenu\n" >> ${IDF_PATH}/Kconfig;
    make defconfig > /dev/null;
    grep "CONFIG_TEST_OLD_OPTION=7" sdkconfig || failure "CONFIG_TEST_OLD_OPTION=7 should be in sdkconfig for backward compatibility"
    grep "CONFIG_TEST_NEW_OPTION=7" sdkconfig || failure "CONFIG_TEST_NEW_OPTION=7 should be in sdkconfig"
    rm -f sdkconfig.defaults;
    pushd ${IDF_PATH}
    git checkout -- sdkconfig.rename Kconfig
    popd

    print_status "Project components prioritized over EXTRA_COMPONENT_DIRS"
    clean_build_dir
    mkdir -p extra_dir/my_component
    echo "COMPONENT_CONFIG_ONLY := 1" > extra_dir/my_component/component.mk
    cp Makefile Makefile.bak # set EXTRA_COMPONENT_DIRS to point to the other directory
    sed -i "s%PROJECT_NAME := app-template%PROJECT_NAME := app-template\nEXTRA_COMPONENT_DIRS := extra_dir%" Makefile
    (make list-components | grep "$PWD/extra_dir/my_component") || failure  "Unable to find component specified in EXTRA_COMPONENT_DIRS"
    mkdir -p components/my_component
    echo "COMPONENT_CONFIG_ONLY := 1" > components/my_component/component.mk
    (make list-components | grep "$PWD/components/my_component") || failure  "Project components should be prioritized over EXTRA_COMPONENT_DIRS"
    mv Makefile.bak Makefile # revert previous modifications
    rm -rf extra_dir components

    print_status "COMPONENT_OWNBUILDTARGET, COMPONENT_OWNCLEANTARGET can work"
    take_build_snapshot
    mkdir -p components/test_component
    cat > components/test_component/component.mk  <<EOF
COMPONENT_OWNBUILDTARGET:=custom_build
COMPONENT_OWNCLEANTARGET:=custom_clean

.PHONY: custom_target

custom_build:
	echo "Running custom_build!"
	echo "" | \$(CC) -x c++ -c -o dummy_obj.o -
	\$(AR) cr libtest_component.a dummy_obj.o

custom_clean:
	rm -f libtest_component.a dummy_obj.o
EOF
    make || failure "Failed to build with custom component build target"
    [ -f ${BUILD}/test_component/dummy_obj.o ] || failure "Failed to build dummy_obj.o in custom target"
    [ -f ${BUILD}/test_component/libtest_component.a ] || failure "Failed to build custom component library"
    grep -q "libtest_component.a" ${BUILD}/*.map || failure "Linker didn't see the custom library"
    make clean || failure "Failed to make clean with custom clean target"
    [ -f ${BUILD}/test_component/dummy_obj.o ] && failure "Custom clean target didn't clean object file"
    [ -f ${BUILD}/test_component/libtest_component.a ] && failure "Custom clean target didn't clean library"
    rm -rf components/test_component

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
            failure "File $1 should have been original build snapshot"
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

# do a "clean" that doesn't depend on 'make clean'
function clean_build_dir()
{
    rm -rf --preserve-root ${BUILD}/*
}

# check the bytes 3-4 of the binary image header. e.g.:
#   bin_header_match app.bin 0210
function bin_header_match()
{
    expected=$2
    filename=$1
    actual=$(xxd -s 2 -l 2 -ps $1)
    if [ ! "$expected" = "$actual" ]; then
        failure "Incorrect binary image header, expected $expected got $actual"
    fi
}

cd ${TESTDIR}
run_tests
