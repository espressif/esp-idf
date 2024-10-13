idf_component_register(SRCS "dev_hid.c"
                            "dev_isoc.c"
                            "dev_msc.c"
                            "mock_msc.c"
                       INCLUDE_DIRS "."
                       REQUIRES usb unity)
