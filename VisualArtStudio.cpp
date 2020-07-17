#include "framework.h"
#include "Visual ArtStudio.h"
#include "resource.h"
#include <windowsx.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <ShObjIdl.h>
#include <commdlg.h>
#include <commctrl.h>
#include <objidl.h>
#include <gdiplus.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#define MAX_LOADSTRING 64
#define RGB2BGR(a_ulColor) (a_ulColor & 0xFF000000) | ((a_ulColor & 0xFF0000) >> 16) | (a_ulColor & 0x00FF00) | ((a_ulColor & 0x0000FF) << 16)

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HBRUSH penBrush;
HBITMAP hImg;
HWND sticker1, fsa, objectTree, toolbarHandle;
HPEN penDCPen;
HDC hdc, mdc, sdc;
LOGBRUSH hatchBrush;
char filestring[FILESTRINGBUFFER];
LineCap lineCap;
wchar_t imageFileField[FILESTRINGBUFFER];
extern wchar_t currentBrushTexture[64];
CHOOSECOLOR chooseColor;
HWND hWnd;
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK Controls(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ColorPallette(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK MainToolbar(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK GradientTool(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK UtensilSuite(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK IntroDialog(HWND, UINT, WPARAM, LPARAM);

void initMemBitmap(HDC hdc);
void pushvscr(void);
extern void mylmousemove(int x, int y);
extern void myrmousemove(int x, int y, HDC hdc);
extern void mylbutton(int x, int y, HDC hdc);
extern void mymousemove(int x, int y, HDC hdc);
extern void mylbuttonup(int x, int y, HDC hdc);


VOID initChooseColorStruct(void) {
    COLORREF colorArray[16];
    colorArray[0] = RGB(0, 0, 0);
    colorArray[1] = RGB(255, 0, 0);
    colorArray[2] = RGB(0, 0, 255);
    colorArray[3] = RGB(0, 255, 0);
    chooseColor.lStructSize = sizeof(CHOOSECOLOR);
    chooseColor.hwndOwner = hWnd;
    chooseColor.hInstance = 0;
    chooseColor.rgbResult = 0;
    chooseColor.lpCustColors = colorArray;
    chooseColor.Flags = CC_FULLOPEN | CC_RGBINIT;
    chooseColor.lCustData = NULL;
    chooseColor.lpTemplateName = 0;
    chooseColor.lpfnHook = 0;
    ChooseColor(&chooseColor);
    penColor = chooseColor.rgbResult;

}
VOID UploadGlobalImage(HDC hdc, int x, int y)
{
    Graphics graphics(hdc);
    Image image(imageFileField);
    graphics.DrawImage(&image, x, y, image.GetWidth(), image.GetHeight());
    pushvscr();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    wsprintf(currentBrushTexture, L"BrushTexture11.png");
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VISUALARTSTUDIO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }
    MSG msg;
    hdc = GetDC(hWnd);
    sdc = hdc;
    initClearPen();
    initMemBitmap(hdc);

    CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, MainToolbar);
    CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG7), hWnd, UtensilSuite);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    GdiplusShutdown(gdiplusToken);
    ReleaseDC(hWnd, hdc);
    return msg.wParam;
}

void initMemBitmap(HDC hdc) {
    int k;
    HBITMAP hbmapm;
    BITMAP bma;
    BITMAPINFO bmi;
    HDC memdc;
    unsigned long* lp;

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = winsizew;
    bmi.bmiHeader.biHeight = -(winsizeh);
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    hbmapm = CreateDIBSection(0, &bmi, DIB_RGB_COLORS, (void**)&lp, 0, 0);

    memdc = CreateCompatibleDC(0);
    SelectObject(memdc, hbmapm);
    GetObject(hbmapm, sizeof(BITMAP), &bma);
    for (k = 0; k < winsizeh * winsizew; k++)
        lp[k] = WHITE;

    BitBlt(hdc, 0, 0, winsizew, winsizeh, memdc, 0, 0, SRCCOPY);

    mdc = memdc;
    vscrmemp = lp;
}


VOID OnPaint(HDC hdc) {
    Graphics graphics(hdc);
}

PWSTR fileOpenFunction(void) {
    wchar_t tmp[] = L"Temp";
    PWSTR returnFileName = tmp;
    int succeeded = 0;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            hr = pFileOpen->Show(NULL);
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr)) {
                        succeeded = 1;
                        returnFileName = pszFilePath;
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return(returnFileName);
}

