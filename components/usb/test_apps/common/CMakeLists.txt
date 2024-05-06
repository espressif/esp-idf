idf_component_register(SRCS "mock_hid.c"
                            "mock_msc.c"
                            "test_usb_common.c"
                       INCLUDE_DIRS "."
                       REQUIRES usb unity)
