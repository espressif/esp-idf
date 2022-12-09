#!/usr/bin/env python
#
# Copyright 2019 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Creating GATT Application which then becomes available to remote devices.

from __future__ import print_function
import sys

try:
    import dbus
    import dbus.service
except ImportError as e:
    if 'linux' not in sys.platform:
        raise e
    print(e)
    print("Install packages `libgirepository1.0-dev gir1.2-gtk-3.0 libcairo2-dev libdbus-1-dev libdbus-glib-1-dev` for resolving the issue")
    print("Run `pip install -r $IDF_PATH/tools/ble/requirements.txt` for resolving the issue")
    raise

alert_status_char_obj = None

GATT_APP_OBJ = False
CHAR_READ = False
CHAR_WRITE = False
CHAR_SUBSCRIBE = False

DBUS_OM_IFACE = 'org.freedesktop.DBus.ObjectManager'
DBUS_PROP_IFACE = 'org.freedesktop.DBus.Properties'
GATT_MANAGER_IFACE = 'org.bluez.GattManager1'
GATT_SERVICE_IFACE = 'org.bluez.GattService1'
GATT_CHRC_IFACE = 'org.bluez.GattCharacteristic1'
GATT_DESC_IFACE = 'org.bluez.GattDescriptor1'


class InvalidArgsException(dbus.exceptions.DBusException):
    _dbus_error_name = 'org.freedesktop.DBus.Error.InvalidArgs'


class NotSupportedException(dbus.exceptions.DBusException):
    _dbus_error_name = 'org.bluez.Error.NotSupported'


class Application(dbus.service.Object):
    """
    org.bluez.GattApplication1 interface implementation
    """

    def __init__(self, bus, path):
        self.path = path
        self.services = []
        srv_obj = AlertNotificationService(bus, '0001')
        self.add_service(srv_obj)
        dbus.service.Object.__init__(self, bus, self.path)

    def __del__(self):
        pass

    def get_path(self):
        return dbus.ObjectPath(self.path)

    def add_service(self, service):
        self.services.append(service)

    @dbus.service.method(DBUS_OM_IFACE, out_signature='a{oa{sa{sv}}}')
    def GetManagedObjects(self):
        global GATT_APP_OBJ
        response = {}

        for service in self.services:
            response[service.get_path()] = service.get_properties()
            chrcs = service.get_characteristics()
            for chrc in chrcs:
                response[chrc.get_path()] = chrc.get_properties()
                descs = chrc.get_descriptors()
                for desc in descs:
                    response[desc.get_path()] = desc.get_properties()

        GATT_APP_OBJ = True
        return response

    def Release(self):
        pass


class Service(dbus.service.Object):
    """
    org.bluez.GattService1 interface implementation
    """
    PATH_BASE = '/org/bluez/hci0/service'

    def __init__(self, bus, index, uuid, primary=False):
        self.path = self.PATH_BASE + str(index)
        self.bus = bus
        self.uuid = uuid
        self.primary = primary
        self.characteristics = []
        dbus.service.Object.__init__(self, bus, self.path)

    def get_properties(self):
        return {
            GATT_SERVICE_IFACE: {
                'UUID': self.uuid,
                'Primary': self.primary,
                'Characteristics': dbus.Array(
                    self.get_characteristic_paths(),
                    signature='o')
            }
        }

    def get_path(self):
        return dbus.ObjectPath(self.path)

    def add_characteristic(self, characteristic):
        self.characteristics.append(characteristic)

    def get_characteristic_paths(self):
        result = []
        for chrc in self.characteristics:
            result.append(chrc.get_path())
        return result

    def get_characteristics(self):
        return self.characteristics

    @dbus.service.method(DBUS_PROP_IFACE,
                         in_signature='s',
                         out_signature='a{sv}')
    def GetAll(self, interface):
        if interface != GATT_SERVICE_IFACE:
            raise InvalidArgsException()
        return self.get_properties()[GATT_SERVICE_IFACE]


