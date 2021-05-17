#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include "opencv2/opencv.hpp"
#include "HelperFunctions.h"
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>


using namespace cv;
using namespace std;

//Variables
const int windowHeight = 360;
const int windowWidth = 640;
int xThreshold = 41;
int yThreshold = 245;
string path = "Resources/Plates/russian2.jpg";
CascadeClassifier plateCascade;
vector<Rect> plates;
int scaleF = 1, scaleFD = 1, minN = 10;
string extractedLicensePlate = "nummerbord0";
vector<vector<Point>> contours;
vector<Vec4i> hierachy;
vector<Point> contour;
vector<vector<Point>> contoursStep11;
vector<Vec4i> hierarchyStep12;
int area_thresh = 0;
vector<Point> big_contour;
//Imgs
Mat imgGray, imgEroded, imgEroded2, imgDilated, imgDilated2, imgEroded3;
Mat imageStep7, imgNoNoiseBlurred, imageStep8, imageStep10, imageStep9;
string tesseractSaveString;

//declaring methods from helperfunctions.h
void save_image(string location, Mat img);

void ReadImageWithTesseract()
{
	char* outText;

	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	// Open input image with leptonica library
	Pix* image = pixRead("Resources/test2.png");
	api->SetImage(image);
	// Get OCR result
	outText = api->GetUTF8Text();
	printf("OCR output:\n%s", outText);

	// Destroy used object and release memory
	api->End();
	delete api;
	delete[] outText;
	pixDestroy(&image);
}

void Detect_License_Plate()
{
	Mat img = imread(path);
	plateCascade.load("Resources/haarcascade_russian_plate_number.xml");
	plateCascade.detectMultiScale(img, plates, 1.1, 20);
	for (int i = 0; i < plates.size(); i++)
	{
		Mat imgCrop = img(plates[i]);
		extractedLicensePlate = "Resources/Plates/nummerbord" + to_string(i) + ".png";
		save_image(extractedLicensePlate, imgCrop);
		rectangle(img, plates[i].tl(), plates[i].br(), Scalar(255, 0, 255), 3);
	}
	imshow("Image", img);

	//waitKey(0);

}

void Read_License_Plate()
{
	string path = extractedLicensePlate;
	Mat img = imread(path);

	// 1 Convert to grayscale
	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	imshow("imgGray", imgGray);

	// 2	Threshold
	Mat imgTreshhold;
	threshold(imgGray, imgTreshhold, xThreshold, yThreshold, THRESH_BINARY);
	imshow("imgTreshhold", imgTreshhold);

	// 3	Use morphology to remove small white or black regions and to close over the text with white
	Mat kernelStep3 = getStructuringElement(MORPH_RECT, Size(5, 5));
	erode(imgTreshhold, imgEroded, kernelStep3);
	dilate(imgEroded, imgDilated, kernelStep3);
	dilate(imgDilated, imgDilated2, kernelStep3);
	erode(imgDilated2, imgEroded2, kernelStep3);
	imshow("Image morph", imgEroded2);

	Mat kernelStep4 = getStructuringElement(MORPH_RECT, Size(3, 3));
	// Creates a kernel that we can use with dilation, bigger = dilate more
	erode(imgEroded2, imgEroded3, kernelStep4);
	imshow("imgEroded3", imgEroded3);

	// 5	Extract the text from the bounding box of that contour
	findContours(imgEroded3, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	if (contours.size() == 1) { contour = contours[0]; }
	else { contour = contours[1]; }
	if (contours.size() != 0)
	{
		for (vector<Point> p : contours)
		{
			double area = contourArea(p);
			Rect boundingBox = boundingRect(p);
			double aspect = boundingBox.width / boundingBox.height;

			if (area > area_thresh)
			{
				if (aspect > 1)
				{
					big_contour = p;
					area_thresh = area;
				}
			}
		}
	}

	Rect boundingBox = boundingRect(big_contour);
	Mat imgCrop = img(boundingBox);
	imshow("Image cropped", imgCrop);

	//Saving cropped image to resources
	string NNPATH = "Resources/Plates/NummerBordNoNoise.png";
	save_image(NNPATH, imgCrop);

	//  6 We now have a cropped easy to read image. We can try and extract text from here.
	//Finding contours
	Mat imageStep6 = imread(NNPATH);
	//imshow(" imgNoNoise", imageStep6);

	//  7 Convert the cropped image to gray again
	cvtColor(imageStep6, imageStep7, COLOR_BGRA2GRAY);
	//imshow(" imageStep7", imageStep7);

	// 8 Threshold the cropped image again
	threshold(imageStep7, imageStep8, 0, 255, THRESH_OTSU);
	Mat kernelStep8 = getStructuringElement(MORPH_RECT, Size(3, 3));

	// 9 Erode
	erode(imageStep8, imageStep9, kernelStep8);

	// 10
	Mat imageStep10;
	erode(imageStep9, imageStep10, kernelStep8);

	// 11
	Mat imgStep11;
	threshold(imageStep10, imgStep11, 0, 255, THRESH_OTSU);

	// 11 Find the contours which should be the obvious chars by now, and save them in seperate files.
	cv::Mat contourMat = cv::Mat(imageStep10.size(), CV_8UC1, cv::Scalar(0));

	imshow(" FINALIMG", imageStep10);

	// 12 Find the contours in the newly filtered image
	findContours(imageStep10, contoursStep11, hierarchyStep12, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contoursStep11.size(); i++)
	{
		// Draw the given contour, on the black canvas.
		drawContours(contourMat, contoursStep11, i, cv::Scalar(255), FILLED);
		save_image("Resources/Plates/Contours/Contour" + std::to_string(i) + ".jpg", contourMat);

		// Reset the image again to black.
		contourMat.setTo(cv::Scalar(0));
	}

	//13 Save final image to use for tesseract
	tesseractSaveString = "Resources/Plates/TesseractReady/Licenseplate.jpg";
	save_image(tesseractSaveString, imageStep10);

	// 13 Use tesseract to read the license plate
	char* outText;
	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	// 14 Open input image with leptonica library
	Pix* image = pixRead("Resources/Plates/TesseractReady/Licenseplate.jpg");
	api->SetImage(image);
	// 15 Get OCR result
	outText = api->GetUTF8Text();
	printf("OCR output:\n%s", outText);

	// 16 Destroy used object and release memory
	api->End();
	delete api;
	delete[] outText;
	pixDestroy(&image);
	waitKey(0);
}

int main()
{
	Detect_License_Plate();
	Read_License_Plate();
}
