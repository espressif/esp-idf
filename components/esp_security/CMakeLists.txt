idf_build_get_property(target IDF_TARGET)
idf_build_get_property(non_os_build NON_OS_BUILD)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "")
set(priv_requires "")
set(priv_includes "")

if(NOT non_os_build)
    list(APPEND srcs "src/init.c")
    list(APPEND priv_includes "src/${IDF_TARGET}")

    if(CONFIG_SOC_HMAC_SUPPORTED)
        list(APPEND srcs "src/esp_hmac.c")
    endif()

    if(CONFIG_SOC_DIG_SIGN_SUPPORTED)
        list(APPEND srcs "src/esp_ds.c")
    endif()

    if(CONFIG_SOC_KEY_MANAGER_SUPPORTED)
        list(APPEND srcs "src/esp_key_mgr.c")
    endif()

    if(CONFIG_SOC_CRYPTO_DPA_PROTECTION_SUPPORTED)
        list(APPEND srcs "src/esp_dpa_protection.c")
    endif()

    list(APPEND srcs "src/esp_crypto_lock.c")
    list(APPEND priv_requires efuse esp_hw_support esp_system esp_timer)
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS ${priv_includes}
                    PRIV_REQUIRES ${priv_requires})

if(NOT non_os_build)
    target_link_libraries(${COMPONENT_LIB} PRIVATE "-u esp_security_init_include_impl")
endif()
