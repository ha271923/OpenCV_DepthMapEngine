#pragma once
#include "opencv2/highgui.hpp"

using namespace cv;


#define WND_X_OFFSET 400
#define WND_Y_OFFSET 10

#define LANDSCAPE_PIC 1

#if LANDSCAPE_PIC
#define WNDOUT_WIDTH   480
#define WNDOUT_HEIGHT  360
#else
#define WNDOUT_WIDTH   360
#define WNDOUT_HEIGHT  480
#endif

void showWindow(String wndName, InputArray mat, int pos);
void adjustWindow(String wndName, int pos);