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

# Register Advertisement

from __future__ import print_function

import sys

try:
    import dbus
    import dbus.service
except ImportError as e:
    if 'linux' not in sys.platform:
        raise e
    print(e)
    print('Install packages `libgirepository1.0-dev gir1.2-gtk-3.0 libcairo2-dev libdbus-1-dev libdbus-glib-1-dev` for resolving the issue')
    print('Run `pip install -r $IDF_PATH/tools/ble/requirements.txt` for resolving the issue')
    raise


DBUS_PROP_IFACE = 'org.freedesktop.DBus.Properties'
LE_ADVERTISEMENT_IFACE = 'org.bluez.LEAdvertisement1'


class InvalidArgsException(dbus.exceptions.DBusException):
    _dbus_error_name = 'org.freedesktop.DBus.Error.InvalidArgs'


class Advertisement(dbus.service.Object):
    PATH_BASE = '/org/bluez/hci0/advertisement'

    def __init__(self, bus, index, advertising_type, uuid, name):
        self.path = self.PATH_BASE + str(index)
        self.bus = bus
        self.ad_type = advertising_type
        self.service_uuids = [uuid]
        self.local_name = dbus.String(name)
        dbus.service.Object.__init__(self, bus, self.path)

    def __del__(self):
        pass

    def get_properties(self):
        properties = dict()
        properties['Type'] = self.ad_type

        if self.service_uuids is not None:
            properties['ServiceUUIDs'] = dbus.Array(self.service_uuids,
                                                    signature='s')
        if self.local_name is not None:
            properties['LocalName'] = dbus.String(self.local_name)

        return {LE_ADVERTISEMENT_IFACE: properties}

    def get_path(self):
        return dbus.ObjectPath(self.path)

    @dbus.service.method(DBUS_PROP_IFACE,
                         in_signature='s',
                         out_signature='a{sv}')
    def GetAll(self, interface):
        if interface != LE_ADVERTISEMENT_IFACE:
            raise InvalidArgsException()
        return self.get_properties()[LE_ADVERTISEMENT_IFACE]

    @dbus.service.method(LE_ADVERTISEMENT_IFACE,
                         in_signature='',
                         out_signature='')
    def Release(self):
        pass
