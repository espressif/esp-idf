# Touch Pad Interrupt Example

Demonstrates how to set up ESP32's capacitive touch pad peripheral to trigger interrupt when a pad is touched. 

Application has been developed and tested using [ESP32 Demo Board V2](https://dl.espressif.com/dl/schematics/ESP32-Demo-Board-V2_sch.pdf) that has ten capacitive sensor pads T0 to T9 exposed.

![alt text](http://dl.espressif.com/dl/schematics/pictures/esp32-demo-board-v2.jpg "ESP32 Demo Board V2")

The following output is shown when a pad is touched:

```
I (6303) Touch pad: Waiting for any pad being touched...
I (6733) Touch pad: T6 activated!
I (7333) Touch pad: T5 activated!
I (7723) Touch pad: T3 activated!
I (8043) Touch pad: T2 activated!
I (8883) Touch pad: T4 activated!
I (9523) Touch pad: T7 activated!
I (12503) Touch pad: Waiting for any pad being touched...
I (15483) Touch pad: T6 activated!
I (16253) Touch pad: T5 activated!
I (17903) Touch pad: Waiting for any pad being touched...
I (22903) Touch pad: Waiting for any pad being touched...
```

Note: Sensing threshold is set up automatically at start up by performing simple calibration. Application is reading current value for each pad and assuming half of this value as the sensing threshold. Do not touch pads on application start up, otherwise sensing may not work correctly.

For a simpler example how to configure and read capacitive touch pads, please refer to [touch_pad_read](../touch_pad_read).

See the README.md file in the upper level 'examples' directory for more information about examples.
