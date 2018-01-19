# Set some global esptool.py variables
set(ESPTOOLPY "${PYTHON}" "${CMAKE_CURRENT_LIST_DIR}/esptool/esptool.py" --chip esp32)
set(ESPSECUREPY "${PYTHON}" "${CMAKE_CURRENT_LIST_DIR}/esptool/espsecure.py")

set(ESPPORT $ENV{ESPPORT})
if(NOT ESPPORT)
  set(ESPPORT ${CONFIG_ESPTOOLPY_PORT})
endif()

set(ESPBAUD $ENV{ESPPORT})
if(NOT ESPBAUD)
  set(ESPPORT ${CONFIG_ESPTOOLPY_PORT})
endif()

set(ESPFLASHMODE ${CONFIG_ESPTOOLPY_FLASHMODE})
set(ESPFLASHFREQ ${CONFIG_ESPTOOLPY_FLASHFREQ})
set(ESPFLASHSIZE ${CONFIG_ESPTOOLPY_FLASHSIZE})

set(ESPTOOLPY_SERIAL "${ESPTOOLPY}" --port "${ESPPORT}" --baud ${ESPBAUD})

if(CONFIG_ESPTOOLPY_FLASHSIZE_DETECT)
  set(flashsize_arg detect)
else()
  set(flashsize_arg ${ESPFLASHSIZE})
endif()

set(ESPTOOLPY_ELF2IMAGE_FLASH_OPTIONS --flash_mode ${ESPFLASHMODE} --flash_freq ${ESPFLASHFREQ} --flash_size ${ESPFLASHSIZE})

set(ESPTOOLPY_WRITE_FLASH_OPTIONS --flash_mode ${ESPFLASHMODE} --flash_freq ${ESPFLASHFREQ} --flash_size ${flashsize_Arg})

set(ESPTOOLPY_WRITE_FLASH ${ESPTOOLPY_SERIAL} write_flash ${ESPTOOLPY_WRITE_FLASH_OPTIONS})

#
# Add 'binary' target - generates with elf2image
#
add_custom_command(OUTPUT "${PROJECT_NAME}.bin"
  COMMAND ${ESPTOOLPY} elf2image ${ESPTOOLPY_ELF2IMAGE_FLASH_OPTIONS} -o "${PROJECT_NAME}.bin" "${PROJECT_NAME}.elf"
  DEPENDS ${PROJECT_NAME}.elf
  VERBATIM
  )
add_custom_target(binary ALL DEPENDS "${PROJECT_NAME}.bin")

#
# Add 'flash' target - not all build systems can run this directly
#
add_custom_target(flash DEPENDS binary partition_table bootloader_subproject)
# TODO: this target should call "idftool" not esptool directly, so it can
# override things (port, baud, etc) at runtime not configure time
