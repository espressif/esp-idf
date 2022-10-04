// Nimbus SRP: Secure Remote Password (SRP-6a) protocol implementation for Java
//
//		Copyright (c) Connect2id Ltd. and others, 2011 - 2019

package com.espressif.provisioning.srp6a;

import android.util.Log;

import java.io.Serializable;
import java.math.BigInteger;
import java.nio.charset.Charset;
import java.security.MessageDigest;


/**
 * Stateful client-side Secure Remote Password (SRP-6a) authentication session.
 * Handles the computing and storing of SRP-6a variables between the protocol
 * steps as well as timeouts.
 *
 * <p>Usage:
 *
 * <ul>
 *     <li>Create a new SRP-6a client session for each authentication attempt.
 *     <li>If you wish to use custom routines for the password key 'x', the
 *         server evidence message 'M1', and / or the client evidence message
 *        'M2' specify them at this point.
 *     <li>Proceed to {@link #step1 step one} by recording the input user
 *         identity 'I' (submitted to the server) and password 'P'.
 *     <li>Proceed to {@link #step2 step two} on receiving the password salt
 *         's' and the public server value 'B' from the server. At this point
 *         the SRP-6a crypto parameters 'N', 'g' and 'H' must also be
 *         specified. These can either be agreed in advance between server and
 *         client or suggested by the server in its step one response.
 *     <li>Proceed to {@link #step3 step three} on receiving the server
 *         evidence message 'M2'.
 * </ul>
 *
 * @author Vladimir Dzhuvinov
 * @author Bernard Wittwer
 */
public class SRP6ClientSession extends SRP6Session implements Serializable {


    /**
     * Serializable class version number
     */
    private static final long serialVersionUID = -479060216624675478L;


    /**
     * Enumerates the states of a client-side SRP-6a authentication
     * session.
     */
    public static enum State {


        /**
         * The session is initialised and ready to begin authentication
         * by proceeding to {@link #STEP_1}.
         */
        INIT,


        /**
         * The authenticating user has input their identity 'I'
         * (username) and password 'P'. The session is ready to proceed
         * to {@link #STEP_2}.
         */
        STEP_1,


        /**
         * The user identity 'I' is submitted to the server which has
         * replied with the matching salt 's' and its public value 'B'
         * based on the user's password verifier 'v'. The session is
         * ready to proceed to {@link #STEP_3}.
         */
        STEP_2,


        /**
         * The client public key 'A' and evidence message 'M1' are
         * submitted and the server has replied with own evidence
         * message 'M2'. The session is finished (authentication was
         * successful or failed).
         */
        STEP_3
    }


    /**
     * The user password 'P'.
     */
    private String password;


    /**
     * The password key 'x'.
     */
    private BigInteger x = null;


    /**
     * The client private value 'a'.
     */
    private BigInteger a = null;


    /**
     * The current SRP-6a auth state.
     */
    private State state;


    /**
     * Custom routine for password key 'x' computation.
     */
    private XRoutine xRoutine = null;


    /**
     * Creates a new client-side SRP-6a authentication session and sets its
     * state to {@link State#INIT}.
     *
     * @param timeout The SRP-6a authentication session timeout in seconds.
     *                If the authenticating counterparty (server or client)
     *                fails to respond within the specified time the session
     *                will be closed. If zero timeouts are disabled.
     */
    public SRP6ClientSession(final int timeout) {

        super(timeout);

        state = State.INIT;

        updateLastActivityTime();
    }


    /**
     * Creates a new client-side SRP-6a authentication session and sets its
     * state to {@link State#INIT}. Session timeouts are disabled.
     */
    public SRP6ClientSession() {

        this(0);
    }


    /**
     * Sets a custom routine for the password key 'x' computation. Note that
     * the custom routine must be set prior to {@link State#STEP_2}.
     *
     * @param routine The password key 'x' routine or {@code null} to use
     *                the {@link SRP6Routines#computeX default one} instead.
     */
    public void setXRoutine(final XRoutine routine) {

        xRoutine = routine;
    }


