ESP BLE Mesh Client Model Demo
========================

This is a demo of using Generic OnOff client model to get/set the generic onoff state and the following are the basic procedures for this demo.

1. Download and run this demo;
2. Use App to provision this device and another device which runs the Generic OnOff server demo;
3. After both onoff client and server devices are provisioned, use UART1 to input the unicast address of the element within the server device;
4. The Generic OnOff client will start to get and set the Generic OnOff state periodically.

>Note:
>1. The netkey index and appkey index are fixed to 0x0000 in this demo.
>2. When the client device is re-provisioned but the server device is not, the first several get/set messages will be treated as replay attacks, so better to re-provision both devices.