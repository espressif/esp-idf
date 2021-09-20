| Supported Targets | ESP32 |
| ----------------- | ----- |

# Async request using ASIO

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The application aims to show how to compose async operations using ASIO to build network protocols and operations.

# Configure and Building example

This example doesn't require any configuration, just build it with 

```
idf.py build
```

# Async operations composition and automatic lifetime control

On this example we compose the operation by starting the next step in the chain inside the completion handler of the 
previous operation. Also we pass the `Connection` class itself as the parameter of its final handler to be owned by 
the following operation. This is possible due to the control of lifetime by the usage of `std::shared_ptr`.

The control of lifetime of the class, done by `std::shared_ptr` usage, guarantee that the data will be available for 
async operations until it's not needed any more. This makes necessary that all of the async operation class must start 
its lifetime as a `std::shared_ptr` due to the usage of `std::enable_shared_from_this`.


     User creates a shared_ptr──┐
     of AddressResolution and   │
     ask for resolve.           │
     The handler for the       ┌▼─────────────────────┐
     complete operation is sent│   AddressResolution  │  In the completion of resolve a connection is created.
                               └─────────────────┬────┘  AddressResolution is automaticly destroyed since it's
                                                 │       no longer needed
                                               ┌─▼────────────────────────────────────┐
                                               │         Connection                   │
                                               └──────┬───────────────────────────────┘
  Http::Session is created once we have a Connection. │
  Connection is passed to Http::Session that holds it │
  avoiding it's destruction.                          │
                                                    ┌─▼───────────────────────────────┐
                                                    │       Http::Session             │
                                                    └────────┬────────────────────────┘
                                After the HTTP request is    │
                                sent the completion handler  │
                                is called.                   │
                                                             └────►Completion Handler()


The previous diagram shows the process and the life span of each of the tasks in this examples. At each stage the 
object responsible for the last action inject itself to the completion handler of the next stage for reuse.
