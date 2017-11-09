#include <iostream>                        // for standard I/O    cout
#include <string>                          // for strings
#include <ctime>

#include <opencv2/core.hpp>                // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp>             // Video write
#include <opencv2/highgui/highgui.hpp>     // CV_FOURCC

using namespace std;
using namespace cv;

int width = 640;
int height = 480;
int fps = 30;
int frames = 1000;
RNG rng(static_cast<int>(time(nullptr)));

static Mat NextFrame(Mat src)
{
	Mat res;
	src.copyTo(res);
		
	for (int i = 2; i < width - 2; i++)
	{
		for (int j = 2; j < height - 2; j++)
		{			
			int neighbor5 = 0;
		
			for (int ii = -2; ii < 3; ii++)
			{
				for (int jj = -2; jj < 3; jj++)
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

//VideoWrite 640 480 30 1000 D:\Test.avi
int main(int argc, char *argv[])
{
	if (argc != 6)
    {
        cout << "Not enough parameters" << endl;
        return -1;
    }

	width = atoi(argv[1]);
	height = atoi(argv[2]);
	fps = atoi(argv[3]);
	frames = atoi(argv[4]);

	const string NAME = argv[5];

	Size S = Size(width, height);
	VideoWriter outputVideo(NAME, CV_FOURCC('X','V','I','D'), fps, S, 0);

    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: " << NAME << endl;
        return -1;
    }

	Mat img(S,CV_8UC1,Scalar::all(255));

	for (int i = width/4; i < width/2; i++)
	{
		for (int j = height/4; j < height/2; j++)
		{			
			img.at<uchar>(j,i) = 0;
		}
	}

	for (int i = width/2; i < width/4*3; i++)
	{
		for (int j = height/2; j < height/4*3; j++)
		{			
			img.at<uchar>(j,i) = 0;
		}
	}

	double begin = double(getTickCount());

	for (int i = 0; i < frames; i++)
	{
		img = NextFrame(img);
		outputVideo.write(img);
	}
	double duration = (double(getTickCount()) - begin) / getTickFrequency();

	cout << "Run Time = " << duration << "s" << endl;
	cout << "FPS = " << frames / duration << endl;
    cout << "Finish!" << endl;
    return 0;
}