#include <iostream>
#include <opencv2/opencv.hpp>
#include <libfreenect/libfreenect.hpp>
#include "Freenect_Controller.h"
#include <time.h>
#include <math.h>

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    bool die(false);
	string filename("snapshot");
	string suffix(".png");
	int i_snap(0),iter(0);

	Mat depthMat(Size(640,480),CV_16UC1);
	Mat depthf (Size(640,480),CV_8UC1);
	Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
	Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));

	// The next two lines must be changed as Freenect::Freenect
	// isn't a template but the method createDevice:
	// Freenect::Freenect<MyFreenectDevice> freenect;
	// MyFreenectDevice& device = freenect.createDevice(0);
	// by these two lines:

	Freenect::Freenect freenect;
	Freenect_Controller& device = freenect.createDevice<Freenect_Controller>(0);

	//namedWindow("rgb",CV_WINDOW_AUTOSIZE);
	namedWindow("depth",CV_WINDOW_AUTOSIZE);
	//device.startVideo();
	device.startDepth();

	time_t start_,stop_;
	double seconds;


	while (!die) {
        time(&start_);
        //device.getVideo(rgbMat);
		device.getDepth(depthMat);
		//cv::imshow("rgb", rgbMat);
		depthMat.convertTo(depthf, CV_8UC1, 255.0/2048.0);
        time(&stop_);

        double seconds = difftime (stop_, start_);
        double fps = pow(seconds, -1);


		cv::imshow("depth",depthf);
		char k = cvWaitKey(5);
		if( k == 27 ){
			//cvDestroyWindow("rgb");
			cvDestroyWindow("depth");
			break;
		}
		if( k == 8 ) {
			std::ostringstream file;
			file << filename << i_snap << suffix;
			cv::imwrite(file.str(),rgbMat);
			i_snap++;
		}
		if(iter >= 1000) break;
		iter++;
	}

    //device.stopVideo();
    device.stopDepth();

	return 0;
}
