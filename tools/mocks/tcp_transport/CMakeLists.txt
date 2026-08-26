# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building TCP TRANSPORT MOCKS")

idf_component_get_property(original_tcp_transport_dir tcp_transport COMPONENT_OVERRIDEN_DIR)

idf_component_mock(INCLUDE_DIRS "${original_tcp_transport_dir}/include"
                   MOCK_HEADER_FILES ${original_tcp_transport_dir}/include/esp_transport.h
                                     ${original_tcp_transport_dir}/include/esp_transport_tcp.h
                                     ${original_tcp_transport_dir}/include/esp_transport_ssl.h
                                     ${original_tcp_transport_dir}/include/esp_transport_ws.h
                   REQUIRES esp-tls
    )
