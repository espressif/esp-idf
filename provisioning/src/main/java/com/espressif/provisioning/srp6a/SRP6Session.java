// Nimbus SRP: Secure Remote Password (SRP-6a) protocol implementation for Java
//
//		Copyright (c) Connect2id Ltd. and others, 2011 - 2019

package com.espressif.provisioning.srp6a;


import java.io.Serializable;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.SecureRandom;
import java.util.HashMap;
import java.util.Map;


/**
 * The base abstract class for client and server-side Secure Remote Password 
 * (SRP-6a) authentication sessions.
 *
 * @author Vladimir Dzhuvinov
 * @author John Kim
 * @author Bernard Wittwer
 * @author Simon Massey
 */
public abstract class SRP6Session implements Serializable {

	protected final SRP6Routines srp6Routines;

	/**
	 * Serializable class version number
	 */
	private static final long serialVersionUID = 3813344182070859518L;


	/**
	 * The crypto configuration.
	 */
	protected SRP6CryptoParams config;


	/**
	 * Source of randomness.
	 */
	protected SecureRandom random = new SecureRandom();

	/**
	 * The SRP-6a authentication session timeout in seconds. If the 
	 * authenticating counterparty (server or client) fails to respond 
	 * within the specified time the session will be closed. Zero implies
	 * no timeout.
	 */
	protected final int timeout;
	
	
	/**
	 * The last activity timestamp, from System.currentTimeMillis().
	 */
	protected long lastActivity;
	
	
	/**
	 * The identity 'I' of the authenticating user.
	 */
	protected String userID = null;
	
	
	/**
	 * The password salt 's'.
	 */
	protected BigInteger s = null;
	
	
	/**
	 * The client public value 'A'.
	 */
	protected BigInteger A = null;
	
	
	/**
	 * The server public value 'B'.
	 */
	protected BigInteger B = null;
	
	
	/**
	 * The random scrambling parameter 'u'.
	 */
	protected BigInteger u = null;
	
	
	/**
	 * The multiplier 'k'.
	 */
	protected BigInteger k = null;
	
	
	/**
	 * The shared session key 'S'.
	 */
	protected BigInteger S = null;

	/**
	 * The shared session key 'K' for "Espressif" devices.
	 */
	public BigInteger K = null;
	
	
	/**
	 * The client evidence message 'M1'.
	 */
	protected BigInteger M1 = null;
	
	
	/**
	 * The server evidence message 'M2'.
	 */
	protected BigInteger M2 = null;
	
	
	/**
	 * Custom routine for the client evidence message 'M1' computation.
	 */
	protected ClientEvidenceRoutine clientEvidenceRoutine = null;
	
	
	/**
	 * Custom routine for the server evidence message 'M2' computation.
	 */
	protected ServerEvidenceRoutine serverEvidenceRoutine = null;


	/**
	 * Custom routine for the hashed keys 'u' computation.
	 */
	protected URoutine hashedKeysRoutine = null;


	/**
	 * Optional storage of arbitrary session attributes.
	 */
	private Map<String,Object> attributes = null;
	
	
	/**
	 * Creates a new SRP-6a authentication session.
	 *
	 * @param timeout The SRP-6a authentication session timeout in seconds. 
	 *                If the authenticating counterparty (server or client) 
	 *                fails to respond within the specified time the
	 *                session will be closed. If zero timeouts are
	 *                disabled.
	 * @param srp6Routines The math routines to use.
	 */
	public SRP6Session(final int timeout, final SRP6Routines srp6Routines) {
	
		if (timeout < 0)
			throw new IllegalArgumentException("The timeout must be zero (no timeout) or greater");
		
		this.timeout = timeout;
		this.srp6Routines = srp6Routines;
	}

	/**
	 * Creates a new SRP-6a authentication session.
	 *
	 * @param timeout The SRP-6a authentication session timeout in seconds.
	 *                If the authenticating counterparty (server or client)
	 *                fails to respond within the specified time the
	 *                session will be closed. If zero timeouts are
	 *                disabled.
	 */
	public SRP6Session(final int timeout) {
		this(timeout, new SRP6Routines());
	}
	
