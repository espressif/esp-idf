/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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

esp_err_t HashList::insert(const Item& item, size_t index)
{
    const uint32_t hash_24 = item.calculateCrc32WithoutValue() & 0xffffff;
    // add entry to the end of last block if possible
    if (mBlockList.size()) {
        auto& block = mBlockList.back();
        if (block.mCount < HashListBlock::ENTRY_COUNT) {
            block.mNodes[block.mCount++] = HashListNode(hash_24, index);
            return ESP_OK;
        }
    }
    // if the above failed, create a new block and add entry to it
    HashListBlock* newBlock = new (std::nothrow) HashListBlock;

    if (!newBlock) return ESP_ERR_NO_MEM;

    mBlockList.push_back(newBlock);
    newBlock->mNodes[0] = HashListNode(hash_24, index);
    newBlock->mCount++;

    return ESP_OK;
}

bool HashList::erase(size_t index)
{
    for (auto it = mBlockList.begin(); it != mBlockList.end();) {
        bool haveEntries = false;
        bool foundIndex = false;
        for (size_t i = 0; i < it->mCount; ++i) {
            if (it->mNodes[i].mIndex == index) {
                it->mNodes[i].mIndex = 0xff;
                foundIndex = true;
                /* found the item and removed it */
            }
            if (it->mNodes[i].mIndex != 0xff) {
                haveEntries = true;
            }
            if (haveEntries && foundIndex) {
                /* item was found, and HashListBlock still has some items */
                return true;
            }
        }
        /* no items left in HashListBlock, can remove */
        if (!haveEntries) {
            auto tmp = it;
            ++it;
            mBlockList.erase(tmp);
            delete static_cast<HashListBlock*>(tmp);
        } else {
            ++it;
        }
        if (foundIndex) {
            /* item was found and empty HashListBlock was removed */
            return true;
        }
    }

    // item hasn't been present in cache");
    return false;
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
