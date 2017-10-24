#include <iostream>                        // for standard I/O    cout
#include <string>                          // for strings
#include <ctime>

#include <opencv2/core.hpp>                // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp>             // Video write
#include <opencv2/highgui/highgui.hpp>     // CV_FOURCC

using namespace std;
using namespace cv;

int width = 400;
int height = 400;

int frames = 1000;
RNG rng(static_cast<int>(time(nullptr)));

static Mat NextFrame(Mat src)
{
	Mat res;
	src.copyTo(res);
		
	for (auto i = 2; i < width - 2; i++)
	{
		for (auto j = 2; j < height - 2; j++)
		{			
			auto neighbor5 = 0;
		
			for (auto ii = -2; ii < 3; ii++)
			{
				for (auto jj = -2; jj < 3; jj++)
				{
					neighbor5 += src.at<uchar>(j+jj,i+ii);
				}
			}

			neighbor5 -= src.at<uchar>(j,i);

			if (rng.uniform(0,256) > neighbor5/24)
			{
				res.at<uchar>(j,i) = 0;
			}
			else
			{
				res.at<uchar>(j,i) = 255;
			}
		}
	}

	return res;
}

int main()
{          
	const string NAME = "D:\\TEST.avi";
	auto S = Size(width, height);
	VideoWriter outputVideo(NAME, CV_FOURCC('X','V','I','D'), 30, S, 0);

    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: " << NAME << endl;
        return -1;
    }

	Mat img(S,CV_8UC1,Scalar::all(255));

	for (auto i = width/4; i < width/2; i++)
	{
		for (auto j = height/4; j < height/2; j++)
		{			
			img.at<uchar>(j,i) = 0;
		}
	}

	for (auto i = width/2; i < width/4*3; i++)
	{
		for (auto j = height/2; j < height/4*3; j++)
		{			
			img.at<uchar>(j,i) = 0;
		}
	}

	auto begin = static_cast<double>(getTickCount());

	for (auto i = 0; i < frames; i++)
	{
		img = NextFrame(img);
		outputVideo.write(img);
	}
	auto duration = static_cast<double>(getTickCount()) - begin;
	duration /= getTickFrequency();

	printf( "run time = %g s\n", duration);
	printf( "fps = %g \n", frames / duration);
    cout << "Finished writing" << endl;

	system("pause");
    return 0;
}