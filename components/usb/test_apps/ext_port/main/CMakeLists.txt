# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
set(srcs)

list(APPEND srcs "test_app_main.c"
                 "test_ext_port.c"
                 # Internal common test files
                 "hub_common.c"
                 "ext_port_common.c"
)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_INCLUDE_DIRS "."
                       REQUIRES usb unity common
                       WHOLE_ARCHIVE)
