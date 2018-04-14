#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

class Plate
{
private:
	Mat img;
	Rect rect;

public:
	Plate();
	Plate(Mat image, Rect rectangle) :img(image), rect(rectangle) {};
	~Plate();
};

