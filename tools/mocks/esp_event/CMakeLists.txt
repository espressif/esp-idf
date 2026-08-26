# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building ESP EVENT MOCKS")

idf_component_get_property(original_esp_event_dir esp_event COMPONENT_OVERRIDEN_DIR)

idf_component_mock(INCLUDE_DIRS "${original_esp_event_dir}/include"
                   MOCK_HEADER_FILES ${original_esp_event_dir}/include/esp_event.h
    )
