if(CONFIG_OPENTHREAD_ENABLED)
    if(CONFIG_OPENTHREAD_FTD)
        include(ftd.cmake)
    else()
        include(radio.cmake)
    endif()

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
                       REQUIRES mbedtls ieee802154 console)

if(CONFIG_OPENTHREAD_ENABLED)
    if(CONFIG_OPENTHREAD_RADIO)
        set(CONFIG_FILE_TYPE "radio")
    else()
        set(CONFIG_FILE_TYPE "ftd")
    endif()

    target_compile_definitions(
        ${COMPONENT_LIB}
        PUBLIC
        "OPENTHREAD_CONFIG_FILE=\"openthread-core-esp32x-${CONFIG_FILE_TYPE}-config.h\""
            "${device_type}"
        PRIVATE
        "PACKAGE_VERSION=\"${IDF_VERSION_FOR_OPENTHREAD_PACKAGE}-${OPENTHREAD_VERSION}\"")

    if(CONFIG_OPENTHREAD_ESP_LIB_FROM_INTERNAL_SRC)
        idf_component_get_property(openthread_port_lib openthread_port COMPONENT_LIB)
        idf_component_get_property(esp_system_lib esp_system COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} PUBLIC
                              $<TARGET_FILE:${openthread_port_lib}>
                              $<TARGET_FILE:${esp_system_lib}>)

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
