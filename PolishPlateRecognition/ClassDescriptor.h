#pragma once
#include <opencv2/opencv.hpp>
#include <string>

class ClassDescriptor
{
public:
	
	static int getClassesCount();

	static cv::Mat getClassCode(char className);

	static char getCharacter(size_t index);

	static const std::string& getDictionary();

private:

	static const std::string classesDictionary;
};

