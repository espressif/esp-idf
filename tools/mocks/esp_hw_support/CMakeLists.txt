# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building ESP HW SUPPORT MOCKS")

idf_component_get_property(original_esp_hw_support_dir esp_hw_support COMPONENT_OVERRIDEN_DIR)

idf_component_mock(INCLUDE_DIRS "${original_esp_hw_support_dir}/include"
                   MOCK_HEADER_FILES ${original_esp_hw_support_dir}/include/esp_mac.h
                                     ${original_esp_hw_support_dir}/include/esp_random.h
    )
