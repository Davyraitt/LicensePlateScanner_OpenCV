#pragma once
#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include "HelperFunctions.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include "opencv2/opencv.hpp"


class HelperFunctions
{
public:

	void save_image(std::string location, cv::Mat img);
};

#endif
