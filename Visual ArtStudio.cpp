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
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 64
#define FILESTRINGBUFFER 1024
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HBRUSH penBrush;
extern HBRUSH clearBrush;
HBITMAP hImg;
HWND sticker1, fsa, objectTree, toolbarHandle;
HPEN penDCPen;
HDC hdc, mdc, sdc;
LOGBRUSH hatchBrush;
char filestring[FILESTRINGBUFFER];
unsigned long* vscrmemp;
int brushHatch = HS_CROSS;
int brushState = BS_SOLID;
int thickness = PS_SOLID;
int thickness2 = 3;
int pencilState = 1;
int eraserState = 1;
int bucketState = 1;
int dropperState = 1;
int penType = NORMAL;
int objectType = NONE;
int penColor = BLACK;
int eraserThickness = 3;
int eraserType = NORMAL; 
int winsizew = 1280;
int winsizeh = 786;
int imageDrawState = 0;
wchar_t imageFileField[FILESTRINGBUFFER];
extern int eraserState2;
extern int drawstate;
extern int shapedrawstate;
extern int lbuttonstate;
extern unsigned long tmpregion[1280 * 786];
unsigned long prevscreens[100][1280 * 786];
int prevscreenindex = 0;
int maxprevscreen = 0;
CHOOSECOLOR chooseColor;
extern void         initClearPen(void);
extern void         BlackWhite(void);
extern void         YellowFilter(void);
extern void         RedFilter(void);
extern void         BlueFilter(void);
extern void         WhiteBlack(void);
extern void         ApplyCustomFilter(int color);
HWND hWnd;
void                checkToolbarItems(void);
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Controls(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ColorPallette(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK MainToolbar(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK HatchMenu(HWND, UINT, WPARAM, LPARAM);

void                initMemBitmap(HDC hdc);
void pushvscr(void);
extern void mylmousemove (int x, int y, HDC hdc);
extern void myrmousemove(int x, int y, HDC hdc);
extern void mylbutton(int x, int y, HDC hdc);
extern void mymousemove(int x, int y, HDC hdc);


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
VOID OnPaint(HDC hdc) {
    Graphics graphics(hdc);
    HPEN blackPen = CreatePen(BS_SOLID, 3, 0x343434);
    RECT frame;
    frame.left = 0; 
    frame.right = winsizew;
    frame.top = 0;
    frame.bottom = winsizeh;
    SelectObject(hdc, blackPen);
    MoveToEx(hdc, frame.left, frame.top, NULL);
    LineTo(hdc, frame.right, frame.top);
    MoveToEx(hdc, frame.right, frame.top, NULL);
    LineTo(hdc, frame.right, frame.bottom);
    MoveToEx(hdc, frame.right, frame.bottom, NULL);
    LineTo(hdc, frame.left, frame.bottom);
    MoveToEx(hdc, frame.left, frame.bottom, NULL);
    LineTo(hdc, frame.left, frame.top);
    MoveToEx(hdc, frame.left, frame.top, NULL);
    pushvscr();
}
VOID UploadGlobalImage(HDC hdc, int x, int y)
{
    Graphics graphics(hdc);
    Image image(imageFileField);
    if ((int)image.GetWidth() > winsizew || (int)image.GetHeight() > winsizeh)
        graphics.DrawImage(&image, x, y, winsizew/2, winsizeh/2);
    else
        graphics.DrawImage(&image, x, y);
    pushvscr();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VISUALARTSTUDIO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VISUALARTSTUDIO));
    MSG msg;
    hdc = GetDC(hWnd);
    sdc = hdc;
    initClearPen();
    initMemBitmap(hdc);
    
    CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, MainToolbar);
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    GdiplusShutdown(gdiplusToken);
    ReleaseDC(hWnd, hdc);
    return (int) msg.wParam;
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
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_VISUALARTSTUDIO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, winsizew, winsizeh, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
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
    bmi.bmiHeader.biHeight = -winsizeh;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0;
    hbmapm = CreateDIBSection(0, &bmi, DIB_RGB_COLORS, (void**)&lp, 0, 0);

    memdc = CreateCompatibleDC(0);	/* 0 for app's screen */
    SelectObject(memdc, hbmapm);
    GetObject(hbmapm, sizeof(BITMAP), &bma);
    for (k = 0; k < winsizeh * winsizew; k++)
        lp[k] = WHITE;

    BitBlt(hdc, 0, 0, winsizew, winsizeh, memdc, 0, 0, SRCCOPY);

    mdc = memdc;
    vscrmemp = lp;
}

