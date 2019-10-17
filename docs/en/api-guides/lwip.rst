LwIP
====

Overview About Socket Error Handling
------------------------------------

Socket error handling code is very important for robust socket applications. Normally the socket error handling involves the following aspects:

- Detecting the error.
- Geting the error reason code.
- Handle the error according to the reason code.

In LwIP, we have two different scenarios of handling socket errors:

- Socket API returns an error. For more information, see <`Error Handling of Socket API`_>.
- ``select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout)`` has exception descriptor indicating that the socket has an error. For more information, see <`Error Handling of Select`_>.

Error Handling of Socket API
++++++++++++++++++++++++++++

The error detection
  - We can know that the socket API fails according to its return value.

Get the error reason code
  - When socket API fails, the return value doesn't contain the failure reason and the application can get the error reason code by accessing errno. Different values indicate different meanings. For more information, see <`Socket Error Reason Code`_>.

Example::

        int err;
        int sockfd;

        if (sockfd = socket(AF_INET,SOCK_STREAM,0) < 0) {
            // the error code is obtained from errno
            err = errno;
            return err;
        }

Error Handling of Select
++++++++++++++++++++++++

The error detection
  - Socket error when select has exception descriptor

Get the error reason code
  - If the ``select`` indicates that the socket fails, we can't get the error reason code by accessing errno, instead we should call ``getsockopt()`` to get the failure reason code. Because ``select()`` has exception descriptor, the error code will not be given to errno.

.. note:: ``getsockopt`` function prototype ``int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)``. Its function is to get the current value of the option of any type, any state socket, and store the result in optval. For example, when you get the error code on a socket, you can get it by ``getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &optlen)``.

Example::

        int err;
        
        if (select(sockfd + 1, NULL, NULL, &exfds, &tval) <= 0) {
            err = errno;
            return err;
        } else {
            if (FD_ISSET(sockfd, &exfds)) {
                // select() exception set using getsockopt()
                int optlen = sizeof(int);
                getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &optlen);
                return err;
            }
        }

Socket Error Reason Code
++++++++++++++++++++++++

Below is a list of common error codes. For more detailed error codes, please query :component:`newlib/include/sys/errno.h`

+-----------------+-------------------------------------+
| Error code      | Description                         |
+=================+=====================================+
| ECONNREFUSED    | Connection refused                  |
+-----------------+-------------------------------------+
| EADDRINUSE      | Address already in use              |
+-----------------+-------------------------------------+
| ECONNABORTED    | Software caused connection abort    |
+-----------------+-------------------------------------+
| ENETUNREACH     | Network is unreachable              |
+-----------------+-------------------------------------+
| ENETDOWN        | Network interface is not configured |
+-----------------+-------------------------------------+
| ETIMEDOUT       | Connection timed out                |
+-----------------+-------------------------------------+
| EHOSTDOWN       | Host is down                        |
+-----------------+-------------------------------------+ 
| EHOSTUNREACH    | Host is unreachable                 |
+-----------------+-------------------------------------+ 
| EINPROGRESS     | Connection already in progress      |
+-----------------+-------------------------------------+ 
| EALREADY        | Socket already connected            |
+-----------------+-------------------------------------+ 
| EDESTADDRREQ    | Destination address required        |
+-----------------+-------------------------------------+ 
| EPROTONOSUPPORT | Unknown protocol                    |
+-----------------+-------------------------------------+ 

