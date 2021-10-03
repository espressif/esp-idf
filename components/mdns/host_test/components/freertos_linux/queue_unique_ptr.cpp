// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "queue_unique_ptr.hpp"
#include <cstdint>
#include <vector>
#include <memory>
#include <cstring>

extern "C" void * create_q(void)
{
    auto * q = new QueueMock<std::vector<uint8_t>>();
    return q;
}

extern "C" void destroy_q(void* q)
{
    auto * queue = static_cast<QueueMock<std::vector<uint8_t>> *>(q);
    delete(queue);
}

extern "C" bool send_q(void* q, uint8_t *data, size_t len)
{
    auto v = std::make_unique<std::vector<uint8_t>>(len);
    v->assign(data, data+len);
    auto queue = static_cast<QueueMock<std::vector<uint8_t>> *>(q);
    queue->send(std::move(v));
    return true;
}

extern "C" bool recv_q(void* q, uint8_t *data, size_t len, uint32_t ms)
{
    auto queue = static_cast<QueueMock<std::vector<uint8_t>> *>(q);
    auto v = queue->receive(ms);
    if (v == nullptr) {
        return false;
    }
    memcpy(data, (void *)v->data(), len);
    return true;
}
