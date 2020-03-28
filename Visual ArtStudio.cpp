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
#include <stdexcept>
using namespace Gdiplus;
using std::runtime_error;
#pragma comment (lib,"Gdiplus.lib")
#define MAX_LOADSTRING 64
#define FILESTRINGBUFFER 1024
#define PREVSCREENLIMIT 50
#define RGB2BGR(a_ulColor) (a_ulColor & 0xFF000000) | ((a_ulColor & 0xFF0000) >> 16) | (a_ulColor & 0x00FF00) | ((a_ulColor & 0x0000FF) << 16)
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];    // the main window class name
HBRUSH penBrush;
extern HBRUSH clearBrush;
HBITMAP hImg;
HWND sticker1, fsa, objectTree, toolbarHandle;
HPEN penDCPen;
HDC hdc, mdc, sdc;
LOGBRUSH hatchBrush;
char filestring[FILESTRINGBUFFER];
unsigned long* vscrmemp;
LineCap lineCap;
int brushHatch = HS_CROSS;
int brushState = BS_SOLID;
int thickness = PS_SOLID;
int thickness2 = 3;
int pencilState = 2;
int eraserState = 1;
int bucketState = 1;
int dropperState = 1;
int penType = NORMAL;
int objectType = NONE;
int penColor = BLACK;
int eraserThickness = 3;
int eraserType = NORMAL; 
int winsizew = WIDTH;
int winsizeh = HEIGHT;
int imageDrawState = 0;
int objectTransformState = 0;
int utensilType = MARKER;
wchar_t imageFileField[FILESTRINGBUFFER];
extern int texturePenState;
extern int eraserState2;
extern int drawstate;
extern int shapedrawstate;
extern int lbuttonstate;
extern int gradientPenState;
extern int objectIndex;
extern Color gradientColor1;
extern Color gradientColor2;
extern int selectState;
extern int selectTransformState;
extern RECT focusRect;
extern unsigned long tmpregion[WIDTH * HEIGHT];
unsigned long prevscreens[PREVSCREENLIMIT][WIDTH * HEIGHT];
int prevscreenindex = 0;
int maxprevscreen = 0;
extern wchar_t currentBrushTexture[64];
CHOOSECOLOR chooseColor;
extern void         initClearPen(void);
extern void         BlackWhite(void);
extern void         YellowFilter(void);
extern void         RedFilter(void);
extern void         BlueFilter(void);
extern void         WhiteBlack(void);
extern void         ApplyCustomFilter(int color);
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
extern void mylmousemove (int x, int y);
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
    HWND IntroHandle;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    wsprintf(currentBrushTexture, L"BrushTexture11.png");
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VISUALARTSTUDIO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow)){
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
    char ss[64];
    for (k = 0; k < 64; k++)
        ss[k] = 0x20;

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = winsizew;
    bmi.bmiHeader.biHeight = -(winsizeh);
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = 0; 
    bmi.bmiHeader.biSizeImage = 0;
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

    wcex.style          = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VISUALARTSTUDIO));
    wcex.hCursor        = LoadCursor(NULL, IDC_HAND);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_VISUALARTSTUDIO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_SIZEBOX| WS_MINIMIZEBOX | WS_DLGFRAME,
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
    BitBlt(sdc, 125, 0, winsizew-320, winsizeh, mdc,125, 0, SRCCOPY);  
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
        case ID_ERASEROPTIONS_CUSTOMTHICKNESS:
            //CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOGBAR), hWnd, Controls);
            break;
        case ID_PENS_CUSTOMTHICKNESS:
            //CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOGBAR), hWnd, Controls);
            break;
        case ID_FILTERS_BLACKWHITE:
            BlackWhite();
            pushvscr();
            EndDialog(toolbarHandle, NULL);
            CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, MainToolbar);
            break;
        case ID_FILTERS_RED:
            RedFilter();
            pushvscr();
            EndDialog(toolbarHandle, NULL);
            CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, MainToolbar);
            break;
        case ID_FILTERS_YELLOW:
            YellowFilter();
            pushvscr();
            EndDialog(toolbarHandle, NULL);
            CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, MainToolbar);
            break;
        case ID_FILTERS_BLUE:
            BlueFilter();
            pushvscr();
            EndDialog(toolbarHandle, NULL);
            CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, MainToolbar);
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
            if(GET_X_LPARAM(lParam) > 100 && GET_X_LPARAM(lParam) < winsizew && GET_Y_LPARAM(lParam) > 50 && GET_Y_LPARAM(lParam) < winsizeh)
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
            if (prevscreenindex < PREVSCREENLIMIT) {
                for (int i = 0; i < 1280; i++)
                    for (int i2 = 0; i2 < 786; i2++)
                        prevscreens[prevscreenindex][i + WIDTH * i2] = vscrmemp[i + WIDTH * i2];
                prevscreenindex += 1;
                maxprevscreen += 1;
            }

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

