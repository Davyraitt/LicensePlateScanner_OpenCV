#include "HelperFunctions.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

void save_image(string location, Mat img)
{
	imwrite(location, img);
}

