// Nimbus SRP: Secure Remote Password (SRP-6a) protocol implementation for Java
//
//		Copyright (c) Connect2id Ltd. and others, 2011 - 2019

package com.espressif.provisioning.srp6a;


import java.math.BigInteger;


/**
 * Custom routine interface for computing the client evidence message 'M1'.
 *
 * <p>If you don't want to employ the default 
 * {@link SRP6Routines#computeClientEvidence routine} for computing the client 
 * evidence message you can use this interface to define your own. Remember to 
 * make sure that exactly the same routine is used by both client and server
 * session, else authentication will fail.
 *
 * @author Vladimir Dzhuvinov
 */
public interface ClientEvidenceRoutine {


	/**
	 * Computes a client evidence message 'M1'.
	 *
	 * @param cryptoParams The crypto parameters for the SRP-6a protocol.
	 * @param ctx          Snapshot of the SRP-6a client session variables 
	 *                     which may be used in the computation of the 
	 *                     client evidence message.
	 *
	 * @return The resulting client evidence message 'M1'.
	 */
	public BigInteger computeClientEvidence(final SRP6CryptoParams cryptoParams,
	                                        final SRP6ClientEvidenceContext ctx);

}
