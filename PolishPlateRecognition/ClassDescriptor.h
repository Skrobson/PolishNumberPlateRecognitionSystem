#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class ClassDescriptor
{
public:
	
	static int getClassesCount();

	static cv::Mat getClassCode(char className);

	static char getCharacter(size_t index);

private:

	static const std::string classesDictionary;
};

