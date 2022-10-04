// Nimbus SRP: Secure Remote Password (SRP-6a) protocol implementation for Java
//
//		Copyright (c) Connect2id Ltd. and others, 2011 - 2019

package com.espressif.provisioning.srp6a;


import java.math.BigInteger;


/**
 * Custom routine interface for computing 'u' as 'H(A | B)'.
 * 
 * <p>
 * If you don't want to employ the default {@link SRP6Routines#computeU routine}
 * for computing 'H(A | B)' message you can use this interface to define your
 * own. Remember to make sure that exactly the same routine is used by both
 * client and server session, else authentication will fail.
 * 
 * @author Simon Massey
 */
public interface URoutine {


	/**
	 * Computes 'u' as 'H(A | B)'.
	 * 
	 * @param cryptoParams The crypto parameters for the SRP-6a protocol.
	 * @param ctx          Snapshot of the SRP-6a client session variables
	 *                     which may be used in the computation of the
	 *                     hashed keys message.
	 * 
	 * @return The resulting 'u' as 'H(A | B)'.
	 */
	public BigInteger computeU(final SRP6CryptoParams cryptoParams, final URoutineContext ctx);
}
