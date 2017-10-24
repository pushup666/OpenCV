#include <iostream> // for standard I/O
#include <string> // for strings
#include <iomanip> // for controlling float print precision
#include <sstream> // string to number conversion
#include <fstream> 

#include <opencv2/core.hpp> // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp> // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp> // OpenCV window I/O


using namespace std;
using namespace cv;

double getPSNR(const Mat& I1, const Mat& I2);
Scalar getMSSIM(const Mat& I1, const Mat& I2);


int main()
{
	//const string sourceReference = "D:\\Codecs\\sample.mkv";
	//const string sourceCompare = "D:\\Codecs\\sample.mp4";
	const string sourceReference = "D:\\Codecs\\TEST.avi";
	const string sourceCompare = "D:\\Codecs\\TEST.mkv";

	VideoCapture captRef(sourceReference);
	VideoCapture captCom(sourceCompare);

	if (!captRef.isOpened())
	{
		cout << "Could not open REFERENCE video" << sourceReference << endl;

		system("pause");
		return -1;
	}

	if (!captCom.isOpened())
	{
		cout << "Could not open case COMPARE video" << sourceCompare << endl;

		system("pause");
		return -1;
	}

	int refWidth		= int(captRef.get(CAP_PROP_FRAME_WIDTH));
	int refHeight		= int(captRef.get(CAP_PROP_FRAME_HEIGHT));
	int refFrameCount	= int(captRef.get(CAP_PROP_FRAME_COUNT));

	int comWidth		= int(captCom.get(CAP_PROP_FRAME_WIDTH));
	int comHeight		= int(captCom.get(CAP_PROP_FRAME_HEIGHT));
	int comFrameCount	= int(captCom.get(CAP_PROP_FRAME_COUNT));

	if (refWidth != comWidth || refHeight != comHeight)
	{
		cout << "REFERENCE Width=" << refWidth << " Height=" << refHeight << endl;
		cout << "COMPARE   Width=" << comWidth << " Height=" << comHeight << endl;
		cout << "Inputs have different size! Closing." << endl;

		system("pause");
		return -1;
	}

	if (refFrameCount != comFrameCount)
	{
		cout << "REFERENCE FrameCount=" << refFrameCount << endl;
		cout << "COMPARE   FrameCount=" << comFrameCount << endl;
		cout << "Inputs have different frame, but we will continue" << endl;

		system("pause");
	}

	int minFrameCount = min(refFrameCount,comFrameCount);

	const char* WIN_REF = "Reference";
	const char* WIN_CMP = "Compare";

	// Windows
	namedWindow(WIN_REF, WINDOW_AUTOSIZE);
	namedWindow(WIN_CMP, WINDOW_AUTOSIZE);
	moveWindow(WIN_REF, 0, 0);
	moveWindow(WIN_CMP, refWidth, 0);

	cout << "Width=" << refWidth << " Height=" << refHeight << " FrameCount=" << refFrameCount << endl;

	Mat frameReference, frameCompare;
	double psnrV;
	Scalar mssimV;

	ofstream outTxt("D:\\Codecs\\data.txt");
	for (int i = 1; i <= minFrameCount; i++) //Show the image captured in the window and repeat
	{
		captRef >> frameReference;
		captCom >> frameCompare;

		psnrV = getPSNR(frameReference, frameCompare);
		mssimV = getMSSIM(frameReference, frameCompare);

		cout << "Frame: " << i << "# ";
		cout << setiosflags(ios::fixed) << setprecision(3) << psnrV << "dB";
		cout << " MSSIM: "
             << " R " << setiosflags(ios::fixed) << setprecision(2) << mssimV.val[2] * 100 << "%"
             << " G " << setiosflags(ios::fixed) << setprecision(2) << mssimV.val[1] * 100 << "%"
             << " B " << setiosflags(ios::fixed) << setprecision(2) << mssimV.val[0] * 100 << "%";
		cout << endl;

		outTxt << psnrV << ", "<< (mssimV.val[0] + mssimV.val[1] + mssimV.val[2]) / 3 << "\n";

		////////////////////////////////// Show Image /////////////////////////////////////////////
		imshow(WIN_REF, frameReference);
		imshow(WIN_CMP, frameCompare);
		
		waitKey(1);
	}
	outTxt.close();
	//matlab m file
	//data = load('D:\Codecs\data.txt');
	//figure(1);;plot(data(:,1));title('psnr');
	//figure(2);;plot(data(:,2));title('ssim');
	
	cout << "Finish!";
	system("pause");
	return 0;
}

double getPSNR(const Mat& I1, const Mat& I2)
{
	Mat s1;
	absdiff(I1, I2, s1); // |I1 - I2|
	s1.convertTo(s1, CV_32F); // cannot make a square on 8 bits
	s1 = s1.mul(s1); // |I1 - I2|^2

	Scalar s = sum(s1); // sum elements per channel

	double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

	if (sse <= 1e-10) // for small values return zero
		return 0;
	else
	{
		double mse = sse / double(I1.channels() * I1.total());
		double psnr = 10.0 * log10((255 * 255) / mse);
		return psnr;
	}
}

Scalar getMSSIM(const Mat& i1, const Mat& i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	/***************************** INITS **********************************/
	int d = CV_32F;

	Mat I1, I2;
	i1.convertTo(I1, d); // cannot calculate on one byte large values
	i2.convertTo(I2, d);

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

	Scalar mssim = mean(ssim_map); // mssim = average of ssim map
	return mssim;
}