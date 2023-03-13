idf_component_register(SRCS "esp_lvgl_port.c" INCLUDE_DIRS "include" REQUIRES "esp_lcd" PRIV_REQUIRES "esp_timer")

idf_build_get_property(build_components BUILD_COMPONENTS)
if("espressif__button" IN_LIST build_components)
    target_link_libraries(${COMPONENT_LIB} PRIVATE idf::espressif__button)
endif()