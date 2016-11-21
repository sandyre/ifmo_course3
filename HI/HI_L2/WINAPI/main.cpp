#include <Windows.h>
#include <iostream>
#include "resource.h"

// global vars
bool bIsSaved = false;
HBITMAP hBITMAP;
BITMAPINFO stBitmapInfo;
HINSTANCE hInst;

int CreateBMPFile(HWND hwnd, 
    LPCTSTR pszFile, 
    PBITMAPINFO pbi,
    HBITMAP hBMP,
    HDC hDC)
{
    HANDLE hf;                  // file handle  
    BITMAPFILEHEADER hdr;       // bitmap file-header  
    PBITMAPINFOHEADER pbih;     // bitmap info-header  
                                // LPBYTE lpBits
    DWORD dwTotal;              // total count of bytes  
    DWORD cb;                   // incremental count of bytes  
    BYTE *hp;                   // byte pointer  
    DWORD dwTmp;
    int ret = 0;

    if(pbi == NULL)
    {
        return ret;
    }
    pbih = (PBITMAPINFOHEADER)pbi;

    RGBQUAD *rgbq;
    rgbq = pbi->bmiColors;
    PALETTEENTRY pe[256];
    GetSystemPaletteEntries(hDC, 0, pbih->biClrUsed, pe);
    for(DWORD i = 0; i < pbih->biClrUsed; i++)
    {
        rgbq[i].rgbRed = pe[i].peRed;
        rgbq[i].rgbBlue = pe[i].peBlue;
        rgbq[i].rgbGreen = pe[i].peGreen;
        rgbq[i].rgbReserved = 0;
    }

    // CE5.0 + CE6.0
    HDC tHDC;
    tHDC = CreateCompatibleDC(hDC);
    HBITMAP h = CreateDIBSection(hDC, pbi, DIB_PAL_COLORS, (void **)&hp, NULL, 0);
    if(h == NULL)
    {
        goto close_bmp;
    }
    SelectObject(tHDC, h);
    BitBlt(tHDC, 0, 0, stBitmapInfo.bmiHeader.biWidth, stBitmapInfo.bmiHeader.biHeight, hDC, 0, 0, SRCCOPY);

    // Create the .BMP file.  
    hf = CreateFile(pszFile,
        GENERIC_READ | GENERIC_WRITE,
        (DWORD)0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        (HANDLE)NULL);
    if(hf == INVALID_HANDLE_VALUE)
    {
        std::cout << "Error occured, code: INVALID_HANDLE_VALUE" << std::endl;
        goto close_bmp;
    }
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
                                // Compute the size of the entire file.  
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    if(!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
        (LPDWORD)&dwTmp, NULL))
    {
        goto close_bmp;
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    if(!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
        + pbih->biClrUsed * sizeof(RGBQUAD),
        (LPDWORD)&dwTmp, (NULL)))
    {
        std::cout << "Error occured, code: " << GetLastError() << std::endl;
    }

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage;

    //hp = lpBits;     
    if(!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL))
    {
        std::cout << "Error occured, code: " << GetLastError() << std::endl;
        goto close_bmp;
    }

close_bmp:
    // Close the .BMP file.  
    if(hf != INVALID_HANDLE_VALUE)
    {
        if(!CloseHandle(hf))
        {
            std::cout << "Error occured, code: " << GetLastError() << std::endl;
            return 2;
        }
        else
        {
            ret = 1;
        }
    }

    // Free memory.  
    if(tHDC != NULL)
        DeleteObject(tHDC);
    if(h != NULL)
        DeleteObject(h);

    return ret;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        HDC memDC = CreateCompatibleDC(hdc);

        SelectObject(memDC, hBITMAP);
        BitBlt(hdc, 0, 0, stBitmapInfo.bmiHeader.biWidth,
            stBitmapInfo.bmiHeader.biHeight, memDC, 0, 0, SRCCOPY);

        if(!bIsSaved)
        {
            CreateBMPFile(hwnd, L"c:\\Users\\sandyre\\Desktop\\resulted.bmp", &stBitmapInfo,
                hBITMAP, hdc);
        }

        DeleteDC(memDC);
        EndPaint(hwnd, &ps);
    }
    break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(NULL);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    hInst = hInstance;

    HDC hDC = GetDC(0);
    hBITMAP = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0,
        NULL);

    stBitmapInfo.bmiHeader.biSize = sizeof(stBitmapInfo.bmiHeader);
    if(!GetDIBits(hDC, hBITMAP, 0, 0, NULL, &stBitmapInfo, DIB_RGB_COLORS))
    {
        std::cout << "Error occured, code: " << GetLastError() << std::endl;
        return 1;
    }


    BYTE * lpPixels = new BYTE[stBitmapInfo.bmiHeader.biSizeImage];
    stBitmapInfo.bmiHeader.biCompression = BI_RGB;

    if(!GetDIBits(hDC, hBITMAP, 0, stBitmapInfo.bmiHeader.biHeight,
        LPVOID(lpPixels), &stBitmapInfo, DIB_RGB_COLORS))
    {
        std::cout << "Error occured, code: " << GetLastError() << std::endl;
        return 2;
    }

    unsigned char cTMP = 0;
    BYTE * pPixelAddress = lpPixels;
    for(auto i = 0; i < stBitmapInfo.bmiHeader.biHeight; ++i)
    {
        for(auto j = 0; j < stBitmapInfo.bmiHeader.biWidth; ++j)
        {
            cTMP = pPixelAddress[0];
            pPixelAddress[0] = pPixelAddress[1];
            pPixelAddress[1] = cTMP;

            pPixelAddress += (stBitmapInfo.bmiHeader.biBitCount / 8);
        }
    }

    int hResult = SetDIBits(hDC, hBITMAP, 0, stBitmapInfo.bmiHeader.biHeight, (LPVOID)lpPixels, &stBitmapInfo, DIB_RGB_COLORS);
    if(!hResult)
    {
        std::cout << "Error occured, code: " << GetLastError() << std::endl;
        return 2;
    }
    delete[] lpPixels;

    // save to file

    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    wc.cbSize = sizeof WNDCLASSEX;
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
    wc.lpszMenuName = 0;
    wc.lpszClassName = L"HI_L2 V2";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"FAILED", L"ERROR", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"HI_L2 V2",
        L"HI_L2 V2",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        stBitmapInfo.bmiHeader.biWidth + 50, stBitmapInfo.bmiHeader.biHeight + 50,
        NULL, NULL, hInst, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, L"FAILED", L"ERROR", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while(GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}