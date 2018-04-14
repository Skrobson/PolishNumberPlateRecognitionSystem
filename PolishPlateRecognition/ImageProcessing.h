#pragma once
#include <opencv2/opencv.hpp>


cv::Mat deskew(const cv::Mat& image);

/**

*/
std::vector<cv::RotatedRect> findRectangles(const cv::Mat& image);

std::vector<cv::RotatedRect> findRectangles2(const cv::Mat& image);

cv::Mat correctGamma(const cv::Mat& img, double gamma);

std::vector<cv::Vec4i> detectLines(const cv::Mat& contours);

cv::Point2f computeIntersect(const cv::Vec4i& a, const cv::Vec4i& b);

bool PlateSearch::verifySizes(RotatedRect candidate)
{
	float error = 0.4;
	//Spain car plate size: 52x11 aspect 4,7272
	const float aspect = 4.7272;
	//Set a min and max area. All other patches are discarded
	int min = 15 * aspect * 15; // minimum area
	int max = 125 * aspect * 125; // maximum area
								  //Get only patches that match to a respect ratio.
	float rmin = aspect - aspect * error;
	float rmax = aspect + aspect * error;
	int area = candidate.size.height * candidate.size.width;
	float r = (float)candidate.size.width / (float)candidate.size.height;
	if (r < 1)
		r = 1 / r;
	if ((area < min || area > max) || (r < rmin || r > rmax)) {
		return false;
	}
	else {
		return true;
	}
}