# ESP-IDF SPP GATT CLIENT demo

## 1. Overview

  In Bluetooth classic (BR/EDR) systems, a Serial Port Profile (SPP) is an adopted profile defined by the Bluetooth Special Interest Group (SIG) used to emulate a serial port connection over a Bluetooth wireless connection. For BLE systems, an adopted SPP profile over BLE is not defined, thus emulation of a serial port must be implemented as a vendor-specific custom profile.

  This reference design consists of two Demos, the ble spp server and ble spp client that run on their respective endpoints. These devices connect and exchange data wirelessly with each other. This capability creates a virtual serial link over the air. Each byte input can be sent and received by both the server and client. The spp server is implemented as the [ble_spp_server](../ble_spp_server) demo while the spp client is implemented as the [ble_spp_client](../ble_spp_client) demo. Espressif designed the BLE SPP applications to use the UART transport layer but you could adapt this design to work with other serial protocols, such as SPI.

## 2. Packet Structure

  After the Uart received data, the data will be posted to Uart task. Then, in the UART_DATA event, the raw data may be retrieved. The max length is 120 bytes every time.
  If you run the ble spp demo with two ESP32 chips, the MTU size will be exchanged for 200 bytes after the ble connection is established, so every packet can be send directly.
  If you only run the ble_spp_server demo, and it was connected by a phone, the MTU size may be less than 123 bytes. In such a case the data will be split into fragments and send in turn. 
  In every packet, we add 4 bytes to indicate that this is a fragment packet. The first two bytes contain "##" if this is a fragment packet, the third byte is the total number of the packets, the fourth byte is the current number of this packet.
  The phone APP need to check the structure of the packet if it want to communicate with the ble_spp_server demo.

## 3. Software Description
  
  The application is implemented in [spp_client_demo.c](../ble_spp_client/main/spp_client_demo.c) and [spp_server_demo.c](../ble_spp_server/main/ble_spp_server_demo.c).

### 3.1 Initialization

  Both the server and client will first initialize the uart and ble. The server demo will set up the serial port service with standard GATT and GAP services in the attribute server. The client demo will scan the ble broadcast over the air to find the spp server.

### 3.2 Event Processing

  The spp server has two main event processing functions for BLE event:

  * static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t * param); 
  * static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t * param);

  The spp client has two main event processing function for BLE event:

  * esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t * param);
  * void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t * param);

  These are some queues and tasks used by SPP application:

  Queues:

  * spp_uart_queue       - Uart data messages received from the Uart 
  * cmd_cmd_queue        - commands received from the client
  * cmd_heartbeat_queue  - heartbeat received, if supported

  Tasks:

  * uart_task            - processs Uart
  * spp_cmd_task         - process command messages, the commands and processing were defined by customer
  * spp_heartbeat_task   - if heartbeat is supported, the task will send a heatbeat packet to the remote device
  
### 3.3 Sending Data Wirelessly

  The client will be sending WriteNoRsp packets to the server. The server side sends data through notifications. When the Uart receives data, the Uart task places it in the buffer. if the size of the data is larger than (MTU size - 3), the data will be split into packets and send in turn. 

### 3.4 Receiving Data Wirelessly

  The server will receive this data in the ESP_GATTS_WRITE_EVT event and send data to the Uart terminal by `uart_wrire_bytes` function. For example:

    case ESP_GATTS_WRITE_EVT:
            ...
        if(res == SPP_IDX_SPP_DATA_RECV_VAL){
            uart_write_bytes(UART_NUM_0, (char *)(p_data->write.value), p_data->write.len);
        }
            ...
    break;

### 3.5 GATT Server Attribute Table

  charactertistic|UUID|Permissions  
  :-:|:-:|:-:  
  SPP_DATA_RECV_CHAR|0xABF1|READ&WRITE_NR      
  SPP_DATA_NOTIFY_CHAR|0xABF2|READ&NOTIFY  
  SPP_COMMAND_CHAR|0xABF3|READ&WRITE_NR  
  SPP_STATUS_CHAR|0xABF4|READ & NOTIFY  
  SPP_HEARTBEAT_CHAR|0xABF5|READ&WRITE_NR&NOTIFY  

## 4. How to run these demos

  Compile and download each application to the ESP32. The spp cilent will auto connect to the spp server, do service search, exchange MTU size and register notification. 
  if you input data to the Uart terminal, it will be print in the remote device Uart terminal.
