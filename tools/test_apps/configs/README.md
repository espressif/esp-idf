# Configs

Certain combinations of configs, e.g. stack and heap debug configs, are useful to set in most test apps.

To facilitiate re-use, this folder contains some of the most common ones.

These can then be added to the list of default configs in test apps:

```
list(PREPEND SDKCONFIG_DEFAULTS "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.debug_helpers" "sdkconfig.defaults")
```

Note that this must be set in the top-level project `CMakelist.txt`.

These files should not be considered stable, and are thus not recommended for use outside of IDF.
