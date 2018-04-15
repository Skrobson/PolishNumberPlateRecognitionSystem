
#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <ctime>
#include <iostream>
#include "Plate.h"
#include <ctime>

using namespace std;

class PlateSearch
{
public:
	PlateSearch();
	PlateSearch(String path);
	~PlateSearch();

	void convert();
	vector<Mat> getMatVector();
	
	

private:
	Mat correctGamma(Mat& img, double gamma);
	void findPlate();
	bool isPlate(Mat img);
	void cut(Mat input);
	bool verifySizes(RotatedRect candidate);
	Mat img_gray;
	Mat image;
	vector<Plate> output;
	vector<Mat> number;
};



