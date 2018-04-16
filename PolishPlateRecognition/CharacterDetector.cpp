#include "stdafx.h"
#include "CharacterDetector.h"

using namespace cv;

CharacterDetector::CharacterDetector()
{
}

void CharacterDetector::detect(const cv::Mat& posiblePlate)
{
	originalPlateImage = posiblePlate.clone();
	findCharacters();

}

cv::Mat CharacterDetector::preprocess(const cv::Mat & originalImage)
{
	cv::Mat grayPlate;
	cv::cvtColor(originalPlateImage, grayPlate, CV_BGR2GRAY);
	cv::threshold(grayPlate, grayPlate, 150, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV);

	//equalizeHist(grayPlate, grayPlate);

	return grayPlate.clone();
}

void CharacterDetector::findCharacters()
{
	cv::Mat grayPlate = preprocess(originalPlateImage);

	//finding contours
	cv::Mat character = cv::Mat::zeros(grayPlate.size(), CV_8UC3);;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	//
	cv::findContours(grayPlate, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	

	for (int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(255);
		drawContours(character, contours, i, color, 1, 8, hierarchy, 0, cv::Point());
	}

	std::vector<cv::RotatedRect> validChars;
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
			std::cout << boundingBox.center.x << "x" << boundingBox.center.y << " " <<
				boundingBox.size.width << "x" << boundingBox.size.height << std::endl;
		}
		else
		{
			possibleChars.push_back(boundingBox);
			//std::cout << boundingBox.center.x<<"x"<< boundingBox.center.y << " " << 
			//	boundingBox.size.width << "x" << boundingBox.size.height << std::endl;
		}
	}
	std::cout << "<<<<<<<<<<" << std::endl;

	//second chance
	if (validChars.size() >= 4) //7)
	{
		std::list<RotatedRect> characters;
		auto avgH = computeMedianHeight(validChars);
		for (cv::RotatedRect & ch : possibleChars)
		{
			if (verifyCharacterSizeByMedianHeight(ch, avgH))
			{
				characters.push_back(ch);
				//draw
				cv::Point2f points[4];
				ch.points(points);

				cv::line(character, points[0], points[1], cv::Scalar(0, 255, 0));
				cv::line(character, points[1], points[2], cv::Scalar(0, 255, 0));
				cv::line(character, points[2], points[3], cv::Scalar(0, 255, 0));
				cv::line(character, points[3], points[0], cv::Scalar(0, 255, 0));
			}
		}
		possibleChars = characters;
	}

	//sort horizontally
	//check line through centers

	cv::imshow("p", grayPlate);
	cv::waitKey();


	cv::imshow("p", character);
	cv::waitKey();
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
		//std::cout << angle << std::endl;
		return true;
	}
}

size_t CharacterDetector::computeMedianHeight(std::vector<cv::RotatedRect>& chars)
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


	std::cout << "median" << medianValue->first << " " << medianValue->second << std::endl;

	return medianValue->first;
}

bool CharacterDetector::verifyCharacterSizeByMedianHeight(const cv::RotatedRect & possibleChar, float medianHeight)
{
	constexpr float error = 0.15f;

	auto size = possibleChar.size;
	auto height = std::max(size.height, size.width);

	auto min = medianHeight - medianHeight * error;
	auto max = medianHeight + medianHeight * error;

	if (height > max || height < min)
		return false;

	return true;
}

cv::Mat CharacterDetector::cropChar(const cv::RotatedRect & character)
{
	cv::Size rSize(character.size.width, character.size.height);
	float r = (float)rSize.width / (float)rSize.height;
	float angle = character.angle;

	if (r < 1)
		angle = 90 + angle;
	Mat rotmat = getRotationMatrix2D(character.center, angle, 1);
	//Create and rotate image
	Mat rotatedImage;
	warpAffine(originalPlateImage, rotatedImage, rotmat, originalPlateImage.size(), CV_INTER_CUBIC);

	if (r < 1)
		swap(rSize.width, rSize.height);

	Mat cropedImage;
	getRectSubPix(rotatedImage, rSize, character.center, cropedImage);


	//possiblePlatesMats.push_back(std::move(cropedImage));

	return cropedImage.clone();
}


