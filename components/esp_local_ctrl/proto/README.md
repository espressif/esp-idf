# Protobuf files for defining ESP Local Control message structures

The proto files under this directory are used by esp_local_ctrl for defining protobuf messages which are sent and received over protocomm transport layer. These proto files cannot be used directly and have to be compiled into C and Python files. The generated C files are used by esp_local_ctrl itself to create, delete and manipulate transaction packets. The generated Python files can be used by python based applications for implementing client side interface to esp_local_ctrl service running on a device.

Note : These proto files are not automatically compiled during the build process.

# Compilation

Compilation requires protoc (Protobuf Compiler) and protoc-c (Protobuf C Compiler) installed. Since the generated files are to remain the same, as long as the proto files are not modified, therefore the generated files are already available under `components/esp_local_ctrl/proto-c` and `components/esp_local_ctrl/python` directories, and thus running `cmake` / `make` (and installing the Protobuf compilers) is optional.

If using `cmake` follow the below steps. If using `make`, jump to Step 2 directly.

## Step 1 (Only for cmake)

When using cmake, first create a build directory and call cmake from inside:

```
mkdir build
cd build
cmake ..
```

## Step 2

Simply run `make` to generate the respective C and Python files. The newly created files will overwrite those under `components/esp_local_ctrl/proto-c` and `components/esp_local_ctrl/python`
