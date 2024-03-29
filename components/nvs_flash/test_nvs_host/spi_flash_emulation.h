/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef spi_flash_emulation_h
#define spi_flash_emulation_h

#include <vector>
#include <cassert>
#include <algorithm>
#include <random>
#include "spi_flash_mmap.h"
#include "catch.hpp"

class SpiFlashEmulator;

void spi_flash_emulator_set(SpiFlashEmulator*);

class SpiFlashEmulator
{
public:
    SpiFlashEmulator(size_t sectorCount) : mUpperSectorBound(sectorCount)
    {
        mData.resize(sectorCount * SPI_FLASH_SEC_SIZE / 4, 0xffffffff);
        mEraseCnt.resize(sectorCount);
        spi_flash_emulator_set(this);
    }

    SpiFlashEmulator(const char *filename)
    {
        load(filename);
        // At least one page should be free, hence we create mData of size of 2 sectors.
        mData.resize(mData.size() + SPI_FLASH_SEC_SIZE / 4, 0xffffffff);
        mUpperSectorBound = mData.size() * 4 / SPI_FLASH_SEC_SIZE;
        spi_flash_emulator_set(this);
    }

    ~SpiFlashEmulator()
    {
        spi_flash_emulator_set(nullptr);
    }

    bool read(uint32_t* dest, size_t srcAddr, size_t size) const
    {
        if (srcAddr % 4 != 0 ||
                size % 4 != 0 ||
                srcAddr + size > mData.size() * 4) {
            return false;
        }

        copy(begin(mData) + srcAddr / 4, begin(mData) + (srcAddr + size) / 4, dest);

        ++mReadOps;
        mReadBytes += size;
        mTotalTime += getReadOpTime(static_cast<uint32_t>(size));
        return true;
    }

    bool write(size_t dstAddr, const uint32_t* src, size_t size)
    {
        uint32_t sectorNumber = dstAddr/SPI_FLASH_SEC_SIZE;
        if (sectorNumber < mLowerSectorBound || sectorNumber >= mUpperSectorBound) {
            WARN("invalid flash operation detected: erase sector=" << sectorNumber);
            return false;
        }

        if (dstAddr % 4 != 0 ||
                size % 4 != 0 ||
                dstAddr + size > mData.size() * 4) {
            return false;
        }

        for (size_t i = 0; i < size / 4; ++i) {
            if (mFailCountdown != SIZE_MAX && mFailCountdown-- == 0) {
                return false;
            }

            uint32_t sv = src[i];
            size_t pos = dstAddr / 4 + i;
            uint32_t& dv = mData[pos];

            if (((~dv) & sv) != 0) {   // are we trying to set some 0 bits to 1?
                WARN("invalid flash operation detected: dst=" << dstAddr << " size=" << size << " i=" << i);
                return false;
            }

            dv = sv;
        }
        ++mWriteOps;
        mWriteBytes += size;
        mTotalTime += getWriteOpTime(static_cast<uint32_t>(size));
        return true;
    }

    bool erase(size_t sectorNumber)
    {
        size_t offset = sectorNumber * SPI_FLASH_SEC_SIZE / 4;
        if (offset > mData.size()) {
            return false;
        }

        if (sectorNumber < mLowerSectorBound || sectorNumber >= mUpperSectorBound) {
            WARN("invalid flash operation detected: erase sector=" << sectorNumber);
            return false;
        }

        if (mFailCountdown != SIZE_MAX && mFailCountdown-- == 0) {
            return false;
        }

        std::fill_n(begin(mData) + offset, SPI_FLASH_SEC_SIZE / 4, 0xffffffff);

        ++mEraseOps;
        mEraseCnt[sectorNumber]++;
        mTotalTime += getEraseOpTime();
        return true;
    }

    void randomize(uint32_t seed)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        gen.seed(seed);
        std::generate_n(mData.data(), mData.size(), gen);
    }

    size_t size() const
    {
        return mData.size() * 4;
    }

    const uint32_t* words() const
    {
        return mData.data();
    }

    const uint8_t* bytes() const
    {
        return reinterpret_cast<const uint8_t*>(mData.data());
    }

    void load(const char* filename)
    {
        FILE* f = fopen(filename, "rb");
        fseek(f, 0, SEEK_END);
        off_t size = ftell(f);
        assert(size % SPI_FLASH_SEC_SIZE == 0);
        mData.resize(size / sizeof(uint32_t));
        fseek(f, 0, SEEK_SET);
        auto s = fread(mData.data(), SPI_FLASH_SEC_SIZE, size / SPI_FLASH_SEC_SIZE, f);
        assert(s == static_cast<size_t>(size / SPI_FLASH_SEC_SIZE));
        fclose(f);
    }

    void save(const char* filename)
    {
        FILE* f = fopen(filename, "wb");
        auto n_sectors = mData.size() * sizeof(uint32_t) / SPI_FLASH_SEC_SIZE;
        auto s = fwrite(mData.data(), SPI_FLASH_SEC_SIZE, n_sectors, f);
        assert(s == n_sectors);
        fclose(f);
    }

    void clearStats()
    {
        mReadBytes = 0;
        mWriteBytes = 0;
        mEraseOps = 0;
        mReadOps = 0;
        mWriteOps = 0;
        mTotalTime = 0;
    }

    size_t getReadOps() const
    {
        return mReadOps;
    }
    size_t getWriteOps() const
    {
        return mWriteOps;
    }
    size_t getEraseOps() const
    {
        return mEraseOps;
    }
    size_t getReadBytes() const
    {
        return mReadBytes;
    }
    size_t getWriteBytes() const
    {
        return mWriteBytes;
    }
    size_t getTotalTime() const
    {
        return mTotalTime;
    }

    void setBounds(uint32_t lowerSector, uint32_t upperSector) {
        mLowerSectorBound = lowerSector;
        mUpperSectorBound = upperSector;
    }

    void failAfter(uint32_t count) {
        mFailCountdown = count;
    }

    size_t getSectorEraseCount(uint32_t sector) const {
        return mEraseCnt[sector];
    }

protected:
    static size_t getReadOpTime(uint32_t bytes);
    static size_t getWriteOpTime(uint32_t bytes);
    static size_t getEraseOpTime();


    std::vector<uint32_t> mData;
    std::vector<uint32_t> mEraseCnt;

    mutable size_t mReadOps = 0;
    mutable size_t mWriteOps = 0;
    mutable size_t mReadBytes = 0;
    mutable size_t mWriteBytes = 0;
    mutable size_t mEraseOps = 0;
    mutable size_t mTotalTime = 0;
    size_t mLowerSectorBound = 0;
    size_t mUpperSectorBound = 0;

    size_t mFailCountdown = SIZE_MAX;

};



#endif /* spi_flash_emulation_h */
