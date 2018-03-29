#pragma once
#include <opencv2/opencv.hpp>
#include "LabelledImage.h"
class CharacterRecognitionNetwork
{
public:
	CharacterRecognitionNetwork(int inputCells);
	CharacterRecognitionNetwork(const std::string& filename);

	void train(const LabelledImage& data);

	void save(const std::string& fileName);
	
	char predict(const cv::Mat& picture);

	const cv::Ptr<cv::ml::ANN_MLP> getNetwork() const { return ann; }

private:

	cv::Ptr<cv::ml::ANN_MLP> ann;
};

