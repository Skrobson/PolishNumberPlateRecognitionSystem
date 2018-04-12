#pragma once
#include <opencv2/opencv.hpp>
class Preprocess
{
public:
	Preprocess();

	cv::Mat preprocessTrainData(const cv::Mat& img);
private:

	cv::Mat calcHOG(const cv::Mat& img);
	cv::HOGDescriptor hog;

	int x = 16;
	int y = 32;
};

