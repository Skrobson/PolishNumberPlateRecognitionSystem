#pragma once
#include "Plate.h"

#include <opencv2/opencv.hpp>
class PlateDetector
{
public:
	PlateDetector();

	Plate detect(const cv::Mat& img);

private:

	void preprocess();

	cv::Mat originalImage;
	cv::Mat img_threshold;
};

//bool verifySizes(cv::RotatedRect candidate)
//{
//	float error = 0.4;
//	//Spain car plate size: 52x11 aspect 4,7272
//	//const float aspect = 4.7272;
//	constexpr float aspect = 520 / 114;
//	//Set a min and max area. All other patches are discarded
//	int min = 15 * aspect * 15; // minimum area
//	int max = 125 * aspect * 125; // maximum area
//								  //Get only patches that match to a respect ratio.
//	float rmin = aspect - aspect * error;
//	float rmax = aspect + aspect * error;
//
//	float r = (float)candidate.size.width / (float)candidate.size.height;
//	if (r < 1)
//		r = 1 / r;
//	if ((r < rmin || r > rmax)) {
//		return false;
//	}
//	else {
//		return true;
//	}
//}