/*void printColorInfo(HWND hDlg, RECT testColorRect, LPARAM lParam, char *colorVal, char rgb[3][8]) {
    drawstate = 0;
    penColor = GetPixel(GetDC(hDlg), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    sprintf(colorVal, "\0");
    sprintf(colorVal, "%x \0", penColor);
    sprintf(rgb[0], "%d \0", (penColor >> 0) & 0xFF);
    sprintf(rgb[1], "%d \0", (penColor >> 8) & 0xFF);
    sprintf(rgb[2], "%d \0", (penColor >> 16) & 0xFF);
    TextOutA(GetDC(hDlg), 70, 215, "          ", 10);
    TextOutA(GetDC(hDlg), 70, 250, "          ", 10);
    TextOutA(GetDC(hDlg), 70, 232, "          ", 10);
    TextOutA(GetDC(hDlg), 70, 270, "          ", 10);
    TextOutA(GetDC(hDlg), 70, 215, (LPCSTR)colorVal, 6);
    TextOutA(GetDC(hDlg), 70, 250, (LPCSTR)rgb[1], 3);
    TextOutA(GetDC(hDlg), 70, 232, (LPCSTR)rgb[0], 3);
    TextOutA(GetDC(hDlg), 70, 267, (LPCSTR)rgb[2], 3);
    setPenAndBrush(penColor);
    FillRect(GetDC(hDlg), &testColorRect, CreateSolidBrush(penColor));
}*/

