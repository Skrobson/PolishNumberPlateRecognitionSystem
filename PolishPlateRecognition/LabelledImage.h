#pragma once
#include <opencv2/opencv.hpp>
class LabelledImage
{
public:
	LabelledImage(cv::Mat&& data, char className);
	LabelledImage(const cv::Mat& data, char className);

	cv::Mat getData()const;
	cv::Mat getClassCode()const;
	char getClassName()const;

private:

	cv::Mat classCode;
	char className;
	cv::Mat data;

};

