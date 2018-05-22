# Touch Pad Read Example

Read and display raw values or IIR filtered values from capacitive touch pad sensors. 

Once configured, ESP32 is continuously measuring capacitance of touch pad sensors. Measurement is reflected as numeric value inversely related to sensor's capacitance. The capacitance is bigger when sensor is touched with a finger and the measured value smaller. In opposite situation, when finger is released, capacitance is smaller and the measured value bigger.

To detect when a sensor is touched and when not, each particular design should be calibrated by obtaining both measurements for each individual sensor. Then a threshold between both values should be established. Using specific threshold, API is then able to distinguish whether specific sensor is touched or released.

ESP32 supports reading up to ten capacitive touch pad sensors T0 - T9, connected to specific GPIO pins. For information on available pins please refer to [Technical Reference Manual](https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf). Application initializes all ten sensor pads. Then in a loop reads sensors T0 - T9 and displays obtained values (after a colon) on a serial terminal:

```
Touch Sensor filter mode read, the output format is: 
Touchpad num:[raw data, filtered data]

T0:[1072,1071] T1:[ 475, 475] T2:[1004,1003] T3:[1232,1231] T4:[1675,1676] T5:[1146,1146] T6:[1607,1607] T7:[1118,1118] T8:[1695,1695] T9:[1223,1222] 
T0:[1072,1071] T1:[ 475, 475] T2:[1003,1003] T3:[1231,1231] T4:[1676,1676] T5:[1146,1146] T6:[1607,1607] T7:[1118,1118] T8:[1694,1694] T9:[1222,1221] 
T0:[1071,1071] T1:[ 475, 475] T2:[1004,1004] T3:[1231,1231] T4:[1678,1677] T5:[1147,1146] T6:[1607,1607] T7:[1118,1118] T8:[1694,1694] T9:[1222,1221] 
```
For hardware and firmware design guidelines on ESP32 touch sensor system, please refer to [Touch Sensor Application Note](https://github.com/espressif/esp-iot-solution/blob/master/documents/touch_pad_solution/touch_sensor_design_en.md), where you may find comprehensive information on how to design and implement touch sensing applications, such as linear slider, wheel slider, matrix buttons and spring buttons.

There is another similar example that demonstrates how to perform simple calibration and trigger an interrupt when a pat is touched - see [touch_pad_interrupt](../touch_pad_interrupt).

See the README.md file in the upper level 'examples' directory for more information about examples.
