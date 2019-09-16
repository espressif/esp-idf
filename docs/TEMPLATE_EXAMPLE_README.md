_Note that this is a template for an ESP-IDF example README.md file. When using this template, replace all these emphasised placeholders with example-specific content._

# _Example Title_

(See the README.md file in the upper level 'examples' directory for more information about examples.)

_What is this example? What does it do?_

_What features of ESP-IDF does it use?_

_What could someone create based on this example? ie applications/use cases/etc_

_If there are any acronyms or Espressif-only words used here, explain them or mention where in the datasheet/TRM this information can be found._

## How to use example

### Hardware Required

_If possible, example should be able to run on any commonly available ESP32 development board. Otherwise, describe what specific hardware should be used._

_If any other items (server, BLE device, app, second chip, whatever) are needed, mention them here. Include links if applicable. Explain how to set them up._

### Configure the project

```
idf.py menuconfig
```

* _If there is any project configuration that the user must set for this example, mention this here._

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

_Include an example of the console output from the running example, here:_

```
Use this style for pasting the log.
```

_If the user is supposed to interact with the example at this point (read/write GATT attribute, send HTTP request, press button, etc. then mention it here)_

_For examples where ESP32 is connected  with some other hardware, include a table or schematics with connection details._

## Troubleshooting

_If there are any likely problems or errors which many users might encounter, mention them here. Remove this section for very simple examples where nothing is likely to go wrong._

## Example Breakdown

_If the example source code is lengthy, complex, or cannot be easily understood, use this section to break down and explain the source code. This can be done by breaking down the execution path step by step, or explaining what each major function/task/source file does. Add sub titles if necessary. Remove this section for very simple examples where the source code is self explanatory._