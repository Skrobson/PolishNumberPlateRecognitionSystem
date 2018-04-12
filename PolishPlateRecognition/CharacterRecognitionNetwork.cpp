#include "stdafx.h"
#include "CharacterRecognitionNetwork.h"
#include "ClassDescriptor.h"

#include <algorithm>

CharacterRecognitionNetwork::CharacterRecognitionNetwork(int inputCells)
{
	ann = cv::ml::ANN_MLP::create();
	cv::Mat_<int> layers(1, 4) ;
	layers(0) = inputCells;     // input
	layers(1) = 123; 
	layers(2) = 77;// hidden
	auto outputSize = ClassDescriptor::getClassesCount();
	layers(3) = outputSize;      // output, 1 pin per class.

	ann->setLayerSizes(layers);
	ann->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM,1,1);
	ann->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 300, 0.0001));
	ann->setTrainMethod(cv::ml::ANN_MLP::BACKPROP, 0.0001);
	ann->setBackpropMomentumScale(0.05f);
	ann->setBackpropWeightScale(0.05f);	
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

void CharacterRecognitionNetwork::train(const cv::Mat & samples, const cv::Mat & responses)
{
	ann->train(samples, cv::ml::ROW_SAMPLE, responses);
}


void CharacterRecognitionNetwork::save(const std::string & fileName)
{
	ann->save(fileName);
}

char CharacterRecognitionNetwork::predict(const cv::Mat & picture)const
{
	
	std::vector<float> res;
	ann->predict(picture, res);
	
	auto resultCharacter = std::max_element(res.begin(), res.end());
	auto index = std::distance(res.begin(), resultCharacter);

	
	return ClassDescriptor::getCharacter(index);

}


