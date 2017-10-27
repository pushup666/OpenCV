#include <opencv2/opencv.hpp>
#include <time.h>
#include "test.hpp"
 
using namespace cv;
using namespace std;

int main()
{
	Mat testInput = Mat::ones(2000,400000, CV_32F);
	clock_t start, stop;

	start = clock();
	parallelTestWithFor(testInput);
	stop = clock();
	cout<<"Running time using \'for\':"<<(double)(stop - start)/CLOCKS_PER_SEC<<"s"<<endl;

	start = clock();
	parallelTestWithParallel_for_(testInput);
	stop = clock();
	cout<<"Running time using \'parallel_for_\':"<<(double)(stop - start)/CLOCKS_PER_SEC<<"s"<<endl;

	system("pause");
}