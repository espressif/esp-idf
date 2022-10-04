// Nimbus SRP: Secure Remote Password (SRP-6a) protocol implementation for Java
//
//		Copyright (c) Connect2id Ltd. and others, 2011 - 2019

package com.espressif.provisioning.srp6a;


import java.math.BigInteger;


/**
 * Custom routine interface for computing the server evidence message 'M2'.
 *
 * <p>If you don't want to employ the {@link SRP6Routines#computeServerEvidence 
 * default routine} for computing the server evidence message you can use this 
 * interface to define your own. Remember to make sure that exactly the same 
 * routine is used by both client and server session, else authentication will 
 * fail.
 *
 * @author Vladimir Dzhuvinov
 */
public interface ServerEvidenceRoutine {


	/**
	 * Computes a server evidence message 'M2'.
	 *
	 * @param cryptoParams The crypto parameters for the SRP-6a protocol.
	 * @param ctx          Snapshot of the SRP-6a server session variables 
	 *                     which may be used in the computation of the 
	 *                     server evidence message.
	 *
	 * @return The resulting server evidence message 'M1'.
	 */
	public BigInteger computeServerEvidence(final SRP6CryptoParams cryptoParams,
	                                        final SRP6ServerEvidenceContext ctx);
}
