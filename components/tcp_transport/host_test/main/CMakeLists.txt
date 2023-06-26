idf_component_register(SRCS  "test_socks_transport.cpp" "catch_main.cpp"
                        REQUIRES tcp_transport mocked_transport
                        INCLUDE_DIRS "$ENV{IDF_PATH}/tools"
                        WHOLE_ARCHIVE)

idf_component_get_property(lwip_component lwip COMPONENT_LIB)
idf_component_get_property(esp_timer_component esp_timer COMPONENT_LIB)
idf_component_get_property(tcp_transport_component tcp_transport COMPONENT_LIB)
target_link_libraries(${tcp_transport_component} PUBLIC ${lwip_component} ${esp_timer_component})
target_compile_options(${COMPONENT_LIB} PUBLIC -fsanitize=address -fconcepts)
target_link_options(${COMPONENT_LIB} PUBLIC -fsanitize=address)
set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 20)
