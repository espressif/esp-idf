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
        sys.exit("Error: Only supported on Linux platform")
    print(e)
    print("Install packages `libgirepository1.0-dev gir1.2-gtk-3.0 libcairo2-dev libdbus-1-dev libdbus-glib-1-dev` for resolving the issue")
    print("Run `pip install -r $IDF_PATH/tools/ble/requirements.txt` for resolving the issue")
    raise

import lib_gatt
import lib_gap

srv_added_old_cnt = 0
srv_added_new_cnt = 0
verify_signal_check = 0
blecent_retry_check_cnt = 0
verify_service_cnt = 0
verify_readchars_cnt = 0
blecent_adv_uuid = '1811'
gatt_app_obj_check = False
gatt_app_reg_check = False
adv_data_check = False
adv_reg_check = False
read_req_check = False
write_req_check = False
subscribe_req_check = False
ble_hr_chrc = False

DISCOVERY_START = False
SIGNAL_CAUGHT = False

TEST_CHECKS_PASS = False
ADV_STOP = False

SERVICES_RESOLVED = False
SERVICE_UUID_FOUND = False

BLUEZ_SERVICE_NAME = 'org.bluez'
DBUS_OM_IFACE = 'org.freedesktop.DBus.ObjectManager'
DBUS_PROP_IFACE = 'org.freedesktop.DBus.Properties'

ADAPTER_IFACE = 'org.bluez.Adapter1'
DEVICE_IFACE = 'org.bluez.Device1'

GATT_MANAGER_IFACE = 'org.bluez.GattManager1'
LE_ADVERTISING_MANAGER_IFACE = 'org.bluez.LEAdvertisingManager1'

GATT_SERVICE_IFACE = 'org.bluez.GattService1'
GATT_CHRC_IFACE = 'org.bluez.GattCharacteristic1'

ADAPTER_ON = False
DEVICE_CONNECTED = False
GATT_APP_REGISTERED = False
ADV_REGISTERED = False
ADV_ACTIVE_INSTANCE = False

CHRC_VALUE_CNT = False

# Set up the main loop.
dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
dbus.mainloop.glib.threads_init()
# Set up the event main loop.
event_loop = GLib.MainLoop()


def verify_signal_is_caught():
    global verify_signal_check
    verify_signal_check += 1

    if (not SIGNAL_CAUGHT and verify_signal_check == 15) or (SIGNAL_CAUGHT):
        if event_loop.is_running():
            event_loop.quit()
            return False  # polling for checks will stop

    return True  # polling will continue


