# Register all of the "performance" tests as a component

# In order for the cases defined by `TEST_CASE` in "performance" to be linked into
# the final elf, the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRC_DIRS "."
                       PRIV_REQUIRES test_utils
                       WHOLE_ARCHIVE)

# Todo: Fix no-format errors
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
