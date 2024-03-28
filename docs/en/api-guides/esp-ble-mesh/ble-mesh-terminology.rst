ESP-BLE-MESH Terminology
========================

:link_to_translation:`zh_CN:[中文]`

.. _ble-mesh-terminology-role:

.. list-table:: Table 1 ESP-BLE-MESH Terminology - Role
  :widths: 10 40 60
  :header-rows: 1

  * - Term
    - Official Definition
    - Detailed Explanation
  * - Unprovisioned Device
    - A device that is not a member of a mesh network is known as an unprovisioned device.
    - Examples: lighting devices, temperature control devices, manufacturing equipment and electric doors, etc.
  * - Node
    - A node is a provisioned device.
    - The role of unprovisioned device will change to node after being provisioned to ESP-BLE-MESH network. Nodes (such as lighting devices, temperature control devices, manufacturing equipment, and electric doors) are devices that can send, receive, or relay messages in ESP-BLE-MESH network, and they can optionally support one or more subnets.
  * - Relay Node
    - A node that supports the Relay feature and has the Relay feature enabled is known as a Relay node.
    - Relay nodes can receive and resend ESP-BLE-MESH messages, so the messages can be transferred further. Users can decide whether or not to enable forwarding function of nodes according to nodes' status. Messages can be relayed for multiple times, and each relay is considered as a "hop". Messages can hop up to 126 times, which is enough for message transmission in a wide area.
  * - Proxy Node
    - A node that supports the Proxy feature and has the Proxy feature enabled is known as a Proxy node.
    - Proxy nodes receive messages from one bearer (it generally includes advertising bearer and GATT bearer) and resend it from another one. The purpose is to connect communication equipment that only support GATT bearer to ESP-BLE-MESH network. Generally, mobile apps need a Proxy node to access Mesh network. Without Proxy nodes, mobile apps cannot communicate with members in Mesh network.
  * - Friend Node
    - A node that supports the Friend feature, has the Friend feature enabled, and has a friendship with a node that supports the Low Power feature is known as a Friend node.
    - Friend node, like the backup of Low Power node (LPN), can store messages that are sent to Low Power node and security updates; the stored information will be transferred to Low Power node when Low Power node needs it. Low Power node must establish "friendship" with another node that supports the Friend Feature to reduce duty cycle of its receiver, thus power consumption of Low Power node can be reduced. Low Power node needs to find a Friend node to establish a friendship with it. The process involved is called "friendship establishment". Cooperation between Low Power node and Friend nodes enables Low Power node to schedule the use of the radio, thus Low Power node can receive messages at an appropriate or lower frequency without the need of keeping listening. Low Power node will poll Friend node to see if there is new message.
  * - Low Power Node
    - A node that supports the Low Power feature and has a friendship with a node that supports the Friend feature is known as a Low Power node.
    - By polling, Low Power node gets information from Friend node, such as messages, security updates, and etc.
  * -  Provisioner
    - A node that is capable of adding a device to a mesh network.
    - The device that can provision unprovisioned devices is called a Provisioner. This process usually needs to be implemented through an app that is typically provided by the product manufacturer and can be used on a gateway, a smartphone, tablet or other carriers.


.. _ble-mesh-terminology-composition:

.. list-table:: Table 2 ESP-BLE-MESH Terminology - Composition
  :widths: 10 40 60
  :header-rows: 1

  * - Term
    - Official Definition
    - Detailed Explanation
  * - State
    - A value representing a condition of an element that is exposed by an element of a node.
    - Each node in a ESP-BLE-MESH network has an independent set of state values that indicate certain states of the device, like brightness, and color of lighting device. Change of state value will lead to change of the physical state of devices. For example, changing the on/off state of a device is actually turning on/off the device.
  * - Model
    - A model defines the basic functionality of a node.
    - A node may contain multiple models, and each model defines basic functionalities of nodes, like the states needed by the nodes, the messages controlling the states, and actions resulted from messages handling. The function implementation of the nodes is based on models, which can be divided into SIG Model and Vendor Model, with the former defined by SIG and latter defined by users.
  * - Element
    - An addressable entity within a device.
    - A node can contain one or more elements, with each having a unicast address and one or more models, and the models contained by the same element must not be the same.
  * - Composition Data State
    - The Composition Data state contains information about a node, the elements it includes, and the supported models.
    - By reading the value of the Composition Data state, users can know basic information of the node, such as the number of elements, and the models in each element. Provisioner gets this message to further provision the device, such as configuring subscription address and publishing address of nodes.


.. _ble-mesh-terminology-features:

