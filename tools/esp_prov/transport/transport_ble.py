# SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

from . import ble_cli
from .transport import Transport


class Transport_BLE(Transport):
    def __init__(self, service_uuid, nu_lookup):
        self.nu_lookup = nu_lookup
        self.service_uuid = service_uuid
        self.name_uuid_lookup = None
        # Expect service UUID like '0000ffff-0000-1000-8000-00805f9b34fb'
        for name in nu_lookup.keys():
            # Calculate characteristic UUID for each endpoint
            nu_lookup[name] = service_uuid[:4] + '{:02x}'.format(
                int(nu_lookup[name], 16) & int(service_uuid[4:8], 16)) + service_uuid[8:]

        # Get BLE client module
        self.cli = ble_cli.get_client()

    async def connect(self, devname):
        # Use client to connect to BLE device and bind to service
        if not await self.cli.connect(devname=devname, iface='hci0',
                                      chrc_names=self.nu_lookup.keys(),
                                      fallback_srv_uuid=self.service_uuid):
            raise RuntimeError('Failed to initialize transport')

        # Irrespective of provided parameters, let the client
        # generate a lookup table by reading advertisement data
        # and characteristic user descriptors
        self.name_uuid_lookup = self.cli.get_nu_lookup()

        # If that doesn't work, use the lookup table provided as parameter
        if self.name_uuid_lookup is None:
            self.name_uuid_lookup = self.nu_lookup
            # Check if expected characteristics are provided by the service
            for name in self.name_uuid_lookup.keys():
                if not self.cli.has_characteristic(self.name_uuid_lookup[name]):
                    raise RuntimeError(f"'{name}' endpoint not found")

    async def disconnect(self):
        await self.cli.disconnect()

    async def send_data(self, ep_name, data):
        # Write (and read) data to characteristic corresponding to the endpoint
        if ep_name not in self.name_uuid_lookup.keys():
            raise RuntimeError(f'Invalid endpoint: {ep_name}')
        return await self.cli.send_data(self.name_uuid_lookup[ep_name], data)