def set_props_status(props):
    """
        Set Adapter status if it is powered on or off
    """
    global ADAPTER_ON, SERVICES_RESOLVED, GATT_OBJ_REMOVED, GATT_APP_REGISTERED, \
        ADV_REGISTERED, ADV_ACTIVE_INSTANCE, DEVICE_CONNECTED, CHRC_VALUE, CHRC_VALUE_CNT, \
        SIGNAL_CAUGHT
    is_service_uuid = False
    # Signal caught for change in Adapter Powered property
    if 'Powered' in props:
        if props['Powered'] == 1:
            SIGNAL_CAUGHT = True
            ADAPTER_ON = True
        else:
            SIGNAL_CAUGHT = True
            ADAPTER_ON = False
    if 'ServicesResolved' in props:
        if props['ServicesResolved'] == 1:
            SIGNAL_CAUGHT = True
            SERVICES_RESOLVED = True
        else:
            SIGNAL_CAUGHT = True
            SERVICES_RESOLVED = False
    if 'UUIDs' in props:
        # Signal caught for add/remove GATT data having service uuid
        for uuid in props['UUIDs']:
            if blecent_adv_uuid in uuid:
                is_service_uuid = True
        if not is_service_uuid:
            # Signal caught for removing GATT data having service uuid
            # and for unregistering GATT application
            GATT_APP_REGISTERED = False
            lib_gatt.GATT_APP_OBJ = False
    if 'ActiveInstances' in props:
        # Signal caught for Advertising - add/remove Instances property
        if props['ActiveInstances'] == 1:
            ADV_ACTIVE_INSTANCE = True
        elif props['ActiveInstances'] == 0:
            ADV_ACTIVE_INSTANCE = False
            ADV_REGISTERED = False
            lib_gap.ADV_OBJ = False
    if 'Connected' in props:
        # Signal caught for device connect/disconnect
        if props['Connected'] == 1:
            SIGNAL_CAUGHT = True
            DEVICE_CONNECTED = True
        else:
            SIGNAL_CAUGHT = True
            DEVICE_CONNECTED = False
    if 'Value' in props:
        # Signal caught for change in chars value
        if ble_hr_chrc:
            CHRC_VALUE_CNT += 1
            print(props['Value'])
            if CHRC_VALUE_CNT == 10:
                SIGNAL_CAUGHT = True
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
        global verify_signal_check, SIGNAL_CAUGHT, ADAPTER_ON
        verify_signal_check = 0
        ADAPTER_ON = False
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
            if ADAPTER_ON:
                print("Adapter already powered on")
                return True

            # Power On Adapter
            print("powering on adapter...")
            self.props_iface_obj.connect_to_signal('PropertiesChanged', props_change_handler)
            self.props_iface_obj.Set(ADAPTER_IFACE, "Powered", dbus.Boolean(1))

            SIGNAL_CAUGHT = False
            GLib.timeout_add_seconds(5, verify_signal_is_caught)
            event_loop.run()

            if ADAPTER_ON:
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
        global DISCOVERY_START, SIGNAL_CAUGHT, DEVICE_CONNECTED, verify_signal_check
        device_found = False
        DEVICE_CONNECTED = False
        try:
            self.adapter.StartDiscovery()
            print("\nStarted Discovery")

            DISCOVERY_START = True

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
                        SIGNAL_CAUGHT = False
                        GLib.timeout_add_seconds(5, verify_signal_is_caught)
                        event_loop.run()
                        if DEVICE_CONNECTED:
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
        global SERVICE_UUID_FOUND

        srv_uuid_found = [uuid for uuid in self.srv_uuid if service_uuid in uuid]
        if srv_uuid_found:
            SERVICE_UUID_FOUND = True

    def get_services(self, service_uuid=None):
        '''
        Retrieve Services found in the device connected
        '''
        global SIGNAL_CAUGHT, SERVICE_UUID_FOUND, SERVICES_RESOLVED, verify_signal_check
        verify_signal_check = 0
        SERVICE_UUID_FOUND = False
        SERVICES_RESOLVED = False
        SIGNAL_CAUGHT = False

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
                if not SERVICES_RESOLVED:
                    raise Exception("Services not found...")

            if service_uuid:
                self.verify_service_uuid_found(service_uuid)
                if not SERVICE_UUID_FOUND:
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
        global chrc, chrc_discovered, SIGNAL_CAUGHT
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
                SIGNAL_CAUGHT = True

    def read_chars(self):
        '''
            Read characteristics found in the device connected
        '''
        global iface_added, chrc_discovered, SIGNAL_CAUGHT, verify_signal_check
        verify_signal_check = 0
        chrc_discovered = False
        iface_added = False
        SIGNAL_CAUGHT = False

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
        global ble_hr_chrc, verify_signal_check, SIGNAL_CAUGHT, CHRC_VALUE_CNT

        srv_path = None
        chrc = None
        uuid = None
        chrc_path = None
        chars_ret = None
        ble_hr_chrc = True
        CHRC_VALUE_CNT = 0

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

            SIGNAL_CAUGHT = False
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

    def create_gatt_app(self):
        '''
            Create GATT data
            Register GATT Application
        '''
        global gatt_app_obj, gatt_manager_iface_obj, GATT_APP_REGISTERED

        gatt_app_obj = None
        gatt_manager_iface_obj = None
        GATT_APP_REGISTERED = False
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
        global GATT_APP_REGISTERED
        GATT_APP_REGISTERED = True

    def gatt_app_error_handler(self, error):
        '''
            GATT Application Register error handler
        '''
        global GATT_APP_REGISTERED
        GATT_APP_REGISTERED = False

    def start_advertising(self, adv_host_name, adv_iface_index, adv_type, adv_uuid):
        '''
            Create Advertising data
            Register Advertisement
            Start Advertising
        '''
        global le_adv_obj, le_adv_manager_iface_obj, ADV_ACTIVE_INSTANCE, ADV_REGISTERED

        le_adv_obj = None
        le_adv_manager_iface_obj = None
        le_adv_iface_path = None
        ADV_ACTIVE_INSTANCE = False
        ADV_REGISTERED = False
        lib_gap.ADV_OBJ = False

        try:
            print("Advertising started")
            gatt_app_ret = self.create_gatt_app()

            if not gatt_app_ret:
                raise Exception

            for path,interface in self.ble_objs.items():
                if LE_ADVERTISING_MANAGER_IFACE in interface:
                    le_adv_iface_path = path

            if le_adv_iface_path is None:
                raise Exception('\n Cannot start advertising. LEAdvertisingManager1 Interface not found')

            le_adv_obj = lib_gap.Advertisement(self.bus, adv_iface_index, adv_type, adv_uuid, adv_host_name)
            le_adv_manager_iface_obj = dbus.Interface(self.bus.get_object(BLUEZ_SERVICE_NAME, le_adv_iface_path), LE_ADVERTISING_MANAGER_IFACE)

            le_adv_manager_iface_obj.RegisterAdvertisement(le_adv_obj.get_path(), {},
                                                           reply_handler=self.adv_handler,
                                                           error_handler=self.adv_error_handler)

            GLib.timeout_add_seconds(2, self.verify_blecent)
            event_loop.run()

            if TEST_CHECKS_PASS:
                return True
            else:
                raise Exception

        except Exception:
            print(traceback.format_exc())
            return False

    def adv_handler(self):
        '''
            Advertisement Register success handler
        '''
        global ADV_REGISTERED
        ADV_REGISTERED = True

    def adv_error_handler(self, error):
        '''
            Advertisement Register error handler
        '''
        global ADV_REGISTERED
        ADV_REGISTERED = False

    def verify_blecent(self):
        """
            Verify blecent test commands are successful
        """
        global blecent_retry_check_cnt, gatt_app_obj_check, gatt_app_reg_check,\
            adv_data_check, adv_reg_check, read_req_check, write_req_check,\
            subscribe_req_check, TEST_CHECKS_PASS

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

        # Check for failures after 10 retries
        if blecent_retry_check_cnt == 10:
            # check for failures
            if not gatt_app_obj_check:
                print("Failure: GATT Data could not be created")
            if not gatt_app_reg_check:
                print("Failure: GATT Application could not be registered")
            if not adv_data_check:
                print("Failure: Advertising data could not be created")
            if not adv_reg_check:
                print("Failure: Advertisement could not be registered")
            if not read_req_check:
                print("Failure: Read Request not received")
            if not write_req_check:
                print("Failure: Write Request not received")
            if not subscribe_req_check:
                print("Failure: Subscribe Request not received")

            # Blecent Test failed
            TEST_CHECKS_PASS = False
            if subscribe_req_check:
                lib_gatt.alert_status_char_obj.StopNotify()
        else:
            # Check for success
            if not gatt_app_obj_check and lib_gatt.GATT_APP_OBJ:
                print("GATT Data created")
                gatt_app_obj_check = True
            if not gatt_app_reg_check and GATT_APP_REGISTERED:
                print("GATT Application registered")
                gatt_app_reg_check = True
            if not adv_data_check and lib_gap.ADV_OBJ:
                print("Advertising data created")
                adv_data_check = True
            if not adv_reg_check and ADV_REGISTERED and ADV_ACTIVE_INSTANCE:
                print("Advertisement registered")
                adv_reg_check = True
            if not read_req_check and lib_gatt.CHAR_READ:
                read_req_check = True
            if not write_req_check and lib_gatt.CHAR_WRITE:
                write_req_check = True
            if not subscribe_req_check and lib_gatt.CHAR_SUBSCRIBE:
                subscribe_req_check = True

        if read_req_check and write_req_check and subscribe_req_check:
            # all checks passed
            # Blecent Test passed
            TEST_CHECKS_PASS = True
            lib_gatt.alert_status_char_obj.StopNotify()

        if (blecent_retry_check_cnt == 10 or TEST_CHECKS_PASS):
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
            adv_data_check, adv_reg_check, ADV_STOP

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
            ADV_STOP = False
        else:
            # Check for success
            if not gatt_app_obj_check and not lib_gatt.GATT_APP_OBJ:
                print("GATT Data removed")
                gatt_app_obj_check = True
            if not gatt_app_reg_check and not GATT_APP_REGISTERED:
                print("GATT Application unregistered")
                gatt_app_reg_check = True
            if not adv_data_check and not adv_reg_check and not (ADV_REGISTERED or ADV_ACTIVE_INSTANCE or lib_gap.ADV_OBJ):
                print("Advertising data removed")
                print("Advertisement unregistered")
                adv_data_check = True
                adv_reg_check = True
                # all checks passed
                ADV_STOP = True

        if (blecent_retry_check_cnt == 10 or ADV_STOP):
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
            global blecent_retry_check_cnt, DISCOVERY_START, verify_signal_check, SIGNAL_CAUGHT
            blecent_retry_check_cnt = 0
            verify_signal_check = 0

            print("\nexiting from test...")

            self.props_iface_obj.connect_to_signal('PropertiesChanged', props_change_handler)

            if ADV_REGISTERED:
                # Unregister Advertisement
                le_adv_manager_iface_obj.UnregisterAdvertisement(le_adv_obj.get_path())

                # Remove Advertising data
                dbus.service.Object.remove_from_connection(le_adv_obj)

            if GATT_APP_REGISTERED:
                # Unregister GATT Application
                gatt_manager_iface_obj.UnregisterApplication(gatt_app_obj.get_path())

                # Remove GATT data
                dbus.service.Object.remove_from_connection(gatt_app_obj)

                GLib.timeout_add_seconds(5, self.verify_blecent_disconnect)
                event_loop.run()

                if ADV_STOP:
                    print("Stop Advertising status: ", ADV_STOP)
                else:
                    print("Warning: Stop Advertising status: ", ADV_STOP)

            # Disconnect device
            if self.device:
                print("disconnecting device...")
                self.device.Disconnect(dbus_interface=DEVICE_IFACE)
                if self.adapter:
                    self.adapter.RemoveDevice(self.device)
                self.device = None
                if DISCOVERY_START:
                    self.adapter.StopDiscovery()
                    DISCOVERY_START = False
            time.sleep(15)

            SIGNAL_CAUGHT = False
            GLib.timeout_add_seconds(5, verify_signal_is_caught)
            event_loop.run()

            if not DEVICE_CONNECTED:
                print("device disconnected")
            else:
                print("Warning: device could not be disconnected")

        except Exception:
            print(traceback.format_exc())
            return False
