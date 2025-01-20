---

### **Usage Instructions**

The **`bt_hci_to_btsnoop.py`** script parses Bluetooth HCI logs and converts them into the BTSnoop format, allowing for detailed analysis. It reads an input log file, processes each line, and writes HCI packets to an output file.

---

### **How to Capture HCI Logs**

To use this tool, you need to enable Bluetooth HCI debug mode in your ESP-IDF project and capture HCI logs as follows:

#### 1. Enable Bluetooth HCI Debug Mode
- Open the ESP-IDF configuration menu:
  - **Top → Component config → Bluetooth**
  - Enable: `[x] Enable Bluetooth HCI debug mode`

#### 2. Capture Logs in the `app_main` Code
In your application code (e.g., `app_main`), call the following functions to log HCI data and advertisement logs:

```c
while (1)
{
    extern void bt_hci_log_hci_data_show(void);
    extern void bt_hci_log_hci_adv_show(void);
    bt_hci_log_hci_data_show();  // Display HCI data logs
    bt_hci_log_hci_adv_show();   // Display HCI advertisement logs
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
```

#### 3. Save Logs to a File
Run the application and redirect the serial output to a log file:

```bash
idf.py -p /dev/ttyUSB0 monitor >> all_log.txt
```

- **Note:** Replace `/dev/ttyUSB0` with your device's actual port (e.g., `/dev/ttyUSB1`, `COM3`, etc.).

#### 4. Manual Log Creation (Optional)
Alternatively, manually create a log file containing HCI data in the expected format.

---

### **Running the Script**

To parse the logs and generate a BTSnoop file, run:

```bash
python bt_hci_to_btsnoop.py -p <input_log_file> -o <output_tag>
```

#### **Parameters**
- `-p` or `--path`: Path to the input log file (e.g., `all_log.txt`). Required.
- `-o` or `--output`: A tag for the output file name. Required.

#### **Example Command**
```bash
python bt_hci_to_btsnoop.py -p /path/to/input_log.txt -o 123
```

This creates the file: `./parsed_logs/parsed_log_123.btsnoop.log`.

#### **Help Information**
For detailed usage options, run:
```bash
python bt_hci_to_btsnoop.py -h
```

---

### **Generated Output**

The parsed file will be saved as `parsed_log_<output_tag>.btsnoop.log` in the BTSnoop format.

### **Supported Tools for Viewing HCI Logs**

1. **Wireshark**
   - **Download:** [Wireshark](https://www.wireshark.org/)
   - Compatible with **Windows**, **Linux**, and **macOS** platforms.
   - Use it to open `.btsnoop.log` files for detailed analysis of HCI packets.

2. **Wireless Protocol Suite** (Teledyne LeCroy)
   - **Download:** [Wireless Protocol Suite](https://www.teledynelecroy.com/support/softwaredownload/psgdocuments.aspx?standardid=2&mseries=672)
   - Supported exclusively on the **Windows** platform.

3. **btmon** (Linux only)
   - **Description:** A command-line tool included with the BlueZ Bluetooth stack for Linux.
   - Use the following command to analyze HCI details directly:
     ```bash
     btmon -r <btsnoop log>
     ```
---

### **Sample Log Format**

Below is an example of expected log entries:

```plaintext
e6 C:35 0c 05 01 01 00 01 00
e7 H:01 00 2a 00 26 00 04 00 12 2a 00 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20 21 22
e8 E:13 05 01 01 00 01 00
e9 D:01 20 05 00 01 00 04 00 13
ea C:35 0c 05 01 01 00 01 00
```

---

### **Notes**
- Ensure valid input file paths and output directories.
- Verify read/write permissions for files.
- The script expects a specific log file format; unexpected formats may cause errors.

By following these steps, you can easily convert HCI logs into the BTSnoop format for analysis.
