#include <Windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include "Visual ArtStudio.h"
#include "resource.h"
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#define FILESTRINGBUFFER 1024
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
extern void pushvscr(void);
int shapedrawstate = 0;
int drawstate = 0;
int lbuttonstate = 0;
int eraserState2 = 0;
int gradientPenState = 0;
int texturePenState = 0;
int objectIndex = 0;
int selectState = 1;
int selectTransformState = 0;
wchar_t currentBrushTexture[64];
Color gradientColor1(255, 255, 0, 0);
Color gradientColor2(255, 0, 0, 255);
HPEN clearPen;
HPEN dashPen;
HBRUSH clearBrush;
RECT focusRect;
LPPOINT mainpoint;
unsigned long tmpregion[1280*786];
void initClearPen(void) {
	clearPen = CreatePen(thickness, 6, (COLORREF)WHITE);
	clearBrush = CreateHatchBrush(brushHatch, (COLORREF)WHITE);
	dashPen = CreatePen(PS_DASH, 1, (COLORREF)0xffbf00);
}
typedef struct vertex_struct {
	unsigned int x;
	unsigned int y;
} vertex;
class lineStruct {
public:
	HDC hdc;
	vertex vertex1;
	vertex vertex2;
	void drawLine(void);
};
class ellipseStruct {
	public:
		HDC hdc;
		int left;
		int right;
		int top;
		int bottom;
		void drawEllipse(void);
};
class triangleStruct {
	public:
		HDC hdc;
		vertex vertex1;
		vertex vertex2;
		vertex vertex3;
		void drawTriangle(void);
};
void triangleStruct::drawTriangle(void) {
	Point vertices[3];
	Graphics graphics(hdc);
	Image textureImg(currentBrushTexture);
	SolidBrush solidBrush(Color(255, penColor & 0xff, (penColor & 0x00ff00) >> 8, penColor >> 16));
	Pen solidPen(&solidBrush);
	solidPen.SetLineJoin((LineJoin)lineCap);
	solidPen.SetWidth(thickness2);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	vertices[0].X = vertex1.x;
	vertices[0].Y = vertex1.y;
	vertices[1].X = vertex2.x;
	vertices[1].Y = vertex2.y;
	vertices[2].X = vertex3.x;
	vertices[2].Y = vertex3.y;
	if (texturePenState == 1) {
		TextureBrush tbrush(&textureImg, WrapModeTile);
		Pen texturePen(&tbrush);
		texturePen.SetStartCap(solidPen.GetStartCap());
		texturePen.SetEndCap(solidPen.GetEndCap());
		texturePen.SetWidth((REAL)thickness2);
		graphics.DrawLine(&texturePen, vertices[0], vertices[1]);
		graphics.DrawLine(&texturePen, vertices[1], vertices[2]);
		graphics.DrawLine(&texturePen, vertices[2], vertices[0]);
	}
	else if (gradientPenState == 1) {
		LinearGradientBrush linGrBrush(
			Point(0, 10),
			Point(200, 10),
			gradientColor1,
			gradientColor2);
		Pen linGrPen(&linGrBrush);
		linGrPen.SetStartCap(solidPen.GetStartCap());
		linGrPen.SetEndCap(solidPen.GetEndCap());
		linGrPen.SetWidth((REAL)thickness2);
		graphics.DrawLine(&linGrPen, vertices[0], vertices[1]);
		graphics.DrawLine(&linGrPen, vertices[1], vertices[2]);
		graphics.DrawLine(&linGrPen, vertices[2], vertices[0]);
	}
	else {
		graphics.DrawPolygon(&solidPen, vertices, 3);
	}
}
void ellipseStruct::drawEllipse(void) {
	Graphics graphics(hdc);
	Image textureImg(currentBrushTexture);
	SolidBrush solidBrush(Color(255, penColor & 0xff, (penColor & 0x00ff00) >> 8, penColor >> 16));
	Pen solidPen(&solidBrush);
	solidPen.SetWidth(thickness2);
	solidPen.SetLineJoin((LineJoin)lineCap);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	if (texturePenState == 1) {
		TextureBrush tbrush(&textureImg, WrapModeTile);
		Pen texturePen(&tbrush);
		texturePen.SetWidth(thickness2);
		graphics.DrawEllipse(&texturePen, left, top, (right - left), (bottom - top));
	}
	else if (gradientPenState == 1) {
		LinearGradientBrush linGrBrush(
			Point(0, 10),
			Point(200, 10),
			gradientColor1,
			gradientColor2);
		Pen linGrPen(&linGrBrush);
		linGrPen.SetWidth(thickness2);
		graphics.DrawEllipse(&linGrPen, left, top, (right - left), (bottom - top));
	}
	else
		graphics.DrawEllipse(&solidPen, left, top, (right - left), (bottom - top));
	return;
}
void lineStruct::drawLine(void) {
	Graphics graphics(hdc);
	Image textureImg(currentBrushTexture);
	SolidBrush solidBrush(Color(255, penColor & 0xff, (penColor & 0x00ff00) >> 8, penColor >> 16));
	Pen solidPen(&solidBrush);
	solidPen.SetStartCap(lineCap);
	solidPen.SetEndCap(lineCap);
	solidPen.SetWidth(thickness2);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	Point point1(vertex1.x, vertex1.y);
	Point point2(vertex2.x, vertex2.y);
	if (texturePenState == 1) {
		TextureBrush tbrush(&textureImg, WrapModeTile);
		Pen texturePen(&tbrush);
		texturePen.SetStartCap(solidPen.GetStartCap());
		texturePen.SetEndCap(solidPen.GetEndCap());
		texturePen.SetWidth(thickness2);
		graphics.DrawLine(&texturePen, point1, point2);
	}
	else if (gradientPenState == 1) {
		LinearGradientBrush linGrBrush(
			Point(0, 10),
			Point(200, 10),
			gradientColor1,
			gradientColor2);
		Pen linGrPen(&linGrBrush);
		linGrPen.SetStartCap(solidPen.GetStartCap());
		linGrPen.SetEndCap(solidPen.GetEndCap());
		linGrPen.SetWidth(thickness2);
		graphics.DrawLine(&linGrPen, point1, point2);
	}
	else {
		graphics.DrawLine(&solidPen, point1, point2);
	}
	return;
}
RECT prevregion;
RECT region;
ellipseStruct ellipse;
triangleStruct triangle;
triangleStruct prevtriangle;
lineStruct line;
lineStruct prevline;

