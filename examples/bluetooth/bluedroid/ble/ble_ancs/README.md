ESP-IDF BLE ANCS Example
==========================

The purpose of the Apple Notification Center Service (ANCS) is to give Bluetooth accessories (that connect to iOS devices through a Bluetooth low-energy link) a simple and convenient way to access many kinds of notifications that are generated on iOS devices.

The Apple Notification Center Service is a primary service whose service UUID is 7905F431-B5CE-4E99-A40F-4B1E122D00D0.

Only one instance of the ANCS may be present on an NP. Due to the nature of iOS, the ANCS is not guaranteed to always be present. As a result, the NC should look for and subscribe to the Service Changed characteristic of the GATT service in order to monitor for the potential publishing and unpublishing of the ANCS at any time.

In its basic form, the ANCS exposes three characteristics:
Notification Source: UUID 9FBF120D-6301-42D9-8C58-25E699A21DBD (notifiable)
Control Point: UUID 69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9 (writeable with response)
Data Source: UUID 22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB (notifiable)
All these characteristics require authorization for access.




