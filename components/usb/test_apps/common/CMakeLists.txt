idf_component_register(SRCS "dev_hid.c"
                            "dev_isoc.c"
                            "dev_msc.c"
                            "mock_msc.c"
                            "phy_common.c"
                            "hcd_common.c"
                       INCLUDE_DIRS
                             "../../private_include" # hcd and urb
                             "."
                       REQUIRES usb unity
                       PRIV_REQUIRES esp_mm
                       )
