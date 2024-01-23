# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building ESP WIFI MOCKS")

idf_component_get_property(original_esp_wifi_dir esp_wifi COMPONENT_OVERRIDEN_DIR)

set(include_dirs
    "${original_esp_wifi_dir}/include" "${original_esp_wifi_dir}/include/local")

idf_component_mock(INCLUDE_DIRS ${include_dirs}
    REQUIRES esp_event esp_netif lwip
    MOCK_HEADER_FILES ${original_esp_wifi_dir}/include/esp_wifi.h
                      ${original_esp_wifi_dir}/include/esp_wifi_default.h
                      ${original_esp_wifi_dir}/include/esp_wifi_netif.h
                      ${original_esp_wifi_dir}/include/esp_wifi_he.h
                      ${original_esp_wifi_dir}/include/esp_wifi_ap_get_sta_list.h
                      ${original_esp_wifi_dir}/include/esp_smartconfig.h
                      ${original_esp_wifi_dir}/include/esp_mesh.h
                      ${original_esp_wifi_dir}/include/esp_now.h
                      ${original_esp_wifi_dir}/include/smartconfig_ack.h)

# -Warray-parameter has been introduced in GCC 11
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL "11.0.0")
    target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-array-parameter)
endif()
target_sources(${COMPONENT_LIB} PRIVATE "global_symbols_mock.c")