/*void checkToolbarItems(void) {
    HWND hDlg = toolbarHandle;
    if (pencilState % 2 == 0) {
        DrawRect(GetDC(hDlg), 22, 239, 78, 292, 1);
        DrawRect(GetDC(hDlg), 22, 305, 79, 357, 0);
        DrawRect(GetDC(hDlg), 22, 366, 79, 420, 0);
        DrawRect(GetDC(hDlg), 20, 430, 80, 483, 0);
    }
    else if (eraserState % 2 == 0) {
        DrawRect(GetDC(hDlg), 22, 239, 78, 292, 0);
        DrawRect(GetDC(hDlg), 22, 305, 79, 357, 1);
        DrawRect(GetDC(hDlg), 22, 366, 79, 420, 0);
        DrawRect(GetDC(hDlg), 20, 430, 80, 483, 0);
    }
    else if (bucketState % 2 == 0) {
        DrawRect(GetDC(hDlg), 22, 239, 78, 292, 0);
        DrawRect(GetDC(hDlg), 22, 305, 79, 357, 0);
        DrawRect(GetDC(hDlg), 22, 366, 79, 420, 1);
        DrawRect(GetDC(hDlg), 20, 430, 80, 483, 0);
    }
    else if (dropperState % 2 == 0) {
        DrawRect(GetDC(hDlg), 22, 239, 78, 292, 0);
        DrawRect(GetDC(hDlg), 22, 305, 79, 357, 0);
        DrawRect(GetDC(hDlg), 22, 366, 79, 420, 0);
        DrawRect(GetDC(hDlg), 20, 430, 80, 483, 1);
    }
    else {
        DrawRect(GetDC(hDlg), 22, 239, 78, 292, 0);
        DrawRect(GetDC(hDlg), 22, 305, 79, 357, 0);
        DrawRect(GetDC(hDlg), 22, 366, 79, 420, 0);
        DrawRect(GetDC(hDlg), 20, 430, 80, 483, 0);
    }
    switch (objectType) {
    case RECTANGLE:
        DrawRect(GetDC(hDlg), 20, 87, 47, 114, 1);
        DrawRect(GetDC(hDlg), 50, 85, 79, 111, 0);
        DrawRect(GetDC(hDlg), 20, 116, 46, 139, 0);
        DrawRect(GetDC(hDlg), 52, 116, 78, 149, 0);
        break;
    case TRIANGLE:
        DrawRect(GetDC(hDlg), 20, 87, 47, 114, 0);
        DrawRect(GetDC(hDlg), 50, 85, 79, 111, 0);
        DrawRect(GetDC(hDlg), 20, 116, 46, 139, 1);
        DrawRect(GetDC(hDlg), 52, 116, 78, 140, 0);
        break;
    case CIRCLE:
        DrawRect(GetDC(hDlg), 20, 87, 47, 114, 0);
        DrawRect(GetDC(hDlg), 50, 85, 79, 111, 1);
        DrawRect(GetDC(hDlg), 20, 116, 46, 139, 0);
        DrawRect(GetDC(hDlg), 52, 116, 78, 140, 0);
        break;
    case LINE:
        DrawRect(GetDC(hDlg), 22, 87, 47, 114, 0);
        DrawRect(GetDC(hDlg), 50, 85, 79, 111, 0);
        DrawRect(GetDC(hDlg), 20, 116, 46, 139, 0);
        DrawRect(GetDC(hDlg), 52, 116, 78, 140, 1);
        break;
    default:
        DrawRect(GetDC(hDlg), 20, 87, 47, 114, 0);
        DrawRect(GetDC(hDlg), 50, 85, 79, 111, 0);
        DrawRect(GetDC(hDlg), 20, 116, 46, 139, 0);
        DrawRect(GetDC(hDlg), 52, 116, 78, 140, 0);
        break;
    }
    switch (penType) {
    case BOLD:
        DrawRect(GetDC(hDlg), 19, 173, 46, 197, 1);
        DrawRect(GetDC(hDlg), 52, 174, 81, 198, 0);
        DrawRect(GetDC(hDlg), 20, 200, 46, 225, 0);
        break;
    case NORMAL:
        DrawRect(GetDC(hDlg), 19, 173, 46, 197, 0);
        DrawRect(GetDC(hDlg), 52, 174, 81, 198, 1);
        DrawRect(GetDC(hDlg), 20, 200, 46, 225, 0);
        break;
    case THIN:
        DrawRect(GetDC(hDlg), 19, 173, 46, 197, 0);
        DrawRect(GetDC(hDlg), 52, 174, 81, 198, 0);
        DrawRect(GetDC(hDlg), 20, 200, 46, 225, 1);
        break;
    default:
        DrawRect(GetDC(hDlg), 19, 173, 46, 197, 0);
        DrawRect(GetDC(hDlg), 52, 174, 81, 198, 0);
        DrawRect(GetDC(hDlg), 20, 200, 46, 225, 0);
        break;
    }
}*/

/*INT_PTR CALLBACK Controls(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    char tmp[16];
    RECT penRectRegion;
    RECT eraserRectRegion;
    penRectRegion.top = 40;
    penRectRegion.bottom = 40+thickness2;
    penRectRegion.left = 60;
    penRectRegion.right = 60+thickness2;
    eraserRectRegion.top = 40;
    eraserRectRegion.bottom = 40+eraserThickness;
    eraserRectRegion.left = 255;
    eraserRectRegion.right = 255+eraserThickness;
    sprintf(tmp, "\0");
    Ellipse(GetDC(hDlg), 60, 40, 60 + thickness2, 40 + thickness2);
    Ellipse(GetDC(hDlg), 255, 40, 255 + eraserThickness, 40 + eraserThickness);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDC_BUTTON1) { 
            FillRect(GetDC(hDlg), &penRectRegion, clearBrush);
            FillRect(GetDC(hDlg), &eraserRectRegion, clearBrush);
            GetDlgItemTextA(hDlg, IDC_EDIT1, tmp, 16);
            sscanf(tmp, "%d", &thickness2);
            GetDlgItemTextA(hDlg, IDC_EDIT2, tmp, 16);
            sscanf(tmp, "%d", &eraserThickness);
            penType = BOLD;
            thickness = PS_SOLID;
            Ellipse(GetDC(hDlg), 60, 40, 60 + thickness2, 40 + thickness2);
            Ellipse(GetDC(hDlg), 255, 40, 255 + eraserThickness, 40 + eraserThickness);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}*/

