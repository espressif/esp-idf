idf_component_register(SRCS "esp-mqtt/mqtt_client.c"
                            "esp-mqtt/lib/mqtt_msg.c"
                            "esp-mqtt/lib/mqtt_outbox.c"
                            "esp-mqtt/lib/platform_esp32_idf.c"
                    INCLUDE_DIRS esp-mqtt/include
                    PRIV_INCLUDE_DIRS "esp-mqtt/lib/include"
                    )

if(TEST_BUILD)
message(STATUS "building MOCKS")
idf_component_get_property(tcp_transport_dir tcp_transport COMPONENT_DIR)
idf_component_get_property(esp_hw_support_dir esp_hw_support COMPONENT_DIR)
idf_component_get_property(esp_event_dir esp_event COMPONENT_DIR)
idf_component_get_property(log_dir log COMPONENT_DIR)
idf_component_get_property(freertos_dir freertos COMPONENT_DIR)
idf_component_get_property(http_parser_dir http_parser COMPONENT_DIR)
idf_component_get_property(esp_wifi_dir esp_wifi COMPONENT_DIR)
idf_component_get_property(esp_hw_support_dir esp_hw_support COMPONENT_DIR)
idf_component_get_property(esp_tls_dir esp-tls COMPONENT_DIR)
idf_component_get_property(esp_netif_dir esp_netif COMPONENT_DIR)
idf_component_get_property(esp_common_dir esp_common COMPONENT_DIR)
idf_component_get_property(esp_rom_dir esp_rom COMPONENT_DIR)
idf_component_get_property(esp_system_dir esp_system COMPONENT_DIR)
idf_component_get_property(mbedtls_dir mbedtls COMPONENT_DIR)

 idf_component_get_property(cmock_lib cmock COMPONENT_LIB)
 set(IDF_PATH $ENV{IDF_PATH})
 set(CMOCK_DIR "${IDF_PATH}/components/cmock/CMock")
 set(MOCK_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/mocks")
 set(ENV{UNITY_DIR} "$ENV{IDF_PATH}/components/cmock/CMock")
 file(MAKE_DIRECTORY ${MOCK_GEN_DIR})

 set(MOCK_OUTPUT
     "${MOCK_GEN_DIR}/Mockesp_transport.c" "${MOCK_GEN_DIR}/Mockesp_transport.h"
     "${MOCK_GEN_DIR}/Mockesp_transport_ssl.c" "${MOCK_GEN_DIR}/Mockesp_transport_ssl.h"
     "${MOCK_GEN_DIR}/Mockesp_transport_ws.c" "${MOCK_GEN_DIR}/Mockesp_transport_ws.h"
     "${MOCK_GEN_DIR}/Mockesp_transport_tcp.c" "${MOCK_GEN_DIR}/Mockesp_transport_tcp.h"
     "${MOCK_GEN_DIR}/Mockesp_event.c" "${MOCK_GEN_DIR}/Mockesp_event.h"
     "${MOCK_GEN_DIR}/Mockesp_mac.c" "${MOCK_GEN_DIR}/Mockesp_mac.h"
     "${MOCK_GEN_DIR}/Mockesp_random.c" "${MOCK_GEN_DIR}/Mockesp_random.h"
     "${MOCK_GEN_DIR}/Mockesp_system.c" "${MOCK_GEN_DIR}/Mockesp_system.h"
     "${MOCK_GEN_DIR}/Mockesp_tls.c" "${MOCK_GEN_DIR}/Mockesp_tls.h"
     "${MOCK_GEN_DIR}/Mockevent_groups.c" "${MOCK_GEN_DIR}/Mockevent_groups.h"
     "${MOCK_GEN_DIR}/Mockqueue.c" "${MOCK_GEN_DIR}/Mockqueue.h"
     "${MOCK_GEN_DIR}/Mocktask.c" "${MOCK_GEN_DIR}/Mocktask.h"
     "${MOCK_GEN_DIR}/Mockesp_log.c" "${MOCK_GEN_DIR}/Mockesp_log.h"
     "${MOCK_GEN_DIR}/Mockhttp_parser.c" "${MOCK_GEN_DIR}/Mockhttp_parser.h"
     )

 set(HEADERS_TO_MOCK
     ${tcp_transport_dir}/include/esp_transport_tcp.h
     ${tcp_transport_dir}/include/esp_transport_ws.h
     ${tcp_transport_dir}/include/esp_transport_ssl.h
     ${tcp_transport_dir}/include/esp_transport.h
     ${esp_event_dir}/include/esp_event.h
     ${esp_hw_support_dir}/include/esp_mac.h
     ${esp_hw_support_dir}/include/esp_random.h
     ${esp_system_dir}/include/esp_system.h
     ${esp_tls_dir}/esp_tls.h
     ${freertos_dir}/FreeRTOS-Kernel/include/freertos/queue.h
     ${freertos_dir}/FreeRTOS-Kernel/include/freertos/task.h
     ${freertos_dir}/FreeRTOS-Kernel/include/freertos/event_groups.h
     ${log_dir}/include/esp_log.h
     ${http_parser_dir}/http_parser.h
     )

 set(srcs
     ${MOCK_GEN_DIR}/Mockesp_transport.c
     ${MOCK_GEN_DIR}/Mockesp_transport_ws.c
     ${MOCK_GEN_DIR}/Mockesp_transport_ssl.c
     ${MOCK_GEN_DIR}/Mockesp_transport_tcp.c
     ${MOCK_GEN_DIR}/Mockesp_transport_tcp.c
     ${MOCK_GEN_DIR}/Mockesp_event.c
     ${MOCK_GEN_DIR}/Mockesp_mac.c
     ${MOCK_GEN_DIR}/Mockesp_random.c
     ${MOCK_GEN_DIR}/Mockesp_system.c
     ${MOCK_GEN_DIR}/Mockesp_tls.c
     ${MOCK_GEN_DIR}/Mockesp_log.c
     ${MOCK_GEN_DIR}/Mockhttp_parser.c
     ${MOCK_GEN_DIR}/Mockevent_groups.c
     ${MOCK_GEN_DIR}/Mockqueue.c
     ${MOCK_GEN_DIR}/Mocktask.c
    )

 add_custom_command(
     OUTPUT ruby_found SYMBOLIC
     COMMAND "ruby" "-v"
     COMMENT "Try to find ruby. If this fails, you need to install ruby"
 )

 add_custom_command(
     OUTPUT ${MOCK_OUTPUT}
     DEPENDS ruby_found
     COMMAND ${CMAKE_COMMAND} -E env "UNITY_DIR=${IDF_PATH}/components/unity/unity"
         ruby
         ${CMOCK_DIR}/lib/cmock.rb
         -o${CMAKE_CURRENT_SOURCE_DIR}/host_test/mocks/config.yaml
         ${HEADERS_TO_MOCK}
   )

 add_library(mocks ${srcs})
 target_include_directories(mocks PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/host_test/mocks/include
    ${tcp_transport_dir}/include
    ${esp_tls_dir}
    ${freertos_dir}/FreeRTOS-Kernel/include
    ${esp_event_dir}/include
    ${esp_system_dir}/include
    ${esp_common_dir}/include
    ${esp_wifi_dir}/include
    ${esp_hw_support_dir}/include
    ${esp_netif_dir}/include
    ${log_dir}/include
    ${esp_rom_dir}/include
    ${mbedtls_dir}/port/include
    ${http_parser_dir}
    ${mbedtls_dir}/mbedtls/include
    ${freertos_dir}/FreeRTOS-Kernel/include/freertos
    esp-mqtt/lib/include
    ${MOCK_GEN_DIR}
    )
 target_link_libraries(mocks PUBLIC ${cmock_lib})
 target_compile_definitions(mocks PUBLIC
     CONFIG_LOG_MAXIMUM_LEVEL=5
     CONFIG_LOG_DEFAULT_LEVEL=3
     CONFIG_ESP_TLS_USING_MBEDTLS
     CONFIG_ESP_TLS_SERVER
     CONFIG_LOG_TIMESTAMP_SOURCE_RTOS)
 target_link_options(${COMPONENT_LIB} INTERFACE -fsanitize=address)

 target_link_libraries(${COMPONENT_LIB} PUBLIC mocks)

else()
    idf_component_get_property(http_parser_lib http_parser COMPONENT_LIB)
    idf_component_get_property(tcp_transport_lib tcp_transport COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PUBLIC ${http_parser_lib} ${tcp_transport_lib})
endif()
