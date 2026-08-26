# For this component, contraty to the bootloader's one, we don't need the WHOLE_ARCHIVE option.
# This is due to the fact that this source file, app_wrapper.c, contains a strong symbol that is required by the linker
# in order to generate the final binary: `app_main`. Thus, the whole app_wrapper object file, including the wrapper
# it contains, will be included inside the application binary.
idf_component_register(SRCS "app_wrapper.c")

# Tell the linker that we want to redefine the function named `esp_restart`.
# We must now define a function named __wrap_esp_restart, which has the same signature as the former implementation.
target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=esp_restart")