PWSTR fileSaveAsFunction(void) {
    wchar_t tmp[] = L"Temp";
    LPCWSTR paintExt;
    paintExt = L"pnt";
    PWSTR returnFileName = tmp;
    int succeeded = 0;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileSaveDialog* pFileSaveAs;
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSaveAs));
        if (SUCCEEDED(hr)) {
            pFileSaveAs->SetDefaultExtension(paintExt);
            pFileSaveAs->SetTitle(L"Save painting as...");
            hr = pFileSaveAs->Show(NULL);
            if (SUCCEEDED(hr)) {
                IShellItem* pItem;
                hr = pFileSaveAs->GetResult(&pItem);
                if (SUCCEEDED(hr)) {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr)) {
                        returnFileName = pszFilePath;
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileSaveAs->Release();
        }
        CoUninitialize();

    }
    return (returnFileName);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VISUALARTSTUDIO));
    wcex.hCursor = LoadCursor(NULL, IDC_HAND);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VISUALARTSTUDIO);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_SIZEBOX | WS_MINIMIZEBOX | WS_DLGFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, winsizew, winsizeh, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    return TRUE;
}

void DrawRect(HDC hdc, int left, int top, int right, int bottom, int state) {
    RECT region;
    region.left = left;
    region.right = right;
    region.top = top;
    region.bottom = bottom;
    SelectObject(hdc, GetStockObject(DC_PEN));
    if (state == 1)
        SelectPen(hdc, CreatePen(BS_SOLID, 2, BLACK));
    else
        SelectPen(hdc, CreatePen(BS_SOLID, 2, 0xf0f0f0));
    MoveToEx(hdc, region.left, region.top, NULL);
    LineTo(hdc, region.right, region.top);
    MoveToEx(hdc, region.right, region.top, NULL);
    LineTo(hdc, region.right, region.bottom);
    MoveToEx(hdc, region.right, region.bottom, NULL);
    LineTo(hdc, region.left, region.bottom);
    MoveToEx(hdc, region.left, region.bottom, NULL);
    LineTo(hdc, region.left, region.top);
    MoveToEx(hdc, region.left, region.top, NULL);
}

void pushvscr(void) {
    BitBlt(sdc, 100, 0, winsizew - 275, winsizeh, mdc, 100, 0, SRCCOPY);
}