void pushvscr(void) {
    BitBlt(sdc, 0, 0, winsizew, winsizeh, mdc, 0, 0, SRCCOPY);
}


int ldbmpf(char* fnm, int xPos, int yPos)
{
    int imgf, k, x;
    int imgw, imgh, bpp, cph, cpw;
    unsigned char hdr[32];
    unsigned long imgp, px;
    unsigned char pp[1024 * 10 * 4];

    imgf = _open(fnm, _O_RDONLY | _O_BINARY);
    if (imgf == -1)
        return 0;
    _read(imgf, hdr, 32);

    imgp = *((unsigned long*)&hdr[10]);
    imgw = *((unsigned long*)&hdr[18]);
    imgh = *((unsigned long*)&hdr[22]);
    bpp = *((unsigned short*)&hdr[28]);

    cpw = winsizew;
    cph = winsizeh;

    if (imgw < winsizew) cpw = imgw;
    if (imgh < winsizeh) cph = imgh;

    for (k = yPos; k < cph+yPos; k++)
    {
        _lseek(imgf, imgp + (cph - k - 1) * cpw * 3, SEEK_SET);
        _read(imgf, pp, 3 * cpw);
        for (x = xPos; x < cpw+xPos; x++)
        {
            unsigned long r, g, b;
            r = pp[x * 3 + 2];
            g = pp[x * 3 + 1];
            b = pp[x * 3];
            px = (r << 16) | (g << 8) | b;
            vscrmemp[k * winsizew + x] = px;
        }
    }

    return 1;
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

void DrawRect(HDC hdc, int left, int top, int right, int bottom, int state) {
    RECT region;
    region.left = left;
    region.right = right;
    region.top = top;
    region.bottom = bottom;
    SelectObject(hdc, GetStockObject(DC_PEN));
    if(state == 1)
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;  
    FILE* fp;
    unsigned long* tmpl;
    tmpl = vscrmemp;
    char fileField[FILESTRINGBUFFER];
    HWND controlDialog;
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
            CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOGBAR), hWnd, Controls);
            break;
        case ID_PENS_CUSTOMTHICKNESS:
            CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOGBAR), hWnd, Controls);
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
            UploadGlobalImage(mdc, 0, 0);
            imageDrawState = 1;
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        checkToolbarItems();
    }
    return(0);
    case WM_MOUSEMOVE:
        setPenAndBrush(penColor);
        if (wParam & MK_LBUTTON) {
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            mylmousemove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), mdc);
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
        if (prevscreenindex < 99) {
            for (int i = 0; i < 1280; i++)
                for (int i2 = 0; i2 < 786; i2++)
                    prevscreens[prevscreenindex][i + 1280 * i2] = vscrmemp[i + 1280 * i2];
            prevscreenindex += 1;
            maxprevscreen += 1;
        }
            if (shapedrawstate == 0) {
                EndDialog(toolbarHandle, NULL);
                CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, MainToolbar);
            }
            eraserState2 = 0;
            drawstate = 0;
            return(0);
        case WM_PAINT:
            {
                BeginPaint(hWnd, &ps);  
                OnPaint(mdc);
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

void printColorInfo(HWND hDlg, RECT testColorRect, LPARAM lParam, char *colorVal, char rgb[3][8]) {
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
}

void checkToolbarItems(void) {
    HWND hDlg = toolbarHandle;
    if (pencilState % 2 == 0) {
        DrawRect(GetDC(hDlg), 510, 10, 568, 60, 1);
        DrawRect(GetDC(hDlg), 580, 10, 638, 60, 0);
        DrawRect(GetDC(hDlg), 648, 10, 708, 60, 0);
        DrawRect(GetDC(hDlg), 718, 10, 775, 60, 0);
    }
    else if (eraserState % 2 == 0) {
        DrawRect(GetDC(hDlg), 510, 10, 568, 60, 0);
        DrawRect(GetDC(hDlg), 580, 10, 638, 60, 1);
        DrawRect(GetDC(hDlg), 648, 10, 708, 60, 0);
        DrawRect(GetDC(hDlg), 718, 10, 775, 60, 0);
    }
    else if (bucketState % 2 == 0) {
        DrawRect(GetDC(hDlg), 510, 10, 568, 60, 0);
        DrawRect(GetDC(hDlg), 580, 10, 638, 60, 0);
        DrawRect(GetDC(hDlg), 648, 10, 708, 60, 1);
        DrawRect(GetDC(hDlg), 718, 10, 775, 60, 0);
    }
    else if (dropperState % 2 == 0) {
        DrawRect(GetDC(hDlg), 510, 10, 568, 60, 0);
        DrawRect(GetDC(hDlg), 580, 10, 638, 60, 0);
        DrawRect(GetDC(hDlg), 648, 10, 708, 60, 0);
        DrawRect(GetDC(hDlg), 718, 10, 775, 60, 1);
    }
    switch (objectType) {
    case RECTANGLE:
        DrawRect(GetDC(hDlg), 90, 31, 118, 55, 1);
        DrawRect(GetDC(hDlg), 120, 31, 150, 55, 0);
        DrawRect(GetDC(hDlg), 155, 31, 180, 55, 0);
        DrawRect(GetDC(hDlg), 185, 31, 210, 55, 0);
        break;
    case TRIANGLE:
        DrawRect(GetDC(hDlg), 90, 31, 118, 55, 0);
        DrawRect(GetDC(hDlg), 120, 31, 150, 55, 0);
        DrawRect(GetDC(hDlg), 155, 31, 180, 55, 1);
        DrawRect(GetDC(hDlg), 185, 31, 210, 55, 0);
        break;
    case CIRCLE:
        DrawRect(GetDC(hDlg), 90, 31, 118, 55, 0);
        DrawRect(GetDC(hDlg), 120, 31, 150, 55, 1);
        DrawRect(GetDC(hDlg), 155, 31, 180, 55, 0);
        DrawRect(GetDC(hDlg), 185, 31, 210, 55, 0);
        break;
    case LINE:
        DrawRect(GetDC(hDlg), 90, 31, 118, 55, 0);
        DrawRect(GetDC(hDlg), 120, 31, 150, 55, 0);
        DrawRect(GetDC(hDlg), 155, 31, 180, 55, 0);
        DrawRect(GetDC(hDlg), 185, 31, 210, 55, 1);
        break;
    }
    switch (penType) {
    case BOLD:
        DrawRect(GetDC(hDlg), 223, 31, 250, 55, 1);
        DrawRect(GetDC(hDlg), 250, 31, 283, 55, 0);
        DrawRect(GetDC(hDlg), 280, 31, 308, 55, 0);
        break;
    case NORMAL:
        DrawRect(GetDC(hDlg), 223, 31, 250, 55, 0);
        DrawRect(GetDC(hDlg), 250, 31, 283, 55, 1);
        DrawRect(GetDC(hDlg), 280, 31, 308, 55, 0);
        break;
    case THIN:
        DrawRect(GetDC(hDlg), 223, 31, 250, 55, 0);
        DrawRect(GetDC(hDlg), 250, 31, 283, 55, 0);
        DrawRect(GetDC(hDlg), 280, 31, 308, 55, 1);
        break;
    }
    switch (thickness) {
    case PS_SOLID:
        DrawRect(GetDC(hDlg), 370, 31, 395, 55, 1);
        DrawRect(GetDC(hDlg), 400, 31, 425, 55, 0);
        DrawRect(GetDC(hDlg), 430, 31, 455, 55, 0);
        break;
    case PS_DASH:
        DrawRect(GetDC(hDlg), 370, 31, 395, 55, 0);
        DrawRect(GetDC(hDlg), 400, 31, 425, 55, 1);
        DrawRect(GetDC(hDlg), 430, 31, 455, 55, 0);
        break;
    case PS_DOT:
        DrawRect(GetDC(hDlg), 370, 31, 395, 55, 0);
        DrawRect(GetDC(hDlg), 400, 31, 425, 55, 0);
        DrawRect(GetDC(hDlg), 430, 31, 455, 55, 1);
        break;
    }
}
INT_PTR CALLBACK Controls(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
}

INT_PTR CALLBACK MainToolbar(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    PAINTSTRUCT ps;   
    wchar_t coordinates[16];
    wsprintfW(coordinates, L"\0");
    toolbarHandle = hDlg;
    HBITMAP hbit;
    switch (message) {
    case WM_CREATE:
        return (INT_PTR)TRUE;
    case WM_LBUTTONDOWN:
        if (GET_X_LPARAM(lParam) > 90 && GET_X_LPARAM(lParam) < 115) 
            objectType = RECTANGLE;
        else if (GET_X_LPARAM(lParam) > 120 && GET_X_LPARAM(lParam) < 150)
            objectType = CIRCLE;
        else if (GET_X_LPARAM(lParam) > 155 && GET_X_LPARAM(lParam) < 175)
            objectType = TRIANGLE;
        else if (GET_X_LPARAM(lParam) > 190 && GET_X_LPARAM(lParam) < 215)
            objectType = LINE;
        else if (GET_X_LPARAM(lParam) > 225 && GET_X_LPARAM(lParam) < 250 && GET_Y_LPARAM(lParam) > 30 && GET_Y_LPARAM(lParam) < 55) {
            penType = BOLD;
            thickness2 = 6;
        }
        else if (GET_X_LPARAM(lParam) > 250 && GET_X_LPARAM(lParam) < 280 && GET_Y_LPARAM(lParam) > 30 && GET_Y_LPARAM(lParam) < 55) {
            penType = NORMAL;
            thickness2 = 3;
        }
        else if (GET_X_LPARAM(lParam) > 280 && GET_X_LPARAM(lParam) < 305 && GET_Y_LPARAM(lParam) > 30 && GET_Y_LPARAM(lParam) < 55) {
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
        else if (GET_X_LPARAM(lParam) > 520 && GET_X_LPARAM(lParam) < 575 && GET_Y_LPARAM(lParam) > 10 && GET_Y_LPARAM(lParam) < 65) {
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            shapedrawstate = 0;
            objectType = NONE;
            pencilState += 1;
            dropperState = 1;
            eraserState = 1;
            bucketState = 1;
        }
        else if (GET_X_LPARAM(lParam) > 590 && GET_X_LPARAM(lParam) < 645 && GET_Y_LPARAM(lParam) > 10 && GET_Y_LPARAM(lParam) < 65) {
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            shapedrawstate = 0;
            objectType = NONE;
            eraserState += 1;
            dropperState = 1;
            pencilState = 1;
            bucketState = 1;
        }
        else if (GET_X_LPARAM(lParam) > 660 && GET_X_LPARAM(lParam) < 715 && GET_Y_LPARAM(lParam) > 10 && GET_Y_LPARAM(lParam) < 65) {
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            shapedrawstate = 0;
            objectType = NONE;
            bucketState += 1;
            dropperState = 1;
            pencilState = 1;
            eraserState = 1;
        }
        else if (GET_X_LPARAM(lParam) > 720 && GET_X_LPARAM(lParam) < 775 && GET_Y_LPARAM(lParam) > 10 && GET_Y_LPARAM(lParam) < 65) {
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_LINE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_TRIANGLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_CIRCLE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_OBJECTS_RECTANGLE, MF_UNCHECKED);
            shapedrawstate = 0;
            objectType = NONE;
            dropperState += 1;
            bucketState = 1;
            pencilState = 1;
            eraserState = 1;
        }
        checkToolbarItems();
    case WM_MOUSEMOVE:
        /*wsprintfW(coordinates, L"%d, %d", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        TextOut(GetDC(hDlg), 0, 0, coordinates, 8);*/
        return(0);
    case WM_LBUTTONUP:
        return(0);
    case WM_PAINT:
        BeginPaint(hDlg, &ps);
        checkToolbarItems();
        EndPaint(hDlg, &ps);
        return (0);
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
            case WM_COMMAND:
            if (LOWORD(wParam) != IDC_BUTTON4 && LOWORD(wParam) != IDC_BUTTON3) {
                if (prevscreenindex < 99) {
                    for (int i = 0; i < 1280; i++)
                        for (int i2 = 0; i2 < 786; i2++)
                            prevscreens[prevscreenindex][i + 1280 * i2] = vscrmemp[i + 1280 * i2];
                    prevscreenindex += 1;
                    maxprevscreen += 1;
                }
            }
            switch (LOWORD(wParam)) {
                case IDOK:
                    EndDialog(hDlg, LOWORD(wParam));
                    return (INT_PTR)TRUE;
                case IDCANCEL:
                    EndDialog(hDlg, LOWORD(wParam));
                    return (INT_PTR)TRUE;
                case IDC_BUTTON1:
                    CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOGBAR), hWnd, Controls);
                    return (INT_PTR)TRUE;
                case IDC_BUTTON2:
                    CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG5), hWnd, HatchMenu);
                    return (INT_PTR)TRUE;
                case IDC_BUTTON4:
                    if (prevscreenindex >= 1) {
                        prevscreenindex -= 1;
                        for (int i = 0; i < 1280; i++)
                            for (int i2 = 0; i2 < 786; i2++)
                                vscrmemp[i + 1280 * i2] = prevscreens[prevscreenindex][i + 1280 * i2];
                        pushvscr();
                        EndDialog(toolbarHandle, NULL);
                        CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, MainToolbar);
                    }
                    return (INT_PTR)TRUE;
                case IDC_BUTTON5:
                    BlueFilter();
                    pushvscr();
                    EndDialog(toolbarHandle, NULL);
                    CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, MainToolbar);
                    return (INT_PTR)TRUE;
                case IDC_BUTTON6:
                    YellowFilter();
                    pushvscr();
                    EndDialog(toolbarHandle, NULL);
                    CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, MainToolbar);
                    return (INT_PTR)TRUE;
                case IDC_BUTTON7:
                    RedFilter();
                    pushvscr();
                    EndDialog(toolbarHandle, NULL);
                    CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, MainToolbar);
                    return (INT_PTR)TRUE;
                case IDC_BUTTON3:
                    if (prevscreenindex < maxprevscreen-1) {
                        prevscreenindex += 1;
                        for (int i = 0; i < 1280; i++)
                            for (int i2 = 0; i2 < 786; i2++)
                                vscrmemp[i + 1280 * i2] = prevscreens[prevscreenindex][i + 1280 * i2];
                        pushvscr();
                        EndDialog(toolbarHandle, NULL);
                        CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, MainToolbar);
                    }
                    return (INT_PTR)TRUE;
            }
            break;
    }
    return (INT_PTR)FALSE;

}
INT_PTR CALLBACK HatchMenu(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
            case IDOK:
                EndDialog(hDlg, NULL);
                return(INT_PTR)TRUE;
            case IDCANCEL:
                EndDialog(hDlg, NULL);
                return(INT_PTR)TRUE;
            case IDC_BUTTON1:
                brushHatch = HS_FDIAGONAL;
                brushState = BS_HATCHED;
                if (thickness2 < 10) thickness2 = 10;
                setPenAndBrush(penColor);
                return(INT_PTR)TRUE;
            case IDC_BUTTON2:
                brushHatch = HS_CROSS;
                brushState = BS_HATCHED;
                if (thickness2 < 10) thickness2 = 10;
                setPenAndBrush(penColor);
                return(INT_PTR)TRUE;
            case IDC_BUTTON5:
                brushHatch = HS_DIAGCROSS;
                brushState = BS_HATCHED;
                if (thickness2 < 10) thickness2 = 10;
                setPenAndBrush(penColor);
                return(INT_PTR)TRUE;
            case IDC_BUTTON6:
                brushHatch = HS_BDIAGONAL;
                brushState = BS_HATCHED;
                if (thickness2 < 10) thickness2 = 10;
                setPenAndBrush(penColor);
                return(INT_PTR)TRUE;
            case IDC_BUTTON7:
                brushHatch = HS_HORIZONTAL;
                brushState = BS_HATCHED;
                if (thickness2 < 10) thickness2 = 10;
                setPenAndBrush(penColor);
                return(INT_PTR)TRUE;
            case IDC_BUTTON8:
                brushHatch = HS_VERTICAL;
                brushState = BS_HATCHED;
                if (thickness2 < 10) thickness2 = 10;
                setPenAndBrush(penColor);
                return(INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}
INT_PTR CALLBACK ColorPallette(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    char colorVal[8];
    char txt[16];
    char rgb[3][8];
    RECT testColorRect;
    testColorRect.left = 190;
    testColorRect.right = 220;
    testColorRect.top = 260;
    testColorRect.bottom = 290;
    sprintf(colorVal, "\0");
    sprintf(txt, "\0");
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON && (GET_Y_LPARAM(lParam)) < 210 && (GET_X_LPARAM(lParam)) > 10 && (GET_X_LPARAM(lParam)) < 215) {
            printColorInfo(hDlg, testColorRect, lParam, colorVal, rgb);
        }
        return (INT_PTR)TRUE;
    case WM_LBUTTONDOWN:
        printColorInfo(hDlg, testColorRect, lParam, colorVal, rgb);
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