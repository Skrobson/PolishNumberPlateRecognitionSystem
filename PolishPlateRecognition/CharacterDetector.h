#pragma once
#include <opencv2/opencv.hpp>
class CharacterDetector
{
public:
	CharacterDetector();

	std::vector<cv::Mat>detect(const cv::Mat& posiblePlate);
	
	bool isValid() { return valid; }

	std::vector<cv::Mat> getCharacters() { return characters; }
private:
	bool valid = false;

	void preprocess(const cv::Mat& originalImage);

	bool findCharacters();

	bool verifyCharacterSize(const cv::RotatedRect & possibleChar, size_t plateHeight);

	size_t computeMedianHeight(std::list<cv::RotatedRect>& chars);

	bool verifyCharacterSizeByMedianHeight(const cv::RotatedRect &  possibleChar, float medianHeight);

	bool verifyByLineTroughCenters(std::list<cv::RotatedRect>& chars);

	std::vector<cv::Mat> extractCharacters(std::list<cv::RotatedRect>& possibleChars);

	//moze przeniesc osobno, taka sama funkcja jest w plateDetector
	cv::Mat cropChar(const cv::RotatedRect& character);

	cv::Mat originalPlateImage;
	cv::Mat tresholdPlateImage;

	std::list<cv::RotatedRect> possibleChars;

	std::vector<cv::Mat> characters;
};

