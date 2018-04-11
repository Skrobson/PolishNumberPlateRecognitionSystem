#include "stdafx.h"
#include "Preprocess.h"


Preprocess::Preprocess() : hog(
	cv::Size(32, 32), //winSize
	cv::Size(8, 8), //blocksize
	cv::Size(4, 4), //blockStride,
	cv::Size(8, 8), //cellSize,
	9, //nbins,
	1, //derivAper,
	-1, //winSigma,
	0, //histogramNormType,
	0.2, //L2HysThresh,
	1,//gammal correction,
	64,//nlevels=64
	1)//Use signed gradients)
{
}

cv::Mat Preprocess::preprocessTrainData(const cv::Mat& img)
{
	std::vector<float> descriptors;
	hog.compute(img, descriptors);
	cv::Mat  des = cv::Mat(descriptors);
	des = des.reshape(0, 1);
	return des.clone();
}