INT_PTR CALLBACK UtensilSuite(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    int x, y;
    char tmp[16];
    HBITMAP hbit;
    wchar_t coordinates[16];
    HWND sliderHandle = GetDlgItem(hDlg, IDC_SLIDER1);
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
        solidBrush.SetColor(Color(240, 240, 240));
        graphics.FillRectangle(&solidBrush, Rect(0, 210, 160, 60));
        if(SendMessageW(sliderHandle, TBM_GETPOS, 0, 0)/2 < 45)
            thickness2 = SendMessageW(sliderHandle, TBM_GETPOS, 0, 0)/2;
        solidPen.SetWidth(thickness2);
        solidPen.SetEndCap(lineCap);
        solidPen.SetStartCap(lineCap);
        solidPen.SetColor(Color(SendMessageW(redHandle, TBM_GETPOS, 0, 0)*2.55, SendMessageW(greenHandle, TBM_GETPOS, 0, 0) * 2.55, SendMessageW(blueHandle , TBM_GETPOS, 0, 0) * 2.55));
        pColor = Color(SendMessageW(redHandle, TBM_GETPOS, 0, 0) * 2.55, SendMessageW(greenHandle, TBM_GETPOS, 0, 0) * 2.55, SendMessageW(blueHandle, TBM_GETPOS, 0, 0) * 2.55);
        gradientColor2 = pColor;
        penColor = pColor.GetValue();
        penColor = RGB2BGR(penColor);
        gradientColor1 = RGB2BGR(gradientColor2.GetValue());
        graphics.DrawBezier(&solidPen, Point(10, 235), Point(50, 270), Point(90, 210), Point(130, 240));
        setPenAndBrush(penColor);
        return (INT_PTR)TRUE;
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        solidBrush.SetColor(Color(240, 240, 240));
        graphics.FillRectangle(&solidBrush, Rect(0, 210, 160, 60));
        switch (LOWORD(wParam)) {
        case IDC_CHECK1:
            penColor = RGB2BGR(penColor);
            lineCap = LineCapFlat;
            solidPen.SetWidth(thickness2);
            solidPen.SetColor(penColor);
            solidPen.SetEndCap(lineCap);
            solidPen.SetStartCap(lineCap);
            graphics.DrawBezier(&solidPen, Point(10, 235), Point(50, 270), Point(90, 210), Point(130, 240));
            return (INT_PTR)TRUE;
        case IDC_CHECK2:
            penColor = RGB2BGR(penColor);
            lineCap = LineCapRound;
            solidPen.SetWidth(thickness2);
            solidPen.SetColor(penColor);
            solidPen.SetEndCap(lineCap);
            solidPen.SetStartCap(lineCap);
            graphics.DrawBezier(&solidPen, Point(10, 235), Point(50, 270), Point(90, 210), Point(130, 240));
            return (INT_PTR)TRUE;
        case IDC_CHECK3:
            penColor = RGB2BGR(penColor);
            lineCap = LineCapTriangle;
            solidPen.SetWidth(thickness2);
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
        /*wsprintfW(coordinates, L"%d, %d", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        TextOut(GetDC(hDlg), 0, 0, coordinates, 8);*/
        return(INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK MainToolbar(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static int objectStates[] = { 1, 1, 1, 1 };
    PAINTSTRUCT ps;   
    wchar_t coordinates[16];
    wsprintfW(coordinates, L"\0");
    toolbarHandle = hDlg;
    HWND sliderHandle = GetDlgItem(hDlg, IDC_SLIDER1);
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_CREATE:
        return (INT_PTR)TRUE;
    case WM_LBUTTONDOWN:
        if (GET_X_LPARAM(lParam) > 23 && GET_X_LPARAM(lParam) < 43 && GET_Y_LPARAM(lParam) > 89 && GET_Y_LPARAM(lParam) < 110) {
            objectStates[0] += 1;
            if(objectStates[0] % 2 == 0)
                objectType = RECTANGLE;
            else 
                objectType = NONE;
        }
        else if (GET_X_LPARAM(lParam) > 51 && GET_X_LPARAM(lParam) < 77 && GET_Y_LPARAM(lParam) > 86 && GET_Y_LPARAM(lParam) < 110) {
            objectStates[1] += 1;
            if (objectStates[1] % 2 == 0)
                objectType = CIRCLE;
            else
                objectType = NONE;
        }
        else if (GET_X_LPARAM(lParam) > 20 && GET_X_LPARAM(lParam) < 42 && GET_Y_LPARAM(lParam) > 116 && GET_Y_LPARAM(lParam) < 137) {
            objectStates[2] += 1;
            if (objectStates[2] % 2 == 0)
                objectType = TRIANGLE;
            else
                objectType = NONE;
        }
        else if (GET_X_LPARAM(lParam) > 54 && GET_X_LPARAM(lParam) < 75 && GET_Y_LPARAM(lParam) > 116 && GET_Y_LPARAM(lParam) < 137) {
            objectStates[3] += 1;
            if (objectStates[3] % 2 == 0)
                objectType = LINE;
            else
                objectType = NONE;
        }
        else if (GET_X_LPARAM(lParam) > 655 && GET_X_LPARAM(lParam) < 875) {
            penColor = GetPixel(GetDC(hDlg), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            setPenAndBrush(penColor);
        }
        else if (GET_X_LPARAM(lParam) > 19 && GET_X_LPARAM(lParam) < 45 && GET_Y_LPARAM(lParam) > 175 && GET_Y_LPARAM(lParam) < 192) {
            penType = BOLD;
            thickness2 = 6;
        }
        else if (GET_X_LPARAM(lParam) > 52 && GET_X_LPARAM(lParam) < 79 && GET_Y_LPARAM(lParam) > 176 && GET_Y_LPARAM(lParam) < 196) {
            penType = NORMAL;
            thickness2 = 3;
        }
        else if (GET_X_LPARAM(lParam) > 20 && GET_X_LPARAM(lParam) < 43 && GET_Y_LPARAM(lParam) > 203 && GET_Y_LPARAM(lParam) < 223) {
            penType = THIN;
            thickness2 = 1;
        }
        else if (GET_X_LPARAM(lParam) > 370 && GET_X_LPARAM(lParam) < 395 && GET_Y_LPARAM(lParam) > 30 && GET_Y_LPARAM(lParam) < 55) {
            thickness = PS_SOLID;
            brushState = BS_SOLID;
        }
        else if (GET_X_LPARAM(lParam) > 400 && GET_X_LPARAM(lParam) < 425 && GET_Y_LPARAM(lParam) > 30 && GET_Y_LPARAM(lParam) < 55) {
            thickness = PS_DASH;
        }
        else if (GET_X_LPARAM(lParam) > 430 && GET_X_LPARAM(lParam) < 455 && GET_Y_LPARAM(lParam) > 30 && GET_Y_LPARAM(lParam) < 55) {
            thickness = PS_DOT;
        }
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
                if (prevscreenindex < 99) {
                    for (int i = 0; i < winsizew; i++)
                        for (int i2 = 0; i2 < winsizeh; i2++)
                            prevscreens[prevscreenindex][i + winsizew * i2] = vscrmemp[i + winsizew * i2];
                    prevscreenindex += 1;
                    maxprevscreen += 1;
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
                    if (prevscreenindex > 0) {
                        prevscreenindex -= 1;
                        for (int i = 0; i < winsizew; i++)
                            for (int i2 = 0; i2 < winsizeh; i2++)
                                vscrmemp[i + winsizew * i2] = prevscreens[prevscreenindex][i + winsizew * i2];
                        pushvscr();
                    }
                    return (INT_PTR)TRUE;
                case IDC_BUTTON3:
                    if (prevscreenindex < maxprevscreen-1) {
                        prevscreenindex += 1;
                        for (int i = 0; i < winsizew; i++)
                            for (int i2 = 0; i2 < winsizeh; i2++)
                                vscrmemp[i + winsizew * i2] = prevscreens[prevscreenindex][i + winsizew * i2];
                        pushvscr();
                    }
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