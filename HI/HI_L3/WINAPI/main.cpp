#include <Windows.h>
#include "bitmap.hpp"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
HDC hdc;
Bitmap * pBITMAP;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    std::wstring filename = L"c:\\Users\\sandyre\\Desktop\\resulted.bmp";
    pBITMAP = new Bitmap(hInstance, filename);
    filename = L"c:\\Users\\sandyre\\Desktop\\resulted_moded.bmp";
    pBITMAP->Save(filename);

    WNDCLASS wc = { 0 };
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"HI_L3";
    RegisterClass(&wc);

    HWND hWnd = CreateWindow(L"HI_L3", L"HI_L3",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 320, 420,
        NULL, NULL, hInstance, NULL);

    hdc = GetDC(hWnd);
    ShowWindow(hWnd, nCmdShow);

    SetTimer(hWnd, 1, USER_TIMER_MINIMUM, NULL);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HDC memDC = CreateCompatibleDC(hdc);

        SelectObject(memDC, pBITMAP->GetHBitmap());
        BitBlt(hdc, 0, 0, 
               pBITMAP->GetBitmapInfo().bmiHeader.biWidth,
               pBITMAP->GetBitmapInfo().bmiHeader.biHeight,
               memDC, 0, 0,
               SRCCOPY);

        DeleteDC(memDC);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }

    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}