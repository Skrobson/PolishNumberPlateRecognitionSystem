#include "stdafx.h"
#include "PlateSearch.h"
#include "ImageProcessing.h"


PlateSearch::PlateSearch()
{
}

PlateSearch::PlateSearch(String path)
{
	image = imread(path, CV_LOAD_IMAGE_COLOR);   // Read the file

	if (!image.data) {
		cout << "Could not open or find the image" << endl;
	}

	else if (image.size().width > 1400) {
		Size size(image.size().width / 2, image.size().height / 2);
		resize(image, image, size);
	}


	//cv::imshow("original", image);
	//cv::waitKey();
}


PlateSearch::~PlateSearch()
{
}

void PlateSearch::convert()
{
	//static int tryCount = 0;
	Mat img_new;
	image.convertTo(img_new, -1, 2.0, 10);

	cvtColor(img_new, img_gray, CV_BGR2GRAY);	//Gray image
												//if(tryCount%2)	blur(img_gray, img_gray, Size(5, 5));
												//else
	GaussianBlur(img_gray, img_gray, Size(0, 0), 2, 2);

	img_gray = correctGamma(img_gray, 3);

	equalizeHist(img_gray, img_gray);
	//tryCount++;
	findPlate();
}

vector<Mat> PlateSearch::getMatVector()
{
	return number;
}

void PlateSearch::findPlate()
{
	//vector<RotatedRect> rects = findRectangles(image);
	//
	//
	//
		//Mat result;
	
		//for (int i = 0; i < rects.size(); i++) {
	
		//	if (rects[i].size.width > 1 && rects[i].size.height > 1)
		//	{
		//		//Get rotation matrix
		//		float r = (float)rects[i].size.width / (float)rects[i].size.height;
		//		float angle = rects[i].angle;
		//		if (r < 1)
		//			angle = 90 + angle;
		//		Mat rotmat = getRotationMatrix2D(rects[i].center, angle, 1);
		//		//Create and rotate image
		//		Mat img_rotated;
		//		warpAffine(image, img_rotated, rotmat, image.size(), CV_INTER_CUBIC);
		//		//Crop image
		//		Size rect_size = rects[i].size;
		//		if (r < 1)
		//			swap(rect_size.width, rect_size.height);
		//		Mat img_crop;
		//

		//		getRectSubPix(img_rotated, rect_size, rects[i].center, img_crop);
		//		if (isPlate(img_crop) == true) {
		//			cout << "Dobbry\n";
		//		}
		//		//if (r < 1)
		//		//	swap(rect_size.width, rect_size.height);
		//		//Mat img_crop;
		//		//getRectSubPix(img_rotated, rect_size, rects[i].center, img_crop);
		//		//Mat resultResized;
		//		//resultResized.create(33, 144, CV_8UC3);
		//		//resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);
		//		//imshow("LL", resultResized);
		//		//waitKey();
		//		////Equalize cropped image
		//		//Mat grayResult;
		//		//cvtColor(resultResized, grayResult, CV_BGR2GRAY);
		//		//blur(grayResult, grayResult, Size(3, 3));
		//		//equalizeHist(grayResult, grayResult);
		//		//cut(grayResult);

		//		else {
		//			cout << "Chujowy\n";
		//			//	imshow("LL", resultResized);
		//			//	waitKey();
		//		}
		//		Mat resultResized;
		//		resultResized.create(33, 144, CV_8UC3);
		//		resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);
		//		imshow("rs", img_crop);

		//
		//		//Equalize cropped image
		//		Mat grayResult;
		//		cvtColor(resultResized, grayResult, CV_BGR2GRAY);
		//		blur(grayResult, grayResult, Size(3, 3));
		//		equalizeHist(grayResult, grayResult);
		//		imshow("LL", grayResult);
		//		waitKey();
		//		//cut(grayResult);
	
		//	}

		//	
		//	//namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
		//	//imshow("Display window", grayResult);
		//	//waitKey(0);
		//}
	}



	//Check new floodfill mask match for a correct patch.
	//Get all points detected for minimal rotated Rect


	//namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
	//imshow("Display window", img_threshold);


	//cut(img_threshold);

	//waitKey(0);                                          // Wait for a keystroke in the window


bool PlateSearch::isPlate(Mat img) {
	int height = img.size().height;
	int width = img.size().width*0.1;
	int isBlue = 0, pixels = 0;
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++) {

			Vec3b intensity = img.at<Vec3b>(y, x);
			uchar blue = intensity.val[0];
			uchar green = intensity.val[1];
			uchar red = intensity.val[2];
			//cout << float(blue) << " " << float(green) << " " << float(red) << endl;
			if ((blue > green) && (blue > red)) isBlue++;
			pixels++;
		}
	}
	cout << endl << (float(isBlue) / float(pixels) * 100) << endl;
	if ((float(isBlue) / float(pixels) * 100) > 60) return true;
	else return false;
}

void PlateSearch::cut(Mat input)
{
	Mat img_threeshold;
	Mat grey;

	grey = correctGamma(input, 1);
	threshold(grey, img_threeshold, 60, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C);
	//equalizeHist(img_threeshold, img_threeshold);

	Size s = img_threeshold.size();

	bool detected_start = false;
	bool detected_end = false;
	bool detected = false;

	vector<int> start;
	vector<int> stop;
	int height = s.height;
	bool detInColumn = false, detCont = false;
	for (int x = 0; x < s.width; x++)
	{
		for (int y = 2; (y <= height - 2) && (detInColumn == false); y++)
		{
			Scalar colour = img_threeshold.at<uchar>(Point(x, y));
			if (colour.val[0] == 255) { // znaleziono
				detInColumn = true;
			}
		}
		if ((detInColumn == true) && (detCont == false)) {
			start.push_back(x);
			detCont = true;

		}
		else if ((detInColumn == false) && (detCont == true)) {
			stop.push_back(x);
			detCont = false;
		}
		detInColumn = false;
	}

	if (start.size() > stop.size())
	{
		stop.push_back(img_threeshold.size().width);
	}

	int wordsCounter = (start.size() <= stop.size() ? start.size() : stop.size());

	int maxWidth = 0;
	{
		for (int i = 0; i < wordsCounter; i++)
		{
			if (maxWidth < stop[i] - start[i]) maxWidth = stop[i] - start[i];
		}

	}


	if (wordsCounter > 5 && wordsCounter < 10) {
		for (int i = 0; i < wordsCounter; i++) {
			Mat cropedImage;
			if (i == 0 && (start[i] <= 0)) {
				cropedImage = img_threeshold(Rect(start[i], 0, stop[i] - start[i] + 2, height));
			}
			else if (i == wordsCounter && stop[i] <= (stop[i] - start[i] + 2))
			{
				cropedImage = img_threeshold(Rect(start[i] - 2, 0, stop[i], height));
			}
			else {
				cropedImage = img_threeshold(Rect(start[i] - 2, 0, stop[i] - start[i] + 2, height));
			}
			//imshow("color result", cropedImage);
			if (cropedImage.size().width > 3 && cropedImage.size().width < 30)
			{
				Size s = cropedImage.size();
				int pixels = 0, isWhite = 0;
				for (int x = 0; x < s.width; x++)
				{
					for (int y = 0; (y < s.height); y++)
					{
						Scalar colour = img_threeshold.at<uchar>(Point(x, y));
						if (colour.val[0] == 255) isWhite++;
						pixels++;
					}
				}
				if ((float(isWhite) / float(pixels) * 100) > 15) number.push_back(cropedImage);

			}
			//waitKey(0);
		}
	}
	//imshow("color result", img_threeshold);
}


Mat PlateSearch::correctGamma(Mat & img, double gamma)
{
	double inverse_gamma = 1.0 / gamma;

	Mat lut_matrix(1, 256, CV_8UC1);
	uchar * ptr = lut_matrix.ptr();
	for (int i = 0; i < 256; i++)
		ptr[i] = (int)(pow((double)i / 255.0, inverse_gamma) * 255.0);

	Mat result;
	LUT(img, lut_matrix, result);

	return result;
}

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
