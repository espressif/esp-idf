# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/toolchain.cmake)

project(subscribe_publish ASM C CXX)

set(MAIN_SRCS main/subscribe_publish_sample.c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Embed the certificate data directly in the built binary
target_add_binary_data(subscribe_publish.elf main/certs/aws-root-ca.pem TEXT)
target_add_binary_data(subscribe_publish.elf main/certs/certificate.pem.crt TEXT)
target_add_binary_data(subscribe_publish.elf main/certs/private.pem.key TEXT)
