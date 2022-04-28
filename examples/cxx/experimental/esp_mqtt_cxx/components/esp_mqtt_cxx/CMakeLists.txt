idf_build_get_property(target IDF_TARGET)

idf_component_register(SRCS "esp_mqtt_cxx.cpp"
                    INCLUDE_DIRS "include"
                    )

target_compile_options(${COMPONENT_LIB} PRIVATE "-std=gnu++17")

if(TEST_BUILD)
    message(STATUS "Test build")
    idf_component_get_property(mqtt_dir mqtt COMPONENT_DIR)
    idf_component_get_property(experimental_cpp_component_dir experimental_cpp_component COMPONENT_DIR)
    idf_component_get_property(esp_common_dir esp_common COMPONENT_DIR)
    idf_component_get_property(esp_event_dir esp_event COMPONENT_DIR)
    target_include_directories(${COMPONENT_LIB} PUBLIC ${mqtt_dir}/esp-mqtt/include
                                                       ${esp_event_dir}/include
                                                       ${experimental_cpp_component_dir}/include
                                                       ${esp_common_dir}/include)

else()
    idf_component_get_property(mqtt_lib mqtt COMPONENT_LIB)
    idf_component_get_property(log_lib log COMPONENT_LIB)
    idf_component_get_property(experimental_cpp_component_lib experimental_cpp_component COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PUBLIC ${log_lib} ${mqtt_lib} ${experimental_cpp_component_lib})
endif()
