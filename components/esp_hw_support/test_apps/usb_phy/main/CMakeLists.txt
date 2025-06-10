# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRC_DIRS "."
                       PRIV_INCLUDE_DIRS "."
                       REQUIRES usb unity
                       WHOLE_ARCHIVE)