    /**
     * Gets the custom routine for the password key 'x' computation.
     *
     * @return The routine instance or {@code null} if the default
     * {@link SRP6Routines#computeX default one} is used.
     */
    public XRoutine getXRoutine() {

        return xRoutine;
    }


    /**
     * Records the identity 'I' and password 'P' of the authenticating
     * user. The session is incremented to {@link State#STEP_1}.
     *
     * <p>Argument origin:
     *
     * <ul>
     *     <li>From user: user identity 'I' and password 'P'.
     * </ul>
     *
     * @param userID   The identity 'I' of the authenticating user, UTF-8
     *                 encoded. Must not be {@code null} or empty.
     * @param password The user password 'P', UTF-8 encoded. Must not be
     *                 {@code null}.
     * @throws IllegalStateException If the method is invoked in a state
     *                               other than {@link State#INIT}.
     */
    public void step1(final String userID, final String password) {

        if (userID == null || userID.trim().isEmpty())
            throw new IllegalArgumentException("The user identity 'I' must not be null or empty");

        this.userID = userID;


        if (password == null)
            throw new IllegalArgumentException("The user password 'P' must not be null");

        this.password = password;

        // Check current state
        if (state != State.INIT)
            throw new IllegalStateException("State violation: Session must be in INIT state");

        state = State.STEP_1;

        updateLastActivityTime();
    }

    /**
     * This method is used to generate client public key 'A'.
     * [Note : Customised 'step2' method into 2 methods.]
     * @param config    The SRP-6a crypto parameters. Must not be {@code null}.
     * @return The client public key
     * @throws SRP6Exception     If the session has timed out or the
     *                           public server value 'B' is invalid.
     */
    public BigInteger getClientPublicKey(final SRP6CryptoParams config)
            throws SRP6Exception {

        // Check arguments
        if (config == null)
            throw new IllegalArgumentException("The SRP-6a crypto parameters must not be null");

        this.config = config;

        MessageDigest digest = config.getMessageDigestInstance();

        if (digest == null)
            throw new IllegalArgumentException("Unsupported hash algorithm 'H': " + config.H);

        // Check current state
        if (state != State.STEP_1)
            throw new IllegalStateException("State violation: Session must be in STEP_1 state");

        // Check timeout
        if (hasTimedOut())
            throw new SRP6Exception("Session timeout", SRP6Exception.CauseType.TIMEOUT);

        // Generate client private and public values
        a = srp6Routines.generatePrivateValue(config.N, random);
        digest.reset();

        A = srp6Routines.computePublicClientValue(config.N, config.g, a);
        return A;
    }

