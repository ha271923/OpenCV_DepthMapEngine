#include "stdafx.h"

#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/ximgproc/disparity_filter.hpp"
#include "Gui/UI.h"
#include "common.h"
#include <iostream>
#include <string>
using namespace cv;
using namespace cv::ximgproc;
using namespace std;

/**
	reference from OpenCV 3.4 

	// BM algorithm Summary:
	//     Create a stereoBM object
	//
	// Parameters:
	//   blockSize:
	//     the linear size of the blocks compared by the algorithm. The size should be odd
	//     (as the block is centered at the current pixel). Larger block size implies smoother,
	//     though less accurate disparity map. Smaller block size gives more detailed disparity
	//     map, but there is higher chance for algorithm to find a wrong correspondence.
	//
	//   numberOfDisparities:
	//     the disparity search range. For each pixel algorithm will find the best disparity
	//     from 0 (default minimum disparity) to numberOfDisparities. The search range can
	//     then be shifted by changing the minimum disparity.



	// SGBM algorithmSummary:
	//     Create a stereo disparity solver using StereoSGBM algorithm (combination of H.
	//     Hirschmuller + K. Konolige approaches)
	//
	// Parameters:
	//   minDisparity:
	//     Minimum possible disparity value. Normally, it is zero but sometimes rectification
	//     algorithms can shift images, so this parameter needs to be adjusted accordingly.
	//
	//   numDisparities:
	//     Maximum disparity minus minimum disparity. The value is always greater than zero.
	//     In the current implementation, this parameter must be divisible by 16.
	//
	//   blockSize:
	//     Matched block size. It must be an odd number >=1 . Normally, it should be somewhere
	//     in the 3..11 range. Use 0 for default.
	//
	//   p1:
	//     The first parameter controlling the disparity smoothness. It is the penalty on
	//     the disparity change by plus or minus 1 between neighbor pixels. Reasonably good
	//     value is 8*number_of_image_channels*SADWindowSize*SADWindowSize. Use 0 for default
	//
	//   p2:
	//     The second parameter controlling the disparity smoothness. It is the penalty
	//     on the disparity change by more than 1 between neighbor pixels. The algorithm
	//     requires p2 > p1. Reasonably good value is 32*number_of_image_channels*SADWindowSize*SADWindowSize.
	//     Use 0 for default
	//
	//   disp12MaxDiff:
	//     Maximum allowed difference (in integer pixel units) in the left-right disparity
	//     check. Set it to a non-positive value to disable the check.
	//
	//   preFilterCap:
	//     Truncation value for the prefiltered image pixels. The algorithm first computes
	//     x-derivative at each pixel and clips its value by [-preFilterCap, preFilterCap]
	//     interval. The result values are passed to the Birchfield-Tomasi pixel cost function.
	//
	//   uniquenessRatio:
	//     Margin in percentage by which the best (minimum) computed cost function value
	//     should “win” the second best value to consider the found match correct. Normally,
	//     a value within the 5-15 range is good enough.
	//
	//   speckleWindowSize:
	//     Maximum size of smooth disparity regions to consider their noise speckles and
	//     invalidate. Set it to 0 to disable speckle filtering. Otherwise, set it somewhere
	//     in the 50-200 range
	//
	//   speckleRange:
	//     Maximum disparity variation within each connected component. If you do speckle
	//     filtering, set the parameter to a positive value, it will be implicitly multiplied
	//     by 16. Normally, 1 or 2 is good enough.
	//
	//   mode:
	//     Set it to HH to run the full-scale two-pass dynamic programming algorithm. It
	//     will consume O(W*H*numDisparities) bytes, which is large for 640x480 stereo and
	//     huge for HD-size pictures. By default, it is set to false.
*/


Rect computeROI(Size2i src_sz, Ptr<StereoMatcher> matcher_instance);

void print_BM_params(Ptr<StereoBM> bm);
void print_SGBM_params(Ptr<StereoSGBM> sgbm);

const String keys =
"{help h usage ? |                  | print this message                                                }"
// "{@left          |testpics\\cvdemo\\grayman_left.jpg | left view of the stereopair                                       }"
// "{@right         |testpics\\cvdemo\\grayman_right.jpg | right view of the stereopair                                      }"
// "{GT             |testpics\\cvdemo\\aloeGT.png| optional ground-truth disparity (MPI-Sintel or Middlebury format) }"
// "{@left          |testpics\\VR\\VR12_unityV_left.jpg  | left view of the stereopair                                       }"
// "{@right         |testpics\\VR\\VR12_unityV_right.jpg | right view of the stereopair                                      }"

