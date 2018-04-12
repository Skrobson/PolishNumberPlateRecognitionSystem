#include "stdafx.h"
#include "ClassDescriptor.h"

const std::string ClassDescriptor::classesDictionary = "ABCDEFGHIJKLMNOPRSTUVWXYZ0123456789";

int ClassDescriptor::getClassesCount()
{
	return static_cast<int>(classesDictionary.size());
}

cv::Mat ClassDescriptor::getClassCode(char className)
{
	cv::Mat code = cv::Mat::zeros(cv::Size((int)classesDictionary.size(), 1), CV_32F);

	auto index = classesDictionary.find_first_of(className);
	if (index != std::string::npos)
		code.at<float>(index) = 1.0f;
	
	return code;
}

char ClassDescriptor::getCharacter(size_t index)
{
	if (index < classesDictionary.size())
		return classesDictionary[index];
	else
		return ' ';
}

const std::string & ClassDescriptor::getDictionary()
{
	return classesDictionary;
}


