# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(utilities)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

#[[api
.. cmakev2:function:: idf_build_set_property

   .. code-block:: cmake

      idf_build_set_property(<property> <value> [APPEND])

   :property[in]: Property name.
   :value[in]: Property value.
   :APPEND: Append the value to the property's current value instead of
            replacing it.

   Set the value of the specified property related to the ESP-IDF build. The
   property is also added to the internal list of build properties if it isn't
   already there.
#]]
function(idf_build_set_property property value)
    set(options APPEND)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    set(append)
    if(ARG_APPEND)
        set(append APPEND)
    endif()

    __set_property(TARGET idf_build_properties
                   PROPERTY "${property}"
                   PROPERTIES BUILD_PROPERTIES
                   VALUE "${value}"
                   ${append})
endfunction()

#[[api
.. cmakev2:function:: idf_build_get_property

   .. code-block:: cmake

      idf_build_get_property(<var> <property> [GENERATOR_EXPRESSION])

   :variable[out]: Variable to store the value in.
   :property[in]: Property name to get the value of.
   :GENERATOR_EXPRESSION: Obtain the generator expression for the property
                          rather than the actual value.

   Get the value of the specified property related to the ESP-IDF build.
#]]
function(idf_build_get_property variable property)
    set(options GENERATOR_EXPRESSION)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if("${property}" STREQUAL BUILD_COMPONENTS)
        idf_die("Build property 'BUILD_COMPONENTS' is not supported")
    endif()

    set(genexpr)
    if(ARG_GENERATOR_EXPRESSION)
        set(genexpr GENERATOR_EXPRESSION)
    endif()

    __get_property(TARGET idf_build_properties
                   PROPERTY "${property}"
                   OUTPUT value
                   ${genexpr})
    set(${variable} ${value} PARENT_SCOPE)
endfunction()

#[[
   __dump_build_properties()

   Dump all build properties.
#]]
function(__dump_build_properties)
    idf_build_get_property(properties BUILD_PROPERTIES)
    idf_msg("build properties: ${properties}")
    foreach(property IN LISTS properties)
        idf_build_get_property(value ${property})
        idf_msg("   ${property}: ${value}")
    endforeach()
endfunction()

#[[
   __init_build_configuration()

   Configure the build settings in one location, incorporating preset
   compilation flags, definitions, and settings based on sdkconfig.
#]]
function(__init_build_configuration)
    set(compile_definitions)
    set(compile_options)
    set(c_compile_options)
    set(cxx_compile_options)
    set(asm_compile_options)
    set(link_options)

    idf_build_get_property(idf_ver IDF_VER)
    idf_build_get_property(idf_target IDF_TARGET)
    idf_build_get_property(components_discovered COMPONENTS_DISCOVERED)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(project_dir PROJECT_DIR)

    list(APPEND compile_definitions "_GLIBCXX_USE_POSIX_SEMAPHORE"  # These two lines enable libstd++ to use
                                    "_GLIBCXX_HAVE_POSIX_SEMAPHORE" # posix-semaphores from components/pthread
                                    "_GNU_SOURCE")

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
        set(preferred_c_versions gnu17 gnu11 gnu99)
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

        set(preferred_cxx_versions gnu++2b gnu++20 gnu++2a gnu++17 gnu++14)
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
        list(APPEND c_compile_options   "-std=gnu17")
        list(APPEND cxx_compile_options "-std=gnu++2b")
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

    if(CONFIG_LIBC_PICOLIBC)
        list(APPEND c_compile_options "-specs=picolibc.specs")
        list(APPEND cxx_compile_options "-specs=picolibcpp.specs")
        list(APPEND link_options "-specs=picolibc.specs")
    endif()

    if(CMAKE_C_COMPILER_ID MATCHES "GNU")
        set(mapfile "${build_dir}/${CMAKE_PROJECT_NAME}.map")
        set(target_upper "${idf_target}")
        string(TOUPPER ${target_upper} target_upper)
        # Add cross-reference table to the map file
        list(APPEND link_options "-Wl,--cref")
        # Add this symbol as a hint for esp_idf_size to guess the target name
        list(APPEND link_options "-Wl,--defsym=IDF_TARGET_${target_upper}=0")
        # Enable map file output
        list(APPEND link_options "-Wl,--Map=${mapfile}")
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
        foreach(component_name ${components_discovered})
            idf_component_get_property(component_dir ${component_name} COMPONENT_DIR)

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

        file(WRITE "${build_dir}/prefix_map_gdbinit" "${gdbinit_file_lines}")  # TODO IDF-11667
        idf_build_set_property(DEBUG_PREFIX_MAP_GDBINIT "${gdbinit_path}")
    else()
        set(gdbinit_file_lines "# There is no prefix map defined for the project.\n")
    endif()
    # Write prefix_map_gdbinit file even it is empty.
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
   __init_build()

   Initialize settings that need to be configured after the project() function
   is called. This primarily involves setting compilation options, define
   settings, and configuring linker options.
#]]
function(__init_build)
    idf_build_get_property(build_initialized __BUILD_INITIALIZED)
    if(build_initialized)
        # Ensure this function is executed only once throughout the entire
        # project.
        return()
    endif()

    # Initialize all compilation options and defines.
    __init_build_configuration()

    idf_build_set_property(__BUILD_INITIALIZED YES)