"{@left          |testpics\\cvdemo\\ambush_5_left.jpg  | left view of the stereopair                                       }"
"{@right         |testpics\\cvdemo\\ambush_5_right.jpg | right view of the stereopair                                      }"
//"{@left          |testpics\\photos\\IMAG0032resize_left.jpg  | left view of the stereopair                                       }"
//"{@right         |testpics\\photos\\IMAG0032crop_right.jpg | right view of the stereopair                                      }"
"{GT             |None              | optional ground-truth disparity (MPI-Sintel or Middlebury format) }"
"{dst_path       |None              | optional path to save the resulting filtered disparity map        }"
"{dst_raw_path   |None              | optional path to save raw disparity map before filtering          }"
"{algorithm      |sgbm              | stereo matching method (bm or sgbm)                               }"
"{filter         |wls_conf          | used post-filtering (wls_conf or wls_no_conf)                     }"
"{no-display     |                  | don't display results                                             }"
"{no-downscale   |                  | force stereo matching on full-sized views to improve quality      }"
"{dst_conf_path  |None              | optional path to save the confidence map used in filtering        }"
"{vis_mult       |10.0              | coefficient used to scale disparity map visualizations            }"
"{num_disparity  |16                | parameter of stereo matching                                      }"
"{max_disparity  |160               | parameter of stereo matching                                      }"
"{window_size    |-1                | parameter of stereo matching                                      }"
"{wls_lambda     |8000.0            | parameter of post-filtering                                       }"
"{wls_sigma      |1.5               | parameter of post-filtering                                       }"
;

String left_im;
String right_im;
String GT_path;

String dst_path;
String dst_raw_path;
String dst_conf_path;
String algo;
String filter;
bool no_display;
bool no_downscale;
int num_disp;
int max_disp;
double wls_lambda;
double wls_sigma;
double vis_mult;
int window_size; // window_size

