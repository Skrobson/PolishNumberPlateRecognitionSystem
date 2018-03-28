// PolishPlateRecognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ImagePipeline.h"
#include "ClassDescriptor.h"
#include "CharacterRecognitionNetwork.h"
#include <iostream>
#include <vector>



int main()
{
	ImagePipeline pipe;
	try 
	{
		pipe.setDirectory("..\\Data\\arklatrs");
		std::cout << "Training set size: " << pipe.getFilesList().size() << std::endl;
		
		CharacterRecognitionNetwork network;
		
		auto files = pipe.getFilesList();

		auto i = 1;
		for (auto& file : files)
		{
			network.train(pipe.loadLabelledImage(file));
			std::cout << "T" << i << std::endl;
			++i;
		}

		network.save("trainedNetwork2");
		std::cout << "end of training";

		//CharacterRecognitionNetwork network("trainedNetwork");

		auto test = pipe.loadLabelledImage("..\\Data\\arklatrs\\1_1522042883526.png");
		auto ch = network.predict(test.getData());
		std::cout << "Test " << test.getClassName() << "  Predicted" << ch << std::endl;

	}
	catch (std::exception & ex)
 	{
		std::cout << ex.what();
	}
	
	

	char c;
	std::cin >> c;
    return 0;
}
