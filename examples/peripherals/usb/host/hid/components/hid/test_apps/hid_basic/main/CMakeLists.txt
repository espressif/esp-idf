# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRC_DIRS "." "../../../"
                       INCLUDE_DIRS "." "../../../include" ${CMAKE_CURRENT_BINARY_DIR}
                       REQUIRES usb unity esp_common
                       WHOLE_ARCHIVE)
