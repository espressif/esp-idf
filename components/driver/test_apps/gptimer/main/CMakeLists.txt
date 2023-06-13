set(srcs "test_app_main.c"
         "test_gptimer.c")

if(CONFIG_GPTIMER_ISR_IRAM_SAFE)
    list(APPEND srcs "test_gptimer_iram.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity driver
                       WHOLE_ARCHIVE)
