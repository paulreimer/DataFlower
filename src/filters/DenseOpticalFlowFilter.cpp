#pragma once

#include "DenseOpticalFlowFilter.h"

DenseOpticalFlowFilter::DenseOpticalFlowFilter()
{
	if (verbose) printf("DenseOpticalFlowFilter::DenseOpticalFlowFilter()\n");
}

DenseOpticalFlowFilter::~DenseOpticalFlowFilter()
{
	if (verbose) printf("DenseOpticalFlowFilter::~DenseOpticalFlowFilter()\n");
	destroy();
}

void DenseOpticalFlowFilter::setup() 
{
	GrayscaleFilter::setup();

	grayInputPrev.allocate(videoSize.x, videoSize.y);

	flowX.allocate(videoSize.x, videoSize.y);
	flowY.allocate(videoSize.x, videoSize.y);

	//inputMatPrev = cvGetMat(grayInputPrev.getCvImage(), NULL, 0, 0);
	//inputMat = cvGetMat(grayInput.getCvImage(), NULL, 0, 0);
	
	CvSize img_sz = cvGetSize( input.getCvImage() );
	flowIpl = cvCreateImage( img_sz, IPL_DEPTH_32F, 2 );
	grayFlowIpl = cvCreateImage( img_sz, IPL_DEPTH_8U, 2 );
	
//	flowMat = cvGetMat(flowIpl, NULL, 0, 0);
	
	//	cv::Mat inputMatPrev = cv::cvarrToMat(grayInputPrev.getCvImage());
	//	cv::Mat inputMat = cv::cvarrToMat(grayInput.getCvImage());
	//	cv::Mat flowMat = cv::cvarrToMat(flowIpl);
	addContent("Flow X", flowX);
	addContent("Flow X", flowY);

	addSlider("Max Iterations", settings.max_iter, 1, 10);
	addSlider("Window Size", settings.win_size, 1, 11);
	addSlider("Pyr. Scale", settings.pyr_scale, 0.0, 1.0);
	addSlider("Pyr. Levels", settings.pyr_levels, 1, 10);
	addSlider("Poly. n", settings.poly_n, 1.0, 20.0);
	addSlider("Poly. sigma", settings.poly_sigma, 0.1, 5.0);
}

void DenseOpticalFlowFilter::update() 
{
#ifdef USE_OPENCV_TRUNK
	inputMatPrev = cv::cvarrToMat(grayInputPrev.getCvImage());
	inputMat = cv::cvarrToMat(input.getCvImage());
	flowMat = cv::cvarrToMat(flowIpl);
	
	cv::calcOpticalFlowFarneback(inputMatPrev, inputMat, flowMat,
								 settings.pyr_scale, settings.pyr_levels, settings.win_size,
								 settings.max_iter, settings.poly_n, settings.poly_sigma,
								 cv::OPTFLOW_FARNEBACK_GAUSSIAN);

    float scale = 255.0f/(settings.scale_max - settings.scale_min);
    cvConvertScale(flowIpl, grayFlowIpl, scale, -(settings.scale_min*scale));

	cvSplit(grayFlowIpl, flowX.getCvImage(), flowY.getCvImage(), NULL, NULL);

	flowX.flagImageChanged();
	flowY.flagImageChanged();

	cvMerge(flowX.getCvImage(), flowY.getCvImage(), NULL, NULL, output.getCvImage());
	output.flagImageChanged();
	
	grayInputPrev = input;
#endif
}

void DenseOpticalFlowFilter::destroy() 
{
	grayInputPrev.clear();

	flowX.clear();
	flowY.clear();

	cvReleaseImage(&flowIpl);
	cvReleaseImage(&grayFlowIpl);
	
	if (verbose) printf("DenseOpticalFlowFilter::destroy()\n");
}
