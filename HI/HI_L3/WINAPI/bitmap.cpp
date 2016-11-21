#include "bitmap.hpp"

#include <iostream>
#include <fstream>

Bitmap::~Bitmap()
{
    DeleteObject(m_hBITMAP);
    m_hBITMAP = nullptr;
}

Bitmap::Bitmap(HINSTANCE hInst,
               BYTE * pData)
{
    BITMAPFILEHEADER * pBMFH = (BITMAPFILEHEADER*)pData;
    BITMAPINFOHEADER * pBMIH =
        (BITMAPINFOHEADER*)(pData + sizeof(BITMAPFILEHEADER));
    BITMAPINFO * pBMI = (BITMAPINFO*)pBMFH;

    void * pPixels = (void*)(pData + pBMFH->bfOffBits);

    HDC hDC = GetDC(NULL);

    m_hBITMAP = CreateDIBitmap(hDC, pBMIH, CBM_INIT, pPixels, pBMI, DIB_RGB_COLORS);
    if(m_hBITMAP == nullptr)
    {
        std::cout << "Error creating Bitmap from file.\n";
        std::cout << "Error code: " << GetLastError() << std::endl;

        ReleaseDC(NULL, hDC);
        return;
    }

    // Copy metadata into structure
    memcpy(&m_stBITMAPINFO, pBMI, sizeof(BITMAPINFO));
    m_stBITMAPINFO.bmiHeader.biCompression = 0;

    ReleaseDC(NULL, hDC);
}

Bitmap::Bitmap(HINSTANCE hInst,
               const std::wstring& sFilename)
{
    m_hINSTANCE = hInst;
    m_stBITMAPINFO = { 0 };
    m_stBITMAPINFO.bmiHeader.biSize = sizeof(m_stBITMAPINFO.bmiHeader);

    m_hBITMAP = (HBITMAP)LoadImage(m_hINSTANCE,
        sFilename.c_str(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION |
        LR_LOADFROMFILE);

    if(m_hBITMAP == nullptr)
    {
        std::cout << "Error creating Bitmap from file.\n";
        std::cout << "Error code: " << GetLastError() << std::endl;
        return;
    }

    // Fill up BITMAP info
    if(!GetDIBits(GetDC(0), m_hBITMAP,
        0, 0, NULL, &m_stBITMAPINFO, DIB_RGB_COLORS))
    {
        std::cout << "Error reading .bmp metadata.\n";
        std::cout << "Error code: " << GetLastError() << std::endl;
        
        DeleteObject(m_hBITMAP);
        m_hBITMAP = nullptr;
        return;
    }

    m_stBITMAPINFO.bmiHeader.biCompression = 0;
}

HBITMAP
Bitmap::GetHBitmap()
{
    return m_hBITMAP;
}

const BITMAPINFO&
Bitmap::GetBitmapInfo() const
{
    return m_stBITMAPINFO;
}

bool
Bitmap::Save(const std::wstring& sFilename) const
{
    std::ofstream output(sFilename, std::ios_base::binary);
    
    BITMAPFILEHEADER stBMFH;
    stBMFH.bfType = 0x4d42;
    stBMFH.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
        m_stBITMAPINFO.bmiHeader.biSize + 
        m_stBITMAPINFO.bmiHeader.biClrUsed * sizeof(RGBQUAD) + 
        m_stBITMAPINFO.bmiHeader.biSizeImage);
    stBMFH.bfReserved1 = 0;
    stBMFH.bfReserved2 = 0;

    // Compute the offset to the array of color indices.  
    stBMFH.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
        m_stBITMAPINFO.bmiHeader.biSize + 
        m_stBITMAPINFO.bmiHeader.biClrUsed * sizeof(RGBQUAD);

    output.write((char*)&stBMFH, sizeof(BITMAPFILEHEADER));
    output.write((char*)&m_stBITMAPINFO.bmiHeader, sizeof(BITMAPINFOHEADER));

    BITMAP oBitmap;
    GetObject(m_hBITMAP, sizeof(BITMAP), &oBitmap);
    output.write((char*)oBitmap.bmBits, m_stBITMAPINFO.bmiHeader.biSizeImage);

    output.close();
    return true;
}