if(CONFIG_OPENTHREAD_ENABLED)
    set(public_include_dirs
        "include"
        "openthread/include")

    set(private_include_dirs
        "openthread/src"
        "openthread/src/core"
        "openthread/src/lib/hdlc"
        "openthread/src/lib/spinel"
        "openthread/src/ncp"
        "openthread/examples/platforms/")

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
        "openthread/src/lib/spinel")

    if(CONFIG_OPENTHREAD_BORDER_ROUTER)
        list(APPEND src_dirs
            "openthread/src/core/border_router")
    endif()

    set(exclude_srcs
        "openthread/examples/apps/cli/main.c"
        "openthread/examples/platforms/utils/logging_rtt.c"
        "openthread/examples/platforms/utils/soft_source_match_table.c"
        "openthread/src/core/common/extension_example.cpp")

    set_source_files_properties("openthread/src/core/net/srp_server.cpp"
        PROPERTIES COMPILE_FLAGS
        -Wno-maybe-uninitialized)

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
                       REQUIRES mbedtls ieee802154)

if(CONFIG_OPENTHREAD_ENABLED)
    target_compile_definitions(
        ${COMPONENT_LIB}
        PUBLIC
            "OPENTHREAD_CONFIG_FILE=\"openthread-core-esp32x-config.h\""
            "${device_type}"
        PRIVATE
            "PACKAGE_VERSION=\"${IDF_VERSION_FOR_OPENTHREAD_PACKAGE}-${OPENTHREAD_VERSION}\"")

    if(CONFIG_OPENTHREAD_ESP_LIB_FROM_INTERNAL_SRC)
        idf_component_get_property(openthread_port_lib openthread_port COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} PUBLIC $<TARGET_FILE:${openthread_port_lib}>)

        if(CONFIG_OPENTHREAD_BORDER_ROUTER)
            idf_component_get_property(openthread_br_lib openthread_br COMPONENT_LIB)
            target_link_libraries(${COMPONENT_LIB} PUBLIC $<TARGET_FILE:${openthread_br_lib}>)
        endif()

    else()
        add_prebuilt_library(openthread_port "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libopenthread_port.a"
                            REQUIRES openthread)
        add_prebuilt_library(openthread_br "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libopenthread_br.a"
                            REQUIRES openthread)

        target_link_libraries(${COMPONENT_LIB} INTERFACE openthread_port)

        if(CONFIG_OPENTHREAD_BORDER_ROUTER)
            target_link_libraries(${COMPONENT_LIB} INTERFACE openthread_br)
        endif()

    endif()

endif()
