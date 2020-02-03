#
# CMakeLists.txt file for cryptoauthlib
#
cmake_minimum_required(VERSION 3.5)
# set(COMPONENT_SUBMODULES cryptoauthlib)
set(CRYPTOAUTHLIB_DIR cryptoauthlib/lib)
# set(COMPONENT_SRCDIRS       "${CRYPTOAUTHLIB_DIR}/atcacert"
#                             "${CRYPTOAUTHLIB_DIR}/basic"
#                             "${CRYPTOAUTHLIB_DIR}/crypto"
#                             "${CRYPTOAUTHLIB_DIR}/crypto/hashes"
#                             "${CRYPTOAUTHLIB_DIR}/host"
#                             "${CRYPTOAUTHLIB_DIR}/mbedtls"
#                             "${CRYPTOAUTHLIB_DIR}"
#                             "port"
#                             )
FILE(GLOB COMPONENT_SRCS    RELATIVE ${COMPONENT_DIR}
                            "${CRYPTOAUTHLIB_DIR}/atcacert/*.c"
                            "${CRYPTOAUTHLIB_DIR}/basic/*.c"
                            "${CRYPTOAUTHLIB_DIR}/crypto/*.c"
                            "${CRYPTOAUTHLIB_DIR}/crypto/hashes/*.c"
                            "${CRYPTOAUTHLIB_DIR}/host/*.c"
                            "${CRYPTOAUTHLIB_DIR}/mbedtls/*.c"
                            "${CRYPTOAUTHLIB_DIR}/*.c"
                            "port/*.c"
                            )
                        
set(COMPONENT_SRCS          "${CRYPTOAUTHLIB_DIR}/hal/atca_hal.c"
                            "${CRYPTOAUTHLIB_DIR}/hal/hal_freertos.c"
                            "${CRYPTOAUTHLIB_DIR}/hal/hal_esp32_i2c.c"
                            "${CRYPTOAUTHLIB_DIR}/hal/hal_esp32_timer.c"
                            "${COMPONENT_SRCS}"
                            )

message("SOURCE DIR PATH is " ${COMPONENT_DIR})
# string(REPLACE "${COMPONENT_DIR}" "." ${COMPONENT_SRCS} ${COMPONENT_SRCS})
message("Paths of SRCS are " ${COMPONENT_SRCS})
set(COMPONENT_INCLUDEDIRS   "${CRYPTOAUTHLIB_DIR}"
                            "${CRYPTOAUTHLIB_DIR}/hal"
                            "port"
                            )

set(COMPONENT_REQUIRES      mbedtls  freertos driver)

# Don't include the default interface configurations from cryptoauthlib
set(COMPONENT_EXCLUDE_SRCS "${CRYPTOAUTHLIB_DIR}/atca_cfgs.c")
set(COMPONENT_CFLAGS ESP32 ATCA_HAL_I2C ATCA_USE_RTOS_TIMER)

idf_component_register(     # SRC_DIRS        "${COMPONENT_SRCDIRS}"
                            SRCS            "${COMPONENT_SRCS}"
                            INCLUDE_DIRS    "${COMPONENT_INCLUDEDIRS}"
                            REQUIRES        "${COMPONENT_REQUIRES}"
                            EXCLUDE_SRCS    "${COMPONENT_EXCLUDE_SRCS}"
                            )

target_compile_definitions(${COMPONENT_LIB} PUBLIC ${COMPONENT_CFLAGS})
set_source_files_properties(${CRYPTOAUTHLIB_DIR}/hal/hal_freertos.c PROPERTIES COMPILE_FLAGS "-I$ENV{IDF_PATH}/components/freertos/include/freertos" )
# Turn off some warnings for some files that have been checked
set_source_files_properties(${CRYPTOAUTHLIB_DIR}/hal/hal_esp32_i2c.c PROPERTIES COMPILE_FLAGS "-Wno-unused-but-set-variable" COMPILE_FLAGS "-Wno-unused-variable")
set_source_files_properties(${CRYPTOAUTHLIB_DIR}/basic/atca_helpers.c PROPERTIES COMPILE_FLAGS "-Wno-type-limits")
