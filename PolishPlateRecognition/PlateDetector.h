#pragma once
#include <opencv2/opencv.hpp>
class PlateDetector
{
public:
	PlateDetector();

	void detect(const cv::Mat& img);
	void showSteps(const std::string& winName);
private:
	void findRects();
	void preprocess();
	void sobel(cv::Mat& img);
	bool verifyPlateSize(const cv::RotatedRect& possiblePlate);
	cv::Mat cropRect(cv::RotatedRect & rect);
	bool verifyPossiblePlates(cv::Mat& plate);


	std::vector<cv::RotatedRect> possiblePlates;
	std::vector<cv::Mat> possiblePlatesMats;

	cv::Mat originalImage;
	cv::Mat wbImage;
	cv::Mat blurImage;
	cv::Mat tresholdImage;
	cv::Mat grayImage;
	cv::Mat sobelImage;
	cv::Mat cannyImage;
	cv::Mat contoursImage;

};

