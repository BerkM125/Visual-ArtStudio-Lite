#include <Windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include "Visual ArtStudio.h"
#include "resource.h"
#define MAXOBJECTS 100
#define FILESTRINGBUFFER 1024
extern HDC mdc;
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
HPEN clearPen;
HPEN dashPen;
HBRUSH clearBrush;
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
	POINT vertices[3];
	vertices[0].x = vertex1.x;
	vertices[0].y = vertex1.y;
	vertices[1].x = vertex2.x;
	vertices[1].y = vertex2.y;
	vertices[2].x = vertex3.x;
	vertices[2].y = vertex3.y;
	Polygon(hdc, vertices, 3);
}
void ellipseStruct::drawEllipse(void) {
	Ellipse(hdc, left, top, right, bottom);
	return;
}
void lineStruct::drawLine(void) {
	MoveToEx(hdc, vertex1.x, vertex1.y, NULL);
	LineTo(hdc, vertex2.x, vertex2.y);
	return;
}
RECT prevregion;
RECT region;
ellipseStruct ellipse;
triangleStruct triangle;
triangleStruct prevtriangle;
lineStruct line;
lineStruct prevline;
RECT allObjects[MAXOBJECTS];
void mylmousemove(int x, int y, HDC hdc) {
	RECT drawregion;
	static int fx = 0;
	static int fy = 0;
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
		else {
			if (drawstate == 1) {
				setPenAndBrush(penColor);
				SelectObject(hdc, penDCPen);
				MoveToEx(hdc, fx, fy, NULL);
				LineTo(hdc, x, y);
				fx = x;
				fy = y;
				pushvscr();
			}
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
	else if (dropperState % 2 == 0) {
		penColor = GetPixel(mdc, x, y);
		setPenAndBrush(penColor);
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

void mylbutton(int x, int y, HDC hdc) {
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
				ellipse.drawEllipse();
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
				triangle.drawTriangle();
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
				line.drawLine();
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
					SelectObject(hdc, penDCPen);
					MoveToEx(hdc, region.left, region.top, NULL);
					LineTo(hdc, region.right, region.top);
					MoveToEx(hdc, region.right, region.top, NULL);
					LineTo(hdc, region.right, region.bottom);
					MoveToEx(hdc, region.right, region.bottom, NULL);
					LineTo(hdc, region.left, region.bottom);
					MoveToEx(hdc, region.left, region.bottom, NULL);
					LineTo(hdc, region.left, region.top);
					MoveToEx(hdc, region.left, region.top, NULL);
					shapedrawstate = 0;
					break;
				case CIRCLE:
					ellipse.left = fx;
					ellipse.top = fy;
					ellipse.hdc = hdc;
					ellipse.right = x;
					ellipse.bottom = y;
					SelectObject(hdc, penDCPen);
					ellipse.drawEllipse();
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
					triangle.drawTriangle();
					shapedrawstate = 0;
					break;
				case LINE:
					line.vertex1.x = fx;
					line.vertex1.y = fy;
					line.vertex2.x = x;
					line.vertex2.y = y;
					SelectObject(hdc, penDCPen);
					line.drawLine();
					shapedrawstate = 0;
					break;
				}
				lbuttonstate = 0;
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
				SelectObject(hdc, penDCPen);
				MoveToEx(hdc, region.left, region.top, NULL);
				LineTo(hdc, region.right, region.top);
				MoveToEx(hdc, region.right, region.top, NULL);
				LineTo(hdc, region.right, region.bottom);
				MoveToEx(hdc, region.right, region.bottom, NULL);
				LineTo(hdc, region.left, region.bottom);
				MoveToEx(hdc, region.left, region.bottom, NULL);
				LineTo(hdc, region.left, region.top);
				MoveToEx(hdc, region.left, region.top, NULL);
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
	else if (imageDrawState == 1) {
		for (int i = 0; i < winsizew; i++)
			for (int i2 = 0; i2 < winsizeh; i2++)
				vscrmemp[i + winsizew * i2] = tmpregion[i + winsizew * i2];
		UploadGlobalImage(mdc, x, y);
	}
}