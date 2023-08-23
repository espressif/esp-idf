set(srcs "test_app_main.c"
         "test_mcpwm_cap.c"
         "test_mcpwm_cmpr.c"
         "test_mcpwm_fault.c"
         "test_mcpwm_gen.c"
         "test_mcpwm_oper.c"
         "test_mcpwm_sync.c"
         "test_mcpwm_timer.c"
         "test_mcpwm_common.c"
         "test_mcpwm_utils.c")

if(CONFIG_MCPWM_ISR_IRAM_SAFE)
    list(APPEND srcs "test_mcpwm_iram.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity driver
                       WHOLE_ARCHIVE)
