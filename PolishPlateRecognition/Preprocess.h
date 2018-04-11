#pragma once
#include <opencv2/opencv.hpp>
class Preprocess
{
public:
	Preprocess();

	cv::Mat preprocessTrainData(const cv::Mat& img);
private:
	cv::HOGDescriptor hog;
};

