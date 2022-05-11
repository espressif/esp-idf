idf_build_get_property(target IDF_TARGET)

set(srcs "test_app_main.c"
         "test_psram.c")

if(${target} STREQUAL "esp32")
    list(APPEND srcs "test_himem.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       WHOLE_ARCHIVE)
