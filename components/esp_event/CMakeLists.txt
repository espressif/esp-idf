idf_build_get_property(target IDF_TARGET)
set(priv_include_dirs "private_include")
set(priv_requires "")
set(requires "log" "esp_common" "freertos")
set(srcs "default_event_loop.c"
         "esp_event.c"
         "esp_event_private.c")

if(${target} STREQUAL "linux")
    list(APPEND requires "linux")
    # Temporary fix until esp_system is available for linux, too
    list(APPEND priv_include_dirs "$ENV{IDF_PATH}/tools/mocks/esp_system/include")
else()
    list(APPEND priv_requires esp_timer)
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS ${priv_include_dirs}
                    REQUIRES ${requires}
                    PRIV_REQUIRES ${priv_requires}
                    LDFRAGMENTS linker.lf)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
