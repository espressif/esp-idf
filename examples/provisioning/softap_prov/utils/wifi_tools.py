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

import dbus
import dbus.mainloop.glib
import netifaces
import time

def get_wiface_name():
    for iface in netifaces.interfaces():
        if iface.startswith('w'):
            return iface
    return None

def get_wiface_IPv4(iface):
    try:
        [info] = netifaces.ifaddresses(iface)[netifaces.AF_INET]
        return info['addr']
    except KeyError:
        return None

class wpa_cli:
    def __init__(self, iface, reset_on_exit = False):
        self.iface_name = iface
        self.iface_obj = None
        self.iface_ifc = None
        self.old_network = None
        self.new_network = None
        self.connected = False
        self.reset_on_exit = reset_on_exit
        dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
        bus = dbus.SystemBus()

        service = dbus.Interface(bus.get_object("fi.w1.wpa_supplicant1", "/fi/w1/wpa_supplicant1"), "fi.w1.wpa_supplicant1")
        paths = service.Get("fi.w1.wpa_supplicant1", "Interfaces", dbus_interface='org.freedesktop.DBus.Properties')
        iface_path = service.GetInterface(self.iface_name)
        self.iface_obj = bus.get_object("fi.w1.wpa_supplicant1", iface_path)
        self.iface_ifc = dbus.Interface(self.iface_obj, "fi.w1.wpa_supplicant1.Interface")
        if self.iface_ifc == None:
            raise RuntimeError('supplicant : Failed to fetch interface')

        self.old_network = self.iface_obj.Get("fi.w1.wpa_supplicant1.Interface", "CurrentNetwork", dbus_interface='org.freedesktop.DBus.Properties')
        if self.old_network == '/':
            self.old_network = None
        else:
            self.connected = True

    def connect(self, ssid, password):
        if self.connected == True:
            self.iface_ifc.Disconnect()
            self.connected = False

        if self.new_network != None:
            self.iface_ifc.RemoveNetwork(self.new_network)

        self.new_network = self.iface_ifc.AddNetwork({"ssid": ssid, "psk": password})
        self.iface_ifc.SelectNetwork(self.new_network)

        ip = None
        retry = 10
        while retry > 0:
            time.sleep(5)
            ip = get_wiface_IPv4(self.iface_name)
            if ip != None:
                self.connected = True
                return ip
            retry -= 1

        self.reset()
        raise RuntimeError('wpa_cli : Connection failed')

    def reset(self):
        if self.iface_ifc != None:
            if self.connected == True:
                self.iface_ifc.Disconnect()
                self.connected = False
            if self.new_network != None:
                self.iface_ifc.RemoveNetwork(self.new_network)
                self.new_network = None
            if self.old_network != None:
                self.iface_ifc.SelectNetwork(self.old_network)
                self.old_network = None

    def __del__(self):
        if self.reset_on_exit == True:
            self.reset()
