# Frequently Asked Questions

## General Questions

### Why I do not get a reply from the remote device when I perform get operation immediately after set operation has been performed?
* Any Client Model operation needs to wait for the completion event of an ongoing operation. Once the completion event is received the next command can be executed. If a command is executed before the completion event is received, a timeout error will occur.

### When I use the API  `esp_ble_mesh_client_model_send_msg`, why does it crash with the log messages *Invalid client value when sent client msg* or *Invalid client value when sent client msg*?
* You should initialize a structure of the type `esp_ble_mesh_client_t` and set its value as the user data of client model.
