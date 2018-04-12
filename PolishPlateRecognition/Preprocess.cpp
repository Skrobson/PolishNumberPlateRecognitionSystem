#include "stdafx.h"
#include "Preprocess.h"


Preprocess::Preprocess() : x(16), y(32), hog(
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

cv::Mat Preprocess::preprocessTrainData(const cv::Mat& img)
{
	cv::Mat resizedImg;
	cv::resize(img, resizedImg, cv::Size(x,y));
	cv::Mat output;
	//deskew
	int SZ = 20;
	cv::Moments m = moments(resizedImg);
	if (abs(m.mu02) < 1e-2)
	{
		// No deskewing needed. 
		output = calcHOG(resizedImg);
		return output.clone();
	}
	// Calculate skew based on central momemts. 
	double skew = m.mu11 / m.mu02;
	// Calculate affine transform to correct skewness. 
	cv::Mat warpMat = (cv::Mat_<double>(2, 3) << 1, skew, -0.5*SZ*skew, 0, 1, 0);

	cv::Mat imgOut = cv::Mat::zeros(resizedImg.rows, resizedImg.cols, resizedImg.type());
	cv::warpAffine(resizedImg, imgOut, warpMat, imgOut.size(), (cv::InterpolationFlags::WARP_INVERSE_MAP | cv::InterpolationFlags::INTER_LINEAR));

	output = calcHOG(resizedImg);
	return output.clone();
}

cv::Mat Preprocess::calcHOG(const cv::Mat & img)
{
	std::vector<float> descriptors;
	hog.compute(img, descriptors);

	cv::Mat  des = cv::Mat(descriptors);
	des = des.reshape(0, 1);
	return des.clone();
}


