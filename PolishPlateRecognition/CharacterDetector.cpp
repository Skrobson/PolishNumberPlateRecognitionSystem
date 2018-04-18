#include "stdafx.h"
#include "CharacterDetector.h"
#include "ImageProcessing.h"

using namespace cv;

CharacterDetector::CharacterDetector()
{
}

std::vector<cv::Mat> CharacterDetector::detect(const cv::Mat& posiblePlate)
{
	originalPlateImage = posiblePlate.clone();
	preprocess(originalPlateImage);
	valid = findCharacters();

	return characters;
}

void CharacterDetector::preprocess(const cv::Mat& originalImage)
{
	cv::Mat grayPlate;
	cv::cvtColor(originalPlateImage, grayPlate, CV_BGR2GRAY);
	cv::threshold(grayPlate, tresholdPlateImage, 150, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV);

	//equalizeHist(grayPlate, grayPlate);

}

bool CharacterDetector::findCharacters()
{
	
	//finding contours
	cv::Mat character = cv::Mat::zeros(tresholdPlateImage.size(), CV_8UC3);;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	//
	cv::findContours(tresholdPlateImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	

	for (int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(255);
		drawContours(character, contours, i, color, 1, 8, hierarchy, 0, cv::Point());
	}

	std::list<cv::RotatedRect> validChars;
	std::vector<cv::RotatedRect> discartedChars;

	for (auto& con : contours)
	{
		cv::RotatedRect boundingBox = cv::minAreaRect(con);

		if (verifyCharacterSize(boundingBox, originalPlateImage.cols))
		{
			// draw the rotated rect
			cv::Point2f corners[4];
			boundingBox.points(corners);
			cv::line(character, corners[0], corners[1], cv::Scalar(0, 0, 255));
			cv::line(character, corners[1], corners[2], cv::Scalar(0, 0, 255));
			cv::line(character, corners[2], corners[3], cv::Scalar(0, 0, 255));
			cv::line(character, corners[3], corners[0], cv::Scalar(0, 0, 255));

			validChars.push_back(std::move(boundingBox));
			possibleChars.push_back(boundingBox);
		}
		else
		{
			possibleChars.push_back(boundingBox);
		}
	}
	std::cout << "<<<<<<<<<<" << std::endl;

	//second chance
	if (validChars.size() >= 4) //7)
	{
		auto avgH = computeMedianHeight(validChars);

		validChars.clear();

		for (cv::RotatedRect & ch : possibleChars)
		{
			if (verifyCharacterSizeByMedianHeight(ch, avgH))
			{
				validChars.push_back(ch);
				//draw
				cv::Point2f points[4];
				ch.points(points);

				cv::line(character, points[0], points[1], cv::Scalar(0, 255, 0));
				cv::line(character, points[1], points[2], cv::Scalar(0, 255, 0));
				cv::line(character, points[2], points[3], cv::Scalar(0, 255, 0));
				cv::line(character, points[3], points[0], cv::Scalar(0, 255, 0));
			}
		}
		
	}
	//debug
	//cv::imshow("p", tresholdPlateImage);
	//cv::waitKey();


	//cv::imshow("p", character);
	//cv::waitKey();

	if (validChars.size() > 8)
		return false;

	else if (validChars.size() < 7)
		return false;
	else if (verifyByLineTroughCenters(validChars))
	{
		extractCharacters(validChars);
		return true;
	}
	return false;


}

bool CharacterDetector::verifyCharacterSize(const cv::RotatedRect & possibleChar, size_t plateHeight)
{
	constexpr float error = 0.55;

	//car char size: 80x(54:43) 
	constexpr float minAspect = 80.0f / 54.0f;
	constexpr float maxAspect = 80.0f / 43.0f;
	//Set a min and max area. All other patches are discarded
	constexpr int min = 8 * 4; // minimum area

	float longestSide = std::max(possibleChar.size.height, possibleChar.size.width);
	if (longestSide > plateHeight)
		return false;


	float rmin = minAspect - minAspect * error;
	float rmax = maxAspect + maxAspect * error;
	int area = possibleChar.size.height * possibleChar.size.width;
	float r = (float)possibleChar.size.width / (float)possibleChar.size.height;

	float angle = possibleChar.angle;//std::fabs
									  //
	if (r < 1)
		r = 1 / r;

	if ((area < min) || ((r < rmin) || (r > rmax)))// 
		return false;

	else
	{
		if (angle > 30 || (angle > -60 && angle < -30))
			return false;
		return true;
	}
}

size_t CharacterDetector::computeMedianHeight(std::list<cv::RotatedRect>& chars)
{
	std::list<float> heights;
	
	for (RotatedRect& rect : chars)
	{
		auto size = rect.size;
		auto height = std::max(size.height, size.width);
		heights.push_back(height);

		
	}
	heights.sort();
	std::map<int, int> median;
	for (auto num : heights)
		median[std::round(num)] += 1;

	auto medianValue = std::max_element(median.begin(), median.end(),
		[](const std::pair<int, int>& p1, const std::pair<int, int>& p2)->bool {
		return p1.second < p2.second;
	});

	return medianValue->first;
}

bool CharacterDetector::verifyCharacterSizeByMedianHeight(const cv::RotatedRect & possibleChar, float medianHeight)
{
	constexpr float error = 0.15f;

	auto size = possibleChar.size;
	auto height = std::max(size.height, size.width);
	auto width = std::min(size.height, size.width);
	if (width < 1)
		return false;

	auto min = medianHeight - medianHeight * error;
	auto max = medianHeight + medianHeight * error;

	float angle = possibleChar.angle;
	if (height > max || height < min)
		return false;

	if (angle > 30 || (angle > -60 && angle < -30))
		return false;

	return true;
}

bool CharacterDetector::verifyByLineTroughCenters(std::list<cv::RotatedRect>& chars)
{
	typedef std::list<cv::RotatedRect>::iterator it;
	auto checkCenters = [](it first, it last) {
		auto lineParams = computeStraithLineEquation(first->center, last->center);

		for (it i = first; i == last; ++i)
		{
			float d = distancePointLine(i->center, lineParams.first, lineParams.second);
			float h = std::max(i->size.height, i->size.width);
			if (d > 0.3*h)
				return false;
		}
		return true;
	};

	chars.sort([](const cv::RotatedRect& ch1, const cv::RotatedRect& ch2)->bool {
		return ch1.center.x < ch2.center.x;
	});

	if (chars.size() == 7)
	{
		return checkCenters(chars.begin(), --chars.end());
	}
	else
	{
		if (!checkCenters(chars.begin(), --(--chars.end())))
		{
			chars.pop_front();
			return checkCenters(chars.begin(), --chars.end());
		}	
	}
	return false;
}

std::vector<cv::Mat> CharacterDetector::extractCharacters(std::list<cv::RotatedRect>& possibleChars)
{
	for (auto & ch : possibleChars)
	{
		cv::Mat c = cropChar(ch);
		cv::cvtColor(c, c, CV_BGR2GRAY);
		cv::threshold(c, c, 150, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV);

		characters.push_back(std::move(c));
	}

	return characters;
}

cv::Mat CharacterDetector::cropChar(const cv::RotatedRect & character)
{
	float height = std::max(character.size.width, character.size.height);
	float width = std::min(character.size.width, character.size.height);
	cv::Size rSize(width * 1.1 , height * 1.2);

	Mat cropedImage;

	getRectSubPix(originalPlateImage, rSize, character.center, cropedImage);

	return cropedImage.clone();
}


