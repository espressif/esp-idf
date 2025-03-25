set(srcs)
list(APPEND srcs "test_main.cpp"
                 "usbh_install_unit_test.cpp"
                 )

idf_component_register(SRCS  ${srcs}
                        REQUIRES cmock usb
                        WHOLE_ARCHIVE)

# The following line would be needed to provide the 'main' function if this test used mocked FreeRTOS.
# As this test uses the real FreeRTOS implementation, we don't need Catch2 to provide 'main'.
#target_link_libraries(${COMPONENT_LIB} PRIVATE Catch2WithMain)
