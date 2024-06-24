
#include <windows.h>
#include <commdlg.h>
#include <stdio.h>

#define IDC_FILENAME 101
#define IDC_BTN_NEW 102
#define IDC_BTN_LOAD 103
#define IDC_BTN_SAVE 104
//gcc -o paint_app paint_app.c -lgdi32 -mwindows
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void SaveBitmap(HWND hwnd, HBITMAP hBitmap, const char *filename);
HBITMAP LoadBitmapFile(HWND hwnd, const char *filename);

HINSTANCE hInst;
HWND hwndTextBox, hwndBtnNew, hwndBtnLoad, hwndBtnSave;
HBITMAP hBitmap = NULL;
POINT ptPrevious = { -1, -1 };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "PaintApp";
    wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 0)); // Fundo amarelo

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, "PaintApp", "Paint Application",
                               WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
                               NULL, NULL, hInstance, NULL);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static BOOL bDrawing = FALSE;
    static HDC hdcMem = NULL;
    static HBITMAP hbmMem = NULL;
    static HBRUSH hbrYellow = NULL;

    switch (uMsg) {
        case WM_CREATE:
            hwndTextBox = CreateWindow("EDIT", "new.bmp",
                                       WS_CHILD | WS_VISIBLE | WS_BORDER,
                                       10, 10, 200, 25,
                                       hwnd, (HMENU)IDC_FILENAME, hInst, NULL);

            hwndBtnNew = CreateWindow("BUTTON", "New",
                                      WS_CHILD | WS_VISIBLE,
                                      220, 10, 80, 30,
                                      hwnd, (HMENU)IDC_BTN_NEW, hInst, NULL);

            hwndBtnLoad = CreateWindow("BUTTON", "Load",
                                       WS_CHILD | WS_VISIBLE,
                                       310, 10, 80, 30,
                                       hwnd, (HMENU)IDC_BTN_LOAD, hInst, NULL);

            hwndBtnSave = CreateWindow("BUTTON", "Save",
                                       WS_CHILD | WS_VISIBLE,
                                       400, 10, 80, 30,
                                       hwnd, (HMENU)IDC_BTN_SAVE, hInst, NULL);
            
            hbrYellow = CreateSolidBrush(RGB(255, 255, 0));
            break;

        case WM_SIZE:
            if (hdcMem) {
                DeleteDC(hdcMem);
                DeleteObject(hbmMem);
            }

            HDC hdc = GetDC(hwnd);
            hdcMem = CreateCompatibleDC(hdc);
            hbmMem = CreateCompatibleBitmap(hdc, LOWORD(lParam), HIWORD(lParam));
            SelectObject(hdcMem, hbmMem);
            ReleaseDC(hwnd, hdc);

            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdcMem, &rect, hbrYellow);

            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case WM_LBUTTONDOWN:
            bDrawing = TRUE;
            ptPrevious.x = LOWORD(lParam);
            ptPrevious.y = HIWORD(lParam);
            break;

        case WM_MOUSEMOVE:
            if (bDrawing) {
                HDC hdc = GetDC(hwnd);
                HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
                SelectObject(hdc, hPen);
                SelectObject(hdcMem, hPen);

                POINT ptCurrent;
                ptCurrent.x = LOWORD(lParam);
                ptCurrent.y = HIWORD(lParam);
                MoveToEx(hdc, ptPrevious.x, ptPrevious.y, NULL);
                LineTo(hdc, ptCurrent.x, ptCurrent.y);
                MoveToEx(hdcMem, ptPrevious.x, ptPrevious.y, NULL);
                LineTo(hdcMem, ptCurrent.x, ptCurrent.y);

                ptPrevious = ptCurrent;
                DeleteObject(hPen);
                ReleaseDC(hwnd, hdc);
            }
            break;

        case WM_LBUTTONUP:
            bDrawing = FALSE;
            ptPrevious.x = -1;
            ptPrevious.y = -1;
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDC_BTN_NEW) {
                RECT rect;
                GetClientRect(hwnd, &rect);
                FillRect(hdcMem, &rect, hbrYellow);
                InvalidateRect(hwnd, NULL, TRUE);
            } else if (LOWORD(wParam) == IDC_BTN_LOAD) {
                char filename[260];
                GetWindowText(hwndTextBox, filename, sizeof(filename));
                hBitmap = LoadBitmapFile(hwnd, filename);
                if (hBitmap) {
                    HDC hdc = GetDC(hwnd);
                    HDC hdcTemp = CreateCompatibleDC(hdc);
                    SelectObject(hdcTemp, hBitmap);
                    BitBlt(hdcMem, 0, 50, 800, 550, hdcTemp, 0, 0, SRCCOPY);
                    DeleteDC(hdcTemp);
                    ReleaseDC(hwnd, hdc);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            } else if (LOWORD(wParam) == IDC_BTN_SAVE) {
                char filename[260];
                GetWindowText(hwndTextBox, filename, sizeof(filename));
                SaveBitmap(hwnd, hbmMem, filename);
            }
            break;

        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                BitBlt(hdc, 0, 50, 800, 550, hdcMem, 0, 50, SRCCOPY);
                EndPaint(hwnd, &ps);
            }
            break;

        case WM_DESTROY:
            if (hbmMem) DeleteObject(hbmMem);
            if (hdcMem) DeleteDC(hdcMem);
            if (hbrYellow) DeleteObject(hbrYellow);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void SaveBitmap(HWND hwnd, HBITMAP hBitmap, const char *filename) {
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = bmp.bmWidth * bmp.bmHeight * 3;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((bmp.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmp.bmHeight;

    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char *lpbitmap = (char *)GlobalLock(hDIB);

    GetDIBits(GetDC(hwnd), hBitmap, 0, (UINT)bmp.bmHeight, lpbitmap, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

    HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfType = 0x4D42; // BM

    DWORD dwBytesWritten;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    CloseHandle(hFile);
}

HBITMAP LoadBitmapFile(HWND hwnd, const char *filename) {
    HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBox(hwnd, "Could not open file.", "Error", MB_OK | MB_ICONERROR);
        return NULL;
    }

    BITMAPFILEHEADER bmfHeader;
    DWORD dwBytesRead;
    ReadFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);

    if (bmfHeader.bfType != 0x4D42) {
        MessageBox(hwnd, "Not a valid bitmap file.", "Error", MB_OK | MB_ICONERROR);
        CloseHandle(hFile);
        return NULL;
    }

    BITMAPINFOHEADER bi;
    ReadFile(hFile, &bi, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);

    HDC hdc = GetDC(hwnd);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, bi.biWidth, bi.biHeight);
    char *lpbitmap = (char *)malloc(bi.biSizeImage);

    ReadFile(hFile, lpbitmap, bi.biSizeImage, &dwBytesRead, NULL);

    SetDIBits(hdc, hBitmap, 0, bi.biHeight, lpbitmap, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

    free(lpbitmap);
    CloseHandle(hFile);
    ReleaseDC(hwnd, hdc);

    return hBitmap;
}
