if(IDF_TARGET STREQUAL "esp32s2")
    idf_component_register(SRCS "touch_element.c"
            "touch_button.c"
            "touch_slider.c"
            "touch_matrix.c"
            INCLUDE_DIRS include
            REQUIRES driver)
endif()
