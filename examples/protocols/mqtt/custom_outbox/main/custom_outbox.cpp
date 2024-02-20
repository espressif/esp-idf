/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <algorithm>
#include <cstddef>
#include <exception>
#include <deque>
#include <cstdint>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>
#include <string>
#include <memory_resource>

#include "esp_log.h"
#include "mqtt_outbox.h"

constexpr auto TAG = "custom_outbox";

/*
 * The trace resource class is created here as an example on how to build a custom memory resource
 * The class is only needed to show where we are allocating from and to track allocations and deallocations.
 */
class trace_resource : public std::pmr::memory_resource {
public:
    explicit trace_resource(std::string resource_name,  std::pmr::memory_resource *upstream_resource = std::pmr::get_default_resource()) : upstream{upstream_resource}, name{std::move(resource_name)} {}
    [[nodiscard]] std::string_view get_name() const noexcept
    {
        return std::string_view(name);
    }
    [[nodiscard]] auto upstream_resource() const
    {
        return upstream;
    }
private:
    void *do_allocate(std::size_t bytes,  std::size_t alignment) override
    {
        auto *allocated = upstream->allocate(bytes, alignment);
        allocated_total += bytes;
        ESP_LOGI(name.c_str(), "%s: %zu bytes allocated, %zu total bytes in use", name.c_str(), bytes, allocated_total);
        return allocated;
    }
    void do_deallocate(void *ptr, std::size_t bytes, std::size_t alignment) override
    {
        upstream->deallocate(ptr, bytes, alignment);
        ESP_LOGI(name.c_str(), "%s: %zu bytes deallocated, %zu total bytes in use", name.c_str(), bytes, allocated_total);
    }

    [[nodiscard]] bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override
    {
        return this == &other;
    }
    size_t allocated_total{};
    std::pmr::memory_resource *upstream;
    std::string name;
};

struct outbox_item {
    /* Defining the allocator_type to let compiler know that our type is allocator aware,
     * This way the allocator used for the outbox is propagated to the messages*/
    using allocator_type = std::pmr::polymorphic_allocator<>;

    /* Few strong types to diferetiate parameters*/
    enum class id_t : int {};
    enum class type_t : int {};
    enum class qos_t : int {};

    /* Allocator aware constructors */
    outbox_item(
        std::pmr::vector<uint8_t> message,
        id_t msg_id,
        type_t msg_type,
        qos_t msg_qos,
        outbox_tick_t tick,
        pending_state_t pending_state,
        allocator_type alloc = {}
    ) : message(std::move(message), alloc), id(msg_id), type(msg_type), qos(msg_qos), tick(tick), pending_state(pending_state) {}

    /*Copy and move constructors have an extra allocator parameter, for copy default and allocator aware are the same.*/
    outbox_item(const outbox_item &other, allocator_type alloc = {}) : message(other.message, alloc), id(other.id), type(other.type), qos(other.qos), tick(other.tick), pending_state(other.pending_state) {}
    outbox_item(outbox_item &&other, allocator_type alloc) noexcept : message(std::move(other.message), alloc), id(other.id), type(other.type), qos(other.qos),  tick(other.tick), pending_state(other.pending_state)
    {}

    outbox_item(const outbox_item &) = default;
    outbox_item(outbox_item &&other) = default;
    outbox_item &operator=(const outbox_item &rhs) = default;
    outbox_item &operator=(outbox_item &&other) = default;
    ~outbox_item() = default;

    /* Getters to support outbox operation */
    [[nodiscard]] auto state() const noexcept
    {
        return pending_state;
    }

    [[nodiscard]] allocator_type get_allocator() const
    {
        return message.get_allocator();
    }

    void set(pending_state state) noexcept
    {
        pending_state = state;
    }

    void set(outbox_tick_t n_tick) noexcept
    {
        tick = n_tick;
    }

    [[nodiscard]] auto get_id() const noexcept
    {
        return id;
    }

    [[nodiscard]] auto get_type() const noexcept
    {
        return type;
    }

    [[nodiscard]] auto get_tick() const noexcept
    {
        return tick;
    }

    [[nodiscard]] auto get_data(size_t *len, uint16_t *msg_id, int *msg_type, int *msg_qos)
    {
        *len = message.size();
        *msg_id = static_cast<uint16_t>(id);
        *msg_type = static_cast<int>(type);
        *msg_qos =  static_cast<int>(qos);
        return message.data();
    }

