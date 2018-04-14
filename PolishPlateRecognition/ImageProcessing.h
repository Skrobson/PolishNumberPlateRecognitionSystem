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