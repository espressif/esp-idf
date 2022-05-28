set(srcs "test_app_main.c"
         "test_mcpwm_cap.c"
         "test_mcpwm_cmpr.c"
         "test_mcpwm_fault.c"
         "test_mcpwm_gen.c"
         "test_mcpwm_oper.c"
         "test_mcpwm_sync.c"
         "test_mcpwm_timer.c"
         "test_mcpwm_utils.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       WHOLE_ARCHIVE)
