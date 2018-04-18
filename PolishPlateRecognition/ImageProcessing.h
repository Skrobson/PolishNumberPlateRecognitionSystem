#pragma once
#include <opencv2/opencv.hpp>


cv::Mat deskew(const cv::Mat& image);

cv::Mat correctGamma(const cv::Mat& img, double gamma);

std::vector<cv::Vec4i> detectLines(const cv::Mat& contours);

cv::Point2f computeIntersect(const cv::Vec4i& a, const cv::Vec4i& b);

std::pair<float, float> computeStraithLineEquation(const cv::Point2f& p1, const cv::Point2f& p2);

float distancePointLine(const cv::Point2f& p, float a, float b, float c = 0);
