# Register all of the "port" tests as a component

# In order for the cases defined by `TEST_CASE` in "port" to be linked into
# the final elf, the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRC_DIRS "."
                       PRIV_REQUIRES unity test_utils
                       WHOLE_ARCHIVE)
