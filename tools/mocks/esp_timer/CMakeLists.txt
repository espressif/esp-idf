# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building ESP TIMER MOCKS")

idf_component_get_property(original_esp_timer_dir esp_timer COMPONENT_OVERRIDEN_DIR)

idf_component_mock(INCLUDE_DIRS "${original_esp_timer_dir}/include"
    REQUIRES esp_common
    MOCK_HEADER_FILES ${original_esp_timer_dir}/include/esp_timer.h)
