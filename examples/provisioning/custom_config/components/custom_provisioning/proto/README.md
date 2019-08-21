# Protobuf files for defining custom config-data packet structures

This is an example proto file defining custom configuration related data packet structures, namely -
1. CustomConfigRequest - for sending configuration data consisting of various fields (Info and Version)
2. CustomConfigResponse - for receiving configuration status (fail/success)

Note : These proto files are not automatically compiled during the build process.

# Compilation

Compilation requires protoc (Protobuf Compiler) and protoc-c (Protobuf C Compiler) installed. Since the generated files are to remain the same, as long as the proto files are not modified, therefore the generated files are already available under `examples/provisioning/custom_config/components/custom_provisioning/proto-c` and `examples/provisioning/custom_config/components/custom_provisioning/python` directories, and thus running cmake / make (and installing the Protobuf compilers) is optional.

If using `cmake` follow the below steps. If using `make`, jump to Step 2 directly.

## Step 1 (Only for cmake)

When using cmake, first create a build directory and call cmake from inside:

```
mkdir build
cd build
cmake ..
```

## Step 2

Simply run `make` to generate the respective C and Python files. The newly created files will overwrite those under `examples/provisioning/custom_config/components/custom_provisioning/proto-c` and `examples/provisioning/custom_config/components/custom_provisioning/python`
