#include "stdafx.h"
#include "ImageProcessing.h"

cv::Mat deskew(const cv::Mat & image)
{

	const int SZ = 20;
	cv::Moments m = cv::moments(image);
	if (fabs(m.mu02) < 1e-2)
	{
		// No deskewing needed. 
		return image.clone();
	}
	// Calculate skew based on central momemts. 
	double skew = m.mu11 / m.mu02;
	// Calculate affine transform to correct skewness. 
	cv::Mat warpMat = (cv::Mat_<double>(2, 3) << 1, skew, -0.5*SZ*skew, 0, 1, 0);

	cv::Mat imgOut = cv::Mat::zeros(image.rows, image.cols, image.type());
	cv::warpAffine(image, imgOut, warpMat, imgOut.size(), (cv::InterpolationFlags::WARP_INVERSE_MAP | cv::InterpolationFlags::INTER_LINEAR));
	
	return imgOut.clone();
}

std::vector<cv::RotatedRect> findRectangles(const cv::Mat & image)
{
	cv::Mat drawing = cv::Mat::zeros(image.size(), CV_8UC3);
	cv::Mat grayImg;
	cv::cvtColor(image, grayImg , CV_BGR2GRAY);	//Gray image

	cv::Mat treshImg;
	cv::threshold(grayImg, treshImg, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

	
	
	// find contours (if always so easy to segment as your image, you could just add the black/rect pixels to a vector)
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	//
	cv::findContours(treshImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	
	// Draw contours and find biggest contour (if there are other contours in the image, we assume the biggest one is the desired rect)
	// drawing here is only for demonstration!
	int biggestContourIdx = -1;
	float biggestContourArea = 0;
	
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(255, 255, 255);
		drawContours(drawing, contours, i, color, 1, 8, hierarchy, 0, cv::Point());
		
		float ctArea = cv::contourArea(contours[i]);
		if (ctArea > biggestContourArea)
		{
			biggestContourArea = ctArea;
			biggestContourIdx = i;
		}
	}
	detectLines(drawing);
	// if no contour found
	if (biggestContourIdx < 0)
	{
		std::cout << "no contour found" << std::endl;
		return std::vector<cv::RotatedRect>();
	}

	std::vector<cv::RotatedRect> rectangles;

	for (auto& con : contours)
	{
		// compute the rotated bounding rect of the biggest contour! (this is the part that does what you want/need)
		cv::RotatedRect boundingBox = cv::minAreaRect(con);
		// one thing to remark: this will compute the OUTER boundary box, so maybe you have to erode/dilate if you want something between the ragged lines

		rectangles.push_back(boundingBox);

		// draw the rotated rect
		cv::Point2f corners[4];
		boundingBox.points(corners);
		cv::line(drawing, corners[0], corners[1], cv::Scalar(0, 0, 255));
		cv::line(drawing, corners[1], corners[2], cv::Scalar(0, 0, 255));
		cv::line(drawing, corners[2], corners[3], cv::Scalar(0, 0,255));
		cv::line(drawing, corners[3], corners[0], cv::Scalar(0, 0, 255));

	}
	
	// display
	cv::imshow("input", image);
	cv::imshow("drawing", drawing);
	cv::waitKey(0);
	return rectangles;
}

std::vector<cv::RotatedRect> findRectangles2(const cv::Mat & image)
{
	return std::vector<cv::RotatedRect>();
}

cv::Point2f computeIntersect(const cv::Vec4i& a,const cv::Vec4i& b)
{
	int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3], x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];
	float denom;

	if (float d = ((float)(x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4)))
	{
		cv::Point2f pt;
		pt.x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / d;
		pt.y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / d;
		return pt;
	}
	else
		return cv::Point2f(-1, -1);
}

cv::Mat correctGamma(const cv::Mat& img, double gamma) {
	double inverse_gamma = 1.0 / gamma;

	cv::Mat lut_matrix(1, 256, CV_8UC1);
	uchar * ptr = lut_matrix.ptr();
	for (int i = 0; i < 256; i++)
		ptr[i] = (int)(pow((double)i / 255.0, inverse_gamma) * 255.0);

	cv::Mat result;
	cv::LUT(img, lut_matrix, result);

	return result.clone();
}

std::vector<cv::Vec4i> detectLines(const cv::Mat & contours)
{
	cv::Mat drawing = cv::Mat::zeros(contours.size(), CV_8UC3);
	cv::Mat img(contours);
	cv::cvtColor(img, img, CV_BGR2GRAY);
	
	cv::blur(img, img, cv::Point(3, 3));
	cv::threshold(img, img, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	cv::Canny(img, img,70, 200, 3);

	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(img, lines,0.5f, CV_PI / 360,10,1,1);

	// Expand the lines
	//for (int i = 0; i < lines.size(); i++)
	//{
	//	cv::Vec4i v = lines[i];
	//	lines[i][0] = 0;
	//	lines[i][1] = ((float)v[1] - v[3]) / (v[0] - v[2]) * -v[0] + v[1];
	//	lines[i][2] = img.cols;
	//	lines[i][3] = ((float)v[1] - v[3]) / (v[0] - v[2]) * (img.cols - v[2]) + v[3];
	//}

	std::vector<cv::Point2f> corners;
	for (int i = 0; i < lines.size(); i++)
	{
		for (int j = i + 1; j < lines.size(); j++)
		{
			cv::Point2f pt = computeIntersect(lines[i], lines[j]);
			if (pt.x >= 0 && pt.y >= 0)
				corners.push_back(pt);
		}
	}

	for (size_t i = 0; i < lines.size(); i++)
	{
		auto line = lines[i];
		int x1 = lines[i][0];

		int x2 = lines[i][2];

		int y1 = lines[i][1];

		int y2 = lines[i][3];

		cv::line(drawing, cv::Point(x1, y1),

			cv::Point(x2, y2), cv::Scalar(0, 0, 255));
	}
	cv::imshow("input", img);
	cv::imshow("drawing2", drawing);
	cv::waitKey(0);

	return std::vector<cv::Vec4i>();
}
