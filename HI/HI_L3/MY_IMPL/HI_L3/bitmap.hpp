//
//  bitmap.hpp
//  HI_L3
//
//  Created by Aleksandr Borzikh on 07.11.16.
//  Copyright © 2016 sandyre. All rights reserved.
//

#ifndef bitmap_hpp
#define bitmap_hpp

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdint.h>

struct RGBA_Pixel
{
    RGBA_Pixel() : m_aunData { 0 },
                   m_bIsAlphaPresented(false)
    {}
    
    RGBA_Pixel(bool IsAlphaPresented,
               const uint32_t& unData)
    {
        m_bIsAlphaPresented = IsAlphaPresented;
        if(IsAlphaPresented)
        {
            m_aunData[3] = (unData & 0xff000000) >> 24;
            m_aunData[2] = (unData & 0x00ff0000) >> 16;
            m_aunData[1] = (unData & 0x0000ff00) >> 8;
            m_aunData[0] = (unData & 0x000000ff);
        }
        else
        {
            m_aunData[2] = (unData & 0x00ff0000) >> 16;
            m_aunData[1] = (unData & 0x0000ff00) >> 8;
            m_aunData[0] = (unData & 0x000000ff);
        }
    }
    
    static RGBA_Pixel RED()
    {
        return RGBA_Pixel(false,
                          0xff0000);
    }
    static RGBA_Pixel RED_A()
    {
        return RGBA_Pixel(true,
                          0xffff0000);
    }
    
    static RGBA_Pixel GREEN()
    {
        return RGBA_Pixel(false,
                          0x00ff00);
    }
    static RGBA_Pixel GREEN_A()
    {
        return RGBA_Pixel(true,
                          0xff00ff00);
    }
    
    static RGBA_Pixel BLUE()
    {
        return RGBA_Pixel(false,
                          0x0000ff);
    }
    static RGBA_Pixel BLUE_A()
    {
        return RGBA_Pixel(true,
                          0xff0000ff);
    }
    
    static RGBA_Pixel WHITE()
    {
        return RGBA_Pixel(false,
                          0xffffff);
    }
    static RGBA_Pixel WHITE_A()
    {
        return RGBA_Pixel(true,
                          0xffffffff);
    }
    
    static RGBA_Pixel BLACK()
    {
        return RGBA_Pixel(false,
                          0x000000);
    }
    static RGBA_Pixel BLACK_A()
    {
        return RGBA_Pixel(true,
                          0xff000000);
    }
    
    uint8_t     GetRedComponent() const
    {
        return m_aunData[2];
    }
    void        SetRedComponent(uint8_t nValue)
    {
        m_aunData[2] = nValue;
    }
    
    uint8_t     GetGreenComponent() const
    {
        return m_aunData[1];
    }
    void        SetGreenComponent(uint8_t nValue)
    {
        m_aunData[1] = nValue;
    }
    
    uint8_t     GetBlueComponent() const
    {
        return m_aunData[0];
    }
    void        SetBlueComponent(uint8_t nValue)
    {
        m_aunData[0] = nValue;
    }
    
    uint8_t     GetAlphaComponent() const
    {
        return m_aunData[3];
    }
    void        SetAlphaComponent(uint8_t nValue)
    {
        m_aunData[3] = nValue;
    }
    
    bool     m_bIsAlphaPresented;
    uint8_t  m_aunData[4];
};

class Bitmap
{
public:
#pragma pack(push, 1)
    typedef struct header
    {
        header() :  unType(0),
        unSizeInBytes(0),
        unReserved1(0),
        unReserved2(0),
        unImageDataOffset(0)
        {}
        
        uint16_t    unType;
        uint32_t    unSizeInBytes;
        uint32_t    unReserved1, unReserved2;
        uint32_t    unImageDataOffset;
    } Header;
#pragma pack(pop)
    
#pragma pack(push, 1)
    typedef struct info_header
    {
        info_header() : unHeaderSize(0),
        nWidth(0),
        nHeight(0),
        unPlanes(0),
        unBitsPerPixel(0),
        unCompressionType(0),
        unImageSizeInBytes(0),
        nXResolution(0),
        nYResolution(0),
        unColors(0),
        unImportantColors(0)
        {}
        
        uint32_t    unHeaderSize;
        int32_t     nWidth, nHeight;
        uint16_t    unPlanes;
        uint16_t    unBitsPerPixel;
        uint32_t    unCompressionType;
        uint32_t    unImageSizeInBytes;
        int32_t     nXResolution, nYResolution;
        uint32_t    unColors;
        uint32_t    unImportantColors;
    } InfoHeader;
#pragma pack(pop)
    
public:
    bool        InitWithFile(const std::string& sFilename);
    bool        InitWithRawData(Header,
                                InfoHeader,
                                uint8_t*);
    bool        SaveToFile(const std::string& sFilename);
    
    void        SetPixelByIndex(size_t iXIndex,
                                size_t iYIndex,
                                const RGBA_Pixel& stPixel);
    RGBA_Pixel  GetPixelByIndex(size_t iXIndex,
                                size_t iYIndex) const;
    
    const Header&       GetHeader() const;
    const InfoHeader&   GetInfoHeader() const;
    
protected:
    Header                    m_stHeader;
    InfoHeader                m_stInfoHeader;
    std::vector< RGBA_Pixel > m_astPixels;
};

#endif /* bitmap_hpp */
