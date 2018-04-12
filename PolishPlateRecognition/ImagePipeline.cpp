#include "stdafx.h"
#include "ImagePipeline.h"
#include <iostream>

//c++17
namespace fs = std::experimental::filesystem;

//boost
//namespace fs = boost::filesystem;
ImagePipeline::ImagePipeline()
{
}

ImagePipeline::ImagePipeline(const std::string & directory) 
{
	setDirectory(directory);
}

void ImagePipeline::setDirectory(const std::string& dir)
{
	fs::path tmpPath{ dir };
	if (fs::is_directory(tmpPath))
	{
		rootDirectory = tmpPath;
		auto endIt = fs::directory_iterator{};
		auto beginIt = fs::directory_iterator{ rootDirectory };

		for (auto it = beginIt; it != endIt; ++it)
		{
			auto file = it->path();

			if (fs::is_regular_file(file))
			{
				auto filePath = file.string();
				files.push_back(std::move(filePath));
			}
		}
	}
	else
		throw fs::filesystem_error("Bad directory", tmpPath, std::error_code());

		//throw fs::filesystem_error("Bad directory", tmpPath, boost::system::error_code());

	
}

const std::vector<std::string> ImagePipeline::getFilesList() const
{
	return files;
}

cv::Mat ImagePipeline::loadImage(const std::string & filename) const
{
	cv::Mat img = cv::imread(filename, cv::ImreadModes::IMREAD_GRAYSCALE);
	if (img.empty())
	{
		std::cerr << "WARNING: Could not read image." << std::endl;

	}
	return img.clone();
}

LabelledImage ImagePipeline::loadLabelledImage(const std::string & filename) const
{
	auto img = loadImage(filename);

	cv::Mat convertedImg;
	//img.convertTo(convertedImg, CV_32F);
	//convertedImg = convertedImg.reshape(0, 1);
	char className = filename[(filename.find_last_of('_')) - 1];

	//return LabelledImage(std::move(convertedImg), className );
	return LabelledImage(std::move(img), className);
}

bool ImagePipeline::isPNGFile(const std::string & file)
{
	auto pos = file.find_last_of('.');
	auto fileExtension = file.substr(pos);

	if (fileExtension == ".png")
		return true;
	else
		return false;
}
