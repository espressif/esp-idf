set(srcs)
set(requires)
set(priv_requires)
idf_build_get_property(target_arch IDF_TARGET_ARCH)
if(target_arch STREQUAL "riscv")
    # esp_privilege_umode_domain.h includes riscv/sv32.h
    list(APPEND requires riscv)
endif()

if(CONFIG_ESP_PRIVILEGE_MODE_USER_CODE)
    list(APPEND srcs
         "port/riscv/privilege_user_mode.c"
         "port/riscv/privilege_user_mode_entry.S"
         "port/riscv/privilege_domain_address_space.c"
         "src/privilege_domain.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "private_include"
                       REQUIRES ${requires}
                       PRIV_REQUIRES ${priv_requires})

if(CONFIG_ESP_PRIVILEGE_MODE_USER_CODE)
    # Nothing references the init function of privilege_user_mode.c, which registers the trap handlers
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_privilege_init_include_func")
endif()