class Characteristic(dbus.service.Object):
    """
    org.bluez.GattCharacteristic1 interface implementation
    """
    def __init__(self, bus, index, uuid, flags, service):
        self.path = service.path + '/char' + str(index)
        self.bus = bus
        self.uuid = uuid
        self.service = service
        self.flags = flags
        self.value = [0]
        self.descriptors = []
        dbus.service.Object.__init__(self, bus, self.path)

    def get_properties(self):
        return {
            GATT_CHRC_IFACE: {
                'Service': self.service.get_path(),
                'UUID': self.uuid,
                'Flags': self.flags,
                'Value': self.value,
                'Descriptors': dbus.Array(self.get_descriptor_paths(), signature='o')

            }
        }

    def get_path(self):
        return dbus.ObjectPath(self.path)

    def add_descriptor(self, descriptor):
        self.descriptors.append(descriptor)

    def get_descriptor_paths(self):
        result = []
        for desc in self.descriptors:
            result.append(desc.get_path())
        return result

    def get_descriptors(self):
        return self.descriptors

    @dbus.service.method(DBUS_PROP_IFACE, in_signature='s', out_signature='a{sv}')
    def GetAll(self, interface):
        if interface != GATT_CHRC_IFACE:
            raise InvalidArgsException()

        return self.get_properties()[GATT_CHRC_IFACE]

    @dbus.service.method(GATT_CHRC_IFACE, in_signature='a{sv}', out_signature='ay')
    def ReadValue(self, options):
        print('\nDefault ReadValue called, returning error')
        raise NotSupportedException()

    @dbus.service.method(GATT_CHRC_IFACE, in_signature='aya{sv}')
    def WriteValue(self, value, options):
        print('\nDefault WriteValue called, returning error')
        raise NotSupportedException()

    @dbus.service.method(GATT_CHRC_IFACE)
    def StartNotify(self):
        print('Default StartNotify called, returning error')
        raise NotSupportedException()

    @dbus.service.method(GATT_CHRC_IFACE)
    def StopNotify(self):
        print('Default StopNotify called, returning error')
        raise NotSupportedException()

    @dbus.service.signal(DBUS_PROP_IFACE,
                         signature='sa{sv}as')
    def PropertiesChanged(self, interface, changed, invalidated):
        print("\nProperties Changed")


class Descriptor(dbus.service.Object):
    """
    org.bluez.GattDescriptor1 interface implementation
    """
    def __init__(self, bus, index, uuid, flags, characteristic):
        self.path = characteristic.path + '/desc' + str(index)
        self.bus = bus
        self.uuid = uuid
        self.flags = flags
        self.chrc = characteristic
        dbus.service.Object.__init__(self, bus, self.path)

    def get_properties(self):
        return {
            GATT_DESC_IFACE: {
                'Characteristic': self.chrc.get_path(),
                'UUID': self.uuid,
                'Flags': self.flags,
            }
        }

    def get_path(self):
        return dbus.ObjectPath(self.path)

    @dbus.service.method(DBUS_PROP_IFACE,
                         in_signature='s',
                         out_signature='a{sv}')
    def GetAll(self, interface):
        if interface != GATT_DESC_IFACE:
            raise InvalidArgsException()

        return self.get_properties()[GATT_DESC_IFACE]

    @dbus.service.method(GATT_DESC_IFACE, in_signature='a{sv}', out_signature='ay')
    def ReadValue(self, options):
        print('Default ReadValue called, returning error')
        raise NotSupportedException()

    @dbus.service.method(GATT_DESC_IFACE, in_signature='aya{sv}')
    def WriteValue(self, value, options):
        print('Default WriteValue called, returning error')
        raise NotSupportedException()


class AlertNotificationService(Service):
    TEST_SVC_UUID = '00001811-0000-1000-8000-00805f9b34fb'

    def __init__(self, bus, index):
        global alert_status_char_obj
        Service.__init__(self, bus, index, self.TEST_SVC_UUID, primary=True)
        self.add_characteristic(SupportedNewAlertCategoryCharacteristic(bus, '0001', self))
        self.add_characteristic(AlertNotificationControlPointCharacteristic(bus, '0002', self))
        alert_status_char_obj = UnreadAlertStatusCharacteristic(bus, '0003', self)
        self.add_characteristic(alert_status_char_obj)


