// PolishPlateRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ImagePipeline.h"
#include "ClassDescriptor.h"
#include "CharacterRecognitionNetwork.h"
#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>



int main()
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
		std::vector<cv::Mat> trainData;
		std::vector<cv::Mat> deskwData;

		//auto i = 1;
		//for (auto& file : files)
		//{
		//	auto img = pipe.loadLabelledImage(file);
		//	samples.push_back(img.getData());
		//	responses.push_back(img.getClassCode());
		//	std::cout << "loaded img " << i << std::endl;
		//	++i;
		//	if (i == 10)
		//		break;
		//}

		

	

		//CharacterRecognitionNetwork network(samples.cols);
		//std::cout << "training.. "<<std::endl;
		//auto net = network.getNetwork();
		//net->train(samples, cv::ml::ROW_SAMPLE, responses);
		//std::cout << "Is classifier " << net->isClassifier() << std::endl;
		//std::cout << "Is trained " << net->isTrained() << std::endl;
		//std::cout << "Is empty " << net->empty() << std::endl;

		//network.save("trainedNetwork2");
		//std::cout << "end of training";

		CharacterRecognitionNetwork network("trainedNetwork2");

		//auto test = pipe.loadLabelledImage("..\\Data\\arklatrs\\6_1522305396182.png");
		//auto ch = network.predict(test.getData());
		//std::cout << "Test " << test.getClassName() << "  Predicted " << ch << std::endl;

		auto net = network.getNetwork();

		auto i = 1;
		for (auto& file : files)
		{
			auto img = pipe.loadLabelledImage(file);
			cv::Mat res;
			net->predict(img.getData(), res);
			auto ch = network.predict(img.getData());
			std::cout << "Test " << img.getClassName() << "  Predicted " << ch << std::endl;

			for (auto i = 0; i < ClassDescriptor::getClassesCount(); ++i)
			{
				std::cout << ClassDescriptor::getDictionary()[i] << res.at<float>(i) << "||";
			}
			std::cout << std::endl;

			++i;
			if (i == 10)
				break;
		}

		//cv::Mat res;
		//net->predict(test.getData(), res);

		//for (auto i = 0; i < ClassDescriptor::getClassesCount(); ++i)
		//{
		//	std::cout << ClassDescriptor::getDictionary()[i] << res.at<float>(i) << "||";
		//}
	}


	catch (std::exception & ex)
 	{
		std::cout << ex.what();
	}
	
	std::getchar();

    return 0;
}

