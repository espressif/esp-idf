idf_build_get_property(target IDF_TARGET)

set(priv_requires "")

set(srcs "src/esp_timer.c"
         "src/esp_timer_init.c"
         "src/system_time.c"
         "src/esp_timer_impl_common.c")

if(CONFIG_ESP_TIMER_IMPL_TG0_LAC)
    list(APPEND srcs "src/esp_timer_impl_lac.c")
elseif(CONFIG_ESP_TIMER_IMPL_SYSTIMER)
    list(APPEND srcs "src/esp_timer_impl_systimer.c")
elseif(CONFIG_ESP_TIMER_IMPL_LINUX)
    if(APPLE)
        list(APPEND srcs "src/esp_timer_impl_apple.c")
    else()
        list(APPEND srcs "src/esp_timer_impl_linux.c")
    endif()
endif()

if(CONFIG_SOC_SYSTIMER_SUPPORT_ETM)
    list(APPEND srcs "src/esp_timer_etm.c")
endif()

if(NOT ${target} STREQUAL "linux")
    list(APPEND srcs "src/ets_timer_legacy.c")
    list(APPEND priv_requires esp_hal_timg esp_hal_systimer)
endif()


idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS include
                       PRIV_REQUIRES "${priv_requires}"
                       PRIV_INCLUDE_DIRS private_include)

# Forces the linker to include esp_timer_init.c
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_timer_init_include_func")

if(${target} STREQUAL "linux" AND NOT APPLE)
    # Link with librt, which contains POSIX real-time APIs, for example: clock_gettime, clock_settime, etc.
    target_link_libraries(${COMPONENT_LIB} PRIVATE "rt")
endif()
