# CMakeLists.txt remote functionality for esp_wifi component
#
# On linux, esp_wifi is registered with idf_component_register() and no sources, so the
# build system creates an INTERFACE library. None of the logic below (includes, sources,
# optional_requires) applies to INTERFACE-only targets, so return early and skip it.
get_target_property(component_lib_type ${COMPONENT_LIB} TYPE)
if(component_lib_type STREQUAL "INTERFACE_LIBRARY")
    return()
endif()

target_include_directories(${COMPONENT_LIB} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(NOT CONFIG_ESP_WIFI_ENABLED AND NOT CONFIG_ESP_HOST_WIFI_ENABLED)
    set(wifi_remote_sources esp_wifi_remote.c esp_wifi_remote_net.c)
    target_include_directories(${COMPONENT_LIB} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/injected)
    set(wifi_remote_sources esp_wifi_remote.c esp_wifi_remote_net.c)

    set(src_wifi_with_remote ${CMAKE_CURRENT_SOURCE_DIR}/esp_wifi_with_remote.c)
    if(CONFIG_ESP_WIFI_REMOTE_EAP_ENABLED)
        list(APPEND src_wifi_with_remote ${CMAKE_CURRENT_SOURCE_DIR}/esp_eap_client_with_remote.c)
    endif()
    get_target_property(wifi_sources ${COMPONENT_LIB} SOURCES)
    # set_target_properties(${COMPONENT_LIB} PROPERTIES SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/dummy_src.c")

    # Update wifi include directories to prepend the injected dir with modified headers supporting SLAVE capability
    get_target_property(original_wifi_dirs ${COMPONENT_LIB} INTERFACE_INCLUDE_DIRECTORIES)
    set(updated_wifi_dirs "${CMAKE_CURRENT_SOURCE_DIR}/include/injected" ${original_wifi_dirs})
    set_target_properties(${COMPONENT_LIB} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${updated_wifi_dirs}")
else()
    set(wifi_remote_sources esp_wifi_remote_net2.c)
endif()

# Add weak symbol for the wifi_remote API if we're using esp_hosted implementation
if(CONFIG_ESP_WIFI_REMOTE_LIBRARY_HOSTED)
    set(src_wifi_remote_weak ${CMAKE_CURRENT_SOURCE_DIR}/esp_wifi_remote_weak.c)
    if(CONFIG_ESP_WIFI_REMOTE_EAP_ENABLED)
        list(APPEND src_wifi_remote_weak ${CMAKE_CURRENT_SOURCE_DIR}/esp_eap_client_remote_weak.c)
    endif()
endif()

target_sources(${COMPONENT_LIB}             PRIVATE  ${src_wifi_remote_weak}
                                                     ${src_wifi_with_remote}
                                                     ${wifi_remote_sources}
                                                     ${wifi_sources})

if(CONFIG_ESP_WIFI_REMOTE_EAP_ENABLED)
    idf_component_optional_requires(PRIVATE wpa_supplicant)
endif()

if(CONFIG_ESP_WIFI_REMOTE_LIBRARY_EPPP)
    idf_component_optional_requires(PRIVATE wifi_remote_over_eppp espressif__wifi_remote_over_eppp)
endif()
