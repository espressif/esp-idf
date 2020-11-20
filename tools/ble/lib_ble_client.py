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
import traceback

try:
    from future.moves.itertools import zip_longest
    import dbus
    import dbus.mainloop.glib
    from gi.repository import GLib
except ImportError as e:
    if 'linux' not in sys.platform:
        raise e
    print(e)
    print("Install packages `libgirepository1.0-dev gir1.2-gtk-3.0 libcairo2-dev libdbus-1-dev libdbus-glib-1-dev` for resolving the issue")
    print("Run `pip install -r $IDF_PATH/tools/ble/requirements.txt` for resolving the issue")
    raise

from . import lib_gatt
from . import lib_gap

srv_added_old_cnt = 0
srv_added_new_cnt = 0
verify_signal_check = 0
blecent_retry_check_cnt = 0
gatt_app_retry_check_cnt = 0
verify_service_cnt = 0
verify_readchars_cnt = 0
adv_retry_check_cnt = 0
blecent_adv_uuid = '1811'
gatt_app_obj_check = False
gatt_app_reg_check = False
adv_checks_done = False
gatt_checks_done = False
adv_data_check = False
adv_reg_check = False
read_req_check = False
write_req_check = False
subscribe_req_check = False
ble_hr_chrc = False
discovery_start = False
signal_caught = False
test_checks_pass = False
adv_stop = False
services_resolved = False
service_uuid_found = False
adapter_on = False
device_connected = False
gatt_app_registered = False
adv_registered = False
adv_active_instance = False
chrc_value_cnt = False

BLUEZ_SERVICE_NAME = 'org.bluez'
DBUS_OM_IFACE = 'org.freedesktop.DBus.ObjectManager'
DBUS_PROP_IFACE = 'org.freedesktop.DBus.Properties'

ADAPTER_IFACE = 'org.bluez.Adapter1'
DEVICE_IFACE = 'org.bluez.Device1'

GATT_MANAGER_IFACE = 'org.bluez.GattManager1'
LE_ADVERTISING_MANAGER_IFACE = 'org.bluez.LEAdvertisingManager1'

GATT_SERVICE_IFACE = 'org.bluez.GattService1'
GATT_CHRC_IFACE = 'org.bluez.GattCharacteristic1'


# Set up the main loop.
dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
dbus.mainloop.glib.threads_init()
# Set up the event main loop.
event_loop = GLib.MainLoop()


def verify_signal_is_caught():
    global verify_signal_check
    verify_signal_check += 1

    if (not signal_caught and verify_signal_check == 15) or (signal_caught):
        if event_loop.is_running():
            event_loop.quit()
            return False  # polling for checks will stop

    return True  # polling will continue


def set_props_status(props):
    """
        Set Adapter status if it is powered on or off
    """
    global adapter_on, services_resolved, GATT_OBJ_REMOVED, gatt_app_registered, \
        adv_registered, adv_active_instance, device_connected, CHRC_VALUE, chrc_value_cnt, \
        signal_caught
    is_service_uuid = False
    # Signal caught for change in Adapter Powered property
    if 'Powered' in props:
        if props['Powered'] == 1:
            signal_caught = True
            adapter_on = True
        else:
            signal_caught = True
            adapter_on = False
    if 'ServicesResolved' in props:
        if props['ServicesResolved'] == 1:
            signal_caught = True
            services_resolved = True
        else:
            signal_caught = True
            services_resolved = False
    if 'UUIDs' in props:
        # Signal caught for add/remove GATT data having service uuid
        for uuid in props['UUIDs']:
            if blecent_adv_uuid in uuid:
                is_service_uuid = True
        if not is_service_uuid:
            # Signal caught for removing GATT data having service uuid
            # and for unregistering GATT application
            gatt_app_registered = False
            lib_gatt.GATT_APP_OBJ = False
    if 'ActiveInstances' in props:
        # Signal caught for Advertising - add/remove Instances property
        if props['ActiveInstances'] == 1:
            adv_active_instance = True
        elif props['ActiveInstances'] == 0:
            adv_active_instance = False
            adv_registered = False
            lib_gap.ADV_OBJ = False
    if 'Connected' in props:
        # Signal caught for device connect/disconnect
        if props['Connected'] == 1:
            signal_caught = True
            device_connected = True
        else:
            signal_caught = True
            device_connected = False
    if 'Value' in props:
        # Signal caught for change in chars value
        if ble_hr_chrc:
            chrc_value_cnt += 1
            print(props['Value'])
            if chrc_value_cnt == 10:
                signal_caught = True
                return False

    return False


