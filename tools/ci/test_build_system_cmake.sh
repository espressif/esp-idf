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
    touch ${IDF_PATH}/components/esp32/cpu_start.c
    idf.py build || failure "Failed to partial build"
    assert_rebuilt ${APP_BINS} esp-idf/esp32/libesp32.a esp-idf/esp32/CMakeFiles/${IDF_COMPONENT_PREFIX}_esp32.dir/cpu_start.c.obj
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
    ALL_BUILD_FILES=$(find ${BUILD} -type f | sed "s@${BUILD}/@@" | grep -v '^.')
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
    assert_not_rebuilt ${BOOTLOADER_BINS} esp-idf/esp32/libesp32.a

    print_status "Re-building does not change app.bin"
    take_build_snapshot
    idf.py build
    assert_not_rebuilt ${APP_BINS} ${BOOTLOADER_BINS} esp-idf/esp32/libesp32.a
    rm -f ${IDF_PATH}/version.txt
    rm -f ${TESTDIR}/template/version.txt

    print_status "Get the version of app from git describe. Project is not inside IDF and do not have a tag only a hash commit."
    idf.py build >> log.log || failure "Failed to build"
    version="Project version: "
    version+=$(git describe --always --tags --dirty)
    grep "${version}" log.log || failure "Project version should have a hash commit"

    print_status "Can set COMPONENT_SRCS with spaces"
    clean_build_dir
    touch main/main2.c
    sed -i 's/^set(COMPONENT_SRCS.*/set(COMPONENT_SRCS "main.c main2.c")/' main/CMakeLists.txt
    idf.py build || failure "Set COMPONENT_SRCS with spaces build failed"
    git checkout -- main/CMakeLists.txt
    rm main/main2.c

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
    cp ${IDF_PATH}/components/esp_rom/esp32/ld/esp32.rom.ld .
    echo "/* (Build test comment) */" >> ${IDF_PATH}/components/esp_rom/esp32/ld/esp32.rom.ld
    tail ${IDF_PATH}/components/esp_rom/esp32/ld/esp32.rom.ld
    idf.py build || failure "Failed to rebuild with modified linker script"
    assert_rebuilt ${APP_BINS} ${BOOTLOADER_BINS}
    mv esp32.rom.ld ${IDF_PATH}/components/esp_rom/esp32/ld/

    print_status "Updating app-only ld file should only re-link app"
    take_build_snapshot
    cp ${IDF_PATH}/components/esp32/ld/esp32.project.ld.in .
    sleep 1  # ninja may ignore if the timestamp delta is too low
    echo "/* (Build test comment) */" >> ${IDF_PATH}/components/esp32/ld/esp32.project.ld.in
    idf.py build || failure "Failed to rebuild with modified linker script"
    assert_rebuilt ${APP_BINS}
    assert_not_rebuilt ${BOOTLOADER_BINS}
    mv esp32.project.ld.in ${IDF_PATH}/components/esp32/ld/

    print_status "Updating fragment file should only re-link app" # only app linker script is generated by tool for now
    take_build_snapshot
    cp ${IDF_PATH}/components/esp32/ld/esp32_fragments.lf .
    sleep 1  # ninja may ignore if the timestamp delta is too low
    echo "# (Build test comment)" >> ${IDF_PATH}/components/esp32/ld/esp32_fragments.lf
    idf.py build || failure "Failed to rebuild with modified linker fragment file"
    assert_rebuilt ${APP_BINS}
    assert_not_rebuilt ${BOOTLOADER_BINS}
    mv esp32_fragments.lf ${IDF_PATH}/components/esp32/ld/

    print_status "sdkconfig update triggers full recompile"
    clean_build_dir
    idf.py build
    take_build_snapshot
    # need to actually change config, or cmake is too smart to rebuild
    sed -i.bak s/^\#\ CONFIG_FREERTOS_UNICORE\ is\ not\ set/CONFIG_FREERTOS_UNICORE=y/ sdkconfig
    idf.py build
    # check the sdkconfig.h file was rebuilt
    assert_rebuilt config/sdkconfig.h
    # pick one each of .c, .cpp, .S that #includes sdkconfig.h
    # and therefore should rebuild
    assert_rebuilt esp-idf/newlib/CMakeFiles/${IDF_COMPONENT_PREFIX}_newlib.dir/syscall_table.c.obj
    assert_rebuilt esp-idf/nvs_flash/CMakeFiles/${IDF_COMPONENT_PREFIX}_nvs_flash.dir/src/nvs_api.cpp.obj
    assert_rebuilt esp-idf/freertos/CMakeFiles/${IDF_COMPONENT_PREFIX}_freertos.dir/xtensa_vectors.S.obj
    mv sdkconfig.bak sdkconfig

    print_status "Updating project CMakeLists.txt triggers full recompile"
    clean_build_dir
    idf.py build
    take_build_snapshot
    # Need to actually change the build config, or CMake won't do anything
    cp CMakeLists.txt CMakeLists.bak
    sed -i.bak 's/^project(/add_compile_options("-DUSELESS_MACRO_DOES_NOTHING=1")\nproject\(/' CMakeLists.txt
    idf.py build || failure "Build failed"
    mv CMakeLists.bak CMakeLists.txt
    # similar to previous test
    assert_rebuilt esp-idf/newlib/CMakeFiles/${IDF_COMPONENT_PREFIX}_newlib.dir/syscall_table.c.obj
    assert_rebuilt esp-idf/nvs_flash/CMakeFiles/${IDF_COMPONENT_PREFIX}_nvs_flash.dir/src/nvs_api.cpp.obj
    assert_rebuilt esp-idf/freertos/CMakeFiles/${IDF_COMPONENT_PREFIX}_freertos.dir/xtensa_vectors.S.obj
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

    print_status "Can build with IDF_PATH set via cmake cache not environment"
    clean_build_dir
    sed -i.bak 's/ENV{IDF_PATH}/{IDF_PATH}/' CMakeLists.txt
    export IDF_PATH_BACKUP="$IDF_PATH"
    (unset IDF_PATH &&
         cd build &&
         cmake -G Ninja .. -DIDF_PATH=${IDF_PATH_BACKUP} &&
         ninja) || failure "Ninja build failed"
    mv CMakeLists.txt.bak CMakeLists.txt
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Can build with IDF_PATH unset and inferred by build system"
    clean_build_dir
    sed -i.bak "s%\$ENV{IDF_PATH}%\${ci_idf_path}%" CMakeLists.txt  # expand to a hardcoded path
    (ci_idf_path=${IDF_PATH} && unset IDF_PATH && cd build &&
         cmake -G Ninja -D ci_idf_path=${ci_idf_path} .. && ninja) || failure "Ninja build failed"
    mv CMakeLists.txt.bak CMakeLists.txt
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Can build with IDF_PATH unset and inferred by cmake when Kconfig needs it to be set"
    clean_build_dir
    sed -i.bak 's/ENV{IDF_PATH}/{IDF_PATH}/' CMakeLists.txt
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


    # Next two tests will use this fake 'esp31b' target
    export fake_target=esp31b
    mkdir -p components/$fake_target
    mkdir -p ${IDF_PATH}/components/xtensa/$fake_target/include
    touch components/$fake_target/CMakeLists.txt
    cp ${IDF_PATH}/tools/cmake/toolchain-esp32.cmake components/$fake_target/toolchain-$fake_target.cmake
    sed -i.bak '/cmake_minimum_required/ a\
        set(COMPONENTS esptool_py)' CMakeLists.txt

    print_status "Can override IDF_TARGET from environment"
    clean_build_dir
    rm sdkconfig
    export IDF_TARGET=$fake_target
    (cd build && cmake -G Ninja .. ) || failure "Failed to configure with IDF_TARGET set in environment"
    grep "CONFIG_IDF_TARGET=\"${fake_target}\"" sdkconfig || failure "Project not configured for IDF_TARGET correctly"
    grep "IDF_TARGET:STRING=${fake_target}" build/CMakeCache.txt || failure "IDF_TARGET not set in CMakeCache.txt"
    unset IDF_TARGET

    print_status "Can set target using idf.py -D"
    clean_build_dir
    rm sdkconfig
    idf.py -DIDF_TARGET=$fake_target reconfigure || failure "Failed to set target via idf.py"
    grep "CONFIG_IDF_TARGET=\"${fake_target}\"" sdkconfig || failure "Project not configured correctly using idf.py -D"
    grep "IDF_TARGET:STRING=${fake_target}" build/CMakeCache.txt || failure "IDF_TARGET not set in CMakeCache.txt using idf.py -D"

    print_status "Can set target using -D as subcommand parameter for idf.py"
    clean_build_dir
    rm sdkconfig
    idf.py reconfigure -DIDF_TARGET=$fake_target || failure "Failed to set target via idf.py subcommand -D parameter"
    grep "CONFIG_IDF_TARGET=\"${fake_target}\"" sdkconfig || failure "Project not configured correctly using idf.py reconfigure -D"
    grep "IDF_TARGET:STRING=${fake_target}" build/CMakeCache.txt || failure "IDF_TARGET not set in CMakeCache.txt using idf.py reconfigure -D"

    # TODO Change the real target to other value than esp32 when we have
    real_target=esp32
    print_status "Can set target using idf.py set-target"
    clean_build_dir
    rm sdkconfig
    idf.py set-target esp32 || failure "Failed to set target via idf.py set-target"
    grep "CONFIG_IDF_TARGET=\"${real_target}\"" sdkconfig || failure "Project not configured correctly using idf.py set-target"
    grep "IDF_TARGET:STRING=${real_target}" build/CMakeCache.txt || failure "IDF_TARGET not set in CMakeCache.txt using idf.py set-target"

    # Clean up modifications for the fake target
    mv CMakeLists.txt.bak CMakeLists.txt
    rm -rf components

    print_status "Can find toolchain file in component directory"
    clean_build_dir
    mv ${IDF_PATH}/tools/cmake/toolchain-esp32.cmake ${IDF_PATH}/components/esp32/
    idf.py build || failure "Failed to build with toolchain file in component directory"
    mv ${IDF_PATH}/components/esp32/toolchain-esp32.cmake ${IDF_PATH}/tools/cmake/
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Can build with auto generated CMakeLists.txt"
    clean_build_dir
    mv CMakeLists.txt CMakeLists.bak
    ${IDF_PATH}/tools/cmake/convert_to_cmake.py .
    idf.py build || failure "Auto generated CMakeLists.txt build failed"
    mv CMakeLists.bak CMakeLists.txt
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN}

    print_status "Setting EXTRA_COMPONENT_DIRS works"
    clean_build_dir
    mkdir -p main/main/main # move main component contents to another directory
    mv main/* main/main/main
    cp CMakeLists.txt CMakeLists.bak # set EXTRA_COMPONENT_DIRS to point to the other directory
    sed -i "s%cmake_minimum_required(VERSION \([0-9]\+\).\([0-9]\+\))%cmake_minimum_required(VERSION \1.\2)\nset(EXTRA_COMPONENT_DIRS main/main/main)%" CMakeLists.txt
    idf.py build || failure "Build with EXTRA_COMPONENT_DIRS set failed"
    mv CMakeLists.bak CMakeLists.txt # revert previous modifications
    mv main/main/main/* main
    rm -rf main/main
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
    rm sdkconfig;
    rm sdkconfig.defaults;

    print_status "can build with phy_init_data"
    idf.py clean > /dev/null;
    idf.py fullclean > /dev/null;
    rm -f sdkconfig.defaults;
    rm -f sdkconfig;
    echo "CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION=y" >> sdkconfig.defaults;
    idf.py reconfigure > /dev/null;
    idf.py build || failure "Failed to build with PHY_INIT_DATA"
    assert_built ${APP_BINS} ${BOOTLOADER_BINS} ${PARTITION_BIN} ${PHY_INIT_BIN}
    rm sdkconfig;
    rm sdkconfig.defaults;

    print_status "Building a project with CMake library imported and PSRAM workaround, all files compile with workaround"
    # Test for libraries compiled within ESP-IDF
    rm -rf build
    echo "CONFIG_ESP32_SPIRAM_SUPPORT=y" >> sdkconfig.defaults
    echo "CONFIG_SPIRAM_CACHE_WORKAROUND=y" >> sdkconfig.defaults
    # note: we do 'reconfigure' here, as we just need to run cmake
    idf.py -C $IDF_PATH/examples/build_system/cmake/import_lib -B `pwd`/build -D SDKCONFIG_DEFAULTS="`pwd`/sdkconfig.defaults" reconfigure
    grep -q '"command"' build/compile_commands.json || failure "compile_commands.json missing or has no no 'commands' in it"
    (grep '"command"' build/compile_commands.json | grep -v mfix-esp32-psram-cache-issue) && failure "All commands in compile_commands.json should use PSRAM cache workaround"
    rm -r sdkconfig.defaults build
    # Test for external libraries in custom CMake projects with ESP-IDF components linked
    mkdir build && touch build/sdkconfig
    echo "CONFIG_ESP32_SPIRAM_SUPPORT=y" >> build/sdkconfig
    echo "CONFIG_SPIRAM_CACHE_WORKAROUND=y" >> build/sdkconfig
    # note: we just need to run cmake
    (cd build && cmake $IDF_PATH/examples/build_system/cmake/idf_as_lib -DCMAKE_TOOLCHAIN_FILE=$IDF_PATH/tools/cmake/toolchain-esp32.cmake -DTARGET=esp32)
    grep -q '"command"' build/compile_commands.json || failure "compile_commands.json missing or has no no 'commands' in it"
    (grep '"command"' build/compile_commands.json | grep -v mfix-esp32-psram-cache-issue) && failure "All commands in compile_commands.json should use PSRAM cache workaround"
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
    idf.py clean > /dev/null;
    rm -f sdkconfig.defaults;
    rm -f sdkconfig;
    echo "" > ${IDF_PATH}/sdkconfig.rename;
    idf.py build > /dev/null;
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
    idf.py build > /dev/null;
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
    idf.py build
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

    # idf.py global and subcommand parameters
    print_status "Cannot set -D twice: for command and subcommand of idf.py (with different values)"
    idf.py -DAAA=BBB build -DAAA=BBB -DCCC=EEE
    if [ $? -eq 0 ]; then
        failure "It shouldn't be allowed to set -D twice (globally and for subcommand) with different set of options"
    fi

    print_status "Can set -D twice: globally and for subcommand, only if values are the same"
    idf.py -DAAA=BBB -DCCC=EEE build -DAAA=BBB -DCCC=EEE || failure "It should be allowed to set -D twice (globally and for subcommand) if values are the same"

    # idf.py subcommand options, (using monitor with as example)
    print_status "Can set options to subcommands: print_filter for monitor"
    mv ${IDF_PATH}/tools/idf_monitor.py ${IDF_PATH}/tools/idf_monitor.py.tmp
    echo "import sys;print(sys.argv[1:])" > ${IDF_PATH}/tools/idf_monitor.py
    idf.py build || "Failed to build project"
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
    (idf.py reconfigure | grep "SRCS:$(realpath main/main.c)") || failure "Component properties should be set"
    rm -rf CMakeLists.txt
    mv CMakeLists.txt.bak CMakeLists.txt
    rm -rf CMakeLists.txt.bak

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
