#include "stdafx.h"
#include "LabelledImage.h"
#include "ClassDescriptor.h"


LabelledImage::LabelledImage(cv::Mat&& data, char className) : data(data), className(className)
{
	classCode = ClassDescriptor::getClassCode(className);
}

LabelledImage::LabelledImage(const cv::Mat & data, char className) : data(data), className(className)
{
	classCode = ClassDescriptor::getClassCode(className);
}

cv::Mat LabelledImage::getData() const
{
	return data;
}

cv::Mat LabelledImage::getClassCode() const
{
	return classCode;
}

char LabelledImage::getClassName() const
{
	return className;
}
