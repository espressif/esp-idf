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
#include "nvs_storage.hpp"

namespace nvs
{

Storage::~Storage()
{
    clearNamespaces();
}

void Storage::clearNamespaces()
{
    for (auto it = std::begin(mNamespaces); it != std::end(mNamespaces); ) {
        auto tmp = it;
        ++it;
        mNamespaces.erase(tmp);
        delete static_cast<NamespaceEntry*>(tmp);
    }
}

esp_err_t Storage::init(uint32_t baseSector, uint32_t sectorCount)
{
    auto err = mPageManager.load(baseSector, sectorCount);
    if (err != ESP_OK) {
        mState = StorageState::INVALID;
        return err;
    }

    clearNamespaces();
    std::fill_n(mNamespaceUsage.data(), mNamespaceUsage.byteSize() / 4, 0);
    for (auto it = mPageManager.begin(); it != mPageManager.end(); ++it) {
        Page& p = *it;
        size_t itemIndex = 0;
        Item item;
        while(p.findItem(Page::NS_INDEX, ItemType::U8, nullptr, itemIndex, item) == ESP_OK) {
            NamespaceEntry* entry = new NamespaceEntry;
            item.getKey(entry->mName, sizeof(entry->mName) - 1);
            item.getValue(entry->mIndex);
            mNamespaces.push_back(entry);
            mNamespaceUsage.set(entry->mIndex, true);
        }
    }
    mNamespaceUsage.set(0, true);
    mNamespaceUsage.set(255, true);
    mState = StorageState::ACTIVE;
    return ESP_OK;
}

esp_err_t Storage::writeItem(uint8_t nsIndex, ItemType datatype, const char* key, const void* data, size_t dataSize)
{
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    Page* findPage = nullptr;
    Item item;
    auto err = findItem(nsIndex, datatype, key, findPage, item);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        return err;
    }

    Page& page = getCurrentPage();
    err = page.writeItem(nsIndex, datatype, key, data, dataSize);
    if (err == ESP_ERR_NVS_PAGE_FULL) {
        page.markFull();
        err = mPageManager.requestNewPage();
        if (err != ESP_OK) {
            return err;
        }

        err = getCurrentPage().writeItem(nsIndex, datatype, key, data, dataSize);
        if (err != ESP_OK) {
            return err;
        }
    }

    if (findPage) {
        if (findPage->state() == Page::PageState::UNINITIALIZED) {
            auto err = findItem(nsIndex, datatype, key, findPage, item);
            assert(err == ESP_OK);
        }
        err = findPage->eraseItem(nsIndex, datatype, key);
        if (err != ESP_OK) {
            return err;
        }
    }

    return ESP_OK;
}

esp_err_t Storage::createOrOpenNamespace(const char* nsName, bool canCreate, uint8_t& nsIndex)
{
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }
    auto it = std::find_if(mNamespaces.begin(), mNamespaces.end(), [=] (const NamespaceEntry& e) -> bool {
        return strncmp(nsName, e.mName, sizeof(e.mName) - 1) == 0;
    });
    if (it == std::end(mNamespaces)) {
        if (!canCreate) {
            return ESP_ERR_NVS_NOT_FOUND;
        }

        uint8_t ns;
        for (ns = 1; ns < 255; ++ns) {
            if (mNamespaceUsage.get(ns) == false) {
                break;
            }
        }

        if (ns == 255) {
            return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
        }

        auto err = writeItem(Page::NS_INDEX, ItemType::U8, nsName, &ns, sizeof(ns));
        if (err != ESP_OK) {
            return err;
        }
        mNamespaceUsage.set(ns, true);
        nsIndex = ns;
        
        NamespaceEntry* entry = new NamespaceEntry;
        entry->mIndex = ns;
        strlcpy(entry->mName, nsName, sizeof(entry->mName));
        mNamespaces.push_back(entry);

    } else {
        nsIndex = it->mIndex;
    }
    return ESP_OK;
}

esp_err_t Storage::readItem(uint8_t nsIndex, ItemType datatype, const char* key, void* data, size_t dataSize)
{
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    Item item;
    Page* findPage = nullptr;
    auto err = findItem(nsIndex, datatype, key, findPage, item);
    if (err != ESP_OK) {
        return err;
    }

    return findPage->readItem(nsIndex, datatype, key, data, dataSize);
}

esp_err_t Storage::eraseItem(uint8_t nsIndex, ItemType datatype, const char* key)
{
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    Item item;
    Page* findPage = nullptr;
    auto err = findItem(nsIndex, datatype, key, findPage, item);
    if (err != ESP_OK) {
        return err;
    }

    return findPage->eraseItem(nsIndex, datatype, key);
}

esp_err_t Storage::getItemDataSize(uint8_t nsIndex, ItemType datatype, const char* key, size_t& dataSize)
{
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    Item item;
    Page* findPage = nullptr;
    auto err = findItem(nsIndex, datatype, key, findPage, item);
    if (err != ESP_OK) {
        return err;
    }

    dataSize = item.varLength.dataSize;
    return ESP_OK;
}

}