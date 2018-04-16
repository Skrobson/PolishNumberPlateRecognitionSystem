#pragma once
#include <opencv2/opencv.hpp>

class PossiblePlate
{
public:
	PossiblePlate() = delete;
	PossiblePlate(const cv::Mat& plateImage);

	bool isValid() { return valid; }

	std::vector<cv::Mat> extractCharacters();
private:

	bool valid = false;
	
	cv::Mat originalImage;
};

