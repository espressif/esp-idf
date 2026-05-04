# ESP HTTP Client State Machine

## States

| State | Value | Description |
|-------|-------|-------------|
| `HTTP_STATE_UNINIT` | 0 | Client not yet created |
| `HTTP_STATE_INIT` | 1 | Client created or connection closed (ready to connect) |
| `HTTP_STATE_CONNECTING` | 2 | Async connect in progress (async only) |
| `HTTP_STATE_CONNECTED` | 3 | TCP/TLS connection established |
| `HTTP_STATE_REQ_COMPLETE_HEADER` | 4 | Request headers sent |
| `HTTP_STATE_REQ_COMPLETE_DATA` | 5 | Request body (POST data) sent |
| `HTTP_STATE_RES_COMPLETE_HEADER` | 6 | Response headers received and parsed |
| `HTTP_STATE_RES_ON_DATA_START` | 7 | Response body ready to be read |
| `HTTP_STATE_RES_COMPLETE_DATA` | 8 | Defined but unused |
| `HTTP_STATE_CLOSE` | 9 | Defined but unused |

## Sync Mode

In sync (blocking) mode, `esp_http_client_perform()` drives the entire request/response
cycle in a single call. Each phase completes fully before moving to the next.
The `HTTP_STATE_CONNECTING` state is never observed — connect either succeeds or fails.

### State Transitions

```mermaid
stateDiagram-v2
    [*] --> UNINIT

    UNINIT --> INIT : esp_http_client_new()

    INIT --> CONNECTED : esp_http_client_connect()\n[esp_transport_connect succeeds]
    INIT --> ERROR : connect fails

    CONNECTED --> REQ_COMPLETE_HEADER : esp_http_client_request_send()\n[headers written]
    CONNECTED --> ERROR : request_send fails

    REQ_COMPLETE_HEADER --> REQ_COMPLETE_DATA : esp_http_client_send_post_data()\n[body sent]
    REQ_COMPLETE_HEADER --> ERROR : send_post_data fails

    REQ_COMPLETE_DATA --> RES_COMPLETE_HEADER : http_on_headers_complete()\n[parser callback]
    RES_COMPLETE_HEADER --> RES_ON_DATA_START : esp_http_client_fetch_headers()\n[headers parsed]
    REQ_COMPLETE_DATA --> ERROR : fetch_headers fails

    RES_ON_DATA_START --> CONNECTED : keep-alive\n[reuse connection]
    RES_ON_DATA_START --> INIT : no keep-alive\n[esp_http_client_close()]

    state ERROR <<choice>>
    ERROR --> INIT : esp_http_client_close()

    note right of CONNECTED
        On keep-alive, state resets here.
        Next perform() skips connection
        and reuses existing transport.
    end note
```

### `perform()` Flow

All switch-case stages fall through in a single invocation:

```mermaid
flowchart TD
    A[perform called] --> B[esp_http_client_connect]
    B -->|fail| ERR1[dispatch ERROR event\nreturn error]
    B -->|ok| C[esp_http_client_request_send]
    C -->|fail| ERR2[dispatch ERROR event\nreturn error]
    C -->|ok| D[esp_http_client_send_post_data]
    D -->|fail| ERR3[dispatch ERROR event\nreturn error]
    D -->|ok| E[esp_http_client_fetch_headers]
    E -->|fail| ERR4[dispatch ERROR event\nreturn error]
    E -->|ok| F[esp_http_check_response]
    F -->|redirect / auth| G[reset to CONNECTED\nre-enter loop]
    F -->|ok| H[read response body]
    H -->|keep-alive| I[reset to CONNECTED]
    H -->|no keep-alive| J[esp_http_client_close → INIT]
    I --> K{redirect?}
    J --> K
    K -->|yes| A
    K -->|no| L[return ESP_OK]
```

## Async Mode

In async (non-blocking) mode, each phase can return `ESP_ERR_HTTP_EAGAIN`
when the underlying operation would block. The caller must retry `esp_http_client_perform()`,
which resumes from the saved state via the switch-case.

### State Transitions

```mermaid
stateDiagram-v2
    [*] --> UNINIT

    UNINIT --> INIT : esp_http_client_new()

    INIT --> CONNECTING : connect_async returns ASYNC_TRANS_CONNECTING
    INIT --> CONNECTED : connect_async succeeds immediately
    CONNECTING --> CONNECTING : retry, connect_async still in progress
    CONNECTING --> CONNECTED : retry, connect_async succeeds

    CONNECTED --> REQ_COMPLETE_HEADER : esp_http_client_request_send()
    REQ_COMPLETE_HEADER --> REQ_COMPLETE_DATA : esp_http_client_send_post_data()

    REQ_COMPLETE_DATA --> RES_COMPLETE_HEADER : http_on_headers_complete() callback
    RES_COMPLETE_HEADER --> RES_ON_DATA_START : esp_http_client_fetch_headers()

    RES_ON_DATA_START --> CONNECTED : keep-alive\n[reuse connection]
    RES_ON_DATA_START --> INIT : no keep-alive, esp_http_client_close()
```

