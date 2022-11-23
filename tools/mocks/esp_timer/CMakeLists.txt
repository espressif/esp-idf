# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building ESP TIMER MOCKS")

idf_component_get_property(original_esp_timer_dir esp_timer COMPONENT_OVERRIDEN_DIR)

set(include_dirs
    "${original_esp_timer_dir}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../esp_hw_support/include")

idf_component_mock(INCLUDE_DIRS ${include_dirs}
    REQUIRES esp_common
    MOCK_HEADER_FILES ${original_esp_timer_dir}/include/esp_timer.h)
