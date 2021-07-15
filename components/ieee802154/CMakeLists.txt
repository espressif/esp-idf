idf_component_register(
    INCLUDE_DIRS include
    )

if(CONFIG_IEEE802154_ENABLED)
    if(CONFIG_IEEE802154_LIB_FROM_INTERNAL_SRC)
        idf_component_get_property(ieee802154_lib ieee802154_driver COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${ieee802154_lib}>)
    else()
        add_prebuilt_library(ieee802154_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/lib802154.a")
        target_link_libraries(${COMPONENT_LIB} INTERFACE ieee802154_lib)
    endif()

    # force the phy libraries to be linked behind ieee802154
    idf_component_get_property(esp_phy_lib esp_phy COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} INTERFACE $<TARGET_FILE:${esp_phy_lib}> libphy.a libbtbb.a
                    $<TARGET_FILE:${esp_phy_lib}>)
endif()
