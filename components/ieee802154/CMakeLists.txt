idf_build_get_property(idf_target IDF_TARGET)

idf_component_register(
    INCLUDE_DIRS include
    REQUIRES esp_phy
    )

if(CONFIG_IEEE802154_ENABLED)
    idf_component_get_property(esp_phy_lib esp_phy COMPONENT_LIB)
    if($ENV{IEEE802154_LIB_FROM_INTERNAL_SRC})
        idf_component_get_property(ieee802154_lib ieee802154_driver COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${ieee802154_lib}>
                            $<TARGET_FILE:${esp_phy_lib}> libphy.a libbtbb.a)
    else()
        if(IDF_TARGET STREQUAL "esp32h4")
            if(CONFIG_IDF_TARGET_ESP32H4_BETA_VERSION_1)
                # TODO: rename esp32h2 to esp32h4 [IDF-6097]
                target_link_libraries(${COMPONENT_LIB} INTERFACE
                            "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32h2/rev1")
            endif()
            if(CONFIG_IDF_TARGET_ESP32H4_BETA_VERSION_2)
                # TODO: rename esp32h2 to esp32h4 [IDF-6097]
                target_link_libraries(${COMPONENT_LIB} INTERFACE
                            "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32h2/rev2")
            endif()
        else()
            target_link_directories(${COMPONENT_LIB} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}")
        endif()
        target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${esp_phy_lib}> lib802154.a libphy.a libbtbb.a
                            $<TARGET_FILE:${esp_phy_lib}>)
    endif()
endif()
