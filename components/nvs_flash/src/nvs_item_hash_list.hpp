// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef nvs_item_hash_list_h
#define nvs_item_hash_list_h

#include "nvs.h"
#include "nvs_types.hpp"
#include "intrusive_list.h"

namespace nvs
{

class HashList
{
public:
    HashList();
    ~HashList();
    
    void insert(const Item& item, size_t index);
    void erase(const size_t index, bool itemShouldExist=true);
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

    struct HashListBlock : public intrusive_list_node<HashList::HashListBlock> {
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
