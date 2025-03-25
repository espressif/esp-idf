# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.

# USBH Layer mock
message(STATUS "building USBH Layer MOCKS")

idf_component_get_property(original_usb_dir usb COMPONENT_OVERRIDEN_DIR)

idf_component_mock(INCLUDE_DIRS "${original_usb_dir}/include"
                                "${original_usb_dir}/include/esp_private"
                                "${original_usb_dir}/include/usb"
                                "${original_usb_dir}/private_include"
                   MOCK_HEADER_FILES ${original_usb_dir}/private_include/hcd.h
                                     ${original_usb_dir}/private_include/usb_private.h
                   REQUIRES freertos)


# We do not mock usbh.c, we use the original implementation of it
target_sources(${COMPONENT_LIB} PRIVATE "${original_usb_dir}/usbh.c")
# Additionally, we include following src files from the above layers
target_sources(${COMPONENT_LIB} PRIVATE "${original_usb_dir}/usb_host.c")
target_sources(${COMPONENT_LIB} PRIVATE "${original_usb_dir}/enum.c")
target_sources(${COMPONENT_LIB} PRIVATE "${original_usb_dir}/hub.c")
