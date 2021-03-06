//
// Utilises the openCV library. Please see license http://opencv.org/license.html
//

#include "stdafx.h"
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>

int pixel_step, channels, pixel_step_out, channels_out;

int xt=1, yt=1, x, y, i, key=0, count=1;

uchar *data_out, *data;
IplImage *out=NULL;
IplImage *frame = NULL;
IplImage *WorkingFrame=NULL;
IplImage *frame1 = NULL;
IplImage *Frame_at_t = NULL;
IplImage *Frame_at_t_dt = NULL;
IplImage *eig_image = NULL;
IplImage *temp_image = NULL;
IplImage *pyramid1 = NULL;
IplImage *frameone = NULL;
IplImage *frametwo = NULL;
IplImage *dots = NULL;

int p=1;

IplImage *pyramid2 = NULL;
CvSeq* first_contour, *contours2;
CvMemStorage* storage = cvCreateMemStorage();	
double Result, Result2;
CvRect rect;

static int array[2]={0,0};

int* findhand(CvCapture *webcam) {
	
	//---Initialise Variables for Optical Flow---//
	CvSize OF_window = cvSize(3,3);						//Setup the size of the window of each pyramid level
	int no_of_points = 15000;
	CvPoint2D32f Frame_t_points[15000];
	CvPoint2D32f Frame_t_dt_points[15000];
	char optical_flow_found_feature[15000];
	float optical_flow_feature_error[15000];
	CvTermCriteria optical_flow_termination_criteria = cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 );

	frame = cvQueryFrame(webcam);						//Grab a frame from the webcam at time t
	WorkingFrame=cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);		//Build a working image

	dots=cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);			//Build a working image

	//----Setup parameters for cvCalcOpticalFlowPyrLK------//
	Frame_at_t = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);		//Build image for Frame at t
	cvConvertImage(frame, Frame_at_t, 0);					//Populate Frame_at_t with filtered data from WorkingFrame

	Sleep(40);								//A delay of 30ms (1 frame) to allow dt to pass

	frame = cvQueryFrame(webcam);						//Grab a frame from the webcam at time t+dt
	
	Frame_at_t_dt = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);	//Build image for Frame at t
	cvConvertImage(frame, Frame_at_t_dt, 0);				//Populate Frame_at_t_dt with filtered data from WorkingFrame

	eig_image = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 1);		//Set up temporary floating-point 32-bit image
	temp_image = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 1);		//Another temporary image of the same size and same format as eig_image

	cvGoodFeaturesToTrack(Frame_at_t, eig_image, temp_image, Frame_t_points, &no_of_points, .01, .01, NULL);
		
	pyramid1 = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
	pyramid2 = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
		
	cvCalcOpticalFlowPyrLK(Frame_at_t, Frame_at_t_dt, pyramid1, pyramid2, Frame_t_points, Frame_t_dt_points, no_of_points, OF_window, 5, optical_flow_found_feature, optical_flow_feature_error, optical_flow_termination_criteria, 0 );
		
	pixel_step = frame->widthStep;						//Assign pixel step from grabbed frame
	channels = frame->nChannels;						//Assign no. channels from grabbed frame
	pixel_step_out = frame->widthStep;					//Assign pixel step from grabbed frame
	channels_out = frame->nChannels;					//Assign no. channels from grabbed frame
	data = (uchar *)frame->imageData;					//Assign pointer for source frame
	data_out = (uchar *)WorkingFrame->imageData;				//Assign pointer for Working frame

	//---Scan through grabbed frame, check pixel colour. If mostly red, saturate corresponding pixel in ---//
	//---output working image. This is designed to filter out nosie and detect skin------------------------//
	for(y=0; y<(frame->height); y++) {
		for(x=0 ; x<(frame->width); x++) {
			if(((data[y*pixel_step+x*channels+2]) > (20+data[y*pixel_step+x*channels])) && ((data[y*pixel_step+x*channels+2]) > (20+data[y*pixel_step+x*channels+1]))) {
				data_out[y*pixel_step_out+x*channels_out]=255;
			}
			else {
				data_out[y*pixel_step_out+x*channels_out]=0;
			}
		}
	}
	
	for (i = 0; i < no_of_points; i++) {
			
		if (optical_flow_found_feature[i] != 0) {
			
		double calc=(((int)Frame_t_points[i].x-(int)Frame_t_dt_points[i].x)^2)+(((int)Frame_t_points[i].x-(int)Frame_t_dt_points[i].x)^2);
		double calc2=(((int)Frame_t_points[i].y-(int)Frame_t_dt_points[i].y)^2)+(((int)Frame_t_points[i].y-(int)Frame_t_dt_points[i].y)^2);
		double calc3=(calc*calc)+(calc2*calc2);
		
		int calc4 = abs((int)sqrt(calc3));
		int thresh = 24;					//Apply Optical Flow Threshold

		if (calc4>thresh) {
			if ((data_out[(int)Frame_t_points[i].y*pixel_step+(int)Frame_t_points[i].x*channels])>0) {
				cvCircle(dots, cvPoint((int)Frame_t_points[i].x, (int)Frame_t_points[i].y), 1, CV_RGB(100, 255, 100), 2);
				xt=xt+(int)Frame_t_points[i].x;
				yt=yt+(int)Frame_t_points[i].y;
				count=count+1;
			}
		}
		}
	}
	cvShowImage("Sat", dots);

	if (count > 12 ) {						//If the number of pixels is above a threshold
			array[0]=(int)abs((double)(xt/count)/(double)640*100);			
			array[1]=(int)abs((double)(yt/count)/(double)480*100);
			xt=0;
			yt=0;
			count=0;
	}
		
	if (array[0]>frame->width) {
		array[0]=frame->width;
	}
		
	if (array[0]<0) {
		array[0]=0;						//'Hold' Function
	}
		
	if (array[1]>frame->width) {
		array[1]=0;
	}
		
	if (array[1]<0) {
		array[1]=0;
	}

	return(0);
}

int main(void) 
{
	//---Setup capture and construct output windows---//
	CvCapture *webcam = cvCaptureFromCAM(0);
	cvNamedWindow("Webcam", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Sat", CV_WINDOW_AUTOSIZE);

	while (key != 27) {
		findhand(webcam);					//Call Hand Detector
		cvCircle(frame, cvPoint((int)abs(array[0]/(double)100*640), (int)abs(array[1]/(double)100*480)), 5, CV_RGB(0, 100, 255), 10);
		printf("X: %d \n", array[0]);
		printf("Y: %d \n", array[1]);
		if (array[0]<(int)abs((double)(frame->width/2)*(double)640/(double)100)) {
			printf("Right \n");
		}
		else {
			printf("Left \n");
		}
		if (array[1]>(int)abs((double)(frame->height/2)*(double)480/(double)100)) {
			printf("Down \n");
		}
		else {
			printf("Up \n");
		}
		cvShowImage("Webcam", frame);				//Output Image to Frame
		key=cvWaitKey(1);					//Check if user has pressed 'esc'
	}
	
	return(0);

}
