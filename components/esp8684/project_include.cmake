set(compile_options "-Wno-error=format="
                    "-nostartfiles"
                    "-Wno-format")

idf_build_set_property(COMPILE_OPTIONS "${compile_options}" APPEND)
