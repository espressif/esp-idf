idf_build_get_property(idf_target IDF_TARGET)

if(${idf_target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if(CONFIG_OPENTHREAD_ENABLED OR CONFIG_IDF_DOC_BUILD OR CONFIG_OPENTHREAD_SPINEL_ONLY)

    set(public_include_dirs
        "include"
        "openthread/include")
endif()

if(CONFIG_OPENTHREAD_ENABLED)

    set(private_include_dirs
        "openthread/examples/platforms"
        "openthread/src"
        "openthread/src/core"
        "openthread/src/lib"
        "openthread/src/lib/hdlc"
        "openthread/src/lib/spinel"
        "openthread/src/ncp"
        "private_include")

    set(src_dirs
        "src"
        "src/port"
        "openthread/examples/platforms/utils"
        "openthread/src/core/api"
        "openthread/src/core/common"
        "openthread/src/core/crypto"
        "openthread/src/core/diags"
        "openthread/src/core/instance"
        "openthread/src/core/mac"
        "openthread/src/core/radio"
        "openthread/src/core/thread"
        "openthread/src/core/utils"
        "openthread/src/lib/hdlc"
        "openthread/src/lib/spinel")

    set(exclude_srcs
        "openthread/examples/platforms/utils/logging_rtt.c"
        "openthread/examples/platforms/utils/soft_source_match_table.c"
        "openthread/src/core/instance/extension_example.cpp")

    if(CONFIG_OPENTHREAD_FTD OR CONFIG_OPENTHREAD_MTD)
        list(APPEND src_dirs
            "openthread/src/core/backbone_router"
            "openthread/src/core/coap"
            "openthread/src/core/meshcop"
            "openthread/src/core/net"
            "openthread/src/lib/platform")

        if(CONFIG_OPENTHREAD_CLI)
            list(APPEND src_dirs
                "openthread/examples/apps/cli"
                "openthread/src/cli")

            list(APPEND exclude_srcs
                "openthread/examples/apps/cli/main.c")
        endif()

    elseif(CONFIG_OPENTHREAD_RADIO)
        list(APPEND src_dirs
            "openthread/src/ncp"
            "openthread/examples/apps/ncp")

        list(APPEND exclude_srcs
            "src/port/esp_openthread_state.c"
            "openthread/examples/apps/ncp/main.c"
            "openthread/src/core/api/backbone_router_api.cpp"
            "openthread/src/core/api/child_supervision_api.cpp"
            "openthread/src/core/api/dataset_api.cpp"
            "openthread/src/core/api/dns_api.cpp"
            "openthread/src/core/api/entropy_api.cpp"
            "openthread/src/core/api/heap_api.cpp"
            "openthread/src/core/api/icmp6_api.cpp"
            "openthread/src/core/api/ip6_api.cpp"
            "openthread/src/core/api/link_api.cpp"
            "openthread/src/core/api/link_metrics_api.cpp"
            "openthread/src/core/api/message_api.cpp"
            "openthread/src/core/api/nat64_api.cpp"
            "openthread/src/core/api/netdata_api.cpp"
            "openthread/src/core/api/netdiag_api.cpp"
            "openthread/src/core/api/network_time_api.cpp"
            "openthread/src/core/api/random_crypto_api.cpp"
            "openthread/src/core/api/tcp_api.cpp"
            "openthread/src/core/api/udp_api.cpp"
            "openthread/src/core/common/heap.cpp"
            "openthread/src/core/common/heap_string.cpp"
            "openthread/src/core/common/notifier.cpp"
            "openthread/src/core/common/settings.cpp"
            "openthread/src/core/common/time_ticker.cpp"
            "openthread/src/core/mac/channel_mask.cpp"
            "openthread/src/core/mac/data_poll_handler.cpp"
            "openthread/src/core/mac/data_poll_sender.cpp"
            "openthread/src/core/mac/mac.cpp"
            "openthread/src/core/mac/mac_filter.cpp"
            "openthread/src/core/mac/mac_links.cpp"
            "openthread/src/core/thread/announce_begin_server.cpp"
            "openthread/src/core/thread/announce_sender.cpp"
            "openthread/src/core/thread/address_resolver.cpp"
            "openthread/src/core/thread/child_supervision.cpp"
            "openthread/src/core/thread/discover_scanner.cpp"
            "openthread/src/core/thread/energy_scan_server.cpp"
            "openthread/src/core/thread/key_manager.cpp"
            "openthread/src/core/thread/link_metrics.cpp"
            "openthread/src/core/thread/lowpan.cpp"
            "openthread/src/core/thread/mesh_forwarder.cpp"
            "openthread/src/core/thread/mesh_forwarder_ftd.cpp"
            "openthread/src/core/thread/mesh_forwarder_mtd.cpp"
            "openthread/src/core/thread/mle.cpp"
            "openthread/src/core/thread/mle_router.cpp"
            "openthread/src/core/thread/mle_types.cpp"
            "openthread/src/core/thread/neighbor.cpp"
            "openthread/src/core/thread/neighbor_table.cpp"
            "openthread/src/core/thread/network_data.cpp"
            "openthread/src/core/thread/network_data_leader.cpp"
            "openthread/src/core/thread/network_data_leader_ftd.cpp"
            "openthread/src/core/thread/network_data_types.cpp"
            "openthread/src/core/thread/network_data_service.cpp"
            "openthread/src/core/thread/network_diagnostic.cpp"
            "openthread/src/core/thread/panid_query_server.cpp"
            "openthread/src/core/thread/thread_netif.cpp"
            "openthread/src/core/thread/time_sync_service.cpp"
            "openthread/src/core/thread/tmf.cpp"
            "openthread/src/core/thread/topology.cpp"
            "openthread/src/core/utils/child_supervision.cpp")
    endif()

    if(CONFIG_OPENTHREAD_RADIO_NATIVE)
        list(APPEND exclude_srcs
            "src/port/esp_openthread_radio_spinel.cpp"
            "src/port/esp_spi_spinel_interface.cpp"
            "src/port/esp_uart_spinel_interface.cpp"
            )
    elseif(CONFIG_OPENTHREAD_RADIO_SPINEL_UART OR CONFIG_OPENTHREAD_RADIO_SPINEL_SPI)
        list(APPEND exclude_srcs
            "src/port/esp_openthread_radio.c"
            "src/port/esp_openthread_sleep.c")
    elseif(CONFIG_OPENTHREAD_RADIO_154_NONE)
        list(APPEND exclude_srcs
            "src/port/esp_openthread_radio_spinel.cpp"
            "src/port/esp_spi_spinel_interface.cpp"
            "src/port/esp_uart_spinel_interface.cpp"
            "src/port/esp_openthread_radio.c"
            "src/port/esp_openthread_sleep.c"
            )
    endif()

    if(NOT CONFIG_OPENTHREAD_RADIO_TREL)
        list(APPEND exclude_srcs
            "src/port/esp_openthread_trel.c")
    endif()

    if(CONFIG_OPENTHREAD_BORDER_ROUTER)
        list(APPEND src_dirs
            "openthread/src/core/border_router")
    endif()

    if(CONFIG_OPENTHREAD_FTD)
        set_source_files_properties("openthread/src/core/net/srp_server.cpp"
            PROPERTIES COMPILE_FLAGS
            -Wno-maybe-uninitialized)
    endif()

    if(CONFIG_OPENTHREAD_NCP_VENDOR_HOOK)
        list(APPEND src_dirs
            "src/ncp")
        if(CONFIG_OPENTHREAD_RCP_UART)
            list(APPEND exclude_srcs
                "src/ncp/esp_openthread_ncp_spi.cpp")
        elseif(CONFIG_OPENTHREAD_RCP_SPI)
            list(APPEND exclude_srcs
                "src/ncp/esp_openthread_ncp_hdlc.cpp")
        endif()
    endif()

    if(NOT CONFIG_OPENTHREAD_DNS64_CLIENT)
        list(APPEND exclude_srcs
            "src/esp_openthread_dns64.c")
    endif()

    if(NOT CONFIG_FREERTOS_USE_TICKLESS_IDLE)
        list(APPEND exclude_srcs
            "src/port/esp_openthread_sleep.c")
    endif()

    if(NOT CONFIG_OPENTHREAD_PLATFORM_MSGPOOL_MANAGEMENT)
        list(APPEND exclude_srcs
            "src/port/esp_openthread_messagepool.c")
    endif()

    if(CONFIG_OPENTHREAD_FTD)
        set(device_type "OPENTHREAD_FTD=1")
    elseif(CONFIG_OPENTHREAD_MTD)
        set(device_type "OPENTHREAD_MTD=1")
    elseif(CONFIG_OPENTHREAD_RADIO)
        set(device_type "OPENTHREAD_RADIO=1")
    endif()
elseif(CONFIG_OPENTHREAD_SPINEL_ONLY)

    set(src_dirs
        "src/spinel"
        "src/port"
        "openthread/src/lib/spinel"
        "openthread/src/lib/hdlc"
        "openthread/src/lib/platform"
        "openthread/src/core/api"
        "openthread/src/core/common"
        "openthread/src/core/mac")

    set(private_include_dirs
        "private_include"
        "openthread/src"
        "openthread/src/core"
        "openthread/src/lib"
        "openthread/src/lib/hdlc"
        "openthread/src/lib/spinel")

    file(GLOB_RECURSE exclude_srcs_list
        "src/port/*"
        "openthread/src/core/api/*.cpp"
        "openthread/src/core/common/*"
        "openthread/src/core/mac/*")

    list(REMOVE_ITEM exclude_srcs_list
        "${CMAKE_CURRENT_SOURCE_DIR}/src/port/esp_openthread_alarm.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/port/esp_openthread_logging.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/openthread/src/core/api/error_api.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/openthread/src/core/api/logging_api.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/openthread/src/core/common/error.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/openthread/src/core/common/error.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/openthread/src/core/common/log.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/openthread/src/core/common/log.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/openthread/src/core/common/logging.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/openthread/src/core/common/string.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/openthread/src/core/common/string.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/openthread/src/core/mac/mac_frame.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/openthread/src/core/mac/mac_frame.hpp")

    list(APPEND exclude_srcs ${exclude_srcs_list})

endif()

if(CONFIG_OPENTHREAD_HEADER_CUSTOM)
    idf_build_get_property(project_dir PROJECT_DIR)
    list(APPEND private_include_dirs
        "${project_dir}/${CONFIG_OPENTHREAD_CUSTOM_HEADER_PATH}")
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

string(TIMESTAMP OT_BUILD_TIMESTAMP " %Y-%m-%d %H:%M:%S UTC" UTC)
string(CONCAT OT_FULL_VERSION_STRING
       "${CONFIG_OPENTHREAD_PACKAGE_NAME}/"
       "${IDF_VERSION_FOR_OPENTHREAD_PACKAGE}-${OPENTHREAD_VERSION}\; "
       "${CONFIG_OPENTHREAD_PLATFORM_INFO}\; ${OT_BUILD_TIMESTAMP}")

idf_component_register(SRC_DIRS "${src_dirs}"
                       EXCLUDE_SRCS "${exclude_srcs}"
                       INCLUDE_DIRS "${public_include_dirs}"
                       PRIV_INCLUDE_DIRS "${private_include_dirs}"
                       REQUIRES esp_netif lwip esp_driver_uart driver
                       LDFRAGMENTS linker.lf
                       PRIV_REQUIRES console esp_event esp_partition esp_timer
                                     ieee802154 mbedtls nvs_flash)

if(CONFIG_OPENTHREAD_RADIO_TREL)
    idf_component_optional_requires(PRIVATE espressif__mdns)
endif()

if(CONFIG_OPENTHREAD_ENABLED OR CONFIG_OPENTHREAD_SPINEL_ONLY)
    if(CONFIG_OPENTHREAD_RADIO)
        set(CONFIG_FILE_TYPE "radio")
    elseif(CONFIG_OPENTHREAD_FTD)
        set(CONFIG_FILE_TYPE "ftd")
    elseif(CONFIG_OPENTHREAD_MTD)
        set(CONFIG_FILE_TYPE "mtd")
    elseif(CONFIG_OPENTHREAD_SPINEL_ONLY)
        set(CONFIG_FILE_TYPE "spinel")
    endif()

    target_compile_definitions(
        ${COMPONENT_LIB}
        PUBLIC
        "OPENTHREAD_CONFIG_FILE=\"openthread-core-esp32x-${CONFIG_FILE_TYPE}-config.h\""
            "${device_type}"
        "OPENTHREAD_PROJECT_LIB_CONFIG_FILE=\"openthread-core-esp32x-${CONFIG_FILE_TYPE}-config.h\""
            "${device_type}"
        PRIVATE
        "PACKAGE_VERSION=\"${IDF_VERSION_FOR_OPENTHREAD_PACKAGE}-${OPENTHREAD_VERSION}\""
        "OPENTHREAD_BUILD_DATETIME=\"${OT_BUILD_TIMESTAMP}\""
        )

    if(CONFIG_OPENTHREAD_RADIO)
        file(WRITE ${CMAKE_BINARY_DIR}/rcp_version ${OT_FULL_VERSION_STRING})
    endif()

    if(CONFIG_OPENTHREAD_BORDER_ROUTER)
        if($ENV{OPENTHREAD_ESP_BR_LIB_FROM_INTERNAL_SRC})
            idf_component_get_property(openthread_br_lib openthread_br COMPONENT_LIB)
            target_link_libraries(${COMPONENT_LIB} PUBLIC $<TARGET_FILE:${openthread_br_lib}>)
        else()
            add_prebuilt_library(openthread_br "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libopenthread_br.a"
                                 REQUIRES openthread)
            target_link_libraries(${COMPONENT_LIB} INTERFACE openthread_br)
        endif()
    endif()

endif()
