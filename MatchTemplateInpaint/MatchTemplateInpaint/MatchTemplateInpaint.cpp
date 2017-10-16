#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp> 
#include <iostream>
#include <stdio.h>
#include <fstream>  

#include <photo\photo.hpp>

using namespace std;
using namespace cv;

Mat _img; Mat _result;Mat _inpaint;
Mat _templ; Mat _mask;Mat _templ1; Mat _mask1;Mat _templ2; Mat _mask2;
Point matchLoc;

//CV_TM_SQDIFF        =0
//CV_TM_SQDIFF_NORMED =1
//CV_TM_CCORR         =2
//CV_TM_CCORR_NORMED  =3
//CV_TM_CCOEFF        =4
//CV_TM_CCOEFF_NORMED =5
int match_method = CV_TM_SQDIFF;

//CV_INPAINT_NS		=0
//CV_INPAINT_TELEA	=1
int inpaint_method = CV_INPAINT_TELEA;
int inpaint_radius = 5;

void MatchingTemplate(Mat templ);
void MatchingMask(Mat mask);

int main()
{
	ofstream out("D:\\Í¼Ïñ·ÖÎö\\2.txt");

	_img = imread("D:\\Í¼Ïñ·ÖÎö\\2.bmp");
	_templ = imread("D:\\Í¼Ïñ·ÖÎö\\plus.bmp", 1);
	_mask = imread("D:\\Í¼Ïñ·ÖÎö\\plusmask.bmp", 1);

	_templ1 = imread("D:\\Í¼Ïñ·ÖÎö\\plus1.bmp", 1);
	_mask1 = imread("D:\\Í¼Ïñ·ÖÎö\\plus1mask.bmp", 1);

	_templ2 = imread("D:\\Í¼Ïñ·ÖÎö\\plus2.bmp", 1);
	_mask2 = imread("D:\\Í¼Ïñ·ÖÎö\\plus2mask.bmp", 1);
		
	//cvtColor(_img, _img, CV_RGB2GRAY, 0);
	//cvtColor(_templ, _templ, CV_RGB2GRAY, 0);
	//cvtColor(_mask, _mask, CV_RGB2GRAY, 0);

	imshow("1", _img);

	MatchingTemplate(_templ1);
	MatchingMask(_mask1);
	imshow("2", _img);
	out << "plus1 (" << matchLoc.x+7 << ", " << matchLoc.y+13 << ")\n";

	MatchingTemplate(_templ2);
	MatchingMask(_mask2);
	imshow("3", _img);
	out << "plus2 (" << matchLoc.x+7 << ", " << matchLoc.y+13 << ")\n";

	MatchingTemplate(_templ);
	MatchingMask(_mask);
	imshow("4", _img);
	out << "plus (" << matchLoc.x+7 << ", " << matchLoc.y+7 << ")\n";

	MatchingTemplate(_templ);
	MatchingMask(_mask);
	imshow("5", _img);
	out << "plus (" << matchLoc.x+7 << ", " << matchLoc.y+7 << ")\n";

	out.close();

	Mat imageGray;
	cvtColor(_img, imageGray, CV_RGB2GRAY, 0);
	Mat imageMask = Mat(imageGray.size(), CV_8UC1, Scalar::all(0));
	threshold(imageGray, imageMask, 250, 255, CV_THRESH_BINARY);
	inpaint(_img, imageMask, _inpaint, inpaint_radius, inpaint_method);
	imshow("8", imageMask);
	imshow("9", _inpaint);
	cout << "Finish" <<endl;
	waitKey();
	return 0;
}

void MatchingTemplate(Mat templ)
{
	Mat img_display;
	_img.copyTo(img_display);

	int result_rows = _img.rows - templ.rows + 1;
	int result_cols = _img.cols - templ.cols + 1;
	
	_result.create(result_cols, result_rows, CV_32FC1);

	matchTemplate(_img, templ, _result, match_method);
	normalize(_result, _result, 0, 1, NORM_MINMAX, -1, Mat());
	
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	minMaxLoc(_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	matchLoc = (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED) ? minLoc : maxLoc;

	//rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols , matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	//rectangle(_result,  Point(matchLoc.x - 0.5*templ.cols , matchLoc.y - 0.5*templ.rows), Point(matchLoc.x + 0.5*templ.cols , matchLoc.y + 0.5*templ.rows), Scalar::all(0), 2, 8, 0);

	//imshow("image_window", img_display);
	//imshow("result_window", _result);
}

void MatchingMask(Mat mask)
{
	for (auto i = 0; i < mask.rows; i++)
	{
		for (auto j = 0; j < mask.cols; j++)
		{			
			if (mask.at<Vec3b>(i,j)[0] == 255)
			{
				_img.at<Vec3b>(i+matchLoc.y,j+matchLoc.x)[0] = 255;
				_img.at<Vec3b>(i+matchLoc.y,j+matchLoc.x)[1] = 255;
				_img.at<Vec3b>(i+matchLoc.y,j+matchLoc.x)[2] = 255;
			}
		}
	}
}

