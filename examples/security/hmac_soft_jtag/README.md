| Supported Targets | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

# JTAG Re-enable Example

This example showcases the use of HMAC peripheral for enabling the soft-disabled JTAG interface.

## How to use example

### Hardware Required

This example can be executed on any development board with a supported Espressif SOC chip - possessing a built-in HMAC peripheral (see `Supported Targets` table above).

### Configure the device

To configure the device for JTAG access, follow these steps:

**Note:** Before running the jtag_example_helper.py script, make sure to install the required dependencies by running the following command:

```bash
pip install -r requirements.txt
```

**Step 1:** Check JTAG status.

```bash
python jtag_example_helper.py check_jtag_status
```

If device is soft disabled, this example can re-enable it using further steps.

**Step 2:** Generate a 32 bytes HMAC key.

```bash
python jtag_example_helper.py generate_hmac_key <KEY_FILE>.bin
```

This generates a new 32-byte random HMAC key and store it in given file.

**Step 3:** Run the following command to burn the eFuse with the generated HMAC key with appropriate purpose. You can use purpose either HMAC_DOWN_ALL or HMAC_DOWN_JTAG. Check efuse summary to identify an available empty key block.

```bash
espefuse.py -p $ESPPORT burn_key <KEY_BLOCK_NO> <KEY_FILE>.bin HMAC_DOWN_ALL
```

**Step 4:** Generate token data from the HMAC key. Keep this token data handy before re-enabling JTAG access.

```bash
python jtag_example_helper.py generate_token <KEY_FILE>.bin
```

### Configure the project

Before the project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

Key block to be used for re-enabling JTAG can be configured in the project configuration menu, under ``Example Configuration`` > ``key block to be used``. The default value is -1, indicating that the example will use the first found keys with the purpose either HMAC_DOWN_ALL or HMAC_DOWN_JTAG.

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

```bash
idf.py -p PORT flash monitor
```

It will open console to enter command. Refer [Re-enable & Disable JTAG](#Re-enable-&-Disable-JTAG) to know more about usage of example.

(To exit the serial monitor, type `Ctrl-]`.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Re-enable & Disable JTAG

#### Re-enable JTAG

This re-enables JTAG access until the next reboot or until disabled using this example. For disabling JTAG, refer [Disable JTAG](#Disable-JTAG)

**Note:** Even upon successful return, JTAG will only be enabled with a valid token_data. 

```bash
enable_jtag <token_data>
```

Console logs while re-enabling JTAG:

```bash
I (314) main_task: Calling app_main()
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
I (394) main_task: Returned from app_main()
esp32c6> enable_jtag b2a49b1cce1be922bb7e431277413e3e8e6c3e8e6e17625c50ac66a9a857949b
I (10974) jtag_re_enable: Device is ready to re-enable.
I (10974) jtag_re_enable: Using HMAC key at block 8 with purpose HMAC_DOWN_JTAG
I (10984) jtag: JTAG re-enablement workflow performed, please check the JTAG connection manually
esp32c6> 
```

#### Disable JTAG

This disables the temporarily enabled JTAG access.

```bash
disable_jtag
```

Console logs while disabling JTAG:

```bash
esp32c6> disable_jtag
I (25104) jtag_re_enable: JTAG disabled temporarily
esp32c6> 
```
