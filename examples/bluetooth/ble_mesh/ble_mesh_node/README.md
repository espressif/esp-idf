ESP-IDF BLE Mesh Node demo
==========================
Configuration Server Model
==========================
Generic OnOff Server Model
==========================

This demo demonstrates the use of BLE mesh as a node device having:

- Only one element
- Two sig models:
	- Configuration Server Model: The role of this model is mainly to configure the APP key and set the relay function, set the TTL size, set subscription, etc for the provisioner device.
   - OnOff Server Model: This model realizes the most basic function of turning on and off the light;
- The default of this demo is to enable the 20ms non-connectable interval advertising function in BLE 5.0. To close it, please 
   make menuconfig --> Compoment config --> Example 'BLE Mesh' Config --> if you need to Send in 20ms ......
- In order to better achieve the demonstration effect, we added an RGB LED on the ESP32-WROOM-32 board. Their corresponding GPIO pins are GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27.
   So if you want to achieve the same effect, just solder an RGB LED onto the ESP32-DevKitC to the same GPIO pins as us and 
   make menuconfig --> Compoment config --> Example 'BLE Mesh' Config --> If used the esp_wroom_32 ...... option can be selected.

Please check the [tutorial](tutorial/Ble_Mesh_Node_Example_Walkthrough.md) for more information about this example.
