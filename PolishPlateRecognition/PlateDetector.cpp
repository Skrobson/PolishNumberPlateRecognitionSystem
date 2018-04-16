#include "stdafx.h"
#include "PlateDetector.h"
#include "ImageProcessing.h"
#include "CharacterDetector.h"
#include <exception>
using namespace cv;
PlateDetector::PlateDetector()
{
}

void PlateDetector::detect(const cv::Mat & img)
{
	originalImage = img.clone();
	preprocess();

	findRects();

	for (auto& rect : possiblePlates)
	{
		possiblePlatesMats.push_back(cropRect(rect));
	}
	CharacterDetector det;
	for (cv::Mat & plate : possiblePlatesMats)
	{
		det.detect(plate);
		
	}


}

void PlateDetector::showSteps(const std::string & winName)
{
	int w = originalImage.size().width;
	int h = originalImage.size().height;
	cv::namedWindow(winName);
	cv::Mat win_mat(cv::Size(w * 2 , h *2), CV_8UC3);
	cv::Mat color;
	originalImage.copyTo(win_mat(cv::Rect(0, 0, w, h)));

	cv::cvtColor(tresholdImage, color, cv::COLOR_GRAY2BGR);
	color.copyTo(win_mat(cv::Rect(0, h, w, h)));
	contoursImage.copyTo(win_mat(cv::Rect(w,0, w, h)));

	cv::cvtColor(cannyImage, color, cv::COLOR_GRAY2BGR);
	color.copyTo(win_mat(cv::Rect(w, h, w, h)));
	try
	{
		cv::resize(win_mat, win_mat, cv::Size(1200, 800));
		moveWindow(winName, 0, 0);
		imshow(winName, win_mat);

		cv::waitKey(0);
		destroyWindow(winName);
	}
	catch (std::exception* e)
	{
		std::cerr << "cant show imgs. " <<e->what() <<  std::endl;
	}

}

void PlateDetector::findRects()
{
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	//
	//cv::findContours(tresholdImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	cv::findContours(cannyImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(255);
		drawContours(contoursImage, contours, i, color, 1, 8, hierarchy, 0, cv::Point());
	}

	std::vector<cv::RotatedRect> rectangles;

	for (auto& con : contours)
	{
		cv::RotatedRect boundingBox = cv::minAreaRect(con);
		
		if (verifyPlateSize(boundingBox))
		{
			// draw the rotated rect
			cv::Point2f corners[4];
			boundingBox.points(corners);
			cv::line(contoursImage, corners[0], corners[1], cv::Scalar(0, 0, 255));
			cv::line(contoursImage, corners[1], corners[2], cv::Scalar(0, 0, 255));
			cv::line(contoursImage, corners[2], corners[3], cv::Scalar(0, 0, 255));
			cv::line(contoursImage, corners[3], corners[0], cv::Scalar(0, 0, 255));

			rectangles.push_back(std::move(boundingBox));
		}
	}
	possiblePlates = rectangles;
}

void PlateDetector::preprocess()
{
	contoursImage = cv::Mat::zeros(originalImage.size(), CV_8UC3);
	originalImage = correctGamma(originalImage, 2);

	originalImage.convertTo(originalImage, -1, 0.5f, -10);
	cv::GaussianBlur(originalImage, blurImage, cv::Size(0, 0), 1, 1);

	cv::cvtColor(blurImage, grayImage, CV_BGR2GRAY);
	//grayImage = correctGamma(grayImage,15);
	sobel(grayImage);
	cv::threshold(grayImage, tresholdImage, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV);
	//cv::threshold(sobelImage, tresholdImage, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	cv::Canny(tresholdImage, cannyImage, 90, 200, 3);
}

void PlateDetector::sobel(cv::Mat & img)
{
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(img, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(img, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);

	convertScaleAbs(grad_x, abs_grad_x);
	convertScaleAbs(grad_y, abs_grad_y);

	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, sobelImage);
}
	

bool PlateDetector::verifyPlateSize(const cv::RotatedRect & possiblePlate)
{
	float error = 0.3;

	//car plate size: 520x115 aspect 4,7272
	constexpr float aspect = 52.0f/11.0f;
	//Set a min and max area. All other patches are discarded
	int min = 15 * aspect * 15; // minimum area
	//int min = 11 * 52;

	float rmin = aspect - aspect * error;
	float rmax = aspect + aspect * error;
	int area = possiblePlate.size.height * possiblePlate.size.width;
	float r = (float)possiblePlate.size.width / (float)possiblePlate.size.height;

	

	float angle = possiblePlate.angle;//std::fabs
	//
	if (r < 1)
		r = 1 / r;
	if ((area<min) || (r < rmin || r > rmax))
		return false;
	else 
	{
		if (angle > 30 || (angle > -60 && angle <-30))
			return false;
		std::cout << angle << std::endl;
		return true;
	}
}

cv::Mat PlateDetector::cropRect(cv::RotatedRect & rect)
{
	cv::Size rSize(rect.size.width, rect.size.height);
	float r = (float)rSize.width / (float)rSize.height;
	float angle = rect.angle;

	if (r < 1)
		angle = 90 + angle;
	Mat rotmat = getRotationMatrix2D(rect.center, angle, 1);
	//Create and rotate image
	Mat rotatedImage;
	warpAffine(originalImage, rotatedImage, rotmat, originalImage.size(), CV_INTER_CUBIC);

	if (r < 1)
		swap(rSize.width, rSize.height);

	Mat cropedImage;
	getRectSubPix(rotatedImage, rSize, rect.center, cropedImage);


	//possiblePlatesMats.push_back(std::move(cropedImage));

	return cropedImage.clone();
}

bool PlateDetector::verifyPossiblePlates(cv::Mat & plate)
{
	return false;
}








