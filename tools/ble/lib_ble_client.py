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

# DBus-Bluez BLE library

from __future__ import print_function

import sys
import time

try:
    import dbus
    import dbus.mainloop.glib
    from gi.repository import GLib
except ImportError as e:
    if 'linux' not in sys.platform:
        raise e
    print(e)
    print('Install packages `libgirepository1.0-dev gir1.2-gtk-3.0 libcairo2-dev libdbus-1-dev libdbus-glib-1-dev` for resolving the issue')
    print('Run `pip install -r $IDF_PATH/tools/ble/requirements.txt` for resolving the issue')
    raise

from . import lib_gap, lib_gatt

BLUEZ_SERVICE_NAME = 'org.bluez'
DBUS_OM_IFACE = 'org.freedesktop.DBus.ObjectManager'
DBUS_PROP_IFACE = 'org.freedesktop.DBus.Properties'

ADAPTER_IFACE = 'org.bluez.Adapter1'
DEVICE_IFACE = 'org.bluez.Device1'

GATT_MANAGER_IFACE = 'org.bluez.GattManager1'
LE_ADVERTISING_MANAGER_IFACE = 'org.bluez.LEAdvertisingManager1'

GATT_SERVICE_IFACE = 'org.bluez.GattService1'
GATT_CHRC_IFACE = 'org.bluez.GattCharacteristic1'


class DBusException(dbus.exceptions.DBusException):
    pass


class Characteristic:
    def __init__(self):
        self.iface = None
        self.path = None
        self.props = None


class Service:
    def __init__(self):
        self.iface = None
        self.path = None
        self.props = None
        self.chars = []


class Device:
    def __init__(self):
        self.iface = None
        self.path = None
        self.props = None
        self.name = None
        self.addr = None
        self.services = []


class Adapter:
    def __init__(self):
        self.iface = None
        self.path = None
        self.props = None


