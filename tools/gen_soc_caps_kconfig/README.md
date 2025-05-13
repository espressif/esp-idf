# Generate kconfig variables from SoC caps headers

`gen_soc_caps_config.py` is used to turn defines from `soc_caps.h` into kconfig variables that can be included and used both in menuconfig and during the build process

The script will automatically run as part of the pre-commit hook if any of the `soc_caps` related files have been modified.

To run it manually, navigate to `${IDF_PATH}` and execute:

    `python tools/gen_soc_caps_kconfig/gen_soc_caps_kconfig.py -d 'components/soc/*/include/soc/'`

The results are `kconfig.soc_caps.in`-files containing the bool/int/hex/string values defined in the `soc_caps.h` file.

This target specific kconfig file is then imported in `$IDF_PATH/Kconfig`.

# Tips
Defines that are 0 or 1 will automatically be interpreted as Booleans. If it's necessary to have this variable available as an integer instead then this can be achieved by adding an `U` suffix when defining it. E.g:

    `#define SOC_I2C_NUM (1U)`


The script is not able to evaluate expressions, e.g X*Y. If you need a variable to be available in kconfig it needs to be a simple assignment expression.

If some `#define` statement should be excluded from being converted into config option, use `// [gen_soc_caps:ignore]` pragma.