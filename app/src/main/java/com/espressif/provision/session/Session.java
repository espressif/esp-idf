// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
package com.espressif.provision.session;

import com.espressif.provision.security.Security;
import com.espressif.provision.transport.ResponseListener;
import com.espressif.provision.transport.Transport;

/**
 * Session object encapsulates the Transport and Security
 * protocol implementations and is responsible for performing
 * initial handshake with the device to establish a secure session.
 */
public class Session {
    private static final String TAG = "Espressif::" + Session.class.getSimpleName();
    public SessionListener sessionListener;
    private Transport transport;
    private Security security;
    private boolean isSessionEstablished;

    /**
     * Initialize Session object with Transport and Security interface implementations
     *
     * @param transport
     * @param security
     */
    public Session(Transport transport,
                   Security security) {
        this.transport = transport;
        this.security = security;
    }

    /**
     * Get the Security implementation object
     *
     * @return
     */
    public Security getSecurity() {
        return security;
    }

    /**
     * Get the Transport implementation object.
     *
     * @return
     */
    public Transport getTransport() {
        return transport;
    }

    /**
     * Get whether a secure Session has been established.
     *
     * @return
     */
    public boolean isEstablished() {
        return isSessionEstablished;
    }

    /**
     * Establish the session by performing handshake with the device
     * based on the Security implementation.
     * Communication with the device will happen over the Transport interface.
     *
     * @throws RuntimeException
     */
    public void init(byte[] response) throws RuntimeException {
        try {
            byte[] request = security.getNextRequestInSession(response);
            if (request == null) {
                isSessionEstablished = true;
                if (sessionListener != null) {
                    sessionListener.OnSessionEstablished();
                }
            } else {
                transport.sendSessionData(request, new ResponseListener() {
                    @Override
                    public void onSuccess(byte[] returnData) {
                        if (returnData == null) {
                            if (sessionListener != null) {
                                sessionListener.OnSessionEstablishFailed(new Exception("Session could not be established"));
                            }
                        } else {
                            init(returnData);
                        }
                    }

                    @Override
                    public void onFailure(Exception e) {
                        if (sessionListener != null) {
                            sessionListener.OnSessionEstablishFailed(e);
                        }
                    }
                });
            }
        } catch (RuntimeException e) {
            if (response == null && sessionListener != null) {
                sessionListener.OnSessionEstablishFailed(new RuntimeException("Session could not be established"));
            }
        }
    }

    /**
     * Callback interface for listening to Session
     * establish events.
     */
    public interface SessionListener {

        /**
         * Called when session is established.
         * Further communication with the device can only
         * occur after this callback is called.
         */
        void OnSessionEstablished();

        /**
         * Called when session establish fails.
         *
         * @param e
         */
        void OnSessionEstablishFailed(Exception e);
    }
}
