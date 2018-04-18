#include "stdafx.h"
#include "CharacterPreprocess.h"
#include "ImageProcessing.h"

CharacterPreprocess::CharacterPreprocess() : x(16), y(32), hog(
	cv::Size(16, 32), //winSize
	cv::Size(8, 16), //blocksize
	cv::Size(4, 8), //blockStride,
	cv::Size(8, 16), //cellSize,
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

cv::Mat CharacterPreprocess::preprocessImage(const cv::Mat& img)
{
	cv::Mat output = deskew(img);

	cv::resize(output, output, cv::Size(x,y),0,0, cv::INTER_NEAREST);

	
	output = calcHOG(output);

	return output.clone();
}

cv::Mat CharacterPreprocess::calcHOG(const cv::Mat & img)
{
	std::vector<float> descriptors;
	hog.compute(img, descriptors);

	cv::Mat  des = cv::Mat(descriptors);
	des = des.reshape(0, 1);
	return des.clone();
}