// int main_DepthMapCalc(int argc, char** argv)
int main(int argc, char** argv)
{
	Ptr<StereoBM>   BM_matcher;
	Ptr<StereoSGBM> SGBM_matcher;

	CommandLineParser parser(argc, argv, keys);
	parser.about("Disparity Filtering Demo");
	if (parser.has("help"))
	{
		parser.printMessage();
		return 0;
	}

	left_im = parser.get<String>(0);
	right_im = parser.get<String>(1);
	GT_path = parser.get<String>("GT");

	dst_path = parser.get<String>("dst_path");
	dst_raw_path = parser.get<String>("dst_raw_path");
	dst_conf_path = parser.get<String>("dst_conf_path");
	algo = parser.get<String>("algorithm");
	filter = parser.get<String>("filter");
	no_display = parser.has("no-display");
	no_downscale = parser.has("no-downscale");
	num_disp = parser.get<int>("num_disparity");
	max_disp = parser.get<int>("max_disparity");
	wls_lambda = parser.get<double>("wls_lambda");
	wls_sigma = parser.get<double>("wls_sigma");
	vis_mult = parser.get<double>("vis_mult");

	if (parser.get<int>("window_size") >= 0) //user provided window_size value
		window_size = parser.get<int>("window_size");
	else
	{
		if (algo == "sgbm")
			window_size = 3; //default window size for SGBM
		else if (!no_downscale && algo == "bm" && filter == "wls_conf")
			window_size = 7; //default window size for BM on downscaled views (downscaling is performed only for wls_conf)
		else
			window_size = 15; //default window size for BM on full-sized views
	}

	if (!parser.check())
	{
		parser.printErrors();
		return -1;
	}

	//! [load_views]
	Mat left = imread(left_im, IMREAD_COLOR);
	if (left.empty())
	{
		cout << "Cannot read image file: " << left_im;
		return -1;
	}

	Mat right = imread(right_im, IMREAD_COLOR);
	if (right.empty())
	{
		cout << "Cannot read image file: " << right_im;
		return -1;
	}
	//! [load_views]

	bool noGT;
	Mat GT_disp;
	if ((GT_path == "../data/aloeGT.png" && left_im != "../data/aloeL.jpg") || GT_path == "None")
		noGT = true;
	else
	{
		noGT = false;
		if (readGT(GT_path, GT_disp) != 0)
		{
			cout << "Cannot read ground truth image file: " << GT_path << endl;
			return -1;
		}
	}

	Mat left_for_matcher, right_for_matcher;
	Mat left_disp, right_disp;
	Mat filtered_disp;
	Mat conf_map = Mat(left.rows, left.cols, CV_8U);
	conf_map = Scalar(255);
	Rect ROI;
	Ptr<DisparityWLSFilter> wls_filter;
	double matching_time, filtering_time;
	if (max_disp <= 0 || max_disp % 16 != 0)
	{
		cout << "Incorrect max_disparity value: it should be positive and divisible by 16";
		return -1;
	}
	if (window_size <= 0 || window_size % 2 != 1)
	{
		cout << "Incorrect window_size value: it should be positive and odd";
		return -1;
	}
	if (filter == "wls_conf") // filtering with confidence (significantly better quality than wls_no_conf)
	{
		if (!no_downscale)
		{
			// downscale the views to speed-up the matching stage, as we will need to compute both left
			// and right disparity maps for confidence map computation
			//! [downscale]
			max_disp /= 2;
			if (max_disp % 16 != 0)
				max_disp += 16 - (max_disp % 16);
			resize(left, left_for_matcher, Size(), 0.5, 0.5, INTER_LINEAR_EXACT);
			resize(right, right_for_matcher, Size(), 0.5, 0.5, INTER_LINEAR_EXACT);
			//! [downscale]
		}
		else
		{
			left_for_matcher = left.clone();
			right_for_matcher = right.clone();
		}

		if (algo == "bm")
		{
			//! [matching]
			Ptr<StereoBM> left_matcher = StereoBM::create(max_disp, window_size);
			left_matcher->setNumDisparities(num_disp);
			wls_filter = createDisparityWLSFilter(left_matcher);
			Ptr<StereoMatcher> right_matcher = createRightMatcher(left_matcher);

			cvtColor(left_for_matcher, left_for_matcher, COLOR_BGR2GRAY);
			cvtColor(right_for_matcher, right_for_matcher, COLOR_BGR2GRAY);

			matching_time = (double)getTickCount();
			left_matcher->compute(left_for_matcher, right_for_matcher, left_disp);
			right_matcher->compute(right_for_matcher, left_for_matcher, right_disp);
			matching_time = ((double)getTickCount() - matching_time) / getTickFrequency();
			BM_matcher = left_matcher;
			//! [matching]
		}
		else if (algo == "sgbm")
		{
			Ptr<StereoSGBM> left_matcher = StereoSGBM::create(0, max_disp, window_size);
			left_matcher->setNumDisparities(num_disp);
			left_matcher->setP1(24 * window_size*window_size);
			left_matcher->setP2(96 * window_size*window_size);
			left_matcher->setPreFilterCap(63);
			left_matcher->setMode(StereoSGBM::MODE_SGBM_3WAY);
			wls_filter = createDisparityWLSFilter(left_matcher);
			Ptr<StereoMatcher> right_matcher = createRightMatcher(left_matcher);

			matching_time = (double)getTickCount();
			left_matcher->compute(left_for_matcher, right_for_matcher, left_disp);
			right_matcher->compute(right_for_matcher, left_for_matcher, right_disp);
			matching_time = ((double)getTickCount() - matching_time) / getTickFrequency();
			SGBM_matcher = left_matcher;
		}
		else
		{
			cout << "Unsupported algorithm";
			return -1;
		}

		//! [filtering]
		wls_filter->setLambda(wls_lambda);
		wls_filter->setSigmaColor(wls_sigma);
		filtering_time = (double)getTickCount();
		wls_filter->filter(left_disp, left, filtered_disp, right_disp);
		filtering_time = ((double)getTickCount() - filtering_time) / getTickFrequency();
		//! [filtering]
		conf_map = wls_filter->getConfidenceMap();

		// Get the ROI that was used in the last filter call:
		ROI = wls_filter->getROI();
		if (!no_downscale)
		{
			// upscale raw disparity and ROI back for a proper comparison:
			resize(left_disp, left_disp, Size(), 2.0, 2.0, INTER_LINEAR_EXACT);
			left_disp = left_disp * 2.0;
			ROI = Rect(ROI.x * 2, ROI.y * 2, ROI.width * 2, ROI.height * 2);
		}
	}
	else if (filter == "wls_no_conf")
	{
		/* There is no convenience function for the case of filtering with no confidence, so we
		will need to set the ROI and matcher parameters manually */

		left_for_matcher = left.clone();
		right_for_matcher = right.clone();

		if (algo == "bm")
		{
			Ptr<StereoBM> matcher = StereoBM::create(max_disp, window_size);
			matcher->setTextureThreshold(0);
			matcher->setUniquenessRatio(0);
			cvtColor(left_for_matcher, left_for_matcher, COLOR_BGR2GRAY);
			cvtColor(right_for_matcher, right_for_matcher, COLOR_BGR2GRAY);
			ROI = computeROI(left_for_matcher.size(), matcher);
			wls_filter = createDisparityWLSFilterGeneric(false);
			wls_filter->setDepthDiscontinuityRadius((int)ceil(0.33*window_size));

			matching_time = (double)getTickCount();
			matcher->compute(left_for_matcher, right_for_matcher, left_disp);
			matching_time = ((double)getTickCount() - matching_time) / getTickFrequency();
			BM_matcher = matcher;
		}
		else if (algo == "sgbm")
		{
			Ptr<StereoSGBM> matcher = StereoSGBM::create(0, max_disp, window_size);
			matcher->setUniquenessRatio(0);
			matcher->setDisp12MaxDiff(1000000);
			matcher->setSpeckleWindowSize(0);
			matcher->setP1(24 * window_size*window_size);
			matcher->setP2(96 * window_size*window_size);
			matcher->setMode(StereoSGBM::MODE_SGBM_3WAY);
			ROI = computeROI(left_for_matcher.size(), matcher);
			wls_filter = createDisparityWLSFilterGeneric(false);
			wls_filter->setDepthDiscontinuityRadius((int)ceil(0.5*window_size));

			matching_time = (double)getTickCount();
			matcher->compute(left_for_matcher, right_for_matcher, left_disp);
			matching_time = ((double)getTickCount() - matching_time) / getTickFrequency();
			SGBM_matcher = matcher;
		}
		else
		{
			cout << "Unsupported algorithm";
			return -1;
		}

		wls_filter->setLambda(wls_lambda);
		wls_filter->setSigmaColor(wls_sigma);
		filtering_time = (double)getTickCount();
		wls_filter->filter(left_disp, left, filtered_disp, Mat(), ROI);
		filtering_time = ((double)getTickCount() - filtering_time) / getTickFrequency();
	}
	else
	{
		cout << "Unsupported filter";
		return -1;
	}

	//collect and print all the stats:
	cout.precision(2);
	cout << "Matching time:  " << matching_time << "s" << endl;
	cout << "Filtering time: " << filtering_time << "s" << endl;
	cout << endl;

	double MSE_before, percent_bad_before, MSE_after, percent_bad_after;
	if (!noGT)
	{
		MSE_before = computeMSE(GT_disp, left_disp, ROI);
		percent_bad_before = computeBadPixelPercent(GT_disp, left_disp, ROI);
		MSE_after = computeMSE(GT_disp, filtered_disp, ROI);
		percent_bad_after = computeBadPixelPercent(GT_disp, filtered_disp, ROI);

		cout.precision(5);
		cout << "MSE before filtering: " << MSE_before << endl;
		cout << "MSE after filtering:  " << MSE_after << endl;
		cout << endl;
		cout.precision(3);
		cout << "Percent of bad pixels before filtering: " << percent_bad_before << endl;
		cout << "Percent of bad pixels after filtering:  " << percent_bad_after << endl;
	}

	if (dst_path != "None")
	{
		Mat filtered_disp_vis;
		getDisparityVis(filtered_disp, filtered_disp_vis, vis_mult);
		imwrite(dst_path, filtered_disp_vis);
	}
	if (dst_raw_path != "None")
	{
		Mat raw_disp_vis;
		getDisparityVis(left_disp, raw_disp_vis, vis_mult);
		imwrite(dst_raw_path, raw_disp_vis);
	}
	if (dst_conf_path != "None")
	{
		imwrite(dst_conf_path, conf_map);
	}

	if (algo == "bm")
		print_BM_params(BM_matcher);
	else
		print_SGBM_params(SGBM_matcher);

	if (!no_display)
	{
		showWindow("left" , left,  1);
		showWindow("right", right, 2);

		if (!noGT)
		{
			Mat GT_disp_vis;
			getDisparityVis(GT_disp, GT_disp_vis, vis_mult);
			showWindow("ground-truth disparity", GT_disp_vis, 6);
		}

		//! [visualization]
		Mat raw_disp_vis;
		getDisparityVis(left_disp, raw_disp_vis, vis_mult);
		showWindow("raw disparity", raw_disp_vis, 3);

		Mat filtered_disp_vis;
		getDisparityVis(filtered_disp, filtered_disp_vis, vis_mult);
		showWindow("filtered disparity", filtered_disp_vis, 4);
		waitKey();
		//! [visualization]
	}

	waitKey();
	return 0;
}

