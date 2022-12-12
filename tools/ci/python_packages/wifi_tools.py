# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import time

import dbus
import dbus.mainloop.glib
import netifaces


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
    def __init__(self, iface, reset_on_exit=False):
        self.iface_name = iface
        self.iface_obj = None
        self.iface_ifc = None
        self.old_network = None
        self.new_network = None
        self.connected = False
        self.reset_on_exit = reset_on_exit

        try:
            dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
            bus = dbus.SystemBus()

            service = dbus.Interface(bus.get_object('fi.w1.wpa_supplicant1', '/fi/w1/wpa_supplicant1'),
                                     'fi.w1.wpa_supplicant1')
            iface_path = service.GetInterface(self.iface_name)
            self.iface_obj = bus.get_object('fi.w1.wpa_supplicant1', iface_path)
            self.iface_ifc = dbus.Interface(self.iface_obj, 'fi.w1.wpa_supplicant1.Interface')
            self.iface_props = dbus.Interface(self.iface_obj, 'org.freedesktop.DBus.Properties')
            if self.iface_ifc is None:
                raise RuntimeError('supplicant : Failed to fetch interface')

            self.old_network = self._get_iface_property('CurrentNetwork')
            print('Old network is %s' % self.old_network)

            if self.old_network == '/':
                self.old_network = None
            else:
                self.connected = True

        except Exception as err:
            raise Exception('Failure in wpa_cli init: {}'.format(err))

    def _get_iface_property(self, name):
        """ Read the property with 'name' from the wi-fi interface object

        Note: The result is a dbus wrapped type, so should usually convert it to the corresponding native
        Python type
        """
        return self.iface_props.Get('fi.w1.wpa_supplicant1.Interface', name)

    def connect(self, ssid, password):
        try:
            if self.connected is True:
                self.iface_ifc.Disconnect()
                self.connected = False

            if self.new_network is not None:
                self.iface_ifc.RemoveNetwork(self.new_network)

            print('Pre-connect state is %s, IP is %s' % (self._get_iface_property('State'), get_wiface_IPv4(self.iface_name)))

            self.new_network = self.iface_ifc.AddNetwork({'ssid': ssid, 'psk': password})
            self.iface_ifc.SelectNetwork(self.new_network)
            time.sleep(10)

            ip = None
            retry = 10
            while retry > 0:
                time.sleep(5)
                state = str(self._get_iface_property('State'))
                print('wpa iface state %s (scanning %s)' % (state, bool(self._get_iface_property('Scanning'))))
                if state in ['disconnected', 'inactive']:
                    self.iface_ifc.Reconnect()
                ip = get_wiface_IPv4(self.iface_name)
                print('wpa iface %s IP %s' % (self.iface_name, ip))
                if ip is not None:
                    self.connected = True
                    return ip
                retry -= 1
                time.sleep(3)

            self.reset()
            print('wpa_cli : Connection failed')

        except Exception as err:
            raise Exception('Failure in wpa_cli init: {}'.format(err))

    def reset(self):
        if self.iface_ifc is not None:
            if self.connected is True:
                self.iface_ifc.Disconnect()
                self.connected = False
            if self.new_network is not None:
                self.iface_ifc.RemoveNetwork(self.new_network)
                self.new_network = None
            if self.old_network is not None:
                self.iface_ifc.SelectNetwork(self.old_network)
                self.old_network = None

    def __del__(self):
        if self.reset_on_exit is True:
            self.reset()
