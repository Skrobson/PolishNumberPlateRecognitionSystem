#pragma once
#include <opencv2/opencv.hpp>
class CharacterDetector
{
public:
	CharacterDetector();

	void detect(const cv::Mat& posiblePlate);
	
	bool isValid() { return valid; }

	std::vector<cv::Mat> extractCharacters() { return characters; }
private:
	bool valid = false;

	cv::Mat preprocess(const cv::Mat& originalImage);

	void findCharacters();

	bool verifyCharacterSize(const cv::RotatedRect & possibleChar, size_t plateHeight);

	size_t computeMedianHeight(std::vector<cv::RotatedRect>& chars);

	bool verifyCharacterSizeByMedianHeight(const cv::RotatedRect &  possibleChar, float medianHeight);

	//moze przeniesc osobno, taka sama funkcja jest w plateDetector
	cv::Mat cropChar(const cv::RotatedRect& character);

	cv::Mat originalPlateImage;

	std::list<cv::RotatedRect> possibleChars;
	std::vector<cv::Mat> characters;
};

