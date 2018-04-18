#pragma once
#include <opencv2/opencv.hpp>
#include <tuple>
#include "LabelledImage.h"
class CharacterRecognitionNetwork
{
public:
	CharacterRecognitionNetwork(int inputCells);
	CharacterRecognitionNetwork(const std::string& filename);

	void train(const LabelledImage& data);
	void train(const cv::Mat& samples,const cv::Mat& responses);

	void save(const std::string& fileName);
	

	std::tuple<char,float, std::vector<float>> predict(const cv::Mat& picture) const;

private:

	cv::Ptr<cv::ml::ANN_MLP> ann;
};