.. list-table:: Table 3 ESP-BLE-MESH Terminology - Features
  :widths: 10 40 60
  :header-rows: 1

  * - Term
    - Official Definition
    - Detailed Explanation
  * - Low Power Feature
    - The ability to operate within a mesh network at significantly reduced receiver duty cycles only in conjunction with a node supporting the Friend feature.
    - Low Power feature reduces power consumption of nodes. When a Low Power node is searching for a Friend node, and there are multiple Friend nodes nearby, it selects the most suitable Friend node through algorithm.
  * - Friend Feature
    - The ability to help a node supporting the Low Power feature to operate by storing messages destined for those nodes.
    - By enabling friend feature, the node can help to store information for Low Power node. The nodes enabled with friend feature may cause more power and memory consumption.
  * - Relay Feature
    - The ability to receive and retransmit mesh messages over the advertising bearer to enable larger networks.
    - The relay feature enables ESP-BLE-MESH messages to hop among nodes for multiple times, and the transmission distance can exceed the range of direct radio transmission between two nodes, thereby covering the entire network. When a node is enabled with the relay feature to relay messages, it only relays the messages of its own subnet, and does not relay the messages of other subnets. The data integrity will not be considered when the node enabled with relay feature relays segmented messages. The node would relay every segmented message once it receives one rather than waiting for the complete message.
  * - Proxy Feature
    - The ability to receive and retransmit mesh messages between GATT and advertising bearers.
    - The purpose of the proxy feature is to allow nodes without an advertising bearer to access the ESP-BLE-MESH network. The proxy feature is typically used in nodes that need to connect to mobile apps.


.. _ble-mesh-terminology-provisioning:

.. list-table:: Table 4 ESP-BLE-MESH Terminology - Provisioning
  :widths: 10 40 60
  :header-rows: 1

  * - Term
    - Official Definition
    - Detailed Explanation
  * - PB-ADV
    - PB-ADV is a provisioning bearer used to provision a device using Generic Provisioning PDUs over the advertising channels.
    - PB-ADV transfers packets generated during the provisioning process over the advertising channels. This way can only be used for provisioning when provisioner and unprovisioned device both support PB-ADV.
  * - PB-GATT
    - PB-GATT is a provisioning bearer used to provision a device using Proxy PDUs to encapsulate Provisioning PDUs within the Mesh Provisioning Service.
    - PB-GATT uses connection channels to transfer packets generated during the provisioning process. If an unprovisioned device wants to be provisioned through this method, it needs to implement the related Mesh Provisioning Service. Unprovisioned devices which don't implement such service cannot be provisioned into mesh network through PB-GATT bearer.
  * - PB-Remote
    - The PB-Remote provisioning bearer uses the existing mesh network to provision an unprovisioned device that is not within immediate radio range of the Provisioner.
    - PB-Remote uses the PB-ADV bearer or the PB-GATT bearer for the last hop to the unprovisioned device.
  * - Provisioning
    - Provisioning is a process of adding an unprovisioned device to a mesh network, managed by a Provisioner.
    - The process of provisioning turns the "unprovisioned device" into a "node", making it a member of the ESP-BLE-MESH network.
  * - Authentication Method
    - Authentication is a step during the provisioning of nodes.
    - There are four authentication methods for unprovisioned devices: Output OOB, Input OOB, Static OOB, and No OOB.
  * - Input OOB
    - Input Out-of-Band
    - For example, a Provisioner generates and displays a random number, and then prompts users to take appropriate actions to input the random number into the unprovisioned device. Taking lighting switch as an example, users can press the button for several times in a certain period of time to input the random number displayed on the Provisioner. Authentication method of the Input OOB is similar to that of Output OOB, but the role of the device is reversed.
  * - Output OOB
    - Output Out-of-Band
    - For example, an unprovisioned device will choose a random number and output the number in a way that is compatible with its functionality. If the unprovisioned device is a bulb, it can flash a specified number of times. If the unprovisioned device has an LCD screen, the random number can display as a multi-digit value. Users who start provisioning should input the observed number to authenticate the unprovisioned device.
  * - Static OOB
    - Static Out-of-Band
    - Authentication method of Static OOB: use Static OOB information. Use 0 as Static OOB information if No OOB information is needed. Use Static OOB information to authenticate devices which are going through provisioning if OOB information is needed.
  * - No OOB
    - No Out-of-Band
    - Authentication method of No OOB: Set the value of the Static OOB field to 0. Using this way is like not authenticating the unprovisioned devices.
  * - Certificate-based Provisioning
    - Certificate-based Out-of-Band
    - The certificate-based provisioning feature makes use of Public Key Infrastructure to authenticate unprovisioned device's public key and UUID information.


