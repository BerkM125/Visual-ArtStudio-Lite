#include <Windows.h>
#include "Visual ArtStudio.h"
#include <stack>
#include <vector>

using namespace std;

void undo(void) {
	int x, y;
	if (prevscreenindex >= 1) {
		for (x = 0; x < winsizew; x++) {
			for (y = 0; y < winsizeh; y++) {
				vscrmemp[x + winsizew * y] = undoScreen[prevscreenindex][x + winsizew * y];
			}
		}
		prevscreenindex-=1;
		redoscreenindex+=1;
		for (x = 0; x < winsizew; x++) {
			for (y = 0; y < winsizeh; y++) {
				redoScreen[redoscreenindex][x + winsizew * y] = vscrmemp[x + winsizew * y];
			}
		}
		pushvscr();
	}
}

void redo(void) {
	int x, y;
	if (redoscreenindex >= 1) {
		for (x = 0; x < winsizew; x++) {
			for (y = 0; y < winsizeh; y++) {
				vscrmemp[x + winsizew * y] = redoScreen[redoscreenindex][x + winsizew * y];
			}
		}
		prevscreenindex+=1;
		redoscreenindex-=1;
		for (x = 0; x < winsizew; x++) {
			for (y = 0; y < winsizeh; y++) {
				undoScreen[prevscreenindex][x + winsizew * y] = vscrmemp[x + winsizew * y];
			}
		}
		pushvscr();
	}
}

void applyChanges(void) {
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
