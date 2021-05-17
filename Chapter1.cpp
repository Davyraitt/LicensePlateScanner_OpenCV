// #include <opencv2/imgcodecs.hpp>
// #include <opencv2/highgui.hpp>
// #include <opencv2/imgproc.hpp>
// #include <opencv2/objdetect.hpp>
// #include "opencv2/opencv.hpp"
// //#include <opencv2/text/ocr.hpp>
// #include <iostream>
//
//
// using namespace cv;
// using namespace std;
//
// //Variables
// const int windowHeight = 360, windowWidth = 640;
// //detectMultiScale variables
// int scaleF = 1, scaleFD = 1, minN = 10;
// //Saved license plate filename
// string extractedLicensePlate = "nummerbord0";
//
// bool load_xml(CascadeClassifier cascade)
// {
// 	cascade.load("Resources/haarcascade_russian_plate_number.xml");
//
// 	if (cascade.empty())
// 	{
// 		cout << "XML could not be loaded" << endl;
// 	}
//
// 	return cascade.empty();
// }
//
// void save_image(string location, Mat img)
// {
// 	imwrite(location, img);
// }
//
// void display_detect_license_plates()
// {
// 	string path = "Resources/Plates/russian2.jpg";
// 	Mat img = imread(path);
//
// 	CascadeClassifier plateCascade;
// 	plateCascade.load("Resources/haarcascade_russian_plate_number.xml");
//
// 	vector<Rect> plates;
// 	plateCascade.detectMultiScale(img, plates, 1.1, 20);
//
// 	for (int i = 0; i < plates.size(); i++)
// 	{
// 		Mat imgCrop = img(plates[i]);
// 		//imshow("License plate " + to_string(i), imgCrop);
// 		extractedLicensePlate = "Resources/Plates/nummerbord" + to_string(i) + ".png";
// 		save_image(extractedLicensePlate, imgCrop);
// 		rectangle(img, plates[i].tl(), plates[i].br(), Scalar(255, 0, 255), 3);
// 	}
//
// 	//imshow("Image", img);
// }
//
// void extractText()
// {
// 	int xThreshold = 41, yThreshold = 245;
//
// 	//EXTRACTING TEXT
// 	//Read the input
// 	// 1 Convert to grayscale
// 	// 2	Threshold
// 	// 3	Use morphology to remove small white or black regions and to close over the text with white
// 	// 4	Get the largest vertically oriented rectangle's contour
// 	// 5	Extract the text from the bounding box of that contour
// 	// 6	Save results
// 	////Read the input
// 	string path = extractedLicensePlate;
// 	Mat img = imread(path);
//
// 	// 1 Convert to grayscale
// 	Mat imgGray;
// 	cvtColor(img, imgGray, COLOR_BGR2GRAY);
// 	imshow("imgGray", imgGray);
//
// 	// 2	Threshold
// 	Mat imgTreshhold;
// 	threshold(imgGray, imgTreshhold, xThreshold, yThreshold, THRESH_BINARY);
// 	//imshow("imgTreshhold", imgTreshhold);
//
// 	// 3	Use morphology to remove small white or black regions and to close over the text with white
// 	Mat imgEroded, imgEroded2, imgDilated, imgDilated2;
// 	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
//
// 	// Creates a kernel that we can use with dilation, bigger = dilate more
// 	erode(imgTreshhold, imgEroded, kernel);
// 	dilate(imgEroded, imgDilated, kernel);
// 	dilate(imgDilated, imgDilated2, kernel);
// 	erode(imgDilated2, imgEroded2, kernel);
// 	imshow("Image morph", imgEroded2);
//
// 	// 4	Get the largest vertically oriented rectangle's contour
// 	Mat imgEroded3;
// 	Mat kernelStep4 = getStructuringElement(MORPH_RECT, Size(3, 3));
// 	// Creates a kernel that we can use with dilation, bigger = dilate more
// 	erode(imgEroded2, imgEroded3, kernelStep4);
// 	imshow("imgEroded3", imgEroded3);
//
//
// 	// 5	Extract the text from the bounding box of that contour
// 	vector<vector<Point>> contours;
// 	vector<Vec4i> hierachy;
// 	vector<Point> contour;
// 	findContours(imgEroded3, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//
// 	cout << contours.size() << endl;
//
// 	if (contours.size() == 1)
// 	{
// 		contour = contours[0];
// 	}
// 	else
// 	{
// 		contour = contours[1];
// 	}
//
// 	int area_thresh = 0;
// 	vector<Point> big_contour;
// 	for (vector<Point> p : contours)
// 	{
// 		double area = contourArea(p);
// 		Rect boundingBox = boundingRect(p);
// 		double aspect = boundingBox.width / boundingBox.height;
//
// 		if (area > area_thresh)
// 		{
// 			if (aspect > 1)
// 			{
// 				big_contour = p;
// 				area_thresh = area;
// 			}
// 		}
// 	}
//
// 	Rect boundingBox = boundingRect(big_contour);
// 	Mat imgCrop = img(boundingBox);
// 	//imshow("Image cropped", imgCrop);
//
// 	//Saving cropped image to resources
// 	string NNPATH = "Resources/Plates/NummerBordNoNoise.png";
// 	save_image(NNPATH, imgCrop);
//
// 	//We now have a cropped easy to read image. We can try and extract text from here.
// 	//Finding contours
// 	Mat imgNoNoise = imread(NNPATH);
// 	//imshow(" imgNoNoise", imgNoNoise);
//
// 	//FIND CONTOURS
// 	Mat imageStep7, imgNoNoiseBlurred, imageStep8, imageStep10, imageStep9;
// 	cvtColor(imgNoNoise, imageStep7, COLOR_BGRA2GRAY);
// 	imshow(" imageStep7", imageStep7);
// 	threshold(imageStep7, imageStep8, 0, 255, THRESH_OTSU);
// 	Mat kernel2 = getStructuringElement(MORPH_RECT, Size(3, 3));
// 	erode(imageStep8, imageStep9, kernel2);
// 	threshold(imageStep9, imageStep10, 0, 255, THRESH_OTSU);
// 	imshow(" imageStep9", imageStep9);
// 	imshow(" imageStep10", imageStep10);
//
// 	vector<vector<Point>> contoursStep11;
// 	vector<Vec4i> hierarchyStep12;
// 	cv::Mat contourMat = cv::Mat(imageStep10.size(), CV_8UC1, cv::Scalar(0));
// 	findContours(imageStep10, contoursStep11, hierarchyStep12, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
// 	for (int i = 0; i < contoursStep11.size(); i++)
// 	{
// 		//# Draw the given contour, on the black canvas.
// 		drawContours(contourMat, contoursStep11, i, cv::Scalar(255), FILLED);
// 		save_image("Resources/Plates/Contour" + std::to_string(i) + ".jpg" , contourMat);
//
// 		//# Reset the image again to black.
// 		contourMat.setTo(cv::Scalar(0));
// 	}
// 	waitKey(0);
//
// 	//Optional: Running OCRTesseract to recognize chars on the sorted image
// 	//Filtered image is: contoursStep11
//
// }
//
//
// int main()
// {
// 	display_detect_license_plates();
//
// 	extractText();
// }
