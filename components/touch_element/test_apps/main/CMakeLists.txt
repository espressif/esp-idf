idf_component_register(SRCS "test_app_main.c" "test_touch_element.c" "test_touch_button.c"
                            "test_touch_slider.c" "test_touch_matrix.c"
                       PRIV_REQUIRES unity touch_element
                       WHOLE_ARCHIVE
)
