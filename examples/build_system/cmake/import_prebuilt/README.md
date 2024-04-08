| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Import Prebuilt Library Example

This example illustrates how to import a prebuilt static library in the ESP-IDF build system.

## Example Flow

Users need to first build the project in the [prebuilt](prebuilt) subdirectory:

```
cd prebuilt
idf.py build
```

This builds a component named [prebuilt](prebuilt/components/prebuilt), which has private dependency on ESP-IDF components `spi_flash`, `log` and `app_update` (see [its CMakeLists.txt](prebuilt/components/prebuilt/CMakeLists.txt)). Once built, the archive file `libprebuilt.a`, along with the header file `prebuilt.h`, is automatically copied to the [`main` component](main) of this example project. 

The [`main` component's CMakeLists.txt](main/CMakeLists.txt) demonstrates how to import `libprebuilt.a` and link it to `main` so that the definitions inside can be used. 
It also demonstrates how to specify the same dependencies the original component had so as to properly resolve symbols used inside the prebuilt library.

Users can then return to this directory and build the main example:

```
cd ..
idf.py build
```


### Output

The example simply outputs the current running partition.

```
I (319) prebuilt: The running partition is 'factory'!
```

---

There is a discussion on importing prebuilt libraries in the programming guide under `API Guides` -> `Build System` -> `Using Prebuilt Libraries with Components`
