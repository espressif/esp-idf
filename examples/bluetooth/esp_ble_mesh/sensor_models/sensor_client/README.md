| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | --------- | -------- | -------- |

ESP BLE Mesh Sensor Client Example
==================================

This example demonstrates how to create a sensor client model in Provisioner, and the [sensor server example](../sensor_server) demonstrates how to create a sensor server model and a sensor setup server model in an unprovisioned device.

### 1. Introduction

Sensor Server model is model which is used to expose a series of sensor states. When Sensor Server model is present on an element, the Sensor Setup Server model shall also be present as an extension of the Sensor Server model.
Sensor Client model is used to consume the state values exposed by the Sensor Server/Setup Server models via Sensor messages.

Sensor state is a composite state which contains the following parts:
* Sensor Descriptor state
* Sensor Setting state
* Sensor Cadence state
* Sensor Data state
* Sensor Series Column state

#### Sensor Descriptor state

Sensor Descriptor state represents the attributes describing the sensor data. This state does not change throughout the lifetime of an element.

#### Sensor Setting state

Sensor Setting state controls parameters of a sensor. For example, an occupancy sensor may have a “sensitivity” setting that controls the sensitivity of the sensor. Sensitivity may be adjusted to prevent small animals from triggering the sensor.

#### Sensor Cadence state

Sensor Cadence state controls the cadence of sensor reports. It allows a sensor to be configured to send measured values using Sensor Status messages at a different cadence for a range of measured values. It also allows a sensor to be configured to send measured values when the value changes up or down by more than a configured delta value.

#### Sensor Data state

Sensor Data state is a sequence of one or more pairs of Sensor Property ID and Raw Value fields, with each Raw Value field size and representation defined by the characteristics referenced by the Sensor Property ID.

#### Sensor Series Column state

Values measured by sensors may be organized as arrays (and represented as series of columns, such as histograms). It contains Sensor Raw Value X, Sensor Column Width and Sensor Raw Value Y.

> Note: For the supported sensor types, please refer to the [Mesh Device Properties Spec](https://www.bluetooth.com/specifications/mesh-specifications/mesh-properties/).

### 2. Example Description

#### Sensor Client example
In the [Sensor Client example](./), the mesh device is a Provisioner with a sensor client model. Once the device which runs the sensor server example is provisioned and configured successfully by the Provisioner, users can press the button to send sensor messages to get the corresponding state (i.e. sensor descriptor, sensor data, etc.) values.

#### Sensor Server example

In the [Sensor Server example](../sensor_server), the mesh device is an unprovisioned device with a sensor server model and a sensor setup server model.

The sensor server model supports two instances of Sensor states, the first one (Sensor Property ID 0x0056) represents the "Present Indoor Ambient Temperature", and the other (Sensor Property ID 0x005B) represents the "Present Outdoor Ambient Temperature". The characteristic of the two properties is "Temperature 8", which is used to represent a measure of temperature with a unit of 0.5 degree Celsius. The minimum value of the characteristic is -64.0, and the maximum value is 63.5.

Currently only the Sensor Descriptor state, Sensor Data state are supported by the Sensor states. The Sensor Property ID, Sensor Descriptor state, format and length of the corresponding Sensor Data state are all pre-initialized.

### 3. Procedures
* Device A runs the sensor client example, and device B runs the sensor server example;
* Working as a Provisioner, once the unprovisioned device beacon from device B is received by device A, device A will provision and configure device B into a node;
* After the provisioning and configuration procedure is completed successfully, users can press the "Boot" button on device A to get the sensor state values of device B;
* Based on the press count, different sensor messages will be sent to device B. Sensor Descriptor Get will be sent when the button is pressed for the first time, at the second time Sensor Cadence Get will be sent, and the following messages are: Sensor Settings Get, Sensor Get and Sensor Series Get;
* If continuing to press the button, the same sensor messages with the above sequence will be sent to device B.

### 4. Relationship

This section is about the relationship between sensor models and messages, which gives a list of messages that can be sent or published by a specific sensor model.

* Sensor Client model
    * Sensor Descriptor Get
    * Sensor Cadence Get
    * Sensor Cadence Set
    * Sensor Cadence Set Unacknowledged
    * Sensor Settings Get
    * Sensor Setting Get
    * Sensor Setting Set
    * Sensor Setting Set Unacknowledged
    * Sensor Get
    * Sensor Column Get
    * Sensor Series Get

* Sensor Server model
    * Sensor Descriptor Status
    * Sensor Status
    * Sensor Column Status
    * Sensor Series Status

* Sensor Setup Server model
    * Sensor Cadence Status
    * Sensor Settings Status
    * Sensor Setting Status