void drawRectGDI(RECT drawRect) {
	Graphics graphics(mdc);
	Image textureImg(currentBrushTexture);
	SolidBrush solidBrush(Color(255, penColor & 0xff, (penColor & 0x00ff00) >> 8, penColor >> 16));
	Pen solidPen(&solidBrush);
	Rect rectRgn;
	int x2, y2;
	if (drawRect.left < drawRect.right) {
		rectRgn.X = drawRect.left;
		x2 = drawRect.right;
	}
	else {
		rectRgn.X = drawRect.right;
		x2 = drawRect.left; 
	}
	
	if (drawRect.top < drawRect.bottom) {
		rectRgn.Y = drawRect.top;
		y2 = drawRect.bottom;
	}
	else {
		rectRgn.Y = drawRect.bottom;
		y2 = drawRect.top;
	}
	rectRgn.Width = (x2 - rectRgn.X);
	rectRgn.Height = (y2 - rectRgn.Y);
	solidPen.SetLineJoin((LineJoin)lineCap);
	solidPen.SetWidth(thickness2);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	if (texturePenState == 1) {
		TextureBrush tbrush(&textureImg, WrapModeTile);
		Pen texturePen(&tbrush);
		texturePen.SetLineJoin((LineJoin)lineCap);
		texturePen.SetWidth(thickness2);
		graphics.DrawRectangle(&texturePen, rectRgn);
	}
	else if (gradientPenState == 1) {
		LinearGradientBrush linGrBrush(
			Point(0, 10),
			Point(200, 10),
			gradientColor1,
			gradientColor2);
		Pen linGrPen(&linGrBrush);
		linGrPen.SetLineJoin((LineJoin)lineCap);
		linGrPen.SetWidth(thickness2);
		graphics.DrawRectangle(&linGrPen, rectRgn);
	}
	else {
		graphics.DrawRectangle(&solidPen, rectRgn);
	}
}
void mylmousemove(int x, int y) {
	RECT drawregion;
	LPPOINT lp;
	Point points[4];
	char debugString[32];
	static int debugIndex = 0;
	wsprintfA(debugString, "\0");
	Graphics graphics(mdc);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	LinearGradientBrush linGrBrush(
		Point(0, 10),
		Point(200, 10),
		gradientColor1,
		gradientColor2);
	Pen linGrPen(&linGrBrush);
	Image textureImg(currentBrushTexture);
	TextureBrush tbrush(&textureImg, WrapModeTile);
	Pen texturePen(&tbrush);
	SolidBrush solidBrush(Color(255, penColor & 0xff, (penColor & 0x00ff00) >> 8, penColor >> 16));
	Pen solidPen(&solidBrush);
	solidPen.SetEndCap(lineCap);
	solidPen.SetStartCap(lineCap);
	texturePen.SetEndCap(lineCap);
	linGrPen.SetEndCap(lineCap);
	solidPen.SetWidth(thickness2);
	texturePen.SetWidth(thickness2);
	linGrPen.SetWidth(thickness2);
	static int fx = 0;
	static int fy = 0;
	Point point1(fx, fy);
	Point point2(x, y);
	if (pencilState % 2 == 0) {
		if (drawstate == 0) {
			drawregion.left = x;
			drawregion.top = y;
			drawregion.right = x;
			drawregion.bottom = y;
			fx = x;
			fy = y;
			drawregion.right = drawregion.left + thickness2;
			drawregion.bottom = drawregion.top - thickness2;
			drawstate += 1;
		}
		else if (drawstate == 1) {
				
				point1.X = fx;
				point1.Y = fy;
				point2.X = x;
				point2.Y = y;
				debugIndex += 1;
				if (texturePenState == 1) {
					graphics.DrawLine(&texturePen, point1, point2);
				}
				else if (gradientPenState == 1) {
					graphics.DrawLine(&linGrPen, point1, point2);
				}
				else {
					switch (utensilType) {
					case CALLIGRAPHYPEN:
						solidPen.SetWidth(thickness2 / 3);
						graphics.DrawLine(&solidPen, fx, fy - 1 * thickness2/3, x, y - 1 * thickness2/3);
						graphics.DrawLine(&solidPen, fx, fy - 2 * thickness2/3, x, y - 2 * thickness2/3);
						graphics.DrawLine(&solidPen, fx, fy, x, y);
						break;
					case MARKER:
						graphics.DrawLine(&solidPen, point1, point2);
						break;
					case ERASER:
						SelectObject(mdc, GetStockObject(DC_PEN));
						penDCPen = CreatePen(PS_SOLID, eraserThickness, (COLORREF)WHITE);
						SelectObject(mdc, penDCPen);
						MoveToEx(mdc, fx, fy, NULL);
						LineTo(mdc, x, y);
						fx = x;
						fy = y;
						break;
					}
					
				}
				pushvscr();
				fx = x;
				fy = y;
			}
	}
	else if (eraserState % 2 == 0) {
		if (eraserState2 == 0) {
			drawregion.left = x;
			drawregion.top = y;
			drawregion.right = x;
			drawregion.bottom = y;
			fx = x;
			fy = y;
			drawregion.right = drawregion.left + eraserThickness;
			drawregion.bottom = drawregion.top - eraserThickness;
			eraserState2 += 1;
		}
		else {
			if (eraserState2 == 1) {
				SelectObject(mdc, GetStockObject(DC_PEN));
				penDCPen = CreatePen(PS_SOLID, eraserThickness, (COLORREF)WHITE);
				SelectObject(mdc, penDCPen);
				MoveToEx(mdc, fx, fy, NULL);
				LineTo(mdc, x, y);
				fx = x;
				fy = y;
				pushvscr();
			}
		}
	}
	else if (dropperState % 2 == 0) {
		penColor = GetPixel(mdc, x, y);
		setPenAndBrush(penColor);
	}
	else {
		return;
	}
}

