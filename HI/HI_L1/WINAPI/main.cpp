#include <Windows.h>
#include <array>
#include <chrono>
#include <iostream>
#include <fstream>

HINSTANCE g_hInst  = nullptr;
HBITMAP   g_hBitmap = nullptr;
unsigned int g_dBitmapX = 0;
unsigned int g_dBitmapY = 0;
bool         g_bFileSaved = false;

const std::array<std::wstring, 10> Files
{
    L"C:\\Users\\sandyre\\Desktop\\HumanInterfaces\\LAB1_1\\LAB1\\Picture1.bmp",
    L"C:\\Users\\sandyre\\Desktop\\HumanInterfaces\\LAB1_1\\LAB1\\Picture2.bmp",
    L"C:\\Users\\sandyre\\Desktop\\HumanInterfaces\\LAB1_1\\LAB1\\Picture3.bmp",
    L"C:\\Users\\sandyre\\Desktop\\HumanInterfaces\\LAB1_1\\LAB1\\Picture4.bmp",
    L"C:\\Users\\sandyre\\Desktop\\HumanInterfaces\\LAB1_1\\LAB1\\Picture5.bmp",
    L"C:\\Users\\sandyre\\Desktop\\HumanInterfaces\\LAB1_1\\LAB1\\Picture6.bmp",
    L"C:\\Users\\sandyre\\Desktop\\HumanInterfaces\\LAB1_1\\LAB1\\Picture7.bmp",
    L"C:\\Users\\sandyre\\Desktop\\HumanInterfaces\\LAB1_1\\LAB1\\Picture8.bmp",
    L"C:\\Users\\sandyre\\Desktop\\HumanInterfaces\\LAB1_1\\LAB1\\Picture9.bmp",
    L"C:\\Users\\sandyre\\Desktop\\HumanInterfaces\\LAB1_1\\LAB1\\Picture10.bmp",
};

bool HDCToFile(const char* FilePath, HDC Context, RECT Area, uint16_t BitsPerPixel = 24)
{
    uint32_t Width = Area.right - Area.left;
    uint32_t Height = Area.bottom - Area.top;

    BITMAPINFO Info;
    BITMAPFILEHEADER Header;
    memset(&Info, 0, sizeof(Info));
    memset(&Header, 0, sizeof(Header));
    Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    Info.bmiHeader.biWidth = Width;
    Info.bmiHeader.biHeight = Height;
    Info.bmiHeader.biPlanes = 1;
    Info.bmiHeader.biBitCount = BitsPerPixel;
    Info.bmiHeader.biCompression = BI_RGB;
    Info.bmiHeader.biSizeImage = Width * Height * (BitsPerPixel > 24 ? 4 : 3);
    Header.bfType = 0x4D42;
    Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);


    char* Pixels = NULL;
    HDC MemDC = CreateCompatibleDC(Context);
    HBITMAP Section = CreateDIBSection(Context, &Info, DIB_RGB_COLORS, (void**)&Pixels, 0, 0);
    DeleteObject(SelectObject(MemDC, Section));
    BitBlt(MemDC, 0, 0, Width, Height, Context, Area.left, Area.top, SRCCOPY);
    DeleteDC(MemDC);

    std::fstream hFile(FilePath, std::ios::out | std::ios::binary);
    if(hFile.is_open())
    {
        hFile.write((char*)&Header, sizeof(Header));
        hFile.write((char*)&Info.bmiHeader, sizeof(Info.bmiHeader));
        hFile.write(Pixels, (((BitsPerPixel * Width + 31) & ~31) / 8) * Height);
        hFile.close();
        DeleteObject(Section);
        return true;
    }

    DeleteObject(Section);
    return false;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	case WM_PAINT:
	{
        PAINTSTRUCT ps;
        HBRUSH brush = CreateSolidBrush(0x00ffff00);
        HDC hdc = BeginPaint(hwnd, &ps);
        HDC memDC = CreateCompatibleDC(hdc);

        size_t bitblt = 0, pixel_man = 0;
        // bitblt
        auto start = std::chrono::high_resolution_clock::now();
        SelectObject(hdc, brush);
        SelectObject(memDC, g_hBitmap);
        BitBlt(hdc, 0, 0, g_dBitmapX, g_dBitmapY, memDC, 0, 0, MERGECOPY);
        auto end = std::chrono::high_resolution_clock::now();
        bitblt = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        // pixel manipulation
        start = std::chrono::high_resolution_clock::now();
        brush = CreateSolidBrush(0x00ffffff);
        SelectObject(hdc, brush);
        BitBlt(hdc, 0, 0, g_dBitmapX, g_dBitmapY, memDC, 0, 0, MERGECOPY);
        for(size_t i = 0; i < g_dBitmapX; ++i)
        {
            for(size_t j = 0; j < g_dBitmapY; ++j)
            {
                COLORREF pixel_val = GetPixel(hdc, i, j);
                SetPixel(hdc, i, j, RGB(0, GetGValue(pixel_val), GetBValue(pixel_val)));
            }
        }
        end = std::chrono::high_resolution_clock::now();
        pixel_man = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        if(!g_bFileSaved)
        {
            HDCToFile("C:\\Users\\sandyre\\Desktop\\kek.bmp", hdc, { 0, 0, (long)g_dBitmapX, (long)g_dBitmapY });
            g_bFileSaved = true;
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
	g_hInst = hInstance;

    g_hBitmap = (HBITMAP)LoadImage(g_hInst, Files[5].c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    BITMAP oBitmap;
    GetObject(g_hBitmap, sizeof(BITMAP), &oBitmap);
    
    g_dBitmapX = oBitmap.bmWidth;
    g_dBitmapY = oBitmap.bmHeight;

	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;

	wc.cbSize = sizeof WNDCLASSEX;
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"HI_L1 V2";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"FAILED", L"ERROR", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		L"HI_L1 V2",
		L"HI_L1 V2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		g_dBitmapX + 50,
        g_dBitmapY + 50,
		NULL, NULL, g_hInst, NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, L"FAILED", L"ERROR", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}