.. _ble-mesh-terminology-address:

.. list-table:: Table 5 ESP-BLE-MESH Terminology - Address
  :widths: 10 40 60
  :header-rows: 1

  * - Term
    - Official Definition
    - Detailed Explanation
  * - Unassigned Address
    - This is a special address type, with a value of 0x0000. Its use indicates that an Element has not yet been configured or had a Unicast Address assigned to it.
    - The addresses owned by elements which has not been configured yet or no address has been allocated are unassigned addresses. These elements will not be used for messages transfer because they have no fixed address. Unassigned address is recommended to set as the value of the address before setting the address of user code.
  * - Unicast Address
    - A unicast address is a unique address allocated to each element.
    - During provisioning, the Provisioner will assign a unicast address to each element of node within the life cycle of the nodes in the network. A unicast address may appear in the source/destination address field of a message. Messages sent to a unicast address can only be processed by the element that owns the unicast address.
  * - Virtual Address
    - A virtual address represents a set of destination addresses. Each virtual address logically represents a Label UUID, which is a 128-bit value that does not have to be managed centrally.
    - Associated with specific UUID labels, a virtual address may serve as the publishing or subscription address of the model. A UUID label is a 128-bit value associated with elements of one or more nodes. For virtual addresses, the 15th and 14th bits are set to 1 and 0 respectively; bits from 13th to 0 are set to hash values (providing 16384 hash values). The hash is a derivation of the Label UUID. To use subscribing elements to check the full 128-bit UUID is very inefficient while hash values provide a more efficient way to determine which elements that which messages are finally sent to.
  * - Group Address
    - A group address is an address that is programmed into zero or more elements
    - Group address is another kind of multicast address in the ESP-BLE-MESH network, which is usually used to group nodes. A message sent to the all-proxies address shall be processed by the primary element of all nodes that have the proxy functionality enabled. A message sent to the all-friends address shall be processed by the primary element of all nodes that have the friend functionality enabled. A message sent to the all-relays address shall be processed by the primary element of all nodes that have the relay functionality enabled. A message sent to the all-nodes address shall be processed by the primary element of all nodes.


.. _ble-mesh-terminology-security:

.. list-table:: Table 6 ESP-BLE-MESH Terminology - Security
  :widths: 10 40 60
  :header-rows: 1

  * - Term
    - Official Definition
    - Detailed Explanation
  * - Device Key (DevKey)
    - There is also a device key, which is a special application key that is unique to each node, is known only to the node and a Configuration Client, and is used to secure communications between the node and a Configuration Client.
    - The device key enables you to provision the devices, configure the nodes. The device key is used to encrypt Configuration Messages, i.e., the message transferred between the Provisioner and the node when the device is configured.
  * - Application Key (AppKey)
    - Application keys are used to secure communications at the upper transport layer.
    - Application key is used for decryption of application data before delivering application data to application layer and encryption of them during the delivery of application layer. Some nodes in the network have a specific purpose and can restrict access to potentially sensitive data based on the needs of the application. With specific application keys, these nodes are associated with specific applications. Generally speaking, the fields using different application keys include security (access control of buildings, machine rooms and CEO offices), lighting (plant, exterior building and sidewalks) and HVAC systems. Application keys are bound to Network keys. This means application keys are only used in a context of a Network key they are bound to. An application key shall only be bound to a single Network key.
  * - Flooding Security Material
    - The flooding security material is derived from the network key (NetKey) and can be used by other nodes in the same network. Messages encrypted with flooding security material can be decoded by any node in the same network.
    - The corresponding friendship messages encrypted with the friendship security material: 1. Friend Poll, 2. Friend Update, 3. Friend Subscription List, add/delete/confirm, 4. The Stored Messages" sent by friend nodes to Low Power node. The corresponding friendship messages encrypted with the flooding security material: 1. Friend Clear, 2. Friend Clear Confirm. Based on the setup of the applications, the messages sent from the Low Power node to the friend nodes will be encrypted with the friendship security material or flooding security material, with the former being used by the messages transmitted between Low Power node and friend nodes and the latter being used by other network messages.
  * - Directed Security Material
    - The directed security material is derived from the network key (NetKey) and can be used by other nodes in the directed forwarding path.
    - The messages that need to be forwarded through the directed forwarding path need to be encrypted with the directed security material. Messages encrypted with directed security material can be decoded by any node in the same directed forwarding path.

.. _ble-mesh-terminology-message:

