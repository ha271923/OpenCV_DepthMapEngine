// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/calib3d.hpp"
#include "common.h"
#include "Gui/UI.h"
using namespace std;
using namespace cv;


/*
	Install OpenCV
	https://docs.opencv.org/3.4.0/d3/d52/tutorial_windows_install.html
	https://docs.opencv.org/3.1.0/d3/d14/tutorial_ximgproc_disparity_filtering.html
	http://blog.csdn.net/GarfieldEr007/article/details/51219162
	https://kheresy.wordpress.com/2013/02/06/fix-the-problem-that-visualstudio-2012-can-not-find-the-dll-files-in-working-directory/

*/
void show_image();
int show_camera();
int show_depthmap();
void calc_depthmap(const IplImage* left, const IplImage* right, Mat& disparity);
void overlay_two_img(String file1, String file2);

static String left_image = "ambush_5_left.jpg";
static String right_image = "ambush_5_right.jpg";

void show_image() {
	Mat img = imread("L0.png", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("Display window", img);
}

int show_camera() {
	VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;

	Mat frame;
	namedWindow("camera", 1);
	for (;;)
	{
		cap >> frame; // put camera frame to Mat
		imshow("frame", frame);
		if (waitKey(30) >= 0)
			break;
	}
}

void overlay_two_img(String file1, String file2) {
	Mat src  = imread(file1, CV_LOAD_IMAGE_UNCHANGED);
	Mat logo = imread(file2, CV_LOAD_IMAGE_UNCHANGED);
	Mat dst  = imread(file1, CV_LOAD_IMAGE_UNCHANGED);
	Mat imgROI = dst(Rect(50, 55, logo.cols, logo.rows));
	addWeighted(imgROI, 0.5, logo, 0.5, 0, imgROI);

	namedWindow("file1");
	namedWindow("file2");
	namedWindow("file1+2");
	imshow("file1s", src);
	imshow("file2", logo);
	imshow("file1+2", dst);
}

void showImage(cv::String wndName, cv::InputArray imgArray) {
	namedWindow(wndName, WINDOW_NORMAL);
	cv::resizeWindow(wndName, WNDOUT_WIDTH, WNDOUT_HEIGHT);
	imshow(wndName, imgArray);
}