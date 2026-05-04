#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX_SIZE 100
#define DATA_LEN 64

/* --- Data Packet --- */
typedef struct {
    int id;
    int priority;       // 0=low, 1=medium, 2=high
    char payload[DATA_LEN];
    time_t timestamp;
} DataPacket;

/* --- Stack --- */
typedef struct {
    DataPacket data[MAX_SIZE];
    int top;
} Stack;

void init(Stack *s)          { s->top = -1; }
bool isEmpty(Stack *s)       { return s->top == -1; }
bool isFull(Stack *s)        { return s->top == MAX_SIZE - 1; }

bool push(Stack *s, DataPacket *pkt) {
    if (isFull(s)) {
        printf("[WARN] Stack full — dropping packet id=%d\n", pkt->id);
        return false;
    }
    s->data[++s->top] = *pkt;
    return true;
}

bool pop(Stack *s, DataPacket *out) {
    if (isEmpty(s)) return false;
    *out = s->data[s->top--];
    return true;
}

bool peek(Stack *s, DataPacket *out) {
    if (isEmpty(s)) return false;
    *out = s->data[s->top];
    return true;
}

int size(Stack *s) { return s->top + 1; }

/* --- Data Handler --- */
typedef struct {
    Stack high;     // priority 2
    Stack medium;   // priority 1
    Stack low;      // priority 0
    int total_received;
    int total_processed;
} DataHandler;

void handler_init(DataHandler *h) {
    init(&h->high);
    init(&h->medium);
    init(&h->low);
    h->total_received = 0;
    h->total_processed = 0;
}

/* Route incoming data to the correct priority stack */
bool handler_receive(DataHandler *h, int id, int priority, const char *payload) {
    DataPacket pkt;
    pkt.id = id;
    pkt.priority = priority;
    pkt.timestamp = time(NULL);
    strncpy(pkt.payload, payload, DATA_LEN - 1);
    pkt.payload[DATA_LEN - 1] = '\0';

    Stack *target;
    switch (priority) {
        case 2:  target = &h->high;   break;
        case 1:  target = &h->medium; break;
        default: target = &h->low;    break;
    }

    bool ok = push(target, &pkt);
    if (ok) {
        h->total_received++;
        printf("[RECV] id=%-3d prio=%d payload=\"%s\"\n", id, priority, payload);
    }
    return ok;
}

/* Process one packet: high priority first, then medium, then low */
bool handler_process_one(DataHandler *h) {
    DataPacket pkt;
    Stack *source = NULL;

    if (!isEmpty(&h->high))        source = &h->high;
    else if (!isEmpty(&h->medium)) source = &h->medium;
    else if (!isEmpty(&h->low))    source = &h->low;

    if (!source) return false;

    pop(source, &pkt);
    h->total_processed++;
    printf("[PROC] id=%-3d prio=%d payload=\"%s\"\n", pkt.id, pkt.priority, pkt.payload);
    return true;
}

/* Drain all stacks in priority order */
void handler_process_all(DataHandler *h) {
    printf("\n--- Processing all data (high -> medium -> low) ---\n");
    while (handler_process_one(h)) { /* keep going */ }
    printf("--- Done ---\n");
}

void handler_status(DataHandler *h) {
    printf("\n[STATUS] received=%d  processed=%d  pending: high=%d med=%d low=%d\n",
           h->total_received, h->total_processed,
           size(&h->high), size(&h->medium), size(&h->low));
}

/* --- Main: simulate application data arriving --- */
int main(void) {
    DataHandler handler;
    handler_init(&handler);

    printf("=== Stack-Based Data Handler ===\n\n");

    /* Simulate incoming data from various application sources */
    handler_receive(&handler, 1, 0, "sensor_temp=23.5");
    handler_receive(&handler, 2, 1, "user_login=admin");
    handler_receive(&handler, 3, 2, "ALERT:overheat");
    handler_receive(&handler, 4, 0, "sensor_humidity=60");
    handler_receive(&handler, 5, 2, "ALERT:intrusion");
    handler_receive(&handler, 6, 1, "config_update=v2");
    handler_receive(&handler, 7, 0, "heartbeat_ok");
    handler_receive(&handler, 8, 2, "CRITICAL:shutdown");

    handler_status(&handler);

    /* Process all — high priority handled first */
    handler_process_all(&handler);

    handler_status(&handler);

    return 0;
}
