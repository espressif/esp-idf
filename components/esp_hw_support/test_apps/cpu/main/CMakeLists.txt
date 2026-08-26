set(srcs "test_app_main.c"
         "test_cpu_basic.c"
         "test_cpu_interrupts.c"
         "test_cpu_reset.c"
         "test_cpu_watchpoint.c"
         "test_cpu_breakpoint.c"
         "test_cpu_non_recoverable.c"
         "test_cpu_stall.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       REQUIRES unity test_utils esp_hw_support esp_system
                       WHOLE_ARCHIVE)
