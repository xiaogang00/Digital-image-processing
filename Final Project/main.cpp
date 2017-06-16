#include<iostream>
#include"HazeRemove.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void TestImg()
{
    const float w0 = 0.95;
	int r = 8;
	double eps = 0.04;
	//Mat img = imread("test2.jpg");
	Mat img = imread("test.jpg");
    Mat gray;
    cvtColor(img, gray, CV_BGR2GRAY);
    
    HazeRemove hazeRomove(img,7,w0);
    hazeRomove.getDarkChannelPrior();
    hazeRomove.getEstimatedTransmissionMap();
	hazeRomove.guidedFilter(gray, hazeRomove.estimatedTransmissionMap, r, eps);
    //imshow("estimatedTransmissionMap", hazeRomove.estimatedTransmissionMap);
    //waitKey(5);
    
    Mat estimatedHazeRemoveImg = hazeRomove.getHazeRemoveImg(hazeRomove.estimatedTransmissionMap);
    //Mat transmissionMapHazeRemove = hazeRomove.getHazeRemoveImg(hazeRomove.transmissionMap);
    Mat filterGuidedHazeRemoveImg = hazeRomove.getHazeRemoveImg(hazeRomove.guidedfilterTransmissionMap);
    
    imshow("src", img);
    waitKey(5);
    imshow("darkChanel", hazeRomove.darkChannelImg);
    waitKey(5);
    imshow("estimatedTransmissionMap", hazeRomove.estimatedTransmissionMap);
    waitKey(5);
    imshow("estimatedHazeRemoveImg", estimatedHazeRemoveImg);
    waitKey(5);

	imshow("guidedfilterTransmissionMap", hazeRomove.guidedfilterTransmissionMap);
    waitKey(5);
	imshow("filterGuidedHazeRemoveImg", filterGuidedHazeRemoveImg);
	waitKey(0);
}

int main()
{
    TestImg();
    return 0;
}
