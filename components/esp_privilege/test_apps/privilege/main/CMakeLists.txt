idf_component_register(SRCS "test_app_main.c"
                            "test_privilege.c"
                            "privilege_test_fixture.c"
                       PRIV_REQUIRES esp_privilege unity umode_elf
                       WHOLE_ARCHIVE)

# Build the U-mode programs and embed their ELF files, see components/umode_elf/project_include.cmake
umode_elf_add_program(app_basic SRCS "user/app_basic.c")
