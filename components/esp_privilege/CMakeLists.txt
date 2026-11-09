set(srcs)
# Requirements are resolved before the configuration is known, so they cannot depend on it.
set(priv_requires)
idf_build_get_property(target_arch IDF_TARGET_ARCH)
if(target_arch STREQUAL "riscv")
    list(APPEND priv_requires riscv)
endif()

if(CONFIG_ESP_PRIVILEGE_MODE_USER_CODE)
    list(APPEND srcs
         "port/riscv/privilege_user_mode.c"
         "port/riscv/privilege_user_mode_entry.S"
         "port/riscv/privilege_user_mode_init.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "private_include"
                       PRIV_REQUIRES ${priv_requires})

if(CONFIG_ESP_PRIVILEGE_MODE_USER_CODE)
    # Nothing references privilege_user_mode_init.c, whose init function registers the trap handlers
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_privilege_init_include_func")
endif()
