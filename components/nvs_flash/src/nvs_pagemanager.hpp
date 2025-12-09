/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <memory>
#include <list>
#include "nvs_types.hpp"
#include "nvs_page.hpp"
#include "partition.hpp"
#include "intrusive_list.h"

namespace nvs
{
class PageManager
{
    using TPageList = intrusive_list<Page>;
    using TPageListIterator = TPageList::iterator;
public:

    PageManager() {}

    esp_err_t load(Partition *partition, uint32_t baseSector, uint32_t sectorCount);

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

    uint32_t getPageCount() {
        return mPageCount;
    }

    esp_err_t requestNewPage();

    esp_err_t fillStats(nvs_stats_t& nvsStats);

    uint32_t getBaseSector()
    {
        return mBaseSector;
    }

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
