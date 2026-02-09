idf_component_register(SRCS "esp_rest_main.c"
                            "rest_server.c"
                       PRIV_REQUIRES esp_http_server nvs_flash
                       INCLUDE_DIRS ".")

if(CONFIG_EXAMPLE_DEPLOY_WEB_PAGES)
    set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../front/web-demo")
    if(EXISTS ${WEB_SRC_DIR}/dist)
        littlefs_create_partition_image(www ${WEB_SRC_DIR}/dist FLASH_IN_PROJECT)
    else()
        message(FATAL_ERROR "'${WEB_SRC_DIR}/dist' doesn't exist. Please run 'pnpm build' under '${WEB_SRC_DIR}'")
    endif()
endif()