### `perform()` Flow

Each stage can yield back to the caller. Dashed arrows show the yield-and-retry path:

```mermaid
flowchart TD
    A[perform called] --> B{client->state?}

    B -->|INIT / CONNECTING| C[esp_http_client_connect]
    C -->|ASYNC_TRANS_CONNECTING| Y1[state = CONNECTING\nreturn EAGAIN to caller]
    Y1 -.->|caller retries perform| A
    C -->|fail| ERR1[return error]
    C -->|ok, state = CONNECTED| D

    B -->|CONNECTED| D[esp_http_client_request_send]
    D -->|EAGAIN| Y2[state = CONNECTED\nreturn EAGAIN to caller]
    Y2 -.->|caller retries perform| A
    D -->|fail| ERR2[return error]
    D -->|ok, state = REQ_COMPLETE_HEADER| E

    B -->|REQ_COMPLETE_HEADER| E[esp_http_client_send_post_data]
    E -->|EAGAIN| Y3[state = REQ_COMPLETE_HEADER\nreturn EAGAIN to caller]
    Y3 -.->|caller retries perform| A
    E -->|fail| ERR3[return error]
    E -->|ok, state = REQ_COMPLETE_DATA| F

    B -->|REQ_COMPLETE_DATA| F[esp_http_client_fetch_headers]
    F -->|EAGAIN| Y4[state = REQ_COMPLETE_DATA\nreturn EAGAIN to caller]
    Y4 -.->|caller retries perform| A
    F -->|fail| ERR4[return error]
    F -->|ok, state = RES_ON_DATA_START| G

    B -->|RES_ON_DATA_START| G[esp_http_check_response]
    G -->|redirect / auth| H[reset to CONNECTED\nre-enter loop]
    G -->|ok| I[read response body]
    I -->|keep-alive| J[reset to CONNECTED]
    I -->|no keep-alive| K[close → INIT]
    J --> L{redirect?}
    K --> L
    L -->|yes| A
    L -->|no| M[return ESP_OK]
```

### Async Connection Retry Detail

Zooms into the CONNECTING retry loop, showing the interaction between the
caller, `perform()`, and `esp_http_client_connect()`:

```mermaid
sequenceDiagram
    participant App as Application
    participant P as perform()
    participant C as esp_http_client_connect()
    participant T as esp_transport_connect_async()

    Note over App: state = INIT
    App->>P: esp_http_client_perform()
    P->>P: switch(state) hits INIT, falls through to CONNECTING
    P->>C: esp_http_client_connect()
    C->>C: state = CONNECTING
    C->>C: select transport
    C->>T: esp_transport_connect_async()
    T-->>C: ASYNC_TRANS_CONNECTING
    C-->>P: ESP_ERR_HTTP_CONNECTING
    P-->>App: ESP_ERR_HTTP_EAGAIN

    Note over App: state = CONNECTING, caller waits then retries

    App->>P: esp_http_client_perform()
    P->>P: switch(state) hits CONNECTING
    P->>C: esp_http_client_connect()
    C->>C: state = CONNECTING (no-op)
    C->>T: esp_transport_connect_async()
    T-->>C: ASYNC_TRANS_CONNECTING
    C-->>P: ESP_ERR_HTTP_CONNECTING
    P-->>App: ESP_ERR_HTTP_EAGAIN

    Note over App: state = CONNECTING, caller retries again

    App->>P: esp_http_client_perform()
    P->>P: switch(state) hits CONNECTING
    P->>C: esp_http_client_connect()
    C->>T: esp_transport_connect_async()
    T-->>C: ASYNC_TRANS_CONNECT_PASS (success)
    C->>C: state = CONNECTED
    C->>C: dispatch HTTP_EVENT_ON_CONNECTED
    C-->>P: ESP_OK
    P->>P: falls through to request_send...
```

## Connection Reuse (Keep-Alive)

When the server responds with `Connection: keep-alive`, the client reuses the
existing TCP/TLS connection for subsequent requests, avoiding the overhead of
a new handshake.

### Sync Connection Reuse

```mermaid
sequenceDiagram
    participant App as Application
    participant P as perform()
    participant C as esp_http_client_connect()

    Note over App: === First Request ===
    App->>P: esp_http_client_perform()
    P->>C: esp_http_client_connect()
    C->>C: state (INIT) < CONNECTED → true
    C->>C: select transport
    C->>C: esp_transport_connect() succeeds
    C->>C: state = CONNECTED
    C-->>P: ESP_OK
    P->>P: request_send → send_post_data → fetch_headers → read body
    P->>P: http_should_keep_alive() → true
    P->>P: state = CONNECTED (keep-alive reset)
    P-->>App: ESP_OK

    Note over App: state = CONNECTED, transport still open

    Note over App: === Second Request (reuses connection) ===
    App->>P: esp_http_client_perform()
    P->>C: esp_http_client_connect()
    C->>C: state (CONNECTED) < CONNECTED → false
    Note over C: Skip transport selection and connect.<br/>Existing connection reused.
    C-->>P: ESP_OK
    P->>P: request_send → send_post_data → fetch_headers → read body
    P-->>App: ESP_OK
```

