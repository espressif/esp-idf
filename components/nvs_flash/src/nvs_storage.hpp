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
#ifndef nvs_storage_hpp
#define nvs_storage_hpp

#include <memory>
#include <string>
#include <unordered_map>
#include "nvs.hpp"
#include "nvs_types.hpp"
#include "nvs_page.hpp"
#include "nvs_pagemanager.hpp"

//extern void dumpBytes(const uint8_t* data, size_t count);

namespace nvs
{

class Storage : public intrusive_list_node<Storage>
{
    enum class StorageState : uint32_t {
        INVALID,
        ACTIVE,
    };

    struct NamespaceEntry : public intrusive_list_node<NamespaceEntry> {
    public:
        char mName[Item::MAX_KEY_LENGTH + 1];
        uint8_t mIndex;
    };

    typedef intrusive_list<NamespaceEntry> TNamespaces;

public:
    ~Storage();

    Storage(const char *pName = NVS_DEFAULT_PART_NAME) : mPartitionName(pName) { };

    esp_err_t init(uint32_t baseSector, uint32_t sectorCount);

    bool isValid() const;

    esp_err_t createOrOpenNamespace(const char* nsName, bool canCreate, uint8_t& nsIndex);

    esp_err_t writeItem(uint8_t nsIndex, ItemType datatype, const char* key, const void* data, size_t dataSize);

    esp_err_t readItem(uint8_t nsIndex, ItemType datatype, const char* key, void* data, size_t dataSize);

    esp_err_t getItemDataSize(uint8_t nsIndex, ItemType datatype, const char* key, size_t& dataSize);

    esp_err_t eraseItem(uint8_t nsIndex, ItemType datatype, const char* key);

    template<typename T>
    esp_err_t writeItem(uint8_t nsIndex, const char* key, const T& value)
    {
        return writeItem(nsIndex, itemTypeOf(value), key, &value, sizeof(value));
    }

    template<typename T>
    esp_err_t readItem(uint8_t nsIndex, const char* key, T& value)
    {
        return readItem(nsIndex, itemTypeOf(value), key, &value, sizeof(value));
    }

    esp_err_t eraseItem(uint8_t nsIndex, const char* key)
    {
        return eraseItem(nsIndex, ItemType::ANY, key);
    }
    
    esp_err_t eraseNamespace(uint8_t nsIndex);

    const char *getPartName() const
    {
        return mPartitionName;
    }

    void debugDump();
    
    void debugCheck();


protected:

    Page& getCurrentPage()
    {
        return mPageManager.back();
    }

    void clearNamespaces();

    esp_err_t findItem(uint8_t nsIndex, ItemType datatype, const char* key, Page* &page, Item& item);

protected:
    const char *mPartitionName;
    size_t mPageCount;
    PageManager mPageManager;
    TNamespaces mNamespaces;
    CompressedEnumTable<bool, 1, 256> mNamespaceUsage;
    StorageState mState = StorageState::INVALID;
};

} // namespace nvs



#endif /* nvs_storage_hpp */
