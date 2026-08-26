## Linker Script Generator

Contains code that implements linker script generation, `ldgen`. For more information about the feature,
see `docs/en/api-guides/linker-script-generation.rst`.

### Source Files

The following are the source files in the directory:

- `ldgen.py` - Python executable that gets called during build.
- `entity.py` - contains classes related to entities (library, object, symbol or combination of the above) with mappable input sections.
- `fragments.py` - contains classes for parsing the different types of fragments in linker fragment files.
- `generation.py` - contains bulk of the logic used to process fragments into output commands.
- `sdkconfig.py` - used for evaluating conditionals in fragment files.
- `linker_script.py` - augments the input linker script template with output commands from generation process to produce the output linker script.
- `output_commands.py` - contains classes that represent the output commands in the output linker script.
- `ldgen_common.py` - contains miscellaneous utilities/definitions that can be used in the files mentioned above.

### Tests

Unit tests are in the `test` directory. These tests are run as part of CI in the job `test_ldgen_on_host`.

There is also a test app for `ldgen` in `tools/test_apps/build_system/ldgen_test`.

### Build System

Linker script generation is a part of the build process. The build scripts `tools/cmake/ldgen.cmake`
and `make/ldgen.mk` contain the build-system-side implementation for CMake and Make, respectively.

### Basic Flow

The build system invokes `ldgen.py`, passing some information from the build.

The linker fragment files are parsed by `fragments.py`, evaluating conditional expressions
with `sdkconfig.py`. 

From the parsed fragments, `generation.py` generates output commands defined in `output_commands.py`,
with some help from `entity.py`.

`linker_script.py` writes the output linker script, replacing markers with output commands generated.

More details about the implementation are in the respective source files.