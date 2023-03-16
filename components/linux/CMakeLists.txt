idf_build_get_property(target IDF_TARGET)
if(NOT "${target}" STREQUAL "linux")
    return()
endif()

idf_component_register(SRCS esp_event_stubs.c
                       INCLUDE_DIRS include ${IDF_PATH}/components/esp_event/include
                       REQUIRED_IDF_TARGETS linux)
