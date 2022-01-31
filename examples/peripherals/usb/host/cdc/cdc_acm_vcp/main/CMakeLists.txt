idf_component_register(SRCS "cdc_acm_vcp.cpp" "cp210x_usb.cpp" "ftdi_usb.cpp"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
