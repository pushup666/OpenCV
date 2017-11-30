#include <iostream> // for standard I/O
#include <string> // for strings
#include <iomanip> // for controlling float print precision
//#include <sstream> // string to number conversion
#include <fstream>

#include <opencv2/core.hpp> // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp> // Gaussian Blur
#include <opencv2/videoio.hpp>
//#include <opencv2/highgui.hpp> // OpenCV window I/O


using namespace std;
using namespace cv;

const int MaxVideoFrameCount = 5000;
vector<vector<double>> result(MaxVideoFrameCount, vector<double>(2));

const int MaxFrameCountPerVector = 128;
vector<int> FramesIndex;vector<Mat> RefFrames;vector<Mat> ComFrames;

static int VideoFrameCount;

void getPSNR(int frame, const Mat& i1, const Mat& i2)
{
	Mat s1;
	absdiff(i1, i2, s1); // |I1 - I2|
	s1.convertTo(s1, CV_32F); // cannot make a square on 8 bits
	s1 = s1.mul(s1); // |I1 - I2|^2

	Scalar s = sum(s1); // sum elements per channel

	double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

	if (sse <= 1e-10)
	{
		result[frame][0] = 200;
		return;
	}
	double mse = sse / double(i1.channels() * i1.total());
	double psnr = 10.0 * log10((255 * 255) / mse);
	result[frame][0] = psnr;
}

void getMSSIM(int frame, const Mat& i1, const Mat& i2)
{
	//C1=(K1*L)^2, C2=(K2*L)^2是用来维持稳定的常数。L是像素值的动态范围。K1=0.01, K2=0.03, L=255。
	const double C1 = 6.5025, C2 = 58.5225;

	/***************************** INITS **********************************/

	Mat I1, I2;
	i1.convertTo(I1, CV_32F); // cannot calculate on one byte large values
	i2.convertTo(I2, CV_32F);

	Mat I2_2 = I2.mul(I2); // I2^2
	Mat I1_2 = I1.mul(I1); // I1^2
	Mat I1_I2 = I1.mul(I2); // I1 * I2

							/*************************** END INITS **********************************/

	Mat mu1, mu2; // PRELIMINARY COMPUTING
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);

	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);

	Mat sigma1_2, sigma2_2, sigma12;

	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;

	///////////////////////////////// FORMULA ////////////////////////////////

	Mat t1, t2, t3;

	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2); // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2); // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

	Mat ssim_map;
	divide(t3, t1, ssim_map); // ssim_map =  t3./t1;

	Scalar mssimV = mean(ssim_map); // mssim = average of ssim map
	double mssim = (mssimV.val[0] + mssimV.val[1] + mssimV.val[2]) / 3;
	result[frame][1] = mssim;
}

class ParallelVideoCompare : public ParallelLoopBody
{
public:
	ParallelVideoCompare(int frameCount) : m_frameCount(frameCount)
	{
	}

	void operator()(const Range& range) const override
	{
		Mat frameReference, frameCompare;

		for (int r = range.start; r < range.end; r++)
		{
			frameReference = RefFrames[r];
			frameCompare = ComFrames[r];

			getPSNR(FramesIndex[r], frameReference, frameCompare);
			getMSSIM(FramesIndex[r], frameReference, frameCompare);
		}
	}

private:
	int m_frameCount;
};

void compareVideoToResultSequential()
{
	Mat frameReference, frameCompare;
	for (int i = 0; i < FramesIndex.size(); i++)
	{
		frameReference = RefFrames[i];
		frameCompare = ComFrames[i];

		getPSNR(FramesIndex[i], frameReference, frameCompare);
		getMSSIM(FramesIndex[i], frameReference, frameCompare);
	}

}

void compareVideoToResultParallel()
{
	ParallelVideoCompare parallelVideoCompare(VideoFrameCount);
	parallel_for_(Range(0, FramesIndex.size()), parallelVideoCompare);
}

void writeResultToText(string fileName)
{
	ofstream outTxt(fileName);
	for (int i = 0; i < min(VideoFrameCount, MaxVideoFrameCount); i++)
	{
		outTxt << result[i][0] << ", " << result[i][1] << "\n";
	}
	outTxt.close();
}


