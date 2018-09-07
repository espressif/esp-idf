# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(MAIN_SRCS main/chat_client.cpp)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(asio_chat_client)
