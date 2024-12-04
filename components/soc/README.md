# The SoC component

The `soc` component provides register-level descriptions for targets supported by ESP-IDF.

| File                | Description                                                                                   |
|---------------------|-----------------------------------------------------------------------------------------------|
| `xxx_reg.h`/`xx_struct.h` | Defines registers layout of a specific module. These files are automated, and should not be updated manually. <br/> Please note the register names and layout are subject to change between different chip series. |
| `xxx_pins.h`        | Defines the unchangeable GPIOs used by a specific module. <br/> e.g. if a high speed signal is routed through IO MUX, its corresponding GPIO is not selectable. |
| `soc_caps.h`        | Describes the differences in capabilities between different chips. <br/> The macros here can also affect cmake build system, Kconfig system, docs system, pytest and CI environment. <br/> **Changes to this file requires extra caution as they are part of the public API.** |
| `xxx_periph.h`      | This is the portal for each peripheral module at the SoC layer, <br/> containing all relevant register header files and organizing other key information, such as interrupt sources, hardware signal IDs, etc. |
| `xxx.peripherals.ld` | This is the linker script that defines each module's memory address. |

## The SoC Capabilities

There are two documents describing SoC capabilities, `soc_caps.h` and `soc_caps_full.h`. The former is a public header file, and the information in it is coarse-grained. The latter is a header file for internal developers that contains fine-grained module information. To used the soc capability macros, you should use the macro functions offered by `soc/soc_caps_eval.h`.

| Macro function | Description | Example |
|----------------|-------------|---------|
| `SOC_IS` | Checks if the current SoC is a specific one. | `SOC_IS(ESP32)` |
| `SOC_HAS` | Checks if the current SoC has a specific module. | `SOC_HAS(DAC)` |
| `SOC_MODULE_ATTR` | Get the attribute of a specific module. | `SOC_MODULE_ATTR(GPTIMER, TIMERS_TOTAL)` |
| `SOC_MODULE_SUPPORT` | Checks if the current SoC supports a specific feature. | `SOC_MODULE_SUPPORT(GPTIMER, ETM)` |
