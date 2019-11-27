# Touch Pad Read Example

## ESP32 plaform

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

## ESP32-S2 platform

Read and display raw values from capacitive touch pad sensors. 

Once configured, ESP32-S2 is continuously measuring capacitance of touch pad sensors. Measurement is reflected as numeric value inversely related to sensor's capacitance. The capacitance is bigger when sensor is touched with a finger and the measured value bigger. In opposite situation, when finger is released, capacitance is smaller and the measured value smaller.

To detect when a sensor is touched and when not, each particular design should be calibrated by obtaining both measurements for each individual sensor. Then a threshold between both values should be established. Using specific threshold, API is then able to distinguish whether specific sensor is touched or released. For ESP32-S2, the hardware integrates the edge detection algorithm, which can achieve the purpose of detecting touch actions by configuring appropriate parameters. Please find the example in [touch_pad_interrupt](../touch_pad_interrupt).

ESP32-S2 supports reading up to 14 capacitive touch pad sensors T1 - T14, connected to specific GPIO pins. For information on available pins please refer to ESP32-S2 Technical Reference Manual. T0 is the internal denoise channel used to filter out system noise and there is no corresponding external GPIO. Application initializes 14 sensor pads. Then in a loop reads sensors T1 - T14 and displays obtained values (after a colon) on a serial terminal:

```
Touch Sensor read, the output format is:
Touchpad num:[raw data]

T1: [6473] T2: [6507] T3: [6638] T4: [8917] T5: [9053] T6: [7190] T7: [7176] T8: [7416] T9: [7145] T10: [7387] T11: [7973] T12: [7776] T13: [8151] T14: [8190]
T1: [6463] T2: [6512] T3: [6643] T4: [8920] T5: [9050] T6: [7191] T7: [7176] T8: [7416] T9: [7143] T10: [7387] T11: [7974] T12: [7778] T13: [8152] T14: [8192]
T1: [6476] T2: [6508] T3: [6641] T4: [8919] T5: [9053] T6: [7190] T7: [7177] T8: [7416] T9: [7143] T10: [7386] T11: [7974] T12: [7776] T13: [8153] T14: [8193]
```

## Reference Information

For hardware and firmware design guidelines on ESP32 touch sensor system, please refer to [Touch Sensor Application Note](https://github.com/espressif/esp-iot-solution/blob/master/documents/touch_pad_solution/touch_sensor_design_en.md), where you may find comprehensive information on how to design and implement touch sensing applications, such as linear slider, wheel slider, matrix buttons and spring buttons.

There is another similar example that demonstrates how to perform simple calibration and trigger an interrupt when a pat is touched - see [touch_pad_interrupt](../touch_pad_interrupt).

See the README.md file in the upper level 'examples' directory for more information about examples.
