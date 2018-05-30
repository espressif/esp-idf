# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(MAIN_SRCS main/esp_http_client_example.c main/app_wifi.c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp-http-client-example)

# Embed the server root certificate into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
target_add_binary_data(esp-http-client-example.elf
    "main/howsmyssl_com_root_cert.pem" TEXT)
