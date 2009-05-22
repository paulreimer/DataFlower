#pragma once

#include "OpticalFlowFilter.h"

OpticalFlowFilter::OpticalFlowFilter(string name) : VideoFilter(name) {
	printf("OpticalFlowFilter::OpticalFlowFilter()\n");

	settings.dv_min_error	= 0;
	settings.dv_max_error	= 550;
	settings.dv_scale		= 1.0;
	settings.max_iter		= 20;
	settings.win_size		= 10;
	settings.eps			= 0.03;
	settings.max_features	= 100;
	settings.pyr_levels	= 5;
	
}

OpticalFlowFilter::~OpticalFlowFilter() {
	printf("OpticalFlowFilter::~OpticalFlowFilter()\n");
	destroy();
}

void OpticalFlowFilter::setup() {
	VideoFilter::setup();
	
	inputPrev.allocate(VIDEO_SIZE);
	grayInputPrev.allocate(VIDEO_SIZE);

	cornersImg = new CvPoint2D32f[ MAX_CORNERS ]; 
	cornersPrev = new CvPoint2D32f[ MAX_CORNERS ]; 
	
	img_sz = cvGetSize( grayInput.getCvImage() ); 
	IplImage* eig_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 ); 
	IplImage* tmp_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
	
	pyr_sz = cvSize( grayInput.getCvImage()->width+8, grayInput.getCvImage()->height/3 ); 
	pyrImg = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
	pyrPrev = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
}

void OpticalFlowFilter::update() {
	corner_count = settings.max_features;
	cvGoodFeaturesToTrack(grayInput.getCvImage(),
						  eig_image,
						  tmp_image,
						  cornersImg,
						  &corner_count,
						  0.01,
						  5.0,
						  0,
						  3,
						  0,
						  0.04
						  ); 
	cvFindCornerSubPix(grayInput.getCvImage(),
					   cornersImg,
					   corner_count,
					   cvSize(settings.win_size,settings.win_size),
					   cvSize(-1,-1),
					   cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03)
					   );
	
	cvCalcOpticalFlowPyrLK(grayInput.getCvImage(),
						   grayInputPrev.getCvImage(),
						   pyrImg,
						   pyrPrev,
						   cornersImg,
						   cornersPrev,
						   corner_count,
						   cvSize( settings.win_size,settings.win_size ),
						   settings.pyr_levels,
						   features_found,
						   feature_errors,
						   cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS,
										  settings.max_iter,
										  settings.eps ),
						   0//CV_LKFLOW_PYR_B_READY | CV_LKFLOW_INITIAL_GUESSES
						   );
	IplImage* swp_pyr = pyrPrev;
	pyrPrev = pyrImg;
	pyrImg = swp_pyr;
}

void OpticalFlowFilter::destroy() {
	printf("OpticalFlowFilter::destroy()\n");
}
