#pragma once
#include <opencv2/opencv.hpp>
#include "LabelledImage.h"
class CharacterRecognitionNetwork
{
public:
	CharacterRecognitionNetwork(int inputCells);
	CharacterRecognitionNetwork(const std::string& filename);

	void train(const LabelledImage& data);
	void train(const cv::Mat& samples,const cv::Mat& responses);

	void save(const std::string& fileName);
	
	char predict(const cv::Mat& picture) const;

	const cv::Ptr<cv::ml::ANN_MLP> getNetwork() const { return ann; }

private:

	cv::Ptr<cv::ml::ANN_MLP> ann;
};

