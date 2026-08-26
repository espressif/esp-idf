# Examples Environmental Capabilities

Examples environmental related capabilities files are describing the attributes which may be considered common among example projects. 

It may include but it is not limited to the following:

- SoC features like maximum/minimum values, available resources, etc. which are not captured in associated ``soc_caps.h`` and there is some reason they cannot be added to ``soc_caps.h``. For example, feature definition is not generic enough and is more related to examples use case or it is just some kind of approximation which is good enough for examples but not for SoC drivers.
- Various system configuration.
- Espressif's official development boards features.

## How to use

- Update the **Environmental Capabilities ``Kconfig``** with capabilities you consider could be shared among other example projects.

- Use ``orsource`` statement in **your example project ``Kconfig``** to include the Environmental Capabilities as shown below.

```
orsource "$IDF_PATH/examples/common_components/env_caps/$IDF_TARGET/Kconfig.env_caps"
```
