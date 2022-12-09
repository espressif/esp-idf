# only compile the "uECC_verify_antifault.c" file which includes the "micro-ecc/uECC.c" source file
idf_component_register(SRCS "uECC_verify_antifault.c"
                    INCLUDE_DIRS . micro-ecc)