    /**
     * Receives the password salt 's' and public value 'B' from the server.
     * The SRP-6a crypto parameters are also set. The session is incremented
     * to {@link State#STEP_2}.
     *
     * @param config The SRP-6a crypto parameters. Must not be {@code null}.
     * @param s      The password salt 's'. Must not be {@code null}.
     * @param B      The public server value 'B'. Must not be {@code null}.
     * @return The client credentials consisting of the client public key
     * 'A' and the client evidence message 'M1'.
     * @throws IllegalStateException If the method is invoked in a state
     *                               other than {@link State#STEP_1}.
     * @throws SRP6Exception         If the session has timed out or the
     *                               public server value 'B' is invalid.
     */
    public SRP6ClientCredentials step2_for_client_evidence(final SRP6CryptoParams config, final BigInteger s, final BigInteger B, byte[] s_temp)
            throws SRP6Exception {

        // Check arguments
        if (config == null)
            throw new IllegalArgumentException("The SRP-6a crypto parameters must not be null");

        this.config = config;

        MessageDigest digest = config.getMessageDigestInstance();

        if (digest == null)
            throw new IllegalArgumentException("Unsupported hash algorithm 'H': " + config.H);


        if (s == null)
            throw new IllegalArgumentException("The salt 's' must not be null");

        this.s = s;


        if (B == null)
            throw new IllegalArgumentException("The public server value 'B' must not be null");

        this.B = B;


        // Check current state
        if (state != State.STEP_1)
            throw new IllegalStateException("State violation: Session must be in STEP_1 state");


        // Check timeout
        if (hasTimedOut())
            throw new SRP6Exception("Session timeout", SRP6Exception.CauseType.TIMEOUT);


        // Check B validity
        if (!srp6Routines.isValidPublicValue(config.N, B))
            throw new SRP6Exception("Bad server public value 'B'", SRP6Exception.CauseType.BAD_PUBLIC_VALUE);


        // Use xRoutine with user identity.
        xRoutine = new XRoutineWithUserIdentity();

        // Compute the password key 'x'
        if (xRoutine != null) {

            // With custom routine
            x = xRoutine.computeX(config.getMessageDigestInstance(),
                    BigIntegerUtils.bigIntegerToBytes(s),
                    userID.getBytes(Charset.forName("UTF-8")),
                    password.getBytes(Charset.forName("UTF-8")));

        } else {
            // With default routine
            x = srp6Routines.computeX(digest, BigIntegerUtils.bigIntegerToBytes(s), password.getBytes(Charset.forName("UTF-8")));
            digest.reset();
        }

        // Compute the session key
        k = srp6Routines.computeK(digest, config.N, config.g);
        digest.reset();

        if (hashedKeysRoutine != null) {
            URoutineContext hashedKeysContext = new URoutineContext(A, B);
            u = hashedKeysRoutine.computeU(config, hashedKeysContext);
        } else {
            u = srp6Routines.computeU(digest, config.N, A, B);
            digest.reset();
        }

        S = srp6Routines.computeSessionKey(config.N, config.g, k, x, u, a, B);
        K = srp6Routines.computeSharedSessionKey(digest, S);
        Log.d("SRP6aClient", "Shared session key generated");

        // Set custom routine (for Espressif devices) to compute client evidence.
        ClientSRP6Routines clientSRP6Routines = new ClientSRP6Routines(digest);
        setClientEvidenceRoutine(clientSRP6Routines);

        // Compute the client evidence message
        if (clientEvidenceRoutine != null) {

            // With custom routine
            // Note : Used shared session key 'K' for Espressif devices instead of 'S'
            SRP6ClientEvidenceContext ctx = new SRP6ClientEvidenceContext(userID, s, A, B, K);
            M1 = clientEvidenceRoutine.computeClientEvidence(config, ctx);
        } else {
            // With default routine
            M1 = srp6Routines.computeClientEvidence(digest, A, B, S);
            digest.reset();
        }

        state = State.STEP_2;


        updateLastActivityTime();

        return new SRP6ClientCredentials(A, M1);
    }


