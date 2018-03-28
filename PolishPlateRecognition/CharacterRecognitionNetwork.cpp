#include "stdafx.h"
#include "CharacterRecognitionNetwork.h"
#include "ClassDescriptor.h"

#include <algorithm>

CharacterRecognitionNetwork::CharacterRecognitionNetwork()
{
	ann = cv::ml::ANN_MLP::create();
	cv::Mat_<int> layers(4, 1) ;
	layers(0 ) = 32*32;     // input
	layers(1) = 164;  // hidden
	layers(2) = 77;  // hidden
	auto outputSize = ClassDescriptor::getClassesCount();
	layers(3) = outputSize;      // output, 1 pin per class.
	ann->setLayerSizes(layers);
	ann->setActivationFunction(cv::ml::ANN_MLP::RELU);
	ann->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 300, 0.0001));
	ann->setTrainMethod(cv::ml::ANN_MLP::BACKPROP, 0.0001);
}

CharacterRecognitionNetwork::CharacterRecognitionNetwork(const std::string & filename)
{
	ann = cv::ml::ANN_MLP::load(filename);
}

void CharacterRecognitionNetwork::train(const LabelledImage & data)
{
	auto train = ann->train(data.getData(), cv::ml::ROW_SAMPLE,data.getClassCode());	

	std::cout << train << std::endl;
}

void CharacterRecognitionNetwork::save(const std::string & fileName)
{
	ann->save(fileName);
}

char CharacterRecognitionNetwork::predict(const cv::Mat & picture)
{
	
	std::vector<float> res;
	ann->predict(picture, res);
	
	auto resultCharacter = std::max_element(res.begin(), res.end());
	auto index = std::distance(res.begin(), resultCharacter);

	
	return ClassDescriptor::getCharacter(index);

}


