/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef nvs_item_hash_list_h
#define nvs_item_hash_list_h

#include "nvs.h"
#include "nvs_types.hpp"
#include "nvs_memory_management.hpp"
#include "intrusive_list.h"

namespace nvs
{

class HashList
{
public:
    HashList();
    ~HashList();

    esp_err_t insert(const Item& item, size_t index);
    bool erase(const size_t index);
    size_t find(size_t start, const Item& item);
    void clear();

private:
    HashList(const HashList& other);
    const HashList& operator= (const HashList& rhs);

protected:

    struct HashListNode {
        HashListNode() :
            mIndex(0xff), mHash(0)
        {
        }

        HashListNode(uint32_t hash, size_t index) :
            mIndex((uint32_t) index), mHash(hash)
        {
        }

        uint32_t mIndex : 8;
        uint32_t mHash  : 24;
    };

    struct HashListBlock : public intrusive_list_node<HashList::HashListBlock>, public ExceptionlessAllocatable {
        HashListBlock();

        static const size_t BYTE_SIZE = 128;
        static const size_t ENTRY_COUNT = (BYTE_SIZE - sizeof(intrusive_list_node<HashListBlock>) - sizeof(size_t)) / 4;

        size_t mCount = 0;
        HashListNode mNodes[ENTRY_COUNT];
    };

    typedef intrusive_list<HashListBlock> TBlockList;
    TBlockList mBlockList;
}; // class HashList

} // namespace nvs


#endif /* nvs_item_hash_list_h */
