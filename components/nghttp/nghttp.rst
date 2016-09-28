The nghttp Instruction
=============================

This is an implementation of the Hypertext Transfer Protocol version 2 in C.

The framing layer of HTTP/2 is implemented as a reusable C library.

An HPACK encoder and decoder are available as a public API.

-   The nghttp support many different processor, but for the most part you only need the following functions:

    **nghttp2_session_callbacks_new**: Initializes \*callbacks_ptr with NULL values

    **nghttp2_session_client_new**: Initializes \*session_ptr for client use

    **nghttp2_session_callbacks_del**: Frees any resources allocated for callbacks

    **nghttp2_submit_settings**: Stores local settings and submits SETTINGS frame

    **nghttp2_submit_request**: Submits HEADERS frame and optionally one or more DATA frames

    **nghttp2_session_want_read**: Returns nonzero value if session wants to receive data from the remote peer

    **nghttp2_session_want_write**: Returns nonzero value if session wants to send data to the remote peer

    **nghttp2_session_recv**: Receives frames from the remote peer

    **nghttp2_session_send**: Sends pending frames to the remote peer

    **nghttp2_session_del**: Frees any resources allocated for session

If you are following TLS related RFC, you know that NPN is not the standardized way to negotiate HTTP/2. NPN itself is not even published as RFC. 

The standard way to negotiate HTTP/2 is ALPN, Application-Layer Protocol Negotiation Extension, defined in RFC 7301. 

-   The following endpoints are available to try out the nghttp2 implementation:

    https://nghttp2.org/ (TLS + ALPN/NPN)

    This endpoint supports h2, h2-16, h2-14, spdy/3.1 and http/1.1 via ALPN/NPN and requires TLSv1.2 for HTTP/2 connection.

More information about nghttp library can be found at https://nghttp2.org

An introductory article on protocol is available on RFC 7540 HTTP/2 and RFC 7541 HPACK - Header Compression for HTTP/2
