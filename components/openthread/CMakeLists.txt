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
        "openthread/src/include"
        "openthread/src/lib"
        "openthread/src/lib/hdlc"
        "openthread/src/lib/spinel"
        "openthread/src/ncp"
        "private_include")

    if(CONFIG_OPENTHREAD_FTD)
        include(srcs_ftd_mtd.cmake)
        set(device_type "OPENTHREAD_FTD=1")
    elseif(CONFIG_OPENTHREAD_MTD)
        include(srcs_ftd_mtd.cmake)
        set(device_type "OPENTHREAD_MTD=1")
    elseif(CONFIG_OPENTHREAD_RADIO)
        include(srcs_radio.cmake)
        set(device_type "OPENTHREAD_RADIO=1")
    endif()
elseif(CONFIG_OPENTHREAD_SPINEL_ONLY)
    include(srcs_spinel.cmake)
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

# Register component based on mode
if(CONFIG_OPENTHREAD_FTD OR CONFIG_OPENTHREAD_MTD)
    # FTD/MTD modes use directory-based collection from srcs_ftd_mtd.cmake
    idf_component_register(SRC_DIRS "${src_dirs}"
                           EXCLUDE_SRCS "${exclude_srcs}"
                           INCLUDE_DIRS "${public_include_dirs}"
                           PRIV_INCLUDE_DIRS "${private_include_dirs}"
                           REQUIRES esp_netif lwip esp_driver_uart esp_driver_gpio esp_driver_spi
                                    esp_driver_usb_serial_jtag
                           LDFRAGMENTS linker.lf
                           PRIV_REQUIRES console esp_coex esp_event esp_partition esp_timer
                                         ieee802154 mbedtls nvs_flash esp_psram)
elseif(CONFIG_OPENTHREAD_RADIO)
    # RCP mode uses explicit source list from srcs_radio.cmake
    idf_component_register(SRCS ${rcp_srcs}
                           INCLUDE_DIRS "${public_include_dirs}"
                           PRIV_INCLUDE_DIRS "${private_include_dirs}"
                           REQUIRES esp_netif lwip esp_driver_uart esp_driver_gpio esp_driver_spi
                                    esp_driver_usb_serial_jtag
                           LDFRAGMENTS linker.lf
                           PRIV_REQUIRES console esp_coex esp_event esp_partition esp_timer
                                         ieee802154 mbedtls nvs_flash esp_psram)
elseif(CONFIG_OPENTHREAD_SPINEL_ONLY)
    # SPINEL_ONLY mode uses explicit source list from srcs_spinel.cmake
    idf_component_register(SRCS ${spinel_srcs}
                           INCLUDE_DIRS "${public_include_dirs}"
                           PRIV_INCLUDE_DIRS "${private_include_dirs}"
                           REQUIRES esp_netif lwip esp_driver_uart esp_driver_gpio esp_driver_spi
                                    esp_driver_usb_serial_jtag
                           LDFRAGMENTS linker.lf
                           PRIV_REQUIRES console esp_coex esp_event esp_partition esp_timer
                                         ieee802154 mbedtls nvs_flash esp_psram)
else()
    # This is not supposed to happen, but in older versions of IDF components couldn't be selectively
    # enabled based on KConfig options so some examples just included openthread for all configs.
    idf_component_register(INCLUDE_DIRS "${public_include_dirs}"
                           PRIV_INCLUDE_DIRS "${private_include_dirs}"
                           REQUIRES esp_netif lwip esp_driver_uart esp_driver_gpio esp_driver_spi
                                    esp_driver_usb_serial_jtag
                           LDFRAGMENTS linker.lf
                           PRIV_REQUIRES console esp_coex esp_event esp_partition esp_timer
                                         ieee802154 mbedtls nvs_flash esp_psram)
endif()

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