	/**
	 * Creates a new SRP-6a authentication session, session timeouts are 
	 * disabled. The default math routines are used.
	 */
	public SRP6Session() {
		this(0, new SRP6Routines());
	}

	/**
	 * Updates the last activity timestamp.
	 */
	protected void updateLastActivityTime() {
		lastActivity = System.currentTimeMillis();
	}

	/**
	 * Gets the last session activity timestamp, in milliseconds since 
	 * midnight, January 1, 1970 UTC (see System.currentTimeMillis()).
	 *
	 * @return The last activity timestamp.
	 */
	public long getLastActivityTime() {
	
		return lastActivity;
	}
	
	
	/**
	 * Returns {@code true} if the session has timed out, based on the 
	 * timeout configuration and the last activity timestamp.
	 *
	 * @return {@code true} if the session has timed out, else 
	 *         {@code false}.
	 */
	public boolean hasTimedOut() {
	
		if (timeout == 0)
			return false;
	
		final long now = System.currentTimeMillis();

		return now > lastActivity + (timeout * 1000);
	}
	
	
	/**
	 * Gets the SRP-6a crypto parameters for this session.
	 *
	 * @return The SRP-6a crypto parameters, {@code null} if undefined.
	 */
	public SRP6CryptoParams getCryptoParams() {
	
		return config;
	}
	
	
	/**
	 * Gets the identity 'I' of the authenticating user.
	 *
	 * @return The user identity 'I', {@code null} if undefined.
	 */
	public String getUserID() {
	
		return userID;
	}
	
	
	/**
	 * Gets the SRP-6a authentication session timeout.
	 *
	 * @return The SRP-6a authentication session timeout, in seconds. Zero
	 *         implies to timeout.
	 */
	public int getTimeout() {
	
		return timeout;
	}
	
	
	/**
	 * Sets a custom routine to compute the client evidence message 'M1'.
	 * Note that the custom routine must be set prior to 
	 * {@link SRP6ClientSession.State#STEP_2} or
	 * {@link SRP6ServerSession.State#STEP_2}.
	 *
	 * @param routine The client evidence message 'M1' routine or 
	 *                {@code null} to use the default 
	 *                {@link SRP6Routines#computeClientEvidence}.
	 */
	public void setClientEvidenceRoutine(final ClientEvidenceRoutine routine) {
	
		clientEvidenceRoutine = routine;
	}
	
	
	/**
	 * Gets the custom routine to compute the client evidence message 'M1'.
	 *
	 * @return The routine instance or {@code null} if the default 
	 *         {@link SRP6Routines#computeClientEvidence} is used.
	 */
	public ClientEvidenceRoutine getClientEvidenceRoutine() {
	
		return clientEvidenceRoutine;
	}
	
	
	/**
	 * Sets a custom routine to compute the server evidence message 'M2'.
	 * Note that the custom routine must be set prior to 
	 * {@link SRP6ClientSession.State#STEP_3} or
	 * {@link SRP6ServerSession.State#STEP_2}.
	 *
	 * @param routine The server evidence message 'M2' routine or 
	 *                {@code null} to use the default 
	 *                {@link SRP6Routines#computeServerEvidence}.
	 */
	public void setServerEvidenceRoutine(final ServerEvidenceRoutine routine) {
	
		serverEvidenceRoutine = routine;
	}
	
	
	/**
	 * Gets the custom routine to compute the server evidence message 'M2'.
	 *
	 * @return The routine instance or {@code null} if the default 
	 *         {@link SRP6Routines#computeServerEvidence} is used.
	 */
	public ServerEvidenceRoutine getServerEvidenceRoutine() {
	
		return serverEvidenceRoutine;
	}


	/**
	 * Gets the custom routine to compute hashed keys 'u' a 'H(A | B)'.
	 * 
	 * @return The routine instance or {@code null} if the default
	 *         {@link SRP6Routines#computeU} is to be used.
	 */
	public URoutine getHashedKeysRoutine() {

		return hashedKeysRoutine;
	}


