# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

#[[
    __init_project_name()

    Initialize the PROJECT_NAME build property, using CMAKE_PROJECT_NAME as a
    fallback.
#]]
function(__init_project_name)
    __get_default_value(VARIABLE PROJECT_NAME
                        DEFAULT "${CMAKE_PROJECT_NAME}"
                        OUTPUT project_name)
    idf_build_set_property(PROJECT_NAME "${project_name}")
endfunction()

#[[
    __init_project_version()

    Initialize the PROJECT_VER build property based on the following
    precedence.

    1. The PROJECT_VER environment or CMake variable.
    2. The version.txt file located in the top-level project directory.
    3. The VERSION argument, if provided, in the project() macro.
    4. The output of git describe if the project is within a Git repository.
    5. Defaults to 1 if none of the above conditions are met.

    The value of PROJECT_VER will be overridden later if
    CONFIG_APP_PROJECT_VER_FROM_CONFIG is defined. For more details, refer to
    components/esp_app_format/CMakeLists.txt.
#]]
function(__init_project_version)
    idf_build_get_property(project_dir PROJECT_DIR)

    # 1. The PROJECT_VER environment or CMake variable.
    __get_default_value(VARIABLE PROJECT_VER
                        DEFAULT NOTFOUND
                        OUTPUT project_ver)
    if(project_ver)
        idf_build_set_property(PROJECT_VER "${project_ver}")
        return()
    endif()

    # 2. The version.txt file located in the top-level project directory.
    if(EXISTS "${project_dir}/version.txt")
        file(STRINGS "${project_dir}/version.txt" project_ver)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${project_dir}/version.txt")
        idf_build_set_property(PROJECT_VER "${project_ver}")
        return()
    endif()

    # 3. The VERSION argument, if provided, in the project() macro.
    if(PROJECT_VERSION)
        idf_build_set_property(PROJECT_VER "${PROJECT_VERSION}")
        return()
    endif()

    # 4. The output of git describe if the project is within a Git repository.
    git_describe(project_ver "${project_dir}")
    if(project_ver)
        idf_build_set_property(PROJECT_VER "${project_ver}")
        return()
    endif()

    # 5. Defaults to 1 if none of the above conditions are met.
    idf_build_set_property(PROJECT_VER 1)
endfunction()

#[[
    __init_project_configuration()

    Configure the build settings in one location, incorporating preset
    compilation flags, definitions, and settings based on sdkconfig.
