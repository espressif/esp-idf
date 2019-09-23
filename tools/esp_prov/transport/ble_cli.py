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
from future.utils import iteritems

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
    except ImportError:
        pass


# --------------------------------------------------------------------


# BLE client (Linux Only) using Bluez and DBus
class BLE_Bluez_Client:
    def connect(self, devname, iface, chrc_names, fallback_srv_uuid):
        self.devname = devname
        self.srv_uuid_fallback = fallback_srv_uuid
        self.chrc_names = [name.lower() for name in chrc_names]
        self.device = None
        self.adapter = None
        self.adapter_props = None
        self.services = None
        self.nu_lookup = None
        self.characteristics = dict()
        self.srv_uuid_adv = None

        dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
        bus = dbus.SystemBus()
        manager = dbus.Interface(bus.get_object("org.bluez", "/"), "org.freedesktop.DBus.ObjectManager")
        objects = manager.GetManagedObjects()

        for path, interfaces in iteritems(objects):
            adapter = interfaces.get("org.bluez.Adapter1")
            if adapter is not None:
                if path.endswith(iface):
                    self.adapter = dbus.Interface(bus.get_object("org.bluez", path), "org.bluez.Adapter1")
                    self.adapter_props = dbus.Interface(bus.get_object("org.bluez", path), "org.freedesktop.DBus.Properties")
                    break

        if self.adapter is None:
            raise RuntimeError("Bluetooth adapter not found")

        self.adapter_props.Set("org.bluez.Adapter1", "Powered", dbus.Boolean(1))
        self.adapter.StartDiscovery()

        retry = 10
        while (retry > 0):
            try:
                if self.device is None:
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
        for path, interfaces in iteritems(objects):
            if "org.bluez.Device1" not in interfaces:
                continue
            if interfaces["org.bluez.Device1"].get("Name") == self.devname:
                dev_path = path
                break

        if dev_path is None:
            raise RuntimeError("BLE device not found")

        try:
            self.device = bus.get_object("org.bluez", dev_path)
            try:
                uuids = self.device.Get('org.bluez.Device1', 'UUIDs',
                                        dbus_interface='org.freedesktop.DBus.Properties')
                # There should be 1 service UUID in advertising data
                # If bluez had cached an old version of the advertisement data
                # the list of uuids may be incorrect, in which case connection
                # or service discovery may fail the first time. If that happens
                # the cache will be refreshed before next retry
                if len(uuids) == 1:
                    self.srv_uuid_adv = uuids[0]
            except dbus.exceptions.DBusException as e:
                print(e)

            self.device.Connect(dbus_interface='org.bluez.Device1')
        except Exception as e:
            print(e)
            self.device = None
            raise RuntimeError("BLE device could not connect")

    def _get_services_(self):
        bus = dbus.SystemBus()
        manager = dbus.Interface(bus.get_object("org.bluez", "/"), "org.freedesktop.DBus.ObjectManager")
        objects = manager.GetManagedObjects()
        service_found = False
        for srv_path, srv_interfaces in iteritems(objects):
            if "org.bluez.GattService1" not in srv_interfaces:
                continue
            if not srv_path.startswith(self.device.object_path):
                continue
            service = bus.get_object("org.bluez", srv_path)
            srv_uuid = service.Get('org.bluez.GattService1', 'UUID',
                                   dbus_interface='org.freedesktop.DBus.Properties')

            # If service UUID doesn't match the one found in advertisement data
            # then also check if it matches the fallback UUID
            if srv_uuid not in [self.srv_uuid_adv, self.srv_uuid_fallback]:
                continue

            nu_lookup = dict()
            characteristics = dict()
            for chrc_path, chrc_interfaces in iteritems(objects):
                if "org.bluez.GattCharacteristic1" not in chrc_interfaces:
                    continue
                if not chrc_path.startswith(service.object_path):
                    continue
                chrc = bus.get_object("org.bluez", chrc_path)
                uuid = chrc.Get('org.bluez.GattCharacteristic1', 'UUID',
                                dbus_interface='org.freedesktop.DBus.Properties')
                characteristics[uuid] = chrc
                for desc_path, desc_interfaces in iteritems(objects):
                    if "org.bluez.GattDescriptor1" not in desc_interfaces:
                        continue
                    if not desc_path.startswith(chrc.object_path):
                        continue
                    desc = bus.get_object("org.bluez", desc_path)
                    desc_uuid = desc.Get('org.bluez.GattDescriptor1', 'UUID',
                                         dbus_interface='org.freedesktop.DBus.Properties')
                    if desc_uuid[4:8] != '2901':
                        continue
                    try:
                        readval = desc.ReadValue({}, dbus_interface='org.bluez.GattDescriptor1')
                    except dbus.exceptions.DBusException:
                        break
                    found_name = ''.join(chr(b) for b in readval).lower()
                    nu_lookup[found_name] = uuid
                    break

            match_found = True
            for name in self.chrc_names:
                if name not in nu_lookup:
                    # Endpoint name not present
                    match_found = False
                    break

            # Create lookup table only if all endpoint names found
            self.nu_lookup = [None, nu_lookup][match_found]
            self.characteristics = characteristics
            service_found = True

            # If the service UUID matches that in the advertisement
            # we can stop the search now. If it doesn't match, we
            # have found the service corresponding to the fallback
            # UUID, in which case don't break and keep searching
            # for the advertised service
            if srv_uuid == self.srv_uuid_adv:
                break

        if not service_found:
            self.device.Disconnect(dbus_interface='org.bluez.Device1')
            if self.adapter:
                self.adapter.RemoveDevice(self.device)
            self.device = None
            self.nu_lookup = None
            self.characteristics = dict()
            raise RuntimeError("Provisioning service not found")

    def get_nu_lookup(self):
        return self.nu_lookup

    def has_characteristic(self, uuid):
        if uuid in self.characteristics:
            return True
        return False

    def disconnect(self):
        if self.device:
            self.device.Disconnect(dbus_interface='org.bluez.Device1')
            if self.adapter:
                self.adapter.RemoveDevice(self.device)
            self.device = None
            self.nu_lookup = None
            self.characteristics = dict()
        if self.adapter_props:
            self.adapter_props.Set("org.bluez.Adapter1", "Powered", dbus.Boolean(0))

    def send_data(self, characteristic_uuid, data):
        try:
            path = self.characteristics[characteristic_uuid]
        except KeyError:
            raise RuntimeError("Invalid characteristic : " + characteristic_uuid)

        try:
            path.WriteValue([ord(c) for c in data], {}, dbus_interface='org.bluez.GattCharacteristic1')
        except dbus.exceptions.DBusException as e:
            raise RuntimeError("Failed to write value to characteristic " + characteristic_uuid + ": " + str(e))

        try:
            readval = path.ReadValue({}, dbus_interface='org.bluez.GattCharacteristic1')
        except dbus.exceptions.DBusException as e:
            raise RuntimeError("Failed to read value from characteristic " + characteristic_uuid + ": " + str(e))
        return ''.join(chr(b) for b in readval)


# --------------------------------------------------------------------


# Console based BLE client for Cross Platform support
class BLE_Console_Client:
    def connect(self, devname, iface, chrc_names, fallback_srv_uuid):
        print("BLE client is running in console mode")
        print("\tThis could be due to your platform not being supported or dependencies not being met")
        print("\tPlease ensure all pre-requisites are met to run the full fledged client")
        print("BLECLI >> Please connect to BLE device `" + devname + "` manually using your tool of choice")
        resp = input("BLECLI >> Was the device connected successfully? [y/n] ")
        if resp != 'Y' and resp != 'y':
            return False
        print("BLECLI >> List available attributes of the connected device")
        resp = input("BLECLI >> Is the service UUID '" + fallback_srv_uuid + "' listed among available attributes? [y/n] ")
        if resp != 'Y' and resp != 'y':
            return False
        return True

    def get_nu_lookup(self):
        return None

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


# --------------------------------------------------------------------


# Function to get client instance depending upon platform
def get_client():
    if fallback:
        return BLE_Console_Client()
    return BLE_Bluez_Client()
