if(CONFIG_OPENTHREAD_ENABLED)
    set(public_include_dirs
        "include"
        "openthread/include")

    set(private_include_dirs
        "openthread/include/openthread"
        "openthread/src"
        "openthread/src/core"
        "openthread/src/lib/hdlc"
        "openthread/src/lib/spinel"
        "openthread/src/ncp"
        "openthread/examples/platforms/"
        "private_include")

    set(src_dirs
        "openthread/examples/apps/cli"
        "openthread/examples/platforms/utils"
        "openthread/src/cli"
        "openthread/src/core/api"
        "openthread/src/core/backbone_router"
        "openthread/src/core/coap"
        "openthread/src/core/common"
        "openthread/src/core/crypto"
        "openthread/src/core/diags"
        "openthread/src/core/mac"
        "openthread/src/core/meshcop"
        "openthread/src/core/net"
        "openthread/src/core/radio"
        "openthread/src/core/thread"
        "openthread/src/core/utils"
        "openthread/src/lib/platform"
        "openthread/src/lib/hdlc"
        "openthread/src/lib/spinel"
        "port")

    set(exclude_srcs
        "openthread/examples/apps/cli/main.cpp"
        "openthread/examples/platforms/utils/logging_rtt.c"
        "openthread/src/core/common/extension_example.cpp")

    if(CONFIG_OPENTHREAD_FTD)
        set(device_type "OPENTHREAD_FTD=1")
    elseif(CONFIG_OPENTHREAD_MTD)
        set(device_type "OPENTHREAD_MTD=1")
    elseif(CONFIG_OPENTHREAD_RADIO)
        set(device_type "OPENTHREAD_RADIO=1")
    endif()
endif()

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE IDF_VERSION_FOR_OPENTHREAD_PACKAGE OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/openthread
    OUTPUT_VARIABLE OPENTHREAD_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE
)

idf_component_register(SRC_DIRS "${src_dirs}"
                       EXCLUDE_SRCS "${exclude_srcs}"
                       INCLUDE_DIRS "${public_include_dirs}"
                       PRIV_INCLUDE_DIRS "${private_include_dirs}"
                       REQUIRES mbedtls spi_flash)

if(CONFIG_OPENTHREAD_ENABLED)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE
                               "OPENTHREAD_CONFIG_FILE=\"openthread-core-esp32x-config.h\""
                               "PACKAGE_VERSION=\"${IDF_VERSION_FOR_OPENTHREAD_PACKAGE}-${OPENTHREAD_VERSION}\""
                               "${device_type}")
endif()
