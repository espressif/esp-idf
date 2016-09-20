#ifndef _SSL_METHODS_H_
#define _SSL_METHODS_H_

#define IMPLEMENT_TLS_METHOD_FUNC(func_name, \
                    new, free, \
                    handshake, shutdown, clear, \
                    read, send, pending, \
                    set_fd, get_fd, \
                    set_bufflen, \
                    get_state) \
        static const SSL_METHOD_FUNC func_name = { \
                new, \
                free, \
                handshake, \
                shutdown, \
                clear, \
                read, \
                send, \
                pending, \
                set_fd, \
                get_fd, \
                set_bufflen, \
                get_state \
        };

#define IMPLEMENT_TLS_METHOD(ver, mode, fun, func_name) \
    const SSL_METHOD* func_name(void) { \
        static const SSL_METHOD func_name##_data = { \
                ver, \
                mode, \
                &(fun), \
        }; \
        return &func_name##_data; \
    }

#define IMPLEMENT_SSL_METHOD(ver, mode, fun, func_name) \
    const SSL_METHOD* func_name(void) { \
        static const SSL_METHOD func_name##_data = { \
                ver, \
                mode, \
                &(fun), \
        }; \
        return &func_name##_data; \
    }

#endif
