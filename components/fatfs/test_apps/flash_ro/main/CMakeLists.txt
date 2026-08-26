idf_component_register(SRCS "test_fatfs_flash_ro.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity spi_flash fatfs vfs test_fatfs_common
                       WHOLE_ARCHIVE)


set(out_dir "${CMAKE_CURRENT_BINARY_DIR}/fatfs_image")

# This helper function creates a set of files expected by the test case.
# The files are then added into the FAT image by 'fatfs_create_rawflash_image' below.
function(create_test_files)
    message(STATUS "Generating source files for test_fatfs_flash_ro in ${out_dir}...")

    # used in "(raw) can read file"
    file(WRITE "${out_dir}/hello.txt" "Hello, World!\n")

    # used in "(raw) can open maximum number of files"
    foreach(i RANGE 1 32)
        file(WRITE "${out_dir}/f/${i}.txt")
    endforeach()

    # used in "(raw) opendir, readdir, rewinddir, seekdir work as expected"
    file(WRITE "${out_dir}/dir/1.txt")
    file(WRITE "${out_dir}/dir/2.txt")
    file(WRITE "${out_dir}/dir/boo.bin")
    file(WRITE "${out_dir}/dir/inner/3.txt")

    # used in "(raw) multiple tasks can use same volume"
    foreach(i RANGE 1 4)
        string(REPEAT "${i}" 32000 file_content)
        file(WRITE "${out_dir}/ccrnt/${i}.txt" ${file_content})
    endforeach()

    # used in "(raw) read speed test"
    string(REPEAT "a" 262144 file_content)
    file(WRITE "${out_dir}/256k.bin" ${file_content})
endfunction()

create_test_files()

fatfs_create_rawflash_image(storage ${out_dir} FLASH_IN_PROJECT PRESERVE_TIME)
