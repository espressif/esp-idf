set(COMPONENT_ADD_INCLUDEDIRS "port/include" "mbedtls/include")
set(COMPONENT_SRCS "mbedtls.c")
set(COMPONENT_REQUIRES lwip)

set(MBEDTLS_PRIV_REQUIRES ${IDF_COMPONENT_REQUIRES_COMMON} soc)

register_component()

# Only build mbedtls libraries
set(ENABLE_TESTING CACHE BOOL OFF)
set(ENABLE_PROGRAMS CACHE BOOL OFF)

# Use same policy between IDF and mbedtls build
function(project)
    set(_args ARGV)
    _project(${${_args}})
    cmake_policy(SET CMP0022 NEW)
endfunction()

# Needed to for include_next includes to work from within mbedtls
include_directories("${COMPONENT_DIR}/port/include")

# Workaround issue with creating symbolic links due to issues with native
# path conversion (TO_NATIVE_PATH). The following summarizes what CMake invocations
# this workaround is for:
#
# 1. CMake from command line + Ninja = No errors
# 2. CMake from command line + MinGW Makefiles = Forward slash for paths, mklink mistakes path for a switch
# 3. CMake from MSYS + Ninja = No errors
# 4. CMake from MSYS + Unix Makefiles/MSYS Makefiles = Forward slash for paths, mklink mistakes path for a switch
#
# There are references to the issue in case (2) and (4) in https://github.com/ARMmbed/mbedtls/issues/1496,
# https://cmake.org/pipermail/cmake/2006-July/010193.html, and https://cmake.org/Bug/view.php?id=5939.
#
# This workaround is meant to circumvent logic inside link_to_source() function in mbedtls/mbedtls/CMakeLists.txt.
if(CMAKE_HOST_WIN32)
    set(msystem $ENV{MSYSTEM})
    if(MSYS OR msystem)
        # Solves case (4). When in MSYS environment, instead opt to use the Unix equivalent of mklink.
        set(CMAKE_HOST_UNIX 1)
    else()
        # Solves case (2). When invoked from command line, create the symbolic links ahead of link_to_source() invocations
        # using a 'hybrid' path format resilient intermediary - in this case a Python wrapper for mklink. This is more unweildy
        # than necessary, since string(REPLACE "/" "\\" ... does not actually work.
        set(target_links "mbedtls/include/mbedtls"
                        "mbedtls/scripts")
        foreach(target_link ${target_links})
            file(TO_NATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR}/${target_link} link)
            file(TO_NATIVE_PATH ${COMPONENT_DIR}/${target_link} target)

            idf_build_get_property(python PYTHON)
            if(NOT EXISTS ${link})
                if(IS_DIRECTORY ${target})
                    set(command ${python} ${COMPONENT_DIR}/mklink.py /j ${link} ${target})
                else()
                    set(command ${python} ${COMPONENT_DIR}/mklink.py /h ${link} ${target})
                endif()

                execute_process(COMMAND ${command}
                    RESULT_VARIABLE result
                    ERROR_VARIABLE output)

                if(NOT ${result} EQUAL 0)
                    message(FATAL_ERROR "Could not create symbolic link for: ${target} --> ${output}")
                endif()
            endif()
        endforeach()
    endif()
endif()

# Import mbedtls library targets
add_subdirectory(mbedtls)

# Use port specific implementation of net_socket.c instead of one from mbedtls
get_target_property(src_tls mbedtls SOURCES)
list(REMOVE_ITEM src_tls net_sockets.c)
set_property(TARGET mbedtls PROPERTY SOURCES ${src_tls})

set(mbedtls_targets mbedtls mbedcrypto mbedx509)

# Add port files to mbedtls targets
target_sources(mbedtls PRIVATE "${COMPONENT_DIR}/port/esp_bignum.c"
                                "${COMPONENT_DIR}/port/esp_hardware.c"
                                "${COMPONENT_PATH}/port/esp_sha.c"
                                "${COMPONENT_DIR}/port/esp_sha1.c"
                                "${COMPONENT_DIR}/port/esp_sha256.c"
                                "${COMPONENT_DIR}/port/esp_sha512.c"
                                "${COMPONENT_DIR}/port/mbedtls_debug.c"
                                "${COMPONENT_DIR}/port/net_sockets.c"
                                "${COMPONENT_DIR}/port/esp32/aes.c"
                                "${COMPONENT_DIR}/port/esp32/sha.c")
target_sources(mbedcrypto PRIVATE "${COMPONENT_DIR}/port/esp_mem.c")

foreach(target ${mbedtls_targets})
    target_compile_definitions(${target} PUBLIC -DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h")
endforeach()

# Link mbedtls libraries to component library
target_link_libraries(${COMPONENT_LIB} ${mbedtls_targets})

# Catch usage of deprecated mbedTLS functions when building tests
if(mbedtls_test IN_LIST BUILD_TEST_COMPONENTS)
    add_definitions(-DMBEDTLS_DEPRECATED_WARNING)
endif()