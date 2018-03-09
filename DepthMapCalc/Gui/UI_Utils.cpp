#include "stdafx.h"
#include "opencv2/highgui.hpp"
#include "../common.h"
#include "UI.h"

using namespace cv;


/*
	Output Window Positions
	----------------
	|   1  |   2   |
	----------------
	|   3  |   4   |
	----------------
	|   5  |   6   |
	----------------
*/
void adjustWindow(String wndName, int pos) {
	resizeWindow(wndName, WNDOUT_WIDTH, WNDOUT_HEIGHT);
	switch (pos) {
	case 1:
		moveWindow(wndName, WND_X_OFFSET, WND_Y_OFFSET);
		break;
	case 2:
		moveWindow(wndName, WND_X_OFFSET + WNDOUT_WIDTH, WND_Y_OFFSET);
		break;

	case 3:
		moveWindow(wndName, WND_X_OFFSET, WND_Y_OFFSET + WNDOUT_HEIGHT);
		break;
	case 4:
		moveWindow(wndName, WND_X_OFFSET + WNDOUT_WIDTH, WND_Y_OFFSET + WNDOUT_HEIGHT);
		break;

	case 5:
		moveWindow(wndName, WND_X_OFFSET, WND_Y_OFFSET + WNDOUT_HEIGHT * 2);
		break;
	case 6:
		moveWindow(wndName, WND_X_OFFSET + WNDOUT_WIDTH, WND_Y_OFFSET + WNDOUT_HEIGHT * 2);
		break;

	}
}

void showWindow(String wndName, InputArray mat, int pos) {
	namedWindow(wndName, WINDOW_NORMAL);
	adjustWindow(wndName, pos);
	imshow(wndName, mat);
}
