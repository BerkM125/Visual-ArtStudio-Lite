#include <Windows.h>
#include "Visual ArtStudio.h"

void ApplyCustomFilter(int color) {
	Graphics graphics(mdc);
	SolidBrush solidBrush(Color(125, color & 0xff, (color & 0x00ff00) >> 8, color >> 16));
	graphics.FillRectangle(&solidBrush, 0, 0, winsizew - 200, winsizeh);
	pushvscr();
}

void WhiteBlack(void)
{
	int lm, r, g, b, a, x1, y1;
	for (y1 = 0; y1 < HEIGHT; y1++)
		for (x1 = 0; x1 < WIDTH; x1++)
		{
			a = vscrmemp[y1 * WIDTH + x1];
			r = (a >> 16);
			g = (a & 0x00ff00) >> 8;
			b = a & 0xff;
			lm = vscrmemp[y1 * WIDTH + x1] & 0xff;
			vscrmemp[y1 * WIDTH + x1] = lm + (lm << 8) + (lm << 16);
		}
}

int getLuminosity (int color) {
	int lm, r, g, b, a;
	a = color;
	r = (a >> 16);
	g = (a & 0x00ff00) >> 8;
	b = a & 0xff;
	lm = (r * 5 + g * 9 + b * 2) >> 4;
	return(lm);
}

void BlackWhite(void) {
	int lm, a, x1 = 0, y1 = 0;
	a = vscrmemp[y1 * WIDTH + x1];
	lm = getLuminosity(a);
	for (y1 = 0; y1 < HEIGHT; y1++)
		for (x1 = 0; x1 < WIDTH; x1++)
		{
			lm = vscrmemp[y1 * WIDTH + x1] & 0xff;
			lm = 255 - lm;
			vscrmemp[y1 * WIDTH + x1] = lm + (lm << 8) + (lm << 16);
		}
}
void RedFilter(void) {
	ApplyCustomFilter(0x0000ff);
}
void BlueFilter(void) {
	ApplyCustomFilter(0xff0000);
}

void YellowFilter(void) {
	ApplyCustomFilter(0x00ffff);
}
