// PolishPlateRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ImagePipeline.h"
#include "ClassDescriptor.h"
#include "CharacterRecognitionNetwork.h"
#include "CharacterPreprocess.h"
#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>
#include "Plate.h"
#include <ctime>
#include "ImageProcessing.h"	
void train(const std::string& networkName, const std::string& learningDir);

void test(const std::vector<std::string>& testFiles, const CharacterRecognitionNetwork& network);

void predict(const std::string& networkName, const std::string& fileName);



using namespace cv;
using namespace std;

void cut(Mat& gray);
Mat brighter(Mat& image);

bool verifySizes(RotatedRect& candidate);

int main(int argc, char** argv)
{/*
	if (argc != 2)
	{
		cout << " Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}
*/
	Mat image;
	image = imread("../Data/10.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file

	if (!image.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	findRectangles(image);
	Mat img_gray;

	//imshow("okno", image);
	cvtColor(image, img_gray, CV_BGR2GRAY);	//Gray image
											//blur(img_gray, img_gray, Size(5, 5));
	img_gray = correctGamma(img_gray, 1);
	//equalizeHist(img_gray, img_gray);
	//imshow("opsdad", img_gray);
	//Mat img_sobel;

	//Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0);	//Sobel filter to find vertical edges

	Mat img_threshold;
	//threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY); // binary image with a threshold value
	threshold(img_gray, img_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	//test
	


	//Mat element = getStructuringElement(MORPH_RECT, Size(17, 3));
	//morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element);

	//vector< vector< Point> > contours;
	//findContours(img_threshold,
	//	contours, // a vector of contours
	//	CV_RETR_EXTERNAL, // retrieve the external contours
	//	CV_CHAIN_APPROX_NONE); // all pixels of each contour

	//						   //Start to iterate to each contour found
	//vector<vector<Point> >::iterator itc = contours.begin();
	//vector<RotatedRect> rects;
	//vector<Plate> output;
	////Remove patch that has no inside limits of aspect ratio and area.
	//while (itc != contours.end()) {
	//	//Create bounding rect of object
	//	RotatedRect mr = minAreaRect(Mat(*itc));
	//	if (!verifySizes(mr)) {
	//		itc = contours.erase(itc);
	//	}
	//	else {
	//		++itc;
	//		rects.push_back(mr);
	//	}
	//}

	//Mat result;

	//for (int i = 0; i < rects.size(); i++) {
	//	//For better rect cropping for each possible box
	//	//Make floodfill algorithm because the plate has white background
	//	//And then we can retrieve more clearly the contour box
	//	circle(result, rects[i].center, 3, Scalar(0, 255, 0), -1);
	//	//get the min size between width and height
	//	float minSize = (rects[i].size.width < rects[i].size.height) ? rects[i].size.width : rects[i].size.height;
	//	minSize = minSize - minSize * 0.5;
	//	//initialize rand and get 5 points around center for floodfill algorithm
	//	srand(time(NULL));
	//	//Initialize floodfill parameters and variables
	//	Mat mask;
	//	mask.create(image.rows + 2, image.cols + 2, CV_8UC1);

	//	mask = Scalar::all(0);
	//	int loDiff = 30;
	//	int upDiff = 30;
	//	int connectivity = 4;
	//	int newMaskVal = 255;
	//	int NumSeeds = 10;
	//	Rect ccomp;
	//	int flags = connectivity + (newMaskVal << 8) + CV_FLOODFILL_FIXED_RANGE + CV_FLOODFILL_MASK_ONLY;
	//	for (int j = 0; j < NumSeeds; j++) {
	//		Point seed;
	//		seed.x = rects[i].center.x + rand() % (int)minSize - (minSize / 2);
	//		seed.y = rects[i].center.y + rand() % (int)minSize - (minSize / 2);
	//		circle(result, seed, 1, Scalar(0, 255, 255), -1);
	//		int area = floodFill(image, mask, seed, Scalar(255, 0, 0), &ccomp,
	//			Scalar(loDiff, loDiff, loDiff), Scalar(upDiff, upDiff, upDiff),
	//			flags);
	//	}
	//	vector<Point> pointsInterest;
	//	Mat_<uchar>::iterator itMask = mask.begin<uchar>();
	//	Mat_<uchar>::iterator end = mask.end<uchar>();
	//	for (; itMask != end; ++itMask)
	//		if (*itMask == 255)
	//			pointsInterest.push_back(itMask.pos());
	//	RotatedRect minRect = minAreaRect(pointsInterest);
	//	if (verifySizes(minRect)) {
	//		//Get rotation matrix
	//		float r = (float)minRect.size.width / (float)minRect.size.height;
	//		float angle = minRect.angle;
	//		if (r < 1)
	//			angle = 90 + angle;
	//		Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1);
	//		//Create and rotate image
	//		Mat img_rotated;
	//		warpAffine(image, img_rotated, rotmat, image.size(), CV_INTER_CUBIC);
	//		//Crop image
	//		Size rect_size = minRect.size;
	//		if (r < 1)
	//			swap(rect_size.width, rect_size.height);
	//		Mat img_crop;
	//		getRectSubPix(img_rotated, rect_size, minRect.center, img_crop);
	//		Mat resultResized;
	//		resultResized.create(33, 144, CV_8UC3);
	//		resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);
	//		//Equalize cropped image
	//		Mat grayResult;
	//		cvtColor(resultResized, grayResult, CV_BGR2GRAY);
	//		blur(grayResult, grayResult, Size(3, 3));
	//		equalizeHist(grayResult, grayResult);

	//		cut(grayResult);
	//		namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
	//		imshow("Display window", grayResult);
	//		waitKey(0);
	//	}
	//}

	//Check new floodfill mask match for a correct patch.
	//Get all points detected for minimal rotated Rect


	//namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
	//imshow("Display window", img_threshold);                   // Show our image inside it.

	//waitKey(0);     // Wait for a keystroke in the window
	std::getchar();
	return 0;
}







bool verifySizes(RotatedRect& candidate) {
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


void cut(Mat& input)
{
	//input = correctGamma(input,0.8);
	Mat img_threeshold;
	Mat grey;
	//cvtColor(input, grey, CV_BGR2GRAY);
	grey = correctGamma(input, 1);
	threshold(grey, img_threeshold, 60, 255, CV_THRESH_BINARY_INV);
	equalizeHist(img_threeshold, img_threeshold);
	//Mat img_contours;

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

		for (int y = 5; (y <= height - 5) && (detInColumn == false); y++)
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


	if (wordsCounter >= 5) {
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
			imshow("color result", cropedImage);
			String nazwa = "c:\\test\\";
			nazwa += std::to_string(time(0));
			nazwa += std::to_string(i);
			nazwa += ".jpg";
			imwrite(nazwa, cropedImage);
			waitKey(0);
		}
	}
	imshow("color result", img_threeshold);


}

Mat brighter(Mat& image) {
	Mat new_image = Mat::zeros(image.size(), image.type());

	double alpha = 2.0;
	int beta = 50;

	/// Do the operation new_image(i,j) = alpha*image(i,j) + beta
	for (int y = 0; y < image.rows; y++)
	{
		for (int x = 0; x < image.cols; x++)
		{
			for (int c = 0; c < 3; c++)
			{
				new_image.at<Vec3b>(y, x)[c] =
					saturate_cast<uchar>(alpha*(image.at<Vec3b>(y, x)[c]) + beta);
			}
		}
	}
	return new_image;
}





//int main(int argc, char** argv)
//{
//
//	//if (argc == 4)
//	//{
//	//	if (argv[1] == "-t")
//	//		train(argv[2], argv[3]);
//
//	//	else if (argv[1] == "-p")
//	//		predict(argv[2], argv[3]);
//
//	//}
//	//train("trainedNetwork3" , "../Data/arklatrs");
//
//	predict("trainedNetwork3", "../Data/15234767500.jpg");
//
//	cv::waitKey(0);
//
//	std::getchar();
//	return 0;
//}

void train(const std::string& networkName, const std::string& learningDir)
{
	ImagePipeline pipe;
	try
	{
		pipe.setDirectory(learningDir);
		
		auto files = pipe.getFilesList();
		std::random_shuffle(files.begin(), files.end());
		
		size_t testFilesSize = static_cast<size_t>(files.size() * 0.1f);

		std::vector<std::string> testFiles;
		for (auto i = 0; i < testFilesSize; ++i)
		{
			testFiles.push_back(files.back());
			files.pop_back();
		}

		std::cout << "Training set size: " << files.size() << std::endl;
		cv::Mat samples;
		cv::Mat responses;
		for (auto& file : files)
		{
			auto img = pipe.loadLabelledImage(file);
			
			samples.push_back(img.getDesacriptor());
			responses.push_back(img.getClassCode());
		}

		CharacterRecognitionNetwork network(samples.cols);
		std::cout << "training.. " << std::endl;

		network.train(samples, responses);

		auto net = network.getNetwork();

		std::cout << "Is classifier " << net->isClassifier() << std::endl;
		std::cout << "Is trained " << net->isTrained() << std::endl;
		std::cout << "Is empty " << net->empty() << std::endl;

		network.save(networkName);
		std::cout << "end of training";
		test(testFiles, network);
	}

	catch (std::exception & ex)
	{
		std::cout << ex.what();
	}
}


void test(const std::vector<std::string>& testFiles,const CharacterRecognitionNetwork& network)
{
	ImagePipeline pipe;
	try
	{
		auto net = network.getNetwork();

		uint wrongPrediction = 0;
		for (auto& file : testFiles)
		{
			auto img = pipe.loadLabelledImage(file);

			
			cv::Mat response;

			//cv::Mat  des = cv::Mat(preprocess.preprocessTrainData(img.getData()));
			//net->predict(des, response);

			net->predict(img.getDesacriptor(), response);
			auto ch = network.predict(img.getDesacriptor());

			if (img.getClassName() != ch)
				wrongPrediction++;

			std::cout <<"Test "<< img.getClassName()<< " predicted " << ch << std::endl;

			for (auto i = 0; i < ClassDescriptor::getClassesCount(); ++i)
			{
				std::cout << ClassDescriptor::getDictionary()[i] << response.at<float>(i) << "||";
			}
			std::cout << std::endl;
		}
		std::cout <<"test set: " << testFiles.size() << " wrong prediction "<< wrongPrediction << std::endl;
		std::cout <<"error = %"<< (100.0f/ testFiles.size()) * wrongPrediction << std::endl;

	}
	catch (std::exception & ex)
	{
		std::cout << ex.what();
	}
}

void predict(const std::string& networkName, const std::string& filename)
{
	CharacterPreprocess prep;
	ImagePipeline pipe;
	CharacterRecognitionNetwork network(networkName);
	try
	{
		auto net = network.getNetwork();

		auto img = pipe.loadImage(filename);
		cv::imshow("before", img);

		cv::Mat res;
		net->predict(prep.preprocessImage(img), res);
		auto ch = network.predict(prep.preprocessImage(img));
		std::cout << "predicted " << ch << std::endl;

		for (auto i = 0; i < ClassDescriptor::getClassesCount(); ++i)
		{
			std::cout << ClassDescriptor::getDictionary()[i] << res.at<float>(i) << "||";
		}
		std::cout << std::endl;
	}
	
	
	catch (std::exception & ex)
	{
		std::cout << ex.what();
	}
}