endfunction()

#[[
   __get_library_interface_or_die(LIBRARY <library>
                                  OUTPUT <variable>)

   :LIBRARY[in]: Library interface or alias.
   :OUTPUT[out]: Output variable to store the library interface.

   Verify that "LIBRARY" is a known interface created by ``idf_build_library``
   or its alias. If it is, return the library interface; otherwise, terminate
   the build process.
#]]
function(__get_library_interface_or_die)
    set(options)
    set(one_value LIBRARY OUTPUT)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_LIBRARY)
        idf_die("LIBRARY option is required")
    endif()

    if(NOT DEFINED ARG_OUTPUT)
        idf_die("OUTPUT option is required")
    endif()

    __get_real_target(TARGET ${ARG_LIBRARY} OUTPUT library_interface)
    idf_build_get_property(library_interfaces LIBRARY_INTERFACES)

    if(NOT "${library_interface}" IN_LIST library_interfaces)
        idf_die("Library interface '${ARG_LIBRARY}' does not exist")
    endif()
    set(${ARG_OUTPUT} ${library_interface} PARENT_SCOPE)
endfunction()

#[[api
.. cmakev2:function:: idf_library_set_property

   .. code-block:: cmake

      idf_library_set_property(<library> <property> <value> [APPEND])

   :library[in]: Library interface target or alias.
   :property[in]: Property name.
   :value[in]: Property value.
   :APPEND: Append the value to the property's current value instead of
            replacing it.

   Set the value of the specified library property. The property is also
   added to the internal list of library properties if it isn't already
   there.
#]]
function(idf_library_set_property library property value)
    set(options APPEND)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    set(append)
    if(ARG_APPEND)
        set(append APPEND)
    endif()

    __get_library_interface_or_die(LIBRARY "${library}" OUTPUT library_interface)
    __set_property(TARGET "${library_interface}"
                   PROPERTY "${property}"
                   PROPERTIES LIBRARY_PROPERTIES
                   VALUE "${value}"
                   ${append})
endfunction()

#[[api
.. cmakev2:function:: idf_library_get_property

   .. code-block:: cmake

      idf_library_get_property(<variable> <library> <property> [GENERATOR_EXPRESSION])

   :variable[out]: Variable to store the value in.
   :library[in]: Library interface target or alias.
   :property[in]: Property name to get the value of.
   :GENERATOR_EXPRESSION: Obtain the generator expression for the property
                          rather than the actual value.

   Retrieve the value of the specified library property.
#]]
function(idf_library_get_property variable library property)
    set(options GENERATOR_EXPRESSION)
    set(one_value)
    set(multi_value)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    set(genexpr)
    if(ARG_GENERATOR_EXPRESSION)
        set(genexpr GENERATOR_EXPRESSION)
    endif()

    __get_library_interface_or_die(LIBRARY "${library}" OUTPUT library_interface)
    __get_property(TARGET "${library_interface}"
                   PROPERTY "${property}"
                   OUTPUT value
                   ${genexpr})
    set(${variable} ${value} PARENT_SCOPE)
endfunction()

#[[
   __dump_library_properties(<libraries>)

   :libraries: List of library interfaces whose properties should be displayed.

   Dump all properties for the libraries listed in ``<libraries>``.
#]]
function(__dump_library_properties libraries)
    foreach(library IN LISTS libraries)
        idf_library_get_property(properties "${library}" LIBRARY_PROPERTIES)
        idf_msg("library '${library}' properties: ${properties}")
        foreach(property IN LISTS properties)
            idf_library_get_property(value "${library}" "${property}")
            idf_msg("   ${property}: ${value}")
        endforeach()
    endforeach()
endfunction()

#[[api
.. cmakev2:function:: idf_build_library

   .. code-block:: cmake

       idf_build_library(INTERFACE <interface>
                         [COMPONENTS <component>...])

   :INTERFACE[in,out]: Name of the library interface to be created.
   :COMPONENTS[in,opt]: List of component names to add to the library.

   Create a new library interface target with the name specified in the
   ``INTERFACE`` option and link component targets to it based on the component
   names provided in the ``COMPONENTS`` option. If ``COMPONENTS`` option is not
   set, link component targets of all discovered components.
#]]
function(idf_build_library)
    set(options)
    set(one_value INTERFACE)
    set(multi_value COMPONENTS)
    cmake_parse_arguments(ARG "${options}" "${one_value}" "${multi_value}" ${ARGN})

    if(NOT DEFINED ARG_INTERFACE)
        idf_die("INTERFACE option is required")
    endif()

    if(NOT DEFINED ARG_COMPONENTS)
        # The library should include all discovered components.
        idf_build_get_property(component_names COMPONENTS_DISCOVERED)
        set(ARG_COMPONENTS "${component_names}")
    endif()

    # Initialize settings that must be configured after the project() function
    # is called.
    __init_build()

    add_library("${ARG_INTERFACE}" INTERFACE)
    idf_build_set_property(LIBRARY_INTERFACES "${ARG_INTERFACE}" APPEND)
    idf_library_set_property("${ARG_INTERFACE}" LIBRARY_COMPONENTS "${ARG_COMPONENTS}")
endfunction()
