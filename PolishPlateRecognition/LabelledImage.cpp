#include "stdafx.h"
#include "LabelledImage.h"
#include "ClassDescriptor.h"
#include "CharacterPreprocess.h"


LabelledImage::LabelledImage(cv::Mat&& data, char className) : image(data), className(className)
{
	classCode = ClassDescriptor::getClassCode(className);

	CharacterPreprocess prep;
	descriptors = prep.preprocessImage(image);
}

LabelledImage::LabelledImage(const cv::Mat & data, char className) : image(data), className(className)
{
	classCode = ClassDescriptor::getClassCode(className);

	CharacterPreprocess prep;
	descriptors = prep.preprocessImage(image);
}

cv::Mat LabelledImage::getDesacriptor() const
{
	return descriptors;
}

cv::Mat LabelledImage::getData() const
{
	return image;
}

cv::Mat LabelledImage::getClassCode() const
{
	return classCode;
}

char LabelledImage::getClassName() const
{
	return className;
}
