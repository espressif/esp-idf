| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Import Third-Party CMake Library Example

This example demonstrates how to import third-party CMake libraries.

## Example Flow

[tinyxml2](https://github.com/leethomason/tinyxml2) is a small C++ XML parser.

It is imported, without modification, into the [tinyxml2](components/tinyxml2/) component. Please refer to the component CMakeLists.txt file for the description of the process: [components/tinyxml2/CMakeLists.txt](components/tinyxml2/CMakeLists.txt).

To demonstrate the library being used, a sample XML is embedded into the project. This sample XML is then read and parsed using `tinyxml2`. Please refer to the [main](main/) component for details.

### Output

```
I (317) example: Setting up...
I (317) example: Copying sample XML to filesystem...
I (647) example: Reading XML file
I (657) example: Read XML data:
<?xml version="1.0" encoding="UTF-8"?>
<note>
    <to>Tove</to>
    <from>Jani</from>
    <heading>Reminder</heading>
    <body>Don't forget me this weekend!</body>
</note>

I (667) example: Parsed XML data:

To: Tove
From: Jani
Heading: Reminder
Body: Don't forget me this weekend!
I (677) example: Example end
```
---

There is a discussion on importing third-party CMake libraries in the programming guide under `API Guides` -> `Build System` -> `Using Third-Party CMake Projects with Components`
