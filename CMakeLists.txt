cmake_minimum_required(VERSION 3.16)
project(esp-idf C CXX ASM)

if(CMAKE_CURRENT_LIST_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Current directory '${CMAKE_CURRENT_LIST_DIR}' is not buildable. "
    "Change directories to one of the example projects in '${CMAKE_CURRENT_LIST_DIR}/examples' and try "
    "again.")
endif()

# Variables compile_options, c_compile_options, cxx_compile_options, compile_definitions, link_options shall
# not be unset as they may already contain flags, set by toolchain-TARGET.cmake files.

# Add the following build specifications here, since these seem to be dependent
# on config values on the root Kconfig.

if(NOT BOOTLOADER_BUILD)

    if(CONFIG_COMPILER_OPTIMIZATION_SIZE)
        list(APPEND compile_options "-Os")
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

else()  # BOOTLOADER_BUILD

    if(CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_SIZE)
        list(APPEND compile_options "-Os")
        if(CMAKE_C_COMPILER_ID MATCHES "GNU")
            list(APPEND compile_options "-freorder-blocks")
        endif()
    elseif(CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_DEBUG)
        list(APPEND compile_options "-Og")
        if(CMAKE_C_COMPILER_ID MATCHES "GNU" AND NOT CONFIG_IDF_TARGET_LINUX)
            list(APPEND compile_options "-fno-shrink-wrap")  # Disable shrink-wrapping to reduce binary size
        endif()
    elseif(CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_NONE)
        list(APPEND compile_options "-O0")
    elseif(CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_PERF)
        list(APPEND compile_options "-O2")
    endif()

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
    list(APPEND link_options "-fno-rtti")           # used to invoke correct multilib variant (no-rtti) during linking
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
    # Logic bug in essl component
    list(APPEND compile_options "-Wno-tautological-overlap-compare")
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

__generate_prefix_map(prefix_map_compile_options)
list(APPEND compile_options ${prefix_map_compile_options})

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

if(CONFIG_COMPILER_DISABLE_DEFAULT_ERRORS)
    if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
        idf_build_replace_option_from_property(COMPILE_OPTIONS "-Werror" "-Werror=all")
    endif()
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

    message(STATUS "Linker Version: ${LD_VERSION_MAJOR_MINOR}")

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
    list(APPEND compile_options "-fno-use-cxa-atexit")
endif()

if(COMPILER_RT_LIB_NAME)
    list(APPEND link_options "-rtlib=${CONFIG_COMPILER_RT_LIB_NAME}")
endif()


idf_build_set_property(COMPILE_OPTIONS "${compile_options}" APPEND)
idf_build_set_property(C_COMPILE_OPTIONS "${c_compile_options}" APPEND)
idf_build_set_property(CXX_COMPILE_OPTIONS "${cxx_compile_options}" APPEND)
idf_build_set_property(ASM_COMPILE_OPTIONS "${asm_compile_options}" APPEND)
idf_build_set_property(COMPILE_DEFINITIONS "${compile_definitions}" APPEND)
idf_build_set_property(LINK_OPTIONS "${link_options}" APPEND)

idf_build_get_property(build_component_targets __BUILD_COMPONENT_TARGETS)

# Add each component as a subdirectory, processing each component's CMakeLists.txt
foreach(component_target ${build_component_targets})
    __component_get_property(dir ${component_target} COMPONENT_DIR)
    __component_get_property(_name ${component_target} COMPONENT_NAME)
    __component_get_property(prefix ${component_target} __PREFIX)
    __component_get_property(alias ${component_target} COMPONENT_ALIAS)
    set(COMPONENT_NAME ${_name})
    set(COMPONENT_DIR ${dir})
    set(COMPONENT_ALIAS ${alias})
    set(COMPONENT_PATH ${dir}) # for backward compatibility only, COMPONENT_DIR is preferred
    idf_build_get_property(build_prefix __PREFIX)
    set(__idf_component_context 1)
    if(NOT prefix STREQUAL build_prefix)
        add_subdirectory(${dir} ${prefix}_${_name})
    else()
        add_subdirectory(${dir} ${_name})
    endif()
    set(__idf_component_context 0)
endforeach()
