if(IDF_TARGET STREQUAL "esp32s2")
    idf_component_register(SRCS "test_touch_element.c"
                                "test_touch_button.c"
                                "test_touch_slider.c"
                                "test_touch_matrix.c"
            PRIV_REQUIRES unity touch_element)
endif()
