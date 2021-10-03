idf_build_get_property(target IDF_TARGET)

set(srcs "esp_timer_cxx.cpp" "esp_exception.cpp" "gpio_cxx.cpp")
set(requires "esp_timer" "driver")

if(NOT ${target} STREQUAL "linux")
    list(APPEND srcs
        "i2c_cxx.cpp"
        "esp_event_api.cpp"
        "esp_event_cxx.cpp")
    list(APPEND requires "esp_event")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include"
                    REQUIRES ${requires})
