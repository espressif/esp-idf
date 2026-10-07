idf_component_register(SRCS "test_app_main.c"
                            "privilege_test_fixture.c"
                       PRIV_INCLUDE_DIRS "../../../private_include"
                       PRIV_REQUIRES esp_privilege unity esp_system
                       WHOLE_ARCHIVE)
