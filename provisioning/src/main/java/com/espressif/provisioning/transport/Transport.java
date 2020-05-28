// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


package com.espressif.provisioning.transport;

import com.espressif.provisioning.listeners.ResponseListener;

/**
 * Transport interface which abstracts the
 * transport layer to send messages to the Device.
 */
public interface Transport {

    /***
     * Send data relating to device configurations
     * @param path path of the config endpoint.
     * @param data config data to be sent
     * @param listener listener implementation which receives events when response is received.
     */
    void sendConfigData(String path, byte[] data, ResponseListener listener);
}