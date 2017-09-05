# Touch Pad Read Example

Read and display raw values from capacitive touch pad sensors. 

Once configured, ESP32 is continuously measuring capacitance of touch pad sensors. Measurement is reflected as numeric value inversely related to sensor's capacitance. The capacitance is bigger when sensor is touched with a finger and the measured value smaller. In opposite situation, when finger is released, capacitance is smaller and the measured value bigger.

To detect when a sensor is touched and when not, each particular design should be calibrated by obtaining both measurements for each individual sensor. Then a threshold between both values should be established. Using specific threshold, API is then able to distinguish whether specific sensor is touched or released.

ESP32 supports reading up to ten capacitive touch pad sensors T0 - T9, connected to specific GPIO pins. For information on available pins please refer to [Technical Reference Manual](https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf). Application initializes all ten sensor pads. Then in a loop reads sensors T0 - T9 and displays obtained values (after a colon) on a serial terminal:

```
T0: 486 T1:   1 T2: 559 T3: 567 T4: 871 T5: 522 T6:1174 T7:1531 T8:1508 T9:1640
T0: 485 T1:   1 T2: 559 T3: 568 T4: 871 T5: 521 T6:1176 T7:1536 T8:1509 T9:1635
T0: 485 T1:   1 T2: 559 T3: 568 T4: 871 T5: 521 T6:1172 T7:1536 T8:1507 T9:1640
T0:  11 T1:   1 T2: 558 T3: 568 T4: 871 T5: 522 T6:1172 T7:1535 T8:1507 T9:1638
```

Above log is prepared using [ESP32 Demo Board V2](https://dl.espressif.com/dl/schematics/ESP32-Demo-Board-V2_sch.pdf) that has all ten pad sensors exposed. Values will be different depending on layout of pads on particular board.

![alt text](https://dl.espressif.com/dl/schematics/pictures/esp32-demo-board-v2.jpg "ESP32 Demo Board V2")

There is another similar example that demonstrates how to perform simple calibration and trigger an interrupt when a pat is touched - see [touch_pad_interrupt](../touch_pad_interrupt).

See the README.md file in the upper level 'examples' directory for more information about examples.