void print_BM_params(Ptr<StereoBM> bm) {
	cout << "----- Stereo BM -----" << endl;
	cout << "  DefaultName=" << bm->getDefaultName() << endl;
	cout << "  PreFilterCap=" <<  bm->getPreFilterCap() << endl;
	cout << "  UniquenessRatio=" << bm->getUniquenessRatio() << endl;
	cout << "  SpeckleRange=" << bm->getSpeckleRange() << endl;
	cout << "  SpeckleWindowSize=" << bm->getSpeckleWindowSize() << endl;
	cout << "  Disp12MaxDiff=" << bm->getDisp12MaxDiff() << endl;
	cout << "  MinDisparity=" << bm->getMinDisparity() << endl;
	cout << "  NumDisparities=" << bm->getNumDisparities() << endl;
	cout << "  MaxDisparity=" << max_disp << endl;
	cout << "  BlockSize=" << bm->getBlockSize() << endl;
	cout << "  filter=" << filter << endl;
	cout << "  no_downscale=" << no_downscale << endl;
	cout << "  window_size=" << window_size << endl;
	cout << "  vis_mult=" << fixed << vis_mult << endl;
	cout << "  wls_lambda=" << fixed << wls_lambda << endl;
	cout << "  wls_sigma=" << fixed << wls_sigma << endl;
	// bm algorithm only
	cout << "  TextureThreshold=" << bm->getTextureThreshold() << endl;
	cout << "  PreFilterSize=" << bm->getPreFilterSize() << endl;
	cout << "  PreFilterType=" << bm->getPreFilterType() << endl;
	cout << "  ROI1=" << bm->getROI1() << endl;
	cout << "  ROI2=" << bm->getROI2() << endl;
	cout << "  SmallerBlockSize=" << bm->getSmallerBlockSize() << endl;

}

