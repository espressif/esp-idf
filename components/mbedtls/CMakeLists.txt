set(COMPONENT_ADD_INCLUDEDIRS "port/include" "mbedtls/include")
set(COMPONENT_SRCS "mbedtls.c")
set(COMPONENT_REQUIRES lwip)

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
include_directories("${COMPONENT_PATH}/port/include")

# Needed in order to workaround issue with improper conversion to native path
# when building on MSYS2. This ensures that when building on MSYS, Unix style 
# paths are used.
set(msystem $ENV{MSYSTEM})

if(MSYS OR msystem)
    set(CMAKE_HOST_UNIX 1)
endif()

# Import mbedtls library targets
add_subdirectory(mbedtls)

# Use port specific implementation of net_socket.c instead of one from mbedtls
get_target_property(src_tls mbedtls SOURCES)
list(REMOVE_ITEM src_tls net_sockets.c)
set_property(TARGET mbedtls PROPERTY SOURCES ${src_tls})

set(mbedtls_targets mbedtls mbedcrypto mbedx509)

# Add port files to mbedtls targets
target_sources(mbedtls PRIVATE "${COMPONENT_PATH}/port/esp_bignum.c"
                                "${COMPONENT_PATH}/port/esp_hardware.c"
                                "${COMPONENT_PATH}/port/esp_mem.c"
                                "${COMPONENT_PATH}/port/esp_sha1.c"
                                "${COMPONENT_PATH}/port/esp_sha256.c"
                                "${COMPONENT_PATH}/port/esp_sha512.c"
                                "${COMPONENT_PATH}/port/mbedtls_debug.c"
                                "${COMPONENT_PATH}/port/net_sockets.c")

foreach(target ${mbedtls_targets})
    # Propagate compile options to mbedtls library targets
    target_include_directories(${target} PRIVATE "${IDF_INCLUDE_DIRECTORIES}")
    target_compile_options(${target} PRIVATE "${IDF_COMPILE_OPTIONS};${IDF_C_COMPILE_OPTIONS}")
    target_compile_definitions(${target} PRIVATE "${IDF_COMPILE_DEFINITIONS}")
    target_compile_definitions(${target} PUBLIC -DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h")

    # The mbedtls targets also depends on core components
    foreach(common ${IDF_COMPONENT_REQUIRES_COMMON})
        component_get_target(common_target ${common})
        set_property(TARGET ${target} APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${common_target})
        set_property(TARGET ${target} APPEND PROPERTY LINK_LIBRARIES ${common_target})
    endforeach()
endforeach()

# Link mbedtls libraries to component library
target_link_libraries(${COMPONENT_TARGET} ${mbedtls_targets})

# Catch usage of deprecated mbedTLS functions when building tests
if(mbedtls_test IN_LIST BUILD_TEST_COMPONENTS)
    add_definitions(-DMBEDTLS_DEPRECATED_WARNING)
endif()
