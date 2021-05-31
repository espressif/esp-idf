/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file ctrl_sock.h
 * \brief Control Socket for select() wakeup
 *
 * LWIP doesn't allow an easy mechanism to on-demand wakeup a thread
 * sleeping on select. This is a common requirement for sending
 * control commands to a network server. This control socket API
 * facilitates the same.
 */
#ifndef _CTRL_SOCK_H_
#define _CTRL_SOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a control socket
 *
 *      LWIP doesn't allow an easy mechanism to on-demand wakeup a thread
 *      sleeping on select. This is a common requirement for sending
 *      control commands to a network server. This control socket API
 *      facilitates the same.
 *
 *      This API will create a UDP control socket on the specified port. It
 *      will return a socket descriptor that can then be added to your
 *      fd_set in select()
 *
 * @param[in] port the local port on which the control socket will listen
 *
 * @return - the socket descriptor that can be added to the fd_set in select.
 *         - an error code if less than zero
 */
int cs_create_ctrl_sock(int port);

/**
 * @brief Free the control socket
 *
 *      This frees up the control socket that was earlier created using
 *      cs_create_ctrl_sock()
 *
 * @param[in] fd the socket descriptor associated with this control socket
 */
void cs_free_ctrl_sock(int fd);

/**
 * @brief Send data to control socket
 *
 *      This API sends data to the control socket. If a server is blocked
 *      on select() with the control socket, this call will wake up that
 *      server.
 *
 * @param[in] send_fd the socket for sending ctrl messages
 * @param[in] port the port on which the control socket was created
 * @param[in] data pointer to a buffer that contains data to send on the socket
 * @param[in] data_len the length of the data contained in the buffer pointed to be data
 *
 * @return  - the number of bytes sent to the control socket
 *          - an error code if less than zero
 */
int cs_send_to_ctrl_sock(int send_fd, int port, void *data, unsigned int data_len);

/**
 * @brief Receive data from control socket
 *
 *      This API receives any data that was sent to the control
 *      socket. This will be typically called from the server thread to
 *      process any commands on this socket.
 *
 * @param[in] fd the socket descriptor of the control socket
 * @param[in] data pointer to a buffer that will be used to store
 * received from the control socket
 * @param[in] data_len the maximum length of the data that can be
 * stored in the buffer pointed by data
 *
 * @return  - the number of bytes received from the control socket
 *          - an error code if less than zero
 */
int cs_recv_from_ctrl_sock(int fd, void *data, unsigned int data_len);

#ifdef __cplusplus
}
#endif

#endif /* ! _CTRL_SOCK_H_ */
