#include <Windows.h>
extern unsigned long* vscrmemp;
extern HDC mdc;
void RedFilter(void);
void BlackWhite(void)
{
	int lm, r, g, b, a, x1, y1;
	for (y1 = 0; y1 < 786; y1++)
		for (x1 = 0; x1 < 1280; x1++)
		{
			a = vscrmemp[y1 * 1280 + x1];
			r = (a >> 16);
			g = (a & 0x00ff00) >> 8;
			b = a & 0xff;
			lm = vscrmemp[y1 * 1280 + x1] & 0xff;
			vscrmemp[y1 * 1280 + x1] = lm + (lm << 8) + (lm << 16);
		}
}
void WhiteBlack(void)
{
	int lm, r, g, b, a, x1 = 0, y1 = 0;
	a = vscrmemp[y1 * 1280 + x1];
	r = (a >> 16);
	g = (a & 0x00ff00) >> 8;
	b = a & 0xff;
	lm = (r * 5 + g * 9 + b * 2) >> 4;
	for (y1 = 0; y1 < 786; y1++)
		for (x1 = 0; x1 < 1280; x1++)
		{
			lm = vscrmemp[y1 * 1280 + x1] & 0xff;
			lm = 255 - lm;
			vscrmemp[y1 * 1280 + x1] = lm + (lm << 8) + (lm << 16);
		}
}
void RedFilter(void) {
	int lm;
	for(int x1 = 0; x1< 1280; x1++)
		for (int y1 = 0; y1 < 786; y1++) {
			lm = vscrmemp[y1 * 1280 + x1] & 0xff0000;
			vscrmemp[y1 * 1280 + x1] = lm + (lm << 8) + (lm << 16);
		}
}
void BlueFilter(void) {
	for (int x = 0; x < 1280; x++) {
		for (int y = 0; y < 786; y++) {
			vscrmemp[x + 1280 * y] = (vscrmemp[x + 1280 * y] & 0x00ff00) >> 8;
		}
	}
}

void YellowFilter(void) {
	int lm;
	for (int x1 = 0; x1 < 1280; x1++)
		for (int y1 = 0; y1 < 786; y1++) {
			lm = vscrmemp[y1 * 1280 + x1] & 0x00ff00;
			vscrmemp[y1 * 1280 + x1] = lm + (lm << 8) + (lm << 16);
		}
}
void ApplyCustomFilter(int color) {
	int lm;
	for (int x1 = 0; x1 < 1280; x1++)
		for (int y1 = 0; y1 < 786; y1++) {
			lm = vscrmemp[y1 * 1280 + x1] & color;
			vscrmemp[y1 * 1280 + x1] = lm + (lm << 8) + (lm << 16);
		}
}