.. list-table:: Table 7 ESP-BLE-MESH Terminology - Message
  :widths: 10 40 60
  :header-rows: 1

  * - Term
    - Official Definition
    - Detailed Explanation
  * - Reassembly/Segmentation
    - Segmentation and reassembly (SAR) is a method of communication network, which is divided into small units before transmitting packets and reassembled in a proper order at the communication receiving end.
    - The lower transport layer will automatically segment the message whose size is too big. The receiving end will return a response message, and the transmitting end will send the data packet again that the receiving end does not receive according to the response message. This is automatically completed by the lower transport layer. Unsegmented messages have at most 15 bytes, of which 4 bytes are transMIC, so the remaining is 11 bytes; in the case of segmentation, there are 12 valid bytes in the first several packets, and 8 in the last one. Special case: A shorter packet requires mandatory segmentation from lower transport layer, in which case the valid byte is 8 bytes.
  * - Unacknowledged/Acknowledged
    - There are two types of messages: Unacknowledged or Acknowledged
    - Based on the whether or not the receiving end needs to send the response message, the messages sent are divided into two kinds. The sending end should set the maximum number of retransmission.


.. _ble-mesh-terminology-foundation-models:

.. list-table:: Table 8 ESP-BLE-MESH Terminology - Foundation Models
  :widths: 10 40 60
  :header-rows: 1

  * - Term
    - Official Definition
    - Detailed Explanation
  * - Configuration Server Model
    - This model is used to represent a mesh network configuration of a device.
    - The node must contain the Configuration Server Model, which is responsible for maintaining configuration-related states. The states that Configuration Server Model maintains include: NetKey List, AppKey List, Model to AppKey List, Node Identity, Key Refresh Phase, Heartbeat Publish, Heartbeat Subscription, Network Transmit, Relay Retransmit etc.
  * - Configuration Client Model
    - The model is used to represent an element that can control and monitor the configuration of a node.
    - The Configuration Client Model uses messages to control the state maintained by the Configuration Server Model. The Provisioner must contain the Configuration Client Model, with which the configuration messages, like Configuration Composition Data Get can be sent.
  * - Health Server Model
    - This model is used to represent a mesh network diagnostics of a device.
    - The Health Server Model is primarily used by devices to check their states and see if there is an error. The states maintained by Health Server model include: Current Fault, Registered Fault, Health Period, and Attention Timer.
  * - Health Client Model
    - The model is used to represent an element that can control and monitor the health of a node.
    - The Health Client Model uses messages to control the state maintained by the Health Server Model. The model can get the self-test information of other nodes through the message "Health Fault Get".
  * - Remote Provisioning Server model
    - The model is used to support the functionality of provisioning a remote device over the mesh network and to perform the Node Provisioning Protocol Interface procedures.
    - The Remote Provisioning Server model is a root model and a main model that does not extend any other models.
  * - Remote Provisioning Client model
    - The model is used to support the functionality of provisioning devices into a mesh network by interacting with a mesh node that supports the Remote Provisioning Server model.
    - The Remote Provisioning Client is a root model and a main model that does not extend any other models. The Remote Provisioning Client may operate on states defined by the Remote Provisioning Server model using Remote Provisioning messages.
  * - Directed Forwarding Configuration Server model
    - The model is used to support the configuration of the directed forwarding functionality of a node.
    - The Directed Forwarding Configuration Server model is a main model that extends the Configuration Server model.
  * - Directed Forwarding Configuration Client model
    - The model is used to support the functionality of a node that can configure the directed forwarding functionality of another node.
    - The Directed Forwarding Configuration Client model is a root model and a main model that does not extend any other models. The Directed Forwarding Configuration Client model may operate on states defined by the Directed Forwarding Configuration Server model using Directed Forwarding Configuration messages.
  * - Bridge Configuration Server model
    - The model is used to support the configuration of the subnet bridge functionality of a node.
    - The Bridge Configuration Server model is a main model that extends the Configuration Server model.
  * - Bridge Configuration Client model
    - The model is used to support the functionality of a node that can configure the subnet bridge functionality of another node.
    - The Bridge Configuration Client model is a root model and a main model that does not extend any other models. The Bridge Configuration Client model may operate on states defined by the Bridge Configuration Server model using Bridge messages.
  * - Mesh Private Beacon Server model
    - The model is used to support the configuration of the Mesh Private beacons functionality of a node.
    - The Mesh Private Beacon Server model is a main model that extends the Configuration Server model.
  * - Mesh Private Beacon Client model
    - The model is used to support the functionality of a node that can configure the Mesh Private beacons functionality of another node.
    - The Mesh Private Beacon Client model is a root model and a main model that does not extend any other models. The Mesh Private Beacon Client model may operate on states defined by the Mesh Private Beacon Server model using Mesh Private Beacon messages.
  * - On-Demand Private Proxy Server model
    - The model is used to support the configuration of the advertising with Private Network Identity type functionality of a node.
    - The On-Demand Private Proxy Server model is a main model that extends the Mesh Private Beacon Server model and corresponds with the Solicitation PDU RPL Configuration Server model.
  * - On-Demand Private Proxy Client model
    - The model is used to support the functionality of a node that can configure the advertising with Private Network Identity type functionality of another node.
    - The On-Demand Private Proxy Client model is a root model and a main model that does not extend any other models. The On-Demand Private Proxy Client model may operate on states defined by the On-Demand Private Proxy Server model using On-Demand Private Proxy messages.
  * - SAR Configuration Server model
    - The model is used to support the configuration of the segmentation and reassembly behavior of a node.
    - The SAR Configuration Server model is a root model and a main model that does not extend any other models.
  * - SAR Configuration Client model
    - The SAR Configuration Client model is used to support the functionality of configuring the behavior of the lower transport layer of a node that supports the SAR Configuration Server model.
    - The SAR Configuration Client model is a root model and a main model that does not extend any other models. The SAR Configuration Client model may operate on states defined by the SAR Configuration Server model using SAR Configuration messages.
  * - Solicitation PDU RPL Configuration Server model
    - The Solicitation PDU RPL Configuration Server model is used to support the functionality of removing items from the solicitation replay protection list of a node.
    - The Solicitation PDU RPL Configuration Server model corresponds with the On-Demand Private Proxy Server model.
  * - Solicitation PDU RPL Configuration Client model
    - The model is used to support the functionality of removing addresses from the solicitation replay protection list of a node that supports the Solicitation PDU RPL Configuration Server model.
    - The Solicitation PDU RPL Configuration Client model is a root model and a main model that does not extend any other models. The Solicitation PDU RPL Configuration Client model may be used to remove items from a solicitation replay protection list of a peer node by using Solicitation PDU RPL Configuration messages.
  * - Opcodes Aggregator Server model
    - The model is used to support the functionality of processing a sequence of access layer messages.
    - The Opcodes Aggregator Server model is a root model that does not extend any other models.
  * - Opcodes Aggregator Client model
    - The model is used to support the functionality of dispatching a sequence of access layer messages to nodes supporting the Opcodes Aggregator Server model.
    - The Opcodes Aggregator Client model is a root model and a main model that does not extend any other models.
  * - Large Composition Data Server model
    - The model is used to support the functionality of exposing pages of Composition Data that do not fit in a Config Composition Data Status message and to expose metadata of the model instances.
    - The Large Composition Data Server is a main model that extends the Configuration Server model.
  * - Large Composition Data Client model
    - The model is used to support the functionality of reading pages of Composition Data that do not fit in a Config Composition Data Status message and reading the metadata of the model instances.
    - The Large Composition Data Client model is a root model that does not extend any other models. The Large Composition Data Client model may operate on states defined by the Large Composition Data Server model using Large Composition Data messages.


