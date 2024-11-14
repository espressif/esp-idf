idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(TOUCH_ELEMENT_COMPATIBLE_TARGETS "esp32s2" "esp32s3")

if(${target} IN_LIST TOUCH_ELEMENT_COMPATIBLE_TARGETS)
    idf_component_register(SRCS "touch_element.c"
            "touch_button.c"
            "touch_slider.c"
            "touch_matrix.c"
            INCLUDE_DIRS include
            REQUIRES driver  # touch_element uses legacy "driver/touch_sensor.h"
            PRIV_REQUIRES esp_timer)
endif()
