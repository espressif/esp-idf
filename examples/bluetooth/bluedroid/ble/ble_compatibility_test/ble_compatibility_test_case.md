# Test Case for BLE Smartphone Compatibility 

This document provides a test case for BLE smartphone compatibility and includes detailed procedures for various test items.
  
## Preparation

### What You Need

* ESP device which needs to flash [this test program] (https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/bluedroid/ble/ble_compatibility_test/main/ble_compatibility_test.c)
* Smartphone with LightBlue® Explorer app

### Initialization

Prior to conducting tests, please initialize the smartphone and the ESP device as follows:

* Set the device name as `BLE_COMP_TEST`.
* Set the maximum transmission unit (MTU) of the device to 33 bytes, to test the assembly and division of data packets.
* If the smartphone has been paired with the ESP device before, please delete the pairing in the Bluetooth setting as follows: `Bluetooth` -> `My Devices` -> `Find this device with "i" in a circle on the right` -> `Forget this device`. Then restart the Bluetooth service.
* Before flashing the test program onto the ESP device, make sure to erase the contents of the flash by executing the command `make erase_flash flash` in the Terminal.
* When the ESP device restarts, the pairing information will be erased automatically. After that, make sure that the pairing information in the Bluetooth setting of the smartphone is deleted.

**Note:**
   
* For tests marked with (*) further in the document, please bear in mind the following:
	* Your phone performance may affect the results of these tests. If such a test fails, it does not mean the phone fails to meet the test requirements, but that you need to arrange targeted tests.
	* Taking "Test for Connection Success Rate" as an example: if the test cannot be passed for 10 consecutive times, you need to record how many times the test was passed and then arrange targeted tests.
* For extended testing, please use the [examples] (https://github.com/espressif/esp-idf/tree/master/examples/bluetooth) provided by Espressif. 

       
## Test for ADV Performance (*)

### Search Device

Refresh the scanning in LightBlue® Explorer to check if the device to be tested can be found quickly. Please repeat this action 10 times.

### Test Results

The test is passed, if you get the following results:

* The device starts advertizing and outputs the log `(0) ***** advertising start successfully *****`.
* LightBlue® Explorer scans and successfully discovers ` BLE_COMP_TEST` each time.

**Note:** 

* The device broadcasts on 3 channels, with an ADV interval of 40 ms.
* Check if the ADV packet can be received.
* Check if the Scan Response packet can be received.
* The device name is included in Scan Response packets only and cannot be found in ADV packets.

## Test for Pairing Performance

### Connect Device

* Open the LightBlue® Explorer scan list and tap on the device name ` BLE_COMP_TEST` to establish connection.
* ESP device prints a passkey: `The passkey notify number: 123456`.
* A prompt on the smartphone appears asking if you want to pair. Tap on *Pair*, and then enter the passkey "123456".

### Test Results

The test is passed, if you get the following results:

* If the connection is successful:
   * Smartphone shows DATA beginning with `ADVERTISEMENT DATA`
   * ESP device outputs the log: `ESP_GATTS_CONNECT_EVT`
* When the pairing is established, the device shows the following log in green: `(1) ***** pair status = success *****`

## Test for Service Discovery Performance

### Test Procedures

In LightBlue® Explorer, check the contents of `GATT SERVICES & CHARACTERISTICS`.

### Test Results

The test is passed, if you get the following results:

* Service that starts with ``000000ff`` appears at the bottom of your smartphone.
* This service contains 3 characteristics
   * `Char_1_Short_WR`
   * `Char_2_Long_WR`
   * `Char_3_Short_Notify`
   
## Test for Read and Encrypt

### Test Procedures

Read the value of `Char_1` in LightBlue, and tap on `READ AGAIN`.

### Test Results

* Encryption is successful, if your smartphone shows the value "11 22 33 44", and the ESP device prints the log: `(2) ***** read char_1 *****`.
* Encryption fails, if your smartphone shows a blank screen, and the ESP device outputs the error log in red: `GATT_INSUF_AUTHENTICATION: MITM Required`. 

## Test for Short Read and Write

### Test Procedures

* Navigate to the WRITE interface in LightBlue® Explorer, and write the value "88 99" to `Char_1`.
* Read `Char_1` and check if its value is consistent with the data you have written to it.

### Test Results

The test is passed, if you get the following results:

* ESP device prints the log: `(3)***** short write success *****`.
* LightBlue® Explorer shows "88 99" below `READ AGAIN`.

## Test for Long Read and Write

### Test Procedures

* Navigate to the WRITE interface in LightBlue® Explorer, and write the string `0x001122…FF001122…FF` of 256 bytes to `Char_2`. The data takes up 16 lines and looks as follows:

	```
	00112233445566778899AABBCCDDEEFF
	00112233445566778899AABBCCDDEEFF
	…
	00112233445566778899AABBCCDDEEFF
	00112233445566778899AABBCCDDEEFF
	```

* Read `Char_2` and check if its value is consistent with the data you have written to it.

### Test Results

The test is passed, if you get the following results:

* The device prints the log: ``ESP_GATTS_EXEC_WRITE_EVT, Length=256`` and ``(4) ***** long write success *****``.
* LightBlue® Explorer shows `(5) ***** read char_2 *****` below `READ AGAIN`.

**Note:**

The data to be written can be copied from a text file and pasted into LightBlue® Explorer.

## Test for Short Notify

### Test Procedures

* Enter `Char_3` and tap on `SUBSCRIBE` to enable its Notify function.
* Your phone automatically receives Notify data from the device.

### Test Results

The test is passed, if you get the following results:

* ESP device prints the log: `(6) ***** send notify AA BB *****`.
* "AA BB" appears on your smartphone.

## Test for Connection Success Rate (*)

### Test procedures

* Break the connection
* Re-establish the connection
* Repeat 10 times

### Test Results

The test is passed, if you get the following results:

* Your phone establishes the connection successfully, and the ESP device outputs the log: `(1) ***** pair status = success *****`.
* Your phone breaks the connection, and the device outputs the log: `ESP_GATTS_DISCONNECT_EVT`.
* Connection can be set up each time with no issues.

## Test for Long Connection Stability

The connection must be stable throughout the tests.

**Note:**

If the existing connection breaks:

* LightBlue® Explorer prints `Disconnected`.
* ESP device outputs the log: ``ESP_GATTS_DISCONNECT_EVT, reason = (0) ***** advertising start successfully *****``.
 
## Further Information

* If you see any log entry in red, please record it for future reference or feedback it to our engineer.
* Tests to be added in the future:
   * Multi-connection Test
   * Automatic Re-connection Test

