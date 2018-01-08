# General questions

## Why do I not get a reply from the remote device when I perform get operation immediately after set operation has been performed?
* Any Client Model operation needs to wait for the completion event of an ongoing operation. Once the completion event is received the next command can be executed. If a command is executed before, a timeout error will occur.

## Why do I see a crash when I use API `esp_ble_mesh_client_model_send_msg` with log "Invalid client value when sent client msg." or Invalid client value when sent client msg."?
* You should initialize a structure of type `esp_ble_mesh_client_t` and set its value as the user data of client model
