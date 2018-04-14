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

