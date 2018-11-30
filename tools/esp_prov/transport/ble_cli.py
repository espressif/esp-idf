# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

from __future__ import print_function
from builtins import input

import platform

import utils

fallback = True

# Check if platform is Linux and required packages are installed
# else fallback to console mode
if platform.system() == 'Linux':
    try:
        import dbus
        import dbus.mainloop.glib
        import time
        fallback = False
    except:
        pass

#--------------------------------------------------------------------

# BLE client (Linux Only) using Bluez and DBus
class BLE_Bluez_Client:
    def connect(self, devname, iface, srv_uuid):
        self.devname = devname
        self.srv_uuid = srv_uuid
        self.device = None
        self.adapter = None
        self.adapter_props = None
        self.services = None

        dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
        bus = dbus.SystemBus()
        manager = dbus.Interface(bus.get_object("org.bluez", "/"), "org.freedesktop.DBus.ObjectManager")
        objects = manager.GetManagedObjects()

        for path, interfaces in objects.items():
            adapter = interfaces.get("org.bluez.Adapter1")
            if adapter != None:
                if path.endswith(iface):
                    self.adapter = dbus.Interface(bus.get_object("org.bluez", path), "org.bluez.Adapter1")
                    self.adapter_props = dbus.Interface(bus.get_object("org.bluez", path), "org.freedesktop.DBus.Properties")
                    break

        if self.adapter == None:
            raise RuntimeError("Bluetooth adapter not found")

        self.adapter_props.Set("org.bluez.Adapter1", "Powered", dbus.Boolean(1))
        self.adapter.StartDiscovery()

        retry = 10
        while (retry > 0):
            try:
                if self.device == None:
                    print("Connecting...")
                    # Wait for device to be discovered
                    time.sleep(5)
                    self._connect_()
                    print("Connected")
                print("Getting Services...")
                # Wait for services to be discovered
                time.sleep(5)
                self._get_services_()
                return True
            except Exception as e:
                print(e)
                retry -= 1
                print("Retries left", retry)
                continue
        self.adapter.StopDiscovery()
        return False

    def _connect_(self):
        bus = dbus.SystemBus()
        manager = dbus.Interface(bus.get_object("org.bluez", "/"), "org.freedesktop.DBus.ObjectManager")
        objects = manager.GetManagedObjects()
        dev_path = None
        for path, interfaces in objects.items():
            if "org.bluez.Device1" not in interfaces.keys():
                continue
            if interfaces["org.bluez.Device1"].get("Name") == self.devname:
                dev_path = path
                break

        if dev_path == None:
            raise RuntimeError("BLE device not found")

        try:
            self.device = bus.get_object("org.bluez", dev_path)
            self.device.Connect(dbus_interface='org.bluez.Device1')
        except Exception as e:
            print(e)
            self.device = None
            raise RuntimeError("BLE device could not connect")

    def _get_services_(self):
        bus = dbus.SystemBus()
        manager = dbus.Interface(bus.get_object("org.bluez", "/"), "org.freedesktop.DBus.ObjectManager")
        objects = manager.GetManagedObjects()
        srv_path = None
        for path, interfaces in objects.items():
            if "org.bluez.GattService1" not in interfaces.keys():
                continue
            if path.startswith(self.device.object_path):
                service = bus.get_object("org.bluez", path)
                uuid = service.Get('org.bluez.GattService1', 'UUID',
                            dbus_interface='org.freedesktop.DBus.Properties')
                if uuid == self.srv_uuid:
                    srv_path = path
                    break

        if srv_path == None:
            raise RuntimeError("Provisioning service not found")

        self.characteristics = dict()
        for path, interfaces in objects.items():
            if "org.bluez.GattCharacteristic1" not in interfaces.keys():
                continue
            if path.startswith(srv_path):
                chrc = bus.get_object("org.bluez", path)
                uuid = chrc.Get('org.bluez.GattCharacteristic1', 'UUID',
                            dbus_interface='org.freedesktop.DBus.Properties')
                self.characteristics[uuid] = chrc

    def has_characteristic(self, uuid):
        if uuid in self.characteristics.keys():
            return True
        return False

    def disconnect(self):
        if self.device:
            self.device.Disconnect(dbus_interface='org.bluez.Device1')
            if self.adapter:
                self.adapter.RemoveDevice(self.device)
        if self.adapter_props:
            self.adapter_props.Set("org.bluez.Adapter1", "Powered", dbus.Boolean(0))

    def send_data(self, characteristic_uuid, data):
        try:
            path = self.characteristics[characteristic_uuid]
        except KeyError:
            raise RuntimeError("Invalid characteristic : " + characteristic_uuid)
        path.WriteValue([ord(c) for c in data], {}, dbus_interface='org.bluez.GattCharacteristic1')
        return ''.join(chr(b) for b in path.ReadValue({}, dbus_interface='org.bluez.GattCharacteristic1'))

#--------------------------------------------------------------------

# Console based BLE client for Cross Platform support
class BLE_Console_Client:
    def connect(self, devname, iface, srv_uuid):
        print("BLE client is running in console mode")
        print("\tThis could be due to your platform not being supported or dependencies not being met")
        print("\tPlease ensure all pre-requisites are met to run the full fledged client")
        print("BLECLI >> Please connect to BLE device `" + devname + "` manually using your tool of choice")
        resp = input("BLECLI >> Was the device connected successfully? [y/n] ")
        if resp != 'Y' and resp != 'y':
            return False
        print("BLECLI >> List available attributes of the connected device")
        resp = input("BLECLI >> Is the service UUID '" + srv_uuid + "' listed among available attributes? [y/n] ")
        if resp != 'Y' and resp != 'y':
            return False
        return True

    def has_characteristic(self, uuid):
        resp = input("BLECLI >> Is the characteristic UUID '" + uuid + "' listed among available attributes? [y/n] ")
        if resp != 'Y' and resp != 'y':
            return False
        return True

    def disconnect(self):
        pass

    def send_data(self, characteristic_uuid, data):
        print("BLECLI >> Write following data to characteristic with UUID '" + characteristic_uuid + "' :")
        print("\t>> " + utils.str_to_hexstr(data))
        print("BLECLI >> Enter data read from characteristic (in hex) :")
        resp = input("\t<< ")
        return utils.hexstr_to_str(resp)

#--------------------------------------------------------------------

# Function to get client instance depending upon platform
def get_client():
    if fallback:
        return BLE_Console_Client()
    return BLE_Bluez_Client()
