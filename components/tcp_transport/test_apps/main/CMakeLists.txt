set(srcs "test_app_main.c" "test_transport_basic.c" "test_transport_connect" "test_transport_fixtures.c")
idf_component_register(SRCS ${srcs}
                    PRIV_INCLUDE_DIRS "../../private_include" "."
                    PRIV_REQUIRES cmock test_utils tcp_transport unity esp_psram
                    WHOLE_ARCHIVE)