    [[nodiscard]] auto get_size() const noexcept
    {
        return message.size();
    }

private:
    std::pmr::vector<uint8_t> message;
    id_t id;
    type_t type;
    qos_t qos;
    outbox_tick_t tick;
    pending_state_t pending_state;
};

/*
 * For the outbox_t we let the special member functions as default and
 * we don't extend the allocator aware versions for the sake of the simplicity, since the operations are not needed in the usage.
 */
struct outbox_t {
    using allocator_type = std::pmr::polymorphic_allocator<>;
    explicit outbox_t(allocator_type alloc = {}) : queue(alloc) {}

    outbox_item_handle_t get(outbox_item::id_t msg_id)
    {
        if (auto item = std::ranges::find_if(queue, [msg_id](auto & item) {
        return item.get_id() == msg_id;
        });
        item != std::end(queue)) {
            return &(*item);
        }
        return nullptr;
    }

    int delete_expired(outbox_tick_t current_tick, outbox_tick_t timeout)
    {
        return std::erase_if(queue, [current_tick, timeout, this](const outbox_item & item) {
            if (current_tick - item.get_tick() > timeout) {
                total_size -= item.get_size();
                return true;
            }
            return false;
        });
    }

    outbox_item::id_t delete_single_expired(outbox_tick_t current_tick, outbox_tick_t timeout)
    {
        if (auto erase = std::ranges::find_if(queue, [current_tick, timeout](auto & item) {
        return (current_tick - item.get_tick() > timeout);
        }); erase != std::end(queue)) {
            auto msg_id = erase->get_id();
            total_size -= erase->get_size();
            queue.erase(erase);
            return msg_id;
        }
        return outbox_item::id_t{-1};
    }

    auto erase(outbox_item_handle_t to_erase)
    {
        return erase_if([to_erase](auto & item) {
            return &item == to_erase;
        });
    }

    auto erase(outbox_item::id_t msg_id, outbox_item::type_t msg_type)
    {
        return erase_if([msg_id, msg_type](auto & item) {
            return (item.get_id() == msg_id && (item.get_type() == msg_type));
        });
    }

    [[nodiscard]] auto size() const noexcept
    {
        return total_size;
    }

    void clear()
    {
        queue.clear();
    }

    outbox_item_handle_t enqueue(outbox_message_handle_t message, outbox_tick_t tick) noexcept
    {
        try {
            auto &item =
                queue.emplace_back(std::pmr::vector<uint8_t> {message->data, message->data + message->len},
                                   outbox_item::id_t{message->msg_id},
                                   outbox_item::type_t{message->msg_type},
                                   outbox_item::qos_t{message->msg_qos},
                                   tick,
                                   QUEUED
                                  );
            total_size += item.get_size();
            ESP_LOGD(TAG, "ENQUEUE msgid=%d, msg_type=%d, len=%d, size=%" PRIu64, message->msg_id, message->msg_type, message->len + message->remaining_len, outbox_get_size(this));
            return &item;
        } catch (const std::exception &e) {
            return nullptr;
        }
    }

    outbox_item_handle_t dequeue(pending_state_t state, outbox_tick_t *tick)
    {
        if (auto item = std::ranges::find_if(queue, [state](auto & item) {
        return item.state() == state;
        });
        item != std::end(queue)) {
            if (tick != nullptr) {
                *tick = item->get_tick();
            }
            return &(*item);
        }
        return nullptr;
    }
    [[nodiscard]] allocator_type get_allocator() const
    {
        return queue.get_allocator();
    }
private:
    [[nodiscard]] esp_err_t erase_if(std::predicate<outbox_item &> auto &&predicate)
    {
        if (auto to_erase = std::ranges::find_if(queue, predicate); to_erase != std::end(queue)) {
            total_size -= to_erase->get_size();
            queue.erase(to_erase);
            return ESP_OK;
        }
        return ESP_FAIL;
    }
    std::size_t total_size{};
    std::pmr::deque<outbox_item> queue ;
};