class SupportedNewAlertCategoryCharacteristic(Characteristic):
    SUPPORT_NEW_ALERT_UUID = '00002A47-0000-1000-8000-00805f9b34fb'

    def __init__(self, bus, index, service):
        Characteristic.__init__(
            self, bus, index,
            self.SUPPORT_NEW_ALERT_UUID,
            ['read'],
            service)

        self.value = [dbus.Byte(2)]

    def ReadValue(self, options):
        global CHAR_READ
        CHAR_READ = True
        val_list = []
        for val in self.value:
            val_list.append(dbus.Byte(val))
        print("Read Request received\n", "\tSupportedNewAlertCategoryCharacteristic")
        print("\tValue:", "\t", val_list)
        return val_list


class AlertNotificationControlPointCharacteristic(Characteristic):
    ALERT_NOTIF_UUID = '00002A44-0000-1000-8000-00805f9b34fb'

    def __init__(self, bus, index, service):
        Characteristic.__init__(
            self, bus, index,
            self.ALERT_NOTIF_UUID,
            ['read','write'],
            service)

        self.value = [dbus.Byte(0)]

    def ReadValue(self, options):
        val_list = []
        for val in self.value:
            val_list.append(dbus.Byte(val))
        print("Read Request received\n", "\tAlertNotificationControlPointCharacteristic")
        print("\tValue:", "\t", val_list)
        return val_list

    def WriteValue(self, value, options):
        global CHAR_WRITE
        CHAR_WRITE = True
        print("Write Request received\n", "\tAlertNotificationControlPointCharacteristic")
        print("\tCurrent value:", "\t", self.value)
        val_list = []
        for val in value:
            val_list.append(val)
        self.value = val_list
        # Check if new value is written
        print("\tNew value:", "\t", self.value)
        if not self.value == value:
            print("Failed: Write Request\n\tNew value not written\tCurrent value:", self.value)


class UnreadAlertStatusCharacteristic(Characteristic):
    UNREAD_ALERT_STATUS_UUID = '00002A45-0000-1000-8000-00805f9b34fb'

    def __init__(self, bus, index, service):
        Characteristic.__init__(
            self, bus, index,
            self.UNREAD_ALERT_STATUS_UUID,
            ['read', 'write', 'notify'],
            service)
        self.value = [dbus.Byte(0)]
        self.cccd_obj = ClientCharacteristicConfigurationDescriptor(bus, '0001', self)
        self.add_descriptor(self.cccd_obj)
        self.notifying = False

    def StartNotify(self):
        global CHAR_SUBSCRIBE
        CHAR_SUBSCRIBE = True

        if self.notifying:
            print('\nAlready notifying, nothing to do')
            return
        self.notifying = True
        print("\nNotify Started")
        self.cccd_obj.WriteValue([dbus.Byte(1), dbus.Byte(0)])
        self.cccd_obj.ReadValue()

    def StopNotify(self):
        if not self.notifying:
            print('\nNot notifying, nothing to do')
            return
        self.notifying = False
        print("\nNotify Stopped")

    def ReadValue(self, options):
        print("Read Request received\n", "\tUnreadAlertStatusCharacteristic")
        val_list = []
        for val in self.value:
            val_list.append(dbus.Byte(val))
        print("\tValue:", "\t", val_list)
        return val_list

    def WriteValue(self, value, options):
        print("Write Request received\n", "\tUnreadAlertStatusCharacteristic")
        val_list = []
        for val in value:
            val_list.append(val)
        self.value = val_list
        # Check if new value is written
        print("\tNew value:", "\t", self.value)
        if not self.value == value:
            print("Failed: Write Request\n\tNew value not written\tCurrent value:", self.value)


class ClientCharacteristicConfigurationDescriptor(Descriptor):
    CCCD_UUID = '00002902-0000-1000-8000-00805f9b34fb'

    def __init__(self, bus, index, characteristic):
        self.value = [dbus.Byte(0)]
        Descriptor.__init__(
            self, bus, index,
            self.CCCD_UUID,
            ['read', 'write'],
            characteristic)

    def ReadValue(self):
        print("\tValue on read:", "\t", self.value)
        return self.value

    def WriteValue(self, value):
        val_list = []
        for val in value:
            val_list.append(val)
        self.value = val_list
        # Check if new value is written
        print("New value on write:", "\t", self.value)
        if not self.value == value:
            print("Failed: Write Request\n\tNew value not written\tCurrent value:", self.value)
