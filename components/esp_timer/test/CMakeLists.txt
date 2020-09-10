idf_component_register(SRC_DIRS "."
                       PRIV_INCLUDE_DIRS "../private_include"
                       PRIV_REQUIRES cmock test_utils)

if(CONFIG_ESP_TIMER_IMPL_FRC2)
    idf_build_set_property(COMPILE_DEFINITIONS "-DESP_TIMER_DYNAMIC_OVERFLOW_VAL" APPEND)
endif()