    /**
     * Receives the password salt 's' and public value 'B' from the server.
     * The SRP-6a crypto parameters are also set. The session is incremented
     * to {@link State#STEP_2}.
     *
     * <p>Argument origin:
     *
     * <ul>
     *     <li>From server: password salt 's', public value 'B'.
     *     <li>From server or pre-agreed: crypto parameters prime 'N',
     *         generator 'g' and hash function 'H'.
     * </ul>
     *
     * @param config The SRP-6a crypto parameters. Must not be {@code null}.
     * @param s      The password salt 's'. Must not be {@code null}.
     * @param B      The public server value 'B'. Must not be {@code null}.
     * @return The client credentials consisting of the client public key
     * 'A' and the client evidence message 'M1'.
     * @throws IllegalStateException If the method is invoked in a state
     *                               other than {@link State#STEP_1}.
     * @throws SRP6Exception         If the session has timed out or the
     *                               public server value 'B' is invalid.
     */
    public SRP6ClientCredentials step2(final SRP6CryptoParams config, final BigInteger s, final BigInteger B)
            throws SRP6Exception {

        // Check arguments
        if (config == null)
            throw new IllegalArgumentException("The SRP-6a crypto parameters must not be null");

        this.config = config;

        MessageDigest digest = config.getMessageDigestInstance();

        if (digest == null)
            throw new IllegalArgumentException("Unsupported hash algorithm 'H': " + config.H);


        if (s == null)
            throw new IllegalArgumentException("The salt 's' must not be null");

        this.s = s;


        if (B == null)
            throw new IllegalArgumentException("The public server value 'B' must not be null");

        this.B = B;


        // Check current state
        if (state != State.STEP_1)
            throw new IllegalStateException("State violation: Session must be in STEP_1 state");


        // Check timeout
        if (hasTimedOut())
            throw new SRP6Exception("Session timeout", SRP6Exception.CauseType.TIMEOUT);


        // Check B validity
        if (!srp6Routines.isValidPublicValue(config.N, B))
            throw new SRP6Exception("Bad server public value 'B'", SRP6Exception.CauseType.BAD_PUBLIC_VALUE);


        // Compute the password key 'x'
        if (xRoutine != null) {

            // With custom routine
            x = xRoutine.computeX(config.getMessageDigestInstance(),
                    BigIntegerUtils.bigIntegerToBytes(s),
                    userID.getBytes(Charset.forName("UTF-8")),
                    password.getBytes(Charset.forName("UTF-8")));

        } else {
            // With default routine
            x = srp6Routines.computeX(digest, BigIntegerUtils.bigIntegerToBytes(s), password.getBytes(Charset.forName("UTF-8")));
            digest.reset();
        }

        // Generate client private and public values
        a = srp6Routines.generatePrivateValue(config.N, random);
        digest.reset();

        A = srp6Routines.computePublicClientValue(config.N, config.g, a);


        // Compute the session key
        k = srp6Routines.computeK(digest, config.N, config.g);
        digest.reset();

        if (hashedKeysRoutine != null) {
            URoutineContext hashedKeysContext = new URoutineContext(A, B);
            u = hashedKeysRoutine.computeU(config, hashedKeysContext);
        } else {
            u = srp6Routines.computeU(digest, config.N, A, B);
            digest.reset();
        }

        S = srp6Routines.computeSessionKey(config.N, config.g, k, x, u, a, B);

        // Compute the client evidence message
        if (clientEvidenceRoutine != null) {

            // With custom routine
            SRP6ClientEvidenceContext ctx = new SRP6ClientEvidenceContext(userID, s, A, B, K);
            M1 = clientEvidenceRoutine.computeClientEvidence(config, ctx);

        } else {
            // With default routine
            M1 = srp6Routines.computeClientEvidence(digest, A, B, S);
            digest.reset();
        }

        state = State.STEP_2;

        updateLastActivityTime();

        return new SRP6ClientCredentials(A, M1);
    }


    /**
     * Receives the server evidence message 'M1'. The session is
     * incremented to {@link State#STEP_3}.
     *
     * <p>
     * Argument origin:
     *
     * <ul>
     * <li>From server: evidence message 'M2'.
     * </ul>
     *
     * @param M2 The server evidence message 'M2'. Must not be
     *           {@code null}.
     * @throws IllegalStateException If the method is invoked in a state
     *                               other than {@link State#STEP_2}.
     * @throws SRP6Exception         If the session has timed out or the
     *                               server evidence message 'M2' is
     *                               invalid.
     */
    public void step3(final BigInteger M2)
            throws SRP6Exception {

        // Check argument

        if (M2 == null)
            throw new IllegalArgumentException("The server evidence message 'M2' must not be null");

        this.M2 = M2;

        // Check current state
        if (state != State.STEP_2)
            throw new IllegalStateException("State violation: Session must be in STEP_2 state");

        // Check timeout
        if (hasTimedOut())
            throw new SRP6Exception("Session timeout", SRP6Exception.CauseType.TIMEOUT);


        // Compute the own server evidence message 'M2'
        BigInteger computedM2;

        if (serverEvidenceRoutine != null) {

            // Pass shared session key "K" instead of "S" to compute server evidence for Espressif devices
            // With custom routine
            SRP6ServerEvidenceContext ctx = new SRP6ServerEvidenceContext(A, M1, K);
            computedM2 = serverEvidenceRoutine.computeServerEvidence(config, ctx);

        } else {
            // Pass shared session key "K" instead of "S" to compute server evidence for Espressif devices
            // With default routine
            MessageDigest digest = config.getMessageDigestInstance();
            computedM2 = srp6Routines.computeServerEvidence(digest, A, M1, K);
        }

        if (!computedM2.equals(M2))
            throw new SRP6Exception("Bad server credentials", SRP6Exception.CauseType.BAD_CREDENTIALS);

        state = State.STEP_3;

        updateLastActivityTime();
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
