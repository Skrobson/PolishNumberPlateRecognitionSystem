#pragma once
#include <opencv2/opencv.hpp>
class CharacterPreprocess
{
public:
	CharacterPreprocess();
	/**
	Preprocess pipeline for treshold image of char
	*/
	cv::Mat preprocessImage(const cv::Mat& img);
private:

	cv::Mat calcHOG(const cv::Mat& img);
	cv::HOGDescriptor hog;

	int x = 16;
	int y = 32;
};