extern "C" {

    outbox_handle_t outbox_init()
    {
        /* First we create a fixed size memory buffer to be used. */
        static constexpr  auto work_memory_size = 16 * 1024;
        static std::array<std::byte, work_memory_size> resource_buffer{};
        try {
            /*
             * Since the outbox is managed by a C API we can't rely on C++ automatic cleanup and smart pointers but, on production code it would be better to add the
             * memory resources to outbox_t, applying RAII principles, and make only outbox_item allocator aware. For the sake of the example we are keeping them
             * separated to explictly show the relations.
             * First we create the monotonic buffer and add null_memory_resource as upstream. This way if our working memory is exausted an exception is thrown.
                */
            auto *monotonic_resource = new std::pmr::monotonic_buffer_resource{resource_buffer.data(), resource_buffer.size(), std::pmr::null_memory_resource()};
            /*Here we add our custom trace wrapper type to trace allocations and deallocations*/
            auto  *trace_monotonic = new trace_resource("Monotonic", monotonic_resource);

            /* We compose monotonic buffer with pool resource, since the monotonic deallocate is a no-op and we need to remove messages to not go out of memory.*/
            auto  *pool_resource = new std::pmr::unsynchronized_pool_resource{trace_monotonic};
            auto  *trace_pool = new trace_resource("Pool", pool_resource);
            /* Our outbox class is created using the trace_pool as memory resource */
            auto *outbox = new outbox_t{trace_pool};
            return outbox;
        } catch (const std::exception &e) {
            ESP_LOGD(TAG, "Not enough memory to construct the outbox, review the resource_buffer size");
            return nullptr;

        }
    }

    outbox_item_handle_t outbox_enqueue(outbox_handle_t outbox, outbox_message_handle_t message, outbox_tick_t tick)
    {
        return outbox->enqueue(message, tick);
    }

    outbox_item_handle_t outbox_get(outbox_handle_t outbox, int msg_id)
    {
        return outbox->get(outbox_item::id_t{msg_id});
    }

    outbox_item_handle_t outbox_dequeue(outbox_handle_t outbox, pending_state_t pending, outbox_tick_t *tick)
    {
        return outbox->dequeue(pending, tick);
    }
}

uint8_t *outbox_item_get_data(outbox_item_handle_t item,  size_t *len, uint16_t *msg_id, int *msg_type, int *qos)
{
    if (item == nullptr) {
        return nullptr;
    }
    return item->get_data(len, msg_id, msg_type, qos);
}

esp_err_t outbox_delete_item(outbox_handle_t outbox, outbox_item_handle_t item_to_delete)
{
    return outbox->erase(item_to_delete);

}

esp_err_t outbox_delete(outbox_handle_t outbox, int msg_id, int msg_type)
{
    return outbox->erase(outbox_item::id_t{msg_id}, outbox_item::type_t{msg_type});
}

int outbox_delete_single_expired(outbox_handle_t outbox, outbox_tick_t current_tick, outbox_tick_t timeout)
{
    return static_cast<int>(outbox->delete_single_expired(current_tick, timeout));
}

int outbox_delete_expired(outbox_handle_t outbox, outbox_tick_t current_tick, outbox_tick_t timeout)
{
    return outbox->delete_expired(current_tick, timeout);
}

esp_err_t outbox_set_pending(outbox_handle_t outbox, int msg_id, pending_state_t pending)
{
    if (auto *item = outbox->get(outbox_item::id_t{msg_id}); item != nullptr) {
        item->set(pending);
        return ESP_OK;
    }
    return ESP_FAIL;
}

pending_state_t outbox_item_get_pending(outbox_item_handle_t item)
{
    if (item != nullptr) {
        return item->state();
    }
    return QUEUED;
}

esp_err_t outbox_set_tick(outbox_handle_t outbox, int msg_id, outbox_tick_t tick)
{
    if (auto *item = outbox->get(outbox_item::id_t{msg_id}); item != nullptr) {
        item->set(tick);
        return ESP_OK;
    }
    return ESP_FAIL;
}

uint64_t outbox_get_size(outbox_handle_t outbox)
{
    return outbox->size();
}

void outbox_delete_all_items(outbox_handle_t outbox)
{
    outbox->clear();
}

void outbox_destroy(outbox_handle_t outbox)
{
    auto  *trace_pool = static_cast<trace_resource *>(outbox->get_allocator().resource());
    auto *pool_resource = static_cast<std::pmr::unsynchronized_pool_resource *>(trace_pool->upstream_resource());
    auto  *trace_monotonic = static_cast<trace_resource *>(pool_resource->upstream_resource());
    auto *monotonic_resource = static_cast<std::pmr::monotonic_buffer_resource *>(trace_monotonic->upstream_resource());

    delete monotonic_resource;
    delete trace_monotonic;
    delete pool_resource;
    delete trace_pool;
    delete outbox;
}