### Async Connection Reuse

```mermaid
sequenceDiagram
    participant App as Application
    participant P as perform()
    participant C as esp_http_client_connect()
    participant T as esp_transport_connect_async()

    Note over App: === First Request (multiple perform retries) ===
    App->>P: esp_http_client_perform()
    P->>C: esp_http_client_connect()
    C->>C: state (INIT) < CONNECTED → true
    C->>C: select transport
    C->>T: esp_transport_connect_async()
    T-->>C: ASYNC_TRANS_CONNECTING
    C-->>P: ESP_ERR_HTTP_CONNECTING
    P-->>App: ESP_ERR_HTTP_EAGAIN

    App->>P: esp_http_client_perform() [retry]
    P->>C: esp_http_client_connect()
    C->>T: esp_transport_connect_async()
    T-->>C: ASYNC_TRANS_CONNECT_PASS
    C->>C: state = CONNECTED
    C-->>P: ESP_OK
    P->>P: request_send → send_post_data → fetch_headers → read body
    P->>P: http_should_keep_alive() → true
    P->>P: state = CONNECTED (keep-alive reset)
    P-->>App: ESP_OK

    Note over App: state = CONNECTED, transport still open

    Note over App: === Second Request (reuses connection) ===
    App->>P: esp_http_client_perform()
    P->>C: esp_http_client_connect()
    C->>C: state (CONNECTED) < CONNECTED → false
    Note over C: Skip transport selection and connect.<br/>Existing connection reused.
    C-->>P: ESP_OK
    P->>P: request_send → send_post_data → fetch_headers → read body
    P-->>App: ESP_OK
```

## Transition Details

### `esp_http_client_new()` → INIT
Sets state to `INIT` after allocating and initializing all client structures.

### `esp_http_client_connect()` → CONNECTING / CONNECTED
- **Sync**: Calls `esp_transport_connect()`. On success → `CONNECTED`. On failure → returns error.
- **Async**: Calls `esp_transport_connect_async()`.
  - `ASYNC_TRANS_CONNECTING` → stays `CONNECTING`, returns `ESP_ERR_HTTP_CONNECTING`.
  - Success → `CONNECTED`.
  - Failure → returns error.
- Dispatches `HTTP_EVENT_ON_CONNECTED` on reaching `CONNECTED`.
- If already `CONNECTED` (keep-alive reuse), skips the entire connection block.

### `esp_http_client_request_send()` → REQ_COMPLETE_HEADER
Writes the request line and headers to the transport. Dispatches `HTTP_EVENT_HEADERS_SENT`.

### `esp_http_client_send_post_data()` → REQ_COMPLETE_DATA
Writes any remaining POST body data to the transport.

### `http_on_headers_complete()` → RES_COMPLETE_HEADER
HTTP parser callback invoked during `esp_http_client_fetch_headers()` when all
response headers are parsed. Dispatches `HTTP_EVENT_ON_HEADERS_COMPLETE`.

### `esp_http_client_fetch_headers()` → RES_ON_DATA_START
Set after the header-parsing loop completes. Client is now ready to read response body.

### Keep-alive reset → CONNECTED
In `esp_http_client_perform()`, after response is fully read, if
`http_should_keep_alive()` is true, state resets to `CONNECTED` for connection reuse.
The transport remains open — the next `perform()` call skips `esp_http_client_connect()`'s
connection block entirely.

### `esp_http_client_close()` → INIT
Closes the transport and resets state to `INIT`. Dispatches `HTTP_EVENT_DISCONNECTED`.

## Events

Events dispatched during the state machine lifecycle:

| Event | When | State After |
|-------|------|-------------|
| `HTTP_EVENT_ON_CONNECTED` | TCP/TLS connection established | `CONNECTED` |
| `HTTP_EVENT_HEADERS_SENT` | Request headers written | `REQ_COMPLETE_HEADER` |
| `HTTP_EVENT_ON_HEADERS_COMPLETE` | Response headers parsed | `RES_COMPLETE_HEADER` |
| `HTTP_EVENT_ON_DATA` | Response body chunk received | `RES_ON_DATA_START` |
| `HTTP_EVENT_ON_FINISH` | Response body fully received | `RES_ON_DATA_START` |
| `HTTP_EVENT_DISCONNECTED` | Connection closed | `INIT` |
| `HTTP_EVENT_ERROR` | Any error during perform | varies |
