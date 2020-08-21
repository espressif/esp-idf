# The Document of ESP32 BLE_MESH Commands

## Overall Command


* `bmreg`: Provisioner/node register callback
    * Example: `bmreg`
    * Result: `Bm:Reg,OK`

* `bminit`: Provisioner/node initialize
    * `-m`: `mesh modle`
    * Example: `bminit -m 0x0001`
    * Result: `Bm:Init,OK`

* `bmpbind`: Provisioner binds Appkey with local model
    * `-a`: `<appkey index>:appkey index`
    * `-e`: `<element address>：element address`
    * `-n`: `<network index>:network index`
    * `-m`: `<model id>:model id`
    * `-c`: `<model id>:company id`
    * Example: `bmpbind -a 0 -e 0x01 -m 0x1001 -n 0x00`
    * Result: `provisioning:AppKeyBind,OK`

* `bmpdev`: Provisioner add/delete unprovisioned device
    * `-z`: `action type <add/delete>`
    * `-d`: `device address`
    * `-u`: `device uuid`
    * `-a`: `address type`
    * `-f`: `address flag`
    * `-b`: `used bearer`
    * `-o`: `oob information`
    * Example: `bmpdev -z add -d bt_mac -b -1 -a 0`
    * Result: `provisioner:DevAdd/Del,OK`

* `bmoob`: Provisioner/node config OOB parameters
    * `-s`: `Static OOB value`
    * `-l`: `Static OOB value length`
    * `-x`: `Maximum size of Output OOB`
    * `-o`: `Supported Output OOB Actions`
    * `-y`: `Maximum size of Input OOB`
    * `-i`: `Supported Input OOB Actions`
    * `-p`: `start address assigned by provisioner`
    * Example: `bmoob -o -0 -x 0`
    * Result: `OOB:Load,OK`

* `bmpbearer`: Enable/disable provisioner different bearer
    * `-b`: `bearer supported`
    * `-e`: `enable or disable bearer`
    * Example: `bmpbearer -b 1 -e 1`
    * Result: `provisioner:EnBearer,OK`

* `bmnbearer`: Enable/disable node different bearer
    * `-b`: `bearer supported`
    * `-e`: `enable or disable bearer`
    * Example: `bmnbearer -b 1 -e 1`
    * Result: `Node:EnBearer,OK`

* `bmpkey`: Add/Delete NetKey and AppKey of Provisioner
    * `-z`: `<action type>:add app key or network key`
    * `-n`: `<net key index>:network key index`
    * `-k`: `<key>:appkey or network`
    * `-a`: `<app key index>:appkey index`
    * Example: `bmpkey -z netkey -n 1 -k <network_key>`
    * Result: `provisioner:NetKeyAdd,OK`

* `bmccm`: BLE Mesh configuration client model operations
    * `-z`: `<action>:action type`:add or del client model
    * `-x`: `<state>:set state`
    * `-o`: `<opcode>:message opcode`
    * `-u`: `<address>:unicast address`
    * `-n`: `<network>:net work index`
    * `-i`: `<index>:appkey index`
    * `-r`: `<relay>:relay statue`
    * `-t`: `<transmit>:relay transmit`
    * `-c`: `<cid>:company id`
    * `-v`: `<value>:value`
    * `-a`: `<address>:address`
    * `-m`: `<mod id>:model id`
    * Example: `bmccm -z reg`
    * Result: `ConfigClient:OK`

* `bmgocm`: BLE Mesh onoff client model operations
    * `-z`: `<action>:action type`: on or off client model
    * `-o`: `<opcode>:message opcode`
    * `-u`: `<address>:unicast address`
    * `-n`: `<netkey index>:network key index`
    * `-a`: `<index>:appkey index`
    * `-r`: `<role>:role`
    * `-t`: `<time>:time to complete state transition`
    * `-e`: `<optional>:whether optional parameters included`
    * `-s` : `<state>:present onoff state`
    * `-a`: `<address>:address`
    * `-i`: `<identifier>:transaction identifier`
    * `-d`: `<delay>:indicate message execution delay`
    * Example: `bmgocm -z reg`
    * Result: `GenONOFFClient:Reg,OK`

* `bmnreset`: Reset node to become an unprovisioned device
    * Example: `bmnreset`
    * Result: `Node:Reset`

* `bmpublish`: BLE Mesh model publication 
    * `-d`: `<data>:message data`
    * `-o`: `<opcode>:operation opcode`
    * `-m`: `<module>:module published to`
    * `-r`: `<role>:device role`
    * `-a`: `<address>:unicast address`
    * `-i`: `<app key>:app key index`
    * `-p`: `<period>:period`
    * Example: `bmpublish -d 1 -o 0x8204 -m 0x1000 -r 0 -a 1 -p 0 -i 0`
    * Result: `PublishSend,OK`

* `bmnnwk`: An unprovisioned device enters the mesh network and becomes a node without the provisioning procedure
    * `-k`: `<net key>:network key`
    * `-n`: `<net index>:network key index`
    * `-u`: `<unicast address>:unicast address`
    * `-d`: `<device key>:device key`
    * `-a`: `<appkey>:app key`
    * `-i`: `<app key>:app key index`
    * `-g`: `<group address>:group address`
    * Example: `bnnnwk -k 0x1000 -n 1 -u 0x0010 -a 0x%s -d0x%s -i 0 -g 0xC000`
    * Result: `Provisioning:Success,%d`

* `bmpaddn`: Provisioner add device's information into the mesh database while unprovisioned device enter mesh network automatically
    * `-o`: `<oob info>:oob information`
    * `-a`: `<unicast address>:unicast address`
    * `-e`: `<element num>:element num`
    * `-n`: `<net index>:net index`
    * `-d`: `<device key>:device key`
    * `-u`: `<device uuid>:device uuid`
    * Example: `bmpaddn -o -0x0 -a %s -e 1 -n 1 -d 0x%s -u 0x%s`
    * Result: `Provisioner:AddNodeInfo,OK`

* `bmcperf`: BLE Mesh client test performance
    * `-z`: `<action>:action type`:init/get/destroy/percent 
    * `-s`: `<test size>:test size`
    * `-n`: `<node number>:node number`
    * `-l`: `<test number>:ttl`
    * Example: `bmcperf -z init -n 1 -s %d -l 7`
    * Result: `VendorPerfTest:InitStatistics,OK`

* `bmperf`: BLE Mesh vendor server model performance test
    * `-z`: `<action>:action type`:init/get/destroy/percent
    * `-p`: `<package>:package number`
    * Example: `bmsperf -z init -p %d`
    * Result: `Node:InitStatistics,OK`

* `bmtpcvm`: BLE Mesh vendor client model performance test
    * `-z`: `<action>:action type`:init or start
    * `-p`: `<byte>:playload byte`
    * `-n`: `<number>:test number`
    * `-o`: `<opcode>:opcode`
    * `-u`: `<address>:unicast address`
    * `-t`: `<ttl>:ttl`
    * `-a`: `<appkey>:appkey index`
    * `-i`: `<network key>:network key index`
    * `-d`: `<role>:device role`
    * Example: `bmtpcvm -z start -p %d -n %d -o 0xC302C4 -u %s -t 7 -a 0 -i 1 -d 1`
    * Result: `VendorModel:SendPackage,Finish`

* `bmtxpower`: Provisioner/node set tx power or rx sensitivity"
    * `-z`: `<action>:action type`:set tx power or rx sensitivity
    * `-t`: `<power>:tx power or sense`
    * Example: `bmtxpower -z tx -t %d`
    * Result: `Node:SetPower,OK`

