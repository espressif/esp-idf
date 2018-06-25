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

#include "nvs_item_hash_list.hpp"

namespace nvs
{

HashList::HashList()
{
}
    
void HashList::clear()
{
    for (auto it = mBlockList.begin(); it != mBlockList.end();) {
        auto tmp = it;
        ++it;
        mBlockList.erase(tmp);
        delete static_cast<HashListBlock*>(tmp);
    }
}
    
HashList::~HashList()
{
    clear();
}

HashList::HashListBlock::HashListBlock()
{
    static_assert(sizeof(HashListBlock) == HashListBlock::BYTE_SIZE,
                  "cache block size calculation incorrect");
}

void HashList::insert(const Item& item, size_t index)
{
    const uint32_t hash_24 = item.calculateCrc32WithoutValue() & 0xffffff;
    // add entry to the end of last block if possible
    if (mBlockList.size()) {
        auto& block = mBlockList.back();
        if (block.mCount < HashListBlock::ENTRY_COUNT) {
            block.mNodes[block.mCount++] = HashListNode(hash_24, index);
            return;
        }
    }
    // if the above failed, create a new block and add entry to it
    HashListBlock* newBlock = new HashListBlock;
    mBlockList.push_back(newBlock);
    newBlock->mNodes[0] = HashListNode(hash_24, index);
    newBlock->mCount++;
}

void HashList::erase(size_t index, bool itemShouldExist)
{
    for (auto it = mBlockList.begin(); it != mBlockList.end();) {
        bool haveEntries = false;
        for (size_t i = 0; i < it->mCount; ++i) {
            if (it->mNodes[i].mIndex == index) {
                it->mNodes[i].mIndex = 0xff;
                return;
            }
            if (it->mNodes[i].mIndex != 0xff) {
                haveEntries = true;
            }
        }
        if (!haveEntries) {
            auto tmp = it;
            ++it;
            mBlockList.erase(tmp);
            delete static_cast<HashListBlock*>(tmp);
        } else {
            ++it;
        }
    }
    if (itemShouldExist) {
        assert(false && "item should have been present in cache");
    }
}

size_t HashList::find(size_t start, const Item& item)
{
    const uint32_t hash_24 = item.calculateCrc32WithoutValue() & 0xffffff;
    for (auto it = mBlockList.begin(); it != mBlockList.end(); ++it) {
        for (size_t index = 0; index < it->mCount; ++index) {
            HashListNode& e = it->mNodes[index];
            if (e.mIndex >= start &&
                    e.mHash == hash_24 &&
                    e.mIndex != 0xff) {
                return e.mIndex;
            }
        }
    }
    return SIZE_MAX;
}


} // namespace nvs