def props_change_handler(iface, changed_props, invalidated):
    """
        PropertiesChanged Signal handler.
        Catch and print information about PropertiesChanged signal.
    """

    if iface == ADAPTER_IFACE:
        set_props_status(changed_props)
    if iface == LE_ADVERTISING_MANAGER_IFACE:
        set_props_status(changed_props)
    if iface == DEVICE_IFACE:
        set_props_status(changed_props)
    if iface == GATT_CHRC_IFACE:
        set_props_status(changed_props)


class BLE_Bluez_Client:
    def __init__(self, iface, devname=None, devaddr=None):
        self.bus = None
        self.device = None
        self.devname = devname
        self.devaddr = devaddr
        self.iface = iface
        self.ble_objs = None
        self.props_iface_obj = None
        self.adapter_path = []
        self.adapter = None
        self.services = []
        self.srv_uuid = []
        self.chars = {}
        self.char_uuid = []

        try:
            self.bus = dbus.SystemBus()
            om_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, "/"), DBUS_OM_IFACE)
            self.ble_objs = om_iface_obj.GetManagedObjects()

        except Exception as e:
            print(e)

    def __del__(self):
        try:
            print("Test Exit")
        except Exception as e:
            print(e)
            sys.exit(1)

    def set_adapter(self):
        '''
            Discover Bluetooth Adapter
            Power On Bluetooth Adapter
        '''
        global verify_signal_check, signal_caught, adapter_on
        verify_signal_check = 0
        adapter_on = False
        try:
            print("discovering adapter...")
            for path, interfaces in self.ble_objs.items():
                adapter = interfaces.get(ADAPTER_IFACE)
                if adapter is not None:
                    if path.endswith(self.iface):
                        self.adapter = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, path), ADAPTER_IFACE)
                        # Create Properties Interface object only after adapter is found
                        self.props_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, path), DBUS_PROP_IFACE)
                        self.adapter_path = [path, interfaces]
                        # Check adapter status - power on/off
                        set_props_status(interfaces[ADAPTER_IFACE])
                        break

            if self.adapter is None:
                raise Exception("Bluetooth adapter not found")

            if self.props_iface_obj is None:
                raise Exception("Properties interface not found")

            print("bluetooth adapter discovered")

            # Check if adapter is already powered on
            if adapter_on:
                print("Adapter already powered on")
                return True

            # Power On Adapter
            print("powering on adapter...")
            self.props_iface_obj.connect_to_signal('PropertiesChanged', props_change_handler)
            self.props_iface_obj.Set(ADAPTER_IFACE, "Powered", dbus.Boolean(1))

            signal_caught = False
            GLib.timeout_add_seconds(5, verify_signal_is_caught)
            event_loop.run()

            if adapter_on:
                print("bluetooth adapter powered on")
                return True
            else:
                raise Exception("Failure: bluetooth adapter not powered on")

        except Exception:
            print(traceback.format_exc())
            return False

    def connect(self):
        '''
            Connect to the device discovered
            Retry 10 times to discover and connect to device
        '''
        global discovery_start, signal_caught, device_connected, verify_signal_check
        device_found = False
        device_connected = False
        try:
            self.adapter.StartDiscovery()
            print("\nStarted Discovery")

            discovery_start = True

            for retry_cnt in range(10,0,-1):
                verify_signal_check = 0
                try:
                    if self.device is None:
                        print("\nConnecting to device...")
                        # Wait for device to be discovered
                        time.sleep(5)
                        device_found = self.get_device()
                    if device_found:
                        self.device.Connect(dbus_interface=DEVICE_IFACE)
                        time.sleep(15)
                        signal_caught = False
                        GLib.timeout_add_seconds(5, verify_signal_is_caught)
                        event_loop.run()
                        if device_connected:
                            print("\nConnected to device")
                            return True
                        else:
                            raise Exception
                except Exception as e:
                    print(e)
                    print("\nRetries left", retry_cnt - 1)
                    continue

            # Device not found
            return False

        except Exception:
            print(traceback.format_exc())
            self.device = None
            return False

    def get_device(self):
        '''
            Discover device based on device name
            and device address and connect
        '''
        dev_path = None

        om_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, "/"), DBUS_OM_IFACE)
        self.ble_objs = om_iface_obj.GetManagedObjects()
        for path, interfaces in self.ble_objs.items():
            if DEVICE_IFACE not in interfaces.keys():
                continue
            device_addr_iface = (path.replace('_', ':')).lower()
            dev_addr = self.devaddr.lower()
            if dev_addr in device_addr_iface and \
                    interfaces[DEVICE_IFACE].get("Name") == self.devname:
                dev_path = path
                break

        if dev_path is None:
            raise Exception("\nBLE device not found")

        device_props_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, dev_path), DBUS_PROP_IFACE)
        device_props_iface_obj.connect_to_signal('PropertiesChanged', props_change_handler)

        self.device = self.bus.get_object(BLUEZ_SERVICE_NAME, dev_path)
        return True

    def srvc_iface_added_handler(self, path, interfaces):
        '''
        Add services found as lib_ble_client obj
        '''
        if self.device and path.startswith(self.device.object_path):
            if GATT_SERVICE_IFACE in interfaces.keys():
                service = self.bus.get_object(BLUEZ_SERVICE_NAME, path)
                uuid = service.Get(GATT_SERVICE_IFACE, 'UUID', dbus_interface=DBUS_PROP_IFACE)
                if uuid not in self.srv_uuid:
                    self.srv_uuid.append(uuid)
                if path not in self.services:
                    self.services.append(path)

    def verify_service_uuid_found(self, service_uuid):
        '''
        Verify service uuid found
        '''
        global service_uuid_found

        srv_uuid_found = [uuid for uuid in self.srv_uuid if service_uuid in uuid]
        if srv_uuid_found:
            service_uuid_found = True

    def get_services(self, service_uuid=None):
        '''
        Retrieve Services found in the device connected
        '''
        global signal_caught, service_uuid_found, services_resolved, verify_signal_check
        verify_signal_check = 0
        service_uuid_found = False
        services_resolved = False
        signal_caught = False

        try:
            om_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, "/"), DBUS_OM_IFACE)
            self.ble_objs = om_iface_obj.GetManagedObjects()
            for path, interfaces in self.ble_objs.items():
                self.srvc_iface_added_handler(path, interfaces)
            # If services not found, then they may not have been added yet on dbus
            if not self.services:
                GLib.timeout_add_seconds(5, verify_signal_is_caught)
                om_iface_obj.connect_to_signal('InterfacesAdded', self.srvc_iface_added_handler)
                event_loop.run()
                if not services_resolved:
                    raise Exception("Services not found...")

            if service_uuid:
                self.verify_service_uuid_found(service_uuid)
                if not service_uuid_found:
                    raise Exception("Service with uuid: %s not found..." % service_uuid)

            # Services found
            return self.srv_uuid
        except Exception:
            print(traceback.format_exc())
            return False

    def chrc_iface_added_handler(self, path, interfaces):
        '''
        Add services found as lib_ble_client obj
        '''
        global chrc, chrc_discovered, signal_caught
        chrc_val = None

        if self.device and path.startswith(self.device.object_path):
            if GATT_CHRC_IFACE in interfaces.keys():
                chrc = self.bus.get_object(BLUEZ_SERVICE_NAME, path)
                chrc_props = chrc.GetAll(GATT_CHRC_IFACE,
                                         dbus_interface=DBUS_PROP_IFACE)
                chrc_flags = chrc_props['Flags']
                if 'read' in chrc_flags:
                    chrc_val = chrc.ReadValue({}, dbus_interface=GATT_CHRC_IFACE)
                uuid = chrc_props['UUID']
                self.chars[path] = chrc_val, chrc_flags, uuid
                signal_caught = True

    def read_chars(self):
        '''
            Read characteristics found in the device connected
        '''
        global iface_added, chrc_discovered, signal_caught, verify_signal_check
        verify_signal_check = 0
        chrc_discovered = False
        iface_added = False
        signal_caught = False

        try:
            om_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, "/"), DBUS_OM_IFACE)
            self.ble_objs = om_iface_obj.GetManagedObjects()
            for path, interfaces in self.ble_objs.items():
                self.chrc_iface_added_handler(path, interfaces)

            # If chars not found, then they have not been added yet to interface
            time.sleep(15)
            if not self.chars:
                iface_added = True
                GLib.timeout_add_seconds(5, verify_signal_is_caught)
                om_iface_obj.connect_to_signal('InterfacesAdded', self.chars_iface_added_handler)
                event_loop.run()
            return self.chars
        except Exception:
            print(traceback.format_exc())
            return False

    def write_chars(self, write_val):
        '''
            Write characteristics to the device connected
        '''
        chrc = None
        chrc_val = None
        char_write_props = False

        try:
            for path, props in self.chars.items():
                if 'write' in props[1]:  # check permission
                    char_write_props = True
                    chrc = self.bus.get_object(BLUEZ_SERVICE_NAME, path)
                    chrc.WriteValue(write_val,{},dbus_interface=GATT_CHRC_IFACE)
                    if 'read' in props[1]:
                        chrc_val = chrc.ReadValue({}, dbus_interface=GATT_CHRC_IFACE)
                    else:
                        print("Warning: Cannot read value. Characteristic does not have read permission.")
                    if not (ord(write_val) == int(chrc_val[0])):
                        print("\nWrite Failed")
                        return False
                    self.chars[path] = chrc_val, props[1], props[2]  # update value
            if not char_write_props:
                raise Exception("Failure: Cannot perform write operation. Characteristic does not have write permission.")

            return self.chars
        except Exception:
            print(traceback.format_exc())
            return False

    def hr_update_simulation(self, hr_srv_uuid, hr_char_uuid):
        '''
            Start Notifications
            Retrieve updated value
            Stop Notifications
        '''
        global ble_hr_chrc, verify_signal_check, signal_caught, chrc_value_cnt

        srv_path = None
        chrc = None
        uuid = None
        chrc_path = None
        chars_ret = None
        ble_hr_chrc = True
        chrc_value_cnt = 0

        try:
            # Get HR Measurement characteristic
            services = list(zip_longest(self.srv_uuid, self.services))
            for uuid, path in services:
                if hr_srv_uuid in uuid:
                    srv_path = path
                    break

            if srv_path is None:
                raise Exception("Failure: HR UUID:", hr_srv_uuid, "not found")

            chars_ret = self.read_chars()

            for path, props in chars_ret.items():
                if path.startswith(srv_path):
                    chrc = self.bus.get_object(BLUEZ_SERVICE_NAME, path)
                    chrc_path = path
                    if hr_char_uuid in props[2]:  # uuid
                        break
            if chrc is None:
                raise Exception("Failure: Characteristics for service: ", srv_path, "not found")

            # Subscribe to notifications
            print("\nSubscribe to notifications: On")
            chrc.StartNotify(dbus_interface=GATT_CHRC_IFACE)

            chrc_props_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, chrc_path), DBUS_PROP_IFACE)
            chrc_props_iface_obj.connect_to_signal('PropertiesChanged', props_change_handler)

            signal_caught = False
            verify_signal_check = 0
            GLib.timeout_add_seconds(5, verify_signal_is_caught)
            event_loop.run()
            chrc.StopNotify(dbus_interface=GATT_CHRC_IFACE)
            time.sleep(2)
            print("\nSubscribe to notifications: Off")

            ble_hr_chrc = False
            return True

        except Exception:
            print(traceback.format_exc())
            return False

    def create_and_reg_gatt_app(self):
        '''
            Create GATT data
            Register GATT Application
        '''
        global gatt_app_obj, gatt_manager_iface_obj, gatt_app_registered

        gatt_app_obj = None
        gatt_manager_iface_obj = None
        gatt_app_registered = False
        lib_gatt.GATT_APP_OBJ = False

        try:
            gatt_app_obj = lib_gatt.Application(self.bus, self.adapter_path[0])
            gatt_manager_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME,self.adapter_path[0]), GATT_MANAGER_IFACE)

            gatt_manager_iface_obj.RegisterApplication(gatt_app_obj, {},
                                                       reply_handler=self.gatt_app_handler,
                                                       error_handler=self.gatt_app_error_handler)
            return True
        except Exception:
            print(traceback.format_exc())
            return False

    def gatt_app_handler(self):
        '''
            GATT Application Register success handler
        '''
        global gatt_app_registered
        gatt_app_registered = True

    def gatt_app_error_handler(self, error):
        '''
            GATT Application Register error handler
        '''
        global gatt_app_registered
        gatt_app_registered = False

    def start_advertising(self, adv_host_name, adv_iface_index, adv_type, adv_uuid):
        '''
            Create Advertising data
            Register Advertisement
            Start Advertising
        '''
        global le_adv_obj, le_adv_manager_iface_obj, adv_active_instance, adv_registered

        le_adv_obj = None
        le_adv_manager_iface_obj = None
        le_adv_iface_path = None
        adv_active_instance = False
        adv_registered = False
        lib_gap.ADV_OBJ = False

        try:
            print("Advertising started")
            gatt_app_ret = self.create_and_reg_gatt_app()

            # Check if gatt app create and register command
            # is sent successfully
            assert gatt_app_ret

            GLib.timeout_add_seconds(2, self.verify_gatt_app_reg)
            event_loop.run()

            # Check if Gatt Application is registered
            assert gatt_app_registered

            for path,interface in self.ble_objs.items():
                if LE_ADVERTISING_MANAGER_IFACE in interface:
                    le_adv_iface_path = path

            # Check LEAdvertisingManager1 interface is found
            assert le_adv_iface_path, '\n Cannot start advertising. LEAdvertisingManager1 Interface not found'

            # Get device when connected
            if not self.device:
                om_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, "/"), DBUS_OM_IFACE)
                self.ble_objs = om_iface_obj.GetManagedObjects()

                for path, interfaces in self.ble_objs.items():
                    if DEVICE_IFACE not in interfaces.keys():
                        continue
                    device_props_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, path), DBUS_PROP_IFACE)
                    device_props_iface_obj.connect_to_signal('PropertiesChanged', props_change_handler)
                    self.device = self.bus.get_object(BLUEZ_SERVICE_NAME, path)

            le_adv_obj = lib_gap.Advertisement(self.bus, adv_iface_index, adv_type, adv_uuid, adv_host_name)
            le_adv_manager_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, le_adv_iface_path), LE_ADVERTISING_MANAGER_IFACE)

            le_adv_manager_iface_obj.RegisterAdvertisement(le_adv_obj.get_path(), {},
                                                           reply_handler=self.adv_handler,
                                                           error_handler=self.adv_error_handler)

            GLib.timeout_add_seconds(2, self.verify_adv_reg)
            event_loop.run()

            # Check if advertising is registered
            assert adv_registered

            ret_val = self.verify_blecent_app()

            # Check if blecent has passed
            assert ret_val

        except Exception:
            print(traceback.format_exc())
            return False

    def verify_blecent_app(self):
        '''
        Verify read/write/subscribe operations
        '''
        try:
            GLib.timeout_add_seconds(2, self.verify_blecent)
            event_loop.run()

            return test_checks_pass

        except Exception:
            print(traceback.format_exc())
            return False

    def adv_handler(self):
        '''
            Advertisement Register success handler
        '''
        global adv_registered
        adv_registered = True

    def adv_error_handler(self, error):
        '''
            Advertisement Register error handler
        '''
        global adv_registered
        adv_registered = False

    def verify_gatt_app_reg(self):
        """
        Verify GATT Application is registered
        """
        global gatt_app_retry_check_cnt, gatt_checks_done
        gatt_app_retry_check_cnt = 0
        gatt_checks_done = False

        # Check for success
        if lib_gatt.GATT_APP_OBJ:
            print("GATT Data created")
        if gatt_app_registered:
            print("GATT Application registered")
            gatt_checks_done = True
        if gatt_app_retry_check_cnt == 20:
            if not gatt_app_registered:
                print("Failure: GATT Application could not be registered")
            gatt_checks_done = True

        # End polling if app is registered or cnt has reached 10
        if gatt_checks_done:
            if event_loop.is_running():
                event_loop.quit()
            return False  # polling for checks will stop

        gatt_app_retry_check_cnt += 1
        # Default return True - polling for checks will continue
        return True

    def verify_adv_reg(self):
        """
        Verify Advertisement is registered
        """
        global adv_retry_check_cnt, adv_checks_done
        adv_retry_check_cnt = 0
        adv_checks_done = False

        if lib_gap.ADV_OBJ:
            print("Advertising data created")
        if adv_registered or adv_active_instance:
            print("Advertisement registered")
            adv_checks_done = True
        if adv_retry_check_cnt == 10:
            if not adv_registered and not adv_active_instance:
                print("Failure: Advertisement could not be registered")
            adv_checks_done = True

        # End polling if success or cnt has reached 10
        if adv_checks_done:
            if event_loop.is_running():
                event_loop.quit()
            return False  # polling for checks will stop

        adv_retry_check_cnt += 1
        # Default return True - polling for checks will continue
        return True

    def verify_blecent(self):
        """
            Verify blecent test commands are successful
        """
        global blecent_retry_check_cnt, read_req_check, write_req_check,\
            subscribe_req_check, test_checks_pass

        # Check for failures after 10 retries
        if blecent_retry_check_cnt == 10:
            # check for failures
            if not read_req_check:
                print("Failure: Read Request not received")
            if not write_req_check:
                print("Failure: Write Request not received")
            if not subscribe_req_check:
                print("Failure: Subscribe Request not received")

            # Blecent Test failed
            test_checks_pass = False
            if subscribe_req_check:
                lib_gatt.alert_status_char_obj.StopNotify()
        else:
            # Check for success
            if not read_req_check and lib_gatt.CHAR_READ:
                read_req_check = True
            if not write_req_check and lib_gatt.CHAR_WRITE:
                write_req_check = True
            if not subscribe_req_check and lib_gatt.CHAR_SUBSCRIBE:
                subscribe_req_check = True

        if read_req_check and write_req_check and subscribe_req_check:
            # all checks passed
            # Blecent Test passed
            test_checks_pass = True
            lib_gatt.alert_status_char_obj.StopNotify()

        if (blecent_retry_check_cnt == 10 or test_checks_pass):
            if event_loop.is_running():
                event_loop.quit()
            return False  # polling for checks will stop

        # Increment retry count
        blecent_retry_check_cnt += 1

        # Default return True - polling for checks will continue
        return True

    def verify_blecent_disconnect(self):
        """
            Verify cleanup is successful
        """
        global blecent_retry_check_cnt, gatt_app_obj_check, gatt_app_reg_check,\
            adv_data_check, adv_reg_check, adv_stop

        if blecent_retry_check_cnt == 0:
            gatt_app_obj_check = False
            gatt_app_reg_check = False
            adv_data_check = False
            adv_reg_check = False

        # Check for failures after 10 retries
        if blecent_retry_check_cnt == 10:
            # check for failures
            if not gatt_app_obj_check:
                print("Warning: GATT Data could not be removed")
            if not gatt_app_reg_check:
                print("Warning: GATT Application could not be unregistered")
            if not adv_data_check:
                print("Warning: Advertising data could not be removed")
            if not adv_reg_check:
                print("Warning: Advertisement could not be unregistered")

            # Blecent Test failed
            adv_stop = False
        else:
            # Check for success
            if not gatt_app_obj_check and not lib_gatt.GATT_APP_OBJ:
                print("GATT Data removed")
                gatt_app_obj_check = True
            if not gatt_app_reg_check and not gatt_app_registered:
                print("GATT Application unregistered")
                gatt_app_reg_check = True
            if not adv_data_check and not adv_reg_check and not (adv_registered or adv_active_instance or lib_gap.ADV_OBJ):
                print("Advertising data removed")
                print("Advertisement unregistered")
                adv_data_check = True
                adv_reg_check = True
                # all checks passed
                adv_stop = True

        if (blecent_retry_check_cnt == 10 or adv_stop):
            if event_loop.is_running():
                event_loop.quit()
            return False  # polling for checks will stop

        # Increment retry count
        blecent_retry_check_cnt += 1

        # Default return True - polling for checks will continue
        return True

    def disconnect(self):
        '''
        Before application exits:
            Advertisement is unregistered
            Advertisement object created is removed from dbus
            GATT Application is unregistered
            GATT Application object created is removed from dbus
            Disconnect device if connected
            Adapter is powered off
        '''
        try:
            global blecent_retry_check_cnt, discovery_start, verify_signal_check, signal_caught
            blecent_retry_check_cnt = 0
            verify_signal_check = 0

            print("\nexiting from test...")

            self.props_iface_obj.connect_to_signal('PropertiesChanged', props_change_handler)

            if adv_registered:
                # Unregister Advertisement
                le_adv_manager_iface_obj.UnregisterAdvertisement(le_adv_obj.get_path())

                # Remove Advertising data
                dbus.service.Object.remove_from_connection(le_adv_obj)

            if gatt_app_registered:
                # Unregister GATT Application
                gatt_manager_iface_obj.UnregisterApplication(gatt_app_obj.get_path())

                # Remove GATT data
                dbus.service.Object.remove_from_connection(gatt_app_obj)

                GLib.timeout_add_seconds(5, self.verify_blecent_disconnect)
                event_loop.run()

                if adv_stop:
                    print("Stop Advertising status: ", adv_stop)
                else:
                    print("Warning: Stop Advertising status: ", adv_stop)

            # Disconnect device
            if self.device:
                print("disconnecting device...")
                self.device.Disconnect(dbus_interface=DEVICE_IFACE)
                if self.adapter:
                    self.adapter.RemoveDevice(self.device)
                self.device = None
                if discovery_start:
                    self.adapter.StopDiscovery()
                    discovery_start = False
            time.sleep(15)

            signal_caught = False
            GLib.timeout_add_seconds(5, verify_signal_is_caught)
            event_loop.run()

            if not device_connected:
                print("device disconnected")
            else:
                print("Warning: device could not be disconnected")

        except Exception:
            print(traceback.format_exc())
            return False