//VideoCompareParallel referenceFileName compareFileName dataFileName
int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		cout << "Not enough parameters" << endl;
		return -1;
	}
	const string referenceFileName = argv[1];
	const string compareFileName = argv[2];
	const string dataFileName = argv[3];

	//const string referenceFileName = "D:\\Codecs\\sample.mkv";
	//const string compareFileName = "D:\\Codecs\\sample.mp4";
	//const string dataFileName = "D:\\Codecs\\sample.txt";


	VideoCapture captReference(referenceFileName);
	VideoCapture captCompare(compareFileName);





	//Check START
	if (!captReference.isOpened())
	{
		cout << "Could not open REFERENCE video" << referenceFileName << endl;
		system("pause");
		return -1;
	}

	if (!captCompare.isOpened())
	{
		cout << "Could not open COMPARE video" << compareFileName << endl;
		system("pause");
		return -1;
	}

	int refWidth = int(captReference.get(CAP_PROP_FRAME_WIDTH));
	int refHeight = int(captReference.get(CAP_PROP_FRAME_HEIGHT));
	int refFrameCount = int(captReference.get(CAP_PROP_FRAME_COUNT));

	int comWidth = int(captCompare.get(CAP_PROP_FRAME_WIDTH));
	int comHeight = int(captCompare.get(CAP_PROP_FRAME_HEIGHT));
	int comFrameCount = int(captCompare.get(CAP_PROP_FRAME_COUNT));

	if (refWidth != comWidth || refHeight != comHeight)
	{
		cout << "REFERENCE Width=" << refWidth << " Height=" << refHeight << endl;
		cout << "COMPARE   Width=" << comWidth << " Height=" << comHeight << endl;
		cout << "Inputs have different size, we will stop!" << endl;

		system("pause");
		return -1;
	}

	if (refFrameCount != comFrameCount)
	{
		cout << "REFERENCE FrameCount = " << refFrameCount << endl;
		cout << "COMPARE   FrameCount = " << comFrameCount << endl;
		cout << "Inputs have different frame, but we will continue." << endl;
	}

	VideoFrameCount = min(refFrameCount, comFrameCount);

	int step = 1;int endFrameCount = VideoFrameCount;
	if (VideoFrameCount > MaxVideoFrameCount)
	{
		step = VideoFrameCount / MaxVideoFrameCount;
		endFrameCount = MaxVideoFrameCount * step;
		//cout << "FrameCount(" << VideoFrameCount << ") > MaxFrameCount(" << MaxVideoFrameCount << ")" << endl;
		//VideoFrameCount = MaxVideoFrameCount;
		//system("pause");
	}
	//Check END

	cout << "Width = " << refWidth << " Height = " << refHeight << " FrameCount = " << VideoFrameCount << endl;
	cout << setiosflags(ios::fixed) << setprecision(2);

	double begin = double(getTickCount());

	Mat frameReference, frameCompare;
	for (int i = 0; i < endFrameCount; i += step)
	{
		for (int j = 0; j < step; j++)
		{
			captReference >> frameReference;
			captCompare >> frameCompare;
		}		

		FramesIndex.push_back(i / step);
		RefFrames.push_back(frameReference.clone());
		ComFrames.push_back(frameCompare.clone());

		if (FramesIndex.size() == MaxFrameCountPerVector || i == endFrameCount - step)
		{
			//compareVideoToResultSequential();
			compareVideoToResultParallel();

			FramesIndex.clear();
			RefFrames.clear();
			ComFrames.clear();

			cout << "\r" << "FrameCount: " << i + 1 << "/" << VideoFrameCount << "(" << (double(i + 1) / VideoFrameCount * 100) << "%)" << flush;
		}
	}

	writeResultToText(dataFileName);

	double duration = (double(getTickCount()) - begin) / getTickFrequency();
	cout << endl;
	cout << "Run Time = " << duration << "s" << endl;
	cout << "Finish!" << endl;
	//system("pause");
	return 0;
}