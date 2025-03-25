# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.

# USB Host layer mock
message(STATUS "building USB HOST Layer MOCKS")

idf_component_get_property(original_usb_dir usb COMPONENT_OVERRIDEN_DIR)

idf_component_mock(INCLUDE_DIRS "${original_usb_dir}/include"
                                "${original_usb_dir}/include/esp_private"
                                "${original_usb_dir}/include/usb"
                                "${original_usb_dir}/private_include"
                   MOCK_HEADER_FILES ${original_usb_dir}/private_include/enum.h
                                     ${original_usb_dir}/private_include/hcd.h
                                     ${original_usb_dir}/private_include/hub.h
                                     ${original_usb_dir}/private_include/usbh.h
                                     ${original_usb_dir}/include/esp_private/usb_phy.h
                   REQUIRES freertos)

# We do not mock usb_host.c, we use the original implementation of it
target_sources(${COMPONENT_LIB} PRIVATE "${original_usb_dir}/usb_host.c")
# Original implementation of usb_private.c to allocate memory for transfers
target_sources(${COMPONENT_LIB} PRIVATE "${original_usb_dir}/usb_private.c")
# We do not mock usb_helpers. We use the original implementation
# This way, we can test descriptor parsing
target_sources(${COMPONENT_LIB} PRIVATE "${original_usb_dir}/usb_helpers.c")
