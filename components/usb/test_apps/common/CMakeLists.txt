idf_component_register(SRCS "test_usb_common.c" "test_usb_mock_msc.c" "test_usb_mock_hid.c"
                       INCLUDE_DIRS "."
                       REQUIRES usb unity)
