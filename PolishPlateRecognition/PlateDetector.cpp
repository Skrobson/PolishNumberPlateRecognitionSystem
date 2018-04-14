#include "stdafx.h"
#include "PlateDetector.h"

using namespace cv;
PlateDetector::PlateDetector()
{
}

void PlateDetector::preprocess()
{
	Mat img_gray;

	cvtColor(originalImage, img_gray, CV_BGR2GRAY);	//Gray image
	equalizeHist(img_gray, img_gray); // Historgram normailze



	blur(img_gray, img_gray, Size(5, 5));


	Mat img_sobel;
	Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0);	//Sobel filter to find vertical edges

	threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY); // binary image with a threshold value
																					//threshold(img_gray, img_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY); // binary image with a threshold value

}