.. _ble-mesh-terminology-network-management:

.. list-table:: Table 9 ESP-BLE-MESH Terminology - Network Management
  :widths: 10 40 60
  :header-rows: 1

  * - Term
    - Official Definition
    - Detailed Explanation
  * - Key Refresh procedure
    - This procedure is used when the security of one or more network keys and/or one or more of the application keys has been compromised or could be compromised.
    - Key Refresh Procedure is used to update network key and application key of ESP-BLE-MESH network. Key Refresh Procedure is used when the security of one or more network keys and/or one or more application keys is threatened or potentially threatened. Keys are usually updated after some nodes in the network are removed.
  * - IV (Initialization Vector) Update Procedure
    - A node can also use an IV Update procedure to signal to peer nodes that it is updating the IV Index.
    - The IV Update procedure is used to update the value of ESP-BLE-MESH network's IV Index. This value is related to the random number required for message encryption. To ensure that the value of the random number is not repeated, this value is periodically incremented. IV Index is a 32-bit value and a shared network resource. For example, all nodes in a mesh network share the same IV Index value. Starting from 0x00000000, the IV Index increments during the IV Update procedure and maintained by a specific process, ensuring the IV Index shared in the mesh network is the same. This can be done when the node believes that it has the risk of exhausting its sequence number, or when it determines that another node is nearly exhausting its sequence number. Note: The update time must not be less than 96 hours. It can be triggered when a secure network beacon is received, or when the node determines that its sequence number is greater than a certain value.

For more terms, please see: `ESP-BLE-MESH Glossary of Terms <https://www.bluetooth.com/learn-about-bluetooth/recent-enhancements/mesh/mesh-glossary/>`_.
