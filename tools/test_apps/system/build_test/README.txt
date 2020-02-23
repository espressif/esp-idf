This project is for testing if the application can be built with a particular sdkconfig setting.

To add new configuration, create one more sdkconfig.ci.NAME file in this directory.
To make the configuration target-specific, add a CONFIG_IDF_TARGET="name" line.

If you need to test for anything other than building, create another test project.
