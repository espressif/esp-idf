set(TOUCH_ELEMENT_COMPATIBLE_TARGETS "esp32s2" "esp32s3")

if(IDF_TARGET IN_LIST TOUCH_ELEMENT_COMPATIBLE_TARGETS)
    idf_component_register(SRCS "test_touch_element.c"
                                "test_touch_button.c"
                                "test_touch_slider.c"
                                "test_touch_matrix.c"
            PRIV_REQUIRES unity touch_element)
    target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
endif()