	/**
	 * Sets a custom routine to compute hashed keys 'u' a 'H(A | B)'. Note
	 * that the custom routine must be set prior to
	 * {@link SRP6ServerSession.State#STEP_2}.
	 * 
	 * @param hashedKeysRoutine The hashed keys 'u' routine or {@code null}
	 *                          to use the default
	 *                          {@link SRP6Routines#computeU}.
	 */
	public void setHashedKeysRoutine(final URoutine hashedKeysRoutine) {

		this.hashedKeysRoutine = hashedKeysRoutine;
	}


	/**
	 * Gets the password salt 's'.
	 * 
	 * @return The salt 's' if available, else {@code null}.
	 */
	public BigInteger getSalt() {
	
		return s;
	}
	
	
	/**
	 * Gets the public client value 'A'.
	 *
	 * @return The public client value 'A' if available, else {@code null}.
	 */
	public BigInteger getPublicClientValue() {
	
		return A;
	}
	
	
	/**
	 * Gets the public server value 'B'.
	 *
	 * @return The public server value 'B' if available, else {@code null}.
	 */
	public BigInteger getPublicServerValue() {
	
		return B;
	}
	
	
	/**
	 * Gets the client evidence message 'M1'.
	 *
	 * @return The client evidence message 'M1' if available, else
	 *         {@code null}.
	 */
	public BigInteger getClientEvidenceMessage() {
	
		return M1;
	}
	
	
	/**
	 * Gets the server evidence message 'M2'.
	 *
	 * @return The server evidence message 'M2' if available, else
	 *         {@code null}.
	 */
	public BigInteger getServerEvidenceMessage() {
	
		return M2;
	}


	/**
	 * Gets the shared session key 'S'
	 *
	 * @return The shared session key 'S'. {@code null} 
	 *         will be returned if authentication failed or the method is
	 *         invoked in a session state when the session key 'S' has not
	 *         been computed yet.
	 */
	public BigInteger getSessionKey() {
		return S;
	}

	/**
	 * Gets the shared session key 'K' for "Espressif" devices.
	 *
	 * @return The shared session key 'K'. {@code null}
	 *         will be returned if authentication failed or the method is
	 *         invoked in a session state when the session key 'K' has not
	 *         been computed yet.
	 */
	public BigInteger getSharedSessionKey() {
		return K;
	}
	
	
	/**
	 * Gets the hash of the shared session key H(S).
	 *
	 * @return The hash of the shared session key H(S). {@code null} 
	 *         will be returned if authentication failed or the method is
	 *         invoked in a session state when the session key 'S' has not
	 *         been computed yet.
	 */
	public byte[] getSessionKeyHash() {
	
		if (S == null)
			return null;

		MessageDigest digest = config.getMessageDigestInstance();

		if (digest == null)
			throw new IllegalArgumentException("Unsupported hash algorithm 'H': " + config.H);
		
		return digest.digest(BigIntegerUtils.bigIntegerToBytes(S));
	}
	
	
	/**
	 * Sets a session attribute. This method can be used to store arbitrary
	 * objects with this session and retrieve them later with 
	 * {@link #getAttribute}.
	 *
	 * @param key   The attribute key. Must not be {@code null}.
	 * @param value The attribute value. May be {@code null}.
	 */
	public void setAttribute(final String key, final Object value) {
	
		if (key == null)
			throw new IllegalArgumentException("The attribute key must not be null");
			
		// create new attribute map on demand
		if (attributes == null)
			attributes = new HashMap<>();
		
		attributes.put(key, value);
	}
	
	
	/**
	 * Gets a session attribute. This method can be used to retrieve
	 * arbitrary objects stored with this session with 
	 * {@link #setAttribute}.
	 *
	 * @param key The attribute key. Must not be {@code null}.
	 *
	 * @return The attribute value, {@code null} if none was found by the
	 *         specified key or its value is {@code null}.
	 */
	public Object getAttribute(final String key) {
	
		if (key == null)
			throw new IllegalArgumentException("The attribute key must not be null");
		
		if (attributes == null)
			return null;
		
		return attributes.get(key);
	}
}
