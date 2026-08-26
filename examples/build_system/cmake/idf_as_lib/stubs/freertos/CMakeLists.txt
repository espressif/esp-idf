add_library(stub_freertos STATIC task.c)
target_include_directories(stub_freertos PUBLIC .)
add_library(stub::freertos ALIAS stub_freertos)
