//
//  bitmap.cpp
//  HI_L3
//
//  Created by Aleksandr Borzikh on 07.11.16.
//  Copyright © 2016 sandyre. All rights reserved.
//

#include "bitmap.hpp"

bool
Bitmap::SaveToFile(const std::string &sFilename)
{
    std::ofstream output(sFilename, std::ios_base::binary);
    if(!output.is_open())
    {
        std::cerr << "FAILED TO OPEN FILE: " << sFilename;
        return false;
    }
    
    output.write((char*)&m_stHeader, 14);
    output.write((char*)&m_stInfoHeader, 40);
    
    for(auto i = 0; i < m_astPixels.size(); ++i)
    {
        if(m_astPixels[i].m_bIsAlphaPresented)
        {
            output.write((char*)&m_astPixels[i].m_aunData, 4);
        }
        else
        {
            output.write((char*)&m_astPixels[i].m_aunData, 3);
        }
    }
    
    output.close();
    return true;
}

void
Bitmap::SetPixelByIndex(size_t iXIndex,
                        size_t iYIndex,
                        const RGBA_Pixel &stPixel)
{
    if(iXIndex >= m_stInfoHeader.nWidth ||
       iYIndex >= m_stInfoHeader.nHeight)
    {
        std::cerr << "Out of bounds index(-es) passed, check bitmap width/height";
        std::cerr << std::endl;
    }
    
    size_t iPixelIndex = iXIndex * m_stInfoHeader.nHeight;
    iPixelIndex += iYIndex;
    
    m_astPixels[iPixelIndex] = stPixel;
}

RGBA_Pixel
Bitmap::GetPixelByIndex(size_t iXIndex,
                        size_t iYIndex) const
{
    if(iXIndex >= m_stInfoHeader.nWidth ||
       iYIndex >= m_stInfoHeader.nHeight)
    {
        std::cerr << "Out of bounds index(-es) passed, check bitmap width/height";
        std::cerr << std::endl;
        return RGBA_Pixel::WHITE();
    }
    
    size_t iPixelIndex = iXIndex * m_stInfoHeader.nHeight;
    iPixelIndex += iYIndex;
    
    return m_astPixels[iPixelIndex];
}

bool
Bitmap::InitWithFile(const std::string &sFilename)
{
    std::ifstream input(sFilename, std::ios_base::binary);
    if(!input.is_open())
    {
        std::cerr << "FAILED TO OPEN FILE: " << sFilename;
        std::cerr << std::endl;
        
        input.close();
        return false;
    }
    
        // headers reading
    input.read((char*)&m_stHeader, 14);
    input.read((char*)&m_stInfoHeader, 40);
    
    if(m_stInfoHeader.unColors != 0 ||
       m_stInfoHeader.unImportantColors != 0)
    {
        std::cerr << "Palletted BMP files arent supported.";
        std::cerr << std::endl;
        
        input.close();
        return false;
    }
    
        // read pixels
    m_astPixels.reserve(m_stInfoHeader.nWidth * m_stInfoHeader.nHeight);
    for(auto i = 0; i < m_stInfoHeader.unImageSizeInBytes; ++i)
    {
        uint32_t unPixelValue;
        input.read((char*)&unPixelValue,
                   m_stInfoHeader.unBitsPerPixel / 8);
        
        m_astPixels.push_back(RGBA_Pixel(m_stInfoHeader.unBitsPerPixel > 24,
                                         unPixelValue));
    }
    
    if(input.eof())
    {
        input.close();
        return true;
    }
    
    std::cerr << "Unknown reading error occured.";
    input.close();
    return false;
}

bool
Bitmap::InitWithRawData(Header stHeader,
                        InfoHeader stInfoHeader,
                        uint8_t * pData)
{
    m_stHeader = stHeader;
    m_stInfoHeader = stInfoHeader;
    
    uint8_t * pCurrentByte = pData;
    bool bHasAlphaChannel = stInfoHeader.unBitsPerPixel > 24;

    if(m_stInfoHeader.unColors != 0 ||
       m_stInfoHeader.unImportantColors != 0)
    {
        std::cerr << "Palletted BMP files arent supported.";
        std::cerr << std::endl;
        
        return false;
    }
    
    for(auto i = 0; i < stInfoHeader.nWidth; ++i)
    {
        for(auto j = 0; j < m_stInfoHeader.nHeight; ++j)
        {
            RGBA_Pixel stPixel(bHasAlphaChannel, 0x0);
            stPixel.SetRedComponent(*pCurrentByte);
            ++pCurrentByte;
            stPixel.SetGreenComponent(*pCurrentByte);
            ++pCurrentByte;
            stPixel.SetBlueComponent(*pCurrentByte);
            ++pCurrentByte;
            if(bHasAlphaChannel)
            {
                stPixel.SetAlphaComponent(*pCurrentByte);
                ++pCurrentByte;
            }
        }
    }
    
    return true;
}

const Bitmap::Header&
Bitmap::GetHeader() const
{
    return m_stHeader;
}

const Bitmap::InfoHeader&
Bitmap::GetInfoHeader() const
{
    return m_stInfoHeader;
}
