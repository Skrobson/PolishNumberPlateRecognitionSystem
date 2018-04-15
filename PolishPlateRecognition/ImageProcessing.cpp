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

cv::Point2f computeIntersect(const cv::Vec4i& a,const cv::Vec4i& b)
{
	int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3], x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];

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
	cv::Mat img ;
	cv::cvtColor(contours, img, CV_BGR2GRAY);
	
	//cv::blur(img, img, cv::Point(3, 3));
	
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









//void PlateSearch::findPlate()
//{
//	vector<RotatedRect> rects = findRectangles(image);
//
//
//
//	Mat result;
//
//	for (int i = 0; i < rects.size(); i++) {
//
//		if (rects[i].size.width > 1 && rects[i].size.height > 1)
//		{
//			//Get rotation matrix
//			float r = (float)rects[i].size.width / (float)rects[i].size.height;
//			float angle = rects[i].angle;
//			if (r < 1)
//				angle = 90 + angle;
//			Mat rotmat = getRotationMatrix2D(rects[i].center, angle, 1);
//			//Create and rotate image
//			Mat img_rotated;
//			warpAffine(image, img_rotated, rotmat, image.size(), CV_INTER_CUBIC);
//			//Crop image
//			Size rect_size = rects[i].size;
//			if (r < 1)
//				swap(rect_size.width, rect_size.height);
//			Mat img_crop;
//			getRectSubPix(img_rotated, rect_size, rects[i].center, img_crop);
//			Mat resultResized;
//			resultResized.create(33, 144, CV_8UC3);
//			resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);
//			//Equalize cropped image
//			Mat grayResult;
//			cvtColor(resultResized, grayResult, CV_BGR2GRAY);
//			blur(grayResult, grayResult, Size(3, 3));
//			equalizeHist(grayResult, grayResult);
//			imshow("LL", grayResult);
//			waitKey();
//			cut(grayResult);
//		}
//
//	}
//}


//void PlateSearch::cut(Mat input)
//{
//	Mat img_threeshold;
//	Mat grey;
//
//	grey = correctGamma(input, 1);
//	threshold(grey, img_threeshold, 60, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C);
//	//equalizeHist(img_threeshold, img_threeshold);
//
//	Size s = img_threeshold.size();
//
//	bool detected_start = false;
//	bool detected_end = false;
//	bool detected = false;
//
//	vector<int> start;
//	vector<int> stop;
//	int height = s.height;
//	bool detInColumn = false, detCont = false;
//	for (int x = 0; x < s.width; x++)
//	{
//
//		for (int y = 2; (y <= height - 2) && (detInColumn == false); y++)
//		{
//			Scalar colour = img_threeshold.at<uchar>(Point(x, y));
//			if (colour.val[0] == 255) { // znaleziono
//				detInColumn = true;
//			}
//		}
//		if ((detInColumn == true) && (detCont == false)) {
//			start.push_back(x);
//			detCont = true;
//
//		}
//		else if ((detInColumn == false) && (detCont == true)) {
//			stop.push_back(x);
//			detCont = false;
//		}
//		detInColumn = false;
//	}
//
//	if (start.size() > stop.size())
//	{
//		stop.push_back(img_threeshold.size().width);
//	}
//
//	int wordsCounter = (start.size() <= stop.size() ? start.size() : stop.size());
//
//	int maxWidth = 0;
//	{
//		for (int i = 0; i < wordsCounter; i++)
//		{
//			if (maxWidth < stop[i] - start[i]) maxWidth = stop[i] - start[i];
//		}
//
//	}
//
//
//	if (wordsCounter > 5 && wordsCounter < 10) {
//		for (int i = 0; i < wordsCounter; i++) {
//			Mat cropedImage;
//			if (i == 0 && (start[i] <= 0)) {
//				cropedImage = img_threeshold(Rect(start[i], 0, stop[i] - start[i] + 2, height));
//			}
//			else if (i == wordsCounter && stop[i] <= (stop[i] - start[i] + 2))
//			{
//				cropedImage = img_threeshold(Rect(start[i] - 2, 0, stop[i], height));
//			}
//			else {
//				cropedImage = img_threeshold(Rect(start[i] - 2, 0, stop[i] - start[i] + 2, height));
//			}
//			//imshow("color result", cropedImage);
//			if (cropedImage.size().width > 3 && cropedImage.size().width < 30)
//			{
//				//String nazwa = "c:\\test\\";
//				//nazwa += std::to_string(time(0));
//				//nazwa += std::to_string(i);
//				//nazwa += ".jpg";
//				//imwrite(nazwa, cropedImage);
//				number.push_back(cropedImage);
//			}
//			//waitKey(0);
//		}
//	}
//	//imshow("color result", img_threeshold);
//}