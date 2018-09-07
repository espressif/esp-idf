#!/bin/bash
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
        $CHECKOUT_REF_SCRIPT esp-idf-template
    fi

    print_status "Updating template config..."
    make defconfig || exit $?

    print_status "Try to clean fresh directory..."
    MAKEFLAGS= make clean || exit $?

    BOOTLOADER_BINS="bootloader/bootloader.elf bootloader/bootloader.bin"
    APP_BINS="app-template.elf app-template.bin"

    print_status "Initial clean build"
    # if make fails here, everything fails
    make || exit $?
    # check all the expected build artifacts from the clean build
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} partitions_singleapp.bin
    [ -f ${BUILD}/partition*.bin ] || failure "A partition table should have been built"

    print_status "Updating component source file rebuilds component"
    # touch a file & do a build
    take_build_snapshot
    touch ${IDF_PATH}/components/esp32/cpu_start.c
    make || failure "Failed to partial build"
    assert_rebuilt ${APP_BINS} esp32/libesp32.a esp32/cpu_start.o
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
    touch ${IDF_PATH}/components/esp32/ld/esp32.rom.ld
    make
    assert_rebuilt ${APP_BINS} ${BOOTLOADER_BINS}

    print_status "Touching app-only ld file should only re-link app"
    take_build_snapshot
    touch ${IDF_PATH}/components/esp32/ld/esp32.common.ld
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
    assert_rebuilt newlib/syscall_table.o
    assert_rebuilt nvs_flash/src/nvs_api.o
    assert_rebuilt freertos/xtensa_vectors.o

    print_status "Updating project Makefile triggers full recompile"
    make
    take_build_snapshot
    touch Makefile
    make
    # similar to previous test
    assert_rebuilt newlib/syscall_table.o
    assert_rebuilt nvs_flash/src/nvs_api.o
    assert_rebuilt freertos/xtensa_vectors.o

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
    echo "custom-version-x.y" > ${IDF_PATH}/version.txt
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
    [ -f ${IDF_PATH}/.gitmodules_backup ] && mv ${IDF_PATH}/.gitmodules_backup ${IDF_PATH}/.gitmodules
    export PATH=$OLD_PATH

    print_status "Build fails if partitions don't fit in flash"
    cp sdkconfig sdkconfig.bak
    sed -i "s/CONFIG_ESPTOOLPY_FLASHSIZE.\+//" sdkconfig  # remove all flashsize config
    echo "CONFIG_ESPTOOLPY_FLASHSIZE_1MB=y" >> sdkconfig     # introduce undersize flash
    make defconfig || failure "Failed to reconfigure with smaller flash"
    ( make 2>&1 | grep "does not fit in configured flash size 1MB" ) || failure "Build didn't fail with expected flash size failure message"
    mv sdkconfig.bak sdkconfig

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

cd ${TESTDIR}
run_tests
