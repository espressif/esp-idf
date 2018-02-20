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
#ifndef nvs_pagemanager_hpp
#define nvs_pagemanager_hpp

#include <memory>
#include <list>
#include "nvs_types.hpp"
#include "nvs_page.hpp"
#include "nvs_pagemanager.hpp"
#include "intrusive_list.h"

namespace nvs
{
class PageManager
{
    using TPageList = intrusive_list<Page>;
    using TPageListIterator = TPageList::iterator;
public:

    PageManager() {}

    esp_err_t load(uint32_t baseSector, uint32_t sectorCount);

    TPageListIterator begin()
    {
        return mPageList.begin();
    }

    TPageListIterator end()
    {
        return mPageList.end();
    }

    Page& back()
    {
        return mPageList.back();
    }

    esp_err_t requestNewPage();

    esp_err_t fillStats(nvs_stats_t& nvsStats);

protected:
    friend class Iterator;

    esp_err_t activatePage();

    TPageList mPageList;
    TPageList mFreePageList;
    std::unique_ptr<Page[]> mPages;
    uint32_t mBaseSector;
    uint32_t mPageCount;
    uint32_t mSeqNumber;
}; // class PageManager


} // namespace nvs


#endif /* nvs_pagemanager_hpp */
