#include <iostream>                        // for standard I/O    cout
#include <string>                          // for strings
#include <ctime>

#include <opencv2/core.hpp>                // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp>             // Video write
#include <opencv2/highgui/highgui.hpp>     // CV_FOURCC

using namespace std;
using namespace cv;

int width = 1920;
int height = 1080;


int main(int argc, char *argv[])
{
	Size S = Size(width, height);

	Mat img(S, CV_8UC1, Scalar::all(0));

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if ((i+j)%2==1)
			{
				img.at<uchar>(j, i) = 255;
			}			
		}
	}

	imwrite("D:\111.png", img);

	return 0;
}