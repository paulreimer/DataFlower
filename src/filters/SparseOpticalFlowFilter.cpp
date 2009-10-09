#pragma once

#include "SparseOpticalFlowFilter.h"

SparseOpticalFlowFilter::SparseOpticalFlowFilter()
{
	if (verbose) printf("SparseOpticalFlowFilter::SparseOpticalFlowFilter()\n");
}

SparseOpticalFlowFilter::~SparseOpticalFlowFilter()
{
	if (verbose) printf("SparseOpticalFlowFilter::~SparseOpticalFlowFilter()\n");
	destroy();
}

void SparseOpticalFlowFilter::setup() 
{
	GrayscaleFilter::setup();

	inputPrev.allocate(videoSize.x, videoSize.y);

	cornersImg = new CvPoint2D32f[ MAX_CORNERS ];
	cornersPrev = new CvPoint2D32f[ MAX_CORNERS ];

	img_sz = cvGetSize( input.getCvImage() );
	IplImage* eig_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
	IplImage* tmp_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );

	pyr_sz = cvSize( input.getCvImage()->width+8, input.getCvImage()->height/3 );
	pyrImg = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
	pyrPrev = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
}

void SparseOpticalFlowFilter::update() 
{
	corner_count = settings.max_features;
	cvGoodFeaturesToTrack(input.getCvImage(),
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
	cvFindCornerSubPix(input.getCvImage(),
					   cornersImg,
					   corner_count,
					   cvSize(settings.win_size,settings.win_size),
					   cvSize(-1,-1),
					   cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03)
					   );

	cvCalcOpticalFlowPyrLK(input.getCvImage(),
						   inputPrev.getCvImage(),
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
	
	inputPrev = input;
}

void SparseOpticalFlowFilter::destroy() 
{
	if (verbose) printf("SparseOpticalFlowFilter::destroy()\n");
	
	delete cornersImg;
	delete cornersPrev;
	
	cvReleaseImage(&eig_image);
	cvReleaseImage(&tmp_image);
	
	cvReleaseImage(&pyrImg);
	cvReleaseImage(&pyrPrev);
	
	input.clear();
	inputPrev.clear();
}
