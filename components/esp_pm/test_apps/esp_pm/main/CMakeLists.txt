set(sources "test_app_main.c"
            "test_pm.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component must be registered as a WHOLE_ARCHIVE
idf_component_register(SRCS ${sources}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_pm ulp driver esp_timer
                       WHOLE_ARCHIVE)
