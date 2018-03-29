// PolishPlateRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ImagePipeline.h"
#include "ClassDescriptor.h"
#include "CharacterRecognitionNetwork.h"
#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

void train(const std::string& networkName);

void predict(const std::string& fileName);

void test();

int main()
{
	test();
	std::getchar();

    return 0;
}

void train(const std::string& networkName)
{

	ImagePipeline pipe;
	try
	{
		pipe.setDirectory("..\\Data\\arklatrs");
		std::cout << "Training set size: " << pipe.getFilesList().size() << std::endl;



		auto files = pipe.getFilesList();
		std::random_shuffle(files.begin(), files.end());
		cv::Mat samples;
		cv::Mat responses;
		

		auto i = 1;
		for (auto& file : files)
		{
			auto img = pipe.loadLabelledImage(file);
			samples.push_back(img.getData());
			responses.push_back(img.getClassCode());
			std::cout << "loaded img " << i << std::endl;

		}

		CharacterRecognitionNetwork network(samples.cols);
		std::cout << "training.. " << std::endl;
		auto net = network.getNetwork();
		net->train(samples, cv::ml::ROW_SAMPLE, responses);
		std::cout << "Is classifier " << net->isClassifier() << std::endl;
		std::cout << "Is trained " << net->isTrained() << std::endl;
		std::cout << "Is empty " << net->empty() << std::endl;

		network.save(networkName);
		std::cout << "end of training";
	}

	catch (std::exception & ex)
	{
		std::cout << ex.what();
	}
}

void predict(const std::string& filename)
{
	ImagePipeline pipe;
	CharacterRecognitionNetwork network("trainedNetwork2");

	auto net = network.getNetwork();

	auto img = pipe.loadImage(filename);

	cv::Mat convertedImg;
	img.convertTo(convertedImg, CV_32F);
	convertedImg = convertedImg.reshape(0, 1);
	cv::Mat resizedImg;
	cv::resize(img, resizedImg,cv::Size(32, 32));
	cv::Mat res;
	net->predict(resizedImg, res);
	auto ch = network.predict(resizedImg);
	std::cout << "predicted " << ch << std::endl;

	for (auto i = 0; i < ClassDescriptor::getClassesCount(); ++i)
	{
		std::cout << ClassDescriptor::getDictionary()[i] << res.at<float>(i) << "||";
	}
	std::cout << std::endl;
}

void test()
{
	ImagePipeline pipe;
	try
	{
		pipe.setDirectory("..\\Data\\test");
		std::cout << "Testing set size: " << pipe.getFilesList().size() << std::endl;

		CharacterRecognitionNetwork network("trainedNetwork2");
		auto net = network.getNetwork();

		auto files = pipe.getFilesList();
		std::random_shuffle(files.begin(), files.end());

		for (auto& file : files)
		{
			auto img = pipe.loadLabelledImage(file);
			cv::Mat response;
			net->predict(img.getData(), response);
			auto ch = network.predict(img.getData());
			std::cout <<"Test "<< img.getClassName()<< " predicted " << ch << std::endl;

			for (auto i = 0; i < ClassDescriptor::getClassesCount(); ++i)
			{
				std::cout << ClassDescriptor::getDictionary()[i] << response.at<float>(i) << "||";
			}
			std::cout << std::endl;
		}
	}
	catch (std::exception & ex)
	{
		std::cout << ex.what();
	}
}
