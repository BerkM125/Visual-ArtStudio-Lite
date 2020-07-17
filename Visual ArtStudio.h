#pragma once
#include "framework.h"
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
#include "resource.h"
#define NORMAL 2
#define THIN 1
#define BOLD 3
#define BLACK 0x000000
#define WHITE 0xffffff
#define RED 0x0000ff
#define ORANGE 0x00a5ff
#define YELLOW 0x00ffff
#define GREEN 0x00ff00
#define BLUE 0xff0000
#define INDIGO 0x82004b
#define VIOLET 0xff007f
#define GRAY 0x606060
#define NONE 0
#define RECTANGLE 1
#define TRIANGLE 2
#define CIRCLE 3
#define LINE 4
#define CALLIGRAPHYPEN 1
#define PENCIL 2
#define ERASER 3
#define GRADIENTPEN 4
#define MARKER 5
#define BRUSH 6
#define MAXOBJECTS 200
#define WIDTH 1215
#define HEIGHT 786
#define PREVSCREENLIMIT 100
#define FILESTRINGBUFFER 1024
//External global variables
extern unsigned long* vscrmemp;
extern HDC mdc;
extern int brushHatch;
extern int brushState;
extern int thickness;
extern int thickness2;
extern int pencilState;
extern int eraserState;
extern int bucketState;
extern int dropperState;
extern int penType;
extern int objectType;
extern int penColor;
extern int eraserThickness;
extern int eraserType;
extern int winsizew;
extern int winsizeh;
extern int imageDrawState;
extern int objectTransformState;
extern int utensilType;
extern HBRUSH clearBrush;
extern int alpha;
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
extern unsigned long* vscrmemp;
extern int winsizew, winsizeh;
extern void pushvscr(void);
extern unsigned long undoScreen[PREVSCREENLIMIT][WIDTH * HEIGHT];
extern unsigned long redoScreen[PREVSCREENLIMIT][WIDTH * HEIGHT];
extern int prevscreenindex;
extern int redoscreenindex;
extern LineCap lineCap;
extern HDC mdc;
extern HDC hdc;
extern int utensilType;
extern int winsizew;
extern int winsizeh;
extern HBRUSH penBrush;
extern HPEN penDCPen;
extern int penType;
extern int objectType;
extern int penColor;
extern void setPenAndBrush(int color);
extern int thickness;
extern int thickness2;
extern int eraserThickness;
extern int brushHatch;
extern int pencilState;
extern int eraserState;
extern int bucketState;
extern int imageDrawState;
extern int dropperState;
extern wchar_t imageFileField[FILESTRINGBUFFER];
extern unsigned long* vscrmemp;
extern void UploadGlobalImage(HDC hdc, int x, int y);
extern int getLuminosity(int color);
extern void pushvscr(void);
//Externally defined functions
extern void initClearPen(void);
extern void BlackWhite(void);
extern void YellowFilter(void);
extern void RedFilter(void);
extern void BlueFilter(void);
extern void WhiteBlack(void);
extern void ApplyCustomFilter(int color);
extern void undo(void);
extern void redo(void);
extern void applyChanges(void);