#pragma once
#include <string>
#include <vector>
#include <opencv2\opencv.hpp>
#include "LabelledImage.h"

#include <filesystem>
//#include <boost\filesystem.hpp>

class ImagePipeline
{
public:
	ImagePipeline();
	ImagePipeline(const std::string& directory);

	void setDirectory(const std::string& dir);

	const std::vector<std::string> getFilesList() const;

	cv::Mat loadImage(const std::string& filename) const;

	LabelledImage loadLabelledImage(const std::string& filename) const;

private:

	std::vector<std::string> files;
	std::experimental::filesystem::path rootDirectory;


	bool isPNGFile(const std::string& file);
};

