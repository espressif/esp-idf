# SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import platform

from utils import hex_str_to_bytes, str_to_bytes

fallback = True


# Check if required packages are installed
# else fallback to console mode
try:
    import bleak
    fallback = False
except ImportError:
    pass


# --------------------------------------------------------------------

def device_sort(device):
    return device[0].address


class BLE_Bleak_Client:
    def __init__(self):
        self.adapter = None
        self.adapter_props = None
        self.characteristics = dict()
        self.chrc_names = None
        self.device = None
        self.devname = None
        self.iface = None
        self.nu_lookup = None
        self.services = None
        self.srv_uuid_adv = None
        self.srv_uuid_fallback = None

    async def connect(self, devname, iface, chrc_names, fallback_srv_uuid):
        self.devname = devname
        self.srv_uuid_fallback = fallback_srv_uuid
        self.chrc_names = [name.lower() for name in chrc_names]
        self.iface = iface

        print('Discovering...')
        try:
            discovery = await bleak.BleakScanner.discover(return_adv=True)
            devices = list(discovery.values())
        except bleak.exc.BleakDBusError as e:
            if str(e) == '[org.bluez.Error.NotReady] Resource Not Ready':
                raise RuntimeError('Bluetooth is not ready. Maybe try `bluetoothctl power on`?')
            raise

        found_device = None

        if self.devname is None:
            if len(devices) == 0:
                print('No devices found!')
                exit(1)

            while True:
                devices.sort(key=device_sort)
                print('==== BLE Discovery results ====')
                print('{0: >4} {1: <33} {2: <12}'.format(
                    'S.N.', 'Name', 'Address'))
                for i, _ in enumerate(devices):
                    print('[{0: >2}] {1: <33} {2: <12}'.format(i + 1, devices[i][0].name or 'Unknown', devices[i][0].address))

                while True:
                    try:
                        select = int(input('Select device by number (0 to rescan) : '))
                        if select < 0 or select > len(devices):
                            raise ValueError
                        break
                    except ValueError:
                        print('Invalid input! Retry')

                if select != 0:
                    break

                discovery = await bleak.BleakScanner.discover(return_adv=True)
                devices = list(discovery.values())

            self.devname = devices[select - 1][0].name
            found_device = devices[select - 1]
        else:
            for d in devices:
                if d[0].name == self.devname:
                    found_device = d

        if not found_device:
            raise RuntimeError('Device not found')

        uuids = found_device[1].service_uuids
        # There should be 1 service UUID in advertising data
        # If bluez had cached an old version of the advertisement data
        # the list of uuids may be incorrect, in which case connection
        # or service discovery may fail the first time. If that happens
        # the cache will be refreshed before next retry
        if len(uuids) == 1:
            self.srv_uuid_adv = uuids[0]

        print('Connecting...')
        self.device = bleak.BleakClient(found_device[0].address)
        await self.device.connect()
        # must be paired on Windows to access characteristics;
        # cannot be paired on Mac
        if platform.system() == 'Windows':
            await self.device.pair()

        print('Getting Services...')
        services = self.device.services

        service = services[self.srv_uuid_adv] or services[self.srv_uuid_fallback]
        if not service:
            await self.device.disconnect()
            self.device = None
            raise RuntimeError('Provisioning service not found')

        nu_lookup = dict()
        for characteristic in service.characteristics:
            for descriptor in characteristic.descriptors:
                if descriptor.uuid[4:8] != '2901':
                    continue
                readval = await self.device.read_gatt_descriptor(descriptor.handle)
                found_name = ''.join(chr(b) for b in readval).lower()
                nu_lookup[found_name] = characteristic.uuid
                self.characteristics[characteristic.uuid] = characteristic

        match_found = True
        for name in self.chrc_names:
            if name not in nu_lookup:
                # Endpoint name not present
                match_found = False
                break

        # Create lookup table only if all endpoint names found
        self.nu_lookup = [None, nu_lookup][match_found]

        return True

    def get_nu_lookup(self):
        return self.nu_lookup

    def has_characteristic(self, uuid):
        print('checking for characteristic ' + uuid)
        if uuid in self.characteristics:
            return True
        return False

    async def disconnect(self):
        if self.device:
            print('Disconnecting...')
            if platform.system() == 'Windows':
                await self.device.unpair()
            await self.device.disconnect()
            self.device = None
            self.nu_lookup = None
            self.characteristics = dict()

    async def send_data(self, characteristic_uuid, data):
        await self.device.write_gatt_char(characteristic_uuid, bytearray(data.encode('latin-1')), True)
        readval = await self.device.read_gatt_char(characteristic_uuid)
        return ''.join(chr(b) for b in readval)
# --------------------------------------------------------------------


# Console based BLE client for Cross Platform support
class BLE_Console_Client:
    async def connect(self, devname, iface, chrc_names, fallback_srv_uuid):
        print('BLE client is running in console mode')
        print('\tThis could be due to your platform not being supported or dependencies not being met')
        print('\tPlease ensure all pre-requisites are met to run the full fledged client')
        print('BLECLI >> Please connect to BLE device `' + devname + '` manually using your tool of choice')
        resp = input('BLECLI >> Was the device connected successfully? [y/n] ')
        if resp != 'Y' and resp != 'y':
            return False
        print('BLECLI >> List available attributes of the connected device')
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

    async def disconnect(self):
        pass

    async def send_data(self, characteristic_uuid, data):
        print("BLECLI >> Write following data to characteristic with UUID '" + characteristic_uuid + "' :")
        print('\t>> ' + str_to_bytes(data).hex())
        print('BLECLI >> Enter data read from characteristic (in hex) :')
        resp = input('\t<< ')
        return hex_str_to_bytes(resp)


# --------------------------------------------------------------------


# Function to get client instance depending upon platform
def get_client():
    if fallback:
        return BLE_Console_Client()
    return BLE_Bleak_Client()
