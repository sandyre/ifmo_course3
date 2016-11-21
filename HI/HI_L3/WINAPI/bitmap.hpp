#ifndef __BITMAP_HPP__
#define __BITMAP_HPP__

#include <Windows.h>
#include <string>

class Bitmap
{
public:
    Bitmap(HINSTANCE, const std::wstring&);
    Bitmap(HINSTANCE, BYTE*);
    ~Bitmap();

    const BITMAPINFO& GetBitmapInfo() const;
    HBITMAP GetHBitmap();
    bool    Save(const std::wstring&) const;
private:
    BITMAPINFO m_stBITMAPINFO;
    HBITMAP m_hBITMAP;

    HINSTANCE m_hINSTANCE;
};

#endif