void setPenAndBrush(int color) {
    hatchBrush.lbHatch = brushHatch;
    hatchBrush.lbColor = color;
    hatchBrush.lbStyle = brushState;
    SelectObject(mdc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(mdc, color);
    penBrush = CreateSolidBrush(GetDCBrushColor(mdc));
    penColor = color;
    SelectObject(mdc, GetStockObject(DC_PEN));
    penDCPen = ExtCreatePen(PS_GEOMETRIC | thickness, thickness2, &hatchBrush, PS_ENDCAP_ROUND, PS_JOIN_ROUND);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    FILE* fp;
    unsigned long* tmpl;
    tmpl = vscrmemp;
    char fileField[FILESTRINGBUFFER];
    Graphics graphics(mdc);
    Image newImage(L"Blank");
    wsprintfA(fileField, "\0");
    switch (message)
    {
    case WM_CREATE:
        return (0);
    case WM_COMMAND:
    {
        switch (LOWORD(wParam)) {

        case ID_FILE_SAVEFILE:
            wsprintfA(filestring, "\0");
            sprintf(filestring, "%ws", fileSaveAsFunction());
            fp = fopen(filestring, "w+");
            if (fp == NULL) {
                switch (MessageBoxA(hWnd, "Error: No painting found with given file name.", "File not found", MB_ICONERROR | MB_OK)) {
                case IDOK:
                    return (1);
                }
            }
            for (int i = 0; i < winsizew; i++) {
                for (int i2 = 0; i2 < winsizeh; i2++) {
                    fprintf(fp, " %d ", vscrmemp[i + winsizew * i2]);
                }
                fprintf(fp, "\n");
            }
            fclose(fp);
            switch (MessageBoxA(hWnd, "Painting saved!", "Saved", MB_ICONINFORMATION | MB_OK)) {
            case IDOK:
                break;
            }
            break;
        case ID_FILE_UPLOADFILE:
            wsprintfA(filestring, "\0");
            sprintf(filestring, "%ws", fileOpenFunction());
            fp = fopen(filestring, "r+");
            if (fp == NULL) {
                switch (MessageBoxA(hWnd, "Error: No painting found with given file name.", "File not found", MB_ICONERROR | MB_OK)) {
                case IDOK:
                    return (1);
                }
            }
            for (int i = 0; i < winsizew; i++)
                for (int i2 = 0; i2 < winsizeh; i2++)
                    fscanf(fp, "%d", &tmpl[i + winsizew * i2]);
            vscrmemp = tmpl;
            pushvscr();
            fclose(fp);

            break;
        case ID_ERASEROPTIONS_THINERASER:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_ERASEROPTIONS_FINEERASER, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_ERASEROPTIONS_BOLDERASER, MF_UNCHECKED);
            eraserThickness = 1;
            break;
        case ID_ERASEROPTIONS_FINEERASER:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_ERASEROPTIONS_THINERASER, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_ERASEROPTIONS_BOLDERASER, MF_UNCHECKED);
            eraserThickness = 3;
        case ID_ERASEROPTIONS_BOLDERASER:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_ERASEROPTIONS_FINEERASER, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_ERASEROPTIONS_THINERASER, MF_UNCHECKED);
            eraserThickness = 6;
            break;
        case ID_PENTEXTURE_SOLIDPEN:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_PENTEXTURE_DASHEDPEN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_PENTEXTURE_DOTPEN, MF_UNCHECKED);
            thickness = PS_SOLID;
            break;
        case ID_PENTEXTURE_DASHEDPEN:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_PENTEXTURE_DOTPEN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_PENTEXTURE_SOLIDPEN, MF_UNCHECKED);
            thickness = PS_DASH;
            break;
        case ID_PENTEXTURE_DOTPEN:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_PENTEXTURE_DASHEDPEN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_PENTEXTURE_SOLIDPEN, MF_UNCHECKED);
            thickness = PS_DOT;
            break;
        case ID_OBJECTS_RECTANGLE:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            objectType = RECTANGLE;
            setPenAndBrush(penColor);
            break;
        case ID_OBJECTS_TRIANGLE:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            objectType = TRIANGLE;
            setPenAndBrush(penColor);
            break;
        case ID_OBJECTS_CIRCLE:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            objectType = CIRCLE;
            setPenAndBrush(penColor);
            break;
        case ID_OBJECTS_LINE:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            objectType = LINE;
            setPenAndBrush(penColor);
            break;
        case ID_OBJECTS_DESELECTOBJECT:
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            objectType = NONE;
            setPenAndBrush(penColor);
            break;
        case ID_PENS_BOLD:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_PENS_THIN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_PENS_NORMAL, MF_UNCHECKED);
            penType = BOLD;
            thickness2 = 6;
            break;
        case ID_PENS_THIN:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_PENS_BOLD, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_PENS_NORMAL, MF_UNCHECKED);
            penType = THIN;
            thickness2 = 1;
            break;
        case ID_PENS_NORMAL:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_PENS_BOLD, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_PENS_THIN, MF_UNCHECKED);
            penType = NORMAL;
            thickness2 = 3;
            break;
        case ID_FILE_EXIT:
            DestroyWindow(hWnd);
            break;
        case ID_COLORS_RED:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLUE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GREEN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_YELLOW, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_WHITE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_INDIGO, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_VIOLET, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_ORANGE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLACK, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GRAY, MF_UNCHECKED);
            setPenAndBrush(RED);
            break;
        case ID_COLORS_BLUE:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_RED, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GREEN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_YELLOW, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_WHITE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_INDIGO, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_VIOLET, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_ORANGE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLACK, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GRAY, MF_UNCHECKED);
            setPenAndBrush(BLUE);
            break;
        case ID_COLORS_GREEN:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLUE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_RED, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_YELLOW, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_WHITE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_INDIGO, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_VIOLET, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_ORANGE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLACK, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GRAY, MF_UNCHECKED);
            setPenAndBrush(GREEN);
            break;
        case ID_COLORS_YELLOW:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLUE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_RED, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GREEN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_WHITE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_INDIGO, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_VIOLET, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_ORANGE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLACK, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GRAY, MF_UNCHECKED);
            setPenAndBrush(YELLOW);
            break;
        case ID_COLORS_ORANGE:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLUE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_RED, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GREEN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_WHITE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_INDIGO, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_VIOLET, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_YELLOW, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLACK, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GRAY, MF_UNCHECKED);
            setPenAndBrush(ORANGE);
            break;
        case ID_COLORS_BLACK:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLUE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_RED, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GREEN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_WHITE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_INDIGO, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_VIOLET, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_ORANGE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_YELLOW, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GRAY, MF_UNCHECKED);
            setPenAndBrush(BLACK);
            break;
        case ID_COLORS_WHITE:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLUE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_RED, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GREEN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_YELLOW, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_INDIGO, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_VIOLET, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_ORANGE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLACK, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GRAY, MF_UNCHECKED);
            setPenAndBrush(WHITE);
            break;
        case ID_COLORS_INDIGO:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLUE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_RED, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GREEN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_WHITE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_YELLOW, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_VIOLET, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_ORANGE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLACK, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GRAY, MF_UNCHECKED);
            setPenAndBrush(INDIGO);
            break;
        case ID_COLORS_GRAY:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLUE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_RED, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GREEN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_WHITE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_INDIGO, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_VIOLET, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_ORANGE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLACK, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_YELLOW, MF_UNCHECKED);
            setPenAndBrush(GRAY);
            break;
        case ID_COLORS_VIOLET:
            CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLUE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_RED, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GREEN, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_WHITE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_INDIGO, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_YELLOW, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_ORANGE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_BLACK, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_COLORS_GRAY, MF_UNCHECKED);
            setPenAndBrush(VIOLET);
            break;
        case ID_COLORS_PALLETTE:
            CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, ColorPallette);
            break;
        case ID_FILTERS_BLACKWHITE:
            BlackWhite();
            pushvscr();
            applyChanges();
            break;
        case ID_FILTERS_RED:
            RedFilter();
            applyChanges();
            break;
        case ID_FILTERS_YELLOW:
            YellowFilter();
            applyChanges();
            break;
        case ID_FILTERS_BLUE:
            BlueFilter();
            applyChanges();
            break;
        case ID_FILTERS_CURRENTCOLORFILTER:
            ApplyCustomFilter(penColor);
            pushvscr();
            applyChanges();
            break;
        case ID_FILE_INSERTIMAGE:
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            shapedrawstate = 0;
            objectType = NONE;
            wsprintfW(imageFileField, L"\0");
            wsprintfW(imageFileField, L"%ws", fileOpenFunction());
            for (int i = 0; i < winsizew; i++)
                for (int i2 = 0; i2 < winsizeh; i2++)
                    tmpregion[i + winsizew * i2] = vscrmemp[i + winsizew * i2];
            imageDrawState = 1;
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    return(0);
    case WM_MOUSEMOVE:
        setPenAndBrush(penColor);
        if (wParam & MK_LBUTTON) {
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            if (GET_X_LPARAM(lParam) > 100 && GET_X_LPARAM(lParam) < winsizew && GET_Y_LPARAM(lParam) > 50 && GET_Y_LPARAM(lParam) < winsizeh)
                mylmousemove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            pushvscr();
        }
        else if (wParam & MK_RBUTTON) {
            myrmousemove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), mdc);
        }
        else
            mymousemove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), mdc);
        return(0);
    case WM_LBUTTONDOWN:
        imageDrawState = 0;
        mylbutton(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), mdc);
        return(0);
    case WM_LBUTTONUP:
        applyChanges();
        eraserState2 = 0;
        drawstate = 0;
        return(0);
    case WM_PAINT:
    {
        BeginPaint(hWnd, &ps);
        pushvscr();
        EndPaint(hWnd, &ps);
    }
    return(0);
    case WM_DESTROY:
        PostQuitMessage(0);
        return(0);
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK UtensilSuite(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    wchar_t coordinates[16];
    HWND sliderHandle = GetDlgItem(hDlg, IDC_SLIDER1);
    HWND eraserHandle = GetDlgItem(hDlg, IDC_SLIDER5);
    HWND redHandle = GetDlgItem(hDlg, IDC_SLIDER2);
    HWND blueHandle = GetDlgItem(hDlg, IDC_SLIDER3);
    HWND greenHandle = GetDlgItem(hDlg, IDC_SLIDER4);
    Color pColor;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    Graphics graphics(GetDC(hDlg));
    SolidBrush solidBrush(Color(0, 0, 0));
    Pen solidPen(&solidBrush);
    Pen flatPen(&solidBrush);
    Pen roundPen(&solidBrush);
    Pen trianglePen(&solidBrush);
    Point point1(10, 235);
    Point point2(135, 235);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    solidPen.SetEndCap(LineCapRound);
    solidPen.SetStartCap(LineCapRound);
    //Set Flat/Square-Cap Pen Attributes
    flatPen.SetEndCap(LineCapFlat);
    flatPen.SetStartCap(LineCapFlat);
    flatPen.SetColor(Color(0, 0, 0));
    flatPen.SetWidth(15);
    //Set Round-Cap Pen Attributes
    roundPen.SetEndCap(LineCapRound);
    roundPen.SetStartCap(LineCapRound);
    roundPen.SetColor(Color(0, 0, 0));
    roundPen.SetWidth(15);
    //Set Tri-Cap Pen Attributes
    trianglePen.SetEndCap(LineCapTriangle);
    trianglePen.SetStartCap(LineCapTriangle);
    trianglePen.SetColor(Color(0, 0, 0));
    trianglePen.SetWidth(15);

    graphics.DrawLine(&flatPen, Point(95, 360), Point(135, 360));
    graphics.DrawLine(&roundPen, Point(95, 384), Point(135, 384));
    graphics.DrawLine(&trianglePen, Point(95, 409), Point(135, 409));
    wsprintfW(coordinates, L"\0");
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_CREATE:
        return (INT_PTR)TRUE;
    case WM_HSCROLL:
        //Clear the bezier-arc region for updating
        solidBrush.SetColor(Color(240, 240, 240));
        graphics.FillRectangle(&solidBrush, Rect(0, 210, 160, 60));
        graphics.FillRectangle(&solidBrush, Rect(0, 465, 160, 80));
        //Get slider values for eraser thickness and pen thickness
        if (SendMessageW(sliderHandle, TBM_GETPOS, 0, 0) / 2 < 45)
            thickness2 = SendMessageW(sliderHandle, TBM_GETPOS, 0, 0) / 2;
        if (SendMessageW(eraserHandle, TBM_GETPOS, 0, 0) / 2 < 45)
            eraserThickness = SendMessageW(eraserHandle, TBM_GETPOS, 0, 0) / 2;
        //Set the solidPen attributes to match those provided by the sliders
        solidPen.SetWidth((REAL)thickness2);
        //Also take the data from the check-box GUI to apply changes to the pen-tip shape.
        solidPen.SetEndCap(lineCap);
        solidPen.SetStartCap(lineCap);
        solidPen.SetColor(Color((BYTE)(SendMessageW(redHandle, TBM_GETPOS, 0, 0) * 2.55), (BYTE)(SendMessageW(greenHandle, TBM_GETPOS, 0, 0) * 2.55), (BYTE)(SendMessageW(blueHandle, TBM_GETPOS, 0, 0) * 2.55)));
        pColor = Color((BYTE)(SendMessageW(redHandle, TBM_GETPOS, 0, 0) * 2.55), (BYTE)(SendMessageW(greenHandle, TBM_GETPOS, 0, 0) * 2.55), (BYTE)(SendMessageW(blueHandle, TBM_GETPOS, 0, 0) * 2.55));
        gradientColor2 = pColor;
        penColor = pColor.GetValue();
        penColor = RGB2BGR(penColor);
        gradientColor1 = RGB2BGR(gradientColor2.GetValue());
        //Draw pen-tip bezier
        graphics.DrawBezier(&solidPen, Point(10, 235), Point(50, 270), Point(90, 210), Point(130, 240));
        //Re-adjust the settings for the eraser bezier
        solidPen.SetWidth((REAL)eraserThickness);
        solidPen.SetColor(Color((BYTE)255, (BYTE)255, (BYTE)255));
        //Draw the eraser bezier
        graphics.DrawBezier(&solidPen, Point(10, 500), Point(50, 535), Point(90, 475), Point(130, 505));
        setPenAndBrush(penColor);
        return (INT_PTR)TRUE;
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        solidBrush.SetColor(Color(240, 240, 240));
        graphics.FillRectangle(&solidBrush, Rect(0, 210, 160, 60));
        switch (LOWORD(wParam)) {
        case IDC_CHECK1:
            CheckDlgButton(hDlg, IDC_CHECK2, BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_CHECK3, BST_UNCHECKED);
            penColor = RGB2BGR(penColor);
            lineCap = LineCapFlat;
            solidPen.SetWidth((REAL)thickness2);
            solidPen.SetColor(penColor);
            solidPen.SetEndCap(lineCap);
            solidPen.SetStartCap(lineCap);
            graphics.DrawBezier(&solidPen, Point(10, 235), Point(50, 270), Point(90, 210), Point(130, 240));
            return (INT_PTR)TRUE;
        case IDC_CHECK2:
            CheckDlgButton(hDlg, IDC_CHECK3, BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
            penColor = RGB2BGR(penColor);
            lineCap = LineCapRound;
            solidPen.SetWidth((REAL)thickness2);
            solidPen.SetColor(penColor);
            solidPen.SetEndCap(lineCap);
            solidPen.SetStartCap(lineCap);
            graphics.DrawBezier(&solidPen, Point(10, 235), Point(50, 270), Point(90, 210), Point(130, 240));
            return (INT_PTR)TRUE;
        case IDC_CHECK3:
            CheckDlgButton(hDlg, IDC_CHECK2, BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
            penColor = RGB2BGR(penColor);
            lineCap = LineCapTriangle;
            solidPen.SetWidth((REAL)thickness2);
            solidPen.SetColor(penColor);
            solidPen.SetEndCap(lineCap);
            solidPen.SetStartCap(lineCap);
            graphics.DrawBezier(&solidPen, Point(10, 235), Point(50, 270), Point(90, 210), Point(130, 240));
            return (INT_PTR)TRUE;
        case IDOK:
            return (INT_PTR)TRUE;
        case IDCANCEL:
            return(INT_PTR)TRUE;
        }
        break;
    case WM_LBUTTONDOWN:
        if (GET_X_LPARAM(lParam) > 52 && GET_X_LPARAM(lParam) < 97 && GET_Y_LPARAM(lParam) > 45 && GET_Y_LPARAM(lParam) < 93) {
            texturePenState = 0;
            gradientPenState = 0;
            utensilType = CALLIGRAPHYPEN;
        }
        else if (GET_X_LPARAM(lParam) > 96 && GET_X_LPARAM(lParam) < 146 && GET_Y_LPARAM(lParam) > 49 && GET_Y_LPARAM(lParam) < 94) {
            texturePenState = 0;
            gradientPenState = 0;
            utensilType = MARKER;
        }
        else if (GET_X_LPARAM(lParam) > 0 && GET_X_LPARAM(lParam) < 47 && GET_Y_LPARAM(lParam) > 100 && GET_Y_LPARAM(lParam) < 145) {
            texturePenState = 1;
            gradientPenState = 0;
            utensilType = NONE;
        }
        else if (GET_X_LPARAM(lParam) > 53 && GET_X_LPARAM(lParam) < 93 && GET_Y_LPARAM(lParam) > 100 && GET_Y_LPARAM(lParam) < 148) {
            texturePenState = 0;
            gradientPenState = 1;
            utensilType = NONE;
        }
        return(INT_PTR)TRUE;
    case WM_MOUSEMOVE:
        wsprintfW(coordinates, L"%d, %d", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        //TextOut(GetDC(hDlg), 0, 0, coordinates, 8);
        return(INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK MainToolbar(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static int objectStates[] = { 1, 1, 1, 1 };
    wchar_t coordinates[16];
    wsprintfW(coordinates, L"\0");
    toolbarHandle = hDlg;
    HWND sliderHandle = GetDlgItem(hDlg, IDC_SLIDER1);
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_CREATE:
        return (INT_PTR)TRUE;
        /*case WM_MOUSEMOVE:
            wsprintfW(coordinates, L"%d, %d", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            TextOut(GetDC(hDlg), 0, 0, coordinates, 8);
            return(0);*/
    case WM_LBUTTONUP:
        return(0);
    case WM_INITDIALOG:
        SendDlgItemMessage(hDlg, IDC_BUTTON1, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RECTANGLEBMP)));
        SendDlgItemMessage(hDlg, IDC_BUTTON6, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ELLIPSEBMP)));
        SendDlgItemMessage(hDlg, IDC_BUTTON7, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TRIANGLEBMP)));
        SendDlgItemMessage(hDlg, IDC_BUTTON8, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LINEBMP)));
        SendDlgItemMessage(hDlg, IDC_BUTTON9, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PENCILBMP)));
        SendDlgItemMessage(hDlg, IDC_BUTTON10, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BUCKETBMP)));
        SendDlgItemMessage(hDlg, IDC_BUTTON11, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_DROPPERBMP)));
        SendDlgItemMessage(hDlg, IDC_BUTTON12, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ERASERBMP)));
        SendDlgItemMessage(hDlg, IDC_BUTTON4, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_UNDOICON)));
        SendDlgItemMessage(hDlg, IDC_BUTTON3, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_REDOICON)));
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) != IDC_BUTTON4 && LOWORD(wParam) != IDC_BUTTON3) {
            if (prevscreenindex < PREVSCREENLIMIT) {
                prevscreenindex++;
                for (int i = 0; i < winsizew; i++) {
                    for (int i2 = 0; i2 < winsizeh; i2++) {
                        undoScreen[prevscreenindex][i + winsizew * i2] = vscrmemp[i + winsizew * i2];
                    }
                }
                redoscreenindex = 0;
            }
        }
        switch (LOWORD(wParam)) {
        case IDC_BUTTON1:
            objectStates[0] += 1;
            if (objectStates[0] % 2 == 0)
                objectType = RECTANGLE;
            else
                objectType = NONE;
            return (INT_PTR)TRUE;
        case IDC_BUTTON6:
            objectStates[1] += 1;
            if (objectStates[1] % 2 == 0)
                objectType = CIRCLE;
            else
                objectType = NONE;
            return (INT_PTR)TRUE;
        case IDC_BUTTON7:
            objectStates[2] += 1;
            if (objectStates[2] % 2 == 0)
                objectType = TRIANGLE;
            else
                objectType = NONE;
            return (INT_PTR)TRUE;
        case IDC_BUTTON8:
            objectStates[3] += 1;
            if (objectStates[3] % 2 == 0)
                objectType = LINE;
            else
                objectType = NONE;
            return (INT_PTR)TRUE;
        case IDC_BUTTON9:
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            pencilState += 1;
            if (pencilState % 2 == 0) {
                shapedrawstate = 0;
                objectType = NONE;
            }
            dropperState = 1;
            eraserState = 1;
            bucketState = 1;
            return (INT_PTR)TRUE;
        case IDC_BUTTON10:
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            bucketState += 1;
            if (bucketState % 2 == 0) {
                shapedrawstate = 0;
                objectType = NONE;
            }
            dropperState = 1;
            pencilState = 1;
            eraserState = 1;
            return (INT_PTR)TRUE;
        case IDC_BUTTON11:
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            dropperState += 1;
            if (dropperState % 2 == 0) {
                shapedrawstate = 0;
                objectType = NONE;
            }
            bucketState = 1;
            pencilState = 1;
            eraserState = 1;
            return (INT_PTR)TRUE;
        case IDC_BUTTON12:
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            eraserState += 1;
            if (eraserState % 2 == 0) {
                shapedrawstate = 0;
                objectType = NONE;
            }
            dropperState = 1;
            pencilState = 1;
            bucketState = 1;
            return (INT_PTR)TRUE;
        case IDOK:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case IDC_BUTTON4:
            undo();
            return (INT_PTR)TRUE;
        case IDC_BUTTON3:
            redo();
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;

}

INT_PTR CALLBACK ColorPallette(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    RECT testColorRect;
    char colorVal[8];
    testColorRect.left = 219;
    testColorRect.right = 249;
    testColorRect.top = 340;
    testColorRect.bottom = 370;
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        SetDlgItemTextA(hDlg, IDC_EDIT6, "255");
        return (INT_PTR)TRUE;
    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON && (GET_Y_LPARAM(lParam)) < 210 && (GET_X_LPARAM(lParam)) > 10 && (GET_X_LPARAM(lParam)) < 215) {
            penColor = GetPixel(GetDC(hDlg), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            gradientColor2 = penColor;
            gradientColor1 = RGB2BGR(penColor);
            sprintf(colorVal, "\0");
            sprintf(colorVal, "%d \0", penColor & 0xff);
            SetDlgItemTextA(hDlg, IDC_EDIT1, colorVal);
            sprintf(colorVal, "\0");
            sprintf(colorVal, "%d \0", (penColor & 0x00ff00) >> 8);
            SetDlgItemTextA(hDlg, IDC_EDIT3, colorVal);
            sprintf(colorVal, "\0");
            sprintf(colorVal, "%d \0", penColor >> 16);
            SetDlgItemTextA(hDlg, IDC_EDIT5, colorVal);
            FillRect(GetDC(hDlg), &testColorRect, CreateSolidBrush(penColor));
        }
        return (INT_PTR)TRUE;
    case WM_LBUTTONDOWN:
        penColor = GetPixel(GetDC(hDlg), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        sprintf(colorVal, "\0");
        sprintf(colorVal, "%d \0", penColor & 0xff);
        SetDlgItemTextA(hDlg, IDC_EDIT1, colorVal);
        sprintf(colorVal, "\0");
        sprintf(colorVal, "%d \0", (penColor & 0x00ff00) >> 8);
        SetDlgItemTextA(hDlg, IDC_EDIT3, colorVal);
        sprintf(colorVal, "\0");
        sprintf(colorVal, "%d \0", penColor >> 16);
        SetDlgItemTextA(hDlg, IDC_EDIT5, colorVal);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            alpha = GetDlgItemInt(hDlg, IDC_EDIT6, NULL, TRUE);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case IDC_BUTTON1:
            initChooseColorStruct();
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}