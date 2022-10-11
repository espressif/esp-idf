# Since the IDF bootloader is part of a project different from the application one, we cannot put bootloader wrappers
# inside the application. Thus, we need to create this bootloader component, which will be included inside the
# final bootloader binary, to store our wrappers.
idf_component_register(SRCS "wrapper.c"
                       # Since our source file doesn't contain any symbol strictly required by the linker, the latter
                       # may completely omit it and discard out wrapper. Thus, the following option will force it to
                       # include our object file inside the final binary.
                       WHOLE_ARCHIVE)

# Tell the linker that we want to redefine the function named `bootloader_print_banner`.
# We must now define a function named __wrap_bootloader_print_banner, which has the same
# signature as the former implementation.
target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=bootloader_print_banner")
