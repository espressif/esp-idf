// Nimbus SRP: Secure Remote Password (SRP-6a) protocol implementation for Java
//
//		Copyright (c) Connect2id Ltd. and others, 2011 - 2019

package com.espressif.provisioning.srp6a;


import java.io.Serializable;
import java.math.BigInteger;
import java.security.MessageDigest;


/**
 * Stateful server-side Secure Remote Password (SRP-6a) authentication session.
 * Handles the computing and storing of SRP-6a variables between the protocol 
 * steps as well as timeouts.
 *
 * <p>Usage:
 *
 * <ul>
 *     <li>Create a new SRP-6a server session for each client authentication
 *         attempt.
 *     <li>If you wish to use custom routines for the server evidence message
 *         'M1' and / or the client evidence message 'M2' specify them at this
 *         point.
 *     <li>Proceed to {@link #step1 step one} on receiving a valid user identity
 *         'I' from the authenticating client. Respond with the server public
 *         value 'B' and password salt 's'. If the SRP-6a crypto parameters 'N',
 *         'g' and 'H' were not agreed in advance between server and client 
 *         append them to the response.
 *     <li>Proceed to {@link #step2 step two} on receiving the public client 
 *         value 'A' and evidence message 'M1'. If the client credentials are
 *         valid signal success and return the server evidence message 'M2'. The
 *         established session key 'S' may be {@link #getSessionKey retrieved} to
 *         encrypt further communication with the client. Else signal an 
 *         authentication failure to the client.
 * </ul>
 *
 * @author Vladimir Dzhuvinov
 * @author Bernard Wittwer
 */
public class SRP6ServerSession extends SRP6Session implements Serializable {

	/**
	 * Serializable class version number
	 */
	private static final long serialVersionUID = -4076520488632450473L;