#]]
function(__init_project_configuration)
    set(compile_definitions)
    set(compile_options)
    set(c_compile_options)
    set(cxx_compile_options)
    set(asm_compile_options)
    set(link_options)

    idf_build_get_property(idf_ver IDF_VER)
    idf_build_get_property(idf_target IDF_TARGET)
    idf_build_get_property(component_interfaces COMPONENT_INTERFACES)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(project_dir PROJECT_DIR)
    idf_build_get_property(project_name PROJECT_NAME)

    # Set the LINKER_TYPE build property. Different linkers may have varying
    # options, so it's important to identify the linker type to configure the
    # options correctly. Currently, LINKER_TYPE is used to set the appropriate
    # linker options for linking the entire archive, which differs between the
    # GNU and Apple linkers when building on the host.
    if(CONFIG_IDF_TARGET_LINUX AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        # Compiling for the host, and the host is macOS, so the linker is Darwin LD.
        # Note, when adding support for Clang and LLD based toolchain this check will
        # need to be modified.
        set(linker_type "Darwin")
    else()
        set(linker_type "GNU")
    endif()
    idf_build_set_property(LINKER_TYPE "${linker_type}")

    list(APPEND compile_definitions "_GLIBCXX_USE_POSIX_SEMAPHORE"  # These two lines enable libstd++ to use
                                    "_GLIBCXX_HAVE_POSIX_SEMAPHORE" # posix-semaphores from components/pthread
                                    "_GNU_SOURCE")

    list(APPEND compile_definitions "ESP_PLATFORM")
    list(APPEND compile_definitions "IDF_VER=\"${idf_ver}\"")

    list(APPEND compile_options     "-ffunction-sections"
                                    "-fdata-sections"
                                    # warning-related flags
                                    "-Wall"
                                    "-Wno-error=unused-function"
                                    "-Wno-error=unused-variable"
                                    "-Wno-error=unused-but-set-variable"
                                    "-Wno-error=deprecated-declarations"
                                    "-Wextra"
                                    "-Wno-error=extra"
                                    "-Wno-unused-parameter"
                                    "-Wno-sign-compare"
                                    # ignore multiple enum conversion warnings since gcc 11
                                    # TODO: IDF-5163
                                    "-Wno-enum-conversion"
                                    # Default is dwarf-5 since GCC 11, fallback to dwarf-4 because of binary size
                                    # TODO: IDF-5160
                                    "-gdwarf-4"
                                    # always generate debug symbols (even in release mode, these don't
                                    # go into the final binary so have no impact on size
                                    "-ggdb")

    if(CONFIG_COMPILER_DISABLE_DEFAULT_ERRORS AND NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
        list(APPEND compile_options "-Werror=all")
    else()
        list(APPEND compile_options "-Werror")
    endif()

    if(IDF_TARGET STREQUAL "linux")
        # Building for Linux target, fall back to an older version of the standard
        # if the preferred one is not supported by the compiler.
        set(preferred_c_versions gnu23 gnu17 gnu11 gnu99)
        set(ver_found FALSE)
        foreach(c_version ${preferred_c_versions})
            check_c_compiler_flag("-std=${c_version}" ver_${c_version}_supported)
            if(ver_${c_version}_supported)
                set(c_std ${c_version})
                set(ver_found TRUE)
                break()
            endif()
        endforeach()
        if(NOT ver_found)
            idf_die("Failed to set C language standard to one of the supported versions: "
                    "${preferred_c_versions}. Please upgrade the host compiler.")
        endif()

        set(preferred_cxx_versions gnu++26 gnu++2b gnu++20 gnu++2a gnu++17 gnu++14)
        set(ver_found FALSE)
        foreach(cxx_version ${preferred_cxx_versions})
            check_cxx_compiler_flag("-std=${cxx_version}" ver_${cxx_version}_supported)
            if(ver_${cxx_version}_supported)
                set(cxx_std ${cxx_version})
                set(ver_found TRUE)
                break()
            endif()
        endforeach()
        if(NOT ver_found)
            idf_die("Failed to set C++ language standard to one of the supported versions: "
                    "${preferred_cxx_versions}. Please upgrade the host compiler.")
        endif()

        list(APPEND c_compile_options   "-std=${c_std}")
        list(APPEND cxx_compile_options "-std=${cxx_std}")
    else()
        # Building for chip targets: we use a known version of the toolchain.
        # Use latest supported versions.
        # For Linux target -std settings, refer to the __linux_build_set_lang_version
        # function, which must be called after project().
        # Please update docs/en/api-guides/c.rst, docs/en/api-guides/cplusplus.rst and
        # tools/test_apps/system/cxx_build_test/main/test_cxx_standard.cpp when changing this.
        list(APPEND c_compile_options   "-std=gnu23")
        list(APPEND cxx_compile_options "-std=gnu++26")
    endif()

    if(CONFIG_COMPILER_OPTIMIZATION_SIZE)
        if(CMAKE_C_COMPILER_ID MATCHES "Clang")
            list(APPEND compile_options "-Oz")
        else()
            list(APPEND compile_options "-Os")
        endif()
        if(CMAKE_C_COMPILER_ID MATCHES "GNU")
            list(APPEND compile_options "-freorder-blocks")
        endif()
    elseif(CONFIG_COMPILER_OPTIMIZATION_DEBUG)
        list(APPEND compile_options "-Og")
        if(CMAKE_C_COMPILER_ID MATCHES "GNU" AND NOT CONFIG_IDF_TARGET_LINUX)
            list(APPEND compile_options "-fno-shrink-wrap")  # Disable shrink-wrapping to reduce binary size
        endif()
    elseif(CONFIG_COMPILER_OPTIMIZATION_NONE)
        list(APPEND compile_options "-O0")
    elseif(CONFIG_COMPILER_OPTIMIZATION_PERF)
        list(APPEND compile_options "-O2")
    endif()

    if(CONFIG_COMPILER_CXX_EXCEPTIONS)
        list(APPEND cxx_compile_options "-fexceptions")
    else()
        list(APPEND cxx_compile_options "-fno-exceptions")
    endif()

    if(CONFIG_COMPILER_CXX_RTTI)
        list(APPEND cxx_compile_options "-frtti")
    else()
        list(APPEND cxx_compile_options "-fno-rtti")
        list(APPEND link_options "-fno-rtti")  # used to invoke correct multilib variant (no-rtti) during linking
    endif()

    if(CONFIG_COMPILER_SAVE_RESTORE_LIBCALLS)
        list(APPEND compile_options "-msave-restore")
    endif()

    if(CMAKE_C_COMPILER_ID MATCHES "GNU")
        list(APPEND c_compile_options "-Wno-old-style-declaration")
    endif()

    # Clang finds some warnings in IDF code which GCC doesn't.
    # All these warnings should be fixed before Clang is presented
    # as a toolchain choice for users.
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        # Clang checks Doxygen comments for being in sync with function prototype.
        # There are some inconsistencies, especially in ROM headers.
        list(APPEND compile_options "-Wno-documentation")
        # GCC allows repeated typedefs when the source and target types are the same.
        # Clang doesn't allow this. This occurs in many components due to forward
        # declarations.
        list(APPEND compile_options "-Wno-typedef-redefinition")
        # This issue is seemingly related to newlib's char type functions.
        # Fix is not clear yet.
        list(APPEND compile_options "-Wno-char-subscripts")
        # Clang seems to notice format string issues which GCC doesn't.
        list(APPEND compile_options "-Wno-format-security")
        # Some pointer checks in mDNS component check addresses which can't be NULL
        list(APPEND compile_options "-Wno-tautological-pointer-compare")
        # Similar to the above, in tcp_transport
        list(APPEND compile_options "-Wno-pointer-bool-conversion")
        # mbedTLS md5.c triggers this warning in md5_test_buf (false positive)
        list(APPEND compile_options "-Wno-string-concatenation")
        # multiple cases of implicit conversions between unrelated enum types
        list(APPEND compile_options "-Wno-enum-conversion")
        # When IRAM_ATTR is specified both in function declaration and definition,
        # it produces different section names, since section names include __COUNTER__.
        # Occurs in multiple places.
        list(APPEND compile_options "-Wno-section")
        # Multiple cases of attributes unknown to clang, for example
        # __attribute__((optimize("-O3")))
        list(APPEND compile_options "-Wno-unknown-attributes")
        # Disable Clang warnings for atomic operations with access size
        # more then 4 bytes
        list(APPEND compile_options "-Wno-atomic-alignment")
        # several warnings in wpa_supplicant component
        list(APPEND compile_options "-Wno-unused-but-set-variable")
        # Clang also produces many -Wunused-function warnings which GCC doesn't.
        list(APPEND compile_options "-Wno-unused-function")
        # many warnings in bluedroid code
        # warning: field 'hdr' with variable sized type 'BT_HDR' not at the end of a struct or class is a GNU extension
        list(APPEND compile_options "-Wno-gnu-variable-sized-type-not-at-end")
        # several warnings in bluedroid code
        list(APPEND compile_options "-Wno-constant-logical-operand")
        # warning: '_Static_assert' with no message is a C2x extension
        list(APPEND compile_options "-Wno-c2x-extensions")
        # warning on xMPU_SETTINGS for esp32s2 has size 0 for C and 1 for C++
        list(APPEND compile_options "-Wno-extern-c-compat")
        if(NOT (CONFIG_IDF_TARGET_LINUX AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin"))
            # warning: implicit truncation from 'int' to a one-bit wide bit-field changes value from 1 to -1
            list(APPEND compile_options "-Wno-single-bit-bitfield-constant-conversion")
        endif()
    endif()
    # More warnings may exist in unit tests and example projects.

    if(CONFIG_COMPILER_WARN_WRITE_STRINGS)
        list(APPEND compile_options "-Wwrite-strings")
    endif()

    if(CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE)
        list(APPEND compile_definitions "-DNDEBUG")
    endif()

    if(CONFIG_COMPILER_NO_MERGE_CONSTANTS)
        list(APPEND compile_options "-fno-merge-constants")
    endif()

    if(CONFIG_COMPILER_STACK_CHECK_MODE_NORM)
        list(APPEND compile_options "-fstack-protector")
    elseif(CONFIG_COMPILER_STACK_CHECK_MODE_STRONG)
        list(APPEND compile_options "-fstack-protector-strong")
    elseif(CONFIG_COMPILER_STACK_CHECK_MODE_ALL)
        list(APPEND compile_options "-fstack-protector-all")
    endif()

    if(CONFIG_COMPILER_DUMP_RTL_FILES)
        list(APPEND compile_options "-fdump-rtl-expand")
    endif()

    if(CMAKE_C_COMPILER_ID MATCHES "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER 15.0)
        list(APPEND c_compile_options "-fzero-init-padding-bits=all" "-fno-malloc-dce")
    endif()

    if(CONFIG_COMPILER_DISABLE_GCC12_WARNINGS)
        list(APPEND compile_options "-Wno-address"
                                    "-Wno-use-after-free")
    endif()

    if(CONFIG_COMPILER_DISABLE_GCC13_WARNINGS)
        list(APPEND compile_options "-Wno-xor-used-as-pow")
        list(APPEND c_compile_options "-Wno-enum-int-mismatch")
        list(APPEND cxx_compile_options "-Wno-self-move"
                                        "-Wno-dangling-reference")
    endif()

    if(CONFIG_COMPILER_DISABLE_GCC14_WARNINGS)
        list(APPEND compile_options "-Wno-calloc-transposed-args")
    endif()

    if(CONFIG_COMPILER_DISABLE_GCC15_WARNINGS)
        list(APPEND c_compile_options "-Wno-unterminated-string-initialization")
        list(APPEND c_compile_options "-Wno-header-guard")
        list(APPEND cxx_compile_options "-Wno-self-move")
        list(APPEND cxx_compile_options "-Wno-template-body")
        list(APPEND cxx_compile_options "-Wno-dangling-reference")
        list(APPEND cxx_compile_options "-Wno-defaulted-function-deleted")
    endif()

    # GCC-specific options
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        list(APPEND compile_options "-fstrict-volatile-bitfields")
        if(CONFIG_COMPILER_STATIC_ANALYZER)
            list(APPEND compile_options "-fanalyzer")
        endif()
    endif()

    if(CONFIG_ESP_SYSTEM_USE_EH_FRAME)
      list(APPEND compile_options "-fasynchronous-unwind-tables")
      list(APPEND link_options "-Wl,--eh-frame-hdr")
    endif()

    if(CONFIG_ESP_SYSTEM_USE_FRAME_POINTER)
        list(APPEND compile_options "-fno-omit-frame-pointer")
        if(CMAKE_C_COMPILER_ID MATCHES "GNU")
            list(APPEND compile_options "-mno-omit-leaf-frame-pointer")
        endif()
    endif()

    list(APPEND link_options "-fno-lto")

    if(CONFIG_IDF_TARGET_LINUX AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        # Not all versions of the MacOS linker support the -warn_commons flag.
        # ld version 1053.12 (and above) have been tested to support it.
        # Hence, we extract the version string from the linker output
        # before including the flag.

        # Get the ld version, capturing both stdout and stderr
        execute_process(
            COMMAND ${CMAKE_LINKER} -v
            OUTPUT_VARIABLE LD_VERSION_OUTPUT
            ERROR_VARIABLE LD_VERSION_ERROR
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_STRIP_TRAILING_WHITESPACE
        )

        # Combine stdout and stderr
        set(LD_VERSION_OUTPUT "${LD_VERSION_OUTPUT}\n${LD_VERSION_ERROR}")

        # Extract the version string
        string(REGEX MATCH "PROJECT:(ld|dyld)-([0-9]+)\\.([0-9]+)" LD_VERSION_MATCH "${LD_VERSION_OUTPUT}")
        set(LD_VERSION_MAJOR_MINOR "${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")

        idf_msg("Linker Version: ${LD_VERSION_MAJOR_MINOR}")

        # Compare the version with 1053.12
        if(LD_VERSION_MAJOR_MINOR VERSION_GREATER_EQUAL "1053.12")
            list(APPEND link_options "-Wl,-warn_commons")
        endif()

        list(APPEND link_options "-Wl,-dead_strip")
    else()
        list(APPEND link_options "-Wl,--gc-sections")
        list(APPEND link_options "-Wl,--warn-common")
    endif()

    # SMP FreeRTOS user provided minimal idle hook. This allows the user to provide
    # their own copy of vApplicationPassiveIdleHook()
    if(CONFIG_FREERTOS_USE_PASSIVE_IDLE_HOOK)
        list(APPEND link_options "-Wl,--wrap=vApplicationPassiveIdleHook")
    endif()

    # Placing jump tables in flash would cause issues with code that required
    # to be placed in IRAM
    list(APPEND compile_options "-fno-jump-tables")
    if(CMAKE_C_COMPILER_ID MATCHES "GNU")
        # This flag is GCC-specific.
        # Not clear yet if some other flag should be used for Clang.
        list(APPEND compile_options "-fno-tree-switch-conversion")
    endif()

    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        list(APPEND compile_options "-fno-use-cxa-atexit")  # TODO IDF-10934
    else()
        list(APPEND cxx_compile_options "-fuse-cxa-atexit")
    endif()

    if(COMPILER_RT_LIB_NAME)
        list(APPEND link_options "-rtlib=${CONFIG_COMPILER_RT_LIB_NAME}")
    endif()

    if("${linker_type}" STREQUAL "GNU")
        set(target_upper "${idf_target}")
        string(TOUPPER ${target_upper} target_upper)
        # Add cross-reference table to the map file
        list(APPEND link_options "-Wl,--cref")
        # Add this symbol as a hint for esp_idf_size to guess the target name
        list(APPEND link_options "-Wl,--defsym=IDF_TARGET_${target_upper}=0")
        # Check if linker supports --no-warn-rwx-segments
        execute_process(COMMAND ${CMAKE_LINKER} "--no-warn-rwx-segments" "--version"
            RESULT_VARIABLE result
            OUTPUT_QUIET
            ERROR_QUIET)
        if(${result} EQUAL 0)
            # Do not print RWX segment warnings
            list(APPEND link_options "-Wl,--no-warn-rwx-segments")
        endif()
        if(CONFIG_COMPILER_ORPHAN_SECTIONS_WARNING)
            # Print warnings if orphan sections are found
            list(APPEND link_options "-Wl,--orphan-handling=warn")
        endif()
    endif()

    # Prepares the list of compiler flags for remapping various paths to fixed
    # names. This is used when reproducible builds are required.  This function
    # also creates a gdbinit file for the debugger to remap the substituted
    # paths back to the real paths in the filesystem.
    set(gdbinit_dir ${build_dir}/gdbinit)
    set(gdbinit_path "${gdbinit_dir}/prefix_map")

    if(CONFIG_COMPILER_HIDE_PATHS_MACROS)
        list(APPEND compile_options "-fmacro-prefix-map=${CMAKE_SOURCE_DIR}=.")
        list(APPEND compile_options "-fmacro-prefix-map=${idf_path}=/IDF")
    endif()

    if(CONFIG_APP_REPRODUCIBLE_BUILD)
        list(APPEND compile_options "-fdebug-prefix-map=${idf_path}=/IDF")
        list(APPEND compile_options "-fdebug-prefix-map=${project_dir}=/IDF_PROJECT")
        list(APPEND compile_options "-fdebug-prefix-map=${build_dir}=/IDF_BUILD")

        # Generate mapping for component paths
        set(gdbinit_file_lines)
        foreach(component_interface ${component_interfaces})
            __idf_component_get_property_unchecked(component_name ${component_interface} COMPONENT_NAME)
            __idf_component_get_property_unchecked(component_dir ${component_interface} COMPONENT_DIR)

            string(TOUPPER ${component_name} component_name_uppercase)
            set(substituted_path "/COMPONENT_${component_name_uppercase}_DIR")
            list(APPEND compile_options "-fdebug-prefix-map=${component_dir}=${substituted_path}")
            string(APPEND gdbinit_file_lines "set substitute-path ${substituted_path} ${component_dir}\n")
        endforeach()

        # Mapping for toolchain path
        execute_process(
            COMMAND ${CMAKE_C_COMPILER} -print-sysroot
            OUTPUT_VARIABLE compiler_sysroot
        )
        if(compiler_sysroot STREQUAL "")
            idf_die("Failed to determine toolchain sysroot")
        endif()
        string(STRIP "${compiler_sysroot}" compiler_sysroot)
        get_filename_component(compiler_sysroot "${compiler_sysroot}/.." REALPATH)
        list(APPEND compile_options "-fdebug-prefix-map=${compiler_sysroot}=/TOOLCHAIN")
        string(APPEND gdbinit_file_lines "set substitute-path /TOOLCHAIN ${compiler_sysroot}\n")
    else()
        set(gdbinit_file_lines "# There is no prefix map defined for the project.\n")
    endif()
    # Write the prefix_map file even if it is empty.
    file(MAKE_DIRECTORY ${gdbinit_dir})
    file(WRITE "${gdbinit_path}" "${gdbinit_file_lines}")

    idf_build_set_property(GDBINIT_FILES_PREFIX_MAP "${gdbinit_path}")
    idf_build_set_property(COMPILE_OPTIONS "${compile_options}" APPEND)
    idf_build_set_property(C_COMPILE_OPTIONS "${c_compile_options}" APPEND)
    idf_build_set_property(CXX_COMPILE_OPTIONS "${cxx_compile_options}" APPEND)
    idf_build_set_property(ASM_COMPILE_OPTIONS "${asm_compile_options}" APPEND)
    idf_build_set_property(COMPILE_DEFINITIONS "${compile_definitions}" APPEND)
    idf_build_set_property(LINK_OPTIONS "${link_options}" APPEND)
endfunction()

#[[
    __create_project_flash_targets()

    Add placeholder flash targets to the build. This is used by components to
    declare dependencies on the flash target.
#]]
function(__create_project_flash_targets)
    if(NOT TARGET flash)
        add_custom_target(flash)
    endif()

    # When flash encryption is enabled, a corresponding 'encrypted-flash' target will be created.
    idf_build_get_property(sdkconfig SDKCONFIG)
    __get_sdkconfig_option(OPTION CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT
                           SDKCONFIG "${sdkconfig}"
                           OUTPUT sdkconfig_target)
    if(encrypted_flash_enabled AND NOT TARGET encrypted-flash)
        add_custom_target(encrypted-flash)
    endif()
endfunction()

#[[
    __init_project_flash_targets()

    If binary generation is enabled, initialize the esptool component and
    enable the generation of esptool flash argument files for the flash and
    encrypted-flash targets. Note that this is done after including the
    project_include.cmake files, as we need the functions defined in the
    esptool_py component.
#]]
function(__init_project_flash_targets)
    if(CONFIG_APP_BUILD_GENERATE_BINARIES)
        idf_component_get_property(main_args esptool_py FLASH_ARGS)
        idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
        esptool_py_flash_target(flash "${main_args}" "${sub_args}")
    endif()
endfunction()

#[[
.. cmakev2:macro:: idf_project_init

    .. code-block:: cmake

        idf_project_init()

    Initialize settings that need to be configured after the ``project()``
    function is called. This must occur after the ``project()`` function and
    before any other build system functions. It initializes the
    ``PROJECT_NAME`` and ``PROJECT_VER`` build properties, as well as all
    default C, CXX, and ASM compile options, link options, and compile
    definitions.

    This macro also includes ``project_include.cmake`` files for the discovered
    components, as these files define project-wide functionality that needs to
    be available before any component's ``CMakeLists.txt`` is evaluated. The
    ``project_include.cmake`` files should be evaluated in the global scope.
    Therefore, this is defined as a macro and should be called only from the
    global scope or from within another macro.
#]]
macro(idf_project_init)
    idf_build_get_property(project_initialized __PROJECT_INITIALIZED)
    if(NOT project_initialized)
        # Ensure this function is executed only once throughout the entire
        # project.

        # Warn about the use of deprecated variables.
        deprecate_variable(COMPONENTS)
        deprecate_variable(EXCLUDE_COMPONENTS)

        # Set PROJECT_NAME build property
        __init_project_name()

        # Set PROJECT_VER build property
        __init_project_version()

        # Create global flash targets.
        __create_project_flash_targets()

        # Discover and initialize components
        __init_components()

        # Generate initial sdkconfig with discovered components
        __generate_sdkconfig()

        # Initialize the component manager and fetch components in a loop
        __fetch_components_from_registry()

        # Include sdkconfig.cmake
        idf_build_get_property(sdkconfig_cmake __SDKCONFIG_CMAKE)
        if(NOT EXISTS "${sdkconfig_cmake}")
            idf_die("sdkconfig.cmake file not found.")
        endif()
        include("${sdkconfig_cmake}")
        unset(sdkconfig_cmake)

        # Initialize all compilation options and defines.
        # This must be done after including sdkconfig.cmake
        __init_project_configuration()

        # Initialize the target architecture based on the configuration
        # Ensure this is done after including the sdkconfig.
        __init_idf_target_arch()

        # Include all project_include.cmake files for the components that have
        # been discovered.
        idf_build_get_property(component_interfaces COMPONENT_INTERFACES)
        foreach(component_interface IN LISTS component_interfaces)
            __idf_component_get_property_unchecked(project_include ${component_interface} __PROJECT_INCLUDE)
            __idf_component_get_property_unchecked(component_dir ${component_interface} COMPONENT_DIR)
            __idf_component_get_property_unchecked(component_name ${component_interface} COMPONENT_NAME)
            if(project_include)
                set(COMPONENT_NAME ${component_name})
                set(COMPONENT_DIR ${component_dir})
                # The use of COMPONENT_PATH is deprecated in cmakev1. Users
                # are encouraged to use COMPONENT_DIR instead.
                set(COMPONENT_PATH ${component_dir})
                idf_dbg("Including ${project_include}")
                include("${project_include}")
                unset(COMPONENT_NAME)
                unset(COMPONENT_DIR)
                unset(COMPONENT_PATH)
            endif()
        endforeach()

        # Initialize global flash targets.
        __init_project_flash_targets()

        # If explicitly requested, include all components by calling
        # `add_subdirectory` for every discovered component. The default
        # behavior is to include only the components based on the requirements.
        __get_default_value(VARIABLE IDF_INCLUDE_ALL_COMPONENTS
                            DEFAULT NO
                            OUTPUT include_all_components)
        if(include_all_components)
            idf_msg("Including all discovered components")
            foreach(component_name IN LISTS component_names)
                idf_component_include("${component_name}")
            endforeach()
        endif()
        unset(include_all_components)

        idf_build_set_property(__PROJECT_INITIALIZED YES)
    endif()
    unset(project_initialized)
endmacro()

#[[
.. cmakev2:function:: idf_build_generate_flasher_args

    .. code-block:: cmake

        idf_build_generate_flasher_args()

    Generate the flasher_args.json file for the global flash target for tools
    that require it.
#]]
function(idf_build_generate_flasher_args)
    # The variables listed below are used to configure the template
    # flasher_args.json.in. Some of these variables, such as flash mode, size,
    # and frequency, are set as properties of the esptool_py component.

    idf_build_get_property(target IDF_TARGET)
    set(ESPTOOLPY_CHIP "${target}")
    set(ESPTOOLPY_BEFORE "${CONFIG_ESPTOOLPY_BEFORE}")
    set(ESPTOOLPY_AFTER  "${CONFIG_ESPTOOLPY_AFTER}")
    if(CONFIG_ESPTOOLPY_NO_STUB)
        set(ESPTOOLPY_WITH_STUB false)
    else()
        set(ESPTOOLPY_WITH_STUB true)
    endif()

    if(CONFIG_SECURE_BOOT OR CONFIG_SECURE_FLASH_ENC_ENABLED)
        # If security enabled then override post flash option
        set(ESPTOOLPY_AFTER "no-reset")
    endif()

    idf_component_get_property(ESPFLASHMODE esptool_py ESPFLASHMODE)
    idf_component_get_property(ESPFLASHFREQ esptool_py ESPFLASHFREQ)
    idf_component_get_property(ESPFLASHSIZE esptool_py ESPFLASHSIZE)
    idf_component_get_property(esptool_py_dir esptool_py COMPONENT_DIR)

    # Generate flasher args files
    idf_build_get_property(build_dir BUILD_DIR)
    file(READ "${esptool_py_dir}/flasher_args.json.in" flasher_args_content)
    string(CONFIGURE "${flasher_args_content}" flasher_args_content)

    # We need to create a flasher_args.json.in to create the final flasher_args.json
    # because CMake only resolves generator expressions in the file_generate command
    # with the INPUT keyword during the generation phase.
    file_generate("${build_dir}/flasher_args.json.in"
                 CONTENT "${flasher_args_content}")
    file_generate("${build_dir}/flasher_args.json"
                 INPUT "${build_dir}/flasher_args.json.in")
endfunction()

#[[
.. cmakev2:macro:: __project_default

    .. code-block:: cmake

        __project_default()

    Helper function implementing the main idf_project_default macro
    functionality, preventing global variable scope pollution.
#]]
function(__project_default)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(executable PROJECT_NAME)
    idf_build_executable("${executable}"
                         COMPONENTS main
                         MAPFILE_TARGET "${executable}_mapfile")

    if(CONFIG_APP_BUILD_GENERATE_BINARIES)
        # Is it possible to have a configuration where
        # CONFIG_APP_BUILD_GENERATE_BINARIES is not set?

        if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
            idf_build_binary("${executable}"
                             OUTPUT_FILE "${build_dir}/${executable}-unsigned.bin"
                             TARGET "${executable}_binary_unsigned")
            idf_sign_binary("${executable}_binary_unsigned"
                             OUTPUT_FILE "${build_dir}/${executable}.bin"
                             TARGET "${executable}_binary_signed")
            idf_check_binary_size("${executable}_binary_signed")

            add_custom_target(app ALL DEPENDS "${executable}_binary_signed")

            idf_flash_binary("${executable}_binary_signed"
                             TARGET app-flash
                             NAME "app"
                             FLASH)
            idf_build_generate_metadata("${executable}_binary_signed")
        else()
            idf_build_binary("${executable}"
                             OUTPUT_FILE "${build_dir}/${executable}.bin"
                             TARGET "${executable}_binary")
            idf_check_binary_size("${executable}_binary")
            idf_check_binary_signed("${executable}_binary")

            add_custom_target(app ALL DEPENDS "${executable}_binary")

            idf_flash_binary("${executable}_binary"
                             TARGET app-flash
                             NAME "app"
                             FLASH)

            idf_create_dfu("${executable}_binary"
                           TARGET dfu)
            idf_build_generate_metadata("${executable}_binary")
        endif()

        idf_build_generate_flasher_args()
    endif()

    idf_create_menuconfig("${executable}"
                          TARGET menuconfig)

    idf_create_confserver("${executable}"
                          TARGET confserver)

    idf_create_save_defconfig()

    idf_create_uf2("${executable}"
                   TARGET uf2)
    idf_create_uf2("${executable}"
                   TARGET uf2-app
                   APP_ONLY)

    if(TARGET "${executable}_mapfile")
        idf_create_size_report("${executable}_mapfile"
                               TARGET size)
    endif()

    idf_build_generate_depgraph("${executable}")
endfunction()

#[[api
.. cmakev2:macro:: idf_project_default

    .. code-block:: cmake

        idf_project_default()

    Create a default project executable based on the main component and its
    transitive dependencies. The executable name is derived from the
    ``PROJECT_NAME`` variable, which by default uses the ``CMAKE_PROJECT_NAME``
    value specified in the CMake's ``project()`` call.

    Generate the binary image for the executable, signed or unsigned based on
    the configuration, and add flash targets for it.
#]]
macro(idf_project_default)
    idf_project_init()
    # Only the idf_project_init macro needs be called within the global scope,
    # as it includes the project_include.cmake files and the cmake version of
    # the configuration. The remaining functionality of the idf_project_default
    # macro is implemented in a __project_default helper function to avoid
    # polluting the global variable space.
    __project_default()
endmacro()
