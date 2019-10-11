# ble mesh provisioner demo
## Introduction  
This demo implements ble mesh provisioner basic features.Based on this demo, provisioner can scan and prove unprovisioned device, send set/get message. Also can define new model.  

Demo steps:  
1. Build the ble mesh provisioner demo with sdkconfig.default  
2. register provisioner and set oob info, load model to init ble mesh provisioner  
3. enable bearer, so that it can scan and prove unprovisioned devices  
4. config appkey and other config info use config client model
5.  send set/get message to nodes