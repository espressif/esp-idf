# LVGLImage.py needs pypng and lz4. Install them into the IDF Python env
# when this example is configured, instead of adding them to IDF requirements.

idf_build_get_property(python PYTHON)
execute_process(
    COMMAND ${python} -c "import png, lz4.block"
    RESULT_VARIABLE example_lvgl_image_py_ok
    OUTPUT_QUIET
    ERROR_QUIET)

if(NOT example_lvgl_image_py_ok EQUAL 0)
    message(STATUS "Installing pypng and lz4 for LVGL image conversion")
    execute_process(
        COMMAND ${python} -m pip install pypng lz4
        RESULT_VARIABLE example_lvgl_image_pip_ok)
    if(NOT example_lvgl_image_pip_ok EQUAL 0)
        message(FATAL_ERROR "Failed to install pypng and lz4 (required by LVGLImage.py)")
    endif()
endif()
