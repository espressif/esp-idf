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

# Some tests assume that ccache is not enabled
unset IDF_CCACHE_ENABLE

function get_file_size() {
  if [[ $OSTYPE == 'darwin'* ]]; then
    BINSIZE=$(stat -f "%z" ${1})
  else
    BINSIZE=$(stat -c "%s" ${1})
  fi
}

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

    print_status "Try to clean fresh directory..."
    idf.py fullclean || exit $?

    # all relative to the build directory
    BOOTLOADER_BINS="bootloader/bootloader.elf bootloader/bootloader.bin"
    APP_BINS="app-template.elf app-template.bin"
    PARTITION_BIN="partition_table/partition-table.bin"
    PHY_INIT_BIN="phy_init_data.bin"
    BUILD_ARTIFACTS="project_description.json flasher_args.json config/kconfig_menus.json config/sdkconfig.json"
    IDF_COMPONENT_PREFIX="__idf"

    print_status "Initial clean build"
    # if build fails here, everything fails
    idf.py build || exit $?

    # check all the expected build artifacts from the clean build
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN} ${BUILD_ARTIFACTS}

    print_status "Updating component source file rebuilds component"
    # touch a file & do a build
    take_build_snapshot
    touch ${IDF_PATH}/components/esp_system/port/cpu_start.c
    idf.py build || failure "Failed to partial build"
    assert_rebuilt ${APP_BINS} esp-idf/esp_system/libesp_system.a esp-idf/esp_system/CMakeFiles/${IDF_COMPONENT_PREFIX}_esp_system.dir/port/cpu_start.c.obj
    assert_not_rebuilt esp-idf/lwip/liblwip.a esp-idf/freertos/libfreertos.a ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Bootloader source file rebuilds bootloader"
    take_build_snapshot
    touch ${IDF_PATH}/components/bootloader/subproject/main/bootloader_start.c
    idf.py build || failure "Failed to partial build bootloader"
    assert_rebuilt ${BOOTLOADER_BINS} bootloader/esp-idf/main/CMakeFiles/${IDF_COMPONENT_PREFIX}_main.dir/bootloader_start.c.obj
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
    ALL_BUILD_FILES=$(find ${BUILD} -type f | ${SED} "s@${BUILD}/@@" | grep -v '^.')
    idf.py build || failure "Partial build failed"
    assert_not_rebuilt ${ALL_BUILD_FILES}

    print_status "Rebuild when app version was changed"
    clean_build_dir
    # App version
    echo "IDF_VER_0123456789_0123456789_0123456789" > ${IDF_PATH}/version.txt
    echo "project-version-1.0" > ${TESTDIR}/template/version.txt
    idf.py build || failure "Failed to build with app version"
    print_status "Change app version"
    take_build_snapshot
	echo "project-version-2.0(012345678901234567890123456789)" > ${TESTDIR}/template/version.txt
	idf.py build || failure "Failed to rebuild with changed app version"
    assert_rebuilt ${APP_BINS}
    assert_not_rebuilt ${BOOTLOADER_BINS} esp-idf/esp_system/libesp_system.a

    print_status "Re-building does not change app.bin"
    take_build_snapshot
    idf.py build
    assert_not_rebuilt ${APP_BINS} ${BOOTLOADER_BINS} esp-idf/esp_system/libesp_system.a
    rm -f ${IDF_PATH}/version.txt
    rm -f ${TESTDIR}/template/version.txt

    print_status "Get the version of app from git describe. Project is not inside IDF and do not have a tag only a hash commit."
    idf.py reconfigure >> log.log || failure "Failed to build"
    version="App \"app-template\" version: "
    version+=$(git describe --always --tags --dirty)
    grep "${version}" log.log || failure "Project version should have a hash commit"

    print_status "Get the version of app from Kconfig option"
    idf.py clean > /dev/null
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    echo "project_version_from_txt" > ${TESTDIR}/template/version.txt
    echo "CONFIG_APP_PROJECT_VER_FROM_CONFIG=y" >> sdkconfig.defaults
    echo 'CONFIG_APP_PROJECT_VER="project_version_from_Kconfig"' >> sdkconfig.defaults
    idf.py build >> log.log || failure "Failed to build"
    version="App \"app-template\" version: "
    version+="project_version_from_Kconfig"
    grep "${version}" log.log || failure "Project version should be from Kconfig"
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    rm -f ${TESTDIR}/template/version.txt

    print_status "Use IDF version variables in component CMakeLists.txt file"
    clean_build_dir
    (echo -e "if (NOT IDF_VERSION_MAJOR)\n message(FATAL_ERROR \"IDF version not set\")\n endif()" \
        && cat main/CMakeLists.txt) > main/CMakeLists.new && mv main/CMakeLists.new main/CMakeLists.txt
    idf.py reconfigure || failure "Failed to use IDF_VERSION_MAJOR in component CMakeLists.txt"
    git checkout -- main/CMakeLists.txt

    print_status "Project is in ESP-IDF which has a custom tag"
    pushd ${IDF_PATH}/examples/get-started/hello_world
    GIT_COMMITTER_NAME="No One" GIT_COMMITTER_EMAIL="noone@espressif.com" git tag mytag -a -m "mytag" || failure "Git cannot create tag"
    idf.py reconfigure &> log.log || failure "Failed to build"
    str="App \"hello_world\" version: mytag"
    grep "${str}" log.log || { cat log.log ; failure "Project version should be the custom tag"; }
    idf_version=$(idf.py --version)
    if [[ "$idf_version" == *"mytag"* ]]; then
        failure "IDF version $idf_version should not contain mytag"
    fi
    git tag -d mytag
    rm -rf sdkconfig build
    popd

    print_status "Moving BUILD_DIR_BASE out of tree"
    clean_build_dir
    OUTOFTREE_BUILD=${TESTDIR}/alt_build
    idf.py -B "${OUTOFTREE_BUILD}" build || failure "Failed to build with out-of-tree build dir"
    NEW_BUILD_FILES=$(find ${OUTOFTREE_BUILD} -type f)
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
    TESTDIR_REL=$($REALPATH ${TESTDIR} --relative-to ${IDF_PATH})
    # Note: trailing slash after ${IDF_PATH} avoids creating esp-idf directory inside ${CRLF_ESPIDF}
    rsync -a --exclude ${TESTDIR_REL} ${IDF_PATH}/ ${CRLF_ESPIDF}
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
    cp ${IDF_PATH}/components/esp_rom/esp32/ld/esp32.rom.ld .
    echo "/* (Build test comment) */" >> ${IDF_PATH}/components/esp_rom/esp32/ld/esp32.rom.ld
    tail ${IDF_PATH}/components/esp_rom/esp32/ld/esp32.rom.ld
    idf.py build || failure "Failed to rebuild with modified linker script"
    assert_rebuilt ${APP_BINS} ${BOOTLOADER_BINS}
    mv esp32.rom.ld ${IDF_PATH}/components/esp_rom/esp32/ld/

    print_status "Updating app-only ld file should only re-link app"
    take_build_snapshot
    cp ${IDF_PATH}/components/esp_system/ld/esp32/sections.ld.in .
    sleep 1  # ninja may ignore if the timestamp delta is too low
    echo "/* (Build test comment) */" >> ${IDF_PATH}/components/esp_system/ld/esp32/sections.ld.in
    idf.py build || failure "Failed to rebuild with modified linker script"
    assert_rebuilt ${APP_BINS}
    assert_not_rebuilt ${BOOTLOADER_BINS}
    mv sections.ld.in ${IDF_PATH}/components/esp_system/ld/esp32

    print_status "Updating ld file should only re-link app"
    take_build_snapshot
    cp ${IDF_PATH}/components/esp_system/ld/esp32/memory.ld .
    sleep 1  # ninja may ignore if the timestamp delta is too low
    echo "/* (Build test comment) */" >> ${IDF_PATH}/components/esp_system/ld/esp32/memory.ld.in
    idf.py build || failure "Failed to rebuild with modified linker script"
    assert_rebuilt ${APP_BINS}
    assert_not_rebuilt ${BOOTLOADER_BINS}
    mv memory.ld ${IDF_PATH}/components/esp_system/ld/esp32/

    print_status "Updating fragment file should only re-link app" # only app linker script is generated by tool for now
    take_build_snapshot
    cp ${IDF_PATH}/components/esp_common/common.lf .
    sleep 1  # ninja may ignore if the timestamp delta is too low
    echo "# (Build test comment)" >> ${IDF_PATH}/components/esp_common/common.lf
    idf.py build || failure "Failed to rebuild with modified linker fragment file"
    assert_rebuilt ${APP_BINS}
    assert_not_rebuilt ${BOOTLOADER_BINS}
    mv common.lf ${IDF_PATH}/components/esp_common

    print_status "sdkconfig update triggers full recompile"
    clean_build_dir
    idf.py build
    take_build_snapshot
    # need to actually change config, or cmake is too smart to rebuild
    ${SED} -i.bak s/^\#\ CONFIG_FREERTOS_UNICORE\ is\ not\ set/CONFIG_FREERTOS_UNICORE=y/ sdkconfig
    idf.py build
    # check the sdkconfig.h file was rebuilt
    assert_rebuilt config/sdkconfig.h
    # pick one each of .c, .cpp, .S that #includes sdkconfig.h
    # and therefore should rebuild
    assert_rebuilt esp-idf/newlib/CMakeFiles/${IDF_COMPONENT_PREFIX}_newlib.dir/newlib_init.c.obj
    assert_rebuilt esp-idf/nvs_flash/CMakeFiles/${IDF_COMPONENT_PREFIX}_nvs_flash.dir/src/nvs_api.cpp.obj
    assert_rebuilt esp-idf/esp_system/CMakeFiles/${IDF_COMPONENT_PREFIX}_esp_system.dir/port/arch/xtensa/panic_handler_asm.S.obj
    mv sdkconfig.bak sdkconfig

    print_status "Updating project CMakeLists.txt triggers full recompile"
    clean_build_dir
    idf.py build
    take_build_snapshot
    # Need to actually change the build config, or CMake won't do anything
    cp CMakeLists.txt CMakeLists.bak
    ${SED} -i.bak 's/^project(/add_compile_options("-DUSELESS_MACRO_DOES_NOTHING=1")\nproject\(/' CMakeLists.txt
    idf.py build || failure "Build failed"
    mv CMakeLists.bak CMakeLists.txt
    # similar to previous test
    assert_rebuilt esp-idf/newlib/CMakeFiles/${IDF_COMPONENT_PREFIX}_newlib.dir/newlib_init.c.obj
    assert_rebuilt esp-idf/nvs_flash/CMakeFiles/${IDF_COMPONENT_PREFIX}_nvs_flash.dir/src/nvs_api.cpp.obj
    assert_rebuilt esp-idf/esp_system/CMakeFiles/${IDF_COMPONENT_PREFIX}_esp_system.dir/port/arch/xtensa/panic_handler_asm.S.obj
    mv sdkconfig.bak sdkconfig

    print_status "Can build with Ninja (no idf.py)"
    clean_build_dir
    (cd build && cmake -G Ninja .. && ninja)  || failure "Ninja build failed"
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Can build with GNU Make (no idf.py)"
    clean_build_dir
    mkdir build
    (cd build && cmake -G "Unix Makefiles" .. && make) || failure "Make build failed"
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "idf.py can build with Ninja"
    clean_build_dir
    idf.py -G Ninja build  || failure "idf.py cannot build with Ninja"
    grep "CMAKE_GENERATOR:INTERNAL=Ninja" build/CMakeCache.txt || failure "Ninja is not set in CMakeCache.txt"
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "idf.py can build with Unix Makefiles"
    clean_build_dir
    mkdir build
    idf.py -G "Unix Makefiles" build || failure "idf.py cannot build with Unix Makefiles"
    grep "CMAKE_GENERATOR:INTERNAL=Unix Makefiles" build/CMakeCache.txt || failure "Unix Makefiles are not set in CMakeCache.txt"
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Can build with IDF_PATH set via cmake cache not environment"
    clean_build_dir
    ${SED} -i.bak 's/ENV{IDF_PATH}/{IDF_PATH}/' CMakeLists.txt
    export IDF_PATH_BACKUP="$IDF_PATH"
    (unset IDF_PATH &&
         cd build &&
         cmake -G Ninja .. -DIDF_PATH=${IDF_PATH_BACKUP} &&
         ninja) || failure "Ninja build failed"
    mv CMakeLists.txt.bak CMakeLists.txt
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Can build with IDF_PATH unset and inferred by build system"
    clean_build_dir
    ${SED} -i.bak "s%\$ENV{IDF_PATH}%\${ci_idf_path}%" CMakeLists.txt  # expand to a hardcoded path
    (ci_idf_path=${IDF_PATH} && unset IDF_PATH && cd build &&
         cmake -G Ninja -D ci_idf_path=${ci_idf_path} .. && ninja) || failure "Ninja build failed"
    mv CMakeLists.txt.bak CMakeLists.txt
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Can build with IDF_PATH unset and inferred by cmake when Kconfig needs it to be set"
    clean_build_dir
    ${SED} -i.bak 's/ENV{IDF_PATH}/{IDF_PATH}/' CMakeLists.txt
    export IDF_PATH_BACKUP="$IDF_PATH"
    mv main/Kconfig.projbuild main/Kconfig.projbuild_bak
    echo "source \"\$IDF_PATH/examples/wifi/getting_started/station/main/Kconfig.projbuild\"" > main/Kconfig.projbuild
    (unset IDF_PATH &&
         cd build &&
         cmake -G Ninja .. -DIDF_PATH=${IDF_PATH_BACKUP} &&
         ninja) || failure "Ninja build failed"
    mv CMakeLists.txt.bak CMakeLists.txt
    mv main/Kconfig.projbuild_bak main/Kconfig.projbuild
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "can build with phy_init_data"
    idf.py clean > /dev/null
    idf.py fullclean > /dev/null
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    echo "CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION=y" >> sdkconfig.defaults
    idf.py reconfigure > /dev/null
    idf.py build || failure "Failed to build with PHY_INIT_DATA"
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN} ${PHY_INIT_BIN}
    rm sdkconfig
    rm sdkconfig.defaults

    print_status "can build with ethernet component disabled"
    idf.py clean > /dev/null
    idf.py fullclean > /dev/null
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    echo "CONFIG_ETH_USE_SPI_ETHERNET=" >> sdkconfig.defaults
    echo "CONFIG_ETH_USE_ESP32_EMAC=" >> sdkconfig.defaults
    idf.py reconfigure > /dev/null
    idf.py build || failure "Failed to build with ethernet component disabled"
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}
    rm sdkconfig
    rm sdkconfig.defaults

    print_status "Compiler flags on build command line are taken into account"
    clean_build_dir
    # Backup original source file
    cp main/main.c main/main.c.bak
    # Alter source file to check user flag
    echo -e "\n#ifndef USER_FLAG \n \
#error \"USER_FLAG is not defined!\" \n \
#endif\n" >> main/main.c
    idf.py build -DCMAKE_C_FLAGS=-DUSER_FLAG || failure "User flags should have been taken into account"
    # Restore original file
    mv main/main.c.bak main/main.c

    print_status "Compiler flags cannot be overwritten"
    clean_build_dir
    # If the compiler flags are overriden, the following build command will
    # cause issues at link time.
    idf.py build -DCMAKE_C_FLAGS= -DCMAKE_CXX_FLAGS= || failure "CMake compiler flags have been overriden"

    # the next tests use the esp32s2 target
    export other_target=esp32s2

    print_status "Can override IDF_TARGET from environment"
    clean_build_dir
    rm sdkconfig
    export IDF_TARGET=$other_target
    (cd build && cmake -G Ninja .. ) || failure "Failed to configure with IDF_TARGET set in environment"
    grep "CONFIG_IDF_TARGET=\"${other_target}\"" sdkconfig || failure "Project not configured for IDF_TARGET correctly"
    grep "IDF_TARGET:STRING=${other_target}" build/CMakeCache.txt || failure "IDF_TARGET not set in CMakeCache.txt"
    unset IDF_TARGET

    print_status "Can set target using idf.py -D"
    clean_build_dir
    rm sdkconfig
    idf.py -DIDF_TARGET=$other_target reconfigure || failure "Failed to set target via idf.py"
    grep "CONFIG_IDF_TARGET=\"${other_target}\"" sdkconfig || failure "Project not configured correctly using idf.py -D"
    grep "IDF_TARGET:STRING=${other_target}" build/CMakeCache.txt || failure "IDF_TARGET not set in CMakeCache.txt using idf.py -D"

    print_status "Can set target using -D as subcommand parameter for idf.py"
    clean_build_dir
    rm sdkconfig
    idf.py reconfigure -DIDF_TARGET=$other_target || failure "Failed to set target via idf.py subcommand -D parameter"
    grep "CONFIG_IDF_TARGET=\"${other_target}\"" sdkconfig || failure "Project not configured correctly using idf.py reconfigure -D"
    grep "IDF_TARGET:STRING=${other_target}" build/CMakeCache.txt || failure "IDF_TARGET not set in CMakeCache.txt using idf.py reconfigure -D"

    print_status "Can set target using idf.py set-target"
    clean_build_dir
    rm sdkconfig
    idf.py set-target ${other_target} || failure "Failed to set target via idf.py set-target"
    grep "CONFIG_IDF_TARGET=\"${other_target}\"" sdkconfig || failure "Project not configured correctly using idf.py set-target"
    grep "IDF_TARGET:STRING=${other_target}" build/CMakeCache.txt || failure "IDF_TARGET not set in CMakeCache.txt using idf.py set-target"

    print_status "idf.py understands alternative target names"
    clean_build_dir
    rm sdkconfig
    idf.py set-target ESP32-S2
    grep "CONFIG_IDF_TARGET=\"${other_target}\"" sdkconfig || failure "Project not configured correctly using idf.py set-target"
    grep "IDF_TARGET:STRING=${other_target}" build/CMakeCache.txt || failure "IDF_TARGET not set in CMakeCache.txt using idf.py set-target"

    print_status "Can guess target from sdkconfig, if CMakeCache does not exist"
    idf.py fullclean || failure "Failed to clean the build directory"
    idf.py reconfigure || failure "Failed to reconfigure after fullclean"
    grep "CONFIG_IDF_TARGET=\"${other_target}\"" sdkconfig || failure "Didn't find the expected CONFIG_IDF_TARGET value"
    grep "IDF_TARGET:STRING=${other_target}" build/CMakeCache.txt || failure "IDF_TARGET not set in CMakeCache.txt after fullclean and reconfigure"

    print_status "Can set the default target using sdkconfig.defaults"
    clean_build_dir
    rm sdkconfig
    echo "CONFIG_IDF_TARGET=\"${other_target}\"" > sdkconfig.defaults
    idf.py reconfigure || failure "Failed to reconfigure with default target set in sdkconfig.defaults"
    grep "CONFIG_IDF_TARGET=\"${other_target}\"" sdkconfig || failure "Didn't find the expected CONFIG_IDF_TARGET value"
    other_target_caps=$(tr 'a-z' 'A-Z' <<< "${other_target}")
    grep "CONFIG_IDF_TARGET_${other_target_caps}=y" sdkconfig || failure "Didn't find CONFIG_IDF_TARGET_${other_target_caps} value"
    grep "IDF_TARGET:STRING=${other_target}" build/CMakeCache.txt || failure "IDF_TARGET not set in CMakeCache.txt after fullclean and reconfigure"
    rm sdkconfig.defaults

    print_status "IDF_TARGET takes precedence over the value of CONFIG_IDF_TARGET in sdkconfig.defaults"
    clean_build_dir
    rm sdkconfig
    echo "CONFIG_IDF_TARGET=\"${other_target}\"" > sdkconfig.defaults
    export IDF_TARGET=esp32
    idf.py reconfigure || failure "Failed to reconfigure with default target set in sdkconfig.defaults and different IDF_TARGET in the environment"
    grep "CONFIG_IDF_TARGET=\"esp32\"" sdkconfig || failure "Didn't find the expected CONFIG_IDF_TARGET value"
    grep "CONFIG_IDF_TARGET_ESP32=y" sdkconfig || failure "Didn't find CONFIG_IDF_TARGET_ESP32 value"
    grep "IDF_TARGET:STRING=esp32" build/CMakeCache.txt || failure "IDF_TARGET not set in CMakeCache.txt after fullclean and reconfigure"
    rm sdkconfig.defaults
    unset IDF_TARGET

    print_status "idf.py fails if IDF_TARGET settings don't match in sdkconfig, CMakeCache.txt, and the environment"
    clean_build_dir
    rm sdkconfig
    idf.py set-target ${other_target} || failure "Couldn't set target to ${other_target}"
    # Change to a different IDF_TARGET in the environment
    export IDF_TARGET=esp32
    ! idf.py reconfigure || failure "Build did't fail when IDF_TARGET was set to an incompatible value in the environment"
    # Now make sdkconfig consistent with the environement (note: not really consistent, just for the purpose of the test)
    echo "CONFIG_IDF_TARGET=\"esp32\"" >> sdkconfig
    ! idf.py reconfigure || failure "Build did't fail when IDF_TARGET in CMakeCache.txt didn't match the environment"
    # Now unset IDF_TARGET in the environment, sdkconfig and CMakeCache.txt are still inconsistent
    unset IDF_TARGET
    ! idf.py reconfigure || failure "Build did't fail when IDF_TARGET in CMakeCache.txt didn't match the sdkconfig"

    unset other_target  # done changing target from the default
    clean_build_dir
    rm sdkconfig

    print_status "Setting EXTRA_COMPONENT_DIRS works"
    clean_build_dir
    (idf.py reconfigure | grep "$PWD/main") || failure  "Failed to verify original `main` directory"
    mkdir -p main/main/main # move main component contents to another directory
    mv main/* main/main/main
    cp CMakeLists.txt CMakeLists.bak # set EXTRA_COMPONENT_DIRS to point to the other directory
    ${SED} -i "s%cmake_minimum_required(VERSION \([0-9]\+\).\([0-9]\+\))%cmake_minimum_required(VERSION \1.\2)\nset(EXTRA_COMPONENT_DIRS main/main/main)%" CMakeLists.txt
    (idf.py reconfigure | grep "$PWD/main/main/main") || failure  "Failed to set EXTRA_COMPONENT_DIRS"
    mv CMakeLists.bak CMakeLists.txt # revert previous modifications
    mv main/main/main/* main
    rm -rf main/main

    print_status "Non-existent paths in EXTRA_COMPONENT_DIRS are not allowed"
    clean_build_dir
    ! idf.py -DEXTRA_COMPONENT_DIRS="extra_components" reconfigure || failure "Build should fail when non-existent component path is added"

    print_status "Component names may contain spaces"
    clean_build_dir
    mkdir -p "extra component"
    echo "idf_component_register" > "extra component/CMakeLists.txt"
    idf.py -DEXTRA_COMPONENT_DIRS="extra component;main" || failure "Build should succeed when a component name contains space"
    rm -rf "extra component"

    print_status "sdkconfig should have contents of all files: sdkconfig, sdkconfig.defaults, sdkconfig.defaults.IDF_TARGET"
    idf.py clean > /dev/null
    idf.py fullclean > /dev/null
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    echo "CONFIG_PARTITION_TABLE_OFFSET=0x10000" >> sdkconfig.defaults
    echo "CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_240=y" >> sdkconfig.defaults.esp32
    echo "CONFIG_PARTITION_TABLE_TWO_OTA=y" >> sdkconfig
    idf.py reconfigure > /dev/null
    grep "CONFIG_PARTITION_TABLE_OFFSET=0x10000" sdkconfig || failure "The define from sdkconfig.defaults should be into sdkconfig"
    grep "CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_240=y" sdkconfig || failure "The define from sdkconfig.defaults.esp32 should be into sdkconfig"
    grep "CONFIG_PARTITION_TABLE_TWO_OTA=y" sdkconfig || failure "The define from sdkconfig should be into sdkconfig"
    rm sdkconfig sdkconfig.defaults sdkconfig.defaults.esp32

    print_status "Test if it can build the example to run on host"
    pushd $IDF_PATH/examples/build_system/cmake/idf_as_lib
    (set -euo pipefail && source build.sh)
    popd
    rm -r $IDF_PATH/examples/build_system/cmake/idf_as_lib/build

    print_status "Test build ESP-IDF as a library to a custom CMake projects for all targets"
    IDF_AS_LIB=$IDF_PATH/examples/build_system/cmake/idf_as_lib
    # note: we just need to run cmake
    for TARGET in "esp32" "esp32s2" "esp32s3" "esp32c3" "esp32h2" "esp32c2"
    do
      echo "Build idf_as_lib for $TARGET target"
      rm -rf build
      mkdir -p build && cd build
      cmake $IDF_AS_LIB -DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-$TARGET.cmake -DTARGET=$TARGET || failure "Failed to generate idf_as_lib build files for target $TARGET"
      cmake --build . || failure "Failed to build idf_as_lib for target $TARGET"
      cd ..
    done

    print_status "Building a project with CMake library imported and PSRAM workaround, all files compile with workaround"
    # Test for libraries compiled within ESP-IDF
    rm -r build sdkconfig
    echo "CONFIG_SPIRAM=y" >> sdkconfig.defaults
    echo "CONFIG_SPIRAM_CACHE_WORKAROUND=y" >> sdkconfig.defaults
    # note: we do 'reconfigure' here, as we just need to run cmake
    idf.py -C $IDF_PATH/examples/build_system/cmake/import_lib -B `pwd`/build -D SDKCONFIG_DEFAULTS="`pwd`/sdkconfig.defaults" reconfigure
    grep -q '"command"' build/compile_commands.json || failure "compile_commands.json missing or has no no 'commands' in it"
    (grep '"command"' build/compile_commands.json | grep -v mfix-esp32-psram-cache-issue) && failure "All commands in compile_commands.json should use PSRAM cache workaround"
    rm -r build sdkconfig sdkconfig.defaults

    print_status "Test for external libraries in custom CMake projects with ESP-IDF components linked"
    mkdir build
    IDF_AS_LIB=$IDF_PATH/examples/build_system/cmake/idf_as_lib
    # note: we just need to run cmake
    (cd build && cmake $IDF_AS_LIB -DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-esp32.cmake -DTARGET=esp32)
    grep -q '"command"' build/compile_commands.json || failure "compile_commands.json missing or has no no 'commands' in it"

    for strat in MEMW NOPS DUPLDST; do
        print_status "Test for external libraries in custom CMake projects with PSRAM strategy $strat"
        rm -r build sdkconfig sdkconfig.defaults sdkconfig.defaults.esp32
        stratlc=`echo $strat | tr A-Z a-z`
        echo "CONFIG_SPIRAM=y" > sdkconfig.defaults
        echo "CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_$strat=y"  >> sdkconfig.defaults
        echo "CONFIG_SPIRAM_CACHE_WORKAROUND=y" >> sdkconfig.defaults
        # note: we do 'reconfigure' here, as we just need to run cmake
        idf.py reconfigure
        grep -q '"command"' build/compile_commands.json || failure "compile_commands.json missing or has no no 'commands' in it"
        (grep '"command"' build/compile_commands.json | grep -v mfix-esp32-psram-cache-strategy=$stratlc) && failure "All commands in compile_commands.json should use PSRAM cache workaround strategy"
        echo ${PWD}
        rm -r sdkconfig.defaults build
    done

    print_status "Cleaning Python bytecode"
    idf.py clean > /dev/null
    idf.py fullclean > /dev/null
    if [ "$(find $IDF_PATH -name "*.py[co]" | wc -l)" -eq 0 ]; then
        failure "No Python bytecode in IDF!"
    fi
    idf.py python-clean
    if [ "$(find $IDF_PATH -name "*.py[co]" | wc -l)" -gt 0 ]; then
        failure "Python bytecode isn't working!"
    fi

    print_status "Displays partition table when executing target partition_table"
    idf.py partition-table | grep -E "# ESP-IDF .+ Partition Table"
    rm -r build

    print_status "Make sure a full build never runs '/usr/bin/env python' or similar"
    OLDPATH="$PATH"
    PYTHON="$(which python)"
    rm -rf build
    cat > ./python << EOF
    #!/bin/sh
    echo "The build system has executed '/usr/bin/env python' or similar"
    exit 1
EOF
    chmod +x ./python
    export PATH="$(pwd):$PATH"
    ${PYTHON} $IDF_PATH/tools/idf.py build || failure "build failed"
    export PATH="$OLDPATH"
    rm ./python

    print_status "Handling deprecated Kconfig options"
    idf.py clean > /dev/null
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    echo "" > ${IDF_PATH}/sdkconfig.rename
    idf.py reconfigure > /dev/null
    echo "CONFIG_TEST_OLD_OPTION=y" >> sdkconfig
    echo "CONFIG_TEST_OLD_OPTION CONFIG_TEST_NEW_OPTION" >> ${IDF_PATH}/sdkconfig.rename
    echo -e "\n\
menu \"test\"\n\
    config TEST_NEW_OPTION\n\
        bool \"test\"\n\
        default \"n\"\n\
        help\n\
            TEST_NEW_OPTION description\n\
endmenu\n" >> ${IDF_PATH}/Kconfig
    idf.py reconfigure > /dev/null
    grep "CONFIG_TEST_OLD_OPTION=y" sdkconfig || failure "CONFIG_TEST_OLD_OPTION should be in sdkconfig for backward compatibility"
    grep "CONFIG_TEST_NEW_OPTION=y" sdkconfig || failure "CONFIG_TEST_NEW_OPTION should be now in sdkconfig"
    grep "#define CONFIG_TEST_NEW_OPTION 1" build/config/sdkconfig.h || failure "sdkconfig.h should contain the new macro"
    grep "#define CONFIG_TEST_OLD_OPTION CONFIG_TEST_NEW_OPTION" build/config/sdkconfig.h || failure "sdkconfig.h should contain the compatibility macro"
    grep "set(CONFIG_TEST_OLD_OPTION \"y\")" build/config/sdkconfig.cmake || failure "CONFIG_TEST_OLD_OPTION should be in auto.conf for backward compatibility"
    grep "set(CONFIG_TEST_NEW_OPTION \"y\")" build/config/sdkconfig.cmake || failure "CONFIG_TEST_NEW_OPTION should be now in auto.conf"
    rm -f sdkconfig sdkconfig.defaults
    pushd ${IDF_PATH}
    git checkout -- sdkconfig.rename Kconfig
    popd

    print_status "Handling deprecated Kconfig options in sdkconfig.defaults"
    idf.py clean
    rm -f sdkconfig
    echo "CONFIG_TEST_OLD_OPTION=7" > sdkconfig.defaults
    echo "CONFIG_TEST_OLD_OPTION CONFIG_TEST_NEW_OPTION" > ${IDF_PATH}/sdkconfig.rename
    echo -e "\n\
menu \"test\"\n\
    config TEST_NEW_OPTION\n\
        int \"TEST_NEW_OPTION\"\n\
        range 0 10\n\
        default 5\n\
        help\n\
            TEST_NEW_OPTION description\n\
endmenu\n" >> ${IDF_PATH}/Kconfig
    idf.py reconfigure > /dev/null
    grep "CONFIG_TEST_OLD_OPTION=7" sdkconfig || failure "CONFIG_TEST_OLD_OPTION=7 should be in sdkconfig for backward compatibility"
    grep "CONFIG_TEST_NEW_OPTION=7" sdkconfig || failure "CONFIG_TEST_NEW_OPTION=7 should be in sdkconfig"
    rm -f sdkconfig.defaults
    pushd ${IDF_PATH}
    git checkout -- sdkconfig.rename Kconfig
    popd

    echo "Can have multiple deprecated Kconfig options map to a single new option"
    idf.py clean > /dev/null
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    echo "" > ${IDF_PATH}/sdkconfig.rename
    idf.py reconfigure > /dev/null
    echo "CONFIG_TEST_NEW_OPTION=y" >> sdkconfig
    echo "CONFIG_TEST_OLD_OPTION_1 CONFIG_TEST_NEW_OPTION" >> ${IDF_PATH}/sdkconfig.rename
    echo "CONFIG_TEST_OLD_OPTION_2 CONFIG_TEST_NEW_OPTION" >> ${IDF_PATH}/sdkconfig.rename
    echo -e "\n\
    menu \"test\"\n\
    config TEST_NEW_OPTION\n\
        bool \"test\"\n\
        default \"n\"\n\
        help\n\
            TEST_NEW_OPTION description\n\
    endmenu\n" >> ${IDF_PATH}/Kconfig
    idf.py reconfigure > /dev/null
    grep "CONFIG_TEST_OLD_OPTION_1=y" sdkconfig || failure "CONFIG_TEST_OLD_OPTION_1 should be in sdkconfig for backward compatibility"
    grep "CONFIG_TEST_OLD_OPTION_2=y" sdkconfig || failure "CONFIG_TEST_OLD_OPTION_2 should be in sdkconfig for backward compatibility"
    grep "#define CONFIG_TEST_OLD_OPTION_1 CONFIG_TEST_NEW_OPTION" build/config/sdkconfig.h || failure "sdkconfig.h should contain the compatibility macro"
    grep "#define CONFIG_TEST_OLD_OPTION_2 CONFIG_TEST_NEW_OPTION" build/config/sdkconfig.h || failure "sdkconfig.h should contain the compatibility macro"
    grep "set(CONFIG_TEST_OLD_OPTION_1 \"y\")" build/config/sdkconfig.cmake || failure "CONFIG_TEST_OLD_OPTION_1 should be in auto.conf for backward compatibility"
    grep "set(CONFIG_TEST_OLD_OPTION_2 \"y\")" build/config/sdkconfig.cmake || failure "CONFIG_TEST_OLD_OPTION_2 should be in auto.conf for backward compatibility"
    rm -rf sdkconfig sdkconfig.defaults build
    pushd ${IDF_PATH}
    git checkout -- sdkconfig.rename Kconfig
    popd

    echo "Can have target specific deprecated Kconfig options"
    idf.py clean
    rm -f sdkconfig
    echo "CONFIG_TEST_OLD_OPTION=y" > sdkconfig
    echo "CONFIG_TEST_OLD_OPTION CONFIG_TEST_NEW_OPTION" >> ${IDF_PATH}/components/esp_system/sdkconfig.rename.esp32s2
    echo -e "\n\
    menu \"test\"\n\
    config TEST_NEW_OPTION\n\
        bool \"TEST_NEW_OPTION\"\n\
        default y\n\
        help\n\
            TEST_NEW_OPTION description\n\
    endmenu\n" >> ${IDF_PATH}/Kconfig
    idf.py set-target esp32 > /dev/null
    grep "CONFIG_TEST_OLD_OPTION=y" sdkconfig && failure "CONFIG_TEST_OLD_OPTION=y should NOT be in sdkconfig"
    grep "CONFIG_TEST_NEW_OPTION=y" sdkconfig || failure "CONFIG_TEST_NEW_OPTION=y should be in sdkconfig"
    rm -f sdkconfig
    idf.py set-target esp32s2 > /dev/null
    grep "CONFIG_TEST_OLD_OPTION=y" sdkconfig || failure "CONFIG_TEST_OLD_OPTION=y should be in esp32s2's sdkconfig for backward compatibility"
    grep "CONFIG_TEST_NEW_OPTION=y" sdkconfig || failure "CONFIG_TEST_NEW_OPTION=y should be in sdkconfig"
    rm -rf sdkconfig sdkconfig.defaults build
    pushd ${IDF_PATH}
    git checkout -- components/esp_system/sdkconfig.rename.esp32s2 Kconfig
    popd


    print_status "Confserver can be invoked by idf.py"
    echo '{"version": 1}' | idf.py confserver || failure "Couldn't load confserver"

    print_status "Check ccache is used to build"
    touch ccache && chmod +x ccache  # make sure that ccache is present for this test
    (export PATH=$PWD:$PATH && idf.py --ccache reconfigure | grep "ccache will be used") || failure "ccache should be used when --cache is specified"
    idf.py fullclean
    (export PATH=$PWD:$PATH && idf.py reconfigure| grep -c "ccache will be used" | grep -wq 0) \
        || failure "ccache should not be used even when present if --ccache is not specified"
    (export PATH=$PWD:$PATH && idf.py --no-ccache reconfigure| grep -c "ccache will be used" | grep -wq 0) \
        || failure "--no-ccache causes no issue for backward compatibility"
    rm -f ccache

    print_status "Custom bootloader overrides original"
    clean_build_dir
    (mkdir components && cd components && cp -r $IDF_PATH/components/bootloader .)
    idf.py bootloader
    grep "$PWD/components/bootloader/subproject/main/bootloader_start.c" build/bootloader/compile_commands.json \
        || failure "Custom bootloader source files should be built instead of the original's"
    rm -rf components

    print_status "Empty directory not treated as a component"
    clean_build_dir
    mkdir -p components/esp32 && idf.py reconfigure
    ! grep "$PWD/components/esp32"  $PWD/build/project_description.json || failure "Failed to build with empty esp32 directory in components"
    rm -rf components

    print_status "If a component directory is added to COMPONENT_DIRS, its subdirectories are not added"
    clean_build_dir
    mkdir -p main/test
    echo "idf_component_register()" > main/test/CMakeLists.txt
    idf.py reconfigure
    ! grep "$PWD/main/test" $PWD/build/project_description.json || failure "COMPONENT_DIRS has added component subdirectory to the build"
    grep "$PWD/main" $PWD/build/project_description.json || failure "COMPONENT_DIRS parent component directory should be included in the build"
    rm -rf main/test

    print_status "If a component directory is added to COMPONENT_DIRS, its sibling directories are not added"
    clean_build_dir
    mkdir -p mycomponents/mycomponent
    echo "idf_component_register()" > mycomponents/mycomponent/CMakeLists.txt
    # first test by adding single component directory to EXTRA_COMPONENT_DIRS
    mkdir -p mycomponents/esp32
    echo "idf_component_register()" > mycomponents/esp32/CMakeLists.txt
    idf.py -DEXTRA_COMPONENT_DIRS=$PWD/mycomponents/mycomponent reconfigure
    ! grep "$PWD/mycomponents/esp32" $PWD/build/project_description.json || failure "EXTRA_COMPONENT_DIRS has added a sibling directory"
    grep "$PWD/mycomponents/mycomponent" $PWD/build/project_description.json || failure "EXTRA_COMPONENT_DIRS valid sibling directory should be in the build"
    rm -rf mycomponents/esp32
    # now the same thing, but add a components directory
    mkdir -p esp32
    echo "idf_component_register()" > esp32/CMakeLists.txt
    idf.py -DEXTRA_COMPONENT_DIRS=$PWD/mycomponents reconfigure
    ! grep "$PWD/esp32" $PWD/build/project_description.json || failure "EXTRA_COMPONENT_DIRS has added a sibling directory"
    grep "$PWD/mycomponents/mycomponent" $PWD/build/project_description.json || failure "EXTRA_COMPONENT_DIRS valid sibling directory should be in the build"
    rm -rf esp32
    rm -rf mycomponents

    print_status "toolchain prefix is set in project description file"
    clean_build_dir
    idf.py reconfigure
    grep "prefix.*esp.*elf-" $PWD/build/project_description.json || failure "toolchain prefix not set or determined by CMake"

    # idf.py subcommand options, (using monitor with as example)
    print_status "Can set options to subcommands: print_filter for monitor"
    clean_build_dir
    mv ${IDF_PATH}/tools/idf_monitor.py ${IDF_PATH}/tools/idf_monitor.py.tmp
    echo "import sys;print(sys.argv[1:])" > ${IDF_PATH}/tools/idf_monitor.py
    idf.py build || failure "Failed to build project"
    idf.py monitor --print-filter="*:I" -p tty.fake | grep "'--print_filter', '\*:I'" || failure "It should process options for subcommands (and pass print-filter to idf_monitor.py)"
    mv ${IDF_PATH}/tools/idf_monitor.py.tmp ${IDF_PATH}/tools/idf_monitor.py

    print_status "Fail on build time works"
    clean_build_dir
    cp CMakeLists.txt CMakeLists.txt.bak
    printf "\nif(NOT EXISTS \"\${CMAKE_CURRENT_LIST_DIR}/hello.txt\") \nfail_at_build_time(test_file \"hello.txt does not exists\") \nendif()" >> CMakeLists.txt
    ! idf.py build || failure "Build should fail if requirements are not satisfied"
    touch hello.txt
    idf.py build || failure "Build succeeds once requirements are satisfied"
    rm -rf hello.txt CMakeLists.txt
    mv CMakeLists.txt.bak CMakeLists.txt
    rm -rf CMakeLists.txt.bak

    print_status "Component properties are set"
    clean_build_dir
    cp CMakeLists.txt CMakeLists.txt.bak
    printf "\nidf_component_get_property(srcs main SRCS)\nmessage(STATUS SRCS:\${srcs})" >> CMakeLists.txt
    (idf.py reconfigure | grep "SRCS:$(${REALPATH} main/main.c)") || failure "Component properties should be set"
    rm -rf CMakeLists.txt
    mv CMakeLists.txt.bak CMakeLists.txt
    rm -rf CMakeLists.txt.bak

    print_status "should be able to specify multiple sdkconfig default files"
    idf.py clean > /dev/null
    idf.py fullclean > /dev/null
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    echo "CONFIG_PARTITION_TABLE_OFFSET=0x10000" >> sdkconfig.defaults1
    echo "CONFIG_PARTITION_TABLE_TWO_OTA=y" >> sdkconfig.defaults2
    idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults1;sdkconfig.defaults2" reconfigure > /dev/null
    grep "CONFIG_PARTITION_TABLE_OFFSET=0x10000" sdkconfig || failure "The define from sdkconfig.defaults1 should be in sdkconfig"
    grep "CONFIG_PARTITION_TABLE_TWO_OTA=y" sdkconfig || failure "The define from sdkconfig.defaults2 should be in sdkconfig"
    rm sdkconfig.defaults1 sdkconfig.defaults2 sdkconfig
    git checkout sdkconfig.defaults

    print_status "Supports git worktree"
    clean_build_dir
    git branch test_build_system
    git worktree add ../esp-idf-template-test test_build_system
    diff <(idf.py reconfigure | grep "App \"app-template\" version: ") <(cd ../esp-idf-template-test && idf.py reconfigure | grep "App \"app-template\" version: ") \
        || failure "Version on worktree should have been properly resolved"
    git worktree remove ../esp-idf-template-test

    print_status "idf.py fallback to build system target"
    clean_build_dir
    msg="Custom target is running"
    echo "" >> CMakeLists.txt
    echo "add_custom_target(custom_target COMMAND \${CMAKE_COMMAND} -E echo \"${msg}\")" >> CMakeLists.txt
    idf.py custom_target 1>log.txt || failure "Could not invoke idf.py with custom target"
    grep "${msg}" log.txt 1>/dev/null || failure "Custom target did not produce expected output"
    git checkout CMakeLists.txt
    rm -f log.txt

    print_status "Build fails if partitions don't fit in flash"
    clean_build_dir
    sed -i.bak "s/CONFIG_ESPTOOLPY_FLASHSIZE.\+//" sdkconfig  # remove all flashsize config
    echo "CONFIG_ESPTOOLPY_FLASHSIZE_1MB=y" >> sdkconfig     # introduce undersize flash
    ( idf.py build 2>&1 | grep "does not fit in configured flash size 1MB" ) || failure "Build didn't fail with expected flash size failure message"
    mv sdkconfig.bak sdkconfig

    print_status "Warning is given if smallest partition is nearly full"
    clean_build_dir
    # Build a first time to get the binary size and to check that no warning is issued.
    ( idf.py build 2>&1 | grep "partition is nearly full" ) && failure "Warning for nearly full smallest partition was given when the condition is not fulfilled"
    # Get the size of the binary, in KB. Add 1 to the total.
    # The goal is to create an app partition which is slightly bigger than the binary itself
    get_file_size "build/app-template.bin"
    # Put the returned size ($BINSIZE) in a new variable, convert it to KB and add 1
    let size=${BINSIZE}/1024+1
    cp ${IDF_PATH}/components/partition_table/partitions_singleapp.csv partitions.csv
    ${SED} -i "s/factory,  app,  factory, ,        1M/factory,  app,  factory, ,        ${size}K/" partitions.csv
    echo "CONFIG_PARTITION_TABLE_CUSTOM=y" > sdkconfig
    # don't use FreeRTOS SMP build for this test - follow up ticket: IDF-5386
    echo "CONFIG_FREERTOS_SMP=n" >> sdkconfig
    ( idf.py build 2>&1 | grep "partition is nearly full" ) || failure "No warning for nearly full smallest partition was given when the condition is fulfilled"
    rm -f partitions.csv sdkconfig

    print_status "Flash size is correctly set in the bootloader image header"
    # Build with the default 2MB setting
    rm sdkconfig
    idf.py bootloader || failure "Failed to build bootloader"
    bin_header_match build/bootloader/bootloader.bin "0210"
    # Change to 4MB
    sleep 1 # delay here to make sure sdkconfig modification time is different
    echo "CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y" > sdkconfig
    idf.py bootloader || failure "Failed to build bootloader"
    bin_header_match build/bootloader/bootloader.bin "0220"
    # Change to QIO, bootloader should still be DIO (will change to QIO in 2nd stage bootloader)
    sleep 1 # delay here to make sure sdkconfig modification time is different
    echo "CONFIG_FLASHMODE_QIO=y" > sdkconfig
    idf.py bootloader || failure "Failed to build bootloader"
    bin_header_match build/bootloader/bootloader.bin "0210"
    # Change to 80 MHz
    sleep 1 # delay here to make sure sdkconfig modification time is different
    echo "CONFIG_ESPTOOLPY_FLASHFREQ_80M=y" > sdkconfig
    idf.py bootloader || failure "Failed to build bootloader"
    bin_header_match build/bootloader/bootloader.bin "021f"
    rm sdkconfig

    print_status "DFU build works"
    rm -f -r build sdkconfig
    idf.py dfu &> tmp.log
    grep "command \"dfu\" is not known to idf.py and is not a Ninja target" tmp.log || (tail -n 100 tmp.log ; failure "DFU build should fail for default chip target")
    idf.py set-target esp32s2
    idf.py dfu &> tmp.log
    grep "build/dfu.bin\" has been written. You may proceed with DFU flashing." tmp.log || (tail -n 100 tmp.log ; failure "DFU build should succeed for esp32s2")
    rm tmp.log
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN} "dfu.bin"
    rm -rf build sdkconfig

    print_status "UF2 build works"
    rm -f -r build sdkconfig
    idf.py uf2 &> tmp.log
    grep "build/uf2.bin\" has been written." tmp.log || (tail -n 100 tmp.log ; failure "UF2 build works for esp32")
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN} "uf2.bin"
    idf.py uf2-app &> tmp.log
    grep "build/uf2-app.bin\" has been written." tmp.log || (tail -n 100 tmp.log ; failure "UF2 build works for application binary")
    assert_built "uf2-app.bin"
    idf.py set-target esp32s2
    idf.py uf2 &> tmp.log
    grep "build/uf2.bin\" has been written." tmp.log || (tail -n 100 tmp.log ; failure "UF2 build works for esp32s2")
    rm tmp.log
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN} "uf2.bin"
    rm -rf build sdkconfig

    print_status "Loadable ELF build works"
    echo "CONFIG_APP_BUILD_TYPE_ELF_RAM=y" > sdkconfig

    # Set recommend configs to reduce memory footprint
    echo "CONFIG_VFS_SUPPORT_TERMIOS=n" >> sdkconfig
    echo "CONFIG_NEWLIB_NANO_FORMAT=y" >> sdkconfig
    echo "CONFIG_ESP_SYSTEM_PANIC_PRINT_HALT=y" >> sdkconfig
    echo "CONFIG_ESP_ERR_TO_NAME_LOOKUP=n" >> sdkconfig

    idf.py reconfigure || failure "Couldn't configure for loadable ELF file"
    test -f build/flasher_args.json && failure "flasher_args.json should not be generated in a loadable ELF build"
    idf.py build || failure "Couldn't build a loadable ELF file"

    print_status "Defaults set properly for unspecified idf_build_process args"
    pushd $IDF_PATH/examples/build_system/cmake/idf_as_lib
    cp CMakeLists.txt CMakeLists.txt.bak
    echo -e "\nidf_build_get_property(project_dir PROJECT_DIR)" >> CMakeLists.txt
    echo -e "\nmessage(\"Project directory: \${project_dir}\")" >> CMakeLists.txt
    mkdir build && cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-esp32.cmake -DTARGET=esp32 &> log.txt
    grep "Project directory: $IDF_PATH/examples/build_system/cmake/idf_as_lib" log.txt || failure "PROJECT_DIR default was not set"
    cd ..
    mv CMakeLists.txt.bak CMakeLists.txt
    rm -rf build
    popd

    print_status "Getting component overriden dir"
    clean_build_dir
    mkdir -p components/hal
    echo "idf_component_get_property(overriden_dir \${COMPONENT_NAME} COMPONENT_OVERRIDEN_DIR)" >> components/hal/CMakeLists.txt
    echo "message(STATUS overriden_dir:\${overriden_dir})" >> components/hal/CMakeLists.txt
    (idf.py reconfigure | grep "overriden_dir:$IDF_PATH/components/hal") || failure  "Failed to get overriden dir" # no registration, overrides registration as well
    print_status "Overriding Kconfig"
    echo "idf_component_register(KCONFIG \${overriden_dir}/Kconfig)" >> components/hal/CMakeLists.txt
    echo "idf_component_get_property(kconfig \${COMPONENT_NAME} KCONFIG)" >> components/hal/CMakeLists.txt
    echo "message(STATUS kconfig:\${overriden_dir}/Kconfig)" >> components/hal/CMakeLists.txt
    (idf.py reconfigure | grep "kconfig:$IDF_PATH/components/hal/Kconfig") || failure  "Failed to verify original `main` directory"
    rm -rf components

    print_status "Project components prioritized over EXTRA_COMPONENT_DIRS"
    clean_build_dir
    mkdir -p extra_dir/my_component
    echo "idf_component_register()" > extra_dir/my_component/CMakeLists.txt
    cp CMakeLists.txt CMakeLists.bak # set EXTRA_COMPONENT_DIRS to point to the other directory
    ${SED} -i "s%cmake_minimum_required(VERSION \([0-9]\+\).\([0-9]\+\))%cmake_minimum_required(VERSION \1.\2)\nset(EXTRA_COMPONENT_DIRS extra_dir)%" CMakeLists.txt
    (idf.py reconfigure | grep "$PWD/extra_dir/my_component") || failure  "Unable to find component specified in EXTRA_COMPONENT_DIRS"
    mkdir -p components/my_component
    echo "idf_component_register()" > components/my_component/CMakeLists.txt
    (idf.py reconfigure | grep "$PWD/components/my_component") || failure  "Project components should be prioritized over EXTRA_COMPONENT_DIRS"
    mv CMakeLists.bak CMakeLists.txt # revert previous modifications
    rm -rf extra_dir components

    print_status "Components in EXCLUDE_COMPONENTS not passed to idf_component_manager"
    clean_build_dir
    idf.py create-component -C components/ to_be_excluded || failure "Failed to create a component"
    echo "invalid syntax..." > components/to_be_excluded/idf_component.yml
    ! idf.py reconfigure || failure "Build should have failed due to invalid syntax in idf_component.yml"
    idf.py -DEXCLUDE_COMPONENTS=to_be_excluded reconfigure || failure "Build should have succeeded when the component is excluded"
    rm -rf components/to_be_excluded

    print_status "Create project using idf.py and build it"
    echo "Trying to create project."
    (idf.py -C projects create-project temp_test_project) || failure "Failed to create the project."
    cd "$IDF_PATH/projects/temp_test_project"
    echo "Building the project temp_test_project . . ."
    idf.py build || failure "Failed to build the project."
    cd "$IDF_PATH"
    rm -rf "$IDF_PATH/projects/temp_test_project"

    print_status "Create component using idf.py, create project using idf.py."
    print_status "Add the component to the created project and build the project."
    echo "Trying to create project . . ."
    (idf.py -C projects create-project temp_test_project) || failure "Failed to create the project."
    echo "Trying to create component . . ."
    (idf.py -C components create-component temp_test_component) || failure "Failed to create the component."
    ${SED} -i '5i\\tfunc();' "$IDF_PATH/projects/temp_test_project/main/temp_test_project.c"
    ${SED} -i '5i#include "temp_test_component.h"' "$IDF_PATH/projects/temp_test_project/main/temp_test_project.c"
    cd "$IDF_PATH/projects/temp_test_project"
    idf.py build || failure "Failed to build the project."
    cd "$IDF_PATH"
    rm -rf "$IDF_PATH/projects/temp_test_project"
    rm -rf "$IDF_PATH/components/temp_test_component"

    print_status "Check that command for creating new project will fail if the target folder is not empty."
    mkdir "$IDF_PATH/example_proj/"
    touch "$IDF_PATH/example_proj/tmp_130698"
    EXPECTED_EXIT_VALUE=3
    expected_failure $EXPECTED_EXIT_VALUE idf.py create-project --path "$IDF_PATH/example_proj/" temp_test_project  || failure "Command exit value is wrong."
    rm -rf "$IDF_PATH/example_proj/"

    print_status "Check that command for creating new project will fail if the target path is file."
    touch "$IDF_PATH/example_proj"
    EXPECTED_EXIT_VALUE=4
    expected_failure $EXPECTED_EXIT_VALUE idf.py create-project --path "$IDF_PATH/example_proj" temp_test_project || failure "Command exit value is wrong."
    rm -rf "$IDF_PATH/example_proj"

    print_status "Check docs command"
    clean_build_dir
    idf.py build
    idf.py set-target esp32s2
    idf.py docs || failure "'idf.py docs' failed"
    idf.py docs --no-browser | grep "https://docs.espressif.com/projects/esp-idf/en" || failure "'idf.py docs --no-browser' failed"
    idf.py docs --no-browser --language en | grep "https://docs.espressif.com/projects/esp-idf/en" || failure "'idf.py docs --no-browser --language en' failed"
    idf.py docs --no-browser --language en --version v4.2.1 | grep "https://docs.espressif.com/projects/esp-idf/en/v4.2.1" || failure "'idf.py docs --no-browser --language en --version v4.2.1' failed"
    idf.py docs --no-browser --language en --version v4.2.1 --target esp32 | grep "https://docs.espressif.com/projects/esp-idf/en/v4.2.1/esp32" || failure "'idf.py docs --no-browser --language en --version v4.2.1 --target esp32' failed"
    idf.py docs --no-browser --language en --version v4.2.1 --target esp32 --starting-page get-started | grep "https://docs.espressif.com/projects/esp-idf/en/v4.2.1/esp32/get-started" || failure "'idf.py docs --no-browser --language en --version v4.2.1 --target esp32 --starting-page get-started' failed"

    print_status "Deprecation warning check"
    cd ${TESTDIR}/template
    # click warning
    idf.py post_debug &> tmp.log
    grep "Error: Command \"post_debug\" is deprecated since v4.4 and was removed in v5.0." tmp.log || failure "Missing deprecation warning with command \"post_debug\""
    rm tmp.log
    # cmake warning
    idf.py efuse_common_table &> tmp.log
    grep "Have you wanted to run \"efuse-common-table\" instead?" tmp.log || failure "Missing deprecation warning with command \"efuse_common_table\""
    rm tmp.log

    print_status "Save-defconfig checks"
    cd ${TESTDIR}/template
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    idf.py fullclean > /dev/null
    echo "CONFIG_COMPILER_OPTIMIZATION_SIZE=y" >> sdkconfig
    echo "CONFIG_ESPTOOLPY_FLASHFREQ_80M=y" >> sdkconfig
    idf.py save-defconfig
    wc -l sdkconfig.defaults
    grep "CONFIG_IDF_TARGET" sdkconfig.defaults && failure "CONFIG_IDF_TARGET should not be in sdkconfig.defaults"
    grep "CONFIG_COMPILER_OPTIMIZATION_SIZE=y" sdkconfig.defaults || failure "Missing CONFIG_COMPILER_OPTIMIZATION_SIZE=y in sdkconfig.defaults"
    grep "CONFIG_ESPTOOLPY_FLASHFREQ_80M=y" sdkconfig.defaults || failure "Missing CONFIG_ESPTOOLPY_FLASHFREQ_80M=y in sdkconfig.defaults"
    idf.py fullclean > /dev/null
    rm -f sdkconfig.defaults
    rm -f sdkconfig
    idf.py set-target esp32c3
    echo "CONFIG_PARTITION_TABLE_OFFSET=0x8001" >> sdkconfig
    idf.py save-defconfig
    wc -l sdkconfig.defaults
    grep "CONFIG_IDF_TARGET=\"esp32c3\"" sdkconfig.defaults || failure "Missing CONFIG_IDF_TARGET=\"esp32c3\" in sdkconfig.defaults"
    grep "CONFIG_PARTITION_TABLE_OFFSET=0x8001" sdkconfig.defaults || failure "Missing CONFIG_PARTITION_TABLE_OFFSET=0x8001 in sdkconfig.defaults"
    idf.py fullclean > /dev/null
    rm -f sdkconfig.defaults
    rm -f sdkconfig

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

function expected_failure() {
    "${@:2}"
    EXIT_VALUE=$?
    if [ $EXIT_VALUE != "$1" ]; then
        echo "[ERROR] Exit value of executed command is $EXIT_VALUE (expected $1)"; return 1
    else return 0
    fi
}

TESTDIR=${PWD}/build_system_tests_$$
mkdir -p ${TESTDIR}
# set NOCLEANUP=1 if you want to keep the test directory around
# for post-mortem debugging
[ -z ${NOCLEANUP} ] && trap "rm -rf ${TESTDIR}" EXIT KILL

SNAPSHOT=${TESTDIR}/snapshot
BUILD=${TESTDIR}/template/build

IS_DARWIN=
export SED=sed
export REALPATH=realpath
if [ "$(uname -s)" = "Darwin" ]; then
    IS_DARWIN=1
    export SED=gsed
    export REALPATH=grealpath
fi

# copy all the build output to a snapshot directory
function take_build_snapshot()
{
    rm -rf ${SNAPSHOT}
    cp -ap ${TESTDIR}/template/build ${SNAPSHOT}
    if [ -n "$IS_DARWIN" ]; then
        # wait at least 1 second before the next build, for the test in
        # file_was_rebuilt to work
        sleep 1
    fi
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
    if [ -z "$IS_DARWIN" ]; then
        # can't use [ a -ot b ] here as -ot only gives second resolution
        # but stat -c %y seems to be microsecond at least for tmpfs, ext4..
        if [ "$(stat -c %y ${SNAPSHOT}/$1)" != "$(stat -c %y ${BUILD}/$1)" ]; then
            return 0
        else
            return 1
        fi
    else
        # macOS: work around 1-second resolution by adding a sleep in take_build_snapshot
        if [ ${SNAPSHOT}/$1 -ot ${BUILD}/$1 ]; then
            return 0
        else
            return 1
        fi
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
    PRESERVE_ROOT_ARG=
    if [ -z "$IS_DARWIN" ]; then
        PRESERVE_ROOT_ARG=--preserve-root
    fi
    rm -rf $PRESERVE_ROOT_ARG ${BUILD}/* ${BUILD}/.*
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
