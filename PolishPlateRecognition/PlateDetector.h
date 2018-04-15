#pragma once
#include "Plate.h"

#include <opencv2/opencv.hpp>
class PlateDetector
{
public:
	PlateDetector();

	Plate detect(const cv::Mat& img);
	void showSteps(const std::string& winName);
private:
	void findRects();
	void preprocess();
	void sobel(cv::Mat& img);
	bool verifyPlateSize(const RotatedRect& possiblePlate);
	cv::Mat cropRect(cv::RotatedRect & rect);
	bool verifyPossiblePlates(cv::Mat& plate);


	float computeAverageHeight(std::vector<RotatedRect>& rects);
	std::vector<cv::Mat> findChars(cv::Mat&);
	bool verifyCharacterSize(const RotatedRect& possiblePlate);
	bool secondCharacterVerification(const RotatedRect& possiblePlate , float avgHeight);


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