void print_SGBM_params(Ptr<StereoSGBM> sgbm) {
	cout << "----- Stereo SGBM -----" << endl;
	cout << "  DefaultName=" << sgbm->getDefaultName() << endl;
	cout << "  PreFilterCap=" << sgbm->getPreFilterCap() << endl;
	cout << "  UniquenessRatio=" << sgbm->getUniquenessRatio() << endl;
	cout << "  SpeckleRange=" << sgbm->getSpeckleRange() << endl;
	cout << "  SpeckleWindowSize=" << sgbm->getSpeckleWindowSize() << endl;
	cout << "  Disp12MaxDiff=" << sgbm->getDisp12MaxDiff() << endl;
	cout << "  MinDisparity=" << sgbm->getMinDisparity() << endl;
	cout << "  NumDisparities=" << sgbm->getNumDisparities() << endl;
	cout << "  BlockSize=" << sgbm->getBlockSize() << endl;
	cout << "  filter=" << filter << endl;
	cout << "  no_downscale=" << no_downscale << endl;
	cout << "  MaxDisparity=" << max_disp << endl;
	cout << "  window_size=" << window_size << endl;
	cout << "  vis_mult=" << fixed << vis_mult << endl;
	cout << "  wls_lambda=" << fixed << wls_lambda << endl;
	cout << "  wls_sigma=" << fixed << wls_sigma << endl;
	// sgbm algorithm only
	cout << "  P1=" << sgbm->getP1() << endl;
	cout << "  P2=" << sgbm->getP2() << endl;
	cout << "  Mode=" << sgbm->getMode() << endl;
}

Rect computeROI(Size2i src_sz, Ptr<StereoMatcher> matcher_instance)
{
	int min_disparity = matcher_instance->getMinDisparity();
	int num_disparities = matcher_instance->getNumDisparities();
	int block_size = matcher_instance->getBlockSize();

	int bs2 = block_size / 2;
	int minD = min_disparity, maxD = min_disparity + num_disparities - 1;

	int xmin = maxD + bs2;
	int xmax = src_sz.width + minD - bs2;
	int ymin = bs2;
	int ymax = src_sz.height - bs2;

	Rect r(xmin, ymin, xmax - xmin, ymax - ymin);
	return r;
}


