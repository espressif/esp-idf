set(priv_requires unity console)

if(NOT CONFIG_IDF_TARGET_LINUX)
    list(APPEND priv_requires esp_driver_uart)
endif()

idf_component_register(SRCS "test_app_main.c" "test_console.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE)
