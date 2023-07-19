idf_build_get_property(target IDF_TARGET)

set(srcs "test_event_main.c" "test_event_common.cpp")
set(priv_requires "esp_event unity")

if(NOT ${target} STREQUAL "linux")
    list(APPEND srcs
        "test_event_target.c")

    list(APPEND priv_requires
        "driver"
        "esp_timer"
        "test_utils")
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_INCLUDE_DIRS . ../../private_include
                       PRIV_REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE)