	/**
	 * Enumerates the states of a server-side SRP-6a authentication session.
	 */
	public static enum State {
	
	
		/**
		 * The session is initialised and ready to begin authentication,
		 * by proceeding to {@link #STEP_1}.
		 */
		INIT,
		
		
		/**
		 * The user identity 'I' is received from the client and the 
		 * server has returned its public value 'B' based on the 
		 * matching password verifier 'v'. The session is ready to 
		 * proceed to {@link #STEP_2}.
		 */
		STEP_1,
		
		
		/**
		 * The client public key 'A' and evidence message 'M1' are
		 * received and the server has replied with its own evidence
		 * message 'M2'. The session is finished (authentication was 
		 * successful or failed).
		 */
		STEP_2
	}
	
	
	/**
	 * Indicates a non-existing use identity and implies mock salt 's' and
	 * verifier 'v' values.
	 */
	private boolean noSuchUserIdentity = false;
	
	
	/**
	 * The password verifier 'v'.
	 */
	private BigInteger v = null;
	
	
	/**
	 * The server private value 'b'.
	 */
	private BigInteger b = null;
	
	
	/**
	 * The current SRP-6a auth state.
	 */
	private State state;
	
	
	/**
	 * Creates a new server-side SRP-6a authentication session and sets its 
	 * state to {@link State#INIT}.
	 *
	 * @param config  The SRP-6a crypto parameters configuration. Must not 
	 *                be {@code null}.
	 * @param timeout The SRP-6a authentication session timeout in seconds. 
	 *                If the authenticating counterparty (server or client) 
	 *                fails to respond within the specified time the session
	 *                will be closed. If zero timeouts are disabled.
	 */
	public SRP6ServerSession(final SRP6CryptoParams config, final int timeout) {
	
		super(timeout);
		
		if (config == null)
			throw new IllegalArgumentException("The SRP-6a crypto parameters must not be null");

		this.config = config;
		
		if (config.getMessageDigestInstance() == null)
			throw new IllegalArgumentException("Unsupported hash algorithm 'H': " + config.H);

		state = State.INIT;
		
		updateLastActivityTime();
	}
	
	
	/**
	 * Creates a new server-side SRP-6a authentication session and sets its 
	 * state to {@link State#INIT}. Session timeouts are disabled.
	 *
	 * @param config  The SRP-6a crypto parameters configuration. Must not 
	 *                be {@code null}.
	 */
	public SRP6ServerSession(final SRP6CryptoParams config) {
	
		this(config, 0);
	}
	
	
	/**
	 * Increments this SRP-6a authentication session to 
	 * {@link State#STEP_1}.
	 *
	 * <p>Argument origin:
	 * 
	 * <ul>
	 *     <li>From client: user identity 'I'.
	 *     <li>From server database: matching salt 's' and password verifier
	 *        'v' values.
	 * </ul>
	 *
	 * @param userID The identity 'I' of the authenticating user. Must not 
	 *               be {@code null} or empty.
	 * @param s      The password salt 's'. Must not be {@code null}.
	 * @param v      The password verifier 'v'. Must not be {@code null}.
	 *
	 * @return The server public value 'B'.
	 *
	 * @throws IllegalStateException If the mehod is invoked in a state 
	 *                               other than {@link State#INIT}.
	 */
	public BigInteger step1(final String userID, final BigInteger s, final BigInteger v) {
	
		// Check arguments
		
		if (userID == null || userID.trim().isEmpty())
			throw new IllegalArgumentException("The user identity 'I' must not be null or empty");
			
		this.userID = userID;
		
		
		if (s == null)
			throw new IllegalArgumentException("The salt 's' must not be null");
			
		this.s = s;
		
		
		if (v == null)
			throw new IllegalArgumentException("The verifier 'v' must not be null");
			
		this.v = v;
		
		
		// Check current state
		if (state != State.INIT)
			throw new IllegalStateException("State violation: Session must be in INIT state");
		
		MessageDigest digest = config.getMessageDigestInstance();
		
		// Generate server private and public values
		k = srp6Routines.computeK(digest, config.N, config.g);
		digest.reset();
		
		b = srp6Routines.generatePrivateValue(config.N, random);
		digest.reset();
		
		B = srp6Routines.computePublicServerValue(config.N, config.g, k, v, b);

		state = State.STEP_1;
		
		updateLastActivityTime();
		
		return B;
	}
	
	
	/**
	 * Increments this SRP-6a authentication session to 
	 * {@link State#STEP_1} indicating a non-existing user identity 'I'
	 * with mock (simulated) salt 's' and password verifier 'v' values.
	 *
	 * <p>This method can be used to avoid informing the client at step one
	 * that the user identity is bad and throw instead a guaranteed general 
	 * "bad credentials" SRP-6a exception at step two.
	 *
	 * <p>Argument origin:
	 * 
	 * <ul>
	 *     <li>From client: user identity 'I'.
	 *     <li>Simulated by server, preferably consistently for the
	 *         specified identity 'I': salt 's' and password verifier 'v' 
	 *         values.
	 * </ul>
	 *
	 * @param userID The identity 'I' of the authenticating user. Must not 
	 *               be {@code null} or empty.
	 * @param s      The password salt 's'. Must not be {@code null}.
	 * @param v      The password verifier 'v'. Must not be {@code null}.
	 *
	 * @return The server public value 'B'.
	 *
	 * @throws IllegalStateException If the method is invoked in a state
	 *                               other than {@link State#INIT}.
	 */
	public BigInteger mockStep1(final String userID, final BigInteger s, final BigInteger v) {
	
		noSuchUserIdentity = true;
		
		return step1(userID, s, v);
	}
	
	
	/**
	 * Increments this SRP-6a authentication session to 
	 * {@link State#STEP_2}.
	 *
	 * <p>Argument origin:
	 * 
	 * <ul>
	 *     <li>From client: public value 'A' and evidence message 'M1'.
	 * </ul>
	 *
	 * @param A  The client public value. Must not be {@code null}.
	 * @param M1 The client evidence message. Must not be {@code null}.
	 *
	 * @return The server evidence message 'M2'.
	 *
	 * @throws SRP6Exception If the session has timed out, the client public
	 *                       value 'A' is invalid or the user credentials
	 *                       are invalid.
	 *
	 * @throws IllegalStateException If the method is invoked in a state
	 *                               other than {@link State#STEP_1}.
	 */
	public BigInteger step2(final BigInteger A, final BigInteger M1)
		throws SRP6Exception {
	
		// Check arguments
		
		if (A == null)
			throw new IllegalArgumentException("The client public value 'A' must not be null");
			
		this.A = A;
		
		if (M1 == null)
			throw new IllegalArgumentException("The client evidence message 'M1' must not be null");
		
		this.M1 = M1;
	
		// Check current state
		if (state != State.STEP_1)
			throw new IllegalStateException("State violation: Session must be in STEP_1 state");
		
		// Check timeout
		if (hasTimedOut())
			throw new SRP6Exception("Session timeout", SRP6Exception.CauseType.TIMEOUT);
	
		// Check A validity
		if (! srp6Routines.isValidPublicValue(config.N, A))
			throw new SRP6Exception("Bad client public value 'A'", SRP6Exception.CauseType.BAD_PUBLIC_VALUE);
		
		MessageDigest digest = config.getMessageDigestInstance();
		
		if (hashedKeysRoutine != null) {
			URoutineContext hashedKeysContext = new URoutineContext(A, B);
			u = hashedKeysRoutine.computeU(config, hashedKeysContext);
		} else {
			u = srp6Routines.computeU(digest, config.N, A, B);
			digest.reset();
		}
		
		S = srp6Routines.computeSessionKey(config.N, v, u, A, b);
		
		// Compute the own client evidence message 'M1'
		BigInteger computedM1;
		
		if (clientEvidenceRoutine != null) {
		
			// With custom routine
			SRP6ClientEvidenceContext ctx = new SRP6ClientEvidenceContext(userID, s, A, B, K);
			computedM1 = clientEvidenceRoutine.computeClientEvidence(config, ctx);
		}
		else {
			// With default routine
			computedM1 = srp6Routines.computeClientEvidence(digest, A, B, S);
			digest.reset();
		}

		// Check for previous mock step 1 then check whether password proof works.
		if (noSuchUserIdentity || ! computedM1.equals(M1))
			throw new SRP6Exception("Bad client credentials", SRP6Exception.CauseType.BAD_CREDENTIALS);
	
		state = State.STEP_2;
		
		
		if (serverEvidenceRoutine != null) {
		
			// With custom routine
			SRP6ServerEvidenceContext ctx = new SRP6ServerEvidenceContext(A, M1, S);
			
			M2 = serverEvidenceRoutine.computeServerEvidence(config, ctx);
		}
		else {
			// With default routine
			M2 = srp6Routines.computeServerEvidence(digest, A, M1, S);
			digest.reset();
		}
		
		updateLastActivityTime();
		
		return M2;
	}
	
	
	/**
	 * Returns the current state of this SRP-6a authentication session.
	 *
	 * @return The current state.
	 */
	public State getState() {
	
		return state;
	}
}