void myrmousemove(int x, int y, HDC hdc) {
	RECT drawregion;
	static int state = 0;
	static int fx = 0;
	static int fy = 0;
	if (state == 0) {
		drawregion.left = x;
		drawregion.top = y;
		drawregion.right = x;
		drawregion.bottom = y;
		fx = x;
		fy = y;
		drawregion.right = drawregion.left + eraserThickness;
		drawregion.bottom = drawregion.top - eraserThickness;
		state += 1;
	}
	else {
		if (state == 1) {
			SelectObject(hdc, GetStockObject(DC_PEN));
			penDCPen = CreatePen(PS_SOLID, eraserThickness, (COLORREF)WHITE);
			SelectObject(hdc, penDCPen);
			MoveToEx(hdc, fx, fy, NULL);
			LineTo(hdc, x, y);
			fx = x;
			fy = y;
			pushvscr();
		}
	}
}

void mylbuttonup(int x, int y, HDC hdc) {
}

void mylbutton(int x, int y, HDC hdc) {

	Graphics graphics(mdc);
	Point points[4];
	LinearGradientBrush linGrBrush(
		Point(0, 10),
		Point(200, 10),
		gradientColor1,
		gradientColor2);
	Pen linGrPen(&linGrBrush);
	linGrPen.SetWidth(thickness2);
	static int fx = 0;
	static int fy = 0;
	if (objectType != NONE) {
		if (lbuttonstate == 0) {
			for (int i = 0; i < winsizew; i++)
				for (int i2 = 0; i2 < winsizeh; i2++)
					tmpregion[i + winsizew * i2] = vscrmemp[i + winsizew * i2];
			switch (objectType) {
			case RECTANGLE:
				region.left = x;
				region.top = y;
				fx = region.left;
				fy = region.top;
				region.right = region.left + 1;
				region.bottom = region.top + 1;
				SelectObject(hdc, penDCPen);
				shapedrawstate = 1;
				break;
			case CIRCLE:
				ellipse.left = x;
				ellipse.top = y;
				ellipse.hdc = hdc;
				fx = ellipse.left;
				fy = ellipse.top;
				ellipse.right = ellipse.left + 1;
				ellipse.bottom = ellipse.top + 1;
				SelectObject(hdc, penDCPen);
				shapedrawstate = 1;
				break;
			case TRIANGLE:
				triangle.hdc = hdc;
				triangle.vertex1.x = x;
				triangle.vertex1.y = y;
				fx = triangle.vertex1.x;
				fy = triangle.vertex1.y;
				triangle.vertex2.x = triangle.vertex1.x + 1;
				triangle.vertex2.y = triangle.vertex1.y + 1;
				triangle.vertex3.x = triangle.vertex2.x;
				triangle.vertex3.y = triangle.vertex2.y;
				SelectObject(hdc, penDCPen);
				shapedrawstate = 1;
				break;
			case LINE:
				line.hdc = hdc;
				line.vertex1.x = x;
				line.vertex1.y = y;
				fx = line.vertex1.x;
				fy = line.vertex1.y;
				line.vertex2.x = line.vertex1.x;
				line.vertex2.y = line.vertex1.y;
				SelectObject(hdc, penDCPen);
				shapedrawstate = 1;
				break;
			}
			lbuttonstate = 1;
			pushvscr();
			return;
		}
		else {
			if (lbuttonstate == 1) {
				switch (objectType) {
				case RECTANGLE:
					region.left = fx;
					region.top = fy;
					region.right = x;
					region.bottom = y;
					shapedrawstate = 0;
					break;
				case CIRCLE:
					ellipse.left = fx;
					ellipse.top = fy;
					ellipse.hdc = hdc;
					ellipse.right = x;
					ellipse.bottom = y;
					SelectObject(hdc, penDCPen);
					shapedrawstate = 0;
					break;

				case TRIANGLE:
					triangle.vertex1.x = fx;
					triangle.vertex1.y = fy;
					triangle.vertex2.x = triangle.vertex1.x;
					triangle.vertex2.y = y;
					triangle.vertex3.x = x;
					triangle.vertex3.y = y;
					SelectObject(hdc, penDCPen);
					shapedrawstate = 0;
					break;
				case LINE:
					line.vertex1.x = fx;
					line.vertex1.y = fy;
					line.vertex2.x = x;
					line.vertex2.y = y;
					SelectObject(hdc, penDCPen);
					shapedrawstate = 0;
					break;
				}
				lbuttonstate = 0;
				objectIndex += 1;
			}
		}
		pushvscr();
		return;
	}
	else if (imageDrawState == 1) {
		UploadGlobalImage(mdc, x, y);
		imageDrawState = 0;
		wsprintfW(imageFileField, L"\0");
		return;
	}
	else if (bucketState % 2 == 0) {
		ExtFloodFill(mdc, x, y, (COLORREF)penColor, FLOODFILLBORDER);
		pushvscr();
		return;
	}
	else if (dropperState % 2 == 0) {
		penColor = GetPixel(mdc, x, y);
		setPenAndBrush(penColor);
	}
}
void mymousemove(int x, int y, HDC hdc) {
		if (shapedrawstate == 1) {
		for (int i = 0; i < winsizew; i++)
			for (int i2 = 0; i2 < winsizeh; i2++)
				vscrmemp[i + winsizew * i2] = tmpregion[i + winsizew * i2];
		switch (objectType) {
		case RECTANGLE:
			prevregion.top = region.top;
			prevregion.left = region.left;
			prevregion.right = region.right;
			prevregion.bottom = region.bottom;
			SelectObject(hdc, clearBrush);
			region.right = x;
			region.bottom = y;
			drawRectGDI(region);
			break;
		case CIRCLE:
			prevregion.top = ellipse.top;
			prevregion.left = ellipse.left;
			prevregion.right = ellipse.right;
			prevregion.bottom = ellipse.bottom;
			ellipse.hdc = hdc;
			SelectObject(hdc, clearBrush);
			ellipse.right = x;
			ellipse.bottom = y;
			SelectObject(hdc, penDCPen);
			ellipse.drawEllipse();
			break;
		case TRIANGLE:
			prevregion.top = triangle.vertex1.y;
			prevregion.left = triangle.vertex1.x;
			prevregion.right = triangle.vertex3.x;
			prevregion.bottom = triangle.vertex3.y;
			triangle.hdc = hdc;
			SelectObject(hdc, clearBrush);
			triangle.vertex3.x = x;
			triangle.vertex3.y = y;
			triangle.vertex2.x = triangle.vertex1.x;
			triangle.vertex2.y = y;
			SelectObject(hdc, penDCPen);
			triangle.drawTriangle();
			break;
		case LINE:
			prevline.vertex1.x = line.vertex1.x;
			prevline.vertex1.y = line.vertex1.y;
			prevline.vertex2.x = line.vertex2.x;
			prevline.vertex2.y = line.vertex2.y;
			line.hdc = hdc;
			SelectObject(hdc, clearBrush);
			line.vertex2.x = x;
			line.vertex2.y = y;
			SelectObject(hdc, penDCPen);
			line.drawLine();
			break;

		}
		pushvscr();
	}
	if (imageDrawState == 1) {
		for (int i = 0; i < winsizew; i++)
			for (int i2 = 0; i2 < winsizeh; i2++)
				vscrmemp[i + winsizew * i2] = tmpregion[i + winsizew * i2];
		UploadGlobalImage(mdc, x, y);
	}
	else {
		return;
	}
}