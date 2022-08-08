set(TOUCH_ELEMENT_COMPATIBLE_TARGETS "esp32s2" "esp32s3")

if(IDF_TARGET IN_LIST TOUCH_ELEMENT_COMPATIBLE_TARGETS)
    idf_component_register(SRCS "touch_element.c"
            "touch_button.c"
            "touch_slider.c"
            "touch_matrix.c"
            INCLUDE_DIRS include
            REQUIRES driver
            PRIV_REQUIRES esp_timer)
endif()