class BLE_Bluez_Client:
    def __init__(self, iface=None):
        self.bus = None
        self.hci_iface = iface
        self.adapter = Adapter()
        self.device = None
        self.gatt_app = None
        self.gatt_mgr = None
        self.mainloop = None
        self.loop_cnt = 0

        try:
            dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
            self.bus = dbus.SystemBus()
        except dbus.exceptions.DBusException as dbus_err:
            raise DBusException('Failed to initialise client: {}'.format(dbus_err))
        except Exception as err:
            raise Exception('Failed to initialise client: {}'.format(err))

    def __del__(self):
        try:
            # Cleanup
            self.disconnect()
            print('Test Exit')
        except Exception as e:
            print(e)

    def set_adapter(self):
        '''
            Discover Bluetooth Adapter
            Power On Bluetooth Adapter
        '''
        try:
            print('discovering adapter')
            dbus_obj_mgr = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, '/'), DBUS_OM_IFACE)
            dbus_objs = dbus_obj_mgr.GetManagedObjects()
            for path, interfaces in dbus_objs.items():
                adapter = interfaces.get(ADAPTER_IFACE)
                if adapter is not None and path.endswith(self.hci_iface):
                    self.adapter.iface = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, path), ADAPTER_IFACE)
                    self.adapter.props = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, path), DBUS_PROP_IFACE)
                    self.adapter.path = path
                    break

            if self.adapter.iface is None:
                print('bluetooth adapter not found')
                return False

            print('bluetooth adapter discovered')
            print('checking if bluetooth adapter is already powered on')
            # Check if adapter is already powered on
            powered = self.adapter.props.Get(ADAPTER_IFACE, 'Powered')
            if powered == 1:
                print('adapter already powered on')
                return True
            # Power On Adapter
            print('powering on adapter')
            self.adapter.props.Set(ADAPTER_IFACE, 'Powered', dbus.Boolean(1))
            # Check if adapter is powered on
            print('checking if adapter is powered on')
            for cnt in range(10, 0, -1):
                time.sleep(5)
                powered_on = self.adapter.props.Get(ADAPTER_IFACE, 'Powered')
                if powered_on == 1:
                    # Set adapter props again with powered on value
                    self.adapter.props = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, self.adapter.path), DBUS_PROP_IFACE)
                    print('bluetooth adapter powered on')
                    return True
                print('number of retries left({})'.format(cnt - 1))

            # Adapter not powered on
            print('bluetooth adapter not powered on')
            return False

        except Exception as err:
            raise Exception('Failed to set adapter: {}'.format(err))

    def connect(self, devname=None, devaddr=None):
        '''
            Start Discovery and Connect to the device
        '''
        try:
            device_found = None
            start_discovery = False
            self.device = Device()

            discovery_val = self.adapter.props.Get(ADAPTER_IFACE, 'Discovering')
            # Start Discovery
            if discovery_val == 0:
                print('starting discovery')
                self.adapter.iface.StartDiscovery()
                start_discovery = True

                for cnt in range(10, 0, -1):
                    time.sleep(5)
                    discovery_val = self.adapter.props.Get(ADAPTER_IFACE, 'Discovering')
                    if discovery_val == 1:
                        print('start discovery successful')
                        break
                    print('number of retries left ({})'.format(cnt - 1))

                if discovery_val == 0:
                    print('start discovery failed')
                    return False

            # Get device
            for cnt in range(10, 0, -1):
                # Wait for device to be discovered
                time.sleep(5)
                device_found = self.get_device(
                    devname=devname,
                    devaddr=devaddr)
                if device_found:
                    break
                # Retry
                print('number of retries left ({})'.format(cnt - 1))

            if not device_found:
                print('expected device {} [ {} ] not found'.format(devname, devaddr))
                return False

            # Connect to expected device found
            print('connecting to device {} [ {} ] '.format(self.device.name, self.device.addr))
            self.device.iface.Connect(dbus_interface=DEVICE_IFACE)
            for cnt in range(10, 0, -1):
                time.sleep(5)
                connected = self.device.props.Get(DEVICE_IFACE, 'Connected')
                if connected == 1:
                    # Set device props again with connected on value
                    self.device.props = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, self.device.path), DBUS_PROP_IFACE)
                    print('connected to device with iface {}'.format(self.device.path))
                    return True
                print('number of retries left({})'.format(cnt - 1))

            # Device not connected
            print('connection to device failed')
            return False

        except Exception as err:
            raise Exception('Connect to device failed : {}'.format(err))
        finally:
            try:
                if start_discovery:
                    print('stopping discovery')
                    self.adapter.iface.StopDiscovery()
                    for cnt in range(10, 0, -1):
                        time.sleep(5)
                        discovery_val = self.adapter.props.Get(ADAPTER_IFACE, 'Discovering')
                        if discovery_val == 0:
                            print('stop discovery successful')
                            break
                        print('number of retries left ({})'.format(cnt - 1))
                    if discovery_val == 1:
                        print('stop discovery failed')
            except dbus.exceptions.DBusException as dbus_err:
                print('Warning: Failure during cleanup for device connection : {}'.format(dbus_err))

    def get_device(self, devname=None, devaddr=None):
        '''
            Get device based on device name
            and device address and connect to device
        '''
        dev_path = None
        expected_device_addr = devaddr.lower()
        expected_device_name = devname.lower()

        print('checking if expected device {} [ {} ] is present'.format(devname, devaddr))

        dbus_obj_mgr = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, '/'), DBUS_OM_IFACE)
        dbus_objs = dbus_obj_mgr.GetManagedObjects()

        # Check if expected device is present
        for path, interfaces in dbus_objs.items():
            if DEVICE_IFACE not in interfaces.keys():
                continue

            # Check expected device address is received device address
            received_device_addr_path = (path.replace('_', ':')).lower()
            if expected_device_addr not in received_device_addr_path:
                continue

            device_props = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, path), DBUS_PROP_IFACE)
            received_device_name = device_props.Get(DEVICE_IFACE, 'Name').lower()

            # Check expected device name is received device name
            if expected_device_name == received_device_name:
                # Set device iface path
                dev_path = path
                break

        if not dev_path:
            print('\nBLE device not found')
            return False

        print('device {} [ {} ] found'.format(devname, devaddr))

        # Set device details
        self.device.iface = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, dev_path), DEVICE_IFACE)
        self.device.props = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, dev_path), DBUS_PROP_IFACE)
        self.device.path = dev_path
        self.device.name = devname
        self.device.addr = devaddr
        return True

    def get_services(self):
        '''
        Retrieve Services found in the device connected
        '''
        try:
            # Get current dbus objects
            dbus_obj_mgr = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, '/'), DBUS_OM_IFACE)
            dbus_objs = dbus_obj_mgr.GetManagedObjects()

            # Get services
            for path, interfaces in dbus_objs.items():
                if GATT_SERVICE_IFACE in interfaces.keys():
                    if not path.startswith(self.device.path):
                        continue
                    received_service = self.bus.get_object(BLUEZ_SERVICE_NAME, path)
                    # Retrieve all services on device iface path
                    # and set each service received
                    service = Service()
                    service.path = path
                    service.iface = dbus.Interface(received_service, GATT_SERVICE_IFACE)
                    service.props = dbus.Interface(received_service, DBUS_PROP_IFACE)
                    self.device.services.append(service)

            if not self.device.services:
                print('no services found for device: {}'.format(self.device.path))
                return False

            return True

        except Exception as err:
            raise Exception('Failed to get services: {}'.format(err))

    def get_chars(self):
        '''
            Get characteristics of the services set for the device connected
        '''
        try:
            if not self.device.services:
                print('No services set for device: {}'.format(self.device.path))
                return

            # Read chars for all the services received for device
            for service in self.device.services:
                char_found = False
                dbus_obj_mgr = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, '/'), DBUS_OM_IFACE)
                dbus_objs = dbus_obj_mgr.GetManagedObjects()
                for path, interfaces in dbus_objs.items():
                    if GATT_CHRC_IFACE in interfaces.keys():
                        if not path.startswith(self.device.path):
                            continue
                        if not path.startswith(service.path):
                            continue
                        # Set characteristics
                        received_char = self.bus.get_object(BLUEZ_SERVICE_NAME, path)
                        char = Characteristic()
                        char.path = path
                        char.iface = dbus.Interface(received_char, GATT_CHRC_IFACE)
                        char.props = dbus.Interface(received_char, DBUS_PROP_IFACE)
                        service.chars.append(char)
                        char_found = True

                if not char_found:
                    print('Characteristic not found for service: {}'.format(service.iface))

        except Exception as err:
            raise Exception('Failed to get characteristics : {}'.format(err))

    def read_chars(self):
        '''
            Read value of characteristics
        '''
        try:
            if not self.device.services:
                print('No services set for device: {}'.format(self.device.path))
                return

            # Read chars for all services of device
            for service in self.device.services:
                # Read properties of characteristic
                for char in service.chars:
                    # Print path
                    print('Characteristic: {}'.format(char.path))
                    # Print uuid
                    uuid = char.props.Get(GATT_CHRC_IFACE, 'UUID')
                    print('UUID: {}'.format(uuid))
                    # Print flags
                    flags = [flag for flag in char.props.Get(GATT_CHRC_IFACE, 'Flags')]
                    print('Flags: {}'.format(flags))
                    # Read value if `read` flag is present
                    if 'read' in flags:
                        value = char.iface.ReadValue({}, dbus_interface=GATT_CHRC_IFACE)
                        print('Value: {}'.format(value))

        except Exception as err:
            raise Exception('Failed to read characteristics : {}'.format(err))

    def write_chars(self, new_value):
        '''
            Write to characteristics
        '''
        try:
            if not self.device.services:
                print('No services set for device: {}'.format(self.device.path))
                return False

            print('writing data to characteristics with read and write permission')
            # Read chars of all services of device
            for service in self.device.services:
                if not service.chars:
                    print('No chars found for service: {}'.format(service.path))
                    continue
                for char in service.chars:
                    flags = [flag.lower() for flag in char.props.Get(GATT_CHRC_IFACE, 'Flags')]
                    if not ('read' in flags and 'write' in flags):
                        continue

                    # Write new value to characteristic
                    curr_value = char.iface.ReadValue({}, dbus_interface=GATT_CHRC_IFACE)
                    print('current value: {}'.format(curr_value))

                    print('writing {} to characteristic {}'.format(new_value, char.path))
                    char.iface.WriteValue(new_value, {}, dbus_interface=GATT_CHRC_IFACE)

                    time.sleep(5)
                    updated_value = char.iface.ReadValue({}, dbus_interface=GATT_CHRC_IFACE)
                    print('updated value: {}'.format(updated_value))

                    if not (ord(new_value) == int(updated_value[0])):
                        print('write operation to {} failed'.format(char.path))
                        return False
                    print('write operation to {} successful'.format(char.path))
                    return True

        except Exception as err:
            raise Exception('Failed to write to characteristics: {}'.format(err))

    def get_char_if_exists(self, char_uuid):
        '''
            Get char if exists for given uuid
        '''
        try:
            for service in self.device.services:
                for char in service.chars:
                    curr_uuid = char.props.Get(GATT_CHRC_IFACE, 'UUID')
                    if char_uuid.lower() in curr_uuid.lower():
                        return char
            print('char {} not found'.format(char_uuid))
            return False
        except Exception as err:
            raise Exception('Failed to get char based on uuid {} - {}'.format(char_uuid, err))

    def get_service_if_exists(self, service_uuid):
        try:
            for service in self.device.services:
                uuid = service.props.Get(GATT_SERVICE_IFACE, 'UUID')
                if service_uuid.lower() in uuid.lower():
                    return service
            print('service {} not found'.format(service_uuid))
            return False
        except Exception as err:
            raise Exception('Failed to get service based on uuid {} - {}'.format(service_uuid, err))

    def start_notify(self, char):
        try:
            notify_started = 0
            notifying = char.props.Get(GATT_CHRC_IFACE, 'Notifying')
            if notifying == 0:
                # Start Notify
                char.iface.StartNotify()
                notify_started = 1
                # Check notify started
                for _ in range(10, 0, -1):
                    notifying = char.props.Get(GATT_CHRC_IFACE, 'Notifying')
                    if notifying == 1:
                        print('subscribe to notifications: on')
                        break
                if notifying == 0:
                    print('Failed to start notifications')
                    return False

            # Get updated value
            for _ in range(10, 0, -1):
                time.sleep(1)
                char_value = char.props.Get(GATT_CHRC_IFACE, 'Value')
                print(char_value)

            return None

        except Exception as err:
            raise Exception('Failed to perform notification operation: {}'.format(err))
        finally:
            try:
                if notify_started == 1:
                    # Stop notify
                    char.iface.StopNotify()
                    for _ in range(10, 0, -1):
                        notifying = char.props.Get(GATT_CHRC_IFACE, 'Notifying')
                        if notifying == 0:
                            print('subscribe to notifications: off')
                            break
                    if notifying == 1:
                        print('Failed to stop notifications')
            except dbus.exceptions.DBusException as dbus_err:
                print('Warning: Failure during cleanup for start notify : {}'.format(dbus_err))

    def _create_mainloop(self):
        '''
            Create GLibMainLoop
        '''
        if not self.mainloop:
            self.mainloop = GLib.MainLoop()

    def register_gatt_app(self):
        '''
            Create Gatt Application
            Register Gatt Application
        '''
        try:
            # Create mainloop, if does not exist
            self._create_mainloop()

            # Create Gatt Application
            self.gatt_app = lib_gatt.AlertNotificationApp(self.bus, self.adapter.path)
            print('GATT Application created')
            self.gatt_mgr = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, self.adapter.path), GATT_MANAGER_IFACE)

            # Register Gatt Application
            self.gatt_mgr.RegisterApplication(
                self.gatt_app, {},
                reply_handler=self.gatt_app_success_handler,
                error_handler=self.gatt_app_error_handler)
            self.mainloop.run()

        except dbus.exceptions.DBusException as dbus_err:
            raise DBusException('Failed to create GATT Application : {}'.format(dbus_err))
        except Exception as err:
            raise Exception('Failed to register Gatt Application: {}'.format(err))

    def gatt_app_success_handler(self):
        print('GATT Application successfully registered')
        self.mainloop.quit()

    def gatt_app_error_handler(self):
        raise DBusException('Failed to register GATT Application')

    def check_le_iface(self):
        '''
            Check if LEAdvertisingManager1 interface exists
        '''
        try:
            dbus_obj_mgr = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, '/'), DBUS_OM_IFACE)
            dbus_objs = dbus_obj_mgr.GetManagedObjects()
            for path, iface in dbus_objs.items():
                if LE_ADVERTISING_MANAGER_IFACE in iface:
                    le_adv_iface_path = path
                    break
            # Check LEAdvertisingManager1 interface is found
            assert le_adv_iface_path, '\n Cannot start advertising. LEAdvertisingManager1 Interface not found'

            return le_adv_iface_path

        except AssertionError:
            raise
        except Exception as err:
            raise Exception('Failed to find LEAdvertisingManager1 interface: {}'.format(err))

    def register_adv(self, adv_host_name, adv_type, adv_uuid):
        try:
            # Gatt Application is expected to be registered
            if not self.gatt_app:
                print('No Gatt Application is registered')
                return

            adv_iface_index = 0

            # Create mainloop, if does not exist
            self._create_mainloop()

            # Check LEAdvertisingManager1 interface exists
            le_iface_path = self.check_le_iface()

            # Create Advertisement data
            leadv_obj = lib_gap.Advertisement(
                self.bus,
                adv_iface_index,
                adv_type,
                adv_uuid,
                adv_host_name)
            print('Advertisement registered')

            # Register Advertisement
            leadv_mgr_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, le_iface_path), LE_ADVERTISING_MANAGER_IFACE)
            leadv_mgr_iface_obj.RegisterAdvertisement(
                leadv_obj.get_path(), {},
                reply_handler=self.adv_success_handler,
                error_handler=self.adv_error_handler)

            # Handler to read events received and exit from mainloop
            GLib.timeout_add_seconds(3, self.check_adv)

            self.mainloop.run()

        except AssertionError:
            raise
        except dbus.exceptions.DBusException as dbus_err:
            raise DBusException('Failure during registering advertisement : {}'.format(dbus_err))
        except Exception as err:
            raise Exception('Failure during registering advertisement : {}'.format(err))
        else:
            try:
                try:
                    # Stop Notify if not already stopped
                    chars = self.gatt_app.service.get_characteristics()
                    for char in chars:
                        if char.uuid == lib_gatt.CHAR_UUIDS['UNREAD_ALERT_STATUS_UUID']:
                            if char.notifying:
                                char.StopNotify()
                except dbus.exceptions.DBusException as dbus_err:
                    print('Warning: {}'.format(dbus_err))

                try:
                    # Unregister Advertisement
                    leadv_mgr_iface_obj.UnregisterAdvertisement(leadv_obj.get_path())
                except dbus.exceptions.DBusException as dbus_err:
                    print('Warning: {}'.format(dbus_err))

                try:
                    # Remove advertising data
                    dbus.service.Object.remove_from_connection(leadv_obj)
                except LookupError as err:
                    print('Warning: Failed to remove connection from dbus for advertisement object: {} - {}'.format(leadv_obj, err))

                try:
                    # Unregister Gatt Application
                    self.gatt_mgr.UnregisterApplication(self.gatt_app.get_path())
                except dbus.exceptions.DBusException as dbus_err:
                    print('Warning: {}'.format(dbus_err))

                try:
                    # Remove Gatt Application
                    dbus.service.Object.remove_from_connection(self.gatt_app)
                except LookupError as err:
                    print('Warning: Failed to remove connection from dbus for Gatt application object: {} - {}'.format(self.gatt_app, err))

            except RuntimeError as err:
                print('Warning: Failure during cleanup of Advertisement: {}'.format(err))

    def adv_success_handler(self):
        print('Registered Advertisement successfully')

    def adv_error_handler(self, err):
        raise DBusException('{}'.format(err))

    def check_adv(self):
        '''
            Handler to check for events triggered (read/write/subscribe)
            for advertisement registered for AlertNotificationApp
        '''
        try:
            retry = 10
            # Exit loop if read and write and subscribe is successful
            if self.gatt_app.service.get_char_status(lib_gatt.CHAR_UUIDS['SUPPORT_NEW_ALERT_UUID'], 'read') and \
                self.gatt_app.service.get_char_status(lib_gatt.CHAR_UUIDS['ALERT_NOTIF_UUID'], 'write') and \
                    self.gatt_app.service.get_char_status(lib_gatt.CHAR_UUIDS['UNREAD_ALERT_STATUS_UUID'], 'notify'):
                if self.mainloop.is_running():
                    self.mainloop.quit()
                    # return False to stop polling
                    return False

            self.loop_cnt += 1
            print('Check read/write/subscribe events are received...Retry {}'.format(self.loop_cnt))

            # Exit loop if max retry value is reached and
            # all three events (read and write and subscribe) have not yet passed
            # Retry total 10 times
            if self.loop_cnt == (retry - 1):
                if self.mainloop.is_running():
                    self.mainloop.quit()
                    # return False to stop polling
                    return False

            # return True to continue polling
            return True

        except RuntimeError as err:
            print('Failure in advertisment handler: {}'.format(err))
            if self.mainloop.is_running():
                self.mainloop.quit()
                # return False to stop polling
                return False

    def disconnect(self):
        '''
        Disconnect device
        '''
        try:
            if not self.device or not self.device.iface:
                return
            print('disconnecting device')
            # Disconnect device
            device_conn = self.device.props.Get(DEVICE_IFACE, 'Connected')
            if device_conn == 1:
                self.device.iface.Disconnect(dbus_interface=DEVICE_IFACE)
                for cnt in range(10, 0, -1):
                    time.sleep(5)
                    device_conn = self.device.props.Get(DEVICE_IFACE, 'Connected')
                    if device_conn == 0:
                        print('device disconnected')
                        break
                    print('number of retries left ({})'.format(cnt - 1))
                if device_conn == 1:
                    print('failed to disconnect device')

                self.adapter.iface.RemoveDevice(self.device.iface)
                self.device = None

        except dbus.exceptions.DBusException as dbus_err:
            print('Warning: {}'.format(dbus_err))
        except Exception as err:
            raise Exception('Failed to disconnect device: {}'.format(err))
