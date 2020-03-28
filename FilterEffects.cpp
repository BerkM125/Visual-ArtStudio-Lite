#include <Windows.h>
#include "Visual ArtStudio.h"
extern unsigned long* vscrmemp;
extern HDC mdc;
void RedFilter(void);
void BlackWhite(void)
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
void WhiteBlack(void)
{
	int lm, r, g, b, a, x1 = 0, y1 = 0;
	a = vscrmemp[y1 * WIDTH + x1];
	r = (a >> 16);
	g = (a & 0x00ff00) >> 8;
	b = a & 0xff;
	lm = (r * 5 + g * 9 + b * 2) >> 4;
	for (y1 = 0; y1 < HEIGHT; y1++)
		for (x1 = 0; x1 < WIDTH; x1++)
		{
			lm = vscrmemp[y1 * WIDTH + x1] & 0xff;
			lm = 255 - lm;
			vscrmemp[y1 * WIDTH + x1] = lm + (lm << 8) + (lm << 16);
		}
}
void RedFilter(void) {
	int lm;
	for(int x1 = 0; x1< WIDTH; x1++)
		for (int y1 = 0; y1 < HEIGHT; y1++) {
			lm = vscrmemp[y1 * WIDTH + x1] & 0xff0000;
			vscrmemp[y1 * WIDTH + x1] = lm + (lm << 8) + (lm << 16);
		}
}
void BlueFilter(void) {
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			vscrmemp[x + WIDTH * y] = (vscrmemp[x + WIDTH * y] & 0x00ff00) >> 8;
		}
	}
}

void YellowFilter(void) {
	int lm;
	for (int x1 = 0; x1 < WIDTH; x1++)
		for (int y1 = 0; y1 < HEIGHT; y1++) {
			lm = vscrmemp[y1 * WIDTH + x1] & 0x00ff00;
			vscrmemp[y1 * WIDTH + x1] = lm + (lm << 8) + (lm << 16);
		}
}
void ApplyCustomFilter(int color) {
	int lm;
	for (int x1 = 0; x1 < WIDTH; x1++)
		for (int y1 = 0; y1 < HEIGHT; y1++) {
			lm = vscrmemp[y1 * WIDTH + x1] & color;
			vscrmemp[y1 * WIDTH + x1] = lm + (lm << 8) + (lm << 16);
		}
}