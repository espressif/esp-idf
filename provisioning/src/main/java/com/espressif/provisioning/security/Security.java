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


package com.espressif.provisioning.security;

/**
 * Security interface which abstracts
 * the handshake and crypto behavior supported by a specific
 * class / family of devices
 */
public interface Security {
    /**
     * Get the next request based upon the current Session
     * state and the response data passed to this function
     * @param responseData
     * @return
     */
    byte[] getNextRequestInSession(byte[] responseData);

    /**
     * Encrypt the data according to the Security implementation
     * @param data
     * @return
     */
    byte[] encrypt(byte[] data);

    /**
     * Decrypt data according to the Security implementation
     * @param data
     * @return
     */
    byte[] decrypt(byte[] data);
}
