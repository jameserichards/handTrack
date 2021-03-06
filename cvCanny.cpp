//
// Utilises the openCV library. Please see license http://opencv.org/license.html
//

#include "stdafx.h"
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <stdio.h>

using namespace std;

int main() {
	int c = 0;															
	
    	CvCapture* capture = cvCaptureFromCAM(0);			//Set Up Capture Variable	
    	CvSize sz = cvGetSize(cvQueryFrame(capture));		//Set up frame variable size			
	IplImage* src = cvCreateImage(sz, IPL_DEPTH_8U, 3);	//	
	IplImage* out = cvCreateImage(sz,  IPL_DEPTH_8U, 1);	//Set up image proccessing variables
	IplImage* out2 = cvCreateImage(sz,  IPL_DEPTH_8U, 1);	//
	IplImage* out3 = cvCreateImage(sz,  IPL_DEPTH_8U, 1);	//

	while(c != 27) {
		src = cvQueryFrame(capture);		//Capture Frame			
		cvNamedWindow("src",1);			//Create output window		
		cvShowImage("src", src);		//Put original frame in Window
		cvCvtColor(src, out, CV_BGR2GRAY);	//Convert frame to Greyscale
		cvSmooth(out, out2, CV_MEDIAN, 7, 7);	//Smooth out frame
		cvCanny(out2, out3, 5, 50);		//Perform Canny edge detection
		cvNamedWindow("image proc",1);		//Create Window for Canny output
		cvShowImage("image proc", out3);	//Put Canny output in Window
		c = cvWaitKey(10);			//Wait to see if the user presses 'esc'
	}
	cvReleaseCapture(&capture);			//Release capture variable
	cvDestroyAllWindows();				//Close all Windows
} 
