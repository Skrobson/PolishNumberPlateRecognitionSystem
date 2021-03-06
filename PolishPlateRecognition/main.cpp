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
#include "PlateDetector.h"
#include "CharacterDetector.h"


void train(const std::string& networkName, const std::string& learningDir);

void test(const std::vector<std::string>& testFiles, const CharacterRecognitionNetwork& network);

void predict(const std::string& networkName, const std::string& fileName);

std::string predictPlate(const std::vector<cv::Mat>& characters, CharacterRecognitionNetwork& network);

void printClassDescriptors(std::vector<float> response);













int main(int argc, char** argv)
{

	//if (argc == 4)
	//{
	//	if (argv[1] == "-t")
	//		train(argv[2], argv[3]);

	//	else if (argv[1] == "-p")
	//		predict(argv[2], argv[3]);

	//}
	//train("trainedNetwork3" , "../Data/arklatrs");

	//predict("trainedNetwork3", "../Data/test/Z_15237054770.jpg");

	//CharacterRecognitionNetwork network("trainedNetwork3");


	for (int i = 1; i < 92; ++i)
	{
		std::string ppath = "../Data/newtest/";
		ppath += std::to_string(i);
		ppath += ".jpg";

		cv::Mat image = cv::imread(ppath, CV_LOAD_IMAGE_COLOR);   // Read the file

		if (!image.data) {
			std::cout << "Could not open or find the image" << std::endl;
			continue;
		}

		else if (image.size().width > 1400) {
			cv::Size size(image.size().width / 2, image.size().height / 2);
			cv::resize(image, image, size);
		}
		std::cout << "detecting img " << i << std::endl;
		PlateDetector det;
		auto possiblePlates = det.detect(image);
		auto steps = det.showSteps();
		
		std::string winName = "ANPR";

		cv::namedWindow(winName);
		cv::moveWindow(winName, 0, 0);
		cv::resizeWindow(winName, steps.cols, steps.rows);
		CharacterRecognitionNetwork network("trainedNetwork3");

		if (possiblePlates.size() > 0)
		{
			bool detected = false;
			CharacterDetector chDet;
			for (auto& pl : possiblePlates)
			{
				auto chars = chDet.detect(pl);
				if (chDet.isValid())
				{
					auto winMat(steps);
					int x = 0;
					int y = winMat.size().height/2;
					
					for (auto& c :chars)
					{
						if (x >winMat.size().width || y > winMat.size().height)
							break;
					
						cv::Mat color;
						try
						{
							cv::cvtColor(c, color, cv::COLOR_GRAY2BGR);
							color.copyTo(winMat(cv::Rect(x, y, c.size().width, c.size().height)));
							x += c.size().width;
						}
						catch (std::exception& e)
						{
							break;
						}
					
					}

					std::string plateNumber = predictPlate(chars, network);
					
					std::cout << "Plate numbers: " << plateNumber << std::endl;
					cv::putText(winMat, plateNumber, cv::Point(2, winMat.size().height/2 - 10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0));


					cv::imshow(winName, winMat);
					cv::waitKey();
					continue;
				}
			}
			if (!detected)
				std::cout << "plate not detect" << std::endl;
		}
		else
		{
			std::cout << "plate not detect" << std::endl;
		}
	}

	std::getchar();
	return 0;
}




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
		uint wrongPrediction = 0;
		for (auto& file : testFiles)
		{
			auto img = pipe.loadLabelledImage(file);

			auto response = network.predict(img.getDesacriptor());
			auto charResponse = std::get<char>(response);
			if (img.getClassName() != charResponse)
				wrongPrediction++;

			std::cout <<"Test "<< img.getClassName()<< " predicted " << charResponse << std::endl;

			printClassDescriptors(std::get<std::vector<float>>(response));
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
		auto img = pipe.loadImage(filename);
		cv::imshow("prediction", img);
		
		auto charToReckognize = prep.preprocessImage(img);
		auto ch = network.predict(charToReckognize);
		std::cout << "predicted " << std::get<char>(ch) << " %" << std::get<float>(ch) << std::endl;

		auto response = std::get < std::vector<float>>(ch);

		printClassDescriptors(response);
	}
	
	catch (cv::Exception & ex)
	{
		std::cout << ex.what();
	}
}

std::string predictPlate(const std::vector<cv::Mat>& characters, CharacterRecognitionNetwork& network)
{
	std::string predictedPlate;
	CharacterPreprocess prep;
	for (auto& ch : characters)
	{

		auto response = network.predict(prep.preprocessImage(ch));
		auto percentage = std::get<float>(response);
		auto percentClassDescriptor = std::get<std::vector<float>>(response);

		char predictedChar;

		if (percentage > 0.5f)
			predictedChar = std::get<char>(response);
		else
			predictedChar = '?';

		std::cout << "predicted " << std::get<char>(response) << " %" << percentage << std::endl;
		printClassDescriptors(percentClassDescriptor);

		predictedPlate += predictedChar;

		//cv::imshow("prediction", ch);
		//
		//cv::waitKey();
	}
	return predictedPlate;
}

void printClassDescriptors(std::vector<float> response)
{
	for (auto i = 0; i < ClassDescriptor::getClassesCount(); ++i)
	{
		std::cout << ClassDescriptor::getDictionary()[i]<<" %"  << response.at(i) << " || ";
		if((i+1)%5 == 0)
			std::cout << std::endl;
	}
	std::cout << std::endl;
}

