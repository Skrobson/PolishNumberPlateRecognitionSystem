// PolishPlateRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ImagePipeline.h"
#include "ClassDescriptor.h"
#include "CharacterRecognitionNetwork.h"
#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

void train(const std::string& networkName, const std::string& learningDir);

void test(const std::vector<std::string>& testFiles, const CharacterRecognitionNetwork& network);

void predict(const std::string& networkName, const std::string& fileName);


int main(int argc, char** argv)
{

	//if (argc == 4)
	//{
	//	if (argv[1] == "-t")
	//		train(argv[2], argv[3]);

	//	else if (argv[1] == "-p")
	//		predict(argv[2], argv[3]);

	//}
	train("trainedNetwork3" , :"");

	std::getchar();

	return 0;
}

void train(const std::string& networkName, const std::string& learningDir)
{

	ImagePipeline pipe;
	try
	{
		pipe.setDirectory(learningDir);
		std::cout << "Training set size: " << pipe.getFilesList().size() << std::endl;



		auto files = pipe.getFilesList();
		std::random_shuffle(files.begin(), files.end());
		cv::Mat samples;
		cv::Mat responses;
		
		size_t testFilesSize = static_cast<size_t>(files.size() * 0.1f);

		std::vector<std::string> testFiles;
		for (auto i = 0; i < testFilesSize; ++i)
		{
			testFiles.push_back(files.back());
			files.pop_back();
		}

		
		for (auto& file : files)
		{
			auto img = pipe.loadLabelledImage(file);
			samples.push_back(img.getData());
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

void predict(const std::string& networkName, const std::string& filename)
{
	ImagePipeline pipe;
	CharacterRecognitionNetwork network(networkName);

	auto net = network.getNetwork();

	auto img = pipe.loadImage(filename);

	cv::Mat convertedImg;
	img.convertTo(convertedImg, CV_32F);
	convertedImg = convertedImg.reshape(0, 1);
	cv::Mat resizedImg;
	cv::resize(img, resizedImg, cv::Size(32, 32));
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
