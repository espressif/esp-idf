_Note that this is a template for an ESP-IDF example README.md file. When using this template, replace all the emphasized placeholders (in italics) with example-specific content._

_For your reference and inspiration, the application README already following this template is [ESP Timer Example](../examples/system/esp_timer/README.md)._

_While creating your example, use this template in conjunction with the guidelines given in [Creating Examples](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/contribute/creating-examples.html)._

| Supported Targets | _ESP32_ | _ESP32-XX_ |
| ----------------- | ------- | ---------- |

# _Example Title_

_Example Title should use the word "example" instead of "demo", "test" or any similar words._

(For general overview of examples and their usage, see the `README.md` file in the upper level 'examples' directory.)

> **Note**:
>
> After you click any link to [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/index.html), go to the top of the sidebar, then make sure you have the appropriate **Espressif chip** (target) and **ESP-IDF version** selected in the dropdown menus.

_What is this example? What does it do?_

_What feature(s) of ESP-IDF does this example use? Briefly introduce them and give a link to the API reference(s)._

_What could someone create based on this example? Think of applications, use cases, etc._

_If there are any acronyms or Espressif-only words used here, explain them or mention where in the hardware or software documentation this information can be found._


## Usage

The subsections below give only absolutely necessary information. For full steps to configure ESP-IDF and use it to build and run projects, see [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started).


### Hardware Required

_If possible, the example should be able to run on any commonly available development board based on a chip listed in the Supported Targets table. Otherwise, describe what specific hardware should be used._

_For beginner-friendly examples, provide a more detailed list of the required hardware. For more advanced examples, omit self-evident information._

_If any other items (server, BLE device, app, second chip, whatever) are needed, mention them here. Include the links if applicable. Explain how to set them up._

_For examples where a chip is connected to some other hardware, include a table or schematics with connection details._


### Software Required _(optional)_

_If any additional software is needed, mention it here and include the links. Mention the software version required if applicable. Explain how to set it up._


### Set Chip Target

First of all, your target must be supported by both:

- **By your ESP-IDF version**: For the full list of supported targets, run:
  ```
  idf.py --list-targets
  ```
- **By this example**: For the full list of supported targets,  refer to the supported targets table at the top of this README.

After you make sure that your target is supported, go to your example project directory and [set the chip target](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/tools/idf-py.html#select-the-target-chip-set-target):

```
idf.py set-target <target>
```

For example, to set esp32 as the chip target, run:

```
idf.py set-target esp32
```


### Configure the Project

_Set the required Kconfig options differing from the ESP-IDF defaults in [sdkconfig.defaults](./sdkconfig.defaults) and add comments there on WHY these options are needed. (Remember, WHAT these options do is already documented in the Project Configuration document, so no need to repeat this information.)_

_If the required Kconfig options are pre-set in `sdkconfig.defaults`, add the following sentences as a separate paragraph: This example does not need configuration. The required Kconfig options differing from the ESP-IDF defaults are pre-set for this particular example in [sdkconfig.defaults](./sdkconfig.defaults)._

For information about Kconfig options, see [Project Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/kconfig.html) > _Name of relevant section(s)_.

To conveniently check or modify Kconfig options for this example in a project configuration menu, run:

```
idf.py menuconfig
```

_If there is anything else that the user must configure for this example to work, mention it here._


### Build and Flash

Execute the following command to build the project, flash it to your development board, and run the monitor tool to view the serial output:

```
idf.py build flash monitor
```

This command can be reduced to `idf.py flash monitor`.

If the above command fails, check the log on the serial monitor which usually provides information on the possible cause of the issue.

To exit the serial monitor, use `Ctrl` + `]`.


## Example Output

_For different reasons, an example's console output might not be informative enough for the user to realize that something went wrong. To allow users to check that the example is running correctly on their side, be sure to include here the expected console output as given below._

If you see the following console output, your example should be running correctly:

```
Paste console output as a code snippet like this
```

_If the user is supposed to interact with the example at this point, mention it here (read/write GATT attribute, send HTTP request, press button, etc.)_


## Example Breakdown _(optional)_

_If the example source code is lengthy, complex, or cannot be easily understood, use this section to break down and explain the source code. This can be done by breaking down the execution path step by step, or explaining what each major function/task/source file does. Add sub titles if necessary. Remove this section for very simple examples where the source code is self explanatory._


## Troubleshooting

_Remove this section for very simple examples where nothing is likely to go wrong._

_There might be typical issues or errors that the user is likely to encounter while working with this example. If the issue is related to the ESP-IDF feature in general, mention it in Troubleshooting of this feature's API reference. If the issue is related to this example only, mention such issue here._

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub.


## Reference

_Provide references to important documents_

- Link to the ESP-IDF feature's API reference, for example [ESP-IDF: ESP Timer feature](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/esp_timer.html)
- [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started)
- [Project Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/kconfig.html) (Kconfig Options)
