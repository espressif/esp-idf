idf_build_get_property(target IDF_TARGET)
if(NOT "${target}" STREQUAL "linux")
    return()
endif()

set(srcs "lock.c")
set(includes "include")
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND srcs getrandom.c assert_func.c)
else()
    list(APPEND srcs fls.c)
    list(APPEND includes "linux_include")
endif()

idf_component_register(INCLUDE_DIRS ${includes}
                       REQUIRED_IDF_TARGETS linux
                       SRCS ${srcs})

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    # Not all versions of the MacOS linker support the -warn_commons flag.
    # ld version 1053.12 (and above) have been tested to support it.
    # Hence, the GNU -Wl,--warn-common option from the default configuration is
    # filtered here and the linker version is checked before including the
    # Darwin-specific flag below.
    idf_build_get_property(link_options LINK_OPTIONS)
    list(FILTER link_options EXCLUDE REGEX "-Wl,--gc-sections|-Wl,--warn-common")
    idf_build_set_property(LINK_OPTIONS "${link_options}")

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
        idf_build_set_property(LINK_OPTIONS "-Wl,-warn_commons" APPEND)
    endif()
    idf_build_set_property(LINK_OPTIONS "-Wl,-dead_strip" APPEND)
endif()

# Building for Linux target, fall back to an older version of the standard
# if the preferred one is not supported by the compiler. The detected version
# replaces the default one from the project-level configuration.
get_property(enabled_languages GLOBAL PROPERTY ENABLED_LANGUAGES)

if(C IN_LIST enabled_languages)
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
        message(FATAL_ERROR "Failed to set C language standard to one of the supported versions: "
                            "${preferred_c_versions}. Please upgrade the host compiler.")
    endif()

    idf_build_get_property(c_compile_options C_COMPILE_OPTIONS)
    list(FILTER c_compile_options EXCLUDE REGEX "-std=.*")
    list(APPEND c_compile_options "-std=${c_std}")
    idf_build_set_property(C_COMPILE_OPTIONS "${c_compile_options}")
endif()

if(CXX IN_LIST enabled_languages)
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
        message(FATAL_ERROR "Failed to set C++ language standard to one of the supported versions: "
                            "${preferred_cxx_versions}. Please upgrade the host compiler.")
    endif()

    idf_build_get_property(cxx_compile_options CXX_COMPILE_OPTIONS)
    list(FILTER cxx_compile_options EXCLUDE REGEX "-std=.*")
    list(APPEND cxx_compile_options "-std=${cxx_std}")
    idf_build_set_property(CXX_COMPILE_OPTIONS "${cxx_compile_options}")
endif()

target_link_libraries(${COMPONENT_LIB} PRIVATE pthread)

find_library(LIB_BSD bsd)
if(LIB_BSD)
    target_link_libraries(${COMPONENT_LIB} PRIVATE ${LIB_BSD})
elseif(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    message(WARNING "Missing LIBBSD library. Install libbsd-dev package and/or check linker directories.")
